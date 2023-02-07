#include <math.h>
#include <complex.h>
#include <stdio.h>
#include "simulation.h"
#include "gl_wrappers/gl_wrappers.h"


void init_sim_programs(SimPrograms *programs) {
    programs->copy = make_quad_program("./shaders/copy.frag");
    programs->zero = make_quad_program("./shaders/zero.frag");
    programs->scale = make_quad_program("./shaders/scale.frag");
    programs->translate = make_quad_program("./shaders/translate.frag");
    programs->curl = make_quad_program("./shaders/curl.frag");
    programs->transpose = make_quad_program("./shaders/transpose.frag");
    programs->electric = make_quad_program("./shaders/electric.frag");
    programs->magnetic = make_quad_program("./shaders/magnetic.frag");
    programs->oscillating_charge
        = make_quad_program("./shaders/oscillating-charge.frag");
}

void init_sim_params(SimParams *params) {
    params->tex_dimensions.width_3d = 64;
    params->tex_dimensions.height_3d = 64;
    params->tex_dimensions.length_3d = 64;
    params->tex_dimensions.width_2d = 512;
    params->tex_dimensions.height_2d = 512;
    params->dt = 0.005;
    params->t = 0.0;
    params->epsilon = 1.0;
    params->mu = 1.0;
    params->sigma = 0.0;
    params->sigma_m = 0.0;
    params->width = (float)params->tex_dimensions.width_3d;
    params->height = (float)params->tex_dimensions.height_3d;
    params->length = (float)params->tex_dimensions.length_3d;
    params->dx = 1.0;
    params->dy = 1.0;
    params->dz = 1.0;
    params->step_count = 0;
}

void init_sim_frames(SimFrames *frames, const SimParams *params) {
    struct TextureParams tex_params ={
        .type=GL_FLOAT,
        .width=params->tex_dimensions.width_2d,
        .height=params->tex_dimensions.height_2d,
        .generate_mipmap=1,
        .wrap_s=GL_REPEAT, .wrap_t=GL_REPEAT,
        .min_filter=GL_LINEAR, .mag_filter=GL_LINEAR,
    };
    for (int i = 0; i < 2; i++) {
        frames->electric[i] = new_quad(&tex_params);
        frames->magnetic[i] = new_quad(&tex_params);
        frames->extras[i] = new_quad(&tex_params);
    }
    frames->extras[2] = new_quad(&tex_params);
    frames->translated_electric = new_quad(&tex_params);
    frames->curl_electric = new_quad(&tex_params);
    frames->translated_magnetic = new_quad(&tex_params);
    frames->curl_magnetic = new_quad(&tex_params);
    frames->zero = new_quad(&tex_params);
    frames->current = new_quad(&tex_params);
}

void transpose(GLuint transpose_program, frame_id dst, frame_id src,
               const struct TextureDimensions *tex_dimensions,
               int permutation0, int permutation1, int permutation2) {
    bind_quad(dst, transpose_program);
    set_ivec3_uniform("texelDimensions3D",
                      tex_dimensions->width_3d,
                      tex_dimensions->height_3d,
                      tex_dimensions->length_3d);
    set_ivec2_uniform("texelDimensions2D",
                      tex_dimensions->width_2d,
                      tex_dimensions->height_2d);
    set_ivec3_uniform("permutation", permutation0, permutation1, permutation2);
    set_sampler2D_uniform("tex", src);
    draw_unbind_quad();
}

void translate(GLuint translate_program, frame_id dst, frame_id src,
               double rx, double ry, double rz,
               const struct TextureDimensions *tex_dimensions) {
    bind_quad(dst, translate_program);
    set_vec3_uniform("r", rx, ry, rz);
    set_sampler2D_uniform("tex", src);
    set_ivec3_uniform("texelDimensions3D",
                      tex_dimensions->width_3d,
                      tex_dimensions->height_3d,
                      tex_dimensions->length_3d);
    set_ivec2_uniform("texelDimensions2D",
                      tex_dimensions->width_2d,
                      tex_dimensions->height_2d);
    draw_unbind_quad();
}

/*static void swap3(frame_id *f0, frame_id *f1, frame_id *f2) {
    frame_id tmp0 = *f0, tmp1 = *f1, tmp2 = *f2;
    *f0 = tmp1, *f1 = tmp2, *f2 = tmp0;
    }*/

static void swap(frame_id *f0, frame_id *f1) {
    frame_id tmp = *f0;
    *f0 = *f1;
    *f1 = tmp;
}

void oscillating_charge(GLuint program, frame_id dst,
                        const struct TextureDimensions *dimensions,
                        float t,
                        float width, float height, float length) {
    bind_quad(dst, program);
    set_float_uniform("t", t);
    set_vec3_uniform("oscillationAmplitude",
                     0.0,
                     0.0,
                     0.1*(float)dimensions->length_3d);
    set_float_uniform("phi", 0.0);
    set_float_uniform("omega", 0.5);
    set_float_uniform("chargeAmplitude", 10.0);
    set_vec3_uniform("sigma",
                     0.01*(float)dimensions->width_3d,
                     0.01*(float)dimensions->height_3d,
                     0.01*(float)dimensions->length_3d);
    set_ivec3_uniform("texelDimensions3D",
                      dimensions->width_3d,
                      dimensions->height_3d,
                      dimensions->length_3d);
    set_ivec2_uniform("texelDimensions2D",
                      dimensions->width_2d,
                      dimensions->height_2d);
    set_vec3_uniform("dimensions3D", width, height, length);
    draw_unbind_quad();
}

/*
The names of variables follow the convention used on page
68 - 69 of the Computational Electrodynamics book by
Taflove A. and Hagness S. The Finite difference discretization of 
Maxwell's equation is based on eq. 3.7 and 3.8 of the same book (pg. 69),
and a basic introduction to the Yee algorithm is found on pg 78 to 79.
*/
void step(SimFrames *frames, SimPrograms *programs,
          SimParams *params) {
    const struct TextureDimensions *dimensions = &params->tex_dimensions;
    // Compute the current
    oscillating_charge(programs->oscillating_charge, frames->current,
                       dimensions, params->t,
                       params->width, params->height, params->length);
    // Compute curl of magnetic field
    bind_quad(frames->curl_magnetic, programs->curl);
    set_int_uniform("sampleOffset", -1);
    set_sampler2D_uniform("tex", frames->magnetic[0]);
    set_ivec3_uniform("texelDimensions3D",
                      dimensions->width_3d,
                      dimensions->height_3d,
                      dimensions->length_3d);
    set_ivec2_uniform("texelDimensions2D",
                      dimensions->width_2d,
                      dimensions->height_2d);
    set_vec3_uniform("dr", params->dx, params->dy, params->dz);
    set_vec3_uniform("dimensions3D",
                     params->width, params->height, params->length);
    draw_unbind_quad();
    // Step electric field
    bind_quad(frames->electric[1], programs->electric);
    set_float_uniform("dt", params->dt);
    set_float_uniform("epsilon", params->epsilon);
    set_float_uniform("sigma", params->sigma);
    set_sampler2D_uniform("texCurlHField", frames->curl_magnetic);
    set_sampler2D_uniform("texEField", frames->electric[0]);
    set_sampler2D_uniform("texJ", frames->current);
    draw_unbind_quad();
    swap(&frames->electric[0], &frames->electric[1]);
    // Compute the current
    oscillating_charge(programs->oscillating_charge, frames->current,
                       dimensions, params->t + 0.5*params->dt,
                       params->width, params->height, params->length);
    // Compute curl of electric field
    bind_quad(frames->curl_electric, programs->curl);
    set_int_uniform("sampleOffset", 0);
    set_sampler2D_uniform("tex", frames->electric[0]);
    set_ivec3_uniform("texelDimensions3D",
                      dimensions->width_3d,
                      dimensions->height_3d,
                      dimensions->length_3d);
    set_ivec2_uniform("texelDimensions2D",
                      dimensions->width_2d,
                      dimensions->height_2d);
    set_vec3_uniform("dr", params->dx, params->dy, params->dz);
    set_vec3_uniform("dimensions3D",
                     params->width, params->height, params->length);
    draw_unbind_quad();
    // Step magnetic field
    bind_quad(frames->magnetic[1], programs->magnetic);
    set_float_uniform("dt", params->dt);
    set_float_uniform("mu", params->mu);
    set_float_uniform("sigma", params->sigma_m);
    set_sampler2D_uniform("texCurlEField", frames->curl_electric);
    set_sampler2D_uniform("texHField", frames->magnetic[0]);
    draw_unbind_quad();
    swap(&frames->magnetic[0], &frames->magnetic[1]);
    params->t += params->dt;
}

void do_multiple_steps(SimFrames *frames, SimPrograms *programs,
                       SimParams *params, int number_of) {
    // TODO!
}

