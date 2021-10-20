#include <stdio.h>

int main() {
    char str1[]  = "by the pricking of my thumbs,";
    char *str2   = "something wicked this way comes.";
    
    printf("%s\n%s\n\n", str1, str2);
    
    //printf("&str1 = %p, &str2 = %p\n\n", str1, str2);    
    //getc(stdin);
    
    str1[0] = 'B';
    str2[0] = 'S';
    
    
    printf("%s\n%s\n", str1, str2);
    
    return 0;
}
