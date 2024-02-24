/**
 * Agent 客户端通信对接
 *
 */
#ifndef PORTAL_AGENT_CLIENT_H
#define PORTAL_AGENT_CLIENT_H

#include <iostream>
#include <list>

#include "./PortalWorker.h"

namespace portal {
namespace client {

class AgentClient {
    public:
        /**
         * 构造方法
         * name: 代理客户端名称
         * host: 代理客户端所属IP
         */
        AgentClient(std::string name, std::string host);

        virtual ~AgentClient();

        /**
         * 启动一个新的Worker进程
         * worker_name: 工作进程的名称
         * thread_count: 工作进程的线程数
         */
        void forkWorker(string worker_name, int thread_count);

        // STL中list数据结构的原理？内存管理？
        std::list<PortalWorker> listWorker();

    protected:
        string name;
        string host;
};

}
}

#endif
