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

static void receiveIncomingMessages(const SOCKET socket, sockaddr_in server_address)
{
	char buffer[1024];
	socklen_t server_address_length = sizeof(server_address);

	while (true)
	{
		const auto bytes_read = recvfrom(
			socket,
			buffer,
			sizeof(buffer) - 1,
			0,
			reinterpret_cast<sockaddr*>(&server_address),
			&server_address_length
		);

		if (bytes_read == -1)
		{
			std::cerr << "recvfrom failed" << std::endl;
			break;
		}

		buffer[bytes_read] = '\0';

		std::cout << buffer << std::endl;
	}
}

int main()
{
	//  Create socket
	const SOCKET socket = ::socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	if (socket == INVALID_SOCKET)
	{
		perror("Failed to create socket.");
		return 1;
	}

	sockaddr_in server_address = {};
	server_address.sin_family = AF_INET;
	server_address.sin_addr.s_addr = INADDR_ANY;
	server_address.sin_port = htons(PORT);
	constexpr socklen_t server_address_length = sizeof(server_address);

	// Connect to server
	const auto connect_success = connect(
		socket,
		reinterpret_cast<sockaddr*>(&server_address),
		server_address_length
	);

	if (connect_success == -1)
	{
		perror("Failed to connect to server.");
		return 1;
	}

	std::string message;

	std::cout << "Enter message to send (type 'exit' to quit):\n";

	std::thread receive_thread([socket, &server_address]
	{
		receiveIncomingMessages(socket, server_address);
	});

	while (true)
	{
		std::getline(std::cin, message);

		if (message == "exit")
			break;

		sendto(
			socket,
			message.c_str(),
			message.size(),
			0,
			reinterpret_cast<sockaddr*>(&server_address),
			server_address_length
		);
	}

	return 0;
}
