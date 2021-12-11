//
// Smple chat client (for use with ex2s server).
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
#define MAX_DELAY		2


char*	random_reply(int);
void	random_delay(int);

int main() {
	int clientSocket;
	char buffer[BUF_SIZE] = {0};
	struct sockaddr_in serverAddr = {0};
	socklen_t addr_size;

	srand(time(NULL));
	
	clientSocket = socket(AF_INET, SOCK_STREAM, 0);
	CHECK_RESULT(clientSocket, "socket");
	
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(5555);
	serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

	int res = connect(clientSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr));
	CHECK_RESULT(res, "connect");

	int i = 0;	
	while (1) {
		res = read(clientSocket, buffer, BUF_SIZE);
		CHECK_RESULT(res, "read");
		printf("Server: %s\n", buffer);

		random_delay(MAX_DELAY);
				
		char *reply = random_reply(i++);
		res = write(clientSocket, reply, strlen(reply) + 1);
		CHECK_RESULT(res, "write");
		printf("Client: %s\n", reply);				
		
		random_delay(MAX_DELAY);
	}
	
	close(clientSocket);
	
	return 0;
}

void random_delay(int delay) {
	sleep(1 + rand() % delay);
}

char* random_reply(int i) {
	static char* hi_arr[] = {
		"Hi!", "Hello!", "Wazzup?", "Hey, dude!", "Howdy!"
	};
	static char* chat_arr[] = {
		"Nice weather today!", "You are damn right!", "Any news?", "It's all the same",
		"Yeah...", "You were saying?..", "You sure?", "How is that possible?",
		"No news is great news", "So, what's up?", "That's nice", "Oh, dear...",
		"All is quiet today", "You look awesome!", "I'm feeling a bit under the weather...",
		"Is everything ok?", "I don't think so", "OK", "All is well that ends well",
		"It's typical", "What were you thinking about?", "It's all those damned russian hackers!",
		"Nice of you to ask", "It's nothing", "I don't care", "Oh, come on!",
		"What kind of answer is that?", "You are joking, right?", "Oh, crap!",
		"Come on, I'm joking!", "That was just a joke, stupid", "Better say nothing",
		"Whatever...", "This chat will not end well", "I have bad feeling about this...",
		"Hmm, I see your point", "Now, don't repeat yourself!", "That's strange...",
		"I hope for the best", "We ought to prepare for the worst", "As simple as that",
		"Thank you!", "Stop it!", "I've had enough!", "I really don't know what to say",
		"Good point", "And who are you to judge?", "I think all politicians are liars",
		"I guess nothing comes easy", "Apparently, yes", "That is obvious, is it not?",		
	};

	#define ARR_LEN(arr) (sizeof(arr)/sizeof(arr[0]))	
	if (i == 0) {
		return hi_arr[ rand() % ARR_LEN(hi_arr) ];
	}
	else {
		return chat_arr[ rand() % ARR_LEN(chat_arr) ];
	}
}
