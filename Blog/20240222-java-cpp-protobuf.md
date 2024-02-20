>Java 与C++ 之间给基于Protobuf 指定TCP 通信协议

主要的预期是实现这样的功能，信息流如下：

1. Postman 发送JSON 格式的请求给到Transfer
2. Transfer 转换为Protobuf 给到Portal
3. Portal 在转发给Agent
4. Agent 根据请求fork 一个进程
