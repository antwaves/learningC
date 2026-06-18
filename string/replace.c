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