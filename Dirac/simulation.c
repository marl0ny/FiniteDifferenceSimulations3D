#include <math.h>
#include <complex.h>
#include <stdio.h>
#include "simulation.h"
#include "gl_wrappers/gl_wrappers.h"
#include "fft_gl.h"


void init_sim_programs(SimPrograms *programs) {
    programs->copy = make_quad_program("./shaders/copy.frag");
    programs->scale = make_quad_program("./shaders/scale.frag");
    programs->zero = make_quad_program("./shaders/zero.frag");
    programs->complex_product
        = make_quad_program("./shaders/complex-product.frag");
    programs->complex_add2
        = make_quad_program("./shaders/complex-add2.frag");
    programs->complex_add3
        = make_quad_program("./shaders/complex-add3.frag");
    programs->complex_cubed
         = make_quad_program("./shaders/complex-cubed.frag");
    programs->vec_dot_grad
        = make_quad_program("./shaders/vec-dot-grad.frag");
    programs->fft_iter = make_quad_program("./shaders/fft-iter.frag");
    programs->fftshift = make_quad_program("./shaders/fftshift.frag");
    programs->rev_bit_sort2
         = make_quad_program("./shaders/rev-bit-sort2.frag");
    programs->gradient = make_quad_program("./shaders/gradient.frag");
    programs->kinetic_step = make_quad_program("./shaders/kinetic-step.frag");
    programs->potential_step
        = make_quad_program("./shaders/potential-step.frag");
    programs->transpose = make_quad_program("./shaders/transpose.frag");
    programs->init_wave_packet
         = make_quad_program("./shaders/init-wave-packet.frag");
    programs->sigma_dot_vec
        = make_quad_program("./shaders/sigma-dot-vec.frag");
    programs->harmonic_oscillator
        = make_quad_program("./shaders/harmonic-oscillator.frag");
    programs->symmetric_potential
        = make_quad_program("./shaders/symmetric-potential.frag");
    programs->init_boundary
         = make_quad_program("./shaders/init-boundary.frag");
    programs->matrix4_transform
        = make_quad_program("./shaders/matrix4-transform.frag");
    programs->hmat2_transform
        = make_quad_program("./shaders/hmat2-transform.frag");
    programs->spin_expectation
        = make_quad_program("./shaders/spin-expectation.frag");
}

void init_sim_params(SimParams *params) {
    // params->dt = 0.0;
    // params->dt = 0.1;
    // params->dt = 0.002;
    params->dt = 0.000028;
    params->m = 1.0;
    params->q = 1.0;
    params->c = 137.036;
    params->hbar = 1.0;
    params->step_count = 0;
    params->width = 2.0;
    params->height = 2.0;
    params->length = 2.0;
    params->tex_dimensions.width_2d = 512;
    params->tex_dimensions.height_2d = 512;
    params->tex_dimensions.width_3d = 64;
    params->tex_dimensions.height_3d = 64;
    params->tex_dimensions.length_3d = 64;
    params->dx = params->width/(float)params->tex_dimensions.width_3d;
    params->dy = params->height/(float)params->tex_dimensions.height_3d;
    params->dz = params->length/(float)params->tex_dimensions.length_3d;
}

void init_sim_frames(SimFrames *frames, const SimParams *params) {
    struct TextureParams tex_params = {
        .type=GL_FLOAT,
        .width=params->tex_dimensions.width_2d,
        .height=params->tex_dimensions.height_2d,
        .generate_mipmap=1,
        .wrap_s=GL_REPEAT, .wrap_t=GL_REPEAT,
        .min_filter=GL_LINEAR, .mag_filter=GL_LINEAR
    };
    frames->potential = new_quad(&tex_params);
    for (int i = 0; i < 2; i++) {
        frames->fft_iter[i] = new_quad(&tex_params);
    }
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 2; j++) {
            frames->wave_func_x[i].ind[j] = new_quad(&tex_params);
            frames->wave_func_p[i].ind[j] = new_quad(&tex_params);
        }
    }
    for (int i = 0; i < 5; i++) {
        frames->extras[i] = new_quad(&tex_params);
    }
    frames->zero = new_quad(&tex_params);
}

void init_wave_func(const SimFrames *frames, const SimPrograms *programs,
                    const SimParams *params) {
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 2; j++) {
            bind_quad(frames->wave_func_x[i].ind[j],
                      programs->init_wave_packet);
            set_ivec3_uniform("texelDimensions3D",
                            params->tex_dimensions.width_3d,
                            params->tex_dimensions.height_3d,
                            params->tex_dimensions.length_3d);
            set_ivec2_uniform("texelDimensions2D",
                            params->tex_dimensions.width_2d,
                            params->tex_dimensions.height_2d);
            if (j == 0) {
                set_float_uniform("amplitude0", 1.0);
                set_float_uniform("amplitude1", 0.0);
            } else {
                set_float_uniform("amplitude0", 0.0);
                set_float_uniform("amplitude1", 0.0);
            }
            set_vec3_uniform("r0", 0.30, 0.5, 0.35);
            set_vec3_uniform("r1", 0.30, 0.5, 0.35);
            float sigma = 0.1;
            set_vec3_uniform("sigma0", sigma, sigma, sigma);
            set_vec3_uniform("sigma1", sigma, sigma, sigma);
            set_vec3_uniform("wavenumber0", 0.0, 0.0, 0.0);
            set_vec3_uniform("wavenumber1", 0.0, 0.0, 0.0);
            draw_unbind_quad();
        }
    }
}

void complex_add2(GLuint complex_add2_program, frame_id dst_tex,
                  double complex z11, double complex z12, frame_id src_tex1,
                  double complex z21, double complex z22, frame_id src_tex2) {
    if (dst_tex == src_tex1 || dst_tex == src_tex2) {
        fprintf(stderr,
                "complex_add2: unable to use buffer for "
                "simultaneous read and write operation.");
        return;
    }
    bind_quad(dst_tex, complex_add2_program);
    set_vec4_uniform("scale1",
                     creal(z11), cimag(z11), creal(z12), cimag(z12));
    set_sampler2D_uniform("tex1", src_tex1);
    set_vec4_uniform("scale2",
                     creal(z21), cimag(z21), creal(z22), cimag(z22));
    set_sampler2D_uniform("tex2", src_tex2);
    draw_unbind_quad();
}


void complex_add3(GLuint complex_add3_program, frame_id dst_tex,
                  double complex z11, double complex z12, frame_id src_tex1,
                  double complex z21, double complex z22, frame_id src_tex2,
                  double complex z31, double complex z32, frame_id src_tex3) {
    if (dst_tex == src_tex1 || dst_tex == src_tex2 || dst_tex == src_tex3) {
        fprintf(stderr,
                "complex_add3: unable to use buffer for "
                "simultaneous read and write operation.");
        return;
    }
    bind_quad(dst_tex, complex_add3_program);
    set_vec4_uniform("scale1",
                     creal(z11), cimag(z11), creal(z12), cimag(z12));
    set_sampler2D_uniform("tex1", src_tex1);
    set_vec4_uniform("scale2",
                     creal(z21), cimag(z21), creal(z22), cimag(z22));
    set_sampler2D_uniform("tex2", src_tex2);
    set_vec4_uniform("scale3",
                     creal(z31), cimag(z31), creal(z32), cimag(z32));
    set_sampler2D_uniform("tex3", src_tex3);
    draw_unbind_quad();
}


void harmonic_oscillator(const SimFrames *frames,
                         const SimPrograms *programs,
                         const SimParams *params) {
    float t = params->t;
    float kx = 150.0, ky = 150.0, kz = 0.0;
    bind_quad(frames->potential, programs->harmonic_oscillator);
    set_float_uniform("t", t);
    set_float_uniform("omega", 1.50);
    set_vec3_uniform("r0", 0.5, 0.5, 0.5);
    set_vec3_uniform("kr", kx, ky, kz);
    set_ivec3_uniform("texelDimensions3D",
                      params->tex_dimensions.width_3d,
                      params->tex_dimensions.height_3d,
                      params->tex_dimensions.length_3d);
    set_ivec2_uniform("texelDimensions2D",
                      params->tex_dimensions.width_2d,
                      params->tex_dimensions.height_2d);
    draw_unbind_quad();
}

void symmetric_potential(const SimFrames *frames,
                         const SimPrograms *programs,
                         const SimParams *params) {
    bind_quad(frames->potential, programs->symmetric_potential);
    set_float_uniform("q", params->q);
    // set_float_uniform("hbar", params->hbar);
    set_float_uniform("m", params->m);
    set_float_uniform("c", params->c);
    set_float_uniform("k", 50.0);
    set_vec3_uniform("direction", 0.0, 1.0, 0.0);
    set_ivec3_uniform("texelDimensions3D",
                      params->tex_dimensions.width_3d,
                      params->tex_dimensions.height_3d,
                      params->tex_dimensions.length_3d);
    set_ivec2_uniform("texelDimensions2D",
                      params->tex_dimensions.width_2d,
                      params->tex_dimensions.height_2d);
    set_vec3_uniform("dimensions3D",
                     params->width, params->height, params->length);
    draw_unbind_quad();
}

void zero_potential(const SimFrames *frames,
                         const SimPrograms *programs,
                         const SimParams *params) {
    bind_quad(frames->potential, programs->zero);
    draw_unbind_quad();
}

void set_hmat2_uniform(const char *name, double complex *mat) {
    set_vec4_uniform(name,
                     creal(mat[0]), creal(mat[3]),
                     creal(mat[1]), cimag(mat[1]));
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

void spin_expectation(GLuint spin_expectation_program,
                      frame_id dst, frame_id src) {
    bind_quad(dst, spin_expectation_program);
    double complex sigma_x[2][2] = {{0.0, 1.0}, {1.0, 0.0}};
    double complex sigma_y[2][2] = {{0.0, -I}, {I, 0.0}};
    double complex sigma_z[2][2] = {{1.0, 0.0}, {0.0, -1.0}};
    set_sampler2D_uniform("tex", src);
    set_hmat2_uniform("sigmaX", (double complex *)&sigma_x[0][0]);
    set_hmat2_uniform("sigmaY", (double complex *)&sigma_y[0][0]);
    set_hmat2_uniform("sigmaZ", (double complex *)&sigma_z[0][0]);
    draw_unbind_quad();
}

static void potential_step_single_spinor(const SimFrames *frames,
                                         const SimPrograms *programs,
                                         const SimParams *params,
                                         float dt, int spinor_index,
                                         frame_id wave_func_next_i,
                                         frame_id wave_func_last_0,
                                         frame_id wave_func_last_1) {
    bind_quad(wave_func_next_i, programs->potential_step);
    set_sampler2D_uniform("uTex", wave_func_last_0);
    set_sampler2D_uniform("vTex", wave_func_last_1);
    // set_int_uniform("useVectorPotential", 1);
    set_sampler2D_uniform("potentialTex", frames->potential);
    set_float_uniform("dt", dt);
    set_float_uniform("c", params->c);
    set_float_uniform("hbar", params->hbar);
    set_int_uniform("spinorIndex", spinor_index);
    draw_unbind_quad();
}

static void potential_step(const SimFrames *frames,
                           const SimPrograms *programs,
                           const SimParams *params,
                           float dt,
                           const struct WaveFuncFrames *psi_f,
                           const struct WaveFuncFrames *psi_i) {
    frame_id psi0_f = psi_f->ind[0];
    frame_id psi1_f = psi_f->ind[1];
    frame_id psi0_i = psi_i->ind[0];
    frame_id psi1_i = psi_i->ind[1];
    // Potential step for first two components
    potential_step_single_spinor(frames, programs, params,
                                 dt, 0, psi0_f, psi0_i, psi1_i);
    // Potential step for the last two components
    potential_step_single_spinor(frames, programs, params,
                                 dt, 1, psi1_f, psi0_i, psi1_i);
}

static void to_momentum_space(const SimFrames *frames, 
                              const SimPrograms *programs,
                              const SimParams *params,
                              const struct WaveFuncFrames *wave_func_p,
                              const struct WaveFuncFrames *wave_func_x) {
    frame_id res = 0;
    // Fourier transform the first two components
    res = fft3d(programs->rev_bit_sort2, programs->fft_iter,
                wave_func_x->ind[0],
                frames->fft_iter[0], frames->fft_iter[1],
                &params->tex_dimensions);
    bind_quad(wave_func_p->ind[0], programs->copy);
    set_sampler2D_uniform("tex", res);
    draw_unbind_quad();
    // Fourier transform the last two components
    res = fft3d(programs->rev_bit_sort2, programs->fft_iter,
                wave_func_x->ind[1],
                frames->fft_iter[0], frames->fft_iter[1],
                &params->tex_dimensions);
    bind_quad(wave_func_p->ind[1], programs->copy);
    set_sampler2D_uniform("tex", res);
    draw_unbind_quad();
}

static void kinetic_step_single_spinor(const SimFrames *frames,
                                       const SimPrograms *programs,
                                       const SimParams *params,
                                       float dt, int spinor_index,
                                       frame_id wave_func_next_i,
                                       frame_id wave_func_last_0,
                                       frame_id wave_func_last_1) {
    bind_quad(wave_func_next_i, programs->kinetic_step);
    set_ivec3_uniform("texelDimensions3D",
                      params->tex_dimensions.width_3d,
                      params->tex_dimensions.height_3d,
                      params->tex_dimensions.length_3d);
    set_ivec2_uniform("texelDimensions2D",
                      params->tex_dimensions.width_2d,
                      params->tex_dimensions.height_2d);
    set_vec3_uniform("dimensions3D",
                     params->width, params->height, params->length);
    set_sampler2D_uniform("uTex", wave_func_last_0);
    set_sampler2D_uniform("vTex", wave_func_last_1);
    set_float_uniform("dt", dt);
    set_float_uniform("m", params->m);
    set_float_uniform("c", params->c);
    set_float_uniform("hbar", params->hbar);
    set_int_uniform("spinorIndex", spinor_index);
    draw_unbind_quad();
}

static void kinetic_step(const SimFrames *frames,
                         const SimPrograms *programs,
                         const SimParams *params,
                         float dt,
                         const struct WaveFuncFrames *psi_f,
                         const struct WaveFuncFrames *psi_i) {
    frame_id psi0_f = psi_f->ind[0];
    frame_id psi1_f = psi_f->ind[1];
    frame_id psi0_i = psi_i->ind[0];
    frame_id psi1_i = psi_i->ind[1];
    // Kinetic step for first two components
    kinetic_step_single_spinor(frames, programs, params,
                               dt, 0, psi0_f, psi0_i, psi1_i);
    // Kinetic step for the last two components
    kinetic_step_single_spinor(frames, programs, params,
                               dt, 1, psi1_f, psi0_i, psi1_i);
}

static void to_position_space(const SimFrames *frames, 
                              const SimPrograms *programs,
                              const SimParams *params,
                              const struct WaveFuncFrames *wave_func_x,
                              const struct WaveFuncFrames *wave_func_p) {
    frame_id res = 0;
    // Inverse Fourier transform the first two components
    res = ifft3d(programs->rev_bit_sort2, programs->fft_iter,
                 wave_func_p->ind[0],
                 frames->fft_iter[0], frames->fft_iter[1],
                 &params->tex_dimensions);
    bind_quad(wave_func_x->ind[0], programs->copy);
    set_sampler2D_uniform("tex", res);
    draw_unbind_quad();
    // Inverse Fourier transform the last two components
    res = ifft3d(programs->rev_bit_sort2, programs->fft_iter,
                 wave_func_p->ind[1],
                 frames->fft_iter[0], frames->fft_iter[1],
                 &params->tex_dimensions);
    bind_quad(wave_func_x->ind[1], programs->copy);
    set_sampler2D_uniform("tex", res);
    draw_unbind_quad();
}

void step(const SimFrames *frames, const SimPrograms *programs,
          const SimParams *params) {
    
    // Compute V
    // harmonic_oscillator(frames, programs, params);
    symmetric_potential(frames, programs, params);
    // zero_potential(frames, programs, params);

    // Potential step
    potential_step(frames, programs, params, params->dt/2.0,
                   &frames->wave_func_x[1],
                   &frames->wave_func_x[0]);

    // Momentum step
    to_momentum_space(frames, programs, params,
                      &frames->wave_func_p[0], &frames->wave_func_x[1]);
    kinetic_step(frames, programs, params, params->dt,
                 &frames->wave_func_p[1], &frames->wave_func_p[0]);
    to_position_space(frames, programs, params,
                      &frames->wave_func_x[1], &frames->wave_func_p[1]);
    
    // Potential step again
    potential_step(frames, programs, params, params->dt/2.0,
                   &frames->wave_func_x[0],
                   &frames->wave_func_x[1]);
}


/* static void swap3(frame_id *f0, frame_id *f1, frame_id *f2) {
    frame_id tmp0 = *f0, tmp1 = *f1, tmp2 = *f2;
    *f0 = tmp1, *f1 = tmp2, *f2 = tmp0;
}*/

void do_multiple_steps(SimFrames *frames, SimPrograms *programs,
                       SimParams *params, int number_of) {
    for (int _ = 0; _ < number_of; _++) {
        step(frames, programs, params);
        params->t += params->dt;
    }
}
