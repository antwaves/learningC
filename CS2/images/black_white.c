#include <stdio.h>
#include <math.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

int main() {
    int x, y, n;
    unsigned char *data = stbi_load("bird.png", &x, &y, &n, 0);
    unsigned char *n_data = malloc(x * y * n * sizeof(unsigned char));

    if (data == NULL) {
        return 1;
    }

    unsigned char r, g, b;
    for (int i = 0; i < x * y * n; i++) {
        switch(i % 3) {
            case 0:
                r = data[i];
                break;
            case 1:
                g = data[i];
                break;
            case 2:
                b = data[i];
                double l = round(0.2126 * r + 0.7152 * g + 0.0722 * b);

                unsigned char c = l < 105 ? 0 : 255;
                n_data[i - 2] = c;
                n_data[i - 1] = c;
                n_data[i] = c;
                break;
        }
    }
    
    stbi_write_png("black_white.png", x, y, n, n_data, x * n * sizeof(unsigned char));
    free(n_data);
}