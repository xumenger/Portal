#include "./InetAddress.h"

#include "./Endian.h"
#include "./SocketsOps.h"

#include <netdb.h>
#include <strings.h>  // bzero
#include <netinet/in.h>

#include <string>     // std::string

#include <boost/static_assert.hpp>


// INADDR_ANY use (type)value casting
static const in_addr_t kInaddrAny = INADDR_ANY;
static const in_addr_t kInaddrLoopback = INADDR_LOOPBACK;


//     /* Structure describing an Internet socket address.  */
//     struct sockaddr_in {
//         sa_family_t    sin_family; /* address family: AF_INET */
//         uint16_t       sin_port;   /* port in network byte order */
//         struct in_addr sin_addr;   /* internet address */
//     };

//     /* Internet address. */
//     typedef uint32_t in_addr_t;
//     struct in_addr {
//         in_addr_t       s_addr;     /* address in network byte order */
//     };

//     struct sockaddr_in6 {
//         sa_family_t     sin6_family;   /* address family: AF_INET6 */
//         uint16_t        sin6_port;     /* port in network byte order */
//         uint32_t        sin6_flowinfo; /* IPv6 flow information */
//         struct in6_addr sin6_addr;     /* IPv6 address */
//         uint32_t        sin6_scope_id; /* IPv6 scope-id */
//     };

using namespace std;

using namespace portal;
using namespace portal::net;


// 对于assert是运行时的断言，它只在运行时断言表达式的值。在编译期间出现的错误时不会提示的
// 如果我们需要在编译期间就需要确认某些表达式是否是我们所期望的那样，比方说结构体的字节对齐，这个是很常见的问题
// 第二种情况就可以使用BOOST_STATIC_ASSERT
BOOST_STATIC_ASSERT(sizeof(InetAddress) == sizeof(struct sockaddr_in6));
BOOST_STATIC_ASSERT(offsetof(sockaddr_in, sin_family) == 0);
BOOST_STATIC_ASSERT(offsetof(sockaddr_in6, sin6_family) == 0);
BOOST_STATIC_ASSERT(offsetof(sockaddr_in, sin_port) == 2);
BOOST_STATIC_ASSERT(offsetof(sockaddr_in6, sin6_port) == 2);


InetAddress::InetAddress(uint16_t port, bool loopbackOnly, bool ipv6)
{
    BOOST_STATIC_ASSERT(offsetof(InetAddress, addr6_) == 0);
    BOOST_STATIC_ASSERT(offsetof(InetAddress, addr_) == 0);

    if (ipv6)
    {
        bzero(&addr6_, sizeof addr6_);
        addr6_.sin6_family = AF_INET6;
        in6_addr ip = loopbackOnly ? in6addr_loopback : in6addr_any;
        addr6_.sin6_addr = ip;
        addr6_.sin6_port = sockets::hostToNetwork16(port);
    }
    else 
    {
        bzero(&addr_, sizeof addr_);
        addr_.sin_family = AF_INET;
        in_addr_t ip = loopbackOnly ? kInaddrLoopback : kInaddrAny;
        addr_.sin_addr.s_addr = sockets::hostToNetwork32(ip);
        addr_.sin_port = sockets::hostToNetwork16(port);
    }
}

InetAddress::InetAddress(string ip, uint16_t port, bool ipv6)
{
    if (ipv6)
    {
        bzero(&addr6_, sizeof addr6_);
        sockets::fromIpPort(ip.c_str(), port, &addr6_);
    }
    else 
    {
        bzero(&addr_, sizeof addr_);
        sockets::fromIpPort(ip.c_str(), port, &addr_);
    }
}

string InetAddress::toIpPort() const
{
    char buf[64] = "";
    sockets::toIpPort(buf, sizeof buf, getSockAddr());
    return buf;
}

string InetAddress::toIp() const
{
    char buf[64] = "";
    sockets::toIp(buf, sizeof buf, getSockAddr());
    return buf;
}

uint32_t InetAddress::ipNetEndian() const
{
    assert(family() == AF_INET);
    return addr_.sin_addr.s_addr;
}

uint16_t InetAddress::toPort() const
{
    return sockets::networkToHost16(portNetEndian());
}


// __thread是GCC内置的线程局部存储设施
// _thread变量每一个线程有一份独立实体，各个线程的值互不干扰
// 可以用来修饰那些带有全局性且值可能变，但是又不值得用全局变量保护的变量
static __thread char t_resolveBuffer[64 * 1024];

bool InetAddress::resolve(string hostname, InetAddress *out)
{
    assert(out != NULL);
    struct hostent hent;
    struct hostent *he = NULL;
    int herrno = 0;
    bzero(&hent, sizeof(hent));

    int ret = gethostbyname_r(hstname.c_str(), &hent, t_resolveBuffer, sizeof t_resolveBuffer, &he, &herrno);
    if (ret == 0 && he != NULL)
    {
        assert(he->h_addrtype == AF_INET && he->h_length == sizeof(uint32_t));
        out->addr_.sin_addr = *reinterpret_cast<struct in_addr *>(he->h_addr);
        return true;
    }
    else
    {
        if (ret)
        {
            // LOG_SYSERR << "InetAddress::resolve";
        }
        return false;
    }
}

