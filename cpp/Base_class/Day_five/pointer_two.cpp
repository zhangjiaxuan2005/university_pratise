//
// Created by 17246 on 2025/6/24.
//
#include <iostream>
using namespace std;

int main() {
    // int *p;
    // *p = 10;
    int *p = nullptr; //空指针
    int a = 10;
    p = &a;
    cout << *p << endl;
}
