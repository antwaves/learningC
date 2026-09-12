#include <stdio.h>
#include <stdbool.h>
#include <string.h>

int main() {

    int age = 0;

    printf("enter your age: ");
    scanf_s("%d", &age);

    if (age >= 18){
        printf("Adult\n");
    } 
    else if (age < 0) {
        printf("Fetus\n");
    }
    else {
        printf("CHIILD\n");
    }

    bool student = true;
    if (student) {
        printf("Okay student\n");
    } 
    else {
        printf("Okay uneducated swine\n");
    }

    getchar();
    char name[100] = "";
    printf("Enter name: ");
    fgets(name, sizeof(name), stdin);
    name[strlen(name) - 1] = '\0';


    if (!(strlen(name))) {
        printf("You didn't do it");
    } 
    else {
        printf("Hello %s", name);
    }
    
    //1:37:00
    return 0;
}
