/*** EventTimerServer */

#include "../Header.h"
#include <stdlib.h>  
#include <stdio.h>  
#include <event2/event.h>
#include <event2/event_compat.h>
#include <event2/event_struct.h>

class EventTimerServer
{
public:
	void Run();
};


/*** EventHelloServer */


#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <signal.h>
#ifndef WIN32
#include <netinet/in.h>
# ifdef _XOPEN_SOURCE_EXTENDED
#  include <arpa/inet.h>
# endif
#include <sys/socket.h>
#endif

#include <event2/bufferevent.h>
#include <event2/buffer.h>
#include <event2/listener.h>
#include <event2/util.h>
#include <event2/event.h>

static const char MESSAGE[] = "Hello, World!\n";

static const int PORT = 9995;

static void listener_cb(struct evconnlistener *, evutil_socket_t,struct sockaddr *, int socklen, void *);
static void conn_writecb(struct bufferevent *, void *);
static void conn_eventcb(struct bufferevent *, short, void *);
static void signal_cb(evutil_socket_t, short, void *);

class EventHelloServer
{
public:
	int Run();
};


/*** EventIOCPServer */
#include "../Header.h"
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <signal.h>
#ifndef WIN32
#include <netinet/in.h>
# ifdef _XOPEN_SOURCE_EXTENDED
#  include <arpa/inet.h>
# endif
#include <sys/socket.h>
#endif

#include <event2/bufferevent.h>
#include <event2/buffer.h>
#include <event2/listener.h>
#include <event2/util.h>
#include <event2/event.h>
#include <event2/thread.h>

#include <iostream>
#include <map>
#include <thread>

static std::map<bufferevent*, sockaddr_in> map_Client;


#define MAX_TO_READ_EVER 16384

#define MAX_TO_WRITE_EVER 16384
static int run = 0; 
class EventIOCPServer
{
public:
	EventIOCPServer();
	bool Init(int port);
	virtual void Run();
	virtual ~EventIOCPServer(); 

	virtual void loop();
private:


	struct event_base *base;
	struct evconnlistener *listener;
	struct event *signal_event;
	std::thread *  dis;
};