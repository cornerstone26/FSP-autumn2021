//
// Get current working directory. Has problems with joining paths.
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

int main(int argc, char *argv[]) {
    // Note to self: To get rid of unused variable warning (-Wall -Wextra -Werror)
    (void)argc; // Why?
    
    fprintf(stdout, "Relative program file path: %s\n", argv[0]);
    
    char *cwd = get_current_dir_name();
    if (cwd) {
        fprintf(stdout, "Current dir: %s\n", cwd);

        // How to get the absolute path of the executable?
        int abspath_len = strlen(argv[0]) + strlen(cwd) + 1; // +1 for separator
        char *abspath = malloc(abspath_len + 1);
        sprintf(abspath, "%s/%s", cwd, argv[0]);
        fprintf(stdout, "Absolute program file path: %s\n", abspath);
        free(abspath);
        // Note to self: Show valgrind here
        free(cwd);    
    }
        
    return 0;
}
