>Portal 服务端支持与Transfer、Agent 通信

Transfer 和Agent 都会给Portal 发送消息，后续Portal 实现Raft 协议之后，Leader 与Follower 也会进行通信，那么就有一个问题出现了：Portal 怎么知道是谁发过来的什么信息？

## 编译Protobuf 文件供C++ 使用

```shell
cd Protobuf/

# 编译后生成Transfer.pb.cc、Transfer.pb.h
protoc -I=./ --cpp_out=../Portal/protobuf/ Transfer.proto
```

编写C++ 服务端程序

```c++

```