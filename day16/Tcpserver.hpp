#pragma once
#include <iostream>
#include <string>
#include <functional>
#include "Log.hpp"
#include <signal.h>
#include "Socket.hpp"
#include "ServerCal.hpp"
Sock sock;
using func_t =std::function<std::string(std::string &package)>;

class Tcpserver
{

public:
    Tcpserver(uint16_t port,func_t callback) : _port(port),_callback(callback)
    {
    }
    bool Init()
    {
        _listensock.Socket();    // 创建套接字
        _listensock.Bind(_port); // 绑定套接字
        _listensock.Listen();    // 将套接字设置成监听状态
        lg(Info, "init server...done");
        return true;
    }
    void Start() // 启动服务器
    {            // 启动之前需要先忽略一些信号
        signal(SIGPIPE, SIG_IGN);
        signal(SIGCHLD, SIG_IGN);
        // 获取连接
        while (true)
        {
            std::string clientip;
            uint16_t clientport;
            int sockfd = _listensock.Accept(&clientip, &clientport);
            if (sockfd < 0)
                continue;
            lg(Info, "accept a new link, sockfd:%d, clientip：%s ,clientport: %d",sockfd,clientip.c_str(),clientport);
            
            // 提供服务-->让子进程提供服务
            if (fork() == 0)
            {
                _listensock.Close();
                std::string inbuffer_stream;
                while (true)
                {
                    // 1.读取网络中的数据流
                    char buffer[128];
                    size_t n = read(sockfd, buffer, sizeof(buffer));
                    if (n > 0)
                    {
                        buffer[n]=0;
                        //将读取的报文进行处理
                       inbuffer_stream+=buffer;//注意读取的内容必须是一个完整的内容，不然调用回调时，就回调用失败
                       lg(Debug,"获取的网络请求:\n %s",inbuffer_stream.c_str());
                       std::string info=_callback(inbuffer_stream);
                       
                       if(info.empty())continue;//如果进行计算时，发现报文有问题，就重新回来读取。

                       // 2.将处理的结果发送回网络中
                       write(sockfd,info.c_str(),info.size());
                    }
                    else if(n==0)break;
                    else break;

                    
                }
                exit(0);
            }
            close(sockfd);
        }
    }
    ~Tcpserver()
    {
    }

private:
    uint16_t _port;
    Sock _listensock;
    func_t _callback;
};