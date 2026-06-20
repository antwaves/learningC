#include <windows.h>
#include <stdio.h>
#include "../../random/random.c"

void display(char* str) {
    char* p = str;
    while (*p != '\0') {
        printf("%c", *p);
        if (*p == '\n') {
            Sleep(randint(100, 250));
        }

        Sleep(randint(10, 50));
        p++;
    }
}

int main() {
    display("Hello world!\nAccording to all known laws of aviation, there is no way a bee should be able to fly.\nIts wings are too small to get its fat little body off the ground.\nThe bee, of course, flies anyway because bees don't care what humans think is impossible.");
}

