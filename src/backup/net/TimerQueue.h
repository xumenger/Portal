/**
 * 2022.03.20
 *
 * 定时器队列
 *
 */ 

#ifndef PORTAL_NET_TIMERQUEUE_H
#define PORTAL_NET_TIMERQUEUE_H

#include <set>
#include <vector>

#include <boost/noncopyable.hpp>

#include "../base/Mutex.h"
#include "../base/Timestamp.h"
#include "./Callbacks.h"
#include "./Channel.h"

namespace portal 
{
namespace net 
{

class EventLoop;
class Timer;
class TimerId;


class TimerQueue : boost::noncopyable 
{
    public:
        explicit TimerQueue(EventLoop *loop);
        ~TimerQueue();

        TimerId addTimer(const TimerCallback &cb, Timestamp when, double interval);

        void cancel(TimerId &timerId);

    private:
        typedef std::pair<Timestamp, Timer *> Entry;
        typedef std::set<Entry> TimerList;
        typedef std::pair<Timer *, int64_t> ActiveTimer;
        typedef std::set<ActiveTimer> ActiveTimerSet;

        void addTimerInLoop(Timer *timer);
        void cancelInLoop(TimerId timerId);

        // called when timerfd alarms 
        void handleRead();

        // move out all expired timers 
        std::vector<Entry> getExpired(Timestamp now);

        void reset(const std::vector<Entry> &expired, Timestamp now);

        bool insert(Timer *timer);

        EventLoop *loop_;
        const int timerfd_;
        Channel timerfdChannel_;

        // Timer list sorted by expiration
        TimerList timers_;

        // for cancel()
        ActiveTimerSet activeTimers_;
        bool callingExpiredTimers_;
        ActiveTimerSet cancelingTimers_;

};

};
};

#endif


