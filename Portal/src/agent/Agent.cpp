#include <stdio.h>

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
	// 启动后注册到Portal

	// 监听Portal，处理任务
	// 涉及到哪些网络接口？

	// 支持so方式fork多线程进程

	// 支持lua方式fork多线程进程

	return 0;
}