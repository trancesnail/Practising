// Client.cpp : �������̨Ӧ�ó������ڵ㡣
//


#ifndef UNICODE
#define UNICODE
#endif

#define WIN32_LEAN_AND_MEAN

#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <winsock2.h>
#include <ws2tcpip.h>
#include <mswsock.h>
#include <stdio.h>

// Need to link with Ws2_32.lib
#pragma comment(lib, "Ws2_32.lib")

int main()
{

	// ����socket��̬���ӿ�(dll)  
	WORD wVersionRequested;
	WSADATA wsaData;    // ��ṹ�����ڽ���Wjndows Socket�Ľṹ��Ϣ��  
	int err;

	wVersionRequested = MAKEWORD(2, 2);   // ����1.1�汾��WinSock��  

	err = WSAStartup(wVersionRequested, &wsaData);
	if (err != 0) {
		return -1;          // ����ֵΪ���ʱ���Ǳ�ʾ�ɹ�����WSAStartup  
	}

	// ����socket������������ʽ�׽��֣������׽��ֺ�sockClient  
	// SOCKET socket(int af, int type, int protocol);  
	// ��һ��������ָ����ַ��(TCP/IPֻ����AF_INET��Ҳ��д��PF_INET)  
	// �ڶ�����ѡ���׽��ֵ�����(��ʽ�׽���)�����������ض���ַ�������Э�飨0Ϊ�Զ���  
	SOCKET sockClient = socket(AF_INET, SOCK_STREAM, 0);

	// ���׽���sockClient��Զ����������  
	// int connect( SOCKET s,  const struct sockaddr* name,  int namelen);  
	// ��һ����������Ҫ�������Ӳ������׽���  
	// �ڶ����������趨����Ҫ���ӵĵ�ַ��Ϣ  
	// ��������������ַ�ĳ���  
	SOCKADDR_IN addrSrv;
	addrSrv.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");      // ���ػ�·��ַ��127.0.0.1;   
	addrSrv.sin_family = AF_INET;
	addrSrv.sin_port = htons(9995);
	connect(sockClient, (SOCKADDR*)&addrSrv, sizeof(SOCKADDR));

	char recvBuf[100] = { 0 };
	recv(sockClient, recvBuf, 100, 0);
	printf("%s", recvBuf);

	send(sockClient, "Attention: A Client has enter...\n", strlen("Attention: A Client has enter...\n") + 1, 0);

	memset(recvBuf,0,100);
	recv(sockClient, recvBuf, 100, 0);
	printf("%s", recvBuf);

	printf("���ǿ�������仰");
	int n = 500;
	do {
		printf("\n��ʣ%d�Σ�", n);
		char talk[100] = {0};
		printf("\nPlease enter what you want to say next(\"quit\"to exit):");
		gets_s(talk);
		send(sockClient, talk, strlen(talk) + 1, 0);          // ������Ϣ  

		char recvBuf[100] = { 0 };
		recv(sockClient, recvBuf, 100, 0);
		printf("%s Says: %s", "Server", recvBuf);     // ������Ϣ  
	} while (--n);

	printf("End linking...\n");
	closesocket(sockClient);
	WSACleanup();   // ��ֹ���׽��ֿ��ʹ��  

	printf("\n");
	return 0;

	////----------------------------------------
	//// Declare and initialize variables
	//WSADATA wsaData;
	//int iResult = 0;
	//BOOL bRetVal = FALSE;

	//HANDLE hCompPort;
	//HANDLE hCompPort2;

	//LPFN_ACCEPTEX lpfnAcceptEx = NULL;
	//GUID GuidAcceptEx = WSAID_ACCEPTEX;
	//WSAOVERLAPPED olOverlap;

	//SOCKET ListenSocket = INVALID_SOCKET;
	//SOCKET AcceptSocket = INVALID_SOCKET;
	//sockaddr_in service;
	//char lpOutputBuf[1024];
	//int outBufLen = 1024;
	//DWORD dwBytes;

	//hostent *thisHost;
	//char *ip;
	//u_short port;

	//// Initialize Winsock
	//iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	//if (iResult != NO_ERROR) {
	//	wprintf(L"Error at WSAStartup\n");
	//	return 1;
	//}

	//// Create a handle for the completion port
	//hCompPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, (u_long)0, 0);
	//if (hCompPort == NULL) {
	//	wprintf(L"CreateIoCompletionPort failed with error: %u\n",
	//		GetLastError());
	//	WSACleanup();
	//	return 1;
	//}

	//// Create a listening socket
	//ListenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	//if (ListenSocket == INVALID_SOCKET) {
	//	wprintf(L"Create of ListenSocket socket failed with error: %u\n",
	//		WSAGetLastError());
	//	WSACleanup();
	//	return 1;
	//}

	//// Associate the listening socket with the completion port
	//CreateIoCompletionPort((HANDLE)ListenSocket, hCompPort, (u_long)0, 0);

	////----------------------------------------
	//// Bind the listening socket to the local IP address
	//// and port 27015
	//port = 6000;
	////thisHost = GetAddrInfo(,);
	//thisHost = gethostbyname("");
	//ip = inet_ntoa(*(struct in_addr *) *thisHost->h_addr_list);

	//service.sin_family = AF_INET;
	//service.sin_addr.s_addr = inet_addr(ip);
	//service.sin_port = htons(port);

	//if (bind(ListenSocket, (SOCKADDR *)& service, sizeof(service)) == SOCKET_ERROR) {
	//	wprintf(L"bind failed with error: %u\n", WSAGetLastError());
	//	closesocket(ListenSocket);
	//	WSACleanup();
	//	return 1;
	//}

	////----------------------------------------
	//// Start listening on the listening socket
	//iResult = listen(ListenSocket, 100);
	//if (iResult == SOCKET_ERROR) {
	//	wprintf(L"listen failed with error: %u\n", WSAGetLastError());
	//	closesocket(ListenSocket);
	//	WSACleanup();
	//	return 1;
	//}

	//wprintf(L"Listening on address: %s:%d\n", ip, port);

	//// Load the AcceptEx function into memory using WSAIoctl.
	//// The WSAIoctl function is an extension of the ioctlsocket()
	//// function that can use overlapped I/O. The function's 3rd
	//// through 6th parameters are input and output buffers where
	//// we pass the pointer to our AcceptEx function. This is used
	//// so that we can call the AcceptEx function directly, rather
	//// than refer to the Mswsock.lib library.
	//iResult = WSAIoctl(ListenSocket, SIO_GET_EXTENSION_FUNCTION_POINTER,
	//	&GuidAcceptEx, sizeof(GuidAcceptEx),
	//	&lpfnAcceptEx, sizeof(lpfnAcceptEx),
	//	&dwBytes, NULL, NULL);
	//if (iResult == SOCKET_ERROR) {
	//	wprintf(L"WSAIoctl failed with error: %u\n", WSAGetLastError());
	//	closesocket(ListenSocket);
	//	WSACleanup();
	//	return 1;
	//}

	//// Create an accepting socket
	//AcceptSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	//if (AcceptSocket == INVALID_SOCKET) {
	//	wprintf(L"Create accept socket failed with error: %u\n", WSAGetLastError());
	//	closesocket(ListenSocket);
	//	WSACleanup();
	//	return 1;
	//}

	//// Empty our overlapped structure and accept connections.
	//memset(&olOverlap, 0, sizeof(olOverlap));

	//bRetVal = lpfnAcceptEx(ListenSocket, AcceptSocket, lpOutputBuf,
	//	outBufLen - ((sizeof(sockaddr_in) + 16) * 2),
	//	sizeof(sockaddr_in) + 16, sizeof(sockaddr_in) + 16,
	//	&dwBytes, &olOverlap);
	//if (bRetVal == FALSE) {
	//	wprintf(L"AcceptEx failed with error: %u\n", WSAGetLastError());
	//	closesocket(AcceptSocket);
	//	closesocket(ListenSocket);
	//	WSACleanup();
	//	return 1;
	//}

	//// Associate the accept socket with the completion port
	//hCompPort2 = CreateIoCompletionPort((HANDLE)AcceptSocket, hCompPort, (u_long)0, 0);
	//// hCompPort2 should be hCompPort if this succeeds
	//if (hCompPort2 == NULL) {
	//	wprintf(L"CreateIoCompletionPort associate failed with error: %u\n",
	//		GetLastError());
	//	closesocket(AcceptSocket);
	//	closesocket(ListenSocket);
	//	WSACleanup();
	//	return 1;
	//}

	//// Continue on to use send, recv, TransmitFile(), etc.,.
	////...

	return 0;
}