// Client.cpp : 定义控制台应用程序的入口点。
//
#include "Client/TestClient.h"
#include "Client/IOCPClient.h"

int main()
{
	//	1.TestClient1	Chat
	//	TestClient1 c;
	//	2.TestClient2	IOCP(...)
	//	TestClient2 c;
	//	3.IOCPClient
	//	IOCPClient c;
	IOCPClient c;
	c.Run();
	return 0;
}