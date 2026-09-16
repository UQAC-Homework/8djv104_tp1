#include <iostream>

#if defined(_WIN32)

#else
#include <unistd.h>
#include <netinet/in.h>
#endif

int main()
{
	const auto serverSocket = socket(AF_INET, SOCK_STREAM, 0);

	sockaddr_in serverAddress{};
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons(8080);
	serverAddress.sin_addr.s_addr = INADDR_ANY;

	const auto _ = bind(
		serverSocket,
		reinterpret_cast<sockaddr*>(&serverAddress),
		sizeof(serverAddress)
	);

	const auto success = listen(serverSocket, 5);

	if (success == -1)
	{
		std::cerr << "Failed to accept connections on socket." << std::endl;
		return 1;
	}

	const auto clientSocket = accept(serverSocket, nullptr, nullptr);
	
	char buffer[1024] = {};
	recv(clientSocket, buffer, sizeof(buffer), 0);
	std::cout << "Message from client: " << buffer << std::endl;

	close(serverSocket);

	return 0;
}
