>Java 与C++ 之间给基于Protobuf 指定TCP 通信协议

主要的预期是实现这样的功能，信息流如下：

1. Postman 发送JSON 格式的请求给到Transfer
2. Transfer 转换为Protobuf 给到Portal
3. Portal 在转发给Agent
4. Agent 根据请求fork 一个进程

## MacOS安装protobuf

protobuf3下载地址：https://github.com/protocolbuffers/protobuf/releases

有很多语言版本的，mac下选择第一个

https://github.com/protocolbuffers/protobuf/releases/tag/v3.13.0

```shell
# 下载下来后解压压缩包，并进入目录
cd protobuf-3.13.0/

# 设置编译目录
./configure --prefix=/usr/local/protobuf

# 切换到root用户
sudo -i

# 先执行编译命令：
make

# 这个执行的比较慢，执行完后再执行命令进行安装：
make install

# 退出root 用户
exit

# 编辑配置
vim ~/.bash_profile

# 末尾添加
# export PROTOBUF=/usr/local/protobuf 
# export PATH=$PROTOBUF/bin:$PATH

# 保存后执行source命令使文件生效：
source ~/.bash_profile

# 测试安装结果
protoc --version
```

## 编译Protobuf 文件供Java 使用

在pom.xml 增加Protobuf 依赖

```xml
<dependency>
  <groupId>com.google.protobuf</groupId>
  <artifactId>protobuf-java</artifactId>
  <version>3.13.0</version>
</dependency>
```

```shell
cd Protobuf/

protoc -I=./ --java_out=../Transfer/src/main/java/ Transfer.proto
```

编译之后得到的Java 文件可能在@Override 的地方有报错，这是由于jdk版本的问题，@Override是JDK5就已经有了，但有个小小的Bug，就是不支持对接口的实现，认为这不是Override 而JDK6修正了这个Bug，无论是对父类的方法覆盖还是对接口的实现都可以加上@Override。

首先要确保安装了jdk 1.6 以上的版本，然后，在eclipse中修改配置，在 Windows->Preferences->java->Compiler->compiler compliance level 中选择 1.6（我设置的是1.8），刷新工程，重新编译下；如果还是不行，就在报错的工程上，鼠标右键选择 Properties->Java Compiler->compiler compliance level中选择 1.6（我设置的是1.8），刷新工程，重新编译下

在Controller 中编写TCP 客户端程序

```java
package com.xum.controller;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.Socket;
import java.net.UnknownHostException;

import org.springframework.beans.factory.annotation.Value;
import org.springframework.stereotype.Controller;
import org.springframework.web.bind.annotation.PostMapping;
import org.springframework.web.bind.annotation.RequestBody;
import org.springframework.web.bind.annotation.RequestMapping;

import com.xum.portal.TransferProto.CreateRequest;

@Controller
@RequestMapping("/manage")
public class ManageController {
    
    @Value("${portal.ip}")
    String gPortalIP;
    
    @Value("${portal.port}")
    int gPortalPort;
    

    @PostMapping("/create")
    public String Create(@RequestBody CreateParam request) throws UnknownHostException, IOException{
        
        // 创建TCP客户端
        Socket clientSocket = new Socket(gPortalIP, gPortalPort);
        
        // 创建输入流和输出流
        InputStream inputStream = clientSocket.getInputStream();
        OutputStream outputStream = clientSocket.getOutputStream();
        
        // 创建请求报文
        CreateRequest proto = CreateRequest.newBuilder()
                .setTaskName(request.getTaskName())
                .setThreadCount(request.getThreadCount())
                .build();
        byte[] data = proto.toByteArray();
        
        // 发送数据给服务端
        outputStream.write(data);
        
        // 读取服务端响应
        byte[] response = new byte[1024];
        int bytesRead = inputStream.read(response);
        String receivedMessage = new String(response, 0, bytesRead);
        System.out.println("收到服务器响应：" + receivedMessage);
        
        // 关闭连接
        clientSocket.close();
        
        return "success";
    }
    
    
    /**
     * 使用静态内部类，简单定义一个入参实体类
     */
    static class CreateParam {
        private String taskName;        // 任务名称
        private Integer threadCount;     // 线程数量

        public String getTaskName() {
            return taskName;
        }
        public void setTaskName(String taskName) {
            this.taskName = taskName;
        }
        public Integer getThreadCount() {
            return threadCount;
        }
        public void setThreadCount(Integer threadCount) {
            this.threadCount = threadCount;
        }
    }
    
}
```


## 编译Protobuf 文件供C++ 使用

```shell
cd Protobuf/

# 编译后生成Transfer.pb.cc、Transfer.pb.h
protoc -I=./ --cpp_out=../Portal/portal/ Transfer.proto
```

编写C++ 服务端程序

```c++

```
