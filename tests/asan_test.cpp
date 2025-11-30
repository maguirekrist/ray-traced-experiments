// asan_test.cpp
#include <iostream>
int main() {
    int* p = new int[1];
    p[5] = 42; // OOB write
    std::cout << p[0] << "\n";
    delete[] p;
}

