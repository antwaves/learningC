#include <stdio.h>
#include <math.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"


void rgb_to_data(unsigned char* data, int index, unsigned char r, unsigned char g, unsigned char b) {
    data[index - 2] = r;
    data[index - 1] = g;
    data[index] = b;
}


int main() {
    int x, y, n;
    unsigned char *data = stbi_load("bird.png", &x, &y, &n, 0);
    unsigned char *n_data = malloc(x * y * n * sizeof(unsigned char));
    unsigned char *g_data = malloc(x * y * n * sizeof(unsigned char));
    unsigned char *bg_data = malloc(x * y * n * sizeof(unsigned char));
    unsigned char *gr_data = malloc(x * y * n * sizeof(unsigned char));
    unsigned char *i_data = malloc(x * y * n * sizeof(unsigned char));


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
                rgb_to_data(n_data, i, c, c, c); // black white

                unsigned char g = (r + g + b) / 3.0;
                rgb_to_data(g_data, i, g, g, g); // grayscale
                rgb_to_data(bg_data, i, l, l, l); // better grayscale

                if (g > r && g  > b) {
                    rgb_to_data(gr_data, i, l, l, l); // greenish
                } 
                else {
                    rgb_to_data(gr_data, i, r, g, b);
                }

                rgb_to_data(i_data, i, 255 - r, 255 - g, 255 - b);
                


                break;
        }
    }
    
    stbi_write_png("black_white.png", x, y, n, n_data, x * n * sizeof(unsigned char));
    stbi_write_png("grayscale.png", x, y, n, g_data, x * n * sizeof(unsigned char));
    stbi_write_png("better_grayscale.png", x, y, n, bg_data, x * n * sizeof(unsigned char));
    stbi_write_png("greenish.png", x, y, n, gr_data, x * n * sizeof(unsigned char));
    stbi_write_png("inverted.png", x, y, n, i_data, x * n * sizeof(unsigned char));


    free(n_data);
    free(g_data);
    free(bg_data);
    free(gr_data);
    free(i_data);
}