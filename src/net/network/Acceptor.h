/**
 * 2022.03.13 
 *
 * 用于接受TCP 连接，它是TcpServer 的成员
 * 生命周期由TcpServer 控制
 *
 */
#ifndef PORTAL_NET_ACCEPTOR_H
#define PORTAL_NET_ACCEPTOR_H

#include <boost/function.hpp>
#include <boost/noncopyable.hpp>

#include "portal/event/network/Channel.h"
#include "portal/event/network/Socket.h"


namespace portal
{
namespace net
{


// 头文件中使用了前向声明，大大简化了头文件之间的依赖关系
// Acceptor.h、Channel.h、Connector.h、TcpConnection.h 都前向声明了EventLoop class
class EventLoop;
class InetAddress;


// Acceptor of incoming TCP connections.
// boost::noncopyable 表示不可复制类。不能使用拷贝构造函数、=
class Acceptor : boost::noncopyable
{
    public:
        typedef boost::function<void (int sockfd, const InetAddress&) NewConnectionCallback;

        // 设置对应的EventLoop、监听的地址/端口
        Acceptor(EventLoop *loop, const InetAddress &listenAddr, bool reusePort);
        ~Acceptor();

        // 设置收到新TCP 连接后的回调处理函数
        void setNewConnectionCallback(const NewConnectionCallback &cb)
        {
            newConnectionCallback_ = cb;
        }

        bool listenning()
        {
            return listenning_;
        }

        void listen();

    private:
        void handleRead();

        EventLoop *loop_;
        Socket acceptSocket_;
        Channel acceptChannel_;
        NewConnectionCallback newConnectionCallback_;
        bool listenning_;
        int idleFd_;
};


}
}

#endif
