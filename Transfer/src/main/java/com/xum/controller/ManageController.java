package com.xum.controller;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.Socket;
import java.net.UnknownHostException;
import java.nio.ByteBuffer;

import org.springframework.beans.factory.annotation.Value;
import org.springframework.stereotype.Controller;
import org.springframework.web.bind.annotation.PostMapping;
import org.springframework.web.bind.annotation.RequestBody;
import org.springframework.web.bind.annotation.RequestMapping;

import com.xum.proto.PortalProto.PortalMessageType;
import com.xum.proto.PortalProto.SetRequest;

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
        String key = "CreateWorker";
        String value = "{\n" + 
                "    \"taskName\": \"testTask\",\n" + 
                "    \"threadCount\": \"222\"\n" + 
                "}";
        SetRequest setReq = SetRequest.newBuilder()
                .setKey(key)
                .setValue(value)
                .build();
        byte[] data = setReq.toByteArray();
        
        
        byte[] type_bytes = ByteBuffer.allocate(Integer.BYTES).putInt(PortalMessageType.MsgSetReq_VALUE).array();
        
        System.out.println("data.length: " + data.length);
        byte[] len_bytes = ByteBuffer.allocate(Integer.BYTES).putInt(data.length).array();
        
        byte[] len_bytes_test = ByteBuffer.allocate(Integer.BYTES).putInt(1207959552).array(); 
        System.out.println("len_bytes_test: " + len_bytes_test);
        
        outputStream.write(type_bytes);
        outputStream.write(len_bytes);
        
        // 发送数据给服务端（是否存在大小端问题？）
        // 发送消息的时候，需要封装消息类型、长度的逻辑，是否单独封装一个API？
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
