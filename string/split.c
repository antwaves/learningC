#include <stdbool.h>
#include <stdlib.h>
#include <string.h>


int count_tokens(char* string, const char delimiter) { // counts the number of tokens in a string based on a given delimiter
    char* current_character = NULL;
    bool was_last_delimiter = false;
    int to_allocate = 0;

    current_character = string;
    while (*current_character) {
        if (*current_character == delimiter) {
            if (was_last_delimiter) {
                current_character++;
                continue;
            }

            was_last_delimiter = true;
            to_allocate++;
            current_character++;
            continue;
        }

        was_last_delimiter = false;
        current_character++;
    }
    to_allocate += !was_last_delimiter; // add for trailing token

    return to_allocate;
}


char** split(char* string, const char delimiter) { // splits a string based on a delimiter
    char** result = 0;
    int size_of_str = strlen(string) + 1; // add for null terminator (same for to_allocate)
    int to_allocate = count_tokens(string, delimiter) + 1;

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


char** split_count(char* string, int tokens, const char delimiter) { // splits a string based on a delimiter
    char** result = 0;
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
