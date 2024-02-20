/**
 * 2022.02.20 by xumenger
 *
 * 集群管理者逻辑实现
 *
 * Manager 以Raft 算法实现，Leader/Follower 管理集群所有工作节点的状态
 * Manager 启动后，所有的Worker 注册到Manager 上，类似于一个服务中心
 * Manager 监控集群中所有Worker 的运行状态
 * Manager 也接收来自Web 应用的管理指令，调度集群
 *
 */
#include "PortalManager.h"

int main()
{

    return 0;
}
