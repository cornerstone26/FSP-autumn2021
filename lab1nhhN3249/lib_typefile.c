//
// Example library for Lab 1
// Allows to calculate entropy of file or its part
//
// gcc -shared -fPIC libavgN0000.c -o libavgN0000.so -ldl -lm
//
//  (c) Alexei Guirik, 2021
//  This source is licensed under CC BY-NC 4.0
//  (https://creativecommons.org/licenses/by-nc/4.0/)
//  

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

#include "plugin_api.h"


static char *g_lib_name = "lib_typefile.so";

static char *g_plugin_purpose = "Check type of a file";

static char *g_plugin_author = "Nguyen Hong Hanh";

#define OPT_TEXT "txt"

static int is_txt(const char *filename);
static char *get_filename_ext(const char *filename);

static struct plugin_option g_po_arr[] = {
/*
    struct plugin_option {
        struct option {
           const char *name;
           int         has_arg;
           int        *flag;
           int         val;
        } opt,
        char *opt_descr
    }
*/
    {
        {
            OPT_TEXT,
            no_argument,
            0, 0,
        },
        "Text file"
    },
   
};

static int g_po_arr_len = sizeof(g_po_arr)/sizeof(g_po_arr[0]);

int plugin_get_info(struct plugin_info* ppi) {
    if (!ppi) {
        fprintf(stderr, "ERROR: invalid argument\n");
        return -1;
    }
    
    ppi->plugin_purpose = g_plugin_purpose;
    ppi->plugin_author = g_plugin_author;
    ppi->sup_opts_len = g_po_arr_len;
    ppi->sup_opts = g_po_arr;
    return 0;
}

int plugin_process_file(const char *fname,
        struct option in_opts[],
        size_t in_opts_len) {
    // Return error by default
    int ret = -1;
    //fprintf(stdout, "CHECK!!!\n");
    // Pointer to file mapping
    unsigned char *ptr = NULL;
    
    char *DEBUG = getenv("LAB1DEBUG");
    
    if (!fname || !in_opts || !in_opts_len) {
        errno = EINVAL;
        return -1;
    }
    
    if (DEBUG) {
        for (size_t i = 0; i < in_opts_len; i++) {
            fprintf(stderr, "DEBUG: %s: Got option '%s' \n",
                g_lib_name, in_opts[i].name);
        }
    }

    int saved_errno = 0;
    
    int fd = open(fname, O_RDONLY);
    if (fd < 0) {
        // errno is set by open()
        return -1;
    }
    
    struct stat st = {0};
    int res = fstat(fd, &st);
    if (res < 0) {
        saved_errno = errno;
        goto END;
    }
    
    // Check that size of file is > 0
    if (st.st_size == 0) {
        if (DEBUG) {
            fprintf(stderr, "DEBUG: %s: File size should be > 0\n",
                g_lib_name);
        }
        saved_errno = ERANGE;
        goto END;
    }

    for (size_t i = 0; i < in_opts_len; i++) {
        if (!strcmp(in_opts[i].name, OPT_TEXT)) {
            ret = is_txt(fname);
        }   
    }
    
    END:
    close(fd);
    if (ptr != MAP_FAILED && ptr != NULL) munmap(ptr, st.st_size);
    
    // Restore errno value
    errno = saved_errno;
    
    return ret;
}        

int is_txt(const char *filename) { 
    if (strcmp(get_filename_ext(filename),"txt")){
        return 1;
    } else {
        return 0;
    }
}

char *get_filename_ext(const char *filename) {
    char *dot = strrchr(filename, '.');
    if(!dot || dot == filename) 
        return "";
    return dot + 1;
}