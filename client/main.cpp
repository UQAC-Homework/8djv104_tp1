#include <cstring>
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

static void receiveIncomingMessages(
	const SOCKET socket,
	const sockaddr_in address,
	const std::atomic<bool>* terminate
)
{
	sockaddr_in server_address = address;
	socklen_t server_address_length = sizeof(server_address);

	char buffer[1024];

	while (terminate != nullptr && !terminate->load(std::memory_order_relaxed))
	{
		// Read from socket
		const auto bytes_count = recvfrom(
			socket,
			buffer,
			sizeof(buffer),
			0,
			reinterpret_cast<sockaddr*>(&server_address),
			&server_address_length
		);

		if (bytes_count == -1)
		{
			// If terminated, ignore
			if (terminate != nullptr && terminate->load(std::memory_order_relaxed))
				break;

			std::cerr << "An error occurred while reading from socket." << std::endl;
			continue;
		}

		buffer[bytes_count] = '\0';

		// Print message
		std::cout << buffer << std::endl;
	}
}

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

	std::atomic terminate = false;
	std::thread receiveMessage(receiveIncomingMessages, socket, server_address, &terminate);

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

		if (strcmp(buffer, "/quit") == 0 || strcmp(buffer, "/exit") == 0)
			break;
	}

	// Terminate and wait for thread
	terminate.store(true, std::memory_order_relaxed);
	shutdown(socket, SHUT_RDWR);
	receiveMessage.join();

	close(socket);

	return 0;
}
