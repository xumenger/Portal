>搭建一个简单的SpringBoot 项目

首先在Eclipse 中创建一个Maven 项目：【New Maven projec】->【Create a simpl project (skip archtype selection】

在pom.xml 中增加SpringBoot 依赖

```xml
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
    
    <dependency>
      <groupId>com.alibaba</groupId>
      <artifactId>fastjson</artifactId>
      <version>1.2.73</version>
  </dependency>
  </dependencies>
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

增加TestController

```java
package com.xum.controller;

import org.springframework.stereotype.Controller;
import org.springframework.web.bind.annotation.PostMapping;
import org.springframework.web.bind.annotation.RequestBody;
import org.springframework.web.bind.annotation.RequestMapping;

import com.alibaba.fastjson.JSONObject;

@Controller
@RequestMapping("/test")
public class TestController {

    /**
     * 请求入参是一个实体,并且加上了 @RequestBody
     * 一般适用于前端Header中Content-Type 为 application/json的场景
     * 注意入参要是json格式
     * @param request
     * @return json
     * 
     */
    @PostMapping("/json")
    public String Create(@RequestBody CreateParam request){
        System.out.println(request.getTaskName());
        
        JSONObject result = new JSONObject();
        result.put("msg", "ok");
        result.put("method", "POST");
        result.put("result", request);
        
        System.out.println(result.toJSONString());
        
        return result.toJSONString();
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

## 遗留问题

需要针对一次HTTP请求，深入分析：

1. 在tomcat层面的表现，逐行调试代码
2. 在Servlet层面的表现
3. 在TCP层面的表现
4. 在Spring层面的表现，逐行调试代码

为什么在Postman看到返回值并不是预期结果，而是

```json
{
    "timestamp": 1708442742976,
    "status": 404,
    "error": "Not Found",
    "message": "No message available",
    "path": "/test/json"
}
```
