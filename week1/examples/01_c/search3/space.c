//
// Search for particular element in multidimentional array.
//
//  (c) Alexei Guirik, 2021
//  This source is licensed under CC BY-NC 4.0 (https://creativecommons.org/licenses/by-nc/4.0/)
// 

#include <stdlib.h>
#ifdef DEBUG
#include <stdio.h>
#endif // DEBUG

#include "space.h"
#include "utils.h"

error_t space_alloc(space_t *sp) {
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
    
    return ERROR_OK;
}

error_t space_init(space_t *sp) {
    if (sp == NULL || 
        sp->dims_len <= 0 || sp->dims_len > MAX_DIMS || 
        sp->dims == NULL || sp->mem == NULL)
        return ERROR_INVALID_ARGS;

    for (int i = 0; i < sp->total_size; i++) {
        // Init space element
        sp->mem[i].color.val = rand_int32();
        sp->mem[i].temp = rand_int32();                
    }

#ifdef TEST_SEARCH
    // Element we will search for
    point_t e = { .color.val = 0xff00ff00, .temp = 1 };
    int rand_pos = rand() % sp->total_size;
    
#ifdef DEBUG
    fprintf(stderr, "The secret element is in position %d\n", rand_pos);
#endif // DEBUG
    
    sp->mem[rand_pos] = e;
#endif // TEST_SEARCH

    return ERROR_OK;
}

error_t space_free(space_t *sp) {
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

error_t space_get(space_t *sp, size_t indices[], size_t indices_len, point_t **elem) {
    if (sp == NULL || 
        sp->dims_len <= 0 || sp->dims_len > MAX_DIMS || sp->dims == NULL || 
        indices_len != sp->dims_len || elem == NULL)
        return ERROR_INVALID_ARGS;

    // Mapping indices in [D1][D2][...][DM] space to flat space:
    // [i1][i2][...][iN] == i1 * D2 * ... * DM + i2 * D3 ... DM + ... + iN 
    
    size_t flat_idx = 0;
    for (int i = 0; i < indices_len; i++) {
        size_t sub_res = 1;
        for (int j = i + 1; j < sp->dims_len; j++) {
            sub_res *= sp->dims[j];
        }
        sub_res *= indices[i];
        flat_idx += sub_res;
    }
    
    if (flat_idx > sp->total_size)
        return ERROR_INVALID_INDEX;
    else
        *elem = &sp->mem[flat_idx];
        
    return ERROR_OK;
}

error_t space_search(space_t *sp, point_t *sample, point_t **found) {
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

