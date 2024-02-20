Buffer 和InetAddress 具有值语义，可以拷贝；其他class 都是对象语义，不可以拷贝，通过继承boost::noncopyable 实现

## 公开接口

* Buffer
* InetAddress
* EventLoop
* EventLoopThread
* TcpConnection
* TcpClient
* TcpServer

## 内部接口

* Channel
* Socket
* SocketOps
* Poller: PollPoller/EPollPoller
* Connector
* Acceptor
* TimeQueue
* EventLoopThreadPool

