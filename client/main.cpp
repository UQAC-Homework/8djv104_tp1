#include <cstring>
#include <iostream>

#if defined(_WIN32)
#else
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#endif

int main()
{
	const auto clientSocket = socket(AF_INET, SOCK_STREAM, 0);

	sockaddr_in serverAddress{};
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons(8080);
	serverAddress.sin_addr.s_addr = INADDR_ANY;

	const auto _ = connect(
		clientSocket,
		reinterpret_cast<sockaddr*>(&serverAddress),
		sizeof(serverAddress)
	);

	constexpr auto message = "Hello, server!";
	send(clientSocket, message, strlen(message), 0);

	close(clientSocket);

	std::cout << "Hello, World! From clients" << std::endl;
	return 0;
}
