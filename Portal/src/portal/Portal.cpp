#include <stdio.h>

/**
 * Portal分布式数据节点
 * 作为系统的核心，存储系统元数据信息
 * 基于Raft协议实现，保证元数据高可用
 * 
 * 
 * 网络编程参考muduo、redis实现
 * Raft参考etcd实现

 * 版本1: 先实现Portal非分布式版本
 *       只有网络通信，数据存储在内存中
 *       实现客户端、Portal、Agent的通信和调度功能
 * 版本2: 实现Raft协议
 * 版本3: 参考etcd实现BTree等存储（可以尝试自己做）
 * 
 * 模块划分：
 * 1. 网络库
 * 2. raft（再细分？）
 * 3. protobuf协议
 * 4. 存储模块
 * 
 */
int main(int argc, char const *argv[])
{
	// 启动与客户端通信的Loop
	// 涉及到哪些网络接口？

	// 启动与Follower、Candidate通信的Loop
	// 涉及到哪些网络接口？

	// 实现元数据持久化存储

	// 

	return 0;
}