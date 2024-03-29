/**
 * 2022.03.20
 *
 */

#ifndef PORTAL_NET_CALLBACKS_H
#define PORTAL_NET_CALLBACKS_H

#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>

#include "../base/Timestamp.h"

namespace portal 
{

template<typename To, typename From>
inline ::boost::shared_ptr<To> down_pointer_cast(const ::boost::shared_ptr<From> &f)
{
    if (false)
    {
        implicit_cast<From *, To *>(0);
    }

#ifndef NDEBUG
    assert(f == NULL || dynamic_cast<To *>(get_pointer(f)) != NULL);
#endif

    return ::boost::static_pointer_cast<To>(f);
}

namespace net 
{

class Buffer;
class TcpConnection;

typedef boost::shared_ptr<TcpConnection> TcpConnectionPtr;
typedef boost::function<void()> TimerCallback;
typedef boost::function<void (const TcpConnectionPtr &)> ConnectionCallback;
typedef boost::function<void (const TcpConnectionPtr &)> CloseCallback;
typedef boost::function<void (const TcpConnectionPtr &)> WriteCompleteCallback;
typedef boost::function<void (const TcpConnectionPtr &, size_t)> HighWaterMarkCallback;

// the data has been read to (buf len)
typedef boost::function<void (const TcpConnectionPtr &, Buffer *, Timestamp)> MessageCallback;

void defaultConnectionCallback(const TcpConnectionPtr &conn);
void defaultMessageCallback(const TcpConnectionPtr &conn, Buffer *buffer, Timestamp receiveTime);

};

};

#endif

