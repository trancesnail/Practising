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
	char MESSAGE[256] = "我连上来了！";
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

	//	1.创建socket和服务器地址
	// 加载socket动态链接库(dll)  
	WORD wVersionRequested;
	WSADATA wsaData;    // 这结构是用于接收Windows Socket的结构信息的  
	int err;

	wVersionRequested = MAKEWORD(2, 2);   // 请求1.1版本的WinSock库  

	err = WSAStartup(wVersionRequested, &wsaData);
	if (err != 0) {
		return INVALID_SOCKET;          // 返回值为零的时候是表示成功申请WSAStartup  
	}

	// 创建socket操作，建立流式套接字，返回套接字号sockClient  
	// SOCKET socket(int af, int type, int protocol);  
	// 第一个参数，指定地址簇(TCP/IP只能是AF_INET，也可写成PF_INET)  
	// 第二个，选择套接字的类型(流式套接字)，第三个，特定地址家族相关协议（0为自动）  
	SOCKET sockClient = socket(AF_INET, SOCK_STREAM, 0);

	//服务器地址
	SOCKADDR_IN addrSrv;
	addrSrv.sin_family = AF_INET;
	addrSrv.sin_port = htons(9995);
	inet_pton(AF_INET, "127.0.0.1", &addrSrv.sin_addr);      // 本地回路地址是127.0.0.1; 

	//	2.LibEvent 部分
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
	//event_config_require_features(cfg, event_method_feature::EV_FEATURE_ET);			//设置后eventops被修改 base生成失败

	evthread_use_windows_threads();				//如果要使用多线程，需要线程安全，那么在调用event_base_new函数之前一定要调用该函数(对应的Windows版本为evthread_use_windows_threads)。如果在event_base_new之后才调用evthread_use_pthreads，那么该event_base就不会是线程安全的了。
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
	//////监听终端输入事件
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
	
	// 加载socket动态链接库(dll)  
	WORD wVersionRequested;
	WSADATA wsaData;    // 这结构是用于接收Windows Socket的结构信息的  
	int err;

	wVersionRequested = MAKEWORD(2, 2);   // 请求1.1版本的WinSock库  

	err = WSAStartup(wVersionRequested, &wsaData);
	if (err != 0) {
		return INVALID_SOCKET;          // 返回值为零的时候是表示成功申请WSAStartup  
	}

	// 创建socket操作，建立流式套接字，返回套接字号sockClient  
	// SOCKET socket(int af, int type, int protocol);  
	// 第一个参数，指定地址簇(TCP/IP只能是AF_INET，也可写成PF_INET)  
	// 第二个，选择套接字的类型(流式套接字)，第三个，特定地址家族相关协议（0为自动）  
	SOCKET sockClient = socket(AF_INET, SOCK_STREAM, 0);

	// 将套接字sockClient与远程主机相连  
	// int connect( SOCKET s,  const struct sockaddr* name,  int namelen);  
	// 第一个参数：需要进行连接操作的套接字  
	// 第二个参数：设定所需要连接的地址信息  
	// 第三个参数：地址的长度  
	SOCKADDR_IN addrSrv;
	addrSrv.sin_family = AF_INET;
	addrSrv.sin_port = htons(9995);
	inet_pton(AF_INET,"127.0.0.1", &addrSrv.sin_addr);      // 本地回路地址是127.0.0.1;   

	int status = connect(sockClient, (SOCKADDR*)&addrSrv, sizeof(SOCKADDR));

	if (status == -1)
	{
		//save_errno = errno;
		closesocket(sockClient);
		WSACleanup();   // 终止对套接字库的使用  
		//errno = save_errno; //the close may be error
		return INVALID_SOCKET;
	}

	evutil_make_socket_nonblocking(sockClient);
	return sockClient;
}