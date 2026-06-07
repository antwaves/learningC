#include <stdio.h>
#include <string.h>

int main() {
    int age = 0;
    char name[100] = "";

    printf("Enter your age: ");
    scanf_s("%d", &age);

    getchar();
    printf("Whats your name, darling?: ");
    fgets(name, sizeof(name), stdin);
    name[strlen(name) - 1] = '\0';


    printf("Heyyy %s. Ur %d. Thats so cool.\n", name, age);
}
