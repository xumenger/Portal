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