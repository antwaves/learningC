#include <stdio.h>
#include <stdbool.h>

int square(int num) {
    return num * num;
}

double cube(double num) { 
    return num * num * num;
}

bool age_check(int age) {
    return age >= 18;
}

int max(int a, int b) {
    if (a >= b) {
        return a;
    }
    return b;
}

int main() {
    int x = square(2);
    int y = square(3);
    int z = square(5);

    printf("%d %d %d\n", x, y, z);

    double a = cube(2.3);
    printf("%lf\n", a);

    int age = 21;
    if (age_check(age)) { 
        printf("okay adultt\n");
    }

    int m = max(2, 3);
    printf("%d", m);
    return 0;
}
