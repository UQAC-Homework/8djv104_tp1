#include <iostream>
#include <thread>

#if defined(_WIN32)
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")
#else
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
using SOCKET = int;
#define INVALID_SOCKET (-1)
#endif

#define PORT 9999

int main()
{
	return 0;
}
