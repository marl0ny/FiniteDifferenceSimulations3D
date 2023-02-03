#include <math.h>
#include <complex.h>
#include <stdio.h>
#include "simulation.h"
#include "gl_wrappers/gl_wrappers.h"


void init_sim_programs(SimPrograms *programs) {
    programs->copy = make_quad_program("./shaders/copy.frag");
    programs->scale = make_quad_program("./shaders/scale.frag");
    programs->zero = make_quad_program("./shaders/zero.frag");
    programs->complex_scale
        = make_quad_program("./shaders/complex-scale.frag");
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
    programs->laplacian = make_quad_program("./shaders/laplacian.frag");
    programs->laplacian_periodic
        = make_quad_program("./shaders/laplacian-periodic.frag");
    programs->curl = make_quad_program("./shaders/curl.frag");
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
}

void init_sim_params(SimParams *params) {
    // params->dt = 0.0;
    // params->dt = 0.002;
    params->dt = 0.004;
    params->m = 1.0;
    params->q = 1.0;
    params->c = 137.036;
    params->hbar = 1.0;
    params->step_count = 0;
    params->width = 10.0;
    params->height = 10.0;
    params->length = 10.0;
    params->tex_dimensions.width_2d = 512;
    params->tex_dimensions.height_2d = 512;
    params->tex_dimensions.width_3d = 64;
    params->tex_dimensions.height_3d = 64;
    params->tex_dimensions.length_3d = 64;
    /*params->width = 18.0;
    params->height = 18.0;
    params->length = 18.0;
    params->tex_dimensions.width_2d = 1000;
    params->tex_dimensions.height_2d = 1000;
    params->tex_dimensions.width_3d = 100;
    params->tex_dimensions.height_3d = 100;
    params->tex_dimensions.length_3d = 100;*/
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
        .wrap_s=GL_CLAMP_TO_EDGE, .wrap_t=GL_CLAMP_TO_EDGE,
        .min_filter=GL_LINEAR, .mag_filter=GL_LINEAR
    };
    for (int i = 0; i < 3; i++) {
        if (i == 1) {
            tex_params.min_filter = GL_NEAREST;
            tex_params.mag_filter = GL_NEAREST;
        } else {
            tex_params.min_filter = GL_LINEAR;
            tex_params.mag_filter = GL_LINEAR;
        }
    }
    frames->boundary_mask = new_quad(&tex_params);
    frames->potential = new_quad(&tex_params);
    frames->magnetic = new_quad(&tex_params);
    frames->sigma_dot_magnetic = new_quad(&tex_params);
    frames->magnetic_psi = new_quad(&tex_params);
    frames->nonlinear = new_quad(&tex_params);
    for (int i = 0; i < 3; i++) {
        frames->wave_func[i] = new_quad(&tex_params);
    }
    frames->potential_psi = new_quad(&tex_params);
    frames->del2_psi = new_quad(&tex_params);
    frames->a_dot_grad_psi = new_quad(&tex_params);
    frames->kinetic_psi = new_quad(&tex_params);
    frames->h_psi = new_quad(&tex_params);
    frames->h_and_nonlinear_psi = new_quad(&tex_params);
    for (int i = 0; i < 5; i++) {
        frames->extras[i] = new_quad(&tex_params);
    }
    frames->zero = new_quad(&tex_params);
}

void init_boundary(const SimFrames *frames, const SimPrograms *programs,
                   const SimParams *params) {
    bind_quad(frames->boundary_mask, programs->init_boundary);
    set_ivec3_uniform("texelDimensions3D",
                      params->tex_dimensions.width_3d,
                      params->tex_dimensions.height_3d,
                      params->tex_dimensions.length_3d);
    set_ivec2_uniform("texelDimensions2D",
                      params->tex_dimensions.width_2d,
                      params->tex_dimensions.height_2d);
    set_vec3_uniform("dr", params->dx, params->dy, params->dz);
    set_vec3_uniform("dimensions3D",
                     params->width, params->height, params->length);
    draw_unbind_quad();
    bind_quad(frames->zero, programs->zero);
    draw_unbind_quad();
}

void init_wave_func(const SimFrames *frames, const SimPrograms *programs,
                    const SimParams *params) {
    for (int i = 0; i < 3; i++) {
        bind_quad(frames->wave_func[i], programs->init_wave_packet);
        set_ivec3_uniform("texelDimensions3D",
                          params->tex_dimensions.width_3d,
                          params->tex_dimensions.height_3d,
                          params->tex_dimensions.length_3d);
        set_ivec2_uniform("texelDimensions2D",
                          params->tex_dimensions.width_2d,
                          params->tex_dimensions.height_2d);
        set_sampler2D_uniform("boundaryMaskTex", frames->boundary_mask);
        set_float_uniform("amplitude0", 1.0);
        set_float_uniform("amplitude1", 0.0);
        set_vec3_uniform("r0", 0.35, 0.5, 0.35);
        set_vec3_uniform("r1", 0.35, 0.5, 0.35);
        float sigma = 0.1;
        set_vec3_uniform("sigma0", sigma, sigma, sigma);
        set_vec3_uniform("sigma1", sigma, sigma, sigma);
        set_vec3_uniform("wavenumber0", 0.0, 0.0, 0.0);
        set_vec3_uniform("wavenumber1", 0.0, 0.0, 0.0);
        draw_unbind_quad();
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
    set_float_uniform("k", 250.0);
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

void step(const SimFrames *frames, const SimPrograms *programs,
          const SimParams *params) {
    float dt = params->dt;
    // Compute V
    // harmonic_oscillator(frames, programs, params);
    symmetric_potential(frames, programs, params);
    // zero_potential(frames, programs, params);
    // Compute magnetic field
    bind_quad(frames->magnetic, programs->curl);
    set_sampler2D_uniform("tex", frames->potential);
    set_ivec3_uniform("texelDimensions3D",
                      params->tex_dimensions.width_3d,
                      params->tex_dimensions.height_3d,
                      params->tex_dimensions.length_3d);
    set_ivec2_uniform("texelDimensions2D",
                      params->tex_dimensions.width_2d,
                      params->tex_dimensions.height_2d);
    set_vec3_uniform("dr", params->dx, params->dy, params->dz);
    set_vec3_uniform("dimensions3D",
                     params->width, params->height, params->length);
    draw_unbind_quad();
    // Compute sigma dot magnetic field term
    bind_quad(frames->sigma_dot_magnetic, programs->sigma_dot_vec);
    double complex sigma_x[2][2] = {{0.0, 1.0}, {1.0, 0.0}};
    double complex sigma_y[2][2] = {{0.0, -I}, {I, 0.0}};
    double complex sigma_z[2][2] = {{1.0, 0.0}, {0.0, -1.0}};
    set_sampler2D_uniform("tex", frames->magnetic);
    set_hmat2_uniform("sigmaX", (double complex *)&sigma_x[0][0]);
    set_hmat2_uniform("sigmaY", (double complex *)&sigma_y[0][0]);
    set_hmat2_uniform("sigmaZ", (double complex *)&sigma_z[0][0]);
    draw_unbind_quad();
    // Compute magnetic psi term
    bind_quad(frames->magnetic_psi, programs->hmat2_transform);
    set_sampler2D_uniform("tex1", frames->sigma_dot_magnetic);
    set_sampler2D_uniform("tex2", frames->wave_func[1]);
    draw_unbind_quad();
    // Compute V psi
    frame_id potential = frames->extras[0];
    bind_quad(potential, programs->matrix4_transform);
    float matrix[4][4] = {{0.0, 0.0, 0.0, 1.0},
                          {0.0, 0.0, 0.0, 0.0},
                          {0.0, 0.0, 0.0, 1.0},
                          {0.0, 0.0, 0.0, 0.0}};
    set_matrix4_uniform("matrix", (float *)&matrix[0][0]);
    set_sampler2D_uniform("tex", frames->potential);
    draw_unbind_quad();
    bind_quad(frames->potential_psi, programs->complex_product);
    set_sampler2D_uniform("tex1", frames->wave_func[1]);
    set_sampler2D_uniform("tex2", potential);
    draw_unbind_quad();
    // Compute the discrete Laplacian of psi
    bind_quad(frames->del2_psi, programs->laplacian_periodic);
    set_sampler2D_uniform("tex", frames->wave_func[1]);
    // set_sampler2D_uniform("boundaryMaskTex", frames->boundary_mask);
    set_ivec3_uniform("texelDimensions3D",
                      params->tex_dimensions.width_3d,
                      params->tex_dimensions.height_3d,
                      params->tex_dimensions.length_3d);
    set_ivec2_uniform("texelDimensions2D",
                      params->tex_dimensions.width_2d,
                      params->tex_dimensions.height_2d);
    set_vec3_uniform("dr", params->dx, params->dy, params->dz);
    set_vec3_uniform("dimensions3D",
                     params->width, params->height, params->length);
    draw_unbind_quad();
    // Compute the A dot p psi term
    bind_quad(frames->a_dot_grad_psi, programs->vec_dot_grad);
    set_vec4_uniform("scale", 1.0, 0.0, 1.0, 0.0);
    set_sampler2D_uniform("tex1", frames->potential);
    set_sampler2D_uniform("tex2", frames->wave_func[1]);
    set_ivec3_uniform("texelDimensions3D",
                      params->tex_dimensions.width_3d,
                      params->tex_dimensions.height_3d,
                      params->tex_dimensions.length_3d);
    set_ivec2_uniform("texelDimensions2D",
                      params->tex_dimensions.width_2d,
                      params->tex_dimensions.height_2d);
    set_vec3_uniform("dr", params->dx, params->dy, params->dz);
    set_vec3_uniform("dimensions3D",
                     params->width, params->height, params->length);
    draw_unbind_quad();
    // Compute kinetic term
    float hbar2_2m = params->hbar*params->hbar/(2.0*params->m);
    double complex iqhbar_mc = I*params->hbar*params->q/(params->m*params->c);
    complex_add2(programs->complex_add2, frames->kinetic_psi,
                 -hbar2_2m, -hbar2_2m, frames->del2_psi,
                 iqhbar_mc, iqhbar_mc, frames->a_dot_grad_psi);
    // Compute H psi
    double complex qhbar_2mc
        = (params->q*params->hbar)/(2.0*params->m*params->c);
    complex_add3(programs->complex_add3, frames->h_psi,
                 1.0, 1.0, frames->kinetic_psi,
                 1.0, 1.0, frames->potential_psi,
                 qhbar_2mc, qhbar_2mc, frames->magnetic_psi);
    // Nonlinear term
    bind_quad(frames->nonlinear, programs->complex_cubed);
    set_sampler2D_uniform("tex", frames->wave_func[1]);
    draw_unbind_quad();
    // Compute H psi + nonlinear psi
    complex_add2(programs->complex_add2, frames->h_and_nonlinear_psi,
                 1.0, 1.0, frames->h_psi,
                 0.0, 0.0, frames->nonlinear);
    // step
    float dt_hbar = dt/params->hbar;
    complex_add2(programs->complex_add2, frames->wave_func[2],
                 -I*dt_hbar, -I*dt_hbar, frames->h_and_nonlinear_psi,
                 1.0, 1.0, frames->wave_func[0]);
}


static void swap3(frame_id *f0, frame_id *f1, frame_id *f2) {
    frame_id tmp0 = *f0, tmp1 = *f1, tmp2 = *f2;
    *f0 = tmp1, *f1 = tmp2, *f2 = tmp0;
}

void do_multiple_steps(SimFrames *frames, SimPrograms *programs,
                       SimParams *params, int number_of) {
    for (int _ = 0; _ < number_of; _++) {
        if (params->step_count == 0)  params->dt *= 0.5;
        else if (params->step_count == 1) params->dt *= 2.0;
        step(frames, programs, params);
        params->t += params->dt;
        params->step_count++;
        swap3(&frames->wave_func[0],
              &frames->wave_func[1],
              &frames->wave_func[2]);
    }
}
