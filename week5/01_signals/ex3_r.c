//
// Receiving realtime signal and checking signal argument (int value).
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


const int secret_number = 777;
int time_to_go = 0;

static void SIG_handler(int signum, siginfo_t *s, void *c) {
    if (s->si_value.sival_int == secret_number) {
        time_to_go = 1;
    }
    else {
        char *msg = "Not now!\n";
	    write(1, msg, strlen(msg));
	}
}

void eat() { sleep(5); }

int main() {
	struct sigaction sa = {0};
	sigemptyset(&sa.sa_mask);
	sa.sa_sigaction = SIG_handler;
	sa.sa_flags |= (SA_SIGINFO | SA_RESTART);
	if (sigaction(SIGRTMAX, &sa, NULL) == -1) {
	    perror("sigaction");
	    exit(1);
	}

    printf("pid = %ld\n", (long)getpid());
    while(!time_to_go) {
        printf("Time to eat...\n");        
        eat();
        printf("Time to sleep...\n");
        sleep(5);
    }

    printf("Ok, bye...\n");        	
	return 0;
}
