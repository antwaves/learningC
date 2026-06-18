#include <stdio.h>
#include <stdlib.h>
#include <corecrt.h>

#include "../../dict/src/hash_table.c"
#include "../../string/split.c"
#include "../../string/replace.c"



FILE* open_file(char* filename) {
    FILE* file;
    errno_t err = fopen_s(&file, filename, "r"); 

    char error_buffer[128];
    if (file == NULL) {
        strerror_s(error_buffer, sizeof(error_buffer), err);
        printf("Error: %s\n", error_buffer);
        return NULL;
    }

    return file;
}


int count_lines(FILE* fp) {
    int to_allocate = 1;
    char ch = '\0';

    while(!feof(fp)) {
        ch = fgetc(fp);
        if(ch == '\n') {
            to_allocate++;
        }
    }
    fseek(fp, 0, SEEK_SET);
    return to_allocate;
}


ht_hash_table** csv_to_dict_list(char* filename) {
    FILE* file = open_file(filename);
    if (file == NULL) {
        return NULL;
    }

    char buffer[1024] = {'\0'};
    char** columns;
    int num_columns = 0;
    int to_allocate = count_lines(file);
    ht_hash_table** result = malloc(sizeof(ht_hash_table*) * (to_allocate + 1));

    if (fgets(buffer, sizeof(buffer) / sizeof(char), file) != NULL) {
        replace(buffer, '\n', '\0');
        num_columns = count_tokens(buffer, ',');
        columns = split_count(buffer, num_columns, ',');
    }   

    int index = 0;
    while (fgets(buffer, sizeof(buffer) / sizeof(char), file) != NULL) {
        replace(buffer, '\n', '\0');
        int num_values = count_tokens(buffer, ',');
        char** row_values = split_count(buffer, num_values, ',');

        if (num_columns != num_values) {
            printf("Error: Badly formatted file");
            return NULL;
        }

        ht_hash_table* row = ht_new();
        for (int i = 0; i < num_columns; i++) {
            ht_insert(row, columns[i], row_values[i]);
        }


        result[index++] = row;
        free(row_values);
    }


    result[index] = NULL;
    fclose(file);
    free(columns);
    return result;
}


void del_csv_list(ht_hash_table** rows) {
    ht_hash_table** p = rows;
    while(*p != NULL) {
        ht_del_hash_table(*p);
        p++;
    }
    
    free(rows);
}


int main() {
    ht_hash_table** rows = csv_to_dict_list("customers-100000.csv");

    del_csv_list(rows);
}