//
// Example of an embryo of a simple "standard C" library.
//
// (c) Alexei Guirik, 2021
// This source is licensed under CC BY-NC 4.0 (https://creativecommons.org/licenses/by-nc/4.0/)
// 

#ifndef MY_STDLIB_H_
#define MY_STDLIB_H_

#ifndef __x86_64__
#error  This embryo of a standard C library only works on x86_64 architecture!
#endif

#define NULL        ((void*)0)

#define MY_STDIN    0
#define MY_STDOUT   1
#define MY_STDERR   2

int write(int fd, const void *buf, int count);
int strlen(const char *str);
void exit(int code);

#endif
