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

	EventHelloServer svr;
	svr.Run();


	return 0;
}
