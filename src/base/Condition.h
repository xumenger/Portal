/**
 * 2022.03.16
 * 条件变量
 *
 */

#ifndef PORTAL_BASE_CONDITION_H
#define PORTAL_BASE_CONDITION_H

#include "./Mutex.h"

#include <boost/noncopyable.hpp>
#include <pthread.h>

namespace portal
{
class Condition : boost::noncopyable
{
    public:
        explicit Condition(MutexLock &mutex)
            : mutex_(mutex)
        {
            assert(0 == pthread_cond_init(&pcond_, NULL));
        }

        ~Condition()
        {
            assert(0 == pthread_cond_destroy(&pcond_));
        }

        void wait()
        {
            MutexLock::UnassignGuard ug(mutex_);
            assert(0 == pthread_cond_wait(&pcond_, mutex_.getPthreadMutex()));
        }

        // returns true if time out, false otherwise
        bool waitForSeconds(double seconds);

        void notify()
        {
            assert(0 == pthread_cond_signal(&pcond_));
        }

        void notifyAll()
        {
            assert(0 == pthread_cond_broadcast(&pcond_));
        }

    private:
        MutexLock &mutex_;
        pthread_cond_t pcond_;
};

};

#endif
