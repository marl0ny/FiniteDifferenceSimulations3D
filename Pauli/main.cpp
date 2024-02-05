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


struct Click {
    double x, y;
    double dx, dy;
    int pressed;
    int released;
    int w, h;
} left_click;


void click_update(struct Click *click, GLFWwindow *window) {
    double prev_x = click->x;
    double prev_y = click->y;
    glfwGetFramebufferSize(window, &click->w, &click->h);
    glfwGetCursorPos(window, &click->x, &click->y);
    #ifdef __APPLE__
    click->x = 2.0*click->x/(double)click->w;
    click->y = 1.0 - 2.0*click->y/(double)click->h;
    #else
    click->x = click->x/(double)click->w;
    click->y = 1.0 - click->y/(double)click->h;
    #endif
    click->dx = click->x - prev_x;
    click->dy = click->y - prev_y;
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1) == GLFW_PRESS) {
        click->pressed = 1;
    } else {
        if (click->released) click->released = 0;
        if (click->pressed) click->released = 1;
        click->pressed = 0;
    }
}

static double s_scroll = 1.0;
void scroll_callback(GLFWwindow *window, double x, double y) {
    s_scroll += (2.0*y)/25.0;
}

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
    render_params.which = 25;
    // getchar();
    glfwSetScrollCallback(window, scroll_callback);
    // bool exit_loop = false;
    loop = [&] {
        if (left_click.pressed) {
            render_params.user_use = 1;
            render_params.user_x = left_click.x;
            render_params.user_y = left_click.y;
            render_params.user_dx = -left_click.dx;
            render_params.user_dy = -left_click.dy;
        } else {
            render_params.user_use = 0;
        }
        render_params.user_scroll = s_scroll;
        render(&render_params);
        glfwPollEvents();
        if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
            render_params.which += 1;
        else if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
            render_params.which -= 1;
        click_update(&left_click, window);
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
