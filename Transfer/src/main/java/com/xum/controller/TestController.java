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
     * 遗留问题：
     * 1. 针对tomcat、Servlet后续可以继续针对性深入研究
     * 2. 为什么在Postman看到返回值并不是预期结果，而是
     * {
     *      "timestamp": 1708442742976,
     *      "status": 404,
     *      "error": "Not Found",
     *      "message": "No message available",
     *      "path": "/test/json"
     *  }
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