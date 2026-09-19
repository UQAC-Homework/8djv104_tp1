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
	// Create socket
	const SOCKET socket = ::socket(
		// IPv4 Internet protocols
		AF_INET,

		// Supports datagrams
		SOCK_DGRAM,

		// UDP datagram sockets
		IPPROTO_UDP
	);

	if (socket == INVALID_SOCKET)
	{
		perror("Failed to create a socket.");
		return 1;
	}

	// Connect socket to server
	sockaddr_in server_address{};
	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(9009);
	server_address.sin_addr.s_addr = htonl(INADDR_ANY);

	const auto connect_error_code = connect(
		socket,
		reinterpret_cast<sockaddr*>(&server_address),
		sizeof(server_address)
	);

	if (connect_error_code != 0)
	{
		perror("Failed to connect to server.");
		return 1;
	}

	char buffer[1024];

	while (true)
	{
		// Get user message
		std::cout << "> ";
		std::cin.getline(buffer, sizeof(buffer));

		// Send message to server
		for (size_t i = 0; i < sizeof(buffer);)
		{
			const auto bytes_count = send(
				socket,
				buffer + i,
				sizeof(buffer),
				0
			);

			i += bytes_count;
		}

		//
	}


	return 0;
}
