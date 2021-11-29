// Call plugin_process_file() from given .so
//
// Compile with:
// gcc -o lab1call lab1call.c -ldl
//
// (c) Alexei Guirik, 2021
// This source is licensed under CC BY-NC 4.0 
// (https://creativecommons.org/licenses/by-nc/4.0/)
//  

#include <errno.h>
#include <stdlib.h>
#include <limits.h>
#include <stdio.h>
#include <string.h>
#include <dlfcn.h>
#include <unistd.h>
#include <dirent.h>
#include <ctype.h>
#include "plugin_api.h"


int main(int argc, char *argv[]) {
    // short option
    int aflag = 0;
    int oflag = 0;
    int nflag = 0;
    int vflag = 0;
    int hflag = 0;
    char *pvalue = NULL;
    int short_opt;
    
    opterr = 0;

    while ((short_opt = getopt (argc, argv, "AONvhP:")) != -1)
        switch (short_opt)
        {
        case 'A':
            aflag = 1;
            break;
        case 'O':
            oflag = 1;
            break;
        case 'N':
            nflag = 1;
            break;
        case 'v':
            vflag = 1;
            break;
        case 'h':
            hflag = 1;
            break;    
        case 'P':
            pvalue = optarg;
            break;
        case '?':
            if (optopt == 'P')
            fprintf (stderr, "Option -%c requires an argument.\n", optopt);
            return -1;
        default:
            fprintf (stderr, "Unknown option\n");
            return -1;
        }

    printf ("aflag = %d, oflag = %d, nflag = %d, vflag = %d, hflag = %d, pvalue = %s\n",
            aflag, oflag, nflag, vflag, hflag, pvalue);

    int opts_to_pass_len = 0;
    struct option *opts_to_pass = NULL;
    struct option *longopts = NULL;
    
    // Minimum number of arguments is 3: 
    // $ program_name lib_name --opt1 file_to_check
    if (argc < 3) {        
        fprintf(stdout, "Usage: lab1nhhN3249 /path/to/libnhhN3249.so --option [value] \n");
        return 0;
    }
    
    // Name of the lib. Should be passed as the first argument.
    char *lib_name = strdup(argv[1]); 
    char *folder = malloc(sizeof(char)*PATH_MAX);
    char *file_name = NULL;
    struct plugin_info pi = {0};
    int ret;
    void *dl = dlopen(lib_name, RTLD_LAZY);
    if (!dl) {
        fprintf(stderr, "ERROR: dlopen() failed: %s\n", dlerror());
        goto END;
    }


    // for (index = optind; index < argc; index++)
    //     printf ("Non-option argument %s\n", argv[index]);
     // Check for plugin_get_info() func
    void *func = dlsym(dl, "plugin_get_info");
    if (!func) {
        fprintf(stderr, "ERROR: dlsym() failed: %s\n", dlerror());
        goto END;
    }
    typedef int (*pgi_func_t)(struct plugin_info*);     //
    pgi_func_t pgi_func = (pgi_func_t)func;            

    ret = pgi_func(&pi);

    if (vflag){   
        if (ret < 0) {        
            fprintf(stderr, "ERROR: plugin_get_info() failed\n");
            goto END;
        }
        // Plugin info       
        fprintf(stdout, "Plugin purpose:\t\t%s\n", pi.plugin_purpose);
        fprintf(stdout, "Plugin author:\t\t%s\n", pi.plugin_author);
        fprintf(stdout, "Supported options: ");
        if (pi.sup_opts_len > 0) {
            fprintf(stdout, "\n");
            for (size_t i = 0; i < pi.sup_opts_len; i++) {
                fprintf(stdout, "\t--%s\t\t%s\n", pi.sup_opts[i].opt.name, pi.sup_opts[i].opt_descr);
            }
        }
        else {
            fprintf(stdout, "none (!?)\n");
        }
        fprintf(stdout, "\n");
        return 1;
    }    

    if (hflag){
        fprintf(stdout, "\n\t-P dir\t\t Plugin directory.\
                    \n\t-A\t\t Combine plugin options using AND operation (default).\
                    \n\t-O\t\t Combine plugin options using OR operation.\
                    \n\t-N\t\t Inverting the search condition (after combining options plugins with -A or -O).\
                    \n\t-v\t\t Display the version of the program and information about the program (full name performer, group number, laboratory version number).\
                    \n\t-h\t\t Display help for options.\n");
        if (pi.sup_opts_len > 0) {
            fprintf(stdout, "\n");
            for (size_t i = 0; i < pi.sup_opts_len; i++) {
                fprintf(stdout, "\t--%s\t\t%s\n", pi.sup_opts[i].opt.name, pi.sup_opts[i].opt_descr);
            }
        }
        else {
            fprintf(stdout, "none (!?)\n");
            return -1;
        }
        return 1;
    }

    if (pvalue != NULL){
        folder = pvalue;
    } else {
        if (getcwd(folder, PATH_MAX) != NULL) {
            printf("Current working dir: %s\n", folder);
        } else {
            perror("getcwd() error");
            goto END;
        }
    }

    if (aflag){
        if (nflag){

        }
    } else if (oflag) {
        if (nflag){

        }

    }
        
    // If library supports no options then we have to stop
    if (pi.sup_opts_len == 0) {
        fprintf(stderr, "ERROR: library supports no options! How so?\n");
        goto END;
    }

    // Get pointer to plugin_process_file()
    func = dlsym(dl, "plugin_process_file");
    if (!func) {
        fprintf(stderr, "ERROR: no plugin_process_file() function found\n");
        goto END;
    }
    
    typedef int (*ppf_func_t)(const char*, struct option*, size_t); 
    ppf_func_t ppf_func = (ppf_func_t)func;                        
   
    // Prepare array of options for getopt_long
    longopts = calloc(pi.sup_opts_len + 1, sizeof(struct option));  // cấp phát bộ nhớ động
    if (!longopts) {
        fprintf(stderr, "ERROR: calloc() failed: %s\n", strerror(errno));
        goto END;
    }
    
    // Copy option information
    for (size_t i = 0; i < pi.sup_opts_len; i++) {
        // Mind this!
        // getopt_long() requires array of struct option in its longopts arg,
        // but pi.sup_opts is array of plugin_option structs, not option structs.
        memcpy(longopts + i, &pi.sup_opts[i].opt, sizeof(struct option));   // Gán giá trị của các option vào mảng
    }
    
    // Prepare array of actually used options that will be passed to 
    // plugin_process_file() (Maximum pi.sup_opts_len options)
    opts_to_pass = calloc(pi.sup_opts_len, sizeof(struct option));  //cấp phát bộ nhớ
    if (!opts_to_pass) {
        fprintf(stderr, "ERROR: calloc() failed: %s\n", strerror(errno));
        goto END;
    }

    /*
    for (size_t i = 0; i < pi.sup_opts_len; i++) {
        fprintf(stderr, "DEBUG: to getopt(): passing option '%s'\n",
            (longopts + i)->name);
    }
    */
    
    // Now process options for the lib
    while (1) {
        int opt_ind = 0;
        ret = getopt_long(argc, argv, "", longopts, &opt_ind);
        fprintf(stdout, "%d\n", ret);
        if (ret == -1) break;
        if (ret != 0) {
            fprintf(stderr, "ERROR: failed to parse options\n");
            goto END;
        }

#ifndef ALLOW_OPT_ABBREV
        // glibc quirk: no proper way to disable option abbreviations
        // https://stackoverflow.com/questions/5182041/turn-off-abbreviation-in-getopt-long-optarg-h
        int idx = (longopts + opt_ind)->has_arg ? 2 : 1;
        const char *actual_opt_name = argv[optind - idx] + 2; // +2 for -- before option
        const char *found_opt_name = (longopts + opt_ind)->name;
        if (strcmp(actual_opt_name, found_opt_name)) {
            // It's probably abbreviated name, which we do not allow
            fprintf(stderr, "ERROR: unknown option: %s\n", argv[optind - idx]);
            goto END;
        }
#endif
        
        // Check how many options we got up to this moment
        if ((size_t)opts_to_pass_len == pi.sup_opts_len) {
            fprintf(stderr, "ERROR: too many options!\n");
            goto END;
        }

        // Add this option to array of options actually passed to plugin_process_file()
        memcpy(opts_to_pass + opts_to_pass_len, longopts + opt_ind, sizeof(struct option));
        // Argument (if any) is passed in flag
        if ((longopts + opt_ind)->has_arg) {
            // Mind this!
            // flag is of type int*, but we are passing char* here (it's ok to do so).
            (opts_to_pass + opts_to_pass_len)->flag = (int*)strdup(optarg);
        }
        opts_to_pass_len++;        
    }

    if (getenv("LAB1DEBUG")) {
        fprintf(stderr, "DEBUG: opts_to_pass_len = %d\n", opts_to_pass_len);
        for (int i = 0; i < opts_to_pass_len; i++) {
            fprintf(stderr, "DEBUG: passing option '%s' with arg '%s'\n",
                (opts_to_pass + i)->name,
                (char*)(opts_to_pass + i)->flag);
        }
    }
    
    // get folder
    //default
    
    // if option -P found
    

    // open folder 
    DIR *dir;
    struct dirent *ent;
    if ((dir = opendir (folder)) != NULL) {
        /* Parse all the files within directory */
        while ((ent = readdir (dir)) != NULL) {
            if (ent->d_type == DT_REG){
                 printf ("**************%s\n", ent->d_name);
                file_name = realpath(ent->d_name, NULL);
                // Call plugin_process_file()
                errno = 0;
                ret = ppf_func(file_name, opts_to_pass, opts_to_pass_len);
                //fprintf(stdout, "%ls\n",opts_to_pass->flag);
                fprintf(stdout, "plugin_process_file() returned %d\n", ret);
                if (ret < 0) {
                    fprintf(stdout, "Error information: %s\n", strerror(errno));
                }
            }
        }
        closedir (dir);
        } else {
        // could not open directory
        perror ("");
        return EXIT_FAILURE;
    }
   
    END:
    if (opts_to_pass) {
        for (int i = 0; i < opts_to_pass_len; i++)
            free( (opts_to_pass + i)->flag );
        free(opts_to_pass);
    }
    if (longopts) free(longopts);
    if (lib_name) free(lib_name);
    if (file_name) free(file_name);
    if (dl) dlclose(dl);

    return 0;
}
