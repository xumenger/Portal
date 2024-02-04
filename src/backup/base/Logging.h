/**
 * 2022.03.20 
 * 日志
 *
 */

#ifndef PORTAL_BASE_LOGGING_H
#define PORTAL_BASE_LOGGING_H

#include "./LogStream.h"
#include "./Timestamp.h"

namespace portal 
{
class TimeZone;

class Logger
{
    public:
        // 定义日志级别
        enum LogLevel
        {
            TRACE,
            DEBUG,
            INFO,
            WARN,
            ERROR,
            FATAL,
            NUM_LOG_LEVELS,
        };

        // compile time calculation of basename of source file
        class SourceFile
        {
            public:
                template<int N>
                inline SourceFile(const char (&arr)[N])
                    : data_(arr),
                      size_(N - 1)
                {
                    const char *slash = strrchr(data_, '/');
                    if (slash)
                    {
                        data_ = slash + 1;
                        size_ -= static_cast<int>(data_ - arr);
                    }
                }

                explicit SourceFile(const char *filename)
                    : data_(filename)
                {
                    // char *strrchr(const char *str, int c) 在参数 str 所指向的字符串中搜索最后一次出现字符 c（一个无符号字符）的位置
                    const char *slash = strrchr(filename, '/');
                    if (slash)
                    {
                        data_ = slash + 1;
                    }
                    size = static_cast<int>(strlen(data_));
                }

                const char *data_;
                int size_;
        };

        Logger(SourceFile file, int line);
        Logger(SourceFile file, int line, LogLevel level);
        Logger(SourceFile file, int line, LogLevel level, const char *func);
        Logger(SourceFile file, int line, bool toAbort);
        ~Logger();

        LogStream &stream()
        {
            return impl_.stream_;
        }

        static LogLevel logLevel();
        static void setLogLevel(LogLevel level);

        // 定义函数指针类型
        typedef void (*OutputFunc)(const char *msg, int len);
        typedef void (*FlushFunc)();

        static void setOutput(OutputFunc);
        static void setFlush(FlushFunc);
        static void setTimeZone(const TimeZone &tz);

    private:
        class Impl
        {
            public:
                typedef Logger::LogLevel LogLevel;
                Impl(LogLevel level, int old_errno, const SourceFile &file, int line);
                void formatTime();
                void finish();

                Timestamp time_;
                LogStream stream_;
                LogLevel level_;
                int line_;
                SourceFile basename_;
        };

        Impl impl_;

};


extern Logger::LogLevel g_logLevel;

inline Logger:LogLevel Logger::logLevel()
{
    return g_logLevel;
}


#define LOG_TRACE if (portal::Logger::logLevel() <= portal::Logger::TRACE) \
  portal::Logger(__FILE__, __LINE__, portal::Logger::TRACE, __func__).stream()
#define LOG_DEBUG if (portal::Logger::logLevel() <= portal::Logger::DEBUG) \
  portal::Logger(__FILE__, __LINE__, portal::Logger::DEBUG, __func__).stream()
#define LOG_INFO if (portal::Logger::logLevel() <= portal::Logger::INFO) \
  portal::Logger(__FILE__, __LINE__).stream()
#define LOG_WARN portal::Logger(__FILE__, __LINE__, portal::Logger::WARN).stream()
#define LOG_ERROR portal::Logger(__FILE__, __LINE__, portal::Logger::ERROR).stream()
#define LOG_FATAL portal::Logger(__FILE__, __LINE__, portal::Logger::FATAL).stream()
#define LOG_SYSERR portal::Logger(__FILE__, __LINE__, false).stream()
#define LOG_SYSFATAL portal::Logger(__FILE__, __LINE__, true).stream()


const char *strerror_tl(int savedErrno);


#define CHECK_NOTNULL(val) \
    ::portal::CheckNotNull(__FILE__, __LINE__, "'" #val "' Must be non NULL", (val))


// A small helper for CHECK_NOTNULL().
template <typename T>
T *CheckNotNull(Logger::SourceFile file, int line, const char *names, T *ptr)
{
    if (ptr == NULL)
    {
        Logger(file, line, Logger::FATAL).stream() << names;
    }

    return ptr;
}

};


#endif

