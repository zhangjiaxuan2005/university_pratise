//
// Created by 17246 on 2025/6/19.
//
#include <iostream>
using namespace std;

int main() {
    int sum = 0;
    int count = 1;
    while (count <= 100) {
        sum += count;
        count++;
    }
    cout << sum << endl;
}
