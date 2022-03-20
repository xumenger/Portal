/**
 * 2022.03.20
 *
 */

#ifndef PORTAL_BASE_LOGSTREAM_H
#define PORTAL_BASE_LOGSTREAM_H

#include "./Types.h"

#include <assert.h>
#include <string.h>   // memcpy
#include <string>
#include <boost/noncopyable.hpp>

namespace portal
{
namespace detail
{

const int kSmallBuffer = 4000;
const int kLargeBuffer = 4000 * 1000;

// 定义一个“定长Buffer”类。通过data_、cur_ 指针，可以事先申请好定长内存，通过这两个指针管理缓冲区，避免内存的频繁申请、释放、扩展
template<int SIZE>
class FixedBuffer : boost::noncopyable
{
    public:
        FixedBuffer()
            : cur_(data_)
        {
            // data_ 是缓冲区的起始地址；cur_ 指向缓冲区的“头部”，暂时称其为缓冲区指针
            // 从data_ 到cur_ 之间的缓冲区是已经使用了的缓冲区
            setCookie(cookieStart);
        }

        ~FixedBuffer() 
        {
            setCookie(cookieEnd);
        }

        // 往缓冲区中添加数据
        void append(const char *buf, size_t len)
        {
            if (implicit_cast<size_t>(avail()) > len)
            {
                memcpy(cur_, buf, len);
                cur_ += len;
            }
        }

        // 获取缓冲区的起始地址
        const char *data() const
        {
            return data_;
        }

        // 获取已使用的缓冲区的长度
        int length() const
        {
            return static_cast<int>(cur_ - data_);
        }

        // write to data_ directly
        char &current()
        {
            return curr_;
        }

        // 获取剩下的可用缓冲区大小
        int avail() const 
        {
            return static_cast<int>(end() - cur_);
        }

        void add(size_t len)
        {
            cur_ += len;
        }

        // 将缓冲区指针指向缓冲区开始的地方，也就是重置缓冲区
        void reset()
        {
            cur_ = data_;
        }

        void bzero() 
        {
            ::bzero(data_, sizeof data_);
        }

        // for used by GDB
        const char *debugString();

        void setCookie(void (*cookie)())
        {
            cookie_ = cookie;
        }

        // for used by unit test
        string toString() const 
        {
            return string(data_, length());
        }

    private:
        const char *end() const 
        {
            return data_ + sizeof data_;
        }

        // Must be outline function for cookies.
        static void cookieStart();
        static void cookieEnd();

        void (*cookie_)();

        char data_[SIZE];
        char *cur_;
};
};


// 日志流类，核心功能就是重载各种类型的 << 操作符
class LogStream : boost::noncopyable
{
    typedef LogStream self;

    public:
        // 定义一个detail::kSmallBuffer 大小的内存缓冲区
        typedef detail::FixedBuffer<detail::kSmallBuffer> Buffer;

        // 重载 << 操作符。往缓冲区中写入数据
        self &operator<<(bool v)
        {
            Buffer_.append(v ? "1" : "0", 1);
            return *this;
        }

        // 重载各种类型的 << 操作符。返回值是 self& 是因为需要这样使用：stream << 1 << "str" << ... ;
        self &operator<<(short);
        self &operator<<(unsigned short);
        self &operator<<(int);
        self &operator<<(unsigned int);
        self &operator<<(long);
        self &operator<<(unsigned long);
        self &operator<<(long long);
        self &operator<<(unsigned long long);

        self &operator<<(const void *);

        self &operator<<(float v)
        {
            *this << static_cast<double>(v);
            return this;
        }
        self &operator<<(double);

        self &operator<<(char v)
        {
            buffer_.append(&v, 1);
            return *this;
        }

        self &operator<<(const char *str)
        {
            if (str)
            {
                buffer_.append(str, strlen(str));
            }
            else 
            {
                buffer_.append("(null)", 6);
            }

            return *this;
        }

        self &operator<<(const unsigned char *str)
        {
            // reinterpret_cast <new_type> (expression)
            // reinterpret_cast运算符是用来处理无关类型之间的转换
            // 它会产生一个新的值，这个值会有与原始参数（expressoin）有完全相同的比特位
            return operator<<(reinterpret_cast<const char *>(str));
        }

        self &operator<<(const string &v)
        {
            buffer_.append(v.c_str(), v.size());
            return *this;
        }

        self &operator<<(const Buffer &v)
        {
            *this << v.toString();
            return *this;
        }

        // 往buffer 中添加数据
        void append(const char *data, int len)
        {
            buffer_.append(data, len);
        }

        // 获取buffer 对象
        const Buffer &buffer() const 
        {
            return buffer_;
        }

        // 重置buffer_
        void resetBuffer()
        {
            buffer_.reset();
        }

    private:
        void staticCheck();

        template<typename T>
        void formatInteger(T);

        Buffer buffer_;

        static const int kMaxNumbericSize = 32;
};


// 格式化类
class Fmt
{
    public:
        template<typename T>
        Fmt(const char *ftm, T val);

        const char *data() const 
        {
            return buf_;
        }

        int length() const 
        {
            return length_;
        }

    private:
        char buf_[32];
        int length_;
};

// 通过内联函数定义日志的格式化输出函数
inline LogStream &operator<<(LogStream &s, const Fmt &fmt)
{
    s.append(fmt.data(), fmt.length());
    return s;
}


};


#endif

