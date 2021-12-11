//
// Sending realtime signal with given signal argument (int value).
//
// (c) Alexei Guirik, 2021
// This source is licensed under CC BY-NC 4.0
// (https://creativecommons.org/licenses/by-nc/4.0/)
//

#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>


int main(int argc, char* argv[]) {
    if (argc != 4) {
        printf("Usage: %s PID SIGNUM NUM\n", argv[0]);
        exit(1);
    }
    pid_t pid = atoi(argv[1]);
    int sig = atoi(argv[2]);
    int num = atoi(argv[3]);
    
    if (sig != 0 && pid != 0) {
        union sigval si;
        si.sival_int = num;
        int res = sigqueue(pid, sig, si);
        if (res < 0)
            perror("ERROR: sigqueue()");
    }
    else {
        printf("ERROR: sig = %d, pid = %d\n", sig, pid);
    }
    
	return 0;
}
