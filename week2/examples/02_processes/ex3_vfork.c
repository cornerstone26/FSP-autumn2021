//
// Compare fork() and vfork().
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

#define MAX_ALLOCS      1024L * 1024L

// Change ALLOC_SIZE to 1024
#define ALLOC_SIZE      2 * 1024

int main(int argc, char* argv[]) {

    for (int i = 0; i < MAX_ALLOCS; i++) {
        void *p = malloc(ALLOC_SIZE);
        if (p != NULL) {
            *(int*)p = rand();
        }
        else {
            fprintf(stderr, "ERROR: malloc failed at %d iteration\n", i);
            break;
        }
    }
    
    fprintf(stderr, "Allocated %ld bytes in heap\n", MAX_ALLOCS * ALLOC_SIZE);
    
    sleep(10);

    fprintf(stderr, "Now trying to fork...\n");
        
	pid_t pid;
	int status;

    // Use fork() or vfork()	
	pid = fork();
	if (pid < 0) {
		fprintf(stderr, "ERROR: failed to fork()\n");
	}
	else if (pid == 0) {
		execlp("ls", "ls", "-lah", argv[1], NULL);
		fprintf(stderr, "ERROR: failed to exec()\n");
	}
	else {
		while (pid != wait(&status));
		if (WIFEXITED(status) && WEXITSTATUS(status) != 0) {
			fprintf(stderr, "ERROR: status = %x\n", status);
		}
	}
	return 0;
}

