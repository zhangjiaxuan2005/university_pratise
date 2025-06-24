//
// Created by 17246 on 2025/6/24.
//指针悬挂和常量指针
//
#include <iostream>
using namespace std;
int main() {
    // int *p = new int;
    // int a=10;
    // *p = a;
    // int *q = p;
    // delete p;
    // cout << *q << endl;

    const int *p;
    int a=10,b=20;
    p=&a;
    // *p=20;
    cout << *p << endl;
    p=&b;
    cout << *p << endl;
    delete p;
}