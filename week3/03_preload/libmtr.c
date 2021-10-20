//
// Example library for intercepting library calls. Counts
//  and compares number of malloc and free calls.
//
// gcc -shared -fPIC libmtr.c -o libmtr.so -ldl
//
// Based on https://stackoverflow.com/questions/6083337/overriding-malloc-using-the-ld-preload-mechanism
//

#define _GNU_SOURCE
#include <dlfcn.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

unsigned long malloc_calls = 0;
unsigned long free_calls = 0;

char tmpbuff[1024];
unsigned long tmppos = 0;
unsigned long tmpallocs = 0;

void *memset(void*,int,size_t);
void *memmove(void *to, const void *from, size_t size);

/*=========================================================
 * interception points
 */

static void * (*myfn_calloc)(size_t nmemb, size_t size);
static void * (*myfn_malloc)(size_t size);
static void   (*myfn_free)(void *ptr);
static void * (*myfn_realloc)(void *ptr, size_t size);
static void * (*myfn_memalign)(size_t blocksize, size_t bytes);

static void init() {
    myfn_malloc     = dlsym(RTLD_NEXT, "malloc");
    myfn_free       = dlsym(RTLD_NEXT, "free");
    myfn_calloc     = dlsym(RTLD_NEXT, "calloc");
    myfn_realloc    = dlsym(RTLD_NEXT, "realloc");
    myfn_memalign   = dlsym(RTLD_NEXT, "memalign");

    if (!myfn_malloc || !myfn_free || !myfn_calloc || !myfn_realloc || !myfn_memalign) {
        fprintf(stderr, "Error in `dlsym`: %s\n", dlerror());
        exit(1);
    }
}

void *malloc(size_t size) {
    static int initializing = 0;
    if (myfn_malloc == NULL) {
        if (!initializing) {
            initializing = 1;
            init();
            initializing = 0;
            
            if (tmppos != 0)
                fprintf(stdout, "libmtr: allocated %lu bytes of temp memory in %lu chunks during initialization\n", tmppos, tmpallocs);
        }
        else {
            if (tmppos + size < sizeof(tmpbuff)) {
                void *retptr = tmpbuff + tmppos;
                tmppos += size;
                ++tmpallocs;
                return retptr;
            }
            else {
                fprintf(stdout, "libmtr: too much memory requested during initialisation - increase tmpbuff size\n");
                exit(1);
            }
        }
    }
    void *ptr = myfn_malloc(size);
    malloc_calls++;
    return ptr;
}

void free(void *ptr) {
    if (myfn_malloc == NULL)
        init();

    if (ptr >= (void*) tmpbuff && ptr <= (void*)(tmpbuff + tmppos))
        fprintf(stdout, "libmtr: freeing temp memory\n");
    else {
        myfn_free(ptr);
        free_calls++;
   }
}

void *realloc(void *ptr, size_t size) {
    if (myfn_malloc == NULL) {
        void *nptr = malloc(size);
        if (nptr && ptr) {
            memmove(nptr, ptr, size);
            free(ptr);
        }
        return nptr;
    }

    void *nptr = myfn_realloc(ptr, size);
    return nptr;
}

void *calloc(size_t nmemb, size_t size) {
    if (myfn_malloc == NULL){
        void *ptr = malloc(nmemb*size);
        if (ptr)
            memset(ptr, 0, nmemb*size);
        return ptr;
    }

    void *ptr = myfn_calloc(nmemb, size);
    return ptr;
}

void *memalign(size_t blocksize, size_t bytes) {
    // TODO!
    void *ptr = myfn_memalign(blocksize, bytes);    
    return ptr;
}

static void __attribute__((destructor)) deinitialize(void) {
    if (malloc_calls != free_calls) {
        printf("libmtr: WARNING: number of calls to malloc() and free() differ! [%lu vs %lu]\n", malloc_calls, free_calls);
    }
}
