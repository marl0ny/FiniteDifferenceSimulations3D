#include "gl_wrappers/gl_wrappers.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _SIMULATION_
#define _SIMULATION_


struct SimPrograms {
    GLuint copy, zero, scale;
    GLuint translate, curl;
    GLuint transpose;
    GLuint oscillating_charge;
    GLuint electric, magnetic;
};

struct SimFrames {
    frame_id electric[2];
    frame_id translated_electric;
    frame_id curl_electric;
    frame_id magnetic[2];
    frame_id translated_magnetic;
    frame_id curl_magnetic;
    frame_id current;
    frame_id zero;
    frame_id extras[3];
};

struct TextureDimensions {
    int width_3d, height_3d, length_3d;
    int width_2d, height_2d;
};

struct SimParams {
    float dt, t;
    float epsilon, mu, sigma, sigma_m;
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

void transpose(GLuint transpose_program,
               frame_id dst, frame_id src,
               const struct TextureDimensions *tex_dimensions,
               int permutation0, int permutation1, int permutation2);

void translate(GLuint translate_program, frame_id dst, frame_id src,
               double rx, double ry, double rz,
               const struct TextureDimensions *tex_dimensions);

void step(SimFrames *frames, SimPrograms *programs,
          SimParams *params);

void do_multiple_steps(SimFrames *frames, SimPrograms *programs,
                       SimParams *params, int number_of);


#endif

#ifdef __cplusplus
}
#endif
