/**
 * 2022.03.16
 * 互斥量
 *
 */

#ifndef PORTAL_BASE_MUTEX_H
#define PORTAL_BASE_MUTEX_H


#include "./CurrentThread.h"
#include <boost/noncopyable.hpp>
#include <assert.h>
#include <pthread.h>

namespace portal
{

class MutexLock : boost::noncopyable
{
    public:
        MutexLock()
            :holder_(0)
        {
            /**
             * pthread_mutex_init() 函数是以动态方式创建互斥锁的，参数attr指定了新建互斥锁的属性
             * 如果参数attr为空(NULL)，则使用默认的互斥锁属性，默认属性为快速互斥锁
             * 互斥锁的属性在创建锁的时候指定，在LinuxThreads实现中仅有一个锁类型属性
             * 不同的锁类型在试图对一个已经被锁定的互斥锁加锁时表现不同
             *
             * 返回0 表示初始化成功，其他表示失败
             */
            assert(0 == pthread_mutex_init(&mutex_, NULL));
        }

        ~MutexLock()
        {
            assert(0 == holder_);
            assert(0 == pthread_mutex_destroy(&mutex_));
        }

        bool isLockedByThisThread() const
        {
            // CurrentThread 是portal::base::CurrentThread 命名空间
            return holder_ == CurrentThread::tid();
        }

        void assertLocked() const
        {
            assert(isLockedByThisThread());
        }


        void lock()
        {
            assert(0 == pthread_mutex_lock(&mutex_));
            assignHolder();
        }

        void unlock()
        {
            unassignHolder();
            assert(0 == pthread_mutex_unlock(&mutex_));
        }

        pthread_mutex_t *getPthreadMutex()
        {
            return &mutex_;
        }

    private:
        friend class Condition;

        class UnassignGuard : boost:noncopyable
        {
            public:
                UnassignGuard(MutexLock &owner)
                    : owner_(owner)
                {
                    owner_.unassignHolder();
                }

                ~UnassignGuard()
                {
                    owner_.assignHolder();
                }
                
            private:
                MutexLock &owner_;
        };

        void unassignHolder()
        {
            holder_ = 0;
        }

        void assignHolder()
        {
            holder_ = CurrentThread::tid();
        }

        pthread_mutex_t mutex_;
        pid_t holder_;
};


// Use as a stack variable, eg.
// int Foo::size() const
// {
//     MutexLockGuard lock(mutex_);
//     return data_.size();
// }
// 利用栈上临时变量作用域生命周期自动调用析构函数函数，保证加锁解锁的一一对应
class MutexLockGuard : boost::noncopyable
{
    public:
        explicit MutexLockGuard(MutexLock &mutex)
            : mutex_(mutex)
        {
            mutex_.lock();
        }

        ~MutexLockGuard()
        {
            mutex_.unlock();
        }

    private:
        MutexLock &mutex_;

};


// Prevent misuse like:
// MutexLockGuard(mutex_);
// A tempory object doesn't hold the lock for long!
//
// 只能这样用: `MutexLockGuard lock(mutex_);`
#define MutexLockGuard(x) error "Missing guard object name"


};

#endif

