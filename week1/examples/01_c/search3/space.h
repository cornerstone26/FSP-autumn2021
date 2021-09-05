//
// Search for particular element in multidimentional array.
//
//  (c) Alexei Guirik, 2021
//  This source is licensed under CC BY-NC 4.0 (https://creativecommons.org/licenses/by-nc/4.0/)
// 

#ifndef SPACE_H_
#define SPACE_H_

#include <stdint.h>

#include "error.h"

// How deep might we go
#define MAX_DIMS 100

typedef union {
    int32_t   val;
    struct {
        int8_t val_r, val_g, val_b, val_a;
    };
} color_t;

typedef int temp_t;

typedef struct {
    color_t     color;
    temp_t      temp;
} point_t;

typedef struct {
    point_t *mem;
    size_t total_size;
    size_t dims_len;
    size_t *dims;
} space_t;

error_t space_alloc(space_t *sp);
error_t space_init(space_t *sp);
error_t space_free(space_t *sp);
error_t space_get(space_t *sp, size_t indices[], size_t indices_len, point_t **elem);
error_t space_search(space_t *sp, point_t *sample, point_t **found);

#endif
