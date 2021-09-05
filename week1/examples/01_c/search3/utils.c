//
// Search for particular element in multidimentional array.
//
//  (c) Alexei Guirik, 2021
//  This source is licensed under CC BY-NC 4.0 (https://creativecommons.org/licenses/by-nc/4.0/)
// 

#include <errno.h>

#include <stdlib.h>
#include <string.h>

#include "error.h"

int32_t rand_int32() {
    #define RND8 (rand() & 0xff)
    return RND8 << 24 | RND8 << 16 | RND8 << 8 | RND8;
}

error_t str_to_long(const char *str, long *pres) {
    if (str == NULL || pres == NULL)
        return ERROR_INVALID_ARGS;
        
    char *end = NULL;
    errno = 0;
    long l = strtol(str, &end, 0);
    if ((end != NULL && *end != '\0') || errno != 0)
        return ERROR_INVALID_VALUE;
    
    *pres = l;
    return ERROR_OK;
}

