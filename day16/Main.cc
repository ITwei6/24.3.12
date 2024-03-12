#include "Tcpserver.hpp"
#include "ServerCal.hpp"
#include <memory>
void Usage(std::string proc)
{
    std::cout << "\n\rUsage: " << proc << " port[1024+]\n"
              << std::endl;
}

int main(int args, char *argv[])
{

    if (args != 2)
    {
        Usage(argv[0]);
        exit(0);
    }

    uint16_t port = std::stoi(argv[1]);
    ServerCal cal;
    Tcpserver *tcpsvr = new Tcpserver(port, std::bind(&ServerCal::Calculator, &cal, std::placeholders::_1));
    tcpsvr->Init();
    tcpsvr->Start();
    return 0;
}