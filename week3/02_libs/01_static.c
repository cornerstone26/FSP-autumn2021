//
// Primitive implementation of linear congruental generator.
// More info:
// https://en.wikipedia.org/wiki/Linear_congruential_generator
//
// (c) Alexei Guirik, 2021
// This source is licensed under CC BY-NC 4.0
// (https://creativecommons.org/licenses/by-nc/4.0/)
//

#include <stdlib.h>
#include <stdio.h>
#include <time.h>

/*
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
*/

#define N 5

int main() {
    srand(time(NULL));
    for (int i = 0; i < N; i++) {
        printf("%u\n", rand());
    }
}
