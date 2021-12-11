//
// Smple socket client (for use with ex1s server).
//
// (c) Alexei Guirik, 2021
// This source is licensed under CC BY-NC 4.0
// (https://creativecommons.org/licenses/by-nc/4.0/)
//

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define CHECK_RESULT(res, msg)			\
do {									\
	if (res < 0) {						\
		perror(msg);					\
		exit(EXIT_FAILURE);				\
	}									\
} while (0)

#define BUF_SIZE		1024

int main() {
	int clientSocket;
	char buffer[BUF_SIZE] = {0};
	struct sockaddr_in serverAddr = {0};
	socklen_t addr_size;

	clientSocket = socket(AF_INET, SOCK_STREAM, 0);
	CHECK_RESULT(clientSocket, "socket");
	
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(5555);
	serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

	int res = connect(clientSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr));
	CHECK_RESULT(res, "connect");
	
	res = recv(clientSocket, buffer, BUF_SIZE, 0);
	//res = read(clientSocket, buffer, BUF_SIZE);
	CHECK_RESULT(res, "recv");
	
	printf("Data received (%d bytes): [%s]\n", (int)res, buffer);   

	close(clientSocket);
	
	return 0;
}
