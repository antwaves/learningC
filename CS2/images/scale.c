#include <stdio.h>
#include <math.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"


int main() {
    int x, y, n;
    unsigned char *data = stbi_load("bird.png", &x, &y, &n, 0);

    int scale = 2;
    int s_x = x * scale;
    int s_y = y  * scale;
    unsigned char *s_data = malloc(s_x * s_y * n * sizeof(unsigned char));

    if (data == NULL) {
        return 1;
    }

    unsigned char** s_array = (unsigned char**)malloc(y * sizeof(unsigned char*));
    for (int i = 0; i < y; i++) {
        s_array[i] = (unsigned char*)malloc(x * n * sizeof(unsigned char));
    }

    int data_index = 0;
    for (int i = 0; i < y; i++) {
        for (int j = 0; j < x * n; j++) {
            s_array[i][j] = data[data_index++];
        }
    }

    int index = 0;
    unsigned char r, g, b;
    int transformed_index;
    for (int i = 0; i < s_y; i++) {
        for (int j = 0; j < s_x; j++){
            r = s_array[i / scale][(j / scale) * n];
            g = s_array[i / scale][(j / scale) * n + 1];
            b = s_array[i / scale][ (j / scale) * n + 2];

            transformed_index = (i * s_x + j) * n;
            s_data[transformed_index] = r;
            s_data[transformed_index + 1] = g;
            s_data[transformed_index + 2] = b;
        }

    }
    
    stbi_write_png("scaled.png", s_x, s_y, n, s_data, s_x * n * sizeof(unsigned char));

    free(data);
    free(s_data);

    for (int i = 0; i < y; i++) {
        free(s_array[i]);
    }
    free(s_array);
}


