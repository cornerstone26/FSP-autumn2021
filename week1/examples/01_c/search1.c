// Attention! This sample program has serious bug! Can you spot it?
//
// Search for particular element in multidimentional array.
//
// Note: This example will only work with gcc, as it requires gcc extensions:
// https://embeddedbits.org/gcc-extensions-to-the-c-language/
//
//  (c) Alexei Guirik, 2021
//  This source is licensed under CC BY-NC 4.0 (https://creativecommons.org/licenses/by-nc/4.0/)
//
 
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <time.h>

// How deep should we go
#define N 4

// Some RGBA type
typedef union {
    int32_t   val;
    struct {
        int8_t val_r, val_g, val_b, val_a;
    };
} color_t;

typedef unsigned int uint_t;

typedef int temp_t;

typedef struct {
    color_t     color;
    temp_t      temp;
} point_t;


int32_t rand_int32() {
    #define RND8 (rand() & 0xff)
    return RND8 << 24 | RND8 << 16 | RND8 << 8 | RND8;
}

int alloc_and_init_space(point_t *****space_ptr, uint_t dim1, uint_t dim2, uint_t dim3, uint_t dim4) {
    point_t ****space = *space_ptr;
    
    space = malloc(sizeof(point_t***) * dim1);
    for (int i1 = 0; i1 < dim1; i1++) {
        space[i1] = malloc(sizeof(point_t**) * dim2);
        for (int i2 = 0; i2 < dim2; i2++) {
            space[i1][i2] = malloc(sizeof(point_t*) * dim3);
            for (int i3 = 0; i3 < dim3; i3++) {
                space[i1][i2][i3] = malloc(sizeof(point_t) * dim4);                
                for (int i4 = 0; i4 < dim4; i4++) {
                    // Init space element
                    space[i1][i2][i3][i4].color.val = rand_int32();
                    space[i1][i2][i3][i4].temp = rand_int32();                
                }
            }
        }
    }

    // Element we will search for
    /*
    point_t e = { .color.val = 0xff00ff00, .temp = 1 };
    space[ rand() % dim1 ]
        [ rand() % dim2 ]
        [ rand() % dim3 ]
        [ rand() % dim4 ] = e;
    */        
    *space_ptr = space;
}

void free_space(point_t ****space, uint_t dim1, uint_t dim2, uint_t dim3, uint_t dim4) {
    for (int i1 = 0; i1 < dim1; i1++) {
        for (int i2 = 0; i2 < dim2; i2++) {
            for (int i3 = 0; i3 < dim3; i3++) {
                free(space[i1][i2][i3]);    
            }
            free(space[i1][i2]);
        }
        free(space[i1]);
    }
    free(space);
}

point_t *search_for_sample(point_t ****space, uint_t dim1, uint_t dim2, uint_t dim3, uint_t dim4, point_t *sample) {
    point_t *elem = NULL;
    
    for (int i1 = 0; i1 < dim1; i1++) {
        for (int i2 = 0; i2 < dim2; i2++) {
            for (int i3 = 0; i3 < dim3; i3++) {
                for (int i4 = 0; i4 < dim4; i4++) {
                    if (space[i1][i2][i3][i4].color.val == sample->color.val &&
                        space[i1][i2][i3][i4].temp == sample->temp) {
                            elem = &space[i1][i2][i3][i4];
                            goto END_SEARCH; // OMG, it's a goto stmt! Get in the car! >:3
                        }
                }       
            }
        }
    }
    END_SEARCH:
    
    return elem;
}

int main(int argc, char *argv[]) {
    if (argc != N + 1) {
        fprintf(stderr, "Expected %d dimension sizes!\n", N);
        return 1;
    }

    // Note: atoi() generally considered bad practice
    uint_t dim1 = atoi(argv[1]),
            dim2 = atoi(argv[2]),
            dim3 = atoi(argv[3]),
            dim4 = atoi(argv[4]);
            
    srand(time(NULL));
    
    point_t ****space = NULL;
    
    if (!alloc_and_init_space(&space, dim1, dim2, dim3, dim4)) {
        fprintf(stderr, "Failed to allocate space\n");
        return 1;
    }
    
    point_t     sample = { .color.val = 0xff00ff00, .temp = 1 },
                *found = NULL;

    found = search_for_sample(space, dim1, dim2, dim3, dim4, &sample);
                    
    printf("Sample was %sfound!\n", found ? "" : "not ");

    free_space(space, dim1, dim2, dim3, dim4);
    
	return 0;
}

