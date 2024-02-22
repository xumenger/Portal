>Agent 程序基于Protobuf 与Portal 服务端通信

Agent 在本项目架构中的作用是：

1. 部署在各个工作机器上
2. 启动的时候注册到Portal
3. 实时向Portal反馈工作机器的负载情况
4. 接收Portal的指令创建工作进程
5. 调度管理工作进程

