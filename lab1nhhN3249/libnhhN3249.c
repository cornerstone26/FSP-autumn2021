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


static char *g_lib_name = "libnhhN3249.so";

static char *g_plugin_purpose = "Search for files containing a given sequence of bits.";

static char *g_plugin_author = "Nguyen Hong Hanh";

#define OPT_ENTROPY_STR "entropy"
#define OPT_OFFSET_FROM_STR "offset-from"
#define OPT_OFFSET_TO_STR "offset-to"

static struct plugin_option g_po_arr[] = {
/*
    struct plugin_option { // cấu trúc option
        struct option {
           const char *name;    // tên option
           int         has_arg; // cần đối số/ không cần đối số
           int        *flag;    // cờ 
           int         val;     // trả về kết quả của flags
        } opt,
        char *opt_descr
    }
*/
    { // tên từng option
        {
            OPT_ENTROPY_STR,
            required_argument,
            0, 0,
        },
        "Target value of entropy"
    }    
};

static int g_po_arr_len = sizeof(g_po_arr)/sizeof(g_po_arr[0]); // lấy kích thước của mảng g_po_arr

//
//  Private functions
//
static double calculate_entropy(unsigned char*, size_t, size_t);

//
//  API functions
//
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
    unsigned char *ptr = NULL;          
    
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
    
    double entropy = 0.0;
    size_t offset_from = 0, offset_to = 0;
    int got_entropy = 0, got_offset_from = 0, got_offset_to = 0;
// lấy tham số truyền vào
// strod lấy cả phần thập phân, strol chỉ lấy phần nguyên
// endptr khác '\0' nghĩa là truyền vào kí tự
#define OPT_CHECK(opt_var, is_double) \
    if (got_##opt_var) { \
        if (DEBUG) { \
            fprintf(stderr, "DEBUG: %s: Option '%s' was already supplied\n", \
                g_lib_name, in_opts[i].name); \
        } \
        errno = EINVAL; \
        return -1; \
    } \
    else { \
        char *endptr = NULL; \
        opt_var = is_double ? \
                    strtod((char*)in_opts[i].flag, &endptr): \
                    strtol((char*)in_opts[i].flag, &endptr, 0); \
        if (*endptr != '\0') { \
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
        if (!strcmp(in_opts[i].name, OPT_ENTROPY_STR)) {
            OPT_CHECK(entropy, 1)
        }
        else if (!strcmp(in_opts[i].name, OPT_OFFSET_FROM_STR)) {
            OPT_CHECK(offset_from, 0)
        }
        else if (!strcmp(in_opts[i].name, OPT_OFFSET_TO_STR)) {
            OPT_CHECK(offset_to, 0)
        }
        else {
            errno = EINVAL;
            return -1;
        }
    }
    // nếu got_entropy = 0 thì in ra lỗi
    if (!got_entropy) {
        if (DEBUG) {
            fprintf(stderr, "DEBUG: %s: Entropy value was not supplied.\n",
                g_lib_name);
        }
        errno = EINVAL;
        return -1;
    }
    
    // Entropy value should be in [0.0 .. 1.0)
    if (entropy < 0 || entropy > 1.0) {
        if (DEBUG) {
            fprintf(stderr, "DEBUG: %s: Entropy should be in [0.0 .. 1.0)\n",
                g_lib_name);
        }
        errno = ERANGE;
        return -1;
    }
    //in ra đối số truyền vào
    if (DEBUG) {
        fprintf(stderr, "DEBUG: %s: Inputs: entropy = %lf, offset_from = %ld, offset_to = %ld\n",
            g_lib_name, entropy, offset_from, offset_to);
    }
    
    int saved_errno = 0;    //khai báo lỗi
    
    int fd = open(fname, O_RDONLY);     // mở file chỉ đọc
    if (fd < 0) {
        // errno is set by open()
        return -1;
    }
    
    struct stat st = {0};   // fstat()
    int res = fstat(fd, &st);       //lưu địa chỉ của file vào st
    if (res < 0) {
        saved_errno = errno;
        goto END;   // đến END
    }
    
    // Check that size of file is > 0
    if (st.st_size == 0) { // kiểm tra kích thước của file
        if (DEBUG) {
            fprintf(stderr, "DEBUG: %s: File size should be > 0\n",
                g_lib_name);
        }
        saved_errno = ERANGE; // lỗi khoảng giá trị
        goto END;
    }
    
    // Check starting offset
    if (offset_from >= (size_t)st.st_size) {    // vị trí bắt đầu check
        saved_errno = ERANGE;
        goto END;
    }
    
    // Check ending offset
    if (offset_to == 0 || offset_to >= (size_t)st.st_size) { //gàn vị trí kiểm tra cuối cùng cho vị trí cuối cungf của file
        offset_to = st.st_size - 1;
        if (DEBUG) {
            fprintf(stderr, "DEBUG: %s: Corrected offset_to to %ld\n",
                g_lib_name, offset_to);
        }
    }
    
    // Check for incorrect offset values
    if (offset_from >= offset_to) {
        if (DEBUG) {
            fprintf(stderr, "DEBUG: %s: offset_from (%ld) >= offset_to to (%ld)\n",
                g_lib_name, offset_from, offset_to);
        }        
        saved_errno = ERANGE;
        goto END;
    }
    
    
    ptr = mmap(0, st.st_size, PROT_READ, MAP_PRIVATE, fd, 0);   //mở file ra đọc
    if (ptr == MAP_FAILED) { // không thể mở
        saved_errno = errno;
        goto END;
    }
        
    double calc_entropy = 0.0;
    calc_entropy = calculate_entropy(ptr, offset_from, offset_to);
    
    if (DEBUG) {
        fprintf(stderr, "DEBUG: %s: Calculated entropy = %lf\n", 
            g_lib_name, calc_entropy);
    }
    
    // 0 or 1
    ret = calc_entropy >= entropy;
    
    END:
    close(fd);
    if (ptr != MAP_FAILED && ptr != NULL) munmap(ptr, st.st_size);
    
    // Restore errno value
    errno = saved_errno;
    
    return ret;
}        

double calculate_entropy(unsigned char *p, size_t offset_from, size_t offset_to) { 
    size_t freq_table[256] = {0};
    unsigned char *ptr = p + offset_from, *pend = p + offset_to;
    
    while (ptr <= pend) {
        freq_table[ *ptr++ ] += 1;
    }

    size_t total_size = offset_to - offset_from + 1;
    double total_entropy = 0.0;
    
    for (int i = 0; i < 256; i++) {
        double prob = (double)freq_table[i] / total_size;
        if (prob > 0) {
            total_entropy -= prob * log2(prob);
        }
    }
    
    return total_entropy/8;
}
