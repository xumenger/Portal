#include "./Poller.h"
#include "./Channel.h"

using namespace portal::net;

Poller::Poller(EventLoop *loop)
    : ownerLoop_(loop)
{
}

Poller::~Poller()
{
}

// 搜索ChannelMap，检查channel 是否在这个容器里面
bool Poller::hasChannel(Channel *channel) const
{
    assertInLoopThread();

    ChannelMap::const_iterator it = channels_.find(channel->fd());
    return it != channels_.end() && it->second == channel;
}

