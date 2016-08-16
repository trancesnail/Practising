#include "../Header.h"
#include <signal.h>
class IOCPClient
{
public:
	int Run();
private:
	SOCKET InitConServer();
};
