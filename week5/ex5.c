//
// An answer to a long pending question.
//
// (c) Alexei Guirik, 2021
// This source is licensed under CC BY-NC 4.0
// (https://creativecommons.org/licenses/by-nc/4.0/)
//

#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
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
	fd_set rfds;
	struct timeval tv;
	int res;
	char buffer[BUF_SIZE] = {0};
	
	fprintf(stdout, "Enter yes or no (default - yes): ");
	fflush(stdout);
	
	
	FD_ZERO(&rfds);
	FD_SET(0, &rfds);

	tv.tv_sec = 5;
	tv.tv_usec = 0;

	res = select(1, &rfds, NULL, NULL, &tv);

	if (res == -1)
	   CHECK_RESULT(res, "select");
	else if (res && FD_ISSET(0, &rfds)) {
		res = read(0, buffer, sizeof(buffer));
		if (strlen(buffer))
			buffer[strlen(buffer) - 1] = '\0';
		if (strcasecmp(buffer, "yes") == 0)
			printf("\nYou agreed\n");
		else if (strcasecmp(buffer, "no") == 0)
			printf("\nYou disagreed\n");
		else
			printf("\nWill treat '%s' as an agreement\n", buffer);
	}
	else
	   printf("\n\nNo choice was made, but it's 'yes' by default\n");

	exit(EXIT_SUCCESS);
}
