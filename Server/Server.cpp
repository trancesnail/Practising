// Server.cpp : 定义控制台应用程序的入口点。
//
#include "Header.h"
#include "Testserver/TestServer.h"
#include "EventServer/EventServer.h"


//#pragma comment (lib,"LibEvent.lib")


int main()
{	

	//	tutorial::Person per;

	//	1.
	//	TestServer svr;
	//	2.
	//	EventTimerServer svr;
	//	3.
	//	EventHelloServer svr
	//	4.
	//	EventIOCPServer svr
	EventIOCPServer svr;
	svr.Run();


	return 0;
}
