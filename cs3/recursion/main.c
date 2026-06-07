#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>


struct Node;
enum Types {START_NODE, INT, CHAR, NODE};
struct Node {
    struct Node* next;
    enum Types type;
    union {
        int* i_data;
        char* c_data;
        struct Node* n_data;
    };
};

struct Node* linked_list();
void* get_data(struct Node* node);
void assign_data(struct Node* node, void* data);
struct Node* append(struct Node* node, void* element, enum Types type);
struct Node* append_to_node(struct Node* node, void* element, enum Types type);
void* index_into(struct Node* first_node, int index);

int factorial(int n);
int fibonacci(int n);
float power2(int n);
int total(struct Node* nums, int index);


int main() {
    printf("%d\n", factorial(10));
    printf("%d\n", fibonacci(10));
    printf("%f\n", power2(10));


    int numbers[] = {5, 6, 7, 8, 9};
    struct Node* nums = linked_list();
    for (int i = 0; i < sizeof(numbers) / sizeof(int); i++) {
        append(nums, &numbers[i], INT);
    }
    printf("%d", total(nums, 0));


    struct Node* l = linked_list();

    struct Node* char_list = linked_list();
    char letters[] = "asdf";
    for (int i = 0; i < sizeof(letters) / sizeof(char); i++) {
        append(char_list, &letters[i], CHAR);
    }

    char letter = 'd';
    int number = 0;

    append(l, char_list, NODE);
    append(l, &letter, CHAR);
    append(l, &number, INT);

    
}


struct Node* linked_list(){
    struct Node* n = (struct Node*)malloc(sizeof(struct Node));
    n->type = START_NODE;
    n->next = NULL;
    return n;
}


struct Node* append(struct Node* node, void* element, enum Types type) {
    struct Node* next_node = node;
    struct Node* temp;

    while (next_node != NULL) {
        temp = next_node;
        next_node = next_node->next;
    }
    return append_to_node(temp, element, type);
}


struct Node* append_to_node(struct Node* node, void* element, enum Types type) {
    switch (type) {
        case INT:
            element = (int*)element;
            break;
        case CHAR:
            element = (char*)element;
            break;
        case NODE:
            element = (struct Node*)element;
            break;
        case START_NODE:
            return NULL;
    }

    int node_size = sizeof(struct Node);
    struct Node* n = (struct Node*)malloc(node_size);
    n->type = type;
    assign_data(n, element);
    node->next = n;
    n->next = NULL;

    return n;
}


void assign_data(struct Node* node, void* data) {
    switch (node->type) {
        case INT:
            node->i_data = (int*)data;
            break;
        case CHAR:
            node->c_data = (char*)data;
            break;
        case NODE:
            node->c_data = (char*)data;
            break;
        case START_NODE:
            break;
    }
}


void* get_data(struct Node* node) {
    enum Types type = node->type;
    switch (node->type) {
        case INT:
            return node->i_data;
        case CHAR:
            return node->c_data;
        case NODE:
            return node->n_data;
        case START_NODE:
            return NULL;
    }
}


void* index_into(struct Node* first_node, int index) {
    if (index < 0) { return NULL; }
    
    struct Node* current = first_node->next;
    for (int i = 0; i < index; i++) {
        if (current == NULL) { return NULL; }
        current = current->next;
    }

    if (current == NULL) { return NULL; }
    return get_data(current);
}


int factorial(int n) {
    if (n == 0) {
        return 1;
    }

    return n * factorial(n - 1);
}


int fibonacci(int n) {
    if (n == 0 || n== 1) {
        return n;
    }

    return fibonacci(n - 1) + fibonacci(n - 2);
}


float power2(int n) {
    if (n == 0) {
        return 1.0;
    }

    if (n > 0) {
        return 2 * power2(n - 1);
    } 
    else {
        return power2(n + 1) / 2;
    }
}


int total(struct Node* nums, int index) {
    int* data = (int*)index_into(nums, index);

    if (data == NULL) { return 0; }
    if (!(nums->type == INT || nums->type == START_NODE)) {return 0;} 


    return *data + total(nums, index + 1);
}

