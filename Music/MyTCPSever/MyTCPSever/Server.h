//Reference source: www.youtube.com/watch?v=TP5Q0cs6uNo
#include<WinSock2.h>
#include<iostream>
//#pragma comment(lib,"ibwsock32.a")
# pragma warning(disable:4996)

using namespace std;

#define BUFFER_SIZE 512

#define PORT 8080

class Server
{
private:
	WSADATA Winsockdata;
	SOCKET TCPServerSocket;
	struct sockaddr_in TCPServerAdd;
	struct sockaddr_in TCPClientAdd;
	int iTCPClientAdd = sizeof(TCPClientAdd);
	SOCKET sAcceptSocket;
	char RecvBuffer[BUFFER_SIZE];
	int iBufferSize = BUFFER_SIZE;

public:
	Server();
	Server(string target);
	void isConnect();
	void sendData(char* data);
	char* receiveData();
	int getBufferSize();
	void setBufferSize(int newBufferSize);
	~Server();

};


