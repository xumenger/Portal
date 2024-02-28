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
std::map<std::string, std::string> kv_map;


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

                    switch (it->second.msg_type) {
                        case com::xum::proto::portal::MsgSetReq: {
                            com::xum::proto::portal::SetRequest set_req;
                            set_req.ParseFromArray(it->second.msg_recv_buffer, it->second.msg_len);

                            // 存储到内存
                            kv_map.insert(std::pair<std::string, std::string>(set_req.key(), set_req.value()));

                            // 发送应答（这部分怎么封装、解耦）
                            com::xum::proto::portal::SetResponse set_rsp;
                            set_rsp.set_key(set_req.key());
                           
                            int size = set_rsp.ByteSize(); 
                            char ss[size];
                            set_rsp.SerializeToArray(ss, size);

                            char int_buffer[sizeof(int32_t)];
                            int32_t msg_type = htonl(com::xum::proto::portal::MsgSetRsp);
                            memcpy(&int_buffer, &msg_type, sizeof(msg_type));
                            send(sockfd, int_buffer, 4, 0);

                            int32_t msg_len = htonl(size);
                            memcpy(&int_buffer, &msg_len, sizeof(msg_len));

                            // 这些send也是非阻塞的，需要考虑怎么封装优化？
                            send(sockfd, int_buffer, 4, 0);

                            if (send(sockfd, ss, size, 0) <= 0) {
                                printf("send error\n");
                            }

                            break;                                        
                        }
                        case com::xum::proto::portal::MsgGetReq: {
                            com::xum::proto::portal::GetRequest get_req;
                            get_req.ParseFromArray(it->second.msg_recv_buffer, it->second.msg_len);

                            // 根据key获取value
                            std::map<std::string, std::string>::iterator kv_it = kv_map.find(get_req.key());
                            // 如果没有找到
                            if (kv_it == kv_map.end()) {
                                // GetResponse 的协议需要完善，增加一个find 字段！
                            } else {
                                // 发送应答（这部分怎么封装、解耦）
                                com::xum::proto::portal::GetResponse get_rsp;
                                get_rsp.set_key(kv_it->first());
                                get_rsp.set_value(kv_it->second());
                               
                                int size = get_rsp.ByteSize(); 
                                char ss[size];
                                get_rsp.SerializeToArray(ss, size);

                                char int_buffer[sizeof(int32_t)];
                                int32_t msg_type = htonl(com::xum::proto::portal::MsgGetRsp);
                                memcpy(&int_buffer, &msg_type, sizeof(msg_type));
                                send(sockfd, int_buffer, 4, 0);

                                int32_t msg_len = htonl(size);
                                memcpy(&int_buffer, &msg_len, sizeof(msg_len));

                                send(sockfd, int_buffer, 4, 0);

                                if (send(sockfd, ss, size, 0) <= 0) {
                                    printf("send error\n");
                                }
                            }

                            break;
                        }
                        default: {
                                 
                        }
                    }
                    // 内存怎么释放？这里面有内存泄漏问题！
                    // msg_recv_buffer还没有释放
                    buffer_map.erase(it);

                    // 发送怎么做成非阻塞的
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
