>Agent 程序基于Protobuf 与Portal 服务端通信

Agent 在本项目架构中的作用是：

1. 部署在各个工作机器上
2. 启动的时候注册到Portal
3. 实时向Portal反馈工作机器的负载情况
4. 接收Portal的指令创建工作进程
5. 调度管理工作进程

本文主要用于实现Agent 启动后主动注册到Portal！

```proto
/**
 * Agent 与Portal 之间的协议接口定义
 */

// 声明proto协议版本
syntax = "proto3";

package com.xum.proto.agent;

message AgentRegister {
    string agent_name = 1;
    string host_ip = 2;
}
```

## 编译Protobuf 文件供C++ 使用

```shell
cd Protobuf/

# 编译后生成Agent.pb.cc、Agent.pb.h
protoc -I=./ --cpp_out=../Portal/protobuf/ Agent.proto
```

## 编译运行客户端程序

```shell
g++ Portal.cpp -o Portal

chmod u+x ./Portal
./Portal
```