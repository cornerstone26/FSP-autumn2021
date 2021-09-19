//
// Show directory tree with nftw().
//
// (c) Alexei Guirik, 2021
// This source is licensed under CC BY-NC 4.0
// (https://creativecommons.org/licenses/by-nc/4.0/)
//

#define _XOPEN_SOURCE 500   // for nftw()

#include <stdlib.h>
#include <stdio.h>
#include <ftw.h>


int walk_func(const char*, const struct stat*, int, struct FTW*);


int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <start_dir>\n", argv[0]);
        return 0;
    }
    
    int res = nftw(argv[1], walk_func, 10, FTW_PHYS);
    if (res < 0) {
        perror("nftw");
        return 1;
    }

    return 0;
}

int walk_func(const char *fpath, const struct stat *sb, 
        int typeflag, struct FTW *ftwbuf) {

    /*
    // Simple list
    fprintf(stdout, "%s\n", fpath);
    */
    
    /*
    // Indented list
    char indent[128] = {0};
    memset(indent, '\t', ftwbuf->level);
    fprintf(stdout, "%s%s\n", indent, fpath);
    */
    
    
    // Colored indented list
    // Learn more about ANSI Escape codes:
    // https://en.wikipedia.org/wiki/ANSI_escape_code
    // https://ru.wikipedia.org/wiki/Управляющие_последовательности_ANSI
    char indent[128] = {0};
    memset(indent, '\t', ftwbuf->level);
    char start_esc[10] = {0}, end_esc[10] = {0};
    
    if (typeflag == FTW_D) {        
        sprintf(start_esc, "%s", "\33[32m");
        sprintf(end_esc, "%s", "\33[0m");
    }
    else if (typeflag == FTW_DNR) {
        sprintf(start_esc, "%s", "\33[31m");
        sprintf(end_esc, "%s", "\33[0m");
    }
    else if (typeflag == FTW_SL) {
        sprintf(start_esc, "%s", "\33[34m");
        sprintf(end_esc, "%s", "\33[0m");
    }
    else if (typeflag == FTW_F) {
        sprintf(start_esc, "%s", "\33[0m");
        sprintf(end_esc, "%s", "\33[0m");
    }
    
    fprintf(stdout, "%s%s%s%s\n", start_esc, indent, fpath, end_esc);
    
    
    return 0;
}

