//
// Created by 17246 on 2025/6/15.
//
#include <stdio.h>
int main() {
    int a = 10;
    int b = 20;
    printf("%d\n",a==b);
    printf("%d\n",a!=b);
    printf("%d\n",a>b);
    printf("%d\n",a<b);

    printf("%d\n",a>1&&a<20);
    printf("%d\n",a>1||a<20);
    printf("%d\n",!a);

    printf("%d\n",a>b?a:b);
    return 0;
}