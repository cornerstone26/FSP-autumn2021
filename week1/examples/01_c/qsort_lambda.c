//
// Not so simple qsort() example with 'anonymous' (kinda) functions
//
// Note: This example will only work with gcc, as it requires gcc extensions:
// https://embeddedbits.org/gcc-extensions-to-the-c-language/
//
//  (c) Alexei Guirik, 2021
//  This source is licensed under CC BY-NC 4.0 (https://creativecommons.org/licenses/by-nc/4.0/)
//  

#include <stdlib.h>
#include <stdio.h>

/*
   Macro from https://stackoverflow.com/questions/10405436/anonymous-functions-using-gcc-statement-expressions
   
   Create a lambda function.  Note: unlike lambdas in functional
   languages, this lambda does not capture the containing
   environment.  Thus, if you access the enclosing environment, you
   must ensure that the lifetime of this lambda is bound by the
   lifetime of the enclosing environment (i.e., until the enclosing
   function returns).  This means that if you access local
   variables, bad things will happen.  If you don't access local
   variables, you're fine.
*/   
#define lambda(l_ret_type, l_arguments, l_body)         \
  ({                                                    \
    l_ret_type l_anonymous_functions_name l_arguments   \
      l_body                                            \
    &l_anonymous_functions_name;                        \
  })


#define ARR_LEN(arr) (sizeof(arr)/sizeof(arr[0]))
#define ARR_LEN_S(arr) ARR_LEN(arr), sizeof(arr[0])

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


int arr_i[] = {10, 34, -7, 1, 9};
char* arr_s[] = {"one", "two", "three", "four", "five", "six"};


int main (int argc, char *argv[]) {
    int comparison = 0;

	print_array(arr_i, ARR_LEN_S(arr_i), print_arr_int);
    
    qsort (arr_i, ARR_LEN_S(arr_i),
         lambda (int, (const void *a, const void *b),
                {
                    //dump ();
                    printf ("Comparison %d: %d and %d\n",
                           ++comparison, *(const int *) a, *(const int *) b);
                    return *(const int *)a - *(const int *)b;
                }));
    
    print_array(arr_i, ARR_LEN_S(arr_i), print_arr_int); 
    return 0;
}

