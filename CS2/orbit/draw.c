#include "include/GLFW/glfw3.h"
#include "include/vulkan/vulkan.h"
#include "include/cglm/cglm.h"

#include <stdint.h>
#include <stdio.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE


int main() {
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    GLFWwindow* window = glfwCreateWindow(800, 600, "Vulkan Window", NULL, NULL);

    uint32_t extension_count = 0;
    vkEnumerateInstanceExtensionProperties(NULL, &extension_count, NULL);
    printf("%u", extension_count);
    mat4 matrix;
    vec4 vec;
    vec4 result;

    glm_mat4_mul(matrix, &vec, &result);
    
    while(!glfwWindowShouldClose(window)) {
        glfwPollEvents();
    }

    glfwDestroyWindow(window);

    glfwTerminate();
}