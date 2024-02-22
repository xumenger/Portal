>Portal 服务端支持与Transfer、Agent 通信

## 编译Protobuf 文件供C++ 使用

```shell
cd Protobuf/

# 编译后生成Transfer.pb.cc、Transfer.pb.h
protoc -I=./ --cpp_out=../Portal/protobuf/ Transfer.proto
```

编写C++ 服务端程序

```c++

```