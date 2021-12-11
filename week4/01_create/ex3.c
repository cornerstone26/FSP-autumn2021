//
// Example of using attributes to create a thread with a given stack size.
// To watch for changes:
// watch -n 0.5 'ps -eLo pid,ppid,tid,tgid,comm | grep ex3'
//
// (c) Alexei Guirik, 2021
// This source is licensed under CC BY-NC 4.0
// (https://creativecommons.org/licenses/by-nc/4.0/)
//

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


void* thread_func(void* arg) {
	printf("child thread: entered, waiting...\n");
	sleep(10);
	printf("child thread: exiting...\n");
	return (void*)777;
}
	
int main(int argc, char *argv[]) {
	pid_t cpid;

	printf("main thread: started, creating child...\n");
	
	pthread_t tid;
	pthread_attr_t attr;
	int res = pthread_attr_init(&attr);
	res = pthread_attr_setstacksize(&attr, 8 * 1024);
	res = pthread_create(&tid, &attr, thread_func, NULL);

	if (res != 0) {
		perror("pthread_create");
		exit(EXIT_FAILURE);
	}

	printf("main thread: exiting...\n");
	
	
	void *ret;
	res = pthread_join(tid, &ret);
	pthread_attr_destroy(&attr);
	printf("main thread: child thread returned %d\n", (int)(long)ret);

	return 0;
}
