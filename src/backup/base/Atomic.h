/**
 * 2022.03.20
 *
 * 原子类
 */ 

#ifndef PORTAL_BASE_ATOMIC_H
#define PORTAL_BASE_ATOMIC_H

#include <boost/noncopyable.hpp>
#include <stdint.h>

namespace portal 
{
namespace detail 
{

template<typename T>
class AtomicIntegerT : boost::noncopyable
{
    public:
        AtomicIntegerT()
            : value_(0)
        {
        }

        T get()
        {
            // type __sync_val_compare_and_swap (type *ptr, type oldval type newval, ...)
            // 比较*ptr与oldval的值，如果两者相等，则将newval更新到*ptr并返回操作之前*ptr的值
            return __sync_val_compare_and_swap(&value_, 0, 0);
        }

        T getAndAdd(T x)
        {
            // type __sync_fetch_and_add (type *ptr, type value, ...)
            // 将value加到*ptr上，结果更新到*ptr，并返回操作之前*ptr的值
            return __sync_fetch_and_add(&value_, x);
        }

        T addAndGet(T x)
        {
            return getAndAdd(x) + x;
        }

        T incrementAndGet()
        {
            return addAndGet(1);
        }

        T decrementAndGet()
        {
            return addAndGet(-1);
        }

        void add(T x)
        {
            getAndAdd(x);
        }

        void increment()
        {
            incrementAndGet();
        }

        void decrement()
        {
            decrementAndGet();
        }

        T getAndSet(T newValue)
        {
            // type __sync_lock_test_and_set (type *ptr, type value, ...)
            // 将value写入*ptr，对*ptr加锁，并返回操作之前*ptr的值。即，try spinlock语义
            return __sync_lock_test_and_set(&value_, newValue);
        }

    private:
        // volatile
        volatile T value_;
};

};

typedef detail::AtomicIntegerT<int32_t> AtomicInt32;
typedef detail::AtomicIntegerT<int64_t> AtomicInt64;

};


#endif

