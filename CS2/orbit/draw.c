#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>


struct App;
void _init_window(struct App* self);
void _init_vulkan(struct App* self);
void _main_loop(struct App* self);
void _clean_up(struct App* self);
void run(struct App* self);


struct App{
    void (*run)(struct App* self);
    GLFWwindow* window;
    uint32_t WIDTH;
    uint32_t HEIGHT;
};


void run(struct App* self) {
    _init_window(self);
    _init_vulkan(self);
    _main_loop(self);
    _clean_up(self);
}


void _init_window(struct App* self) {
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    self->window = glfwCreateWindow(self->WIDTH, self->HEIGHT, "Vulkan", NULL, NULL);
}


void _init_vulkan(struct App* self) {

}


void _main_loop(struct App* self) {
    while (!glfwWindowShouldClose(self->window)) {
        glfwPollEvents();
    }
}


void _clean_up(struct App* self) {
    glfwDestroyWindow(self->window);
    glfwTerminate();
}


struct App* init() {
    struct App* a = malloc(sizeof(struct App));
    a->run = run;
    a->WIDTH = 800;
    a->HEIGHT = 600;
    return a;
}


void destroy_app(struct App* a) {
    free(a);
}


int main() {

    struct App* app = init();
    app->run(app);
    destroy_app(app);
}