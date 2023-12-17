#include<stdio.h>
#include<stdlib.h>

int main(){
    int a;
    for(int i = 0;i<4096;i++){
        a = (i*100/4096);
        printf("i: %4d a: %3d\n",i,a);
    }
    return 0;
}