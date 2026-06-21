#include <float.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

int hex_digit_to_int(char digit);
int hex_to_decimal(char* s);
char* digit_to_hex(int n);
char num_to_hex(int digit);

int main() {
    printf("%d\n", hex_to_decimal("1a"));
    printf("%s\n", digit_to_hex(26123213));

}


int hex_to_decimal(char* s) {
    int r = 0;
    double m = pow(16, strlen(s) - 1);
    char* p = s;
    while (*p != '\0') {
        r += m * hex_digit_to_int(*p);
        m /= 16;
        p++;
    }
    return r;
}


int hex_digit_to_int(char digit) {
    if (digit >= 48 && digit <= 57) {
        return digit - 48;
    }
    else {
        return digit - 87;
    }
}


char num_to_hex(int digit) {
    if (digit >= 0 && digit <= 9) {
        return (char)(digit + 48);
    } 
    else {
        return (char)(digit + 87);
    }
}


char* digit_to_hex(int n) {
    int len = 1;
    int temp = n;
    while (temp > 0) {
        len += 1;
        temp /= 16;
        if (temp == 0) {
            len -= 1;
        }
    }
    
    int index = len - 1;
    char* r = malloc(sizeof(char) * (len + 1));

    while (n > 0) {
        r[index--] = num_to_hex(n % 16);
        n /= 16;
    }
    r[len] = '\0';

    char* result = _strdup(r);
    free(r);
    return result;
}
