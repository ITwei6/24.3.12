#pragma once
//#define MySelf 1
// 在网络通信之前，我们服务器端和客户端都需要知道协议。我们也可以自己定制协议，这个协议要被双方都能识别
// 比如我们可以定制一个计数器协议。协议就是一种约定，除了数据本身还有其他的字段。
// 1.我们要求将数据以结构化的形式保存这样双方都可以识别这个结构体对象，但传入网络里时，需要转换成字符类型。这个过程就是序列化.序列化的过程就是在构建有效载荷
// 2.对方接收到字符串类型的数据时，想要用服务操作时，发现是不能操作的，是因为它不认识，这时还需要将字符类型转成结构体类型，这个过程叫做反序列化。
// 3.为了能让对方接收时，能接收读取到对方想要的完整报文时，我们采取添加报头的形式来解决。
// 4.所以在将报文传入到网络里时，还需要添加报文，当对端接收到报文时，想要对它进行处理之前，还需要将报文的报头解包才可以正确处理。
#include <iostream>
#include <jsoncpp/json/json.h>
#include <string>
const std::string blank_space = " ";
const std::string protocol_space="\n";
// 封包：报文在发送到网络之前需要添加一些报头，来达到一些要求
std::string Encode(const std::string &content)//content就是有效载荷
{
  //"x + y"------>"len"\n"x + y"\n"   添加了一个报文长度和两个\n
  std::string packpage=std::to_string(content.size());
  packpage+=protocol_space;
  packpage+=content;
  packpage+=protocol_space;
  return packpage;
}

// 解包：对端读取到报文(可能读取到的不是想要的，根据原先添加上去的报头来获取准确想要的报文)，想要处理它，需要先解除报头才能处理
bool Decode(std::string &packpage, std::string *content)
{ 
  //"len"\n"x + y"\n"---->"x + y"
  std::size_t pos=packpage.find(protocol_space);
  if(pos==std::string::npos)
  return false;

  std::string len_str=packpage.substr(0,pos);
  //判断一下是否读取的内容是全部的
  std::size_t len =std::stoi(len_str);
  std::size_t total_len=len_str.size()+len+2;
  if(packpage.size()<total_len)//说明不是一个完整的报文
  return false;
  *content=packpage.substr(pos+1,len);
  //为了真正的拿走报文，还需要将响应inbuffer里的报文移除erase，这样才是真正的拿走报文
  
  packpage.erase(0,total_len);
  return true;
}

class Request
{
public:
  Request()
  {}
  Request(int data1, int data2, char op) : _x(data1), _y(data2), _op(op) // 最初形成结构化数据
  {
  }
  bool Serialize(std::string *out) // 序列化，单纯的就是将结构体转换成字符串
  {
#ifdef MySelf    // 构建报文的有效载荷
    // struct==》"x + y"
    std::string s = std::to_string(_x);
    s += blank_space;
    s += _op;
    s += blank_space;
    s += std::to_string(_y);
    *out = s;
    return true;
 #else
     Json::Value root;//定义一个万能对象，可以存储数据，k-v形式的结构体
     root["x"]=_x;
     root["y"]=_y;
     root["op"]=_op;
     //Json::FastWriter w;
     Json::StyledWriter w;
     *out=w.write(root);//序列化成字符串
     return true;   
    
 #endif

  }
  bool Deserialize(std::string &in) // 反序列化，就单纯的将字符串类型转成结构体
  {
#ifdef MySelf   
    //"x + y"==>struct
    //获取左操作数x
    std::size_t left=in.find(blank_space);
    if(left==std::string::npos)
    return false;
    std::string part_x=in.substr(0,left);
    //获取右操作数y
    std::size_t right=in.rfind(blank_space);
    if(right==std::string::npos)
    return false;
    std::string part_y=in.substr(right+1);
    //获取操作码op
    if(left+2!=right)
    return false;

    _op=in[left+1];
    _x=std::stoi(part_x);
    _y=std::stoi(part_y);
    return true;
#else
    Json::Value root;//定义一个万能对象，将序列化的数据存储在里面
    Json::Reader r;
    r.parse(in,root);
    //将数据存到万能对象里后，我们就可以根据key值找到
    _x=root["x"].asInt();
    _y=root["y"].asInt();
    _op=root["op"].asInt();
    return true;
#endif    
  }
    void DebugPrint()
    {
        std::cout<<"新请求构建完毕："<<_x<<_op<<_y<<"=???"<<std::endl;
    }
public: // x + y
  int _x;
  int _y;
  char _op;
};
class Response
{
public:
  Response(int reslut, int code) : _reslut(reslut), _code(code)
  {
  }
  Response()
  {}
  bool Serialize(std::string *out) // 序列化，单纯的就是将结构体转换成字符串
  {
#ifdef MySelf
    //"reslut code"
    //构建报文的有效载荷
    std::string s=std::to_string(_reslut);
    s+=blank_space;
    s+=std::to_string(_code);
    *out=s;
    return true;
#else
     Json::Value root;
     root["reslut"]=_reslut;
     root["code"]=_code;
     //Json::FastWriter w;
     Json::StyledWriter w;
     *out=w.write(root);
     return true;
#endif    
  }
   bool Deserialize(std::string &in)
   {
#ifdef MySelf
    //"reslut code"-->结构体类型
    std::size_t pos=in.find(blank_space);
    if(pos==std::string::npos)
    return false;

    std::string part_left=in.substr(0,pos);
    std::string part_right=in.substr(pos+1);

    _reslut=std::stoi(part_left);
    _code=std::stoi(part_right);
    return true;
#else
    Json::Value root;
    Json::Reader r;
    r.parse(in,root);//将字符串数据存到万能对象里

    _reslut=root["reslut"].asInt();
    _code=root["code"].asInt();
    return true;
#endif
   }

   void DebugPrint()
   {
    std::cout<<"结果响应完成,reslut: "<<_reslut<<",code: "<<_code<<std::endl;
   }

public:
  int _reslut;
  int _code;
};
