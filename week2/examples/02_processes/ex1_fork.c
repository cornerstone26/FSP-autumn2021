//
// Fork new process.
//
// (c) Alexei Guirik, 2021
// This source is licensed under CC BY-NC 4.0
// (https://creativecommons.org/licenses/by-nc/4.0/)
//

#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <stdio.h>

int main(int argc, char* argv[]) {
	pid_t pid;
	
	pid = fork();
	
	if (pid < 0) {
		fprintf(stderr, "ERROR: failed to fork()\n");
	}
	else if (pid == 0) {
	    //sleep(20);
		execlp("ls", "ls", "-lah", argv[1], NULL);
		fprintf(stderr, "ERROR: failed to exec()\n");		
	}
	else {
	    //sleep(20);
	    int status;
		while (pid != wait(&status));
		if (WIFEXITED(status) && WEXITSTATUS(status) != 0) {
			fprintf(stderr, "ERROR: status = %x\n", status);
		}
	}
	
	return 0;
}
