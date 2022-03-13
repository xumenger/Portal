/**
 * 2022.03.13 
 *
 */
#ifndef PORTAL_NET_SOCKET_H
#define PORTAL_NET_SOCKET_H

#include <boost/noncopyable.hpp>

// struct tcp_info is in <netinet/tcp.h>
struct tcp_info;


namespace portal
{
namespace net
{

class InetAddress;

class Socket : boost::noncopyable
{
    public:
        explicit Socket(int sockfd)
            : sockfd_(sockfd)
        {  }

        ~Socket();

        int fd() const
        {
            return sockfd_;
        }

        // return true if success
        bool getTcpInfo(struct tcp_info *) const;
        bool getTcpInfoString(char *buf, int len) const;

        // abort if address is true
        void bindAddress(const InetAddress &localaddr);
        // abort if address in use
        void listen();

        // 如果成功，返回正整数表示接受一个TCP 连接的sockfd，并且设置为非阻塞、close-on-exec
        // 如果失败，返回-1
        int accept(InetAddress *peeraddr);

        // TCP 是全双工的，该方法关闭发送这条通路
        void shutdownWrite();

        // Enable/disable TCP_NODELAY (disable/enable Nagle's algorithm)
        void setTcpNoDelay(bool on);

        // Enable/disable SO_REUSEADDR
        void setReuseAddr(bool on);

        // Enable/disable SO_REUSEPORT
        void setReusePort(bool on);

        // Enable/disable SO_KEEPLIVE
        void setKeepAlive(bool on);

    private:
        const int sockfd_;
};

};
};

#endif

