#include <iostream>

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

int main(const int argc, char* argv[])
{
	const SOCKET server_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	if (server_socket == INVALID_SOCKET)
	{
		perror("Failed to create a socket.");
		return 1;
	}

	sockaddr_in server_address = {};
	server_address.sin_family = AF_INET;
	server_address.sin_addr.s_addr = INADDR_ANY;
	server_address.sin_port = htons(PORT);

	const auto bind_code = bind(
		server_socket,
		reinterpret_cast<sockaddr*>(&server_address),
		sizeof(server_address)
	);

	if (bind_code == -1)
	{
		perror("Failed to bind the socket.");
		return 1;
	}

	std::cout << "UDP server listening on port " << PORT << "..." << std::endl;

	sockaddr_in client_address{};
	socklen_t client_address_length = sizeof(client_address);

	char buffer[1024];

	while (true)
	{
		const ssize_t bytes_count = recvfrom(
			server_socket,
			buffer,
			sizeof(buffer) - 1,
			0,
			reinterpret_cast<sockaddr*>(&client_address),
			&client_address_length
		);

		if (bytes_count == -1)
		{
			perror("Failed to receive from the socket.");
			break;
		}

		buffer[bytes_count] = '\0';
		std::cout << "Received: " << buffer << std::endl;

		// Echo back
		sendto(server_socket, buffer, bytes_count, 0, reinterpret_cast<sockaddr*>(&client_address), client_address_length);
	}

	return 0;
}
