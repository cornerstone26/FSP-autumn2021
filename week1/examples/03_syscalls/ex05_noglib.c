//
// Example of inline gcc assembly (gas). Only works with gcc.
// Information about inline gcc assembly:
// https://gcc.gnu.org/onlinedocs/gcc/Using-Assembly-Language-with-C.html
//
// Compile:
// gcc -nostdlib -static -o ex05_noglibc ex05_noglibc.c
//
// Optionally include -v for verbose output.
// Note: without -static dynamic sections are still added.
//
// (c) Alexei Guirik, 2021
// This source is licensed under CC BY-NC 4.0 (https://creativecommons.org/licenses/by-nc/4.0/)
// 

void _start() {
    /* exit system call */
    asm("movl $1,%eax;"
        "movl $42, %ebx;"
        "int  $0x80"
    );
}
