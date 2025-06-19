//
// Created by 17246 on 2025/6/19.
//

#include <iostream>
using namespace std;

int main() {
    const int a = 5;
    int b;
    cout << "请输入我想的数字:";
    cin >> b;
    if (b == a) {
        cout << "恭喜你猜对咯" << endl;
    } else {
        cin >> b;
        if (b == a) {
            cout << "恭喜你猜对咯" << endl;
        } else {
            cout << "没有机会啦" << endl;
        }
    }
}
