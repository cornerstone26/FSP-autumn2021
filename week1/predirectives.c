#include <stdio.h> 
// #define HELLO {printf("Hello Hanh!\n");}
// #define ITMO "Itmo University"
// //#undef ITMO 
// #define mul(x,y) (x)*(y)
#define DLEVEL 10
#if DLEVEL > 5
    #define SIGNAL  1
    #if STACKUSE == 1
        #define STACK   200
    #else
        #define STACK   100
    #endif
#else
    #define SIGNAL  0
    #if STACKUSE == 1
        #define STACK   100
    #else
        #define STACK   50
    #endif
#endif
#if DLEVEL == 0
    #define STACK 0
#elif DLEVEL == 1
    #define STACK 100
#elif DLEVEL > 5
    display( debugptr );
#else
    #define STACK 200
#endif

int main(){
    // HELLO;
    // printf(ITMO);
    // printf("\n%d", mul(8+5, 20));
    printf("%d %d", STACK, SIGNAL);
    return 0;
}