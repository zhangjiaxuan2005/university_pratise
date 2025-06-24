//
// Created by 17246 on 2025/6/24.
//结构体数组
//
#include <iostream>
using namespace std;

struct Student {
    int id;
    string name;
    int age;
};

int main() {
    Student s[3];
    s[0]={1,"张三",18};
    for (int i = 1; i < 3; i++) {
        cin >> s[i].id >> s[i].name >> s[i].age;
    }
    for (int i = 0; i < 3; i++) {
        cout << s[i].id << " " << s[i].name << " " << s[i].age << endl;
    }
    return 0;
}