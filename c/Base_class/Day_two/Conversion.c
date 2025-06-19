//
// Created by 17246 on 2025/6/15.
//
int main() {
    //implicit conversion
    short a = 1;
    short b = 2;
    int c = a + b;

    int i=10;
    long l = 100L;
    double d = 20.0;
    double f = i + l + d;

    //explicit conversion
    int q = 10;
    short n = (short)q;
}