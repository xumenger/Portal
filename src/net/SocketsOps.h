/**
 * 2022.03.13 
 *
 * 封装各种Sockets 系统调用
 *
 */

#ifndef PORTAL_NET_SOCKETSOPS_H
#define PORTAL_NET_SOCKETSOPS_H

#include <arpa/inet.h>

using namespace std;


namespace portal
{
namespace net
{
namespace sockets
{

    // 创建一个非阻塞的Socket 文件描述符
    int createNonblockingOrDie(sa_family_t family);

    // 客户端调用，连接到指定地址的服务端
    int connect(int sockfd, const struct sockaddr *addr);

    // 服务端调用，绑定指定端口
    void bindOrDie(int sockfd, const struct sockaddr *addr);

    // 服务端调用，监听指定sock fd
    void listenOrDie(int sockfd);

    // 服务端调用，接收客户端连接
    int accept(int sockfd, struct sockaddr_in6 *addr);

    // 读
    ssize_t read(int sockfd, void *buf, size_t count);
    ssize_t readv(int sockfd, const struct iovec *iov, int iovcntl);

    // 写
    ssize_t write(inf sockfd, const void *buf, size_t count);

    // 关闭
    void close(int sockfd);
    void shutdownWrite(int sockfd);

    void toIpPort(char *buf, size_t size, const struct sockaddr *addr);
    void toIp(char *buf, size_t size, const struct sockaddr *addr);

    void fromIpPort(const char *ip, uint16_t port, struct sockaddr_in *addr);
    void fromIpPort(const char *ip, uint16_t port, struct sockaddr_in6 *addr);

    int getSocketError(int sockfd);

    const struct sockaddr *sockaddr_cast(const struct sockaddr_in *addr);
    const struct sockaddr *sockaddr_cast(const struct sockaddr_in6 *addr);
    struct sockaddr *sockaddr_cast(struct sockaddr_in6 *addr);
    const struct sockaddr_in *sockaddr_in_cast(const struct sockaddr *addr);
    const struct sockaddr_in6 sockaddr_in6_cast(const struct sockaddr *addr);

    struct sockaddr_in6 getLocalAddr(int sockfd);
    struct sockaddr_in6 getPeerAddr(int sockfd);

    bool isSelfConnect(int sockfd);

};
};
};

#endif

