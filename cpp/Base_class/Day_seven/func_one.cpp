//
// Created by 17246 on 2025/6/24.
//函数地址传递
//
#include <iostream>
using namespace std;
int fibonacci(int n) {
    if (n <= 1) {
        return n;
    } else {
        return fibonacci(n - 1) + fibonacci(n - 2);
    }
}

void swap(int *a, int *b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}

void swap(int &a,int &b) noexcept {
    int temp=a;
    a=b;
    b=temp;
}

int main() {
    int x=1, y=2;
    swap(&x, &y);
    swap(x,y);
    cout << x << endl;
    cout << y << endl;
    cout << fibonacci(7) << endl;
}