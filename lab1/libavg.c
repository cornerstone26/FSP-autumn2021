
//
// gcc -shared -fPIC libavgN0000.c -o libavgN0000.so -ldl -lm
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


static char *g_lib_name = "libavg.so";

static char *g_plugin_purpose = "Check uuids";

static char *g_plugin_author = "TRAN";

#define OPT_UUIDS "uuids"


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
            OPT_UUIDS,
            0,
            0, 0,
        },
        "check uuids"
    }
    
    
};

static int g_po_arr_len = sizeof(g_po_arr)/sizeof(g_po_arr[0]);

//

//
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
    
    // Pointer to file mapping
    unsigned char *ptr = NULL;
    
    char *DEBUG = getenv("LAB1DEBUG");
    
    if (!fname || !in_opts || !in_opts_len) {
        errno = EINVAL;
        return -1;
    }
    
    if (DEBUG) {
        for (size_t i = 0; i < in_opts_len; i++) {
            fprintf(stderr, "DEBUG: %s: Got option '%s' with arg '%s'\n",
                g_lib_name, in_opts[i].name, (char*)in_opts[i].flag);
        }
    }
    
   char uuids = '\0';
    
/*    int got_uuids = 0;

#define OPT_CHECK(opt_var) \
    if (got_##opt_var) { \
        if (DEBUG) { \
            fprintf(stderr, "DEBUG: %s: Option '%s' was already supplied\n", \
                g_lib_name, in_opts[i].name); \
        } \
        errno = EINVAL; \
        return -1; \
    } \
    else { \
        if (opt_var != '\0' ) { \
            if (DEBUG) { \
                fprintf(stderr, "DEBUG: %s: Failed to convert '%s'\n", \
                    g_lib_name, (char*)in_opts[i].flag); \
            } \
            errno = EINVAL; \
            return -1; \
        } \
        got_##opt_var = 1; \
    }
    
    for (size_t i = 0; i < in_opts_len; i++) {
        if (!strcmp(in_opts[i].name, OPT_UUIDS)) {
            OPT_CHECK(uuids);
        }
        
        else {
            errno = EINVAL;
            return -1;
        }
    }
    
    
    // uuids value should be null
    if (uuids != '\0') {
        if (DEBUG) {
            fprintf(stderr, "DEBUG: %s: uuids should be NULL\n",
                g_lib_name);
        }
        errno = EINVAL;
        return -1;
    }
*/  
    
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
    
  
    
    ptr = mmap(0, st.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (ptr == MAP_FAILED) {
        saved_errno = errno;
        goto END;
    }
    char *get_uuid = alloca(256);
    char *f;
    /* Get the size of the file. */
    //int status = fstat (fd, &st);
    
	
    f = (char *) mmap (0, st.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    if(f[8] == 45 && f[13] == 45 && f[18] == 45 && f[23] == 45)
    {	
    	for (int i = 0; i < st.st_size; i++) {
    		char c;
    		c=f[i];
       
       		if( (48<= c && c <= 57 )|| (97 <= c && c <= 122))
       			{
       				get_uuid[i] = c;	
       			}  	
    	}
    }
    else{
    	get_uuid = " UUIDS not exist";
 	}
    if (DEBUG) {
        fprintf(stderr, "DEBUG: %s: value uuids of = %s\n",g_lib_name, get_uuid);
    }   
   
    int check = get_uuid != NULL?1:0;
    // 0 or 1
    ret = check >= uuids;
    
    END:
    close(fd);
    if (ptr != MAP_FAILED && ptr != NULL) munmap(ptr, st.st_size);
    
    // Restore errno value
    errno = saved_errno;
    
    return ret;
}        
