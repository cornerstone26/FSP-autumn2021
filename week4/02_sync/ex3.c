//
// Readers and a writer synchronize work with the help of a read-write lock.
//
// (c) Alexei Guirik, 2021
// This source is licensed under CC BY-NC 4.0
// (https://creativecommons.org/licenses/by-nc/4.0/)
//

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
 
char* data;
 
typedef struct arg_tag {
    pthread_rwlock_t *lock;
    pthread_spinlock_t *stop_flag_lock;
    int *stop;
    const char* thread_name;
} arg_t;
 
void* reader_func(void *args) {
    arg_t *arg = (arg_t*)args;
    int stop_flag;
    do {
        pthread_spin_lock(arg->stop_flag_lock);
        stop_flag = *arg->stop;
        pthread_spin_unlock(arg->stop_flag_lock);
        if (stop_flag) {
            break;
        }
        pthread_rwlock_rdlock(arg->lock);
        printf("reader %s [%s]\n", arg->thread_name, data);
        pthread_rwlock_unlock(arg->lock);
        sleep(3);
    } while(1);
}
 
void* writer_func(void *args) {
    arg_t *arg = (arg_t*)args;
    int stop_flag;
 
    do {
        pthread_spin_lock(arg->stop_flag_lock);
        stop_flag = *arg->stop;
        pthread_spin_unlock(arg->stop_flag_lock);
        
        if (stop_flag) {
            break;
        }
 
        pthread_rwlock_wrlock(arg->lock);
        free(data);
        data = (char*) malloc(100);
        int rnd = rand() % 3;
        sprintf(data, "%d", rnd);
        printf("writer %s [%d]\n", arg->thread_name, rnd);
        pthread_rwlock_unlock(arg->lock);
        sleep(rnd);
    } while(1);
}
 
int main() {
    pthread_rwlock_t rw_lock;
    pthread_spinlock_t stop_spin;
    pthread_t readers[5];
    pthread_t writer;
    int stop = 0;
    int i;
 
    arg_t wtr = {
        &rw_lock,
        &stop_spin,
        &stop,
        "writer"
    };
    arg_t rdr[] = { {
        &rw_lock,
        &stop_spin,
        &stop,
        "reader 1"
    },{
        &rw_lock,
        &stop_spin,
        &stop,
        "reader 2"
    },{
        &rw_lock,
        &stop_spin,
        &stop,
        "reader 3"
    },{
        &rw_lock,
        &stop_spin,
        &stop,
        "reader 4"
    },{
        &rw_lock,
        &stop_spin,
        &stop,
        "reader 5"
        }
    };
 
 	srand(time(NULL));
 	
    data = (char*) malloc(100);
    strcpy(data, "source");
 
    pthread_rwlock_init(&rw_lock, NULL);
    pthread_spin_init(&stop_spin, 0);
 
    for (i = 0; i < 5; i++) {
        pthread_create(&readers[i], NULL, reader_func, &rdr[i]);
        pthread_detach(readers[i]);
    }
    pthread_create(&writer, NULL, writer_func, &wtr);
    pthread_detach(writer);
 
    fgetc(stdin);
    pthread_spin_lock(&stop_spin);
    stop = 1;
    pthread_spin_unlock(&stop_spin);
    fgetc(stdin);
 
    return 0;
}
