//
// Created by 17246 on 2025/5/30.
//

#include <stdio.h>

int main() {
    int a, b;
    a = 17;
    b = 6;
    printf("%d\n", a + b);
    printf("%d\n", a - b);
    printf("%d\n", a * b);
    printf("%d\n", a % b);
    double c, d;
    c = 17;
    d = 6;
    printf("%.2f\n", c / d);
    printf("\n");

    int e;
    scanf("%d", &e);
    printf("%d\n", e/100);
    printf("%d\n", e/10%10);
    printf("%d\n", e%10);

    char f='a';
    printf("%c\n", f+1);

    return 0;
}
