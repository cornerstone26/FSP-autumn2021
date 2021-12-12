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

static int count = 0; // number of files found 

typedef int (*pgi_func_t)(struct plugin_info*); //plugin_info
typedef int (*ppf_func_t)(const char*, struct option*, size_t); //plugin_process_file

struct lib{
    size_t opts_to_pass_len;
    struct option *opts_to_pass;
    struct option *longopts;
    size_t longopts_len;

    ppf_func_t ppf_func;
    pgi_func_t pgi_info;
};

static char *folder_working;
static char *folder_lib;
static int need_lib2;
static int count_lib;

static void search_dir(char *folder, struct lib plugin[], int aflag, int oflag, int nflag);
static void get_abs_path(char *path, char *folder, char*file_name);
static int get_lib(char* folder, void* dl[]);
static int is_lib(const char *filename);
static char *get_filename_ext(const char *filename);

void count_so(char* folder){
    char file_name0[255] = "";
    DIR *dir;
    struct dirent *ent;
    if ((dir = opendir (folder)) != NULL) {
        while ((ent = readdir (dir)) != NULL) {
            if (ent->d_type == DT_REG){
                get_abs_path(file_name0, folder, ent->d_name);
                
                if (file_name0 != NULL){
                    if (is_lib(file_name0)){
                        count_lib++;                       
                    }
                } else {
                    perror("Error when getting absolute path file: ");
                    return;
                }    
            }
        }
        closedir (dir);
        return;
    } else {
        perror ("Error opendir(): ");
        return;
    }
    
}

int main(int argc, char *argv[]) {
    struct lib *plugin = 0;
    struct plugin_info *pi = 0;
    void **dl = 0;
    int num_iter = 0;
    char *DEBUG = getenv("LAB1DEBUG");
    // Minimum number of arguments is 3: 
    // $ program_name lib_name --opt1
    if (argc < 2) {        
        fprintf(stdout, "Usage: ./lab1nhhN3249 --options [value] \n");
        return 0;
    }

    // short option
    int aflag = 0;
    int oflag = 0;
    int nflag = 0;
    int vflag = 0;
    int hflag = 0;
    char *pvalue = NULL;
    int short_opt;
    
    need_lib2 = 0;
    opterr = 0;
    while ((short_opt = getopt(argc, argv, "-AONvhP:")) != -1){
        //fprintf(stdout, "short_opt = %c\t optind = %d\n", (char) short_opt, optind);
        switch (short_opt){
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
                need_lib2 = 1;
                break;
            case 'h':
                hflag = 1;
                need_lib2 = 1;
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

        if (optind < argc){
             if (argv[optind][1] == '-'){    // long option is detected 
                goto START;
            }
        }
    }

    START: ;
    if (DEBUG){
        fprintf(stdout,"\nargv[]: ");
        for (int i = 0; i < argc; i++){
            fprintf(stdout, "%s ", argv[i]);
        }
        fprintf(stdout,"\n");
    }
    
    if (DEBUG){
        fprintf(stdout, "\nDEBUG: Short options: aflag = %d, oflag = %d, nflag = %d, vflag = %d, hflag = %d, pvalue = %s\n", aflag, oflag, nflag, vflag, hflag, pvalue);
    }
    
    folder_working = malloc(sizeof(char)*PATH_MAX);
    folder_lib = malloc(sizeof(char)*PATH_MAX);

    if (getcwd(folder_working, PATH_MAX) != NULL) {
        if (DEBUG) fprintf(stdout, "Current working dir: %s\n", folder_working);
    } else {
        if (DEBUG) perror("Error getcwd() : ");
        goto END;
    }
    
    if (pvalue){
        strcpy(folder_lib, pvalue);
    } else {
        strcpy(folder_lib, folder_working);
    }

    count_lib = 0;
    count_so(folder_lib);
    num_iter = count_lib;
    if ((num_iter < 2) && (need_lib2 == 1)){
        fprintf(stderr, "ERROR: The program need more dynamic libraries (*.so)\n");
        goto END;
    }

    dl = calloc(num_iter, sizeof(void*));
    get_lib(folder_lib, dl);

    plugin = calloc(num_iter, sizeof(struct lib));
    int ret;
    pi = calloc(num_iter, sizeof(struct plugin_info));
    for (int i = 0; i < num_iter; ++i){
        
        // Check for plugin_get_info() func
        if (!dlsym(dl[i], "plugin_get_info")) {
            fprintf(stderr, "ERROR: dlsym() failed: %s\n", dlerror());
            goto END;
        } else {
            plugin[i].pgi_info = dlsym(dl[i], "plugin_get_info");
            ret = plugin[i].pgi_info(&pi[i]);
            if (ret < 0){
                fprintf(stderr, "ERROR: plugin_get_info() failed\n");
                goto END;
            }
        }
            
        // If library supports no options then we have to stop
        if (pi[i].sup_opts_len == 0) {
            fprintf(stderr, "ERROR: library supports no options! How so?\n");
            goto END;
        }
        
        // Get pointer to plugin_process_file()
        
        plugin[i].ppf_func = dlsym(dl[i], "plugin_process_file");
        if(!plugin[i].ppf_func) {
            fprintf(stderr, "ERROR: no plugin_process_file() function found\n");
            goto END;
        }
        
        // Prepare array of options for getopt_long
        // Copy option information
        
        plugin[i].longopts = calloc(pi[i].sup_opts_len + 1, sizeof(struct option));
        plugin[i].longopts_len = pi[i].sup_opts_len;
        if (!plugin[i].longopts) {
            fprintf(stderr, "ERROR: calloc() failed: %s\n", strerror(errno));
            goto END;
        }
        for (size_t j = 0; j < pi[i].sup_opts_len; j++) {
            // Mind this!
            // getopt_long() requires array of struct option in its longopts arg,
            // but pi.sup_opts is array of plugin_option structs, not option structs.
            memcpy(&plugin[i].longopts[j], &pi[i].sup_opts[j].opt, sizeof(struct option));
        }
    
        // Prepare array of actually used options that will be passed to 
        // plugin_process_file() (Maximum pi.sup_opts_len options)
       
        plugin[i].opts_to_pass = calloc(pi[i].sup_opts_len, sizeof(struct option));
        if (!plugin[i].opts_to_pass) {
            fprintf(stderr, "ERROR: calloc() failed: %s\n", strerror(errno));
            goto END;
        }
        
    }

    if (vflag){   
        // Plugin info    
        for (int k = 0; k < num_iter; k++){   
            fprintf(stdout, "Plugin %d:\n\n", k);
            fprintf(stdout, "Plugin purpose:\t\t%s\n", pi[k].plugin_purpose);
            fprintf(stdout, "Plugin author:\t\t%s\n", pi[k].plugin_author);
            fprintf(stdout, "Supported options: ");
            if (pi[k].sup_opts_len > 0) {
                fprintf(stdout, "\n");
                for (size_t i = 0; i < pi[k].sup_opts_len; i++) {
                    fprintf(stdout, "\t--%s\t\t%s\n", pi[k].sup_opts[i].opt.name, pi[k].sup_opts[i].opt_descr);
                }
            } else {
                fprintf(stdout, "none (!?)\n");
            }
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
        for (int j = 0; j < num_iter; j++) {
            if (pi[j].sup_opts_len > 0) {
                fprintf(stdout, "\n");
                for (size_t i = 0; i < pi[j].sup_opts_len; i++) {
                    fprintf(stdout, "\t--%s\t\t%s\n", pi[j].sup_opts[i].opt.name, pi[j].sup_opts[i].opt_descr);
                }
            } else {
                fprintf(stdout, "none (!?)\n");
                return -1;
            }
        }
        return 1;
    }

    // Now process options for the lib
    
    opterr = 0;
    for (int i = 0; i < num_iter; i++) {
        optind = 1;
        
        while (1){
            int opt_ind = 0;
            int ret = getopt_long_only(argc, argv, "", plugin[i].longopts, &opt_ind);
            if (ret == -1) break;

            if(ret != 0) continue;

            // Check how many options we got up to this moment
            if ((size_t) plugin[i].opts_to_pass_len == plugin[i].longopts_len){
                fprintf(stderr, "ERROR: too many options!\n");
                goto END;
            }

            // Add this option to array of options actually passed to plugin_process_file()
            memcpy(plugin[i].opts_to_pass + plugin[i].opts_to_pass_len, plugin[i].longopts + opt_ind, sizeof(struct option));

            // Argument (if any) is passed in flag
            if ((plugin[i].longopts + opt_ind)->has_arg) {
                // Mind this!
                // flag is of type int*, but we are passing char* here (it's ok to do so). 
                (plugin[i].opts_to_pass + plugin[i].opts_to_pass_len)->flag = (int*)strdup(optarg);
            }

            plugin[i].opts_to_pass_len++;
        }

    }
     
    if (getenv("LAB1DEBUG")) {
        for (int i = 0; i < num_iter; i++){
            fprintf(stderr, "DEBUG: opts_to_pass_len = %ld\n", plugin[i].opts_to_pass_len);
            for (size_t j = 0; j < plugin[i].opts_to_pass_len; j++) {
            fprintf(stderr, "DEBUG: passing option '%s' with arg '%s'\n",
                (plugin[i].opts_to_pass[j]).name,
                (char*)(plugin[i].opts_to_pass[j]).flag);
            }
        }
    }


    search_dir(folder_working, plugin, aflag, oflag, nflag);
    fprintf(stdout, "\n%d file(s) have been found\n", count);
    
    END:
    if (plugin){
        for (int i = 0; i < num_iter; i++){
            for(size_t j = 0; j < plugin[i].opts_to_pass_len; j++){
                if (plugin[i].opts_to_pass[j].flag) free(plugin[i].opts_to_pass[j].flag);
            }
            if (plugin[i].opts_to_pass) free(plugin[i].opts_to_pass);
            if (plugin[i].longopts) free(plugin[i].longopts);
        }
        free(plugin);
    }

    for (int i = 0; i < num_iter; i++){        
        dlclose(dl[i]);
    }

    if (pi) free(pi);
    if (dl) free (dl);
    if (folder_working) free(folder_working);
    if (folder_lib) free (folder_lib);
    
    return 0;
}

//recursive func
void search_dir(char *folder, struct lib plugin[], int aflag, int oflag, int nflag){
    if (getenv("LAB1DEBUG")) fprintf(stdout, "\n\nWORKING FOLDER %s\n", folder);
    char file_name0[255] = "";

    //char *file_name0 = malloc(sizeof(char)*PATH_MAX*2);
    if (folder == NULL){
        return;
    }
    int ret = 0;
    DIR *dir;
    struct dirent *ent;
    if ((dir = opendir (folder)) != NULL) {
        //printf ("\nFolder opened\n");
        /* Parse all the files within directory */
        while ((ent = readdir (dir)) != NULL) {
            if (ent->d_type == DT_REG){
                get_abs_path(file_name0, folder, ent->d_name);
                if (getenv("LAB1DEBUG")) printf ("\nCHECKING FILE: %s\n", file_name0);
                if (file_name0 != NULL){
                    // Call plugin_process_file()
                    errno = 0;
                    int call = 0;
                    int return_true = 0;

                    for (int i = 0; i < count_lib; i++){
                        if(plugin[i].opts_to_pass_len > 0) {
                            call++ ;
                            ret = plugin[i].ppf_func(file_name0, plugin[i].opts_to_pass, plugin[i].opts_to_pass_len);
                            if (ret == 1){ 
                                return_true++;
                            } 
                            if (getenv("LAB1DEBUG")) fprintf(stdout, "plugin_process_file() returned %d\n", ret);
                            if (ret < 0){
                                if (getenv("LAB1DEBUG")) fprintf(stdout, "Error information: %s\n", strerror(errno));
                                return;
                            }
                        }
                    }
                    if (getenv("LAB1DEBUG")) fprintf(stdout, "\nNumber of calls that return true %d / %d\n", return_true, call);

                
                    if (call){
                        if (aflag == 1 && (return_true == call)){
                            count++;
                            fprintf(stdout, "%s\n",file_name0);
                        } else if (aflag == 1 && nflag == 1 && (return_true < call)){
                            count++;
                            fprintf(stdout, "%s\n",file_name0);
                        } else if (oflag == 1 && return_true > 0){
                            count++;
                            fprintf(stdout, "%s\n",file_name0);
                        } else if (oflag == 1 && nflag == 1 && return_true == 0){
                            count++;
                            fprintf(stdout, "%s\n",file_name0);
                        } else if (aflag == 0 && oflag == 0 && aflag == 0 && return_true == 1){
                            count++;
                            fprintf(stdout, "%s\n",file_name0);
                        }
                    }
                    
                
                } else {
                        perror("Error when getting absolute path file: ");
                        return;

                }
        
            } else if (ent->d_type == DT_DIR){
               
                if ((strcmp(ent->d_name, ".") != 0) && (strcmp(ent->d_name, "..") != 0)){
                    get_abs_path(file_name0, folder, ent->d_name);
                    search_dir(file_name0, plugin, aflag, oflag, nflag);
                }
            }
            
        }

        closedir (dir);
        return;
    } else {
        // could not open directory
        perror ("Error when open directory: ");
        errno = EXIT_FAILURE;
        return;
    } 
    return;  
}    

void get_abs_path (char *path, char *dir, char *fname){
    //char *path = malloc(sizeof(char)*PATH_MAX*2);
    //printf("\nBefore calling func: path %s dir %s fname %s\n", path, dir, fname);
    strcpy(path, dir);
    strcat(path, "/");
    strcat(path, fname);    
    //printf("\npath %s", path);
}

int get_lib(char* folder, void* dl[]){
    char file_name0[255] = "";
    DIR *dir;
    struct dirent *ent;
    int i = 0;
    if ((dir = opendir (folder)) != NULL) {
        /* Parse all the files within directory */
        while ((ent = readdir (dir)) != NULL) {
            if (ent->d_type == DT_REG){
                get_abs_path(file_name0, folder, ent->d_name);
                //printf("\nFILE LIB %s\n", file_name0);
                if (file_name0 != NULL){
                    if (is_lib(file_name0)){
                        dl[i] = dlopen(file_name0, RTLD_LAZY);   
                        if (!dl[i]){
                            fprintf(stderr, "ERROR: dlopen() failed: %s\n", dlerror());
                            return -1;
                        } else {
                            if (getenv("LAB1DEBUG")) printf("\n%s opened", file_name0);
                            i++;
                        }
                    }
                } else {
                    perror("Error when getting absolute path file: ");
                    return -1;
                }
            }
        }
        //perror ("\nReach end ");
        closedir (dir);
        return 0;

    } else {
        // could not open directory
        perror ("Error opendir(): ");
        return -1;
    }
    
}

int is_lib(const char *filename) { 
    //fprintf(stdout, "file name parsed to is_lib() %s\n", filename);
    char *ext = get_filename_ext(filename);
    if (strcmp(ext,".so") == 0){
        return 1;
    } else {
        return 0;
    }
}

char *get_filename_ext(const char *filename) {
    char *dot = strrchr(filename, '.');
    //fprintf(stdout, "dot %s\n", dot);
    if(!dot || dot == filename) 
        return "";
    return dot;
}
