#include <stdio.h>
#include <math.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"


int main() {
    int x, y, n;
    unsigned char *data = stbi_load("dali.png", &x, &y, &n, 0);

    if (data == NULL) {
        return 1;
    }

    printf("%d\n\n\n", n);
    unsigned char r, g, b;
    char* result = malloc(2 * x * y * sizeof(char) + y * sizeof(char) + 1);
    char symbols[] = "   ``'-~:+<ceOEB";
    int len = strlen(symbols);

    int index = 0;
    for (int i = 0; i < y; i++) {
        for (int j = 0; j < x; j++) {
            unsigned char r = data[(i * x + j) * n ];
            unsigned char g = data[(i * x + j) * n  + 1];
            unsigned char b = data[(i * x + j) * n + 2];

            int l = 0.2126*r + 0.7152*g + 0.0722*b;
            result[index++] = symbols[l * (len-1) / 255];
            result[index++] = ' ';
        }

        result[index++] = '\n';
    }
    printf("%s", result);
    
    free(data);
    free(result);
}