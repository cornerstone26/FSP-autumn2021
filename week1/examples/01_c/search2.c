//
// Search for particular element in multidimentional array.
//
//  (c) Alexei Guirik, 2021
//  This source is licensed under CC BY-NC 4.0 (https://creativecommons.org/licenses/by-nc/4.0/)
// 

#include <errno.h>

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdint.h>
#include <time.h>

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

typedef enum {
    ERROR_OK = 0,
    ERROR_INVALID_ARGS = 1,
    ERROR_ALLOC = 2,
    ERROR_INVALID_VALUE = 3,
} error_t;

int32_t rand_int32() {
    #define RND8 (rand() & 0xff)
    return RND8 << 24 | RND8 << 16 | RND8 << 8 | RND8;
}

error_t alloc_and_init_space(space_t *sp) {
    if (sp == NULL || 
        sp->dims_len <= 0 || sp->dims_len > MAX_DIMS || 
        sp->dims == NULL)
        return ERROR_INVALID_ARGS;

    size_t total_size = sp->dims[0];
    for (int i = 1; i < sp->dims_len; i++)
        total_size *= sp->dims[i];    
    sp->total_size = total_size;

    sp->mem = malloc(sizeof(point_t) * total_size);
    if (sp->mem == NULL) {
        return ERROR_ALLOC;
    }

    for (int i = 0; i < total_size; i++) {
        // Init space element
        sp->mem[i].color.val = rand_int32();
        sp->mem[i].temp = rand_int32();                
    }

    // Element we will search for
    point_t e = { .color.val = 0xff00ff00, .temp = 1 };
    int rand_pos = rand() % total_size;
#ifdef DEBUG
    fprintf(stderr, "The secret element is in position %d\n", rand_pos);
#endif    
    sp->mem[rand_pos] = e;
    
    return ERROR_OK;
}

error_t free_space(space_t *sp) {
    if (sp == NULL || 
        sp->dims_len <= 0 || sp->dims_len > MAX_DIMS || 
        sp->dims == NULL)
        return ERROR_INVALID_ARGS;

    free(sp->mem);
    sp->mem = NULL;     // !!
    sp->total_size = 0;
    free(sp->dims);
    sp->dims = NULL;    // !!
    sp->dims_len = 0;
    
    return ERROR_OK;
}

error_t search_for_sample(space_t *sp, point_t *sample, point_t **found) {
    if (sp == NULL || 
        sp->dims_len <= 0 || sp->dims_len > MAX_DIMS || 
        sp->dims == NULL || sample == NULL || found == NULL)
        return ERROR_INVALID_ARGS;

    point_t *elem = NULL;
    
    for (int i = 0; i < sp->total_size; i++) {
        if (sp->mem[i].color.val == sample->color.val &&
            sp->mem[i].temp == sample->temp) {
            elem = &sp->mem[i];
        }
    }
    
    *found = elem;
    return ERROR_OK;
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

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Expected dimension sizes!\n");
        return 1;
    }

    space_t s = {0};
    
    s.dims_len = argc - 1;
    s.dims = (size_t*)malloc(sizeof(size_t) * s.dims_len);
    if (s.dims == NULL) {
        fprintf(stderr, "malloc failed\n");
        return 2;
    }
    
    for (int i = 1; i < argc; i++) {
        long res;
        if (ERROR_OK != str_to_long(argv[i], &res)) {
            fprintf(stderr, "Integer required, not \"%s\"\n", argv[i]);
            return 1;
        }
        if (res > 0) {
            s.dims[i - 1] = (size_t)res;
        }
        else {
            fprintf(stderr, "Dimension size should be > 0\n");
            return 1;
        }
    }
        
    srand(time(NULL));
    
    if (ERROR_OK != alloc_and_init_space(&s)) {
        fprintf(stderr, "Failed to allocate space\n");
        return 2;
    }
    
    point_t     sample = { .color.val = 0xff00ff00, .temp = 1 },
                *found = NULL;
    
    if(ERROR_OK == search_for_sample(&s, &sample, &found))
        printf("Sample was %sfound!\n", found ? "" : "not ");
    else
        fprintf(stderr, "Something went wrong\n");
        
    free_space(&s);
    
	return 0;
}

