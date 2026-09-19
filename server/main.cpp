#include <iostream>
#include <ranges>
#include <unordered_map>

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

	std::unordered_map<int64_t, std::string> clients = {};
	sockaddr_in client_address{};
	socklen_t client_address_length = sizeof(client_address);

	char buffer[1024];

	while (true)
	{
		const auto bytes_count = recvfrom(
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

		int64_t current_identifier = static_cast<int64_t>(client_address.sin_addr.s_addr) << 32 | client_address.
			sin_port;
		const auto current_client = clients.find(current_identifier);

		if (current_client == clients.end())
		{
			auto message = "User '" + std::string(buffer) + "' has joined.";

			clients.insert({current_identifier, buffer});
			sendto(
				server_socket,
				message.c_str(),
				message.length(),
				0,
				reinterpret_cast<sockaddr*>(&client_address),
				client_address_length
			);
			continue;
		}

		auto message = "[" + current_client->second + "]: " + buffer;

		for (const auto& id : clients | std::views::keys)
		{
			if (id == current_identifier)
				continue;

			sendto(
				server_socket,
				message.c_str(),
				message.length(),
				0,
				reinterpret_cast<sockaddr*>(&client_address),
				client_address_length
			);
		}
	}

	return 0;
}
