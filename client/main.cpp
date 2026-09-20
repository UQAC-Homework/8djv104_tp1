#include <atomic>
#include <cstring>
#include <iostream>
#include <thread>

#if defined(_WIN32)
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")
using SOCKET_LENGTH = int;
#else
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
using SOCKET = int;
using SOCKET_LENGTH = socklen_t;
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
	SOCKET_LENGTH server_address_length = sizeof(server_address);

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

			perror("An error occurred while reading from socket.");
			continue;
		}

		if (bytes_count < sizeof(buffer))
			buffer[bytes_count] = '\0';

		// Print message
		std::cout << buffer << std::endl;
	}
}

int main(const int argc, char* argv[])
{
	std::string username = "Player";
	std::string server_ip = "127.0.0.1";
	size_t server_port = 9999;

	if (argc > 1)
		username = argv[1];

	if (argc > 2)
		server_ip = argv[2];

	if (argc > 3)
		server_port = strtol(argv[3], nullptr, 10);

#if defined(_WIN32)
	WSADATA wsaData;

	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		perror("WSAStartup failed.");
		return 1;
	}
#endif

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
	server_address.sin_port = htons(server_port);

#if defined(_WIN32)
	server_address.sin_addr.s_addr = inet_addr(server_ip.c_str());
#else
	inet_pton(AF_INET, server_ip.c_str(), &server_address.sin_addr.s_addr);
#endif

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

	// Send username as first message
	send(
		socket,
		username.c_str(),
		username.length(),
		0
	);

	std::atomic terminate = false;
	std::thread receiveMessage(receiveIncomingMessages, socket, server_address, &terminate);

	char buffer[1024];

	while (true)
	{
		// Get user message
		std::cout << "> ";
		std::cin.getline(buffer, sizeof(buffer));

		if (std::cin.fail())
		{
			perror("Failed to read from user.");
			break;
		}

		// Send message to server
		send(
			socket,
			buffer,
			sizeof(buffer),
			0
		);

		if (strcmp(buffer, "/quit") == 0 || strcmp(buffer, "/exit") == 0)
			break;
	}

	// Close socket
#if defined(_WIN32)
	shutdown(socket, SD_BOTH);
	closesocket(socket);
	WSACleanup();
#else
	shutdown(socket, SHUT_RDWR);
	close(socket);
#endif

	// Terminate and wait for thread
	terminate.store(true, std::memory_order_relaxed);
	receiveMessage.join();

	return 0;
}
