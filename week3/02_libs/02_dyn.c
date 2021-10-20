//
// Calling function from shared library.
//
// Compile with:
// gcc -o 02_dyn 02_dyn.c libmy.so
// Execute with:
// LD_LIBRARY_PATH=. ./02_dyn
//
// (c) Alexei Guirik, 2021
// This source is licensed under CC BY-NC 4.0
// (https://creativecommons.org/licenses/by-nc/4.0/)
//

#include <stdio.h>
#include <time.h>

#include "my_lib.h"

#define N 5

int main() {
    my_srand(time(NULL));
    for (int i = 0; i < N; i++) {
        printf("%u\n", my_rand());
    }
}
