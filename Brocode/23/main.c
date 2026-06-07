#include <stdio.h>


void inc(int* age);

int main() {
    // * dereference operator
    // & returns mem adress
    int age = 25;

    inc(&age);
    printf("%d\n", age);
}


void inc(int* age) {
    (*age)++;
}