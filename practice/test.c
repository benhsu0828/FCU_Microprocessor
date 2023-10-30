#include<stdio.h>
#include<stdlib.h>

void main(){
    char a[16] = "                ";

    sprintf(a, "%d", 1234564789123456);
    printf("%s\n", a);
    return 0;
}