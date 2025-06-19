//
// Created by 17246 on 2025/6/19.
//
#include <iostream>
using namespace std;

int main() {
    int i = 1;
    int j = 1;
    while (i <= 9) {
        while (j <= i) {
            cout << j << "*" << i << "=" << i * j << "\t";
            j++;
        }
        cout << endl;
        i++;
        j=1;
    }
}
