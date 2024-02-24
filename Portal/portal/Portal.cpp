#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cerrno>

#include <iostream>

#include "../protobuf/Portal.pb.h"


#define PORTAL_PORT 7777  // 端口号
#define BUF_SIZE 1024     // 最大缓存
#define MAX_QUEUE 20      // 最大连接数 

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
        com::xum::proto::portal::PortalMessageType msg_type;
        int len = recv(connect_fd, &msg_type, 4, 0);

        int msg_len;
        len = recv(connect_fd, &msg_len, 4, 0);

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
