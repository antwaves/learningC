#include <stdio.h>


int main () {
    int age = 0;
    int d_age;

    printf("Enter your birth year: ");
    scanf_s(" %d", &age);
    printf("Enter your dog's birth year: ");
    scanf_s(" %d", &d_age);


    printf("You are (about) %d years old\n", 2026 - age);
    printf("Your dog is (about) %d years old\n", 2026 - d_age);
    printf("Your dog is (about) %d years old in dog years\n", (2026 - d_age) * 7);

}


