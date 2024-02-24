/**
 * date: 20240224
 * desc: 工作进程的封装
 */
#ifndef PORTAL_WORKER_H
#define PORTAL_WORKER_H

#include <iostream>

#include "./AgentClient.h"

namespace portal {
namespace client {

class PortalWorker {
    public:
        /**
         * 构造方法
         *
         * agent: 代理客户端对象
         * name: 工作进程名称
         * thread_count: 工作进程线程数
         */
        Worker(AgentClient agent, std::string name, int thread_count);

        ~Worker();

        // 启动工作进程
        void Start();

        // 关闭工作进程
        void Stop();

    protected:
        AgentClient agent;
        std::string name;
        int thread_count;
};

}
}

#endif
