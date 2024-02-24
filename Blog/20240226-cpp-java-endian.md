>TCP 通信时的大小端问题

在Java 端编写这样的客户端代码：

```java
// 创建TCP客户端
Socket clientSocket = new Socket("172.16.192.128", 7777);

// 创建输入流和输出流
InputStream inputStream = clientSocket.getInputStream();
OutputStream outputStream = clientSocket.getOutputStream();

// 测试将int 类型的72 转换为byte 类型发送
byte[] test_int_bytes = ByteBuffer.allocate(Integer.BYTES).putInt(72).array();
outputStream.write(test_int_bytes);
```

在C++ 端编写这样的服务端代码：

```c++
int32_t test_int;
int len = recv(connect_fd, &test_int, 4, 0);
```

可能会出现大小端不一致的问题，在C++ 端收到的test_int 预期是72，但实际是1207959552

72 对应的字节应该是`00000000 00000000 00000000 01001000`，大端位

1207959552 对应的字节是`01001000 00000000 00000000 00000000`，小端位

显然是大端位和小端位的问题导致的，怎么解决这类问题？

由于Java 发送的都是网络字节序（大端位），而C++ 是主机字节序，因此当消息中有整型、浮点型的时候需要使用htonl、htons、ntohl、ntohs 等函数转换一下。对于C/C++程序，send() 发送前使用htonl、htons，将主机字节序转换成网络字节序；recv() 接收之后，调用ntohl、ntohs，将网络字节序转换为主机字节序！对于Java 代码则不需要做处理！

```c++
#include <arpa/inet.h>


int32_t test_int;
int len = recv(connect_fd, &test_int, 4, 0);
test_int = ntohl(test_int);
```

字符串因为是单字节排序的，不需要转换！但需要注意C++ 字符串是以`'/0'` 为结束符的，若找不到`'/0'` 可能会出现一些乱码，因此接收的时候能够分配一个length+1 的buffer 用来接收消息
