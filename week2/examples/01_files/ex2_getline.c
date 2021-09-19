//
// Read file line by line.
//
// (c) Alexei Guirik, 2021
// This source is licensed under CC BY-NC 4.0
// (https://creativecommons.org/licenses/by-nc/4.0/)
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
       fprintf(stderr, "Usage: %s <file_to_read>\n", argv[0]);
       return 1;
    }

    FILE *fp = fopen(argv[1], "r");
    if (!fp) {
       perror("fopen");
       return 1;
    }

    char *buf = NULL;
    size_t buf_len = 0;    
    ssize_t nread;
    size_t lineno = 0;
    while ((nread = getline(&buf, &buf_len, fp)) != -1) {
       fprintf(stdout, "%02ld [%03ld] %03zu: %s", 
            lineno, malloc_usable_size(buf), nread, buf);
       lineno++;
    }

    free(buf);
    fclose(fp);
    
    return 0;
}
