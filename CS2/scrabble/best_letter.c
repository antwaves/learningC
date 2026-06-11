#include <float.h>
#include <stdio.h>
#include <corecrt.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <vcruntime.h>

#include "../../dict/src/hash_table.c"

int count_tokens(char* string, const char delimiter);
char** split(char* string, const char delimiter);
void replace(char string[], const char a, const char b);


int main() {
    FILE* file;
    errno_t err = fopen_s(&file, "alice.txt", "r"); 
    if (file == NULL) {
        printf("Failed to open file");
        return 1;
    }
    
    char** words = NULL;
    char** temp_words = NULL;
    int words_to_allocate = 0;
    char buffer[1024] = {'\0'};

    while (fgets(buffer, sizeof(buffer), file) != NULL) {
        replace(buffer, '\n', ' ');
        words_to_allocate += count_tokens(buffer, ' ');
    }
    words_to_allocate++; // for final null terminator
    fseek(file, 0, SEEK_SET);

    char** current_word = NULL;
    words = malloc(sizeof(char*) * words_to_allocate);
    int word_index = 0;
    while (fgets(buffer, sizeof(buffer), file) != NULL) {
        replace(buffer, '\n', ' ');
        temp_words = split(buffer, ' ');

        current_word = temp_words;
        while (*current_word != NULL) {
            words[word_index++] = *current_word;
            current_word++;
        }

        free(temp_words);
    }

    words[word_index] = NULL;
    fclose(file);

    ht_hash_table* best_words = ht_new();
    ht_hash_table* best_scores = ht_new();
    ht_hash_table* letter_points = ht_new();

    char letters[][2] = {"a","b","c","d","e","f","g","h","i", "j", "k", "l","m",
                         "n","o","p","q","r","s","t","u","v","w","x","y","z"};

    char scores[][3] =  {"1", "3", "3", "2", "1", "4", "2", "4", "1", "8", 
                         "5", "1", "3", "1", "1", "3", "10", "1", "1", "1", 
                         "1", "4", "4", "8", "4", "10"};

    
    for (int i = 0; i < 26; i++) {
        ht_insert(letter_points, letters[i], scores[i]);
    }

    char* c_word;
    int word_score;
    char letter[2] = {'\0'};

    char* best_word = NULL;
    int best_score = 0;

    for (int i = 0; i < word_index; i++) {
        c_word = words[i];

        word_score = 0;
        size_t len = strlen(c_word);
        for (int j = 0; j < len; j++){
            letter[0] = c_word[j];
            word_score += atoi(ht_search(letter_points, letter));
        }

        if (word_score > best_score) {
            best_word = c_word;
            best_score = word_score;
        }

        char word_score_buffer[20];
        _itoa_s(word_score, word_score_buffer, 20, 10);

        letter[0] = c_word[0];

        if (ht_search(best_words, letter) != NULL) {
            if (word_score > atoi(ht_search(best_scores, ht_search(best_words, letter)))) {
                ht_insert(best_words, letter, c_word);
                ht_insert(best_scores, c_word, word_score_buffer);
            } else {
            }
        } 
        else {
            ht_insert(best_words, letter, c_word);
            ht_insert(best_scores, c_word, word_score_buffer);
        }
    }           

    for (int i = 0; i < 26; i++) {
        char* l = letters[i];
        char* w = ht_search(best_words, l);
        printf("%s %s %s\n", l, w, ht_search(best_scores, w));
    }

    printf("\n%s %d", best_word, best_score);
 
    for (int i = 0; i < word_index; i++) { free(words[i]); }
    free(words);
    ht_del_hash_table(best_words);
    ht_del_hash_table(best_scores);
    ht_del_hash_table(letter_points);

    return 0;
}


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


void replace(char string[], char a, char b) { // replaces character a with character b in a given string
    char* current_character = string;
    size_t index = 0;
    while (*current_character) {
        if (*current_character == a) {
            string[index] = b;
        }
        
        index++;
        current_character++;
    }
}
