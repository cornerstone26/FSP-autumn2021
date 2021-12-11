//
// Proper signal handling with sigaction().
//
// (c) Alexei Guirik, 2021
// This source is licensed under CC BY-NC 4.0
// (https://creativecommons.org/licenses/by-nc/4.0/)
//

#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <errno.h>

#define BUF_SIZE 1024

char* taunt_arr[] = {
	"Not now!", "I'm busy!", "Later", "You still there!", "It's NOT the time",
	"Listen, relax", "You again!", "Get off!", "I'm in the middle of some calibrations",
};

void check(int ret) {
	if (ret < 0) {
		fprintf(stderr, "ERROR: failed to set proc mask: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
}

static void SIG_handler(int signum, siginfo_t *s, void *c) {
	//check(sigaction(SIGINT, &sa, NULL));
	char * taunt = taunt_arr[ rand() % (sizeof(taunt_arr) / sizeof(taunt_arr[0]))];
	write(1, taunt, strnlen(taunt, BUF_SIZE));
	write(1, "\n", 1);
	
}


int main() {
	//signal(SIGINT, SIGINT_handler);
	struct sigaction sa = {0};
	sigemptyset(&sa.sa_mask);
	sigaddset(&sa.sa_mask, SIGINT);
	sigaddset(&sa.sa_mask, SIGQUIT);
	sa.sa_sigaction = SIG_handler;
	sa.sa_flags |= (SA_SIGINFO | SA_RESTART); // mind SA_RESTART here!
	check(sigaction(SIGINT, &sa, NULL));
	check(sigaction(SIGQUIT, &sa, NULL));
	
	srand(time(NULL));
	char buffer[BUF_SIZE] = {0};
	while (fgets(buffer, BUF_SIZE, stdin)) {
		if (strcasecmp(buffer, "quit\n") == 0) break;
	}
	return 0;
}
