// Client.cpp : 定义控制台应用程序的入口点。
//
#include "Client/TestClient.h"
#include "Client/IOCPClient.h"
#include <thread>
#include <vector>
#include <iostream>

void  WorkerThread()
{
	IOCPClient c;
	c.Run();
}

int main()
{
	//	1.TestClient1	Chat
	//	TestClient1 c;
	//	2.TestClient2	IOCP(...)
	//	TestClient2 c;
	//	3.IOCPClient
	//	IOCPClient c;


	//IOCPClient c;
	//c.Run();

	int m_nThreads = 1000;
	HANDLE* m_phWorkerThreads = new HANDLE[m_nThreads];
	std::vector<std::thread> threads;
	for (int i = 0; i < m_nThreads; i++)
	{
		std::cout << i << std::endl;;
		threads.push_back(std::thread(WorkerThread));
		Sleep(500);
	}
	for (std::vector<std::thread>::iterator It = threads.begin(); It != threads.end();It++)
	{
		It->join();
	}
	return 0;
}