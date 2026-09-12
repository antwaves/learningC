#include <stdio.h>
#include <string.h>

int main() {
    char fruits[][10] = {"Apple", "Banana", "Coconut"};

    for (int i = 0; i < sizeof(fruits) / (sizeof(char) * 10); i++) {
        printf("%s ", fruits[i]);
    }
    printf("\n");

    char names[3][50] = {0};

    for (int i = 0; i < sizeof(names) / (sizeof(char) * 50); i++) {
        printf("Enter a name: ");
        fgets(names[i], sizeof(names[i]), stdin);
        names[i][strlen(names[i]) - 1] = '\0';
    }

    for (int i = 0; i < sizeof(names) / (sizeof(char) * 50); i++) {
        printf("%s ", names[i]);
    }


    return 0;
}

