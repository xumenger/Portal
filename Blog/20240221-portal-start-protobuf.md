>搭建初版的Portal 进程

本文档记录编写一个可运行的简单版本Portal 阻塞IO 的服务器的流程

编写Portal.cpp 程序如下：

```c++
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cerrno>

#define PORTAL_PORT 7777  // 端口号
#define BUF_SIZE 1024     // 最大缓存
#define MAX_QUEUE 20      // 最大连接数 


int main(int argc, char const *argv[])
{
    int server_sockfd = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in server_sockaddr;                 // 保存本地地址信息
    server_sockaddr.sin_family = AF_INET;               // 采用ipv4
    server_sockaddr.sin_port = htons(PORTAL_PORT);      // 指定端口
    server_sockaddr.sin_addr.s_addr = htonl(INADDR_ANY);// 获取主机接收的所有响应

    // 绑定本地IP与端口
    if (bind(server_sockfd, (struct sockaddr *)&server_sockaddr, sizeof(server_sockaddr)) == -1) {
        perror("Bind Failuer\n");
        printf("Error: %s\n", strerror(errno));     // 输出错误信息
        return -1;
    }

    // 设置监听状态
    printf ("Listen Port: %d\n", PORTAL_PORT);
    if (listen(server_sockfd, MAX_QUEUE) == -1) {
        perror("Listen Error\n");
        return -1;
    }

    char buffer[BUF_SIZE];                      // 一次传输的数据缓存
    struct sockaddr_in client_addr;             // 保存客户端地址信息
    socklen_t length = sizeof(client_addr);     // 需要的内存大小

    printf("Waiting for connection!\n");

    // 等待连接，返回服务器端建立连接的socket
    int connect_fd = accept(server_sockfd, (struct sockaddr *)&client_addr, &length);
    if (-1 == connect_fd) {
        perror("Connect Error\n");
        return -1;
    }

    printf("Connection Successful\n");

    // 数据收发与传输
    while(1) {
        memset(buffer, 0, sizeof(buffer));
        int len = recv(connect_fd, buffer, sizeof(buffer), 0);
        if (strcmp(buffer, "exit\n") == 0 || len <= 0)
            break;
        printf("client send message: %s", buffer);
        strcpy(buffer, "successful");
        send(connect_fd, buffer, strlen(buffer), 0);
        printf("send message: %s\n", buffer);
    }

    close(connect_fd);
    close(server_sockfd);

    return 0;
}
```

在Ubuntu 执行下面的命令编译程序

```shell
# 注意不能使用g++ -c Portal.cpp -o Portal
# 上述命令得到的是 .o 文件，而不是可执行程序
# 否则运行的时候会报错 bash: ./Portal: cannot execute binary file: Exec format error
g++ Portal.cpp -o Portal
```

然后运行程序

```shell
chmod u+x ./Portal
./Portal
```

在Ubuntu 执行`ifconfig` 查看IP 地址为：172.16.192.128

然后回到MacOS 中执行下面的命令，就可以和服务端通信了

```shell
telnet 172.16.192.128 7777
```


## 遗留问题

1. Socket API 的各个参数的作用需要专门梳理
2. 后续的Socket 编程，可以深度参考redis 代码
3. 网络问题排查常用的命令行有哪些
4. Docker的网络原理？
5. Makefile 怎么写？
6. 关于TCP的状态流转图需要再好好研究！


## 参考资料

[C++网络编程(一)本地socket通信](https://blog.csdn.net/QYQHxyh/article/details/129427692)