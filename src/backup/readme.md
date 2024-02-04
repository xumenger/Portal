## base

基础服务，封装其他模块使用的基础类

## net

事件驱动模块，包括网络通信、定时器实现

## extend

框架支持的扩展方式，包括so、lua

## logger

本地日志

## raft

raft 分布式共识算法核心实现

## master

管理进程，负责管理各个master 进程

管理进程之间是基于raft 算法实现的主从模式，master 是一个集群，包括leader 与follower

## worker

工作进程，通过so、lua 的方式实现业务逻辑扩展

