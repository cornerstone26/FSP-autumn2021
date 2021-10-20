//
// Allocates memory and frees some of it.
// Check it:
// LD_PRELOAD=./libmtr.so ./04_malloc 
//
// (c) Alexei Guirik, 2021
// This source is licensed under CC BY-NC 4.0
// (https://creativecommons.org/licenses/by-nc/4.0/)
//

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>

#define ALLOC_NUM       10
#define ALLOC_SIZE      256

int main() {
    srand(time(NULL));
    void *ptr;
    for (int i = 0; i < ALLOC_NUM; i++) {
        ptr = malloc(ALLOC_SIZE);
        printf("."); fflush(stdout);
        if (rand() % 2)
            free(ptr);
        usleep(100000);
    }
    puts("\n");   
    return 0;
}
