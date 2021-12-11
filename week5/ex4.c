//
// Server that uses select(). Based on:
// https://gist.github.com/droxer/908d01ae5d7253cb2762
//
// Use nc as client to connect to server:
// nc 127.0.0.1 5555
//
// (c) Alexei Guirik, 2021
// This source is licensed under CC BY-NC 4.0
// (https://creativecommons.org/licenses/by-nc/4.0/)
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/select.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

#define CHECK_RESULT(res, msg)			\
do {									\
	if (res < 0) {						\
		perror(msg);					\
		exit(EXIT_FAILURE);				\
	}									\
} while (0)

#define BUF_SIZE		1024 
#define PORT			"5555"
 

void die(const char*);

 
int main(int argc, char *argv[]) {
        int sockfd, new, maxfd, on = 1, nready, i;
 
        struct addrinfo *res0, *res, servAddr = {0};
 
        char buffer[BUF_SIZE];
 
        fd_set master, readfds;
 
        ssize_t nbytes;
 
        servAddr.ai_family = AF_INET;
        servAddr.ai_socktype = SOCK_STREAM;
        servAddr.ai_protocol = IPPROTO_TCP;
        servAddr.ai_flags = AI_PASSIVE;
 
        if(-1 == (getaddrinfo(NULL, PORT, &servAddr, &res0)))
                die("getaddrinfo()");
 
        for(res = res0; res; res = res->ai_next) {
                if(-1 == (sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol))) {
                        perror("socket()");
                        continue;
                }
 
                if(-1 == (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (char*)&on, sizeof(int)))) {
                        perror("setsockopt()");
                        continue;
                }
 
                if(-1 == (bind(sockfd, res->ai_addr, res->ai_addrlen))) {
                        perror("bind");
                        continue;
                }
 
                break;
        }
 
        if(-1 == sockfd)
                exit(EXIT_FAILURE);
 
        freeaddrinfo(res0);
 
        if(-1 == (listen(sockfd, 32)))
                die("listen()");
 
        //if(-1 == (fcntl(sockfd, F_SETFD, O_NONBLOCK)))
        //        die("fcntl()");
 
        FD_ZERO(&master);
        FD_ZERO(&readfds);
 
        FD_SET(sockfd, &master);
 
        maxfd = sockfd;
 
        for(;;) {
                memcpy(&readfds, &master, sizeof(master));
 
                printf("Sleeping in select()\n");
 
                if(-1 == (nready = select(maxfd + 1, &readfds, NULL, NULL, NULL)))
                        die("select()");
 
                printf("Number of ready descriptors: %d\n", nready);
 
                for(i = 0; i <= maxfd && nready > 0; i++) {
                        if(FD_ISSET(i, &readfds)) {
                                nready--;
                                if(i == sockfd) {
                                        printf("Trying to accept() new connection(s)\n");
 
                                        if(-1 == (new = accept(sockfd, NULL, NULL))) {
                                                if(EWOULDBLOCK != errno)
                                                        die("accept()");
                                                break;
                                        }
                                        else {
 
                                                //if(-1 == (fcntl(new, F_SETFD, O_NONBLOCK)))
                                                //        die("fcntl()");
                                                FD_SET(new, &master);
                                                if(maxfd < new)
                                                        maxfd = new;
                                        }
                                }
                                else {
                                        printf("Received data from descriptor [%d]\n", i);
 
                                        nbytes = recv(i, buffer, sizeof(buffer), 0);
                                        buffer[nbytes] = '\0';
                                        printf("{%s}", buffer);
 
                                        printf("%zi bytes received\n", nbytes);
 
                                        if (nbytes <= 0) {
                                                if(EWOULDBLOCK != errno)
                                                        die("recv()");
                                                break;
                                        }

                                        close(i);
                                        FD_CLR(i, &master); 
                                }
                        }
                }
        }

        return 0;
}
 
void die(const char *msg) {
        perror(msg);
        exit(EXIT_FAILURE);
}
