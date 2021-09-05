// Example application that does not use "standard" standard C library (glibc).
// Instead it is statically linked with homemade example of a standard C library mystdlib.o.
//
// Compile with:
// gcc -static -nostdlib -ffreestanding -fno-builtin -o hello hello.c mystdlib.o
//
// (c) Alexei Guirik, 2021
// This source is licensed under CC BY-NC 4.0 (https://creativecommons.org/licenses/by-nc/4.0/)
// 

#include "mystdlib.h"

int main() {
    char *hello = "Хеллоу, ворлд!\n";
    write(MY_STDOUT, hello, strlen(hello));
    exit(0);
}
