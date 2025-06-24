//
// Created by 17246 on 2025/6/24.
//
#include <iostream>
using namespace std;

struct Student {
    int id = 1;
    string name;
    int age = 18;
};

int main() {
    const Student s = {1, "张三", 20};
    cout << s.id << " " << s.name << " " << " " << s.age << endl;
}

