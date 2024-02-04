/**
 * 2022.03.20
 *
 */ 

#ifndef PORTAL_NET_TIMERID_H
#define PORTAL_NET_TIMERID_H

#include "../base/copyable.h"

namespace portal 
{
namespace net 
{

class Timer;

class TimerId : public portal::copyable 
{
    public:
        TimerId()
            : timer_(NULL),
              sequence_(0)
        {
        }

        TimerId(Timer *timer, int64_t seq)
            : timer_(timer),
              sequence_(seq)
        {
        }


        friend class TimerQueue;

    private:
        Timer *timer_;
        int64_t sequence_;
};

};
};


#endif

