#include <stdio.h>
#include <windows.h>

// #include <unistd.h> linux/mac

int main() {
    for (int i = i; i < 10; i++) {
        printf("%d\n", i);
    }

    printf("\n");

    for (int i = 2; i < 100; i *= 2) {
        if (i == 32) {
            break;
        }
        for (int j = 1; j < 10; j++) {
            printf("%d\n", j);
        }
        printf("%d\n", i);
    }

    printf("\n");
    
    for (int i = 10; i > -20; i -= 2) {
        if (i < -2) {
            continue;
        }
        printf("%d\n", i);
    }

    printf("\n");


    int c = 10;
    int r = 10;
    char symbol = '@';

    for (int i = 0; i < r; i++) {
        for (int j = 0; j < c; j++) {
            printf("%c", symbol);
        }
        printf("\n");
    }

    for (int i = 10; i > 0; i -= 1) {
        printf("%d\n", i);
        Sleep(1000);
    }
    printf("Happy new year!");
}

