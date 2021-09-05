//
// Example of inline gcc assembly (gas). Only works with gcc.
// Information about inline gcc assembly:
// https://gcc.gnu.org/onlinedocs/gcc/Using-Assembly-Language-with-C.html
//
// Compile:
// gcc -nostdlib -e foo -o ex04_inline ex04_inline.c
//
// (c) Alexei Guirik, 2021
// This source is licensed under CC BY-NC 4.0 (https://creativecommons.org/licenses/by-nc/4.0/)
// 

int foo() {
    unsigned long sys_write = 1, sys_exit = 60;
    unsigned long fd = 1;
    long exit_code = 42;
    char *msg = "Hello, world!\n";
    long msg_len = 14;
    
    asm(
        "movq       %0, %%rax\n"
        "movq       %1, %%rdi\n"
        "movq       %2, %%rsi\n"
        "movq       %3, %%rdx\n"
        "syscall\n"
        "movq       %4, %%rax\n"
        "movq       %5, %%rdi\n"
        "syscall"
        : /* no output parameters */
        : "m"(sys_write), "m"(fd), "m"(msg), "m"(msg_len), 
            "m"(sys_exit), "m"(exit_code)
    );   
}
