#include "gl_wrappers/gl_wrappers.h"
#include "texture_dimensions.h"

#ifdef __cplusplus
extern "C" {
#endif



#ifndef _SIMULATION_
#define _SIMULATION_

struct SimPrograms {
    GLuint copy, scale, zero;
    GLuint complex_scale;
    GLuint complex_product;
    GLuint complex_add2;
    GLuint complex_add3;
    GLuint complex_cubed;
    GLuint vec_dot_grad;
    GLuint matrix4_transform;
    GLuint hmat2_transform;
    GLuint curl;
    GLuint transpose;
    GLuint sigma_dot_vec;
    GLuint init_wave_packet;
    GLuint harmonic_oscillator;
    GLuint symmetric_potential;
    GLuint init_boundary;
    GLuint spin_expectation;
    GLuint fft_iter;
    GLuint fftshift;
    GLuint rev_bit_sort2;
    GLuint gradient;
    GLuint kinetic_step;
    GLuint potential_step;
};

struct WaveFuncFrames {
    frame_id ind[2];
};

struct SimFrames {
    frame_id potential;
    frame_id fft_iter[2];
    struct WaveFuncFrames wave_func_x[2];
    struct WaveFuncFrames wave_func_p[2];
    frame_id extras[5];
    frame_id zero;
};

struct SimParams {
    float m, q, c, hbar, dt, t;
    float width, height, length;
    float dx, dy, dz;
    int step_count;
    struct TextureDimensions tex_dimensions;
};

typedef struct SimPrograms SimPrograms;
typedef struct SimFrames SimFrames;
typedef struct SimParams SimParams;


void init_sim_programs(SimPrograms *programs);

void init_sim_params(SimParams *params);

void init_sim_frames(SimFrames *frames, const SimParams *params);

void init_wave_func(const SimFrames *frames, const SimPrograms *programs,
                    const SimParams *params);

void transpose(GLuint transpose_program,
               frame_id dst, frame_id src,
               const struct TextureDimensions *tex_dimensions,
               int permutation0, int permutation1, int permutation2);

void translate(GLuint translate_program, frame_id dst, frame_id src,
               double rx, double ry, double rz,
               const struct TextureDimensions *tex_dimensions);

void spin_expectation(GLuint spin_expectation_program,
                      frame_id dst, frame_id src);

void step(const SimFrames *frames, const SimPrograms *programs,
          const SimParams *params);

void do_multiple_steps(SimFrames *frames, SimPrograms *programs,
                       SimParams *params, int number_of);

#endif

#ifdef __cplusplus
}
#endif
