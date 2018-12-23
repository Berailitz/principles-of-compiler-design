// 4.c
#include <stdio.h>

typedef int A[10][20];
A a;
void fun(){
    printf("%d,%d,%d\n",a,a+1,&a+1);
}
int main(){
    fun();
}