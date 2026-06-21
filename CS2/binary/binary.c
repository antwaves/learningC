#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

int binary_to_decimal(char* s);
char* decimal_to_binary(int n);


int main() {
    printf("%d\n", binary_to_decimal("11010"));
    printf("%s\n", decimal_to_binary(128));
}


int binary_to_decimal(char* s) {
    int r = 0;
    int len = strlen(s) - 1;
    double add = pow(2, len);
    char* p = s;

    while (*p != '\0') {
        if (*p == '1') {
            r += add;
        }

        add /= 2;
        p++;    
    }

    return r;
}


char* decimal_to_binary(int n) {
    int len = 1;
    int temp = n;
    while (temp > 1) {
        temp /= 2;
        len++;
    }

    int index = len - 1;
    char* test = malloc(sizeof(char) * (len + 1));
    for (int i = 0; i < len; i++) {
        if (n % 2 == 0) {
            test[index--] = '0';
        }
        else {
            test[index--] = '1';
        }

        n /= 2;
    }
    test[len] = '\0';;
    char* result = _strdup(test);
    free(test);

    return result;
}
