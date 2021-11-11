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
#include "plugin_api.h"

static char *g_lib_name = "libnhhN3249.so";

static char *g_plugin_purpose = "Search for files containing a given sequence of bits.";

static char *g_plugin_author = "Nguyen Hong Hanh";

#define OPT_BIT_SEQ "bit-seq"

int bit_seq_match(FILE *pFile, long long pattern, size_t pSize);
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
        "bit-seq"
    }
    
};

static int g_po_arr_len = sizeof(g_po_arr)/sizeof(g_po_arr[0]); // lấy kích thước của mảng g_po_arr

int plugin_get_info(struct plugin_info* ppi) {  // in thông tin -v
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

int plugin_process_file(const char *fname,  // tên file truyền vào
        struct option in_opts[],            // option
        size_t in_opts_len) {               // kích thước của option

    // Return error by default
    int ret = -1;                           // mặc định là nếu lỗi thì trả về -1
    
    // Pointer to file mapping

    //unsigned char *ptr = NULL;          
    
    char *DEBUG = getenv("LAB1DEBUG");      // biến môi trường
    
    if (!fname || !in_opts || !in_opts_len) {   // thiếu tham số truyền vào
        errno = EINVAL;                         //
        return -1;
    }
    
    if (DEBUG) {
        for (size_t i = 0; i < in_opts_len; i++) {
            fprintf(stderr, "DEBUG: %s: Got option '%s' with arg '%s'\n",
                g_lib_name, in_opts[i].name, (char*)in_opts[i].flag);
        }
    }
  
    char* pat = NULL;
    size_t pat_size = 0;
    int got_bit_seq = 0;
    long long pattern = 0;
    for (size_t i = 0; i < in_opts_len; i++) {
        if (!strcmp(in_opts[i].name, OPT_BIT_SEQ)) {
          pat = (char*) in_opts[i].flag;
        } 
        char *endptr = NULL;
        int base = 0; //default in base 10 
        if ((!memcmp(pat, "0x", 2)) || (!memcmp(pat, "0X", 2))){
          base = 16;
          pat_size = strlen(pat)/4 + 1;
        } else if ((!memcmp(pat, "0b", 2)) || (!memcmp(pat, "0B", 2))){
          base = 2;
          pat = &pat[2]; // remove "0b"/"0B"
          pat_size = strlen(pat)/8 + 1;
        } 
        pattern = strtoll(pat, &endptr, base);
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

         //in ra đối số truyền vào
        if (DEBUG) {
            fprintf(stderr, "DEBUG: %s: Inputs: bit_seq = %s\n",
                g_lib_name, pat);
        }

        sprintf(pat, "%x", pattern);
        printf("pattern in binary %s\n", pat);
    }
    
    // nếu got_entropy = 0 thì in ra lỗi
    if (!got_bit_seq) {
        if (DEBUG) {
            fprintf(stderr, "DEBUG: %s: Bit sequence value was not supplied.\n",
                g_lib_name);
        }
        errno = EINVAL;
        return -1;
    }
    
    int saved_errno = 0;    //khai báo lỗi
    FILE *pFile = fopen(fname, "r");

    int res = bit_seq_match(pFile, pattern, 1);
    if (res == 0){
        ret = 1;
        printf("%s\n", realpath(fname, NULL));
    }

    errno = saved_errno;
    return ret;
}  
int bit_seq_match(FILE *pFile, long long pattern, size_t pSize){
  size_t fSize;
  char * buffer;
  size_t result;

  printf("%lld %ld\n", pattern, pSize);
  if (pFile==NULL) {
    fputs ("File error",stderr); 
    exit (1);
    }

  // obtain file size:
  fseek (pFile , 0 , SEEK_END);
  fSize = ftell (pFile);
  rewind (pFile);

  // allocate memory to contain the whole file:
  buffer = (char*) malloc (sizeof(char) *fSize);
  if (buffer == NULL) {
    fputs ("Memory error",stderr); 
    exit (2);
    }

  // copy the file into the buffer:
  result = fread (buffer,1,fSize,pFile);
  if (result != fSize) {
    fputs ("Reading error",stderr); 
    exit (3);
    }
  /* the whole file is now loaded in the memory buffer. */

  // search for bit pattern in file
  size_t pos = 0;
  while (pos < fSize){
    if (memcmp(&buffer[pos], &pattern, pSize) == 0){
      //printf("matched in %ld\n", pos);
      return 0;
    } else {
      //printf("not found in %ld %d \n", pos, buffer[pos]);
      pos++;
    }
  }
     
  fclose (pFile);
  free (buffer);
  return 1;
  
}

