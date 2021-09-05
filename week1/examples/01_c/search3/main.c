//
// Search for particular element in multidimentional array.
//
//  (c) Alexei Guirik, 2021
//  This source is licensed under CC BY-NC 4.0 (https://creativecommons.org/licenses/by-nc/4.0/)
// 

#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include "space.h"
#include "utils.h"
#include "error.h"

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Expected dimension sizes!\n");
        fprintf(stderr, "Usage: %s D1 D2 ... DM\n", argv[0]);
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
        s.dims[i - 1] = (size_t)res;        
    }
        
    srand(time(NULL));
    
    if (ERROR_OK != space_alloc(&s)) {
        fprintf(stderr, "Failed to allocate space\n");
        return 2;
    }

    if (ERROR_OK != space_init(&s)) {
        fprintf(stderr, "Failed to initialize space\n");
        return 2;
    }
    
    point_t     sample = { .color.val = 0xff00ff00, .temp = 1 },
                *found = NULL;
    
    if(ERROR_OK == space_search(&s, &sample, &found))
        printf("Sample was %sfound!\n", found ? "" : "not ");
    else
        fprintf(stderr, "Something went wrong\n");
        
    space_free(&s);
    
	return 0;
}

