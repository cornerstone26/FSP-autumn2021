#include <stdlib.h>
#include <stdio.h>

int global_var;

int main() {
    int local_var;
    
    int static static_var;
    
    int *heap_var = malloc(sizeof(int));
    
    int *big_var = malloc(1024 * 1024 * 1024);
    
    printf(
"&global_var = %p\n"
"&local_var  = %p\n"
"&static_var = %p\n"
"&heap_var   = %p\n"
"&big_var    = %p\n"
"&main       = %p\n",
        &global_var, &local_var, &static_var, heap_var, big_var, main);
        
    return 0;
}
