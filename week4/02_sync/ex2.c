//
// N consumers and 1 produces synchronize work with the help of conditional variables.
//
// (c) Alexei Guirik, 2021
// This source is licensed under CC BY-NC 4.0
// (https://creativecommons.org/licenses/by-nc/4.0/)
//

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/time.h>

#define N 3			// number of threads
#define M 10		// number of work adding cycles

pthread_cond_t work_cond = PTHREAD_COND_INITIALIZER;
pthread_mutex_t work_mutex = PTHREAD_MUTEX_INITIALIZER;
int work;
bool polundra;

void* thread_func(void* arg) {
	pthread_mutex_lock(&work_mutex);
	while (1) {
		while(work == 0 && !polundra)
			pthread_cond_wait(&work_cond, &work_mutex);
				
		while (work) {
			fprintf(stderr, "thread %ld: doing some work... (%d)\n", pthread_self(), work--);
			sleep(1);
		}

		if (polundra) {
			fprintf(stderr, "thread %ld: finished\n", pthread_self());
			break;
		} 		
	}
	pthread_mutex_unlock(&work_mutex);
	return NULL;
}
	
int main(int argc, char *argv[]) {
	pthread_t tid[N];
	int res;
	for (int i = 0; i < N; i++) {
		res = pthread_create(&tid[i], NULL, thread_func, NULL);
	}
	
	for (int i = 0; i < M; i++) {
		pthread_mutex_lock(&work_mutex);
		int add = rand() % 3 + 1;
		work += add;
		fprintf(stderr, "main: [cycle %d]: added %d more units of work\n", i, add);
		pthread_cond_signal(&work_cond);
		pthread_mutex_unlock(&work_mutex);
	}
	
	polundra = true;
	
	void *ret;
	for (int i = 0; i < N; i++) {
		pthread_cond_signal(&work_cond);
		res = pthread_join(tid[i], &ret);
	}

	fprintf(stderr, "main: finished\n");
	return 0;
}
