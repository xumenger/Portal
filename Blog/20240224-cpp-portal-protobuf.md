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
    bytes key = 1;
    bytes value = 2;
}

message SetResponse {
    bytes key = 1;
}

message GetRequest {
    bytes key = 1;
}

message GetResponse {
    bytes key = 1;
    bytes value = 2;
}
```

分别编译为Java 和C++ 文件（编译Protobuf 文件供C++ 使用，注意，如果是在Ubuntu 上运行，那么需要在Ubuntu 上执行该编译命令，在MacOS 上编译得到的程序无法在Ubuntu 上运行！）

```shell
protoc -I=./ --java_out=../Transfer/src/main/java/ Portal.proto

protoc -I=./ --cpp_out=../Portal/protobuf/ Portal.proto
```

Portal 的代码简单实现如下

```c++

```


## Transfer 设计

将Portal 视为一个存储的节点，把创建Worker、查看Worker 信息、关闭Worker 等功能打包为JSON 格式，然后以KV 的形式发送到Portal

Key 和Value 该怎么设计，这个就是另外一个层面的问题了，需要充分考虑，后续专门写文章总结！

Transfer 的ManageController 的代码简单实现如下

```c++

```


## Agent 设计

Agent 监听Transfer 存储到Portal 的比如创建Worker、关闭Worker 的指令信息

另外Agent 也把Worker、集群等运行情况打包为JSON 格式后，再封装为KV 存储到Portal 中

Agent 的代码实现如下（暂时不实现Watch 功能，通过每1s 轮询发起一次Get 获取相关的指令）：

```c++

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