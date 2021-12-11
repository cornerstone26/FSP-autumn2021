#include <errno.h>
#include <stdlib.h>
#include <limits.h>
#include <stdio.h>
#include <string.h>
#include <dlfcn.h>
#include <unistd.h>
#include <dirent.h>
#include <ctype.h>

void get_abs_path(char* path, char* dir, char *fname)
{	
	printf("%s\n", path);
	strcat(path, dir);
	strcat(path, "/");
	strcat(path, fname);
}

int main()
{
	for (int i = 0; i < 5; i++){
		char path[255]  = "";
		get_abs_path(path, "Hanh", "Nguyen");
		printf("%s\n", path);
	}
	return 0;
}