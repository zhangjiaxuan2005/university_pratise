//
// Created by 17246 on 2025/6/24.
//结构体指针
//
#include <iostream>
using namespace std;

struct Student {
    int id;
    string name;
    int age;
};

int main() {
    const auto *s=new Student{1,"张三",18};
    cout << s->id << " " << s->name << " " << s->age << endl;
    delete s;
}