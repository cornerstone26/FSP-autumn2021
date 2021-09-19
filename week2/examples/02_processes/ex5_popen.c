//
// Read process output with popen().
//
// (c) Alexei Guirik, 2021
// This source is licensed under CC BY-NC 4.0
// (https://creativecommons.org/licenses/by-nc/4.0/)
//

#include <stdlib.h>
#include <stdio.h>

#define BUF_SIZE	1024

int main() {
	FILE *f = popen("ls -lah | wc -l", "r");

	char buffer[BUF_SIZE] = {0};
	fgets(buffer, BUF_SIZE, f);
	int status = pclose(f);
	fprintf(stdout, "Process output: [%s]\n", buffer);
	fprintf(stdout, "Process return code: [%d]\n", status);	

	return 0;
}
