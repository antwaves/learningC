#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


#define Task(t) {t, false};


typedef struct{
    char* task_name;
    bool completed;
} _Task;


char* t_repr(_Task* task, char* buffer) {
    buffer[0] = '[';
    buffer[1] = task->completed ? 'X' : ' ';
    buffer[2] = ']';
    buffer[3] = ' ';
    
    int index = 4;
    int len = strlen(task->task_name);
    
    for (int i = 0; i < len; i++) {
        buffer[index++] = task->task_name[i];
    }
    buffer[index] = '\0';

    return buffer;
}


struct TaskNode{
    _Task task;
    struct TaskNode* next_node;
};


typedef struct {
    struct TaskNode* tasks;
} ToDolist;


void add(ToDolist* td, char* task_name) {
    if (td->tasks == NULL) {
        struct TaskNode* task_node = malloc(sizeof(struct TaskNode));
        _Task task = {task_name, false};
        task_node->task = task;
        task_node->next_node = NULL;
        td->tasks = task_node;
        return;
    }   

    struct TaskNode* current_node = td->tasks;
    while (current_node->next_node != NULL) {
        current_node = current_node->next_node;
    }

    struct TaskNode* node = malloc(sizeof(struct TaskNode));
    _Task to_add = Task(task_name);
    node->task = to_add;
    node->next_node= NULL;

    current_node->next_node = node;
}


void finish(ToDolist* td, char* task_name) {
    struct TaskNode* current_node = td->tasks;
    while (current_node->next_node != NULL) {
        if (strcmp(current_node->task.task_name, task_name) == 0) {
            current_node->task.completed = true;
        }

        current_node = current_node->next_node;
    }

    if (strcmp(current_node->task.task_name, task_name) == 0) {
        current_node->task.completed = true;
    }
}


void remove_task(ToDolist* td, char* task_name) {
    struct TaskNode* current_node = td->tasks;
    struct TaskNode* last_node = NULL;

    while (current_node->next_node != NULL) {
        if (strcmp(current_node->task.task_name, task_name) == 0) {
            if (last_node != NULL) {
                last_node->next_node = current_node->next_node;
                free(current_node);
                return;
            } 
            else {
                td->tasks = current_node->next_node;
                free(current_node);
            }
        }
        last_node = current_node;
        current_node = current_node->next_node;
    }

    if (strcmp(current_node->task.task_name, task_name) == 0) {
        last_node->next_node = NULL;
        free(current_node);
    }
}


void free_todo(ToDolist* td) {
    struct TaskNode* current_node = td->tasks;
    struct TaskNode* last_node;
    while (current_node->next_node != NULL) {
        last_node = current_node;
        current_node = current_node->next_node;
        free(last_node);
    }

    free(current_node);
}


void write_buffer_from_node(struct TaskNode* n, int* index, char* buffer, char* temp_buffer) {
    t_repr(&n->task, temp_buffer);

    char* t = temp_buffer;
    while (*t != '\0') {
        buffer[(*index)++] = *t;
        t++;
    }
    buffer[(*index)++] = '\n';
}


char* td_repr(ToDolist* td, char* buffer) {
    int index = 0;
    struct TaskNode* current_node = td->tasks;
    char temp_buffer[1024];

    while (current_node->next_node != NULL) {
        write_buffer_from_node(current_node, &index, buffer, temp_buffer);
        current_node = current_node->next_node;
    }

    write_buffer_from_node(current_node, &index, buffer, temp_buffer);
    buffer[index] = '\0';
    return buffer;
}


int main() {
    _Task t = Task("walk around");

    printf("Test task\n\n");
    printf("%s\n", t.task_name);
    printf("%s\n", t.completed ? "True" : "False");
    t.completed = true;

    char buffer[1024];
    printf("%s\n", t_repr(&t, buffer));
    
    printf("Test todo list\n\n");
    ToDolist td = {NULL};

    add(&td, "do homework");
    add(&td, "feed puppy");
    add(&td, "water plants");
    finish(&td, "feed puppy");
    remove_task(&td, "do homework");

    char b[1024];
    printf("%s", td_repr(&td, b));

    free_todo(&td);
}
