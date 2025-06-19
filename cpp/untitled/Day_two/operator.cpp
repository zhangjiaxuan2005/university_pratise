//
// Created by 17246 on 2025/6/19.
//
#include <iostream>
using namespace std;

int main() {
    cout << 10 + 20 << endl;
    cout << 30 - 10 << endl;
    cout << 30.0 / 20 << endl;
    cout << 30 * 20 << endl;
    cout << 30 % 9 << endl;
    int a = 10;
    cout << a++ << endl;
    cout << a-- << endl;
    cout << a << endl;
    int b = 10;
    b += 1; //b=b+1
    cout << b << endl;
    bool c = 1 == 0;
    cout << c << endl;
    bool d = 2 != 0;
    cout << d << endl;
    bool e = c || d;
    cout << e << endl;
    int f = a > b ? a : b;
    cout << f << endl;
}
