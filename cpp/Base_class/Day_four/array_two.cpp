//
// Created by 17246 on 2025/6/19.
//
#include <iostream>
using namespace std;

int main() {
    char arr[] = "hello";
    for (int i = 0; i < 6; i++) {
        cout << arr[i];
    }
    cout << endl;
    for (char i: arr) {
        cout << i;
    }
    cout << endl;
    int arr1[10][10];
    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < 10; j++) {
            arr1[i][j] = 0;
            cout << arr1[i][j] << " ";
        }
        cout << endl;
    }
}
