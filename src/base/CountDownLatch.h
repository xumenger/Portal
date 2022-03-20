/**
 * 2022.03.16
 *
 */

#ifndef PORTAL_BASE_COUNTDOWNLATCH_H
#define PORTAL_BASE_COUNTDOWNLATCH_H

#include "./Condition.h"
#include "./Mutex.h"

#include <boost/noncopyable.hpp>

namespace portal
{

class CountDownLatch : boost::noncopyable
{
    public:
        explicit CountDownLatch(int count);

        void wait();

        void countDown();

        int getCount() const;

    private:
        // 在C++中，mutable是为了突破const的限制而设置的。被mutable修饰的变量，将永远处于可变的状态，即使在一个const函数中
        mutable MutexLock mutex_;
        Condition condition_;
        int count_;
};

};


#endif

