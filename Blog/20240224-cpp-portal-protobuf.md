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

## 编译Protobuf 文件供C++ 使用

```shell
cd Protobuf/

# 编译后生成Transfer.pb.cc、Transfer.pb.h
protoc -I=./ --cpp_out=../Portal/protobuf/ Transfer.proto
```

编写C++ 服务端程序

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