//
// Getting thread identifier with syscall gettid().
//
// (c) Alexei Guirik, 2021
// This source is licensed under CC BY-NC 4.0
// (https://creativecommons.org/licenses/by-nc/4.0/)
//

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/syscall.h>


long gettid() {
    // no glibc wrapper for this function,
    // hence we need to call it via syscall
    return syscall(SYS_gettid);
}

void* threadMethod(void* arg) {
    int intArg = (int) *((int*) arg);

    int32_t pid = getpid();
    uint64_t pti = pthread_self();
    long   tid = gettid();
    
    printf("[Thread %d] getpid() = %d\n", intArg, pid);
    printf("[Thread %d] gettid() = %lu\n", intArg, tid);
    printf("[Thread %d] pthread_self() = %lu\n", intArg, pti);
    
    pthread_t thread;
    if (intArg == 1) {
        // start thread from 'child' thread
        int thread3 = 3;

        if ((pthread_create(&thread, NULL, threadMethod, (void*) &thread3)) != 0) {
            fprintf(stderr, "pthread_create: error\n");
            exit(EXIT_FAILURE);
        }
        
    }
    
    sleep(100);

    if ((pthread_join(thread, NULL)) != 0) {
        fprintf(stderr, "Could not join thread 1\n");
        exit(EXIT_FAILURE);
    }
}

int main()
{
    pthread_t threads[2];

    int thread1 = 1;

    if ((pthread_create(&threads[0], NULL, threadMethod, (void*) &thread1))
         != 0)
    {
        fprintf(stderr, "pthread_create: error\n");
        exit(EXIT_FAILURE);
    }

    int thread2 = 2;

    if ((pthread_create(&threads[1], NULL, threadMethod, (void*) &thread2))
         != 0)
    {
        fprintf(stderr, "pthread_create: error\n");
        exit(EXIT_FAILURE);
    }

    int32_t pid = getpid();
    uint64_t pti = pthread_self();
    long   tid = gettid();
    
    printf("[Process] getpid() = %d\n", pid);
    printf("[Process] gettid() = %lu\n", tid);
    printf("[Process] pthread_self() = %lu\n", pti);

    sleep(100);
    
    if ((pthread_join(threads[0], NULL)) != 0) {
        fprintf(stderr, "Could not join thread 1\n");
        exit(EXIT_FAILURE);
    }

    if ((pthread_join(threads[1], NULL)) != 0) {
        fprintf(stderr, "Could not join thread 2\n");
        exit(EXIT_FAILURE);
    }

    return 0;
}
