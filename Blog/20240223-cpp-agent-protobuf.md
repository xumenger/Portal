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

```shell
g++ -std=c++11 Agent.cpp ../protobuf/Agent.pb.cc -o Agent `pkg-config --cflags --libs protobuf`

chmod u+x ./Portal
./Portal
```