>一些基础的命令操作

1. 网络相关的命令有哪些，分别用于排查哪些网络问题
2. Linux进程相关的命令有哪些，分别用于排查哪些问题
3. Linux线程相关的命令有哪些，分别用于排查哪些问题
4. JDK相关的命令有哪些，分别用于排查哪些问题


## MacOS

```shell
# 解决VMWare启动虚拟机时报错问题
sudo launchctl limit maxfiles 10240 10240

# protobuf 编译为Java 程序
protoc -I=./ --java_out=../Transfer/src/main/java/ Transfer.proto

# protobuf 编译为C++ 程序
protoc -I=./ --cpp_out=../Portal/protobuf/ Transfer.proto
```


## g++

```shell
# 编译使用到Protobuf
g++ -std=c++11 Agent.cpp ../protobuf/Portal.pb.cc -o Agent `pkg-config --cflags --libs protobuf`
g++ -std=c++11 Portal.cpp ../protobuf/Portal.pb.cc -o Portal `pkg-config --cflags --libs protobuf`
```


## gdb

首先gcc 或者g++ 编译的时候需要加上-g 参数

```shell
# 调试某个程序，比如调试Portal 进程
gdb Portal

# 使用break 或者b 在指定行行增加断点
> b 79

# 查看断点设置情况
> info b

# 删除指定断点
> delete <breakpoint_id>

# 执行run 或者r 启动程序
> r

# list 或者l 查看当前断点附近的代码
> l

# n 或者next 步进调试；s 或者step 步入调试
> n
> s

# p 或者print 可以打印某个变量的值
> p msg_len
```


## git

```shell
# 克隆远程仓库
git clone https://github.com/xumenger/Portal.git

# 将文件加入版本控制
git add *

# 提交变更
git commit -m "描述"

# push到远程master 分支
git push origin master

# 切换到一个分支，如果没有则创建一个新的分支
git checkout -b v0.1-20240220-springboot

# push到远程v0.1-20240220-springboot 分支
git push origin v0.1-20240220-springboot

# 删除本地分支
git branch -d v0.1-20240220-springboot

# 删除远程分支
git push origin --delete v0.1-20240220-springboot
```