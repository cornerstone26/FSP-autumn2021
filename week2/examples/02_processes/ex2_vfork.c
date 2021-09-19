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


int main(int argc, char **argv) {                                         

    int a = 5, b = 8;                                                     
    pid_t pid;

    pid = fork();   // change to vfork
    if(pid == 0) {
        a = a + 5;                                                        
        b = b + 2;                                                        
        _exit(0);                                                          
    }                                                                     
                                                        
    wait(NULL);                                                           
    printf("pid = %d\n", pid);
    printf("a + b = %d\n", a + b);
    exit(0);
}
