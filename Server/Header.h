#pragma once
#include <winsock2.h>
#include <windows.h>
#include <Ws2tcpip.h>
#include <mswsock.h>
#include <time.h>
#include <stdio.h>
#include <string>
#pragma comment(lib,"Ws2_32.lib")
#pragma comment(lib,"mswsock.lib")


static std::string GetTime()
{
	SYSTEMTIME Time = { 0 };
	GetLocalTime(&Time);//获得当前本地时间
	char cTime[32] = { 0 };
	sprintf(cTime, "[%4d-%02d-%02d %02d:%02d:%02d.%03d]", Time.wYear ,Time.wMonth, Time.wDay, Time.wHour, Time.wMinute, Time.wSecond, Time.wMilliseconds);
	std::string s(cTime);
	return s;
}