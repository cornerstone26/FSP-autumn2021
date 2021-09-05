//
// Simple qsort() example
//
//  (c) Alexei Guirik, 2021
//  This source is licensed under CC BY-NC 4.0 (https://creativecommons.org/licenses/by-nc/4.0/)
//  

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/*
    Printing support
*/
void print_array(void *base, size_t el_num, size_t el_size, void (*print_func)(const void *e)) {
	for(size_t i = 0; i < el_num; ++i)
		print_func((char*)base + i * el_size);

	printf("\n");
}

void print_arr_int(const void *el) {
	printf("%d ", *(int*)el);
}

void print_arr_str(const void *el) {
	printf("%s ", *(char**)el);
}

/*
    Comparing support
*/
//static 
int cmp_arr_int(const void *a, const void *b) {
	return *(int*)a - *(int*)b;
}

int cmp_arr_str(const void *a, const void *b) {
	return strcmp(*(char**)a, *(char**)b);
}



int arr_i[] = {10, 34, -7, 1, 9};
char* arr_s[] = {"one", "two", "three", "four", "five", "six"};

int main() {

	print_array(arr_i, sizeof(arr_i)/sizeof(arr_i[0]), sizeof(arr_i[0]), print_arr_int);	
	qsort(arr_i, sizeof(arr_i)/sizeof(arr_i[0]), sizeof(arr_i[0]), cmp_arr_int);
	print_array(arr_i, sizeof(arr_i)/sizeof(arr_i[0]), sizeof(arr_i[0]), print_arr_int);

#define ARR_LEN(arr) (sizeof(arr)/sizeof(arr[0]))
#define ARR_LEN_S(arr) ARR_LEN(arr), sizeof(arr[0])
	
	print_array(arr_s, ARR_LEN_S(arr_s), print_arr_str);
	qsort(arr_s, ARR_LEN_S(arr_s), cmp_arr_str);
	print_array(arr_s, ARR_LEN_S(arr_s), print_arr_str);

	return 0;
}
