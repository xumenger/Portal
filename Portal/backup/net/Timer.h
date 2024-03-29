/**
 * 2022.03.20
 *
 * 定时器
 *
 */ 

#ifndef PORTAL_NET_TIMER_H
#define PORTAL_NET_TIMER_H

#include <boost/noncopyable.hpp>

#include "../base/Atomic.h"
#include "../base/Timestamp.h"
#include "./Callbacks.h"

namespace portal 
{
namespace net 
{

class Timer : boost::noncopyable 
{
    public:
        Timer(const TimerCallback &cb, Timestamp when, double interval)
            : callback_(cb),
              expiration_(when),
              interval_(interval),
              repeat_(interval > 0.0),
              sequence_(s_numCreated_.incrementAndGet())
        {
        }

        void run() const 
        {
            callback_();
        }

        // expiration 是截止的意思
        Timestamp expiration() const 
        {
            return expiration_;
        }

        bool repeat() const 
        {
            return repeat_;
        }

        int64_t sequence() const 
        {
            return sequence_;
        }

        void restart(Timestamp now);

        static int64_t numCreated()
        {
            return s_numCreated_.get();
        }

    private:
        const TimerCallback callback_;
        Timestamp expiration_;
        const double interval_;
        const bool repeat_;
        const int64_t sequence_;

        // 使用static关键字，表示该变量是类的成员变量，而不是对象的成员变量
        static AtomicInt64 s_numCreated_;
};

};
};


#endif

