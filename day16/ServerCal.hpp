#pragma once
#include "Protocol.hpp"
#include <iostream>
#include <string>
// 服务器端，从网络里读取到数据后，就要进行处理服务。
// 1.首先需要对报文进行解包，2.解包后还需要将报文转成结构体类型对方才能识别
enum
{
  Div_Zero = 1,
  Mod_Zero,
  Other_Oper
} ;
class ServerCal
{
public:
  Response Calculatorhelpor(const Request &req)
  {
    Response resp(0, 0);
    switch (req._op)
    {
    case '+':
      resp._reslut = req._x + req._y;
      break;
    case '-':
      resp._reslut = req._x - req._y;
      break;
    case '*':
      resp._reslut = req._x * req._y;
      break;
    case '/':
    {
      if (req._y == 0)
        resp._code = Div_Zero;
      else
        resp._reslut = req._x / req._y;
    }
    break;
    case '%':
    {
      if (req._y == 0)
        resp._code = Mod_Zero;
      else
        resp._reslut = req._x % req._y;
    }
    break;

    default:
     resp._code=Other_Oper;
      break;
    }
    return resp;
  }

  std::string Calculator(std::string &package)
  {
    std::string content;                //"len""\n""20 + 10""\n"
    bool r = Decode(package, &content); //"20 + 10"
    if (!r)
      return "";
    Request req; // 反序列化
    r = req.Deserialize(content);
    if (!r)
      return "";

    // 服务器端解包获取到报文后，就可以进行计算，再将计算结果返回回到网络里,网络里需要序列化的数据

    Response res = Calculatorhelpor(req); // reslut=30  code=0
    content = "";
    res.Serialize(&content);   //"30 0"
    content = Encode(content); //"len""\n""30 0""\n"

    return content;
  }
};