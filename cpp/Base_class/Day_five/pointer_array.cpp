//
// Created by 17246 on 2025/6/24.
//
#include <iostream>
using namespace std;

int main() {
    int *p = new int[10] {3,5,1,11,99,66,22,2,8,6};
   for(int i=0;i<10;i++) {
       for(int j=i;j<10;j++) {
           if(p[i]<p[j]) {
               int temp = p[i];
               p[i] = p[j];
               p[j] = temp;
           }
       }
   }
    for(int i=0;i<10;i++) {
        cout << p[i] << " ";
    }
    delete[] p;
}
