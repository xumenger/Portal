/**
 * 2022.03.13 
 *
 * 封装IPv4 地址（end point）
 * 不支持解析域名，只认IP 地址
 * 因为直接用gethostbyname(3) 解析域名会阻塞IO 线程
 *
 */
#ifndef PORTAL_NET_INETADDRESS_H
#define PORTAL_NET_INETADDRESS_H

#include <boost/function.hpp>

#include "../base/copyable.h"

using namespace std;


namespace portal
{
namespace net
{
namespace sockets
{
    const struct sockaddr *sockaddr_cast(const struct sockaddr_in6 *addr);
};

class InetAddress : public portal::base::copyable
{
    public:
        // explicit 取消了隐式转换
        // 给定一个端口构造，主要用在TcpServer listening
        explicit InetAddress(uint16_t port = 0, bool loopbackOnly = false, bool ipv6 = false);

        // 给定具体IP、端口的方式构造
        InetAddress(string ip, uint16_t port, uint16_t port, bool ipv6 = false);

        explicit InetAddress(const struct sockaddr_in &addr)
            : addr_(addr)
        {  }

        explicit InetAddress(const struct sockaddr_in6 &addr)
            : addr6_(addr)
        {  }

        // sa_family_t 协议类型，AF_INET 对应的是TCP
        sa_family_t family() const
        {
            return addr_.sin_family;
        }

        string toIp() const;
        string toIpPort() const;
        uint16_t toPort() const;


        const struct sockaddr *getSockAddr() const
        {
            return sockets::sockaddr_cast(&addr6_);
        }

        uint32_t ipNetEndian() const;
        uint16_t portNetEndian() const
        {
            return addr_.sin_port;
        }

        // static 修饰函数时，表明该函数只在同一文件中调用
        // resolve hostname to IP address, not changing port or sin_family
        // return true on success.
        // thread safe
        static bool resolve(string hostname, InetAddress *result);

    private:
        union
        {
            struct sockaddr_in addr_;
            struct sockaddr_in6 addr6_;
        };
};


};
};

#endif

