#include <cstring>
#include <iostream>

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

int main()
{
	SOCKET client_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	if (client_socket == INVALID_SOCKET)
	{
		perror("Failed to create socket.");
		return 1;
	}

	sockaddr_in serverAddr = {};
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = INADDR_ANY; //inet_addr("127.0.0.1");
	serverAddr.sin_port = htons(9999);

	std::string message;
	char buffer[1024];
	socklen_t serverAddrLen = sizeof(serverAddr);

	std::cout << "Enter message to send (type 'exit' to quit):\n";

	while (true)
	{
		std::getline(std::cin, message);

		if (message == "exit")
			break;

		sendto(client_socket, message.c_str(), message.size(), 0, reinterpret_cast<sockaddr*>(&serverAddr), serverAddrLen);

		const auto recvLen = recvfrom(client_socket, buffer, sizeof(buffer) - 1, 0, reinterpret_cast<sockaddr*>(&serverAddr), &serverAddrLen);

		if (recvLen == -1)
		{
			std::cerr << "recvfrom failed" << std::endl;
			break;
		}
		buffer[recvLen] = '\0';
		std::cout << "Received from server: " << buffer << std::endl;
	}

	return 0;
}
