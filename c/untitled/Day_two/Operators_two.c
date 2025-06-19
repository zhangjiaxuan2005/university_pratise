//
// Created by 17246 on 2025/6/15.
//
#include<stdio.h>
int main(){
    int a = 10;
    printf("%d\n", a++);
    printf("%d\n", ++a);

    int b = 10;
    b+=20;
    //b=b+20
    printf("%d\n", b);
    b-=10;
    //b=b-10;
    printf("%d\n", b);

    return 0;
}