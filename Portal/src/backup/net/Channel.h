/**
 * 2022.03.13
 *
 */

#ifndef PORTAL_NET_CHANNEL_H
#define PORTAL_NET_CHANNEL_H

#include <boost/function.hpp>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>

#include "../base/Timestamp.cc"


namespace portal
{
namespace net 
{

using namespace portal::base;

class EventLoop;

// A selectable I/O channel
// This class doesn't own the file descriptor.
// The file descriptor could be a socket,
// an eventfd, a timerfd, or a signalfd
class Channel : boost:noncopyable
{
    public:
        // 定义事件发生时回调函数指针类型
        typedef boost::function<void()> EventCallback;

        // 定义收到数据时回调函数指针类型
        typedef boost::function<void(Timestamp)> ReadEventCallback;

        // EventLoop 和Channel 是一对多的关系！
        Channel(EventLoop *loop, int fd);
        ~Channel();

        void handleEvent(Timestamp receiveTime);

        // 设置可读事件发生时的回调
        void setReadCallback(const ReadEventCallback &cb)
        {
            readCallback_ = cb;
        }

        // 设置可写事件发生时的回调
        void setWriteCallback(const EventCallback &cb)
        {
            writeCallback_ = cb;
        }

        // 设置关闭连接事件发生时的回调
        void setCloseCallback(const EventCallback &cb)
        {
            closeCallback_ = cb;
        }

        // 设置错误发生时的回调
        void setErrorCallback(const EventCallback &cb)
        {
            errorCallback_ = cb;
        }


        // Tie this channel to the owner object managed by shared_ptr,
        // prevent the owner object being destroyed in handleEvent.
        void tie(const boost::shared_ptr<void> &);

        // Channel 与sock_fd 时一对一的关系
        int fd() const
        {
            return fd_;
        }

        // fd 关注的事件
        int events() const 
        {
            return events_;
        }

        // 在Poller 调用poll 后，设置该sockfd 的活动事件
        void set_revents(int revt)
        {
            revents = revt;
        }

        bool isNoneEvent() const
        {
            return events_ == kNoneEvent;
        }

        // 设置可读、可写属性
        void enableReading()
        {
            events_ |= kReadEvent;
            update();
        }
        void disableReading()
        {
            events_ &= kReadEvent;
            update();
        }
        void enableWriting()
        {
            events_ |= kWriteEvent;
            update();
        }
        void disableWriting()
        {
            events_ &= kWriteEvent;
            update();
        }
        void disableAll()
        {
            events_ = kNoneEvent;
            update();
        }

        bool isWriting() const
        {
            return events_ & kWriteEvent;
        }
        bool isReading() const
        {
            return events_ & kReadEvent;
        }

        // for Poller
        // 在poll 数组中的顺序
        int index()
        {
            return index_;
        }
        void set_index(int idx)
        {
            index_ = idx;
        }

        // for debug
        string reventsToString() const;
        string eventsToString() const;

        void doNotLogHup()
        {
            logHup_ = false;
        }

        EventLoop *ownerLoop()
        {
            return loop_;
        }
        void remove();

    private:
        static string eventsToString(int fd, int ev);

        void update();
        void handleEventWithGuard(Timestamp receiveTime);

        static const int kNoneEvent;
        static const int kReadEvent;
        static const int kWriteEvent;

        EventLoop *loop_;
        const int fd_;
        int events_;
        int revents_;     // it's the received event types of epoll or poll
        int index_;
        bool logHup_;

        boost::weak_ptr<void> tie_;
        bool tied_;
        bool eventHandling_;
        bool addedToLoop_;
        ReadEventCallback readCallback_;
        EventCallback writeCallback_;
        EventCallback closeCallback_;
        EventCallback errorCallback_;

};


};
};

#endif

