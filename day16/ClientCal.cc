#include <iostream>
#include <time.h>
#include <unistd.h>
#include <assert.h>
#include "Socket.hpp"
#include "Protocol.hpp"
void Usage(std::string proc)
{
    std::cout<<"\n\rUsage: "<<proc<<" port[1024+]\n"<<std::endl;
}
//./tcpclient ip port
int main(int args,char* argv[])
{
    if(args!=3)
    {
     Usage(argv[0]);
     exit(1);
    }
    std::string serverip=argv[1];
    uint16_t serverport=std::stoi(argv[2]);
    
    Sock sockfd;
    sockfd.Socket();//创建套接字
    bool r=sockfd.Connect(serverip,serverport);//发起连接
    if(!r)return 1;

    
    srand(time(nullptr)^getpid());
    int cnt=1;
    std::string oper="+-*/%=$";
    std::string inbuffer_stream;
    while(cnt<=10)
    {
        std::cout<<"========第"<<cnt<<"次测试"<<"============"<<std::endl;
        //1.开始构建请求
        int x=rand()%100+1;
        usleep(1234);
        int y=rand()%100;
        usleep(4321);
        char op=oper[rand()%oper.size()];
        Request req(x,y,op);
        //2.请求构建完毕
        req.DebugPrint();
        //3.数据序列化形成报文
        std::string content;
        req.Serialize(&content);
        //4.添加报头
        std::string packpage=Encode(content);
        //5.发送到网络里
        write(sockfd.Fd(),packpage.c_str(),packpage.size());

        //6.接收服务器端发送来的响应
        char buffer[128];
        ssize_t n=read(sockfd.Fd(),buffer,sizeof(buffer));
       //6.1处理读取
        if(n>0)
        {
            buffer[n]=0;
            inbuffer_stream+=buffer;//接收到的是一个协议报文"len"\n"reslut code"\n
            std::cout<<std::endl;
            std::cout<<"获取到的网络答应："<<std::endl;
            std::cout<<inbuffer_stream<<std::endl;//将从网络里获取到的报文打印出来

            //7.首先需要解包检测
            std::string content;
            bool r =Decode(inbuffer_stream,&content);
            assert(r);
            //8.反序列化，将答应变成客户端可认识的形式
            Response resp;
            r=resp.Deserialize(content);
            assert(r);
            //9.结果响应完成
            resp.DebugPrint();
        }
        std::cout<<"============================="<<std::endl;
        sleep(1);
        cnt++;

    }

    sockfd.Close();
}
