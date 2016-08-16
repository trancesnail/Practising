#include "../Header.h"
#include "IOCPClient.h"
#include <event2/event.h>
#include <event2/event_struct.h>
#include <event2/bufferevent.h>
#include <event2/bufferevent_struct.h>
#include <event2/buffer.h>
#include <event2/util.h>
#include <event2/thread.h>

#include <iostream>
#include <string>


static void timeout_cb(evutil_socket_t fd, short event, void *arg)
{
	bufferevent * bev = (bufferevent *)arg;
	char MESSAGE[256] = "���������ˣ�";
	bufferevent_write(bev, MESSAGE, strlen(MESSAGE));
}

static void iocp_conn_writecb(struct bufferevent *bev, void *user_data)
{
	struct evbuffer *output = bufferevent_get_output(bev);
	if (evbuffer_get_length(output) == 0) {
		printf("flushed write answer\n");
		//bufferevent_free(bev);
	}
	else
	{
		std::cout << "iocp_conn_writecb:" << evbuffer_get_length(output) << std::endl;
	}
}

static void iocp_conn_readcb(struct bufferevent *bev, void *user_data)
{
	struct evbuffer *input = bufferevent_get_input(bev);
	if (evbuffer_get_length(input) == 0) {
		printf("flushed read answer\n");
		//bufferevent_free(bev);
	}
	else
	{
		char readbuf[100] = { 0 };
		size_t len = bufferevent_read(bev, (void *)readbuf, 100);
		std::string a(readbuf);
		std::cout<<a<<std::endl;
		struct event timeout;
		struct timeval tv;
		event_assign(&timeout, bev->ev_base, -1, 0, timeout_cb, (void*)bev);
		evutil_timerclear(&tv);
		tv.tv_sec = 2;
		event_add(&timeout, &tv);
	}
}

static void iocp_conn_eventcb(struct bufferevent *bev, short events, void *user_data)
{
	if (events & BEV_EVENT_EOF) {
		printf("Connection closed.\n");
		char readbuf[100] = { 0 };
		size_t len = bufferevent_read(bev, (void *)readbuf, 100);
		std::string a(readbuf);
		std::cout << a << std::endl;
	}
	else if (events & BEV_EVENT_ERROR) {
		printf("Got an error on the connection: %s\n",
			strerror(errno));/*XXX win32*/
	}
	else if (events & BEV_EVENT_CONNECTED)
	{
		std::cout << "Connect Sucess Back!" << std::endl;
	}
	/* None of the other events can happen here, since we haven't enabled
	* timeouts */
}

static void signal_cb(evutil_socket_t sig, short events, void *user_data)
{
	struct event_base *base = (event_base *)user_data;
	struct timeval delay = { 2, 0 };

	printf("Caught an interrupt signal; exiting cleanly in two seconds.\n");

	event_base_loopexit(base, &delay);
}


int IOCPClient::Run()
{

	//	1.����socket�ͷ�������ַ
	// ����socket��̬���ӿ�(dll)  
	WORD wVersionRequested;
	WSADATA wsaData;    // ��ṹ�����ڽ���Windows Socket�Ľṹ��Ϣ��  
	int err;

	wVersionRequested = MAKEWORD(2, 2);   // ����1.1�汾��WinSock��  

	err = WSAStartup(wVersionRequested, &wsaData);
	if (err != 0) {
		return INVALID_SOCKET;          // ����ֵΪ���ʱ���Ǳ�ʾ�ɹ�����WSAStartup  
	}

	// ����socket������������ʽ�׽��֣������׽��ֺ�sockClient  
	// SOCKET socket(int af, int type, int protocol);  
	// ��һ��������ָ����ַ��(TCP/IPֻ����AF_INET��Ҳ��д��PF_INET)  
	// �ڶ�����ѡ���׽��ֵ�����(��ʽ�׽���)�����������ض���ַ�������Э�飨0Ϊ�Զ���  
	SOCKET sockClient = socket(AF_INET, SOCK_STREAM, 0);

	//��������ַ
	SOCKADDR_IN addrSrv;
	addrSrv.sin_family = AF_INET;
	addrSrv.sin_port = htons(9995);
	inet_pton(AF_INET, "127.0.0.1", &addrSrv.sin_addr);      // ���ػ�·��ַ��127.0.0.1; 

	//	2.LibEvent ����
	//	2.1	event_base
	struct event_base *base;
	event_config * cfg = event_config_new();
	if (cfg == nullptr)
	{
		fprintf(stderr, "Could not initialize event_config!\n");
		return 1;
	}
	SYSTEM_INFO si;

	GetSystemInfo(&si);

	event_config_set_num_cpus_hint(cfg, (int)si.dwNumberOfProcessors);
	event_config_set_flag(cfg, event_base_config_flag::EVENT_BASE_FLAG_STARTUP_IOCP);
	//event_config_require_features(cfg, event_method_feature::EV_FEATURE_ET);			//���ú�eventops���޸� base����ʧ��

	evthread_use_windows_threads();				//���Ҫʹ�ö��̣߳���Ҫ�̰߳�ȫ����ô�ڵ���event_base_new����֮ǰһ��Ҫ���øú���(��Ӧ��Windows�汾Ϊevthread_use_windows_threads)�������event_base_new֮��ŵ���evthread_use_pthreads����ô��event_base�Ͳ������̰߳�ȫ���ˡ�
	base = event_base_new_with_config(cfg);
	//base = event_base_new();
	// 2.1
	std::cout<<"Main()Base"<< base<< std::endl;
	struct bufferevent* bev = bufferevent_socket_new(base, sockClient,
		BEV_OPT_CLOSE_ON_FREE);
	bufferevent_setcb(bev, iocp_conn_readcb, iocp_conn_writecb, iocp_conn_eventcb, NULL);
	bufferevent_enable(bev, EV_READ | EV_PERSIST| EV_WRITE);

	if (!bufferevent_socket_connect(bev, (struct sockaddr*)&addrSrv, sizeof(addrSrv)))
	{
		std::cout<<"Connect Sucess!"<<std::endl;
	}
	else
	{
		std::cout << "Connect Faild!" << std::endl;
	}
	
	//HANDLE STD = GetStdHandle(STD_INPUT_HANDLE);
	//////�����ն������¼�
	//struct event* ev_cmd = event_new(base, /*STDIN_FILENO*/(evutil_socket_t) *STD,
	//	EV_READ | EV_PERSIST | EV_WRITE, iocp_conn_readcb,
	//	(void*)bev);


	//event_add(ev_cmd, NULL);

	struct event *signal_event = evsignal_new(base, SIGINT, signal_cb, (void *)base);

	if (!signal_event || event_add(signal_event, NULL) < 0) {
		fprintf(stderr, "Could not create/add a signal event!\n");
		return 1;
	}

	event_base_dispatch(base);




	//bufferevent_free(bev);
	//event_base_free(base);
	return 0;
}

SOCKET IOCPClient::InitConServer()
{
	
	// ����socket��̬���ӿ�(dll)  
	WORD wVersionRequested;
	WSADATA wsaData;    // ��ṹ�����ڽ���Windows Socket�Ľṹ��Ϣ��  
	int err;

	wVersionRequested = MAKEWORD(2, 2);   // ����1.1�汾��WinSock��  

	err = WSAStartup(wVersionRequested, &wsaData);
	if (err != 0) {
		return INVALID_SOCKET;          // ����ֵΪ���ʱ���Ǳ�ʾ�ɹ�����WSAStartup  
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
	addrSrv.sin_family = AF_INET;
	addrSrv.sin_port = htons(9995);
	inet_pton(AF_INET,"127.0.0.1", &addrSrv.sin_addr);      // ���ػ�·��ַ��127.0.0.1;   

	int status = connect(sockClient, (SOCKADDR*)&addrSrv, sizeof(SOCKADDR));

	if (status == -1)
	{
		//save_errno = errno;
		closesocket(sockClient);
		WSACleanup();   // ��ֹ���׽��ֿ��ʹ��  
		//errno = save_errno; //the close may be error
		return INVALID_SOCKET;
	}

	evutil_make_socket_nonblocking(sockClient);
	return sockClient;
}