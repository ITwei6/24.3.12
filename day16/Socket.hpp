#pragma once
//将网络套接字编程部分直接封装打包，因为服务器和客户端都需要使用创建套接字等操作。
#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "Log.hpp"
#include <cstdlib>
#include <cstring>
#include <unistd.h>
enum
{
  SocketErr=1,
  BindErr,
  ListenErr,
};
const int backlog=10;
class Sock
{

public:
    Sock()
    {}
    ~Sock()
    {}

    void Socket()//创建套接字
    {
     _socket=socket(AF_INET,SOCK_STREAM,0);
     if(_socket<0)
     {
        lg(Fatal,"socket err:%s :%d",strerror(errno),errno);
        exit(SocketErr);
     }
    }
    void Bind(uint16_t &port)//绑定套接字
    {
      struct sockaddr_in local;
      memset(&local,0,sizeof(local));
      local.sin_addr.s_addr=INADDR_ANY;//将ip地址初始化成0
      local.sin_family=AF_INET;
      local.sin_port=htons(port);
      if(bind(_socket,(struct sockaddr*)&local,sizeof(local))<0)
      {
        lg(Fatal,"bind err :%s :%d",strerror(errno),errno);
        exit(BindErr);
      }
    }
    void Listen()//将套接字设置成监听状态
    {
       if(listen(_socket,backlog)<0)
       {
        lg(Fatal,"listen err :%s :%d",strerror(errno),errno);
        exit(ListenErr);
       }
    }
    int Accept(std::string *clientip,std::uint16_t* clientport)//服务器获取连接，并获取对方的网络信息,将获取的到的新连接交给服务函数操作
    {
        struct sockaddr_in client;
        socklen_t len=sizeof(client);
        int newsock=accept(_socket,(struct sockaddr*)&client,&len);
        if(newsock<0)
        {
            lg(Warning,"accept err :%s :%d",strerror(errno),errno);
            return -1;
        }
        *clientport=ntohs(client.sin_port);
        char Clientip[32];
        inet_ntop(AF_INET,&client.sin_addr,Clientip,sizeof(Clientip));
        *clientip=Clientip;
        return newsock;
    }
    bool Connect(const std::string &serverip,const uint16_t &serverport)//需要知道要连接的对方的网络信息
    {
        struct sockaddr_in local;
        socklen_t len=sizeof(local);
        memset(&local,0,sizeof(local));
        local.sin_family=AF_INET;
        local.sin_port=htons(serverport);
        inet_pton(AF_INET,serverip.c_str(),&local.sin_addr);
     
        int n=connect(_socket,(struct sockaddr*)&local,len);
        if(n==-1)
        {
            std::cerr<<"connect to"<<serverip<<":"<<serverport<<"error"<<std::endl;
            return false;
        }
        return true;
    }
    void Close()
    {
        close(_socket);
    }
    int Fd()
    {
      return _socket;
    }
private: 
  int _socket;
};