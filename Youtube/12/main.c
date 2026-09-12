#include <stdio.h>
#include <string.h>

int main() {
    char name[50] = "";

    printf("Enter your name: ");
    fgets(name, sizeof(name), stdin);
    name[strlen(name) - 1] = '\0';

    while (!strlen(name)) {
        printf("Can't have empty name: ");
        fgets(name, sizeof(name), stdin);
        name[strlen(name) - 1] = '\0';
    }

    printf("Heyyy %s", name);
    
}

