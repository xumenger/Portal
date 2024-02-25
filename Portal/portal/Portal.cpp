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


struct RequestBuffer {
    int sock_fd;

    int buffer_readed = 0;

    char msg_type_buffer[sizeof(int32_t)];
    char msg_len_buffer[sizeof(int32_t)];

    com::xum::proto::portal::PortalMessageType msg_type;
    int32_t msg_len;

    char *msg_recv_buffer;
};

std::map<int, RequestBuffer> buffer_map;


/**
 * Portal分布式数据节点
 * 作为系统的核心，存储系统元数据信息
 * 基于Raft协议实现，保证元数据高可用
 * 
 * 
 * 网络编程参考muduo、redis实现
 * Raft参考etcd实现

 * 版本1: 先实现Portal非分布式版本
 *       只有网络通信，数据存储在内存中
 *       实现客户端、Portal、Agent的通信和调度功能
 * 版本2: 实现Raft协议
 * 版本3: 参考etcd实现BTree等存储（可以尝试自己做）
 * 
 * 模块划分：
 * 1. 网络库
 * 2. raft（再细分？）
 * 3. protobuf协议
 * 4. 存储模块
 * 
 */
int main(int argc, char const *argv[])
{
    if (argc <= 2) {
        printf("usage: %s ip_address port_number\n", basename(argv[0]));
        return 1;
    }

    const char* ip = argv[1];
    int port = atoi(argv[2]);
    
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

        // ret返回值指的是？
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
                it->second.buffer_readed += ret;
                if (ret <= 0) {
                    close(sockfd);
                    // 内存怎么释放？这里面有内存泄漏问题！
                    buffer_map.erase(it);
                    continue;
                }
                else if (ret == 4 - it->second.buffer_readed) {
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
                it->second.buffer_readed += ret;
                if (ret <= 0) {
                    close(sockfd);
                    // 内存怎么释放？这里面有内存泄漏问题！
                    buffer_map.erase(it);
                    continue;
                }
                else if (ret == 8 - it->second.buffer_readed) {
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
            if (ret <= 0) {
                close(sockfd);
                // 内存怎么释放？这里面有内存泄漏问题！
                // msg_recv_buffer还没有释放
                buffer_map.erase(it);
                continue;
            } else {
                it->second.buffer_readed += ret;
                if (it->second.buffer_readed - 8 == it->second.msg_len) {
                    // TODO 根据msg_type 分类处理
                    com::xum::proto::portal::SetRequest set_req;
                    set_req.ParseFromArray(it->second.msg_recv_buffer, it->second.msg_len);

                    std::cout << "value is: " << set_req.value() << "\n";

                    // 内存怎么释放？这里面有内存泄漏问题！
                    // msg_recv_buffer还没有释放
                    buffer_map.erase(it);

                    // 发送怎么做成非阻塞的
                    char buffer[msg_len];
                    strcpy(buffer, "successful");
                    send(connect_fd, buffer, strlen(buffer), 0);
                    printf("send message: %s\n", buffer);
                }
            }
        }
        else {
            printf("something else happened \n");
        }
    }
}




int backup(int argc, char const *argv[])
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


	// 启动与客户端通信的Loop
	// 涉及到哪些网络接口？

	// 启动与Follower、Candidate通信的Loop
	// 涉及到哪些网络接口？

	// 实现元数据持久化存储

	// 

	return 0;
}
