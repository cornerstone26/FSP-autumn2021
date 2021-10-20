//
// Example of a library implementing LCG.
//
// More info about LCG:
// https://en.wikipedia.org/wiki/Linear_congruential_generator
//
// Compile with:
// gcc -fPIC -shared my_lib.c -o libmy.so
//
// (c) Alexei Guirik, 2021
// This source is licensed under CC BY-NC 4.0
// (https://creativecommons.org/licenses/by-nc/4.0/)
//

#include <stdlib.h>
#include <stdio.h>
#include <time.h>

int m = 1 << 31;
int a = 1103515245;
int c = 12345;
int seed = 1;

void my_srand(int seed_) {
    seed = seed_;
}

int my_rand() {
    return (seed = (a * seed + c) % m);
}


// try to comment out constructors/destructors
static void my_cleaner() {
    printf("Keep it clean! Or I will.\n");
}

void __attribute__ ((constructor)) init(void) {
    //
    // Function that is called when the library is loaded
    //
    printf("Library is initialized\n"); 
    atexit(my_cleaner);
}

void __attribute__ ((destructor)) finish(void) {
    //
    // Function that is called when the library is unloaded
    //
    printf("Library is finalized\n"); 
}
