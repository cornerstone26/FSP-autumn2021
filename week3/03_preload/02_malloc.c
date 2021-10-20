//
// Allocates memory and frees some of it. Uses mtrace (man 3 mrace)
//  to trace allocations.
//
// Check it:
// MALLOC_TRACE=/tmp/mtrace.log ./02_malloc
// mtrace /tmp/mtrace.log
//
// (c) Alexei Guirik, 2021
// This source is licensed under CC BY-NC 4.0
// (https://creativecommons.org/licenses/by-nc/4.0/)
//

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <mcheck.h>     // !!

#define ALLOC_NUM       10
#define ALLOC_SIZE      256

int main() {
    srand(time(NULL));
    mtrace();           // !!
    void *ptr;
    for (int i = 0; i < ALLOC_NUM; i++) {
        ptr = malloc(ALLOC_SIZE);
        printf("."); fflush(stdout);
        if (rand() % 2)
            free(ptr);
        usleep(100000);
    }
    
    return 0;
}
