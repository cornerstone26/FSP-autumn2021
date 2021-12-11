//
// Smple socket server (for use with ex1c client).
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
#include <time.h>
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
#define MAX_MSG_LEN		10
#define DEFAULT_SET		"0123456789abcdefghijklmnopqrstuvwxyz"

int main() {
	int serverSocket, clientSocket;
	char buffer[BUF_SIZE];
	struct sockaddr_in serverAddr = {0};
	struct sockaddr_storage serverStorage;
	socklen_t addr_size;
	int res;
	
	srand(time(NULL));
	
	serverSocket = socket(AF_INET, SOCK_STREAM, 0);
	CHECK_RESULT(serverSocket, "socket");

	res = setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &(int){ 1 }, sizeof(int));
	CHECK_RESULT(res, "setsockopt");

	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(5555);
	serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

	res = bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr));
	CHECK_RESULT(res, "bind");

	res = listen(serverSocket, 5);
	CHECK_RESULT(res, "listen");

	addr_size = sizeof(serverStorage);
	clientSocket = accept(serverSocket, (struct sockaddr *)&serverStorage, &addr_size);
	CHECK_RESULT(clientSocket, "accept");

	sleep(2);
	
	int msg_len = 1 + rand() % MAX_MSG_LEN;
	
	for (int i = 0; i < msg_len; i++)
		buffer[i] = DEFAULT_SET[ rand() % strlen(DEFAULT_SET) ];

	//res = send(clientSocket, buffer, msg_len, 0);
	res = write(clientSocket, buffer, msg_len);
	CHECK_RESULT(res, "send");

	close(clientSocket);
	close(serverSocket);

	return 0;
}






//	res = setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &(int){ 1 }, sizeof(int));
//	CHECK_RESULT(res, "setsockopt");
