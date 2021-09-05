//
// Simple example for make utility.
//
//  (c) Alexei Guirik, 2021
//  This source is licensed under CC BY-NC 4.0 (https://creativecommons.org/licenses/by-nc/4.0/)
// 

#include <stdio.h>

void more_func();


int n = 10;

void util_func() {

    printf("util_func() called\n");
    more_func();
}
