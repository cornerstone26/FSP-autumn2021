//
// Example of an embryo of a simple "standard C" library.
//
// Compile with:
// gcc -fno-builtin -c mystdlib.c
//
// (c) Alexei Guirik, 2021
// This source is licensed under CC BY-NC 4.0 (https://creativecommons.org/licenses/by-nc/4.0/)
// 



#include "mystdlib.h"

#define SYS_write   1
#define SYS_exit    60

/*
    Source of macros:
    https://stackoverflow.com/questions/39927790/c-standard-library-functions-vs-system-calls-which-is-open
    
    More info about gcc inline assembly syntax:
    https://www.felixcloutier.com/documents/gcc-asm.html
*/
#define MY_SYSCALL1_NORET(nr, arg1) \
    __asm__ ( "syscall\n\t" \
            : \
            : "a" (nr), "D" (arg1) \
            : "rcx", "r11" )

#define MY_SYSCALL3(retval, nr, arg1, arg2, arg3) \
    __asm__ ( "syscall\n\t" \
            : "=a" (retval) \
            : "a" (nr), "D" (arg1), "S" (arg2), "d" (arg3) \
            : "rcx", "r11" )

/*
    Pure library functions, no syscalls
*/

int strlen(const char *str) {
    if (str == NULL) return -1;
    const char *beg = str;
    while(*str++);
    return str - beg - 1;
}

/*
    Syscall wrappers
*/

int write(int fd, const void *buf, int count) {
    if (fd == -1 || buf == NULL || count <= 0) return -1;
    int ret = 0;
    MY_SYSCALL3(ret, SYS_write, fd, buf, count);
    return ret;    
}

void exit(int code) {
    MY_SYSCALL1_NORET(SYS_exit, code);
}


/*
    User code entry point
*/
int main();

/*
    Actual entry point
*/
void _start() {
    int code = main();
    exit(code);
}
