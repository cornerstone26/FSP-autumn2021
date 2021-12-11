//
// Smple chat server (for use with ex3c client). Uses UDP instead of TCP.
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

char *strrev(char *str);

int main() {
	int serverSocket;
	char buffer[BUF_SIZE] = {0};
	struct sockaddr_in serverAddr = {0};
	struct sockaddr_in clientAddr = {0};
	socklen_t addr_size;
	int res;
	
	srand(time(NULL));
	
	serverSocket = socket(AF_INET, SOCK_DGRAM, 0);
	CHECK_RESULT(serverSocket, "socket");

	res = setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &(int){ 1 }, sizeof(int));
	CHECK_RESULT(res, "setsockopt");

	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(5555);
	serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	//serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);

	res = bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr));
	CHECK_RESULT(res, "bind");
	
	int len = sizeof(clientAddr);
	res = recvfrom(serverSocket, buffer, sizeof(buffer), 0, (struct sockaddr*)&clientAddr, &len);
	CHECK_RESULT(res, "recvfrom");
	
	printf("Data received (%d bytes): [%s] from %s:%d\n", (int)res, buffer,
		inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port));
	
	strrev(buffer);
	
	res = sendto(serverSocket, buffer, strlen(buffer), 0, 
		(const struct sockaddr*)&clientAddr, len);
	CHECK_RESULT(res, "sendto");


	close(serverSocket);

	return 0;
}

char *strrev(char *str) {
	char *p1, *p2;
	if (! str || ! *str)
		return str;
	for (p1 = str, p2 = str + strlen(str) - 1; p2 > p1; ++p1, --p2) {
		*p1 ^= *p2;
		*p2 ^= *p1;
		*p1 ^= *p2;
	}
	return str;
}
