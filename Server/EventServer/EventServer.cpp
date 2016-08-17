#include "EventServer.h"

/*** EventTimerServer */

void onTime(intptr_t sock, short event, void *arg)
{
	printf("Hello,World!\n");

	struct timeval tv;
	tv.tv_sec = 1;
	tv.tv_usec = 0;
	// 重新添加定时事件（定时事件触发后默认自动删除）   
	event_add((struct event*)arg, &tv);
}

void EventTimerServer::Run()
{
	// 初始化   
	event_init();

	struct event ev_time;
	// 设置定时事件   
	evtimer_set((event*)&ev_time, onTime, (void*)&ev_time);

	struct timeval tv;
	tv.tv_sec = 1;
	tv.tv_usec = 0;
	// 添加定时事件   
	event_add((event*)&ev_time, &tv);

	// 事件循环   
	event_dispatch();
}

/*** EventHelloServer */
int EventHelloServer::Run()
{
	struct event_base *base;
	struct evconnlistener *listener;
	struct event *signal_event;

	struct sockaddr_in sin;
#ifdef WIN32
	WSADATA wsa_data;
	WSAStartup(0x0201, &wsa_data);
#endif

	base = event_base_new();
	if (!base) {
		fprintf(stderr, "Could not initialize libevent!\n");
		return 1;
	}

	memset(&sin, 0, sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_port = htons(PORT);

	listener = evconnlistener_new_bind(base, listener_cb, (void *)base,
		LEV_OPT_REUSEABLE | LEV_OPT_CLOSE_ON_FREE, -1,
		(struct sockaddr*)&sin,
		sizeof(sin));

	if (!listener) {
		fprintf(stderr, "Could not create a listener!\n");
		return 1;
	}

	signal_event = evsignal_new(base, SIGINT, signal_cb, (void *)base);

	if (!signal_event || event_add(signal_event, NULL) < 0) {
		fprintf(stderr, "Could not create/add a signal event!\n");
		return 1;
	}

	event_base_dispatch(base);

	evconnlistener_free(listener);
	event_free(signal_event);
	event_base_free(base);

	printf("done\n");
	return 0;
}




static void listener_cb(struct evconnlistener *listener, evutil_socket_t fd,struct sockaddr *sa, int socklen, void *user_data)
{
	struct event_base *base = (event_base *)user_data;
	struct bufferevent *bev;

	bev = bufferevent_socket_new(base, fd, BEV_OPT_CLOSE_ON_FREE);
	if (!bev) {
		fprintf(stderr, "Error constructing bufferevent!");
		event_base_loopbreak(base);
		return;
	}
	bufferevent_setcb(bev, NULL, conn_writecb, conn_eventcb, NULL);
	bufferevent_enable(bev, EV_WRITE);
	bufferevent_disable(bev, EV_READ);

	bufferevent_write(bev, MESSAGE, strlen(MESSAGE));
}

static void conn_writecb(struct bufferevent *bev, void *user_data)
{
	struct evbuffer *output = bufferevent_get_output(bev);
	if (evbuffer_get_length(output) == 0) {
		printf("flushed answer\n");
		bufferevent_free(bev);
	}
}

static void conn_eventcb(struct bufferevent *bev, short events, void *user_data)
{
	if (events & BEV_EVENT_EOF) {
		printf("Connection closed.\n");
	}
	else if (events & BEV_EVENT_ERROR) {
		printf("Got an error on the connection: %s\n",
			strerror(errno));/*XXX win32*/
	}
	/* None of the other events can happen here, since we haven't enabled
	* timeouts */
	bufferevent_free(bev);
}

static void signal_cb(evutil_socket_t sig, short events, void *user_data)
{
	struct event_base *base = (event_base *) user_data;
	struct timeval delay = { 2, 0 };

	printf("Caught an interrupt signal; exiting cleanly in two seconds.\n");

	event_base_loopexit(base, &delay);
}



/*** EventIOCPServer */


static void iocp_conn_writecb(struct bufferevent *bev, void *user_data)
{
	struct evbuffer *output = bufferevent_get_output(bev);
	if (evbuffer_get_length(output) == 0) {
		//printf("flushed write answer\n");
		//bufferevent_free(bev);
	}
	else
	{
		std::cout <<"iocp_conn_writecb:"<<evbuffer_get_length(output) << std::endl;
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

		size_t bufferevent_read(struct bufferevent *bufev, void *data, size_t size);
		char readbuf[100] = {0};
		size_t len = bufferevent_read(bev, (void *)readbuf,100);
		std::string a ;
		std::map<bufferevent*, sockaddr_in>::const_iterator ItFind = map_Client.find(bev); 
		
		if (ItFind != map_Client.cend())
		{	
			a.append(GetTime());
			//a.append(inet_ntoa(ItFind->second.sin_addr));
			a.append(":");
			char sPort[8] = { 0 };
			sprintf(sPort, "%5d\t", ItFind->second.sin_port);
			a.append(sPort);
		}
		else
		{
			a.append("UNKNOW:IP-");
		}

		a.append(" By Svr Sent:");
		a.append(readbuf);
		a.append("\r\n");
		//printf(a.c_str());

		bufferevent_write(ItFind->first, a.c_str(), a.length() + 1);

		//std::map<bufferevent*, sockaddr_in>::const_iterator ItBegin = map_Client.cbegin();
		//std::map<bufferevent*, sockaddr_in>::const_iterator ItEnd = map_Client.cend();

		//for ( ; ItBegin != ItEnd ; ItBegin ++)
		//{
		//	bufferevent_write(ItBegin->first, a.c_str(), a.length() + 1);
		//}
	}
}


static void iocp_conn_eventcb(struct bufferevent *bev, short events, void *user_data)
{
	if (events & BEV_EVENT_EOF) {
		printf("Connection closed.\n");
	}
	else if (events & BEV_EVENT_ERROR) {
		printf("Got an error on the connection: %s\n",
			strerror(errno));/*XXX win32*/
	}
	/* None of the other events can happen here, since we haven't enabled
	* timeouts */
	map_Client.erase(bev);
	bufferevent_free(bev);
}


static void iocp_listener_cb(struct evconnlistener *listener, evutil_socket_t fd, struct sockaddr *sa, int socklen, void *user_data)
{
	struct event_base *base = (event_base *)user_data;
	struct bufferevent *bev;

	bev = bufferevent_socket_new(base, fd, BEV_OPT_CLOSE_ON_FREE|BEV_OPT_THREADSAFE);
	if (!bev) {
		fprintf(stderr, "Error constructing bufferevent!");
		event_base_loopbreak(base);
		return;
	}
	sockaddr_in * psinTmp = (sockaddr_in*) sa;
	sockaddr_in psin = (*psinTmp);
	map_Client[bev] = psin;

	std::cout << "Connect Form " << inet_ntoa(psin.sin_addr) <<":"<< psin.sin_port << std::endl;
	std::cout << "Clinet Count:" << map_Client.size() << std::endl;
	bufferevent_setcb(bev, iocp_conn_readcb, iocp_conn_writecb,  iocp_conn_eventcb, NULL);
	bufferevent_enable(bev, EV_WRITE);
	bufferevent_enable(bev, EV_READ);

	bufferevent_write(bev, MESSAGE, strlen(MESSAGE));
}




int EventIOCPServer::Run()
{
	struct event_base *base;
	struct evconnlistener *listener;
	struct event *signal_event;

	struct sockaddr_in sin;
#ifdef WIN32
	WSADATA wsa_data;
	WSAStartup(0x0201, &wsa_data);
#endif


	memset(&sin, 0, sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_port = htons(PORT);
	
	event_config * cfg = event_config_new();
	if (cfg == nullptr)
	{
		fprintf(stderr, "Could not initialize event_config!\n");
		return 1;
	}
	SYSTEM_INFO si;

	GetSystemInfo(&si);

	event_config_set_num_cpus_hint(cfg,(int)si.dwNumberOfProcessors);
	event_config_set_flag(cfg,event_base_config_flag::EVENT_BASE_FLAG_STARTUP_IOCP);
	//event_config_require_features(cfg, event_method_feature::EV_FEATURE_ET);			//设置后eventops被修改 base生成失败

	evthread_use_windows_threads();				//如果要使用多线程，需要线程安全，那么在调用event_base_new函数之前一定要调用该函数(对应的Windows版本为evthread_use_windows_threads)。如果在event_base_new之后才调用evthread_use_pthreads，那么该event_base就不会是线程安全的了。
	base = event_base_new_with_config(cfg);
	
	if (!base) {
		fprintf(stderr, "Could not initialize libevent!\n");
		return 1;
	}

	listener = evconnlistener_new_bind(base, iocp_listener_cb, (void *)base,
		LEV_OPT_REUSEABLE | LEV_OPT_CLOSE_ON_FREE, -1,
		(struct sockaddr*)&sin,
		sizeof(sin));

	if (!listener) {
		fprintf(stderr, "Could not create a listener!\n");
		return 1;
	}

	signal_event = evsignal_new(base, SIGINT, signal_cb, (void *)base);

	if (!signal_event || event_add(signal_event, NULL) < 0) {
		fprintf(stderr, "Could not create/add a signal event!\n");
		return 1;
	}

	event_base_dispatch(base);

	evconnlistener_free(listener);
	event_free(signal_event);
	event_base_free(base);

	printf("done\n");

	return 0;
}


