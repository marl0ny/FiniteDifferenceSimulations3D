#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "gl_wrappers/gl_wrappers.h"
#include "init_render.h"


#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif
#include <functional>
static std::function <void()> loop;
#ifdef __EMSCRIPTEN__
static void main_loop() {
    loop();
}
#endif


int main() {
#ifdef __APPLE__
    int pixel_width = 1024;
    int pixel_height = 1024;
#else
    int pixel_width = 512;
    int pixel_height = 512;
#endif
    GLFWwindow *window = init_window(pixel_width, pixel_height);
    init();
    struct RenderParams render_params = {};
    // getchar();
    loop = [&] {
        render(&render_params);
        glfwPollEvents();
        glfwSwapBuffers(window);
    };
    #ifdef __EMSCRIPTEN__
    emscripten_set_main_loop(main_loop, 0, true);
    #else
    for (int k = 0; !glfwWindowShouldClose(window); k++) {
        loop();
    }
    #endif
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
