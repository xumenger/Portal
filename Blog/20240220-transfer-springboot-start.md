>搭建一个简单的SpringBoot 项目

首先在Eclipse 中创建一个Maven 项目：【New Maven projec】->【Create a simpl project (skip archtype selection】

在pom.xml 中增加SpringBoot 依赖

```xml
<project xmlns="http://maven.apache.org/POM/4.0.0" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
  xsi:schemaLocation="http://maven.apache.org/POM/4.0.0 http://maven.apache.org/maven-v4_0_0.xsd">
  <modelVersion>4.0.0</modelVersion>
  <groupId>com.xum</groupId>
  <artifactId>Transfer</artifactId>
  <packaging>jar</packaging>
  <version>0.0.1-SNAPSHOT</version>
  <name>Transfer Maven Webapp</name>
  <url>http://maven.apache.org</url>
  
  <dependencies>
    <dependency>
      <groupId>org.springframework.boot</groupId>
      <artifactId>spring-boot-starter-web</artifactId>
      <version>1.5.6.RELEASE</version>
    </dependency>
    
    <dependency>
      <groupId>org.springframework.boot</groupId>
      <artifactId>spring-boot-starter-web</artifactId>
      <version>1.5.6.RELEASE</version>
    </dependency>
  </dependencies>
    
  <build>
    <finalName>Transfer</finalName>
  </build>
</project>
```

在src/main/resources 下面增加application.yml，内容如下：

```yml
server:
  port: 8881
```

增加入口启动类

```java
package com.xum;

import org.springframework.boot.SpringApplication;
import org.springframework.boot.autoconfigure.SpringBootApplication;

@SpringBootApplication 
public class TransferApplication {
	public static void main(String[] args) 
    {
        SpringApplication.run(TransferApplication.class, args);
    }
}
```

增加ManageController

```java
package com.xum.controller;

import org.springframework.stereotype.Controller;
import org.springframework.web.bind.annotation.PostMapping;
import org.springframework.web.bind.annotation.RequestBody;
import org.springframework.web.bind.annotation.RequestMapping;

@Controller
@RequestMapping("/manage")
public class ManageController {

    @PostMapping("/create")
    public String Create(@RequestBody CreateParam request){
        System.out.println(request.getTaskName());
        return "success";
    }


    /**
     * 使用静态内部类，简单定义一个入参实体类
     */
    static class CreateParam {
        private String taskName;        // 任务名称
        private String threadCount;     // 线程数量

        public String getTaskName() {
            return taskName;
        }
        public void setTaskName(String taskName) {
            this.taskName = taskName;
        }
        public String getThreadCount() {
            return threadCount;
        }
        public void setThreadCount(String threadCount) {
            this.threadCount = threadCount;
        }
    }
}
```

启动后，在Postman 发起请求：127.0.0.1:8881/manage/create

```json
{
    "taskName": "testTask",
    "threadCount": "222"
}
```
