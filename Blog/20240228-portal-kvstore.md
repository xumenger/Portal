>Portal 实现简单版本的KV 存储，支持set/get 方法

之前的所有demo 中，主要是为了让Portal 运行起来，其中只是实现了set 方法的支持，本文开始增加get 功能，如此需要

1. 考虑Key-Value 的存储怎么实现？
2. get、set 等方法的分发可以参考redis、Kafka 的实现，主要是代码架构的设计！
3. 以后如果再实现Raft，怎么预留？怎么分层？

本文的需求计算机底层技术都是之前的技术，主要是代码架构层面的考虑，方法的拆分、分层设计等等考虑！

## Portal 程序

编译C++ 程序的时候可能出现这样的报错

```
g++ -std=c++11 Portal.cpp ../protobuf/Portal.pb.cc -o Portal `pkg-config --cflags --libs protobuf`
Portal.cpp: In function ‘void lt(epoll_event*, int, int, int)’:
Portal.cpp:290:62: error: no match for call to ‘(const std::__cxx11::basic_string<char>) ()’
                                 get_rsp.set_key(kv_it->first().c_str());
                                                              ^
Portal.cpp:291:65: error: no match for call to ‘(std::__cxx11::basic_string<char>) ()’
                                 get_rsp.set_value(kv_it->second().c_str());
```

对应的代码如下

```c++
get_rsp.set_key(kv_it->first().c_str());
get_rsp.set_value(kv_it->second().c_str());
```

因为应该是（大乌龙！first、second 不是函数！）

```c++
get_rsp.set_key(kv_it->first.c_str());
get_rsp.set_value(kv_it->second.c_str());
```

在Portal 中使用`map<string, string>` 存储客户端发送过来的KV 信息，并且后续可以在这里查询

具体代码可以直接看`v1.0-20240303-kvstore` 分支的Portal.cpp 代码，但是有一些问题没有解决：

1. send 发送的非阻塞IO 没有封装好
2. 内存管理还是比较乱
3. 关于不同请求应答的策略现在代码耦合在一起
4. 需要参考Redis、Muduo 等考虑一下对于网络模块的封装
