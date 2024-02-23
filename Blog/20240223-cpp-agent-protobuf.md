>Agent 程序基于Protobuf 与Portal 服务端通信

Agent 在本项目架构中的作用是：

1. 部署在各个工作机器上
2. 启动的时候注册到Portal
3. 实时向Portal反馈工作机器的负载情况
4. 接收Portal的指令创建工作进程
5. 调度管理工作进程

本文主要用于实现Agent 启动后主动注册到Portal！

```proto
/**
 * Agent 与Portal 之间的协议接口定义
 */

// 声明proto协议版本
syntax = "proto3";

package com.xum.proto.agent;

message AgentRegister {
    string agent_name = 1;
    string host_ip = 2;
}
```

## 编译Protobuf 文件供C++ 使用

在Ubuntu 上编译安装protobuf

```shell
tar -xzf protobuf-all-3.13.0.tar.gz
cd protobuf-3.13.0/
./configure --prefix=$INSTALL_DIR 
make 
make check 
make install

apt-get install pkg-config
```

编译Protobuf 文件供C++ 使用，注意，如果是在Ubuntu 上运行，那么需要在Ubuntu 上执行该编译命令，在MacOS 上编译得到的程序无法在Ubuntu 上运行！

```shell
cd Protobuf/

# 编译后生成Agent.pb.cc、Agent.pb.h
protoc -I=./ --cpp_out=../Portal/protobuf/ Agent.proto
```

## 编译运行客户端程序

编写Agent.cpp 程序如下

```c++
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <assert.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#include "../protobuf/Agent.pb.h"

int main(int argc, char const *argv[])
{
    if (argc <= 3) {
        printf("args params less than 3");
        return -1;
    }

    const char *agent_name = argv[1];
    const char *portal_ip = argv[2];
    int portal_port = atoi(argv[3]);

    struct sockaddr_in server_address;
    bzero(&server_address, sizeof(server_address));
    server_address.sin_family = AF_INET;
    inet_pton(AF_INET, portal_ip, &server_address.sin_addr);
    server_address.sin_port = htons(portal_port);

    int sockfd = socket(PF_INET, SOCK_STREAM, 0);
    assert(sockfd >= 0);
    if (connect(sockfd, (struct sockaddr*)&server_address, sizeof(server_address)) < 0) {
       printf("connection failed\n");
    }
    else {
        com::xum::proto::agent::AgentRegister reg;
        reg.set_agent_name(agent_name);
        reg.set_host_ip("127.0.0.1");
        
        char ss[1024];
        reg.SerializeToArray(ss, 1024);

        if (send(sockfd, ss, strlen(ss) + 1, 0) <= 0) {
            printf("send error\n");
        }
    }
    close(sockfd);

    return 0;
}
```

可以试一下编译运行程序

```shell
g++ -std=c++11 Agent.cpp ../protobuf/Agent.pb.cc -o Agent `pkg-config --cflags --libs protobuf`

chmod u+x ./Portal

# 运行会报错，缺少参数
./Agent
```
