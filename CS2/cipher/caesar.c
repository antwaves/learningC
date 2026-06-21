#include <corecrt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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


char* encode(char* str, int offset) {
    char* r = malloc(strlen(str));
    char* p = str;
    int i = 0;
    while (*p != '\0') {
        r[i++] = shift(*p, offset);
        p++;
    }
    r[i] = '\0';
    char* result = _strdup(r);
    free(r);
    return result;
}


int main() {
    FILE* fp;
    errno_t err = fopen_s(&fp, "caesar_challenge.txt", "r");
    if (fp == NULL) {
        printf("Failed to open file!");
        return 1;
    }


    char buffer[1024] = {'\0'};
    while (fgets(buffer, 1024, fp) != NULL) {
        continue;
    }

    for (int i = 0; i < 26; i++) {
        printf("%s\n", encode(buffer, i));
    }

    // Solved: And when we find ourselves in a place just right, it will be in the valley of love and delight.
}