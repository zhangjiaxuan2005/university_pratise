//
// Created by 17246 on 2024/11/6.
//
#include <iostream>
using namespace std;
int main(){
    int arr[100]={1,1};
    for (int i = 0; i < 25; i++) {
        arr[i+2]= arr[i]+arr[i+1];
        cout<<arr[i]<<" ";
    }
}