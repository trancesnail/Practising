#pragma once
#include "../Header.h"
#include <iostream>
#include <vector>

#define CompletionPortThreadCount 20

#define SEND_POSTED 0
#define RECV_POSTED 1
#define ACCEPT 2


#define DATA_BUFFER_LEN 1024


typedef struct {
	WSABUF* buf;
	int len;
}  WSADataBuf, *LPPER_WSADataBuf_DATA;


//单句柄数据定义
typedef struct _PER_HANDLE_DATA
{
	SOCKET Socket;     //相关的套接字
	SOCKADDR_STORAGE clientAddr;     //客户端的地址
}PER_HANDLE_DATA, *LPPER_HANDLE_DATA;

//单IO操作数据
typedef struct {
	OVERLAPPED Overlapped;
	WSABUF Buffer;						//一个数据缓冲区,用于WSASend/WSARecv中的第二个参数
	WSADataBuf DataBuf;    //实际的数据缓冲区
	int dataLength;                     //实际的数据缓冲区长度
	int OperationType;                 //操作类型,可以为SEND/RECV两种
	SOCKET client;                     //分别表示发送的字节数和接收的字节数
}PER_IO_DATA, *LPPER_IO_DATA, *LPPER_IO_0PERATI0N_DATA;

class TestServer
{
public:
	void Run();
};

