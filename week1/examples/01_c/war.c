//
// Destroyer of worlds. Exercise caution!
//
// (c) Alexei Guirik, 2021
// This source is licensed under CC BY-NC 4.0 (https://creativecommons.org/licenses/by-nc/4.0/)
// 

#include <stdio.h>
#include <stdbool.h>
#include <string.h>

// Function prototype (declaration)
// This file (war.c) can be compiled without 
// definition of launch_rockets(), but cannot be 
// built into executable.
void launch_rockets();

bool confirm_total_war() {
	bool ret = false;
	puts("Feeling confident? (yes/no)");
	char buf[10] = {0};
	fgets(buf, sizeof(buf), stdin);         // <-- how to make a pause for 5 secs
	if (strlen(buf) != 0 && buf[0] == 'y')  // and continue with no input from user?
		ret = true;
	return ret;	
}

int main() {
	if (confirm_total_war())
		// Весь мир в труху!
		launch_rockets();
	else
	    puts("Next time then...");
	return 0;
}

