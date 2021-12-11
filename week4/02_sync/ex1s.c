//
// Thread synchronization using spinlock.
//
// (c) Alexei Guirik, 2021
// This source is licensed under CC BY-NC 4.0
// (https://creativecommons.org/licenses/by-nc/4.0/)
//

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>

#define N 3

pthread_spinlock_t spin;
int counter;

void* thread_func(void* arg) {
	for (int i = 0; i < 1000000; i++) {
		pthread_spin_lock(&spin);
		counter++;
		pthread_spin_unlock(&spin);
	}
	return NULL;
}
	
int main(int argc, char *argv[]) {
	struct timeval t0, t1;
	gettimeofday(&t0, 0);

	pthread_spin_init(&spin, 0);
	pthread_t tid[N];
	int res;
	for (int i = 0; i < N; i++) {
		res = pthread_create(&tid[i], NULL, thread_func, NULL);

		if (res != 0) {
			perror("pthread_create");
			exit(EXIT_FAILURE);
		}
	}
	
	void *ret;
	for (int i = 0; i < N; i++) {
		res = pthread_join(tid[i], &ret);
	}

	gettimeofday(&t1, 0);	
	long long elapsed = (t1.tv_sec - t0.tv_sec) * 1000000LL + t1.tv_usec - t0.tv_usec;
	
	printf("counter = %d\n", counter);
	printf("time elapsed = %lldμs\n", elapsed);

	pthread_spin_destroy(&spin);

	return 0;
}
