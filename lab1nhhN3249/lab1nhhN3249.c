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
 
    // Minimum number of arguments is 4: 
    // $ program_name lib_name --opt1 file_to_check
    if (argc < 3) {        
        fprintf(stdout, "Usage: lab1nhhN3249 /path/to/libnhhN3249.so --option [value] \n");
        return 0;
    }
    // Name of the lib. Should be passed as the first argument.
    //char *lib_name = strdup(argv[1]); 
    
    // short option
    int aflag = 0;
    int oflag = 0;
    int nflag = 0;
    int vflag = 0;
    int hflag = 0;
    char *pvalue = NULL;
    int short_opt;
    
    int need_lib2 = 0;
    opterr = 0;

    while ((short_opt = getopt (argc, argv, "-AONvhP:")) != -1){
        //printf("short_opt = %c\n", (char) short_opt);
    
        switch (short_opt)
        {
        case 'A':
            aflag = 1;
            need_lib2 = 1;
            break;
        case 'O':
            oflag = 1;
            need_lib2 = 1;
            break;
        case 'N':
            nflag = 1;
            need_lib2 = 1;
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
            if (optopt == 'P'){
                fprintf (stderr, "Option -%c requires an argument.\n", optopt);
                return -1;
            } else {
                break;
            }              
        }
    }

    printf ("aflag = %d, oflag = %d, nflag = %d, vflag = %d, hflag = %d, pvalue = %s\n",
            aflag, oflag, nflag, vflag, hflag, pvalue);

    // debug
    for (int i = 0; i < argc; i++){
        printf("%s ", argv[i]);
    }
    printf("\n");

    int num_iter = need_lib2 + 1;

    //first lib
    int opts_to_pass_len = 0;
    struct option *opts_to_pass = NULL;
    struct option *longopts = NULL;
    
    //second lib

    int opts_to_pass2_len = 0;
    struct option *opts_to_pass2 = NULL;
    struct option *longopts2 = NULL;
    
    char **lib_name = (char**)malloc(num_iter * sizeof(char*)); //NULL;
    for (int i = 0; i < num_iter; ++i){
        lib_name[i] = (char*)malloc(sizeof(argv[i + 1]));
    }

    char *folder = malloc(sizeof(char)*PATH_MAX);
    char *file_name = malloc(sizeof(char)*PATH_MAX);
    struct plugin_info pi[2];
    int ret;
    void *dl[2] = {NULL, NULL};

    void *func = NULL;
    void *func1 = NULL;

    typedef int (*pgi_func_t)(struct plugin_info*);
    pgi_func_t pgi_func = NULL;
    typedef int (*ppf_func_t)(const char*, struct option*, size_t);

    if ((((argv[1][0] != '.') && (argv[1][1] != '/')) || ((argv[2][0] != '.') && (argv[2][1] != '/'))) && (need_lib2 == 1)){
        fprintf(stderr, "ERROR: Short option is entered, but only one dynamic library (*.so) exists:\n");
        goto END;
    }

    for (int i = 0; i < num_iter; ++i){
        // Name of the lib. Should be passed as the first argument.
        lib_name[i] = strdup(argv[i + 1]);

        if (!dlopen(lib_name[i], RTLD_LAZY)){
            fprintf(stderr, "ERROR: dlopen() failed: %s\n", dlerror());
            goto END;
        } else {
            dl[i] = dlopen(lib_name[i], RTLD_LAZY);
            fprintf(stdout, " %s\n", lib_name[i]);
        }

        // Check for plugin_get_info() func
        if (!dlsym(dl[i], "plugin_get_info")) {
            fprintf(stderr, "ERROR: dlsym() failed: %s\n", dlerror());
            goto END;
        } else {
            if (i == 0){
                func = dlsym(dl[i], "plugin_get_info");
                pgi_func = (pgi_func_t)func;
            }   
            else if (i == 1){
                func1 = dlsym(dl[i], "plugin_get_info");
                pgi_func = (pgi_func_t)func1;
            }
        }

        ret = pgi_func(&pi[i]);
        if (ret < 0) {        
            fprintf(stderr, "ERROR: plugin_get_info() failed\n");
            goto END;
        }
       
        // If library supports no options then we have to stop
        if (pi[i].sup_opts_len == 0) {
            fprintf(stderr, "ERROR: library supports no options! How so?\n");
            goto END;
        }


        // Get pointer to plugin_process_file()
        if (i == 0)///!!!!!!!!!!!!!!!!!!!!
        {
            func = dlsym(dl[i], "plugin_process_file");
            pgi_func = (pgi_func_t)func;
        }   
        else if (i == 1)
        {
            func1 = dlsym(dl[i], "plugin_process_file");
          pgi_func = (pgi_func_t)func1;
        }       
            
    
        // Prepare array of options for getopt_long
        
        // Copy option information
        if (i == 0){
            longopts = calloc(pi[i].sup_opts_len + 1, sizeof(struct option));
            if (!longopts) {
                fprintf(stderr, "ERROR: calloc() failed: %s\n", strerror(errno));
                goto END;
            }
            for (size_t j = 0; j < pi[i].sup_opts_len; j++) {
                // Mind this!
                // getopt_long() requires array of struct option in its longopts arg,
                // but pi.sup_opts is array of plugin_option structs, not option structs.
                memcpy(longopts + j, &pi[i].sup_opts[j].opt, sizeof(struct option));
            }
        } else if (i == 1){
            longopts2 = calloc(pi[i].sup_opts_len + 1, sizeof(struct option));
            if (!longopts2) {
                fprintf(stderr, "ERROR: calloc() failed: %s\n", strerror(errno));
                goto END;
            }
            for (size_t j = 0; j < pi[i].sup_opts_len; j++) {
                    // Mind this!
                    // getopt_long() requires array of struct option in its longopts arg,
                    // but pi.sup_opts is array of plugin_option structs, not option structs.
                    memcpy(longopts2 + j, &pi[i].sup_opts[j].opt, sizeof(struct option));
            }
        }
        // Prepare array of actually used options that will be passed to 
        // plugin_process_file() (Maximum pi.sup_opts_len options)
        if (i == 0)
        {
            opts_to_pass = calloc(pi[i].sup_opts_len, sizeof(struct option));
            if (!opts_to_pass) {
                fprintf(stderr, "ERROR: calloc() failed: %s\n", strerror(errno));
                goto END;
            }
        }
        else if (i == 1)
        {
            opts_to_pass2 = calloc(pi[i].sup_opts_len, sizeof(struct option));
            if (!opts_to_pass2) {
                fprintf(stderr, "ERROR: calloc() failed: %s\n", strerror(errno));
                goto END;
            }
        }
        fprintf(stdout, "\n");
    }

    if (vflag){   
        if (ret < 0) {        
            fprintf(stderr, "ERROR: plugin_get_info() failed\n");
            goto END;
        }
        // Plugin info       
        fprintf(stdout, "Plugin purpose:\t\t%s\n", pi[0].plugin_purpose);
        fprintf(stdout, "Plugin author:\t\t%s\n", pi[0].plugin_author);
        fprintf(stdout, "Supported options: ");
        if (pi[0].sup_opts_len > 0) {
            fprintf(stdout, "\n");
            for (size_t i = 0; i < pi[0].sup_opts_len; i++) {
                fprintf(stdout, "\t--%s\t\t%s\n", pi[0].sup_opts[i].opt.name, pi[0].sup_opts[i].opt_descr);
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
        if (pi[0].sup_opts_len > 0) {
            fprintf(stdout, "\n");
            for (size_t i = 0; i < pi[0].sup_opts_len; i++) {
                fprintf(stdout, "\t--%s\t\t%s\n", pi[0].sup_opts[i].opt.name, pi[0].sup_opts[i].opt_descr);
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

    printf("Here\n");
    // Now process options for the lib
    opterr = 1;

    if (need_lib2 == 0)
    {
        optind = 0;
    }
    int ret2 = -1;
    int lib_number = -1;
    int opt_count = 0, opt_count2 = 0;
    char* opt_arg1 = NULL;
    int val_len_opts2 = 0;
    if (need_lib2 == 1){
        val_len_opts2 = pi[1].sup_opts_len;
    }
    while (1) {
        int opt_ind = 0;
        ret = getopt_long_only(argc, argv, "", longopts, &opt_ind);
        printf("ret = %d\n", ret);
        if (ret == 0){
            ret2 = 0;
            opt_arg1 = optarg;
            opt_count++;
            lib_number = 1;

            printf("opt_arg1: %s\t opt_count: %d\t lib_num: %d\n", opt_arg1, opt_count, lib_number);
        }
    
        if ((ret == '?') && (ret2 != -1)){
            memcpy(longopts + opt_ind, (int*)opt_arg1, sizeof(&opt_arg1));
            optind--;
            ret2 = getopt_long_only(argc, argv, "", longopts2, &opt_ind);

            if (ret2 == 0){
                ret = 0;
                opt_count2++;
                lib_number = 2;
            }
        }

        if (ret == -1) break;
            
        if (ret != 0) {
            fprintf(stderr, "ERROR: failed to parse options\n");
            goto END;
        }

/*
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
*/
        
        // Check how many options we got up to this moment
        if ((size_t)opts_to_pass_len == pi[0].sup_opts_len + val_len_opts2) {
            fprintf(stderr, "ERROR: too many options!\n");
            goto END;
        }

        if (lib_number == 1){
            // Add this option to array of options actually passed to plugin_process_file()
            memcpy(opts_to_pass + opts_to_pass_len, longopts + opt_ind, sizeof(struct option));
            // Argument (if any) is passed in flag
            if ((longopts + opt_ind)->has_arg) {
            // Mind this!
            // flag is of type int*, but we are passing char* here (it's ok to do so).
            (opts_to_pass + opts_to_pass_len)->flag = (int*)strdup(optarg);
            }

            opts_to_pass_len++;     

        } else if (need_lib2 == 1) {
            memcpy(opts_to_pass2 + opts_to_pass2_len, longopts2 + (opt_count2 - 1), sizeof(struct option));

            // Argument (if any) is passed in flag
            if ((longopts2 + (opt_count2 - 1))->has_arg) {
                // Mind this!
                // flag is of type int*, but we are passing char* here (it's ok to do so). 
                (opts_to_pass2 + opts_to_pass2_len)->flag = (int*)strdup(optarg);
            }

            opts_to_pass2_len++;

        }
    }
     
    

    if (getenv("LAB1DEBUG")) {
        fprintf(stderr, "DEBUG: opts_to_pass_len = %d %d\n", opts_to_pass_len, opts_to_pass2_len);
        for (int i = 0; i < opts_to_pass_len; i++) {
            fprintf(stderr, "DEBUG: passing option '%s' with arg '%s'\n",
                (opts_to_pass + i)->name,
                (char*)(opts_to_pass + i)->flag);
        }

        if (need_lib2 == 1){
            for (int i = 0; i < opts_to_pass2_len; i++) {
                fprintf(stderr, "DEBUG: passing option '%s' with arg '%s'\n",
                    (opts_to_pass2 + i)->name,
                    (char*)(opts_to_pass2 + i)->flag);
            }
        }
    }
    
    // open folder 
    DIR *dir;
    struct dirent *ent;
    if ((dir = opendir (folder)) != NULL) {
        printf ("Folder opened\n");
        /* Parse all the files within directory */
        while ((ent = readdir (dir)) != NULL) {
            if (ent->d_type != DT_DIR){
                 printf ("**************%s\n", ent->d_name);
                if ((file_name = realpath(ent->d_name, NULL)) != NULL){
                    // Call plugin_process_file()
                    errno = 0;
                    ppf_func_t ppf_func = (ppf_func_t)func;
                    ret = ppf_func(file_name, opts_to_pass, opts_to_pass_len);
                    //fprintf(stdout, "%ls\n",opts_to_pass->flag);
                    fprintf(stdout, "plugin_process_file() returned %d\n", ret);
                    if (ret < 0) {
                        fprintf(stdout, "Error information: %s\n", strerror(errno));
                    } 
                    
                    if (need_lib2 == 1){
                        ppf_func_t ppf_func1 = (ppf_func_t)func1;
                        ret2 = ppf_func1(file_name, opts_to_pass2, opts_to_pass2_len);
                        fprintf(stdout, "plugin_process_file() (lib2) returned %d\n", ret2);
                        if (ret2 < 0) {
                            fprintf(stdout, "Error information: %s\n", strerror(errno));
                        }
                    }

                    if (aflag){
                        if (nflag){
                            if (!(ret == 1 && ret2 == 1)){
                                fprintf(stdout, "%s\n",file_name);
                            }
                            continue;
                        } else {
                            if (ret == 1 && ret2 == 1){
                                fprintf(stdout, "%s\n",file_name);
                            }
                            continue;
                        }
                    }

                    if (oflag){
                        if (nflag){
                            if (!(ret == 1 || ret2 == 1)){
                                fprintf(stdout, "%s\n",file_name);
                            }
                            continue;
                        } else {
                            if (ret == 1 || ret2 == 1){
                                fprintf(stdout, "%s\n",file_name);
                            }
                            continue;
                        }
                    }

                    if (ret == 1){
                        fprintf(stdout, "%s\n",file_name);
                    }                
                } else {
                    perror("Error when get absolute path file: ");
                    goto END;
                }
                
            }
        }
        closedir (dir);
    } else {
        // could not open directory
        perror ("Error when open directory: ");
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
    if (folder) free(folder);
    if (file_name) free(file_name);
    return 0;
}
