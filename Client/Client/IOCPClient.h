#include "../Header.h"
#include <signal.h>
#include <vector>
#include <map>
static struct bufferevent* m_bev;
static std::vector<bufferevent*> v_bev;
class IOCPClient
{
public:
	int Run();
private:
	 
};
