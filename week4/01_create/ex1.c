//
// Modification of variable in two different processes.
//
// (c) Alexei Guirik, 2021
// This source is licensed under CC BY-NC 4.0
// (https://creativecommons.org/licenses/by-nc/4.0/)
//

#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int variable = 0;

int main(int argc, char *argv[]) {
	pid_t cpid;
	char buf;

	cpid = fork();
	if (cpid == -1) {
	   perror("fork");
	   exit(EXIT_FAILURE);
	}

	if (cpid == 0) {
		// child
		sleep(10);
		variable = 1;
		_exit(EXIT_SUCCESS);
	} else {
		sleep(12);
		wait(NULL);
		printf("variable = %d\n", variable);		   
		exit(EXIT_SUCCESS);
	}
}
