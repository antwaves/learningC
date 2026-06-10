#include <float.h>
#include <stdio.h>
#include <corecrt.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

int count_tokens(char* string, const char delimiter);
char** split(char* string, const char delimiter);
void replace(char* string, const char a, const char b);


int main() {
    FILE* file;
    errno_t err = fopen_s(&file, "alice.txt", "r");
    if (file == NULL) {
        printf("Failed to open file");
        return 1;
    }

    char** words;
    int words_to_allocate = 0;
    char buffer[1024] = {'\0'};

    while (fgets(buffer, sizeof(buffer), file) != NULL) {

        words_to_allocate += count_tokens(buffer, ' ');
    }
    fseek(file, 0, SEEK_SET);

    words_to_allocate++; // for final null terminator
    words = malloc(sizeof(char*) * words_to_allocate);

    while (fgets(buffer, sizeof(buffer), file) != NULL) {
        //temp_words = split(buffer, ' ');
    }

    fclose(file);
}


int count_tokens(char* string, const char delimiter) {
    char* current_character;
    bool was_last_delimiter;
    int to_allocate;

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


char** split(char* string, const char delimiter) {
    char** result = 0;
    int size_of_str = strlen(string) + 1; // add for null terminator (same for to_allocate)
    int to_allocate = count_tokens(string, delimiter) + 1;

    result = malloc(sizeof(char*) * to_allocate);
    int in_bytes = sizeof(char) * size_of_str;
    char* n_str = malloc(in_bytes);
    strcpy_s(n_str, in_bytes, string);

    if (n_str && result) {
        char delim[2];
        delim[0] = delimiter;
        delim[1] = 0;

        char* current_token;
        char* context = NULL;
        size_t index = 0;
        char* token = strtok_s(n_str, delim, &context);

        while (token)
        {
            *(result + index++) = _strdup(token);
            token = strtok_s(NULL, delim, &context);
        }
    }

    *(result + to_allocate - 1) = NULL;
    return result;
}
