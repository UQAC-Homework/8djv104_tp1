#include <iostream>
#include <ranges>

#if defined(_WIN32)
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")
#else
#include <unistd.h>
#include <netinet/in.h>
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

	sockaddr_in server_address{};
	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(9009);
	server_address.sin_addr.s_addr = htonl(INADDR_ANY);

	// Bind socket to address
	const auto bind_error_code = bind(
		socket,
		reinterpret_cast<struct sockaddr*>(&server_address),
		sizeof(server_address)
	);

	if (bind_error_code != 0)
	{
		perror("Failed to bind the socket to an address.");
		return 1;
	}

	char buffer[1024];

	while (true)
	{
		const auto bytes_count = recvfrom(
			socket,
			buffer,
			sizeof(buffer),
			0,
			nullptr,
			nullptr
		);

		if (bytes_count == -1)
		{
			perror("Failed to read from socket.");
			return 1;
		}

		buffer[bytes_count] = '\0';

		std::cout << buffer << std::endl;
	}

	return 0;
}
