//
// Created by 17246 on 2025/6/24.
//结构体指针数组
//
#include <iostream>
using namespace std;

struct Student {
    int id;
    string name;
    int age;
};

int main() {
    Student *s=new Student[] {
        {1,"张三",18}
    };
    cout << s[0].id << endl;
    cout << s[0].name << endl;
    cout << s[0].age << endl;
    delete s;
}