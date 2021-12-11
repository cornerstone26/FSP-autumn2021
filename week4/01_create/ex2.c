//
// Modification of a variable in two different threads.
// To watch for changes:
// watch -n 0.5 'ps -eLo pid,ppid,tid,tgid,comm | grep ex2'
//
// (c) Alexei Guirik, 2021
// This source is licensed under CC BY-NC 4.0
// (https://creativecommons.org/licenses/by-nc/4.0/)
//

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int variable = 0;

void* thread_func(void* arg) {
	sleep(10);
	variable = 1;
}
	
int main(int argc, char *argv[]) {
	pid_t cpid;
	char buf;

	pthread_t tid;
	int res = pthread_create(&tid, NULL, thread_func, NULL);

	if (res != 0) {
		perror("pthread_create");
		exit(EXIT_FAILURE);
	}
	
	sleep(12);
	printf("variable = %d\n", variable);
	
	return 0;
}
