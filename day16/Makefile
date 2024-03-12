.PHONY:all
all:tcpserver tcpclient

Flag=-DMySelf=1
Lib=-ljsoncpp


tcpserver:Main.cc
	g++ -o $@ $^ -std=c++11 $(Lib) $(Flag)
tcpclient:ClientCal.cc
	g++ -o $@ $^ -std=c++11 $(Lib) $(Flag)
.PHONY :clean
clean:
	rm -f tcpserver tcpclient
