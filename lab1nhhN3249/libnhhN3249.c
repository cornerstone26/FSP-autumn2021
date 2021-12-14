#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <stdarg.h>
#include <math.h>
#include "plugin_api.h"

static char *g_lib_name = "libnhhN3249.so";

static char *g_plugin_purpose = "Variant 5: Search for files containing a given sequence of bits.";

static char *g_plugin_author = "Nguyen Hong Hanh N3249";


#define OPT_BIT_SEQ "bit-seq"

int bit_seq_match(unsigned char *buffer, long long pattern, size_t pSize, size_t fSize);
long long get_pattern(char* pat);

static struct plugin_option g_po_arr[] = {
/*
    struct plugin_option { // cấu trúc option
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
          OPT_BIT_SEQ,
          required_argument,
          0, 0,
        },

        "A sequence of bits, you want to search for"
    },    
    
};

static int g_po_arr_len = sizeof(g_po_arr)/sizeof(g_po_arr[0]); // lấy kích thước của mảng g_po_arr

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

    //printf("\nargument parsed to plugin_process_file fname %s, in_opts %s, in_opts_len %ld\n\n", fname, in_opts[0].name , in_opts_len);
    if (!fname || !in_opts || !in_opts_len) {   // thiếu tham số truyền vào
        errno = EINVAL;                         
        return -1;
    }
    
    if (DEBUG) {
        for (size_t i = 0; i < in_opts_len; i++) {
            fprintf(stderr, "DEBUG: %s: Got option '%s' with arg '%s'\n",
                g_lib_name, in_opts[i].name, (char*)in_opts[i].flag);
        }
    }

    //process pattern 

    char* pat = NULL;
    
    size_t pat_size = 0;
    int got_bit_seq = 0;
    long long pattern = 0;
    for (size_t i = 0; i < in_opts_len; i++) {
      if (!strcmp(in_opts[i].name, OPT_BIT_SEQ)) {
        pat = (char*) in_opts[i].flag;
      }       
      
      char *endptr = NULL;
      int base; 
      if ((!memcmp(pat, "0x", 2)) || (!memcmp(pat, "0X", 2))){
        base = 16;
      } else if ((!memcmp(pat, "0b", 2)) || (!memcmp(pat, "0B", 2))){
        base = 2;
        pat = &pat[2]; // remove "0b"/"0B"
      } else {
        base = 10;
      }

      pattern = strtoll(pat, &endptr, base);
      //fprintf(stdout, "pat %s base %d endpoint %s\n",pat, base, endptr);
      got_bit_seq = 1;
      if (*endptr != '\0') { 
        got_bit_seq = 0;
        if (DEBUG) { 
            fprintf(stderr, "DEBUG: %s: Failed to convert '%s'\n", 
                g_lib_name, pat); 
        } 
        errno = EINVAL; 
        return -1; 
      }

      //fprintf(stdout, "%lld\n", pattern);
      //in ra đối số truyền vào
      if (DEBUG) {
          fprintf(stderr, "DEBUG: %s: Inputs: bit_seq = %s\n",
              g_lib_name, pat);
      }

      // get pattern size
      
      if (base == 16){
        //printf("***%s***\n", pat);
        pat = &pat[2]; //remove 0x/0X
        //printf("***%s***\n", pat);
        pat_size = ceil(strlen(pat)/2.0);
      } else if (base == 2){
        pat_size = ceil(strlen(pat)/8.0);
      } else {
        sprintf(pat, "%llx", pattern);
        pat_size = ceil(strlen(pat)/2.0);
        sprintf(pat, "%lld", pattern);
        //printf ("pattern after processing %s\n", pat);
      }
      
      //printf("pattern size %ld \n", pat_size);
    }
    
    if (!got_bit_seq) {
        if (DEBUG) {
            fprintf(stderr, "DEBUG: %s: Bit sequence value was not supplied.\n",
                g_lib_name);
        }
        errno = EINVAL;
        return -1;
    }
    
    ptr = mmap(0, st.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (ptr == MAP_FAILED) {
        saved_errno = errno;
        goto END;
    }
    //FILE *pFile = fopen(fname, "rb");

    ret = bit_seq_match(ptr, pattern, pat_size, st.st_size);

END:
    errno = saved_errno; 
    close(fd);
    if (ptr != MAP_FAILED && ptr != NULL) munmap(ptr, st.st_size);

    return ret;
}  

int bit_seq_match(unsigned char* buffer, long long pattern, size_t pSize, size_t fSize){
  size_t pos = 0;
  while (fSize - pos >= pSize){
    if (memcmp(&buffer[pos], &pattern, pSize) == 0){
      return 1;
    } else {
      pos++;
    }
  }
  
  return 0;
  
}

