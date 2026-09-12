#include <stdio.h>

typedef long long int big;
// could also do 
// typedef char String[50];

int main() {
    big x = 3;
    big y = 4;
    big z = x + y;

    printf("%llu", z);

    return 0;
}

