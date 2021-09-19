//
// Execute exit handlers. Or not.
//
// (c) Alexei Guirik, 2021
// This source is licensed under CC BY-NC 4.0
// (https://creativecommons.org/licenses/by-nc/4.0/)
//

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

void cleaner_one(void) {
	printf("cleaner_one() called\n");
}

void cleaner_two(int exit_code, void* arg) {
	printf("cleaner_two() called [exit code = %d]\n", exit_code);
}

int main() {
	printf("starting main()\n");
	
	atexit(cleaner_one);
	on_exit(cleaner_two, NULL);
	
	printf("before exit()\n");
	exit(42);
	//_exit(42);
	printf("after exit()\n");
	
	return 0;
}
