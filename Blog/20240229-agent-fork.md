>实现Agent 支持接收Portal 指令并fork 进程

暂时不实现watch 功能，Agent 定时去查询，获取Transfer 发送到Portal 的命令，然后实现多线程进程的创建

还有一个很重要的点需要考虑：

1. Transfer 与Agent 之间的协议该怎么设计？
2. 保证Transfer 存储到Portal 中的key-value 信息可以让Agent 拿到
3. 可以参考K8s 集群中对于etcd 的使用细节！

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


## Agent 程序实现

Agent 使用GetReq 去获取服务端的KV 信息，然后去fork 一个工作进程，下面只是实现一个大框架，关于json 格式value 的解析暂未实现、Agent 的代码结构现在一样是很耦合、fork() 创建的进程怎么作为一个后台进程、现在在Agent 的网络部分还是使用阻塞IO 的方式实现的、Agent 与Worker 之间的通信还没有实现，等等，存在很多问题

```c++
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <assert.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#include "../protobuf/Portal.pb.h"


com::xum::proto::portal::GetResponse get_response(int connect_fd);
void fork_worker(com::xum::proto::portal::GetResponse get_rsp);

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
        com::xum::proto::portal::GetRequest get_req;
        get_req.set_key("CreateWorker");
       
        int size = get_req.ByteSize(); 
        char ss[size];
        get_req.SerializeToArray(ss, size);

        
        char int_buffer[sizeof(int32_t)];
        int32_t msg_type = htonl(com::xum::proto::portal::MsgGetReq);
        memcpy(&int_buffer, &msg_type, sizeof(msg_type));
        send(sockfd, int_buffer, 4, 0);

        int32_t msg_len = htonl(size);
        memcpy(&int_buffer, &msg_len, sizeof(msg_len));
        send(sockfd, int_buffer, 4, 0);

        // 暂不严格考虑发送失败的情况
        if (send(sockfd, ss, size, 0) <= 0) {
            printf("send error\n");
        }

        // 接收应答（暂时只考虑阻塞的方式）
        // 返回值的内存拷贝方式是什么，内存管理有风险
        com::xum::proto::portal::GetResponse get_rsp = get_response(sockfd);

        fork_worker(get_rsp);
    }
    close(sockfd);
    return 0;
}


com::xum::proto::portal::GetResponse get_response(int connect_fd) {
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

    com::xum::proto::portal::GetResponse get_rsp;
    get_rsp.ParseFromArray(buffer, msg_len);

    std::cout << get_rsp.value() << std::endl;

    return get_rsp;
}


// TODO 怎么制作一个后台进程？
void fork_worker(com::xum::proto::portal::GetResponse get_rsp) {
    // 暂不考虑解析请求指令的问题

    // 直接fork 一个进程
    int childpid = fork();
    
    if (childpid == 0) {
        printf("%d This is child process\n\n", childpid);
    } else {
        printf("This is parent process\n\n");
    }
    printf("%d step2 after fork()！！ \n\n", childpid);
}
```