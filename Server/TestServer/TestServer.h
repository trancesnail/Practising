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


//��������ݶ���
typedef struct _PER_HANDLE_DATA
{
	SOCKET Socket;     //��ص��׽���
	SOCKADDR_STORAGE clientAddr;     //�ͻ��˵ĵ�ַ
}PER_HANDLE_DATA, *LPPER_HANDLE_DATA;

//��IO��������
typedef struct {
	OVERLAPPED Overlapped;
	WSABUF Buffer;						//һ�����ݻ�����,����WSASend/WSARecv�еĵڶ�������
	WSADataBuf DataBuf;    //ʵ�ʵ����ݻ�����
	int dataLength;                     //ʵ�ʵ����ݻ���������
	int OperationType;                 //��������,����ΪSEND/RECV����
	SOCKET client;                     //�ֱ��ʾ���͵��ֽ����ͽ��յ��ֽ���
}PER_IO_DATA, *LPPER_IO_DATA, *LPPER_IO_0PERATI0N_DATA;

class TestServer
{
public:
	void Run();
};

