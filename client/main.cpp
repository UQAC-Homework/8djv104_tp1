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

	while (true)
	{
		char buffer[1024];
		std::cin.getline(buffer, sizeof(buffer));

		if (strcmp(buffer, "/quit") == 0 || strcmp(buffer, "/exit") == 0)
		{
			constexpr auto exit_message = "Bye bye";
			send(clientSocket, exit_message, strlen(exit_message), 0);
			break;
		}
		
		send(clientSocket, buffer, sizeof(buffer), 0);
	}

	close(clientSocket);

	return 0;
}
