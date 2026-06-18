#include <stdio.h>
#include <stdlib.h>
#include <corecrt.h>
#include <stdbool.h>

#include "../../dict/src/hash_table.c"
#include "../../string/replace.c"


int count_tokens(char* string, const char delimiter, const char encloser);
char** split_count(char* string, int tokens, const char delimiter, const char encloser);


ht_hash_table** csv_to_dict_list(char* filename);
FILE* open_file(char* filename);
int count_lines(FILE* fp);
void del_csv_list(ht_hash_table** rows);


int main() {
    char* test = "param1,param2,params\n";
    char* test_2 = "3,\"lovers, right?\",5";


    int a = count_tokens(test, ',', '\"');
    int b = count_tokens(test_2, ',', '\"');

    printf("%d %d", a, b);

    // ht_hash_table** rows = csv_to_dict_list("test_2.csv");

    // del_csv_list(rows);
}



// ht_hash_table** csv_to_dict_list(char* filename) {
//     FILE* file = open_file(filename);
//     if (file == NULL) {
//         return NULL;
//     }

//     char buffer[1024] = {'\0'};
//     char** columns;
//     int num_columns = 0;
//     int to_allocate = count_lines(file);
//     ht_hash_table** result = malloc(sizeof(ht_hash_table*) * (to_allocate));

//     for (int i = 0; i < to_allocate; i++) {
//         result[i] = ht_new();
//     }
    
//     if (fgets(buffer, sizeof(buffer) / sizeof(char), file) != NULL) {
//         replace(buffer, '\n', '\0');
//         num_columns = count_tokens(buffer, ',');
//         columns = split_count(buffer, num_columns, ',');
//     }   

//     int index = 0;
//     while (fgets(buffer, sizeof(buffer) / sizeof(char), file) != NULL) {
//         replace(buffer, '\n', '\0');
//         int num_values = count_tokens(buffer, ',');
//         char** row_values = split_count(buffer, num_values, ',');

//         if (num_columns != num_values) {
//             printf("Error: Badly formatted file");
//             return NULL;
//         }

//         ht_hash_table* row = result[index];
//         for (int i = 0; i < num_columns; i++) {
//             ht_insert(row, columns[i], row_values[i]);
//         }


//         result[index++] = row;
//         free(row_values);
//     }


//     result[index] = NULL;
//     fclose(file);
//     free(columns);
//     return result;
// }


// FILE* open_file(char* filename) {
//     FILE* file;
//     errno_t err = fopen_s(&file, filename, "r"); 

//     char error_buffer[128];
//     if (file == NULL) {
//         strerror_s(error_buffer, sizeof(error_buffer), err);
//         printf("Error: %s\n", error_buffer);
//         return NULL;
//     }

//     return file;
// }


// void del_csv_list(ht_hash_table** rows) {
//     ht_hash_table** p = rows;
//     while(*p != NULL) {
//         ht_del_hash_table(*p);
//         p++;
//     }
    
//     free(rows);
// }


// int count_lines(FILE* fp) {
//     int to_allocate = 1;
//     char ch = '\0';

//     while(!feof(fp)) {
//         ch = fgetc(fp);
//         if(ch == '\n') {
//             to_allocate++;
//         }
//     }
//     fseek(fp, 0, SEEK_SET);
//     return to_allocate;
// }



int count_tokens(char* string, const char delimiter, const char encloser) {
    // counts the number of tokens in a string based on a given delimiter
    // the encloser specifies "when the delimiter is in this encloser, interpret it literally"
    
    char* current_character = NULL;
    bool enclosed = false;
    bool was_last_delimiter = false;
    int to_allocate = 0;

    current_character = string;
    while (*current_character) {
        enclosed = ((*current_character == encloser) ^ enclosed);

        if (*current_character == delimiter) {
            to_allocate += !enclosed;
            current_character++;
            was_last_delimiter = true;
            continue;
        }

        was_last_delimiter = false;
        current_character++;
    }
    to_allocate += !was_last_delimiter; // add for trailing token

    return to_allocate;
}


char** split_count(char* string, int tokens, const char delimiter, const char encloser) { 
    // splits a string based on a delimiter
    // the encloser specifies "when the delimiter is in this encloser, interpret it literally"

    char** result = NULL;
    int size_of_str = strlen(string) + 1; // add for null terminator (same for to_allocate)
    int to_allocate = tokens + 1;

    result = malloc(sizeof(char*) * to_allocate);
    int in_bytes = sizeof(char) * size_of_str;
    char* n_str = malloc(in_bytes);
    strcpy_s(n_str, in_bytes, string);

    size_t index = 0;
    if (n_str && result) {
        char delim[] = {delimiter, '\0'};

        char* current_token;
        char* context = NULL;
        char* token = strtok_s(n_str, delim, &context);

        while (token)
        {
            *(result + index++) = _strdup(token);
            token = strtok_s(NULL, delim, &context);
        }
    }

    free(n_str);
    *(result + index) = NULL;
    return result;
}
