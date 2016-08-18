#include "../Header.h"
#include <signal.h>
#include <vector>
#include <map>

#define MAX_TO_READ_EVER 16384

#define MAX_TO_WRITE_EVER 16384

static struct bufferevent* m_bev;
static std::vector<bufferevent*> v_bev;
class IOCPClient
{
public:
	int Run();
private:
	 
};
