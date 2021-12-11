//
// Causing SIGSEGV.
//
// (c) Alexei Guirik, 2021
// This source is licensed under CC BY-NC 4.0
// (https://creativecommons.org/licenses/by-nc/4.0/)
//

#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

int main() {
    printf("Goodbye,");
    int *ptr = NULL;
    *ptr = 42;
    printf(" cruel world!\n");
    
	return 0;
}
