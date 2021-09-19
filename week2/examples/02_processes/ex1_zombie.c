//
// Go full zombie!
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

#define NZ 10

int main(int argc, char* argv[]) {
    pid_t pid;
    
    for (int i = 0; i < NZ; i++) {
	    pid = fork();
        if (pid == 0) {
            fprintf(stderr, "BRRRAAAAINSSSS.... ");
            // Note to self: kill one of sleeping children
            // until they turn into zombie.
            sleep(20);
            exit(0xff);
        }
    }

    getc(stdin);
    
    int status;
    for (int i = 0; i < NZ; i++) {        
	    pid = wait(&status);
	    fprintf(stderr, "status = %x\n", status);
    }
    
	return 0;
}
