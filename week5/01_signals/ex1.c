//
// Handling SIGINT using signal(). signal() is deprecated!
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

#define BUF_SIZE 1024

char* taunt_arr[] = {
	"Not now!", "I'm busy!", "Later", "You still there!", "It's NOT the time",
	"Listen, relax", "You again!", "Get off!", "I'm in the middle of some calibrations",
};

void SIGINT_handler(int signum) {
	fprintf(stdout, "%s\n", taunt_arr[ rand() % (sizeof(taunt_arr) / sizeof(taunt_arr[0]))]);
}

int main() {
	signal(SIGINT, SIGINT_handler);
	srand(time(NULL));
	char buffer[BUF_SIZE] = {0};
	while (fgets(buffer, BUF_SIZE, stdin)) {
		if (strcasecmp(buffer, "quit\n") == 0) break;
	}
	return 0;
}
