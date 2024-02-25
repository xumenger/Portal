>Portal 基于epoll 实现IO 多路复用

暂不考虑使用select、poll，直接使用epoll。对于epoll 底层的红黑树实现等原理也暂时不研究；也暂时不参考redis 对epoll 进行进一步封装；也暂时不单独封装一个可用的网络库

当前的目标主要是在Portal 里面使用epoll 能够先让程序运行起来！

以下当前代码有几个需要特别注意的点：

* std::map 的内存管理细节没有
* 整个程序的内存管理暂时没有深究，这个程序存在内存泄漏问题，后续需要继续优化
* 关于recv() 返回值和errno 的判断逻辑需要特别注意
* lt() 方法针对TCP 请求包的处理，这里面的代码太多了，需要重构解耦
* 本文只是实现recv 的非阻塞处理，对于send 暂时没有，存在缺陷
* 在开发的过程中，使用gdb 对程序进行单步调试，理解的效果才很好，调试很重要
* 对于epoll 的封装，后续继续参考redis
* 针对RequestBuffer 的封装以及buffer 的管理，参考一下redis 是怎么做的！怎么管理请求上下文？
* 目前没有考虑网络安全问题，比如发送过来的报文不合法等
* 对于TCP 的状态转移没有深入分析和总结
* 大量写代码、大量调试才能保持对于技术的敏感度
* recv() 返回-1，errno除了EAGAIN 还有哪些，分别表示什么意思？
    * EAGAIN：套接字已标记为非阻塞，而接收操作被阻塞或者接收超时
    * EBADF：sock不是有效的描述词
    * ECONNREFUSE：远程主机阻绝网络连接
    * EFAULT：内存空间访问出错
    * EINTR：操作被信号中断
    * EINVAL：参数无效
    * ENOMEM：内存不足
    * ENOTCONN：与面向连接关联的套接字尚未被连接上
    * ENOTSOCK：sock索引的不是套接字
* 客户端一次发送部分字节是很好的调试Portal 端针对IO 多路复用实现的代码
* 接下来可以先不参考redis、muduo 的网络库部分，自己独立实现，然后再去对比和他们建模的差距！
* 对于定时器的实现在当前版本是缺失的
* 更进一步的，K8s 集群中怎么利用etcd 做集群的管理的？KV 是怎么设计的？
* Raft 协议现在是完全没有考虑的，接下来也需要考虑在IO 多路复用的框架下，Raft 怎么实现？
* 怎么把Reactor 与协议细节分开？现在代码都写到一起了！
* 现在的代码是C 风格与C++ 风格混在一起的，怎么明确风格？怎么明确区分开？
* Linux 的常用命令还是没有梳理全！
* 试想这样的场景，如果服务端发送给某个客户端消息很多，但是客户端一直不接收，那么就一直需要存储在应用端的缓存中，那么就可能因为一个客户端打爆服务端进程，类似的安全问题后续都需要考虑
* 客户端发送到服务端超时的话，怎么办？幂等，还是查询？具体怎么设计？

```c++
#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <assert.h>
#include <fcntl.h>
#include <stdlib.h>

// 这些是C++的内容
#include <cerrno>
#include <iostream>
#include <map>

#include "../protobuf/Portal.pb.h"


#define PORTAL_PORT 7777        // 端口号
#define BUF_SIZE 1024           // 最大缓存
#define BUFFER_SIZE 10
#define MAX_QUEUE 20            // 最大连接数
#define MAX_EVENT_NUMBER 1024   // epoll最大事件数

int setnonblocking(int fd);
void addfd(int epollfd, int fd);
void lt(epoll_event *events, int number, int epollfd, int listenfd);


// 设计用于保存请求上下文信息
struct RequestBuffer {
    int sock_fd;

    int buffer_readed = 0;

    char msg_type_buffer[sizeof(int32_t)];
    char msg_len_buffer[sizeof(int32_t)];

    com::xum::proto::portal::PortalMessageType msg_type;
    int32_t msg_len;

    char *msg_recv_buffer;
};

// map 的使用细节还未深究！
std::map<int, RequestBuffer> buffer_map;

int main(int argc, char const *argv[])
{
    // if (argc <= 2) {
    //     printf("usage: %s ip_address port_number\n", basename(argv[0]));
    //     return 1;
    // }

    // const char* ip = argv[1];
    // int port = atoi(argv[2]);
    
    const char* ip = "172.16.192.128";
    int port =  7777;

    int ret = 0;
    struct sockaddr_in address;
    bzero(&address, sizeof(address));
    address.sin_family = AF_INET;
    inet_pton(AF_INET, ip, &address.sin_addr);
    address.sin_port = htons(port);

    int listenfd = socket(PF_INET, SOCK_STREAM, 0);
    assert(listenfd >= 0);

    ret = bind(listenfd, (struct sockaddr*)&address, sizeof(address));
    assert(ret != -1);

    ret = listen(listenfd, MAX_QUEUE);
    assert(ret != -1);

    epoll_event events[MAX_EVENT_NUMBER];
    int epollfd = epoll_create(5);  // 参数5是什么？
    assert(epollfd != -1);
    addfd(epollfd, listenfd);       // listenfd是要用ET模式还是LT模式

    while(1) {
        int ret = epoll_wait(epollfd, events, MAX_EVENT_NUMBER, -1);
        if (ret < 0) {
            printf("epoll failure\n");
            break;
        }

        // epoll_wait() 返回值指的是？是不是事件的数量？
        lt(events, ret, epollfd, listenfd);
    }

    close(listenfd);
    return 0;
}

// 将文件描述符设置为非阻塞的
int setnonblocking(int fd)
{
    int old_option = fcntl(fd, F_GETFL);
    int new_option = old_option | O_NONBLOCK;
    fcntl(fd, F_SETFL, new_option);
    return old_option;
}

// 将文件描述符上的EPOLLIN 注册到epollfd 指示的epoll内核事件表，使用LT模式（默认）
void addfd(int epollfd, int fd)
{
    epoll_event event;
    event.data.fd = fd;
    event.events = EPOLLIN;

    epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &event);
    setnonblocking(fd);
}

// lt模式的工作流
void lt(epoll_event *events, int number, int epollfd, int listenfd)
{
    char buf[BUFFER_SIZE];
    for (int i=0; i<number; i++) {
        int sockfd = events[i].data.fd;
        if (sockfd == listenfd) {
            struct sockaddr_in client_address;
            socklen_t client_addrlength = sizeof(client_address);
            int connfd = accept(listenfd, (struct sockaddr*)&client_address, &client_addrlength);
            addfd(epollfd, connfd);
        }
        else if (events[i].events & EPOLLIN) {
            std::map<int, RequestBuffer>::iterator it = buffer_map.find(sockfd);
            
            // 如果没有找到
            if (it == buffer_map.end()) {
                // 后续完善内存管理策略
                RequestBuffer req_buf;
                buffer_map.insert(std::pair<int, RequestBuffer>(sockfd, req_buf));
            }

            it = buffer_map.find(sockfd);

            int ret = 0;

            // 读取msg_type
            if (it->second.buffer_readed < 4) {
                ret = recv(sockfd, it->second.msg_type_buffer + it->second.buffer_readed, 4 - it->second.buffer_readed, 0);
                // <0 出错（如果errno==EAGAIN：套接字已标记为非阻塞，而接收操作被阻塞或者接收超时）
                // =0 连接关闭
                // >0 接收到数据大小
                if (0 == ret) {
                    close(sockfd);
                    // 内存怎么释放？这里面有内存泄漏问题！
                    buffer_map.erase(it);
                    continue;
                }
                else if (ret < 0) {
                    if (EAGAIN == errno) {
                        continue;
                    }
                }
                
                it->second.buffer_readed += ret;
                if (4 == it->second.buffer_readed) {
                    // 获取msg_type
                    int32_t msg_type_temp = *((int32_t*)it->second.msg_type_buffer);
                    msg_type_temp = ntohl(msg_type_temp);
                    // 在c中，enum类型默认是int类型，它们之间可以自动转换
                    // c++编译器支持从enum类型自动转换为int，但反过来是不支持的。需要进行强制转换
                    com::xum::proto::portal::PortalMessageType type = com::xum::proto::portal::PortalMessageType(msg_type_temp);
                    it->second.msg_type = type;
                }
            }
            // 读取msg_len
            if (it->second.buffer_readed < 8) {
                ret = recv(sockfd, it->second.msg_len_buffer + it->second.buffer_readed - 4, 8 - it->second.buffer_readed, 0);
                if (0 == ret) {
                    close(sockfd);
                    // 内存怎么释放？这里面有内存泄漏问题！
                    buffer_map.erase(it);
                    continue;
                }
                else if (ret < 0) {
                    if (EAGAIN == errno) {
                        continue;
                    }
                }

                it->second.buffer_readed += ret;
                if (8 == it->second.buffer_readed) {
                    // 获取msg_len
                    int32_t msg_len = *((int32_t*)it->second.msg_len_buffer);
                    msg_len = ntohl(msg_len);
                    it->second.msg_len = msg_len;

                    // 存在被攻击的风险，怎么做安全保护？
                    it->second.msg_recv_buffer = (char * ) malloc(msg_len);
                }
            }
            // 读取请求内容
            ret = recv(sockfd, 
                       it->second.msg_recv_buffer + (it->second.buffer_readed - 8), 
                       it->second.msg_len - (it->second.buffer_readed - 8), 
                       0);
            if (0 == ret) {
                close(sockfd);
                // 内存怎么释放？这里面有内存泄漏问题！
                // msg_recv_buffer还没有释放
                buffer_map.erase(it);
                continue;
            }
            else if (ret < 0) {
                if (EAGAIN == errno) {
                    continue;
                }
            } 
            else {
                it->second.buffer_readed += ret;
                if (it->second.buffer_readed - 8 == it->second.msg_len) {
                    // TODO 根据msg_type 分类处理
                    com::xum::proto::portal::SetRequest set_req;
                    set_req.ParseFromArray(it->second.msg_recv_buffer, it->second.msg_len);

                    std::cout << "value is: " << set_req.value() << "\n";

                    // 内存怎么释放？这里面有内存泄漏问题！
                    // msg_recv_buffer还没有释放
                    buffer_map.erase(it);

                    // 发送怎么做成非阻塞的？
                    char buffer[1024];
                    strcpy(buffer, "successful");
                    send(sockfd, buffer, strlen(buffer), 0);
                    printf("send message: %s\n", buffer);
                }
            }
        }
        else {
            printf("something else happened \n");
        }
    }
}
```
