#include "app.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdio.h>
#include <corecrt.h>

void _create_graphics_pipeline(struct App* self) {

}

static char* read_shader_binary(const char* filename) {
    FILE* fp;
    errno_t result = fopen_s(&fp, filename, "rb");

    if (fp == NULL) {
        printf("Failed to open file with error code %d", result);
        exit(EXIT_FAILURE);
    }
    fseek(fp, 0L, SEEK_END);
    uint32_t size = ftell(fp);
    fseek(fp, 0L, SEEK_SET);
    char* buffer = malloc(sizeof(char) * size);
}
