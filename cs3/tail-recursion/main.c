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
struct Node* index_into(struct Node* first_node, int index);

int _factorial(unsigned int n, unsigned int result);
int factorial(unsigned int n);
int _power2(unsigned int n, unsigned int result);
int power2(unsigned int n);
int _total(struct Node* list, int index, int result);
int total(struct Node* list);
int _minimum(struct Node* list, int index, int* result);
int minimum(struct Node* list);


int main() {
    printf("%d\n", factorial(10));
    printf("%d\n", power2(10));

    int nums[] = {5, 6, 7, 8, 9};
    struct Node* numbers = linked_list();
    for (int i = 0; i < sizeof(nums) / sizeof(int); i++) {
        append(numbers, &nums[i], INT);
    }

    printf("%d\n", total(numbers));

    int more_nums[] = {68, 10, 13, 2, 13, 10, 57, 12, 80, 82};
    struct Node* more_numbers = linked_list();
    for (int i = 0; i < sizeof(more_nums) / sizeof(int); i++) {
        append(more_numbers, &more_nums[i], INT);
    }
    printf("%d\n", minimum(more_numbers));
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
            element = (struct Node*)element;
            break;
    }

    if (type == START_NODE) {
        node->next = element;
        return element;
    }

    int node_size = sizeof(struct Node);
    struct Node* n = (struct Node*)malloc(node_size);
    node->next = n;
    n->type = type;
    assign_data(n, element);
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
    if (node == NULL) { return NULL; }

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


struct Node* index_into(struct Node* first_node, int index) {
    if (index < 0) { return NULL; }
    
    struct Node* current = first_node->next;
    for (int i = 0; i < index; i++) {
        if (current == NULL) { return NULL; }
        current = current->next;
    }

    if (current == NULL) { return NULL; }
    return current;
}


int _factorial(unsigned int n, unsigned int result) {
    if (n == 0) {
        return result;
    }

    return _factorial(n - 1, result * n);
}


int factorial(unsigned int n) {
    return _factorial(n, 1);
}


int _power2(unsigned int n, unsigned int result) {
    if (n == 0) {
        return result;
    }

    return _power2(n - 1, result * 2);
}


int power2(unsigned int n) {
    return _power2(n, 1);
}


int _total(struct Node* list, int index, int result) {
    void* data = get_data(index_into(list, index));
    if (data == NULL) {
        return result;
    }

    return _total(list, index + 1, result + *(int*)data);
}


int total(struct Node* list) {
    return _total(list, 0, 0);
}


int _minimum(struct Node* list, int index, int* result) {
    void* data = get_data(index_into(list, index));
    if (data == NULL) {
        return *result;
    }
    
    if (result == NULL || *result > *(int*)data) {
        result = (int*)data;
    }

    return _minimum(list, index + 1, result);
}


int minimum(struct Node* list) {
    return _minimum(list, 0, NULL);
}
