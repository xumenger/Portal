/**
 * 2022.02.20 by xumenger
 *
 * Portal 分布式框架的Master 组件
 * 负责管理集群中的所有工作节点
 *
 */
#ifndef PORTAL_MANAGER_H
#define PORTAL_MANAGER_H

#include <iostream>
#include <set>

using namespace std;

namespace portal
{
namespace manager
{
    // 集群中的工作节点信息
    class PortalWorker
    {
        public:
            string workerName;      // 工作节点名称
            float cpuUsage;         // 工作节点CPU使用率
            float memUsgae;         // 工作节点内存使用率

            int threadCount;        // 工作节点工作进程的线程数
            long lastHeartbeat;     // 工作节点最后心跳时间
    };


    // 集群中的管理节点信息
    class PortalManager
    {
        set<PortalWorker> workers;  // 集群中所有的工作节点

    };

}
}

#endif
