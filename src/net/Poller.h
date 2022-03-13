/**
 * 2022.03.13
 *
 */

#ifndef PORTAL_NET_POLLER_H
#define PORTAL_NET_POLLER_H

#include <map>
#include <vector>
#include <boost/noncopyable.hpp>

#include "../base/Timestamp.h"

#include "./EventLoop.h"


namespace portal
{
namespace net
{

class Channel;

// IO 多路复用的基类
class Poller : boost:noncopyable
{
    public:
        typedef std::vector<Channel *> ChanneList;

        Poller(EventLoop *loop);
        virtual ~Poller();       // 因为该类设计为基类，会有其他子类继承它，所以析构函数需要virtual

        // Polls the I/O events
        // Must be called in the loop thread.
        virtual Timestamp poll(int timeoutMs, ChanneList *activeChannels) = 0;

        // Changes the interested I/O events.
        // Must be called in the loop thread.
        // 注册和更新关注的事件，所有的fd 都需要调用它添加到事件循环中
        virtual void updateChannel(Cha *channel) = 0;

        // Remove the channel, when it destructs.
        // Must be called in the loop thread.
        virtual void removeChannel(Channel *channel) = 0;

        virtual bool hasChannel(Channel *channel) const;

        static Poller *newDefaultPoller(EventLoop *loop);

        void assertInLoopThread() const
        {
            ownerLoop_->assertInLoopThread();
        }

    protected:
        // key 是sock_fd
        typedef std::map<int, Channel *> ChannelMap;
        ChannelMap channels_;

    private:
        EventLoop *ownerLoop_;

};


};
};

#endif

