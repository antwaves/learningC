#include <stdio.h>
#include <stdlib.h>

int main() {
    int elms = 0;
    printf("enter elms: ");
    scanf_s("%d", &elms);

    char *grades = malloc(elms * sizeof(char));

    if (grades == NULL) {
        printf("AAAAAAAAAAAAAAAAAAAAA");
        return 1;
    }

    for (int i = 0; i < elms; i++) {
        printf("Enter a grade #%d: ", i + 1);
        scanf_s(" %c", &grades[i]);
    }

    for (int i = 0; i < elms; i++) {
        printf("%c ", grades[i]);
    }


    free(grades);
    grades = NULL; // avoid dangling pointers
}

