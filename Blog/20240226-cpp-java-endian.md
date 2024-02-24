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

