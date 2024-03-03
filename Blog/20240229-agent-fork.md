>实现Agent 支持接收Portal 指令并fork 进程

暂时不实现watch 功能，Agent 定时去查询，获取Transfer 发送到Portal 的命令，然后实现多线程进程的创建

还有一个很重要的点需要考虑：

1. Transfer 与Agent 之间的协议该怎么设计？
2. 保证Transfer 存储到Portal 中的key-value 信息可以让Agent 拿到
3. 可以参考K8s 集群中对于etcd 的使用细节！

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

