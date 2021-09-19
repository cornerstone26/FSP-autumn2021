//
// Read file byte by byte.
//
// (c) Alexei Guirik, 2021
// This source is licensed under CC BY-NC 4.0
// (https://creativecommons.org/licenses/by-nc/4.0/)
//

#include <errno.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <file_to_read>\n", argv[0]);
        return 1;
    }
    
    FILE *f = fopen(argv[1], "r");
    if (!f) {
        fprintf(stderr, "ERROR: Failed to open %s: %s\n", argv[1], strerror(errno));
        return 1;
    }
    
    size_t offset = 0;
    int ch;
    while ((ch = fgetc(f)) != EOF) {
        fprintf(stdout, "%03ld: %X %c\n", offset, (int)ch, ch);
        offset++;
    }
    
    fclose(f);
    
    return 0;
}
