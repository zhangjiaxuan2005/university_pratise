//
// Created by 17246 on 2025/6/24.
//
#include <iostream>
using namespace std;

int main() {
    int *p = new int;
    *p = 10;
    cout << *p << endl;
    delete p;
    int *q = new int[10];
    for (int i = 0; i < 10; i++) {
        q[i] = i;
    }
    for (int i = 0; i < 10; i++) {
        cout << q[i] <<" ";
    }
    delete[] q;
}
