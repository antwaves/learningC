#include <stdio.h>
#include <string.h>

void bday(char name[], int age) {
    printf("\nHappy birthday to you!\n");
    printf("Happy birthday to you!\n");
    printf("Happy birthday dear %s!\n", name);
    printf("Happy birthday to you!\n");
    printf("You are %d years old!\n", age);
}

int main() {
    char name[50] = "";
    int age = 0;

    printf("Enter your name: ");
    fgets(name, sizeof(name), stdin);
    name[strlen(name) - 1] = '\0';

    printf("Enter your age: ");
    scanf_s("%d", &age);

    bday(name, age);

    return 0;
}
