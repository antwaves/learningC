#include <corecrt.h>
#include <stdio.h>


int main() {
    // write
    FILE *p_file;
    errno_t err = fopen_s(&p_file, "output.txt", "w");
    // if err != 0
    // File could not be opened. filepoint was set to NULL
    // error code is returned in err.
    // error message can be retrieved with strerror(err);
    if (p_file == NULL) {
        printf("Failed to open file");
        return 1;
    }

    char text[] = "I love yuri!!\nKiss me!!";
    fprintf(p_file, "%s", text);
    fclose(p_file);


    // read
    FILE* file;
    err = fopen_s(&file, "output.txt", "r");
    if (file == NULL) {
        printf("Failed to open file");
        return 1;
    }

    char buffer[1024] = {0};

    while (fgets(buffer, sizeof(buffer), file) != NULL) {
        printf("%s", buffer);
    }
}

