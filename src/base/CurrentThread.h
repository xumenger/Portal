/**
 * 2022.03.16
 *
 * extern 与"C" 一起使用时，比如`extern "C" void fun(int a, int b)`
 * 是告诉编译器在编译fun 这个函数时按照C 的规则去翻译相应的函数名，而不是C++ 的
 * C++ 的规则在翻译这个函数名时会把fun 这个名字变得面目全非，比如`fun@aBc_int_int#%$`
 * 因为C++ 支持函数的虫灾
 *
 * extern 不与"C" 一起修饰变量或函数时，比如在头文件中`extern int g_Int`
 * 它的作用时声明函数或全局变量的作用范围的关键字，其声明的函数和便利可以在本模块或其他模块中使用
 * 它是一个声明而不是定义
 * 也就是说B 模块（编译单元）要是引用A 模块中定义的全局变量或函数时，它只要包含A 模块的头文件即可
 * 在编译阶段，B 模块虽然找不到该函数或变量，但不会报错，会在连接时从A 模块生成的目标的代码中找到此函数
 *
 */

#ifndef PORTAL_BASE_CURRENTTHREAD_H
#define PORTAL_BASE_CURRENTTHREAD_H

#include <stdint.h>

namespace portal
{
namespace CurrentThread
{
    /**
     * __thread 关键字是GCC 内置的线程局部存储设施。存取效率可以和全局变量相比
     * __thread 变量每个线程有一份独立实体，各个线程的值互不干扰，类似Java 中的ThreadLocal
     */
    extern __thread int t_cachedTid;           // 缓存线程ID

    /**
     * 将线程ID 以字符串的形式缓存起来，方便输出日志等情况下可以快速获取
     * 这样就可以在使用时不需要再去调用函数获取int的ID，以及调用函数去将int转成string
     * 典型的空间换时间
     */
    extern __thread char t_tidString[32];
    extern __thread int t_tidStringLength;
    extern __thread const char *t_threadName;

    void cacheTid();

    inline int tid()
    {
        // 如果发现线程ID 没有缓存，则去缓存线程ID
        // GCC 内建函数`__builtin_expect` 帮助处理分支预测
        if (__builtin_expect(t_cachedTid == 0, 0))
        {
            cacheTid();
        }
        return t_cachedTid;
    }

    inline const char *tidString()     // for logging
    {
        return t_tidString;
    }

    inline int tidStringLength()       // for logging
    {
        return t_tidStringLength;
    }

    inlien const char *name()
    {
        return t_threadName;
    }

    bool isMainThread();

    void sleepUsec(int64_t usec);

};
};


#endif

