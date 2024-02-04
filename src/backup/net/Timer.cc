#include "./Timer.h"

using namespace portal;
using namespace portal::net;

AtomicInt64 Timer::s_numCreated_;


void Timer::restart(Timestamp now)
{
    if (repeat_)
    {
        expiration_ = addTime(now, interval_);
    }
    else 
    {
        expiration_ = Timestamp::invalid();
    }
}


