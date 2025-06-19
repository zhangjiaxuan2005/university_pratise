//
// Created by 17246 on 2025/6/19.
//
#include <iostream>
using namespace std;

int main() {
    int age;
    cout<<"请输入你的年龄"<<endl;
    cin>>age;
    if (age<18) {
        cout<<"你未成年"<<endl;
    }else {
        cout<<"你已成年"<<endl;
    }
}