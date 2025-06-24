//
// Created by 17246 on 2025/6/24.
//
#include <iostream>
using namespace std;

void bubbleSort(int arr[], int size) {
    for (int i = 0; i < size - 1; i++) {
        for (int j = 0; j < size - i - 1; j++) {
            if (arr[j] > arr[j + 1]) {
                int temp = arr[j];
                arr[j] = arr[j + 1];
                arr[j + 1] = temp;
            }
        }
    }
}

int main() {
    int arr[10] = {0};
    for (int & i : arr) {
       cin >> i;
    }
    bubbleSort(arr, 10);
    for (int i : arr) {
        cout << i << " ";
    }
    return 0;


}
