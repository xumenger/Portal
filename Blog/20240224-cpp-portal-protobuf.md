>Portal 服务端支持与Transfer、Agent 通信

Transfer 和Agent 都会给Portal 发送消息，后续Portal 实现Raft 协议之后，Leader 与Follower 也会进行通信，那么就有一个问题出现了：Portal 怎么知道是谁发过来的什么信息？

另外我们的协议应该分三层

1. 最底层是Leader、Follower、Candidate 之间数据传输的协议
2. 再上层是KV 存储、查询等基础功能？
3. 再上层是进程管理的协议，这个是否需要体现？还是Portal 只提供最基础的存储、查询功能，应用层自己用这个实现

在《分布式一致性算法开发实战》中，其通过设计如下的网络协议解决以上的问题（可以通过类型判断是哪个协议，然后去，关于网络协议的分发处理，可以参考redis、Kafka 的源码实现）：

```
|  类型 |  长度  | payload  |
| 4byte | 4byte | ??? byte |
```

接下来也可以先看一下etcd 是怎么做的！etcd 的启动方法位于server/main.go。

1. etcd 收到客户端的KV 之后，怎么一步步处理转发到Follower 的？
2. etcd 中大量使用了goroutine 和channel 等GoLang 语法特性，这部分不是很熟！

详细可以看下一篇文章！

另外的问题：

1. Kafka 的协议格式是怎么设计的？
2. Redis 的协议格式是怎么设计的？可以看一下networking.c 下面的readQueryFromClient() 方法的实现细节


## 重新设计协议

之前针对Transfer 与Portal、Portal 与Agent 之间分别设计了协议，这样导致Portal 太过个性化，所以需要重新规划一下协议设计，以及功能划分了！

1. Portal 提供最基础的KV 存储、查询，以及Watch 功能
2. Transfer 发送给Portal 的消息就是KV
3. Agent 上报的进程运行情况、服务器信息等也都封装成KV 模式存到Portal
4. Portal 与Agent 之间通过KV 传输信息，这一层设计为JSON 格式
5. Portal 完全不感知分布式调度集群的概念，只是管理KV 的基础组件

所以本文的内容会比较多，把原来的内容推翻，重新设计

之前在Portal 里面还定义了针对Transfer、Agent、Worker 的封装，这些全部都废弃，原来的耦合太重了！


## Portal 设计

设计两个Proto 文件，一个是对外协议的封装，比如KV、Watch 功能等，命名为Portal.proto；一个是内部集群节点之间Raft 协议的封装，命名为Raft.proto（Raft.proto 本期暂时不涉及，等动后续Raft 开发的时候再深究！）

对外协议的格式如下（参考《分布式一致性算法开发实战》）：

```
|  类型 |  长度  | payload  |
| 4byte | 4byte | ??? byte |
```

Portal.proto 定义的内容如下：

```proto
/**
 * Portal 对外的服务接口定义
 */

// 声明proto 协议版本
syntax = "proto3";

package com.xum.proto.portal;

// java 类所在的包名
option java_package = "com.xum.proto";
// 定义Protobuf自动生成类的类名
option java_outer_classname = "PortalProto";

/**
 * 消息类型定义
 */
enum PortalMessageType {
    MsgSetReq = 0;
    MsgSetRsp = 1;
    MsgGetReq = 2;
    MsgGetRsp = 3;
}


/**
 * Key-Value 相关的消息定义
 */
message SetRequest {
    string key = 1;
    string value = 2;
}

message SetResponse {
    string key = 1;
}

message GetRequest {
    string key = 1;
}

message GetResponse {
    string key = 1;
    string value = 2;
}
```

分别编译为Java 和C++ 文件（编译Protobuf 文件供C++ 使用，注意，如果是在Ubuntu 上运行，那么需要在Ubuntu 上执行该编译命令，在MacOS 上编译得到的程序无法在Ubuntu 上运行！）

```shell
protoc -I=./ --java_out=../Transfer/src/main/java/ Portal.proto

protoc -I=./ --cpp_out=../Portal/protobuf/ Portal.proto
```

Portal 的代码简单实现如下

```c++
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cerrno>

#include <iostream>

#include "../protobuf/Portal.pb.h"


#define PORTAL_PORT 7777  // 端口号
#define BUF_SIZE 1024     // 最大缓存
#define MAX_QUEUE 20      // 最大连接数 

int main(int argc, char const *argv[])
{
    int server_sockfd = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in server_sockaddr;                 // 保存本地地址信息
    server_sockaddr.sin_family = AF_INET;               // 采用ipv4
    server_sockaddr.sin_port = htons(PORTAL_PORT);      // 指定端口
    server_sockaddr.sin_addr.s_addr = htonl(INADDR_ANY);// 获取主机接收的所有响应

    // 绑定本地IP与端口
    if (bind(server_sockfd, (struct sockaddr *)&server_sockaddr, sizeof(server_sockaddr)) == -1) {
        perror("Bind Failuer\n");
        printf("Error: %s\n", strerror(errno));     // 输出错误信息
        return -1;
    }

    // 设置监听状态
    printf ("Listen Port: %d\n", PORTAL_PORT);
    if (listen(server_sockfd, MAX_QUEUE) == -1) {
        perror("Listen Error\n");
        return -1;
    }

    struct sockaddr_in client_addr;             // 保存客户端地址信息
    socklen_t length = sizeof(client_addr);     // 需要的内存大小

    printf("Waiting for connection!\n");

    // 等待连接，返回服务器端建立连接的socket
    int connect_fd = accept(server_sockfd, (struct sockaddr *)&client_addr, &length);
    if (-1 == connect_fd) {
        perror("Connect Error\n");
        return -1;
    }

    printf("Connection Successful\n");

    // 数据收发与传输
    while(1) {
        char int_buffer[sizeof(int32_t)];

        int32_t msg_type_temp;
        int len = recv(connect_fd, &int_buffer, 4, 0);
        msg_type_temp = *((int32_t*)int_buffer);
        msg_type_temp = ntohl(msg_type_temp);
        // 在c中，enum类型默认是int类型，它们之间可以自动转换
        // c++编译器支持从enum类型自动转换为int，但反过来是不支持的。需要进行强制转换
        com::xum::proto::portal::PortalMessageType type = com::xum::proto::portal::PortalMessageType(msg_type_temp);

        int32_t msg_len;
        len = recv(connect_fd, &int_buffer, 4, 0);
        msg_len = *((int32_t*)int_buffer);
        msg_len = ntohl(msg_len);

        char buffer[msg_len];
        len = recv(connect_fd, buffer, msg_len, 0);

        com::xum::proto::portal::SetRequest set_req;
        set_req.ParseFromArray(buffer, msg_len);

        std::cout << "value is: " << set_req.value() << "\n";

        strcpy(buffer, "successful");
        send(connect_fd, buffer, strlen(buffer), 0);
        printf("send message: %s\n", buffer);
    }

    close(connect_fd);
    close(server_sockfd);

    return 0;
}
```

编译并运行程序

```shell
g++ -std=c++11 Portal.cpp ../protobuf/Portal.pb.cc -o Portal `pkg-config --cflags --libs protobuf`

./Portal
```


## Transfer 设计

将Portal 视为一个存储的节点，把创建Worker、查看Worker 信息、关闭Worker 等功能打包为JSON 格式，然后以KV 的形式发送到Portal

Key 和Value 该怎么设计，这个就是另外一个层面的问题了，需要充分考虑，后续专门写文章总结！

Transfer 的ManageController 的代码简单实现如下

```c++
package com.xum.controller;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.Socket;
import java.net.UnknownHostException;
import java.nio.ByteBuffer;

import org.springframework.beans.factory.annotation.Value;
import org.springframework.stereotype.Controller;
import org.springframework.web.bind.annotation.PostMapping;
import org.springframework.web.bind.annotation.RequestBody;
import org.springframework.web.bind.annotation.RequestMapping;

import com.xum.proto.PortalProto.PortalMessageType;
import com.xum.proto.PortalProto.SetRequest;

@Controller
@RequestMapping("/manage")
public class ManageController {
    
    @Value("${portal.ip}")
    String gPortalIP;
    
    @Value("${portal.port}")
    int gPortalPort;
    

    @PostMapping("/create")
    public String Create(@RequestBody CreateParam request) throws UnknownHostException, IOException{
        
        // 创建TCP客户端
        Socket clientSocket = new Socket(gPortalIP, gPortalPort);
        
        // 创建输入流和输出流
        InputStream inputStream = clientSocket.getInputStream();
        OutputStream outputStream = clientSocket.getOutputStream();
        
        // 创建请求报文
        String key = "CreateWorker";
        String value = "{\n" + 
                "    \"taskName\": \"testTask\",\n" + 
                "    \"threadCount\": \"222\"\n" + 
                "}";
        SetRequest setReq = SetRequest.newBuilder()
                .setKey(key)
                .setValue(value)
                .build();
        byte[] data = setReq.toByteArray();
        
        
        byte[] type_bytes = ByteBuffer.allocate(Integer.BYTES).putInt(PortalMessageType.MsgSetReq_VALUE).array();
        
        System.out.println("data.length: " + data.length);
        byte[] len_bytes = ByteBuffer.allocate(Integer.BYTES).putInt(data.length).array();
        
        byte[] len_bytes_test = ByteBuffer.allocate(Integer.BYTES).putInt(1207959552).array(); 
        System.out.println("len_bytes_test: " + len_bytes_test);
        
        outputStream.write(type_bytes);
        outputStream.write(len_bytes);
        
        // 发送数据给服务端（是否存在大小端问题？）
        // 发送消息的时候，需要封装消息类型、长度的逻辑，是否单独封装一个API？
        outputStream.write(data);
        
        // 读取服务端响应
        byte[] response = new byte[1024];
        int bytesRead = inputStream.read(response);
        String receivedMessage = new String(response, 0, bytesRead);
        System.out.println("收到服务器响应：" + receivedMessage);
        
        // 关闭连接
        clientSocket.close();
        
        return "success";
    }
    
    
    /**
     * 使用静态内部类，简单定义一个入参实体类
     */
    static class CreateParam {
        private String taskName;        // 任务名称
        private Integer threadCount;     // 线程数量

        public String getTaskName() {
            return taskName;
        }
        public void setTaskName(String taskName) {
            this.taskName = taskName;
        }
        public Integer getThreadCount() {
            return threadCount;
        }
        public void setThreadCount(Integer threadCount) {
            this.threadCount = threadCount;
        }
    }
    
}
```


## Agent 设计

Agent 监听Transfer 存储到Portal 的比如创建Worker、关闭Worker 的指令信息

另外Agent 也把Worker、集群等运行情况打包为JSON 格式后，再封装为KV 存储到Portal 中

Agent 的代码实现如下（可以暂时不实现Watch 功能，通过每1s 轮询发起一次Get 获取相关的指令）：

```c++
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <assert.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#include "../protobuf/Portal.pb.h"

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
        com::xum::proto::portal::SetRequest set_req;
        set_req.set_key("key");
        set_req.set_value("value");
       
        int size = set_req.ByteSize(); 
        char ss[size];
        set_req.SerializeToArray(ss, size);

        
        char int_buffer[sizeof(int32_t)];
        int32_t msg_type = htonl(com::xum::proto::portal::MsgSetReq);
        memcpy(&int_buffer, &msg_type, sizeof(msg_type));
        send(sockfd, int_buffer, 4, 0);

        int32_t msg_len = htonl(size);
        memcpy(&int_buffer, &msg_len, sizeof(msg_len));
        send(sockfd, int_buffer, 4, 0);

        if (send(sockfd, ss, size, 0) <= 0) {
            printf("send error\n");
        }
    }
    close(sockfd);

    return 0;
}
```

编译运行代理客户端程序

```shell
g++ -std=c++11 Agent.cpp ../protobuf/Portal.pb.cc -o Agent `pkg-config --cflags --libs protobuf`

./Agent test_agent 172.16.192.128 7777
```


## 补充：gRPC 介绍

在etcd v3 版本中，网络通信部分是基于gRPC 实现的！

gRPC 是云原声计算基金会项目，gRPC 一开始由Google 开发，是一款语言中立、平台中立的服务间通信框架，使用gRPC 可以使得客户端像调用本地方法一样，调用远程主机提供的服务

可以在任何地方运行，它使客户端和服务器应用程序能够透明地通信，并使构建连接系统变得更加容易

gRPC 默认采用Protobuf 作为IDL（Interface Description Language）接口描述语言，服务之间通信的数据序列化和反序列化也是基于Protobuf 的，因为Protobuf 的特殊性，所以gRPC 框架是跨语言的通信框架（与编程语言无关），也就是说用Java 开发的基于gRPC 的服务，可以用GoLang 编程语言调用

gRPC 同时支持同步调用和异步调用，同步RPC 调用时会一直阻塞直到服务端处理完成返回结果，异步RPC 是客户端调用服务端时不等待服务端处理完成返回，而是服务端处理完成后主动回调客户端告诉客户端处理完成

gRPC 是基于http2 协议实现的，http2 协议提供了很多新的特性，并且在性能上也比http1 提高了很多，所以gRPC 的性能是非常好的

基于http2 协议的特性，gRPC 允许定义如下四种服务方法：

1. 单项RPC：客户端发起一次请求，等待服务端响应结果，会话结束，就像一次普通的函数调用这样简单
2. 服务端流式RPC：客户端发起一次请求，服务端回返回一个流，客户端会从流中读取一系列消息，直到没有结果为止
3. 客户端流式RPC：客户端提供一个数据流并写入消息发给服务端，一旦客户端发送完毕，酒等待服务器读取这些消息并返回应答
4. 双向流式RPC：客户端和服务端都有一个数据流，都可以通过各自的流进行读写数据，两个流式互相独立的，客户端和服务端都可以按其希望的任意顺序读写

接下来的一些决策：
1. 基于gRPC 的这些特性，接下来需要研究etcd 的源码，不需要太关注网络部分
2. etcd 的启动方法位于server/main.go
3. 对于etcd 更加关注其Raft 设计与实现，底层数据结构等内容
4. Portal 还是只基于proto3 设计接口，网络部分还是自己实现
