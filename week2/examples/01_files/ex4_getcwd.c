//
// Get current working directory. Uses linux-specific tool to get
//  absolute path to executable file. Also shows proper way to
//  use readlink().
//
// (c) Alexei Guirik, 2021
// This source is licensed under CC BY-NC 4.0
// (https://creativecommons.org/licenses/by-nc/4.0/)
//

#define _GNU_SOURCE // for get_current_dir_name()

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <limits.h> // for PATH_MAX

// Beware of PATH_MAX!
// It's tricky: 
// https://stackoverflow.com/questions/9449241/where-is-path-max-defined-in-linux

#if defined(__linux__)        
#define PROC_LINK   "/proc/self/exe"
#elif defined (__FreeBSD__)
#define PROC_LINK   "/proc/curproc/file"
#else
#error "Location of proc self link is unknown"
#endif

int main(int argc, char *argv[]) {
    (void)argc;
    
    fprintf(stdout, "Relative program file path: %s\n", argv[0]);
    
    char *cwd = get_current_dir_name();
    if (cwd) {
        fprintf(stdout, "Current dir: %s\n", cwd);
 
        size_t abspath_len = PATH_MAX;
        char *abspath = calloc(1, abspath_len);
        while (1) {
            ssize_t res = readlink(PROC_LINK, abspath, abspath_len);
            if (res < 0) {
                perror("readlink");
                return 1;
            }            
            if (abspath[ abspath_len - 1] == '\0')
                // abspath_len was enough
                break; 
            else {
                // Path was truncated by readlink, need more space
                free(abspath);
                abspath_len *= 2;
                abspath = calloc(1, abspath_len);
            }
        }
        fprintf(stdout, "Absolute program file path: %s\n", abspath);
        
        free(abspath);
        free(cwd);    
    }
        
    return 0;
}
