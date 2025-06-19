//
// Created by 17246 on 2025/6/19.
//
#include<iostream>
using namespace std;

int main() {
    int a = 10;
    cout << a << endl;
    cout << sizeof(a) << endl;
    long a1 = 10L;
    cout << a1 << endl;
    unsigned int b = 10U;
    cout << b << endl;
    float c = 10.5F;
    cout << c << endl;
    double d = 10.15;
    cout << d << endl;
    char e = 'c';
    cout << e << "\n";//ASCII表
    string f = "hello";
    string g = "world";
    string h = f+" "+g;
    cout << h << endl;
    bool i = true;//true->1 false->0
    cout << i << endl;
}
