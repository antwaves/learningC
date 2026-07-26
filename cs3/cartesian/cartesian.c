#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <vcruntime.h>


char** product(char** a, char** b) {
    int a_elms = 0;
    int b_elms = 0;
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

    char** prod = malloc(((a_elms * b_elms) + 1) * sizeof(char*));
    int index = 0;
    for (int i = 0; i < a_elms; i++) {
        for (int j = 0; j < b_elms; j++) {
            int a_length = strlen(a[i]);
            int b_length = strlen(b[j]);
            int internal_ind = 0;
            
            prod[index] = malloc((a_length + b_length + 2) * sizeof(char));

            for (int k = 0; k < a_length; k++) {
                prod[index][internal_ind++] = a[i][k];
            }
            prod[index][internal_ind++] = ' ';
            for (int k = 0; k < b_length; k++) {
                prod[index][internal_ind++] = b[j][k];
            }
            prod[index][internal_ind] = '\0';

            index++;
        }
    }
    prod[index] = NULL;
    return prod;
}


char** multi_product(char*** arrs) {
    char*** p_arr = arrs;
    int elms = 0;
    while (*p_arr != NULL) {
        elms++;
        p_arr++;
    }

    if (elms < 2) {
        exit(1);
    }
    else if (elms == 2) {
        return product(arrs[0], arrs[1]);
    }
    
    char*** olds = malloc(sizeof(char**) * (elms - 2));
    char** set = product(arrs[0], arrs[1]);
    for (int i = 2; i < elms; i++) {
        olds[i - 2] = set;
        set = product(set, arrs[i]);
    }
    
    for (int i = 0; i < elms - 2; i++) {
        char** old_p = olds[i];
        while (*old_p != NULL) {
            free(*old_p);
            old_p++;
        } 
        free(olds[i]);
    }

    free(olds);
    return set;
}


void print_and_free(char** arr) {
    char** p = arr;
    while (*p != NULL) {
        printf("%s\n", *p);
        free(*p);
        p++;
    }
    free(arr);
}


int main() {
    char* suits[] = {"C", "D", "H", "S", NULL};
    char* ranks[] = {"A", "2", "3", "4", "5", "6", "7", "8", "9", "10", "J", "Q", "K", NULL};

    char** items = product(suits, ranks);
    print_and_free(items);

    char* counts[] = {"one", "two", "three", NULL};
    char* shapes[]= {"diamond", "squiggle", "oval", NULL};
    char* shadings[] = {"solid", "striped", "open", NULL};
    char* colors[] = {"red", "green", "purple", NULL};

    char*** olds = malloc(sizeof(char**) * 2);
    char** set = product(counts, shapes);
    olds[0] = set;
    set = product(set, shadings);
    olds[1] = set;
    set = product(set, colors);

    for (int i = 0; i < 2; i++) {
        char** old_p = olds[i];
        while (*old_p != NULL) {
            free(*old_p);
            old_p++;
        } 
        free(olds[i]);
    }
    print_and_free(set);
    free(olds);

    printf("\n\n\n\n");
    char** arrs[] = {counts, shapes, shadings, colors, NULL};
    char** m_set = multi_product(arrs);
    print_and_free(m_set);
}

