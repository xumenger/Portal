>实现Agent 支持接收Portal 指令并fork 进程

暂时不实现watch 功能，Agent 定时去查询，获取Transfer 发送到Portal 的命令，然后实现多线程进程的创建

还有一个很重要的点需要考虑：

1. Transfer 与Agent 之间的协议该怎么设计？
2. 保证Transfer 存储到Portal 中的key-value 信息可以让Agent 拿到
3. 可以参考K8s 集群中对于etcd 的使用细节！

