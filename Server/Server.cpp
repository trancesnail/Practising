// Server.cpp : �������̨Ӧ�ó������ڵ㡣
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
	if (svr.Init(0))
	{
		svr.Run();
	}

	return 0;
}
