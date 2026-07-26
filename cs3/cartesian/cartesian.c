#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <vcruntime.h>


char** product(char** a, char** b) {
    int a_elms, b_elms = 0;
    char** p_a = a;
    char** p_b = b;

    while (*p_a != NULL) {
        a_elms++;
        p_a++;
    }
    while (*p_b !=  NULL) {
        b_elms++;
        p_b++;
    }

    char** items = malloc(((a_elms * b_elms) + 1) * sizeof(char*));
    int index = 0;
    for (int i = 0; i < a_elms; i++) {
        for (int j = 0; j < b_elms; j++) {
            int a_length = strlen(a[i]);
            int b_length = strlen(b[j]);
            int internal_ind = 0;
            
            items[index] = malloc((a_length + b_length + 2) * sizeof(char));

            for (int k = 0; k < a_length; k++) {
                items[index][internal_ind++] = a[i][k];
            }
            items[index][internal_ind++] = ' ';
            for (int k = 0; k < b_length; k++) {
                items[index][internal_ind++] = b[j][k];
            }
            items[index][internal_ind] = '\0';

            index++;
        }
    }
    items[index] = NULL;
    return items;
}


int main() {
    char* suits[] = {"C", "D", "H", "S", NULL};
    char* ranks[] = {"A", "2", "3", "4", "5", "6", "7", "8", "9", "10", "J", "Q", "K", NULL};

    char** items = product(suits, ranks);

    char** p = items;
    while (*p != NULL) {
        printf("%s\n", *p);
        char** t = p;
        free(*t);
        p++;
    }
    free(items);
}
