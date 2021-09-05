//
// Search for particular element in multidimentional array.
//
//  (c) Alexei Guirik, 2021
//  This source is licensed under CC BY-NC 4.0 (https://creativecommons.org/licenses/by-nc/4.0/)
// 

#ifndef ERROR_H_
#define ERROR_H_

typedef enum {
    ERROR_OK = 0,
    ERROR_INVALID_ARGS = 1,
    ERROR_ALLOC = 2,
    ERROR_INVALID_VALUE = 3,
    ERROR_INVALID_INDEX = 4,
} error_t;

#endif
