//
// Creating, cancelling and joining threads.
//
// (c) Alexei Guirik, 2021
// This source is licensed under CC BY-NC 4.0
// (https://creativecommons.org/licenses/by-nc/4.0/)
//

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void cleanup_func(void *arg) {
	printf("clean up!\n");
}

void* thread_func(void* arg) {
	pthread_cleanup_push(cleanup_func, arg);
	printf("child thread: entered, waiting...\n");
	while(1) {
		pthread_testcancel();
		sleep(1);
	}
	printf("child thread: exiting...\n");
	pthread_cleanup_pop(0);
	return (void*)777;	
}

int main(int argc, char *argv[]) {
	pid_t cpid;

	printf("main thread: started, creating child...\n");
	
	pthread_t tid;

	int res = pthread_create(&tid, NULL, thread_func, NULL);

	if (res != 0) {
		perror("pthread_create");
		exit(EXIT_FAILURE);
	}

	sleep(5);
	pthread_cancel(tid);
	
	void *ret;
	res = pthread_join(tid, &ret);
	printf("main thread: child thread returned %d\n", (int)(long)ret);

	return 0;
}
