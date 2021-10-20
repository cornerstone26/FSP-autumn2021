//
// Calls my_srand() and my_rand() dynamically. 
//
// (c) Alexei Guirik, 2021
// This source is licensed under CC BY-NC 4.0
// (https://creativecommons.org/licenses/by-nc/4.0/)
//

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <dlfcn.h>

//#include "my_lib.h"

#define N 5

int main() {
    void *my_lib = dlopen("libmy.so",  RTLD_LAZY | RTLD_GLOBAL);
    if (!my_lib)  {
        printf("ERROR: dlopen: %s\n", dlerror());
        exit(1);
    }

    void (*my_srand_fptr)(int) = dlsym(my_lib, "my_srand");
    int (*my_rand_fptr)() = dlsym(my_lib, "my_rand");
    
    if (my_srand_fptr == NULL || my_rand_fptr == NULL) {
        printf("ERROR: dlsym: %s\n", dlerror());
        exit(1);
    }
     
    my_srand_fptr(time(NULL));
    for (int i = 0; i < N; i++) {
        printf("%u\n", my_rand_fptr());
    }
    
    // Check with pmap
    getchar();
    dlclose(my_lib);
 
    // Once more
    getchar();   
    return 0;
}
