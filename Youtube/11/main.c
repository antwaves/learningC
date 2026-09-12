#include <stdio.h>
#include <stdbool.h>

void hey(char name[], int age);
bool age_check(int age);

int main() {
    int age = 18;
    if (age_check(age)) {
        hey("darling", age); 
    } 
    return 0;
}

void hey(char petname[], int age) {
    printf("Heyyy %s, you're %d, right?", petname, age);
}

bool age_check(int age) {
    return age >= 18;
}
