#include <stdio.h>
#include <string.h>
#include <stdlib.h>

char shift(char c, int o) {
    if (c >= 97 && c <= 122) {
        char l_index = ((c - 97) + o) % 26;
        while (l_index < 0) {
            l_index = 26 + l_index;
        }

        return (l_index + 97);
    }
    else if (c >= 65 && c <= 90) {
        c += 32;
        char lower = shift(c, o);
        return lower - 32;
    }
    else {
        return c;
    }
}


char* translate(char* str, char* key, short sign) {
    int l = strlen(str);
    char* r = malloc(l);
    int k_l = strlen(key);

    for (int i = 0; i < l; i++) {
        char c = key[i % k_l];

        if (str[i] >= 97 && str[i] <= 122) {
            r[i] = shift(str[i], -sign * (c - 97));
        }
        else if (str[i] >= 65 && str[i] <= 90) {
            char tmp = str[i] + 32;


            r[i] = shift(tmp, -sign * (c - 97));

            r[i] = r[i] - 32;
        }
        else {
            r[i] = str[i];
        }
    }
    r[l] = '\0';

    char* result = _strdup(r);
    free(r);
    return result;
}


char* encode(char* str, char* key) {
    return translate(str, key, 1);
}


char* decode(char* str, char* key) {
    return translate(str, key, -1);
}


int main() {
    printf("%s", translate("Coding is cool :)", "gfs", -1));
}