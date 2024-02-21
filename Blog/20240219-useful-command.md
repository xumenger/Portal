>一些基础的命令操作

## MacOS

```shell
# 解决VMWare启动虚拟机时报错问题
sudo launchctl limit maxfiles 10240 10240

# protobuf 编译为Java 程序
protoc -I=./ --java_out=../Transfer/src/main/java/ Transfer.proto

# protobuf 编译为C++ 程序
protoc -I=./ --cpp_out=../Portal/portal/ Transfer.proto
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