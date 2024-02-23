#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <assert.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#include "../protobuf/Agent.pb.h"

/**
 * 服务器上的代理节点
 * 监听Portal的消息
 * 
 * 进行具体的工作
 * 1. fork方式创建工作进程
 * 2. 监控服务器上的工作进程
 * 3. 向Portal汇报服务器运行情况
 * 
 * 版本1: 先实现fork多线程进程空跑功能
 *       暂不支持so、lua
 * 版本2: 支持so
 * 版本3: 支持lua
 * 版本4: 从Docker哪里能找到什么灵感
 * 
 * 模块划分：
 * 1. 进程模块
 * 2. 线程模块
 * 3. so模块
 * 4. lua模块
 * 
 */
int main(int argc, char const *argv[])
{
    if (argc <= 3) {
        printf("args params less than 3");
        return -1;
    }

    const char *agent_name = argv[1];
    const char *portal_ip = argv[2];
    int portal_port = atoi(argv[3]);

    struct sockaddr_in server_address;
    bzero(&server_address, sizeof(server_address));
    server_address.sin_family = AF_INET;
    inet_pton(AF_INET, portal_ip, &server_address.sin_addr);
    server_address.sin_port = htons(portal_port);

    int sockfd = socket(PF_INET, SOCK_STREAM, 0);
    assert(sockfd >= 0);
    if (connect(sockfd, (struct sockaddr*)&server_address, sizeof(server_address)) < 0) {
       printf("connection failed\n");
    }
    else {
        com::xum::proto::agent::AgentRegister reg;
        reg.set_agent_name(agent_name);
        reg.set_host_ip("127.0.0.1");
        
        char ss[1024];
        reg.SerializeToArray(ss, 1024);

        if (send(sockfd, ss, strlen(ss) + 1, 0) <= 0) {
            printf("send error\n");
        }
    }
    close(sockfd);



	// 启动后注册到Portal

	// 监听Portal，处理任务
	// 涉及到哪些网络接口？

	// 支持so方式fork多线程进程

	// 支持lua方式fork多线程进程

	return 0;
}
