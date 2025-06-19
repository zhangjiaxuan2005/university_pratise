//
// Created by 17246 on 2025/6/19.
//
#include <iostream>
using namespace std;

enum season {
    spring,
    summer,
    autumn,
    winter
};

int main() {
    int season;
    cin >> season;
    switch (season) {
        case spring:
            cout << "Spring" << endl;
            break;
        case summer:
            cout << "Summer" << endl;
            break;
        case autumn:
            cout << "Autumn" << endl;
            break;
        case winter:
            cout << "Winter" << endl;
            break;
    }
}
