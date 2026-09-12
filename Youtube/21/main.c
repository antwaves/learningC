#include <stdio.h>
#include <string.h>
#include <stdbool.h>

struct Student {
    char name[50];
    int age;
    float gpa;
    bool fulltime;
};

void print_student(struct Student s);

int main() {
    struct Student s = {"girliepop", 18, 3.0, true};
    struct Student s2 = {"girliepop", 18, 3.0, true};

    strcpy_s(s.name, sizeof(s.name), "Annie");
    s.age = 15;
    s.gpa = 4.0;

    print_student(s);
    print_student(s2);
    return 0;
}


void print_student(struct Student s) {
    printf("Name: %s\n", s.name);
    printf("Age: %d\n", s.age);
    printf("GPA: %0.2f\n", s.gpa);
    printf("Fulltime: %s", s.fulltime ? "fulltime" : "not fulltime");
    printf("\n\n");
}