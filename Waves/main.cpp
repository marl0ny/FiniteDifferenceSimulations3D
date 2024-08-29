#include "gl_wrappers.hpp"
#include "glfw_window.hpp"
#include <OpenGL/OpenGL.h>
#include <iostream>

GLFWwindow *s_window = init_window(1024, 1024);
// Quad s_main_render_target = Quad(TextureParams {});

struct SimParams {
    float dt = sqrt(1.0/3.0);
    // float t = 1.0;
    float c = 1.0;
    Vec3 dimensions = {.ind={256.0, 256.0, 256.0}};
    IVec3 grid_dimensions = {.ind={256, 256, 256}};
    // Vec3 dimensions = {.ind={128.0, 128.0, 128.0}};
    // IVec3 grid_dimensions = {.ind={128, 128, 128}};
    // Vec3 dimensions = {.ind={64.0, 64.0, 64.0}};
    // IVec3 grid_dimensions = {.ind={64, 64, 64}};
} s_sim_params;


const TextureParams MAIN_WINDOW_PARAMS = {
    .format=GL_RGBA32F, .width=1024, .height=1024,
    .generate_mipmap=true,
    .mag_filter=GL_LINEAR, .min_filter=GL_LINEAR, 
    .wrap_s=GL_REPEAT, .wrap_t=GL_REPEAT
};

const TextureParams SIMULATION_PARAMS = {
    .format=GL_RGBA32F, 
    .width=(u_int32_t)
        get_2d_from_3d_dimensions(s_sim_params.grid_dimensions)[0],
    .height=(u_int32_t)
        get_2d_from_3d_dimensions(s_sim_params.grid_dimensions)[1],
    .mag_filter=GL_LINEAR, .min_filter=GL_LINEAR,
    .wrap_s=GL_REPEAT, .wrap_t=GL_REPEAT
};

struct MainRenderFrames {
    Quad main_view = Quad(MAIN_WINDOW_PARAMS);
    Quad extra = Quad(MAIN_WINDOW_PARAMS);
    Quad wave1 = Quad(SIMULATION_PARAMS);
    Quad wave2 = Quad(SIMULATION_PARAMS);
    Quad wave3 = Quad(SIMULATION_PARAMS); 
    Quad laplacian = Quad(SIMULATION_PARAMS);
    Quad *iter_frames[3];
    MainRenderFrames() {
        this->iter_frames[0] = &this->wave1;
        this->iter_frames[1] = &this->wave2;
        this->iter_frames[2] = &this->wave3;
    }   
} s_renders {};

struct GLSLPrograms {
    GLuint copy, uniform_color, gaussian_3d, laplacian, wave_step;
    GLSLPrograms() {
        this->copy = Quad::make_program_from_path("./shaders/util/copy.frag");
        this->gaussian_3d = MultidimensionalDataQuad::make_program_from_path(
            "./shaders/init-wavepacket/gaussian3d.frag");
        this->uniform_color = Quad::make_program_from_path("./shaders/util/uniform-color.frag");
        this->laplacian = Quad::make_program_from_path("./shaders/util/laplacian.frag");
        this->wave_step = Quad::make_program_from_path("./shaders/wave-step.frag");
    }
} GLSL_PROGRAMS = {};

void time_step(bool is_initial_step=false) {
    s_renders.laplacian.draw(GLSL_PROGRAMS.laplacian, {
        {"tex", {(is_initial_step)? 
                s_renders.iter_frames[0]: s_renders.iter_frames[1]}},
        {"texelDimensions3D", {s_sim_params.grid_dimensions}},
        {"texelDimensions2D",
            {get_2d_from_3d_dimensions(s_sim_params.grid_dimensions)}},
        {"dimensions3D", {s_sim_params.dimensions}},
    });
    Quad *update_frame = (is_initial_step)? 
        s_renders.iter_frames[1]: s_renders.iter_frames[2];
    update_frame->draw(GLSL_PROGRAMS.wave_step,
    {
        {"tex0", {s_renders.iter_frames[0]}},
        {"tex1", {(is_initial_step)?
            s_renders.iter_frames[0]: s_renders.iter_frames[1]}},
        {"laplacianTex1", {&s_renders.laplacian}},
        {"c", {(float)s_sim_params.c}},
        {"dt", {(float)s_sim_params.dt}}
    }
    );
    if (is_initial_step)
        return;
    Quad *wave0 = s_renders.iter_frames[0];
    Quad *wave1 = s_renders.iter_frames[1];
    Quad *wave2 = s_renders.iter_frames[2];
    s_renders.iter_frames[0] = wave1;
    s_renders.iter_frames[1] = wave2;
    s_renders.iter_frames[2] = wave0;
}


int main() {
    s_renders.iter_frames[0]->draw(GLSL_PROGRAMS.gaussian_3d, 
    {
        {"amplitude", {10.0F}},
        {"waveNumber", {Vec3{.ind={0.0, 0.0, 0.0}}}},
        {"texOffset", {Vec3{.ind={0.5, 0.5, 0.5}}}},
        {"sigma", {Vec3{.ind={0.03, 0.03, 0.03}}}},
        {"texelDimensions3D", {s_sim_params.grid_dimensions}},
        {"texelDimensions2D",
            {get_2d_from_3d_dimensions(s_sim_params.grid_dimensions)}}

    });
    time_step(true);
    while (!glfwWindowShouldClose(s_window)) {
        time_step();
        s_renders.main_view.draw(
            GLSL_PROGRAMS.copy, {{"tex", {
                s_renders.iter_frames[0]}}});
         glfwPollEvents();
         glfwSwapBuffers(s_window);
    }
    glfwDestroyWindow(s_window);
    glfwTerminate();
    return 1;
}