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

static char *g_plugin_purpose = "Search for files containing a given sequence of bits.";

static char *g_plugin_author = "Nguyen Hong Hanh";

#define OPT_BIT_SEQ "bit-seq"
#define OPT_FOLDER "P"
#define OPT_INFOR "v"


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
    },
    {
        {
          OPT_FOLDER,
          required_argument,
          0, 0,
        },
        "P"
    },
    {
        {
          OPT_INFOR,
          0,
          0, 0,
        },
        "v" 

    },
    
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
          printf("***%s***\n", pat);
          pat = &pat[2]; //remove 0x/0X
          printf("***%s***\n", pat);
          pat_size = ceil(strlen(pat)/4.0);
        } else if (base == 2){
          pat_size = ceil(strlen(pat)/8.0);
        } else {
          sprintf(pat, "%llx", pattern);
          pat_size = ceil(strlen(pat)/4.0);
        }
      
        //printf("pattern %s \n", pat);
    }
    
    if (!got_bit_seq) {
        if (DEBUG) {
            fprintf(stderr, "DEBUG: %s: Bit sequence value was not supplied.\n",
                g_lib_name);
        }
        errno = EINVAL;
        return -1;
    }
    
    int saved_errno = 0;    //khai báo lỗi
    FILE *pFile = fopen(fname, "rb");

    int res = bit_seq_match(pFile, pattern, pat_size);
    if (res == 0){
        ret = 1;
        fprintf(stdout, "%s\n", realpath(fname, NULL));
    } else if (res == 1){
      fprintf(stdout, "Not found\n");
      ret = 1;
    } 

    errno = saved_errno;
    return ret;
}  

int bit_seq_match(FILE *pFile, long long pattern, size_t pSize){
  size_t fSize;
  char * buffer;
  size_t result;

  
  if (pFile==NULL) {
    fputs ("File error",stderr); 
    exit (1);
    }

  // obtain file size:
  fseek (pFile , 0 , SEEK_END);
  fSize = ftell (pFile);
  rewind (pFile);
  printf("%lld %ld %ld\n", pattern, pSize, fSize);
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
  
  //fprintf(stdout, "%x\n", buffer);
  size_t pos = 0;
  while (fSize - pos >= pSize){
    if (memcmp(&buffer[pos], &pattern, pSize) == 0){
      //fprintf(stdout, "matched in %ld %x\n", pos, buffer[pos]);
      return 0;
    } else {
      //fprintf(stdout, "not found in %ld %x \n", pos, buffer[pos]);
      pos++;
    }
  }
     
  fclose (pFile);
  free (buffer);
  return 1;
  
}

