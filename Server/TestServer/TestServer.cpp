#include "TestServer.h"

DWORD WINAPI WorkerThread(PVOID pParam)
{
	HANDLE CompletionPort = (HANDLE)pParam;
	DWORD  BytesTransferred;
	LPOVERLAPPED Overlapped;
	LPPER_HANDLE_DATA PerHandleData;
	LPPER_IO_0PERATI0N_DATA PerIoData;
	DWORD  SendBytes, RecvBytes;
	DWORD Flages;
	while (TRUE)
	{
		//wait for I/O to Complete on any socket
		// associated with the completionport
		GetQueuedCompletionStatus(CompletionPort,
			&BytesTransferred,
			(PULONG_PTR)&PerHandleData,
			(LPOVERLAPPED  *)&PerIoData, INFINITE);
		//first check o see whether an error  has occurr
		// on the socket ,if so ,close the socke and clearup the 
		//per-handle and Per-I/O operation data associated with 
		//socket 

		if (PerIoData->OperationType == ACCEPT)
		{
			std::cout << "ClientData:" << GetCurrentThreadId() << std::endl;
		}

		if ((BytesTransferred == 0) &&
			(PerIoData->OperationType == RECV_POSTED) &&
			(PerIoData->OperationType == SEND_POSTED))
		{
			//A Zero BytesTransferred indicates that the 
			//socke has been closed by the peer,so you should 
			//close the socket 
			//Note: Per-handle Data was used to refresence the 
			// socket associated with the I/O operation;
			closesocket(PerHandleData->Socket);
			GlobalFree(PerHandleData);
			GlobalFree(PerIoData);
			continue;
		}
		//service the completed I/O request;You 
		//detemine which I/O request has just completed 
		//by looking as the operationType field contained
		// the per-I/O operation data 
		if (PerIoData->OperationType == RECV_POSTED)
		{
			//do someting with the received data 
			//in PerIoData->Buffer

		}

		//Post another WSASend or WSARecv operation 
		//as a example we will post another WSARecv()

		Flages = 0;

		//Set up the Per-I/O Operation Data for a next 
		//overlapped call

		ZeroMemory(&(PerIoData->Overlapped), sizeof(OVERLAPPED));
		PerIoData->DataBuf.len = DATA_BUFFER_LEN;
		PerIoData->DataBuf.buf = &PerIoData->Buffer;
		PerIoData->OperationType = RECV_POSTED;

		WSARecv(PerHandleData->Socket,
			&(PerIoData->Buffer), 1, &RecvBytes,
			&Flages, &(PerIoData->Overlapped), NULL);
	}
	//int count = *((int *)pParam);
	//std::cout << count << std::endl;

	return 0;
}


void TestServer::Run()
{
	HANDLE CompletionPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, CompletionPortThreadCount);
	SYSTEM_INFO si;

	GetSystemInfo(&si);

	DWORD m_nProcessors = si.dwNumberOfProcessors;
	int m_nThreads = 2 * m_nProcessors + 5;

	HANDLE* m_phWorkerThreads = new HANDLE[m_nThreads];

	for (int i = 0; i < m_nThreads; i++)
	{
		m_phWorkerThreads[i] = ::CreateThread(0, 0, WorkerThread, CompletionPort, 0, 0);
		CloseHandle(m_phWorkerThreads[i]);
	}


	WSADATA wsaData;

	WSAStartup(MAKEWORD(2, 2), &wsaData);

	//��ʼ��Socket

	struct sockaddr_in ServerAddress;

	// ������Ҫ�ر�ע�⣬���Ҫʹ���ص�I/O�Ļ����������Ҫʹ��WSASocket����ʼ��Socket

	// ע�������и�WSA_FLAG_OVERLAPPED����

	SOCKET m_sockListen = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);

	LPPER_HANDLE_DATA perDandleData;
	perDandleData = (LPPER_HANDLE_DATA)GlobalAlloc(GPTR, sizeof(PER_HANDLE_DATA));
	perDandleData->Socket = m_sockListen;
	CreateIoCompletionPort((HANDLE)m_sockListen, CompletionPort, (ULONG_PTR)perDandleData, 0);

	// ����ַ�ṹ��Ϣ

	ZeroMemory((char *)&ServerAddress, sizeof(ServerAddress));

	ServerAddress.sin_family = AF_INET;

	// �������ѡ����κ�һ�����õĵ�ַ���������Լ�ָ����һ��IP��ַ

	ServerAddress.sin_addr.s_addr = htonl(INADDR_ANY);

	//INT Ture = inet_pton(ServerAddress.sin_family,"192.168.0.141", &ServerAddress.sin_addr);

	ServerAddress.sin_port = htons(6000);

	// ��ʼ����
	if (SOCKET_ERROR != bind(m_sockListen, (struct sockaddr *) &ServerAddress, sizeof(ServerAddress)))
	{
		listen(m_sockListen, SOMAXCONN);
	}

	LPFN_ACCEPTEX     lpfnAcceptEx;         // AcceptEx����ָ��

	GUID GuidAcceptEx = WSAID_ACCEPTEX;        // GUID�������ʶ��AcceptEx���������

	DWORD dwBytes = 0;

	WSAIoctl(
		m_sockListen,
		SIO_GET_EXTENSION_FUNCTION_POINTER,
		&GuidAcceptEx,
		sizeof(GuidAcceptEx),
		&lpfnAcceptEx,
		sizeof(lpfnAcceptEx),
		&dwBytes,
		NULL,
		NULL);
	if (lpfnAcceptEx == nullptr)
	{
		std::cout << "AcceptExָ���ȡʧ��" << std::endl;
	}

	std::vector<LPPER_IO_DATA> ClientData;
	LPPER_IO_DATA perIoData = (LPPER_IO_DATA)GlobalAlloc(GPTR, sizeof(PER_IO_DATA));
	//׼������ AcceptEx �������ú���ʹ���ص��ṹ������ɶ˿�����
	memset(&(perIoData->Overlapped), 0, sizeof(OVERLAPPED));
	perIoData->OperationType = ACCEPT;
	//��ʹ��AcceptExǰ��Ҫ�����ؽ�һ���׽���������ڶ�������������Ŀ���ǽ�ʡʱ��
	//ͨ�����Դ���һ���׽��ֿ�
	perIoData->client = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, 0, 0, WSA_FLAG_OVERLAPPED);

	perIoData->dataLength = DATA_BUFFER_LEN;
	DWORD flags = 0;
	while (1)
	{
		//����AcceptEx��������ַ������Ҫ��ԭ�е��������16���ֽ�
		//ע������ʹ�����ص�ģ�ͣ��ú�������ɽ�������ɶ˿ڹ����Ĺ����߳��д���
		//std::cout << "Process AcceptEx function wait for client connect..." << std::endl;
		int rc = lpfnAcceptEx(m_sockListen, perIoData->client, perIoData->DataBuf.buf,
			perIoData->dataLength - ((sizeof(SOCKADDR_IN) + 16) * 2),
			sizeof(SOCKADDR_IN) + 16, sizeof(SOCKADDR_IN) + 16, &dwBytes,
			&(perIoData->Overlapped));


		if (GetOverlappedResult((HANDLE)m_sockListen, &(perIoData->Overlapped), 0, 0))
		{
			std::cout << "GetOverlappedResult Sucess...:" << std::endl;
		}
		//std::cout << "Connect:" << rc << std::endl;

		//if (rc == FALSE)
		//{
		//	if (WSAGetLastError() != ERROR_IO_PENDING)
		//		std::cout << "lpfnAcceptEx failed.." << std::endl;
		//}


		if (rc == TRUE)
		{
			std::cout << "Connect Sucess...:" << std::endl;
			//��¼�����ӵ�����
			ClientData.push_back(perIoData);


			perIoData = (LPPER_IO_DATA)GlobalAlloc(GPTR, sizeof(PER_IO_DATA));
			//׼������ AcceptEx �������ú���ʹ���ص��ṹ������ɶ˿�����
			memset(&(perIoData->Overlapped), 0, sizeof(OVERLAPPED));
			perIoData->OperationType = ACCEPT;
			//��ʹ��AcceptExǰ��Ҫ�����ؽ�һ���׽���������ڶ�������������Ŀ���ǽ�ʡʱ��
			//ͨ�����Դ���һ���׽��ֿ�
			perIoData->client = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, 0, 0, WSA_FLAG_OVERLAPPED);

			perIoData->dataLength = DATA_BUFFER_LEN;
			/*std::cout << "lpfnAcceptEx Sucess...:"<< ClientData.size() << std::endl;*/
		}
		else
		{

		}
	}
}
