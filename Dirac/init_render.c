#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "gl_wrappers/gl_wrappers.h"
#include "init_render.h"
#include "simulation.h"


static struct SimFrames s_sim_frames = {};
static struct SimPrograms s_sim_programs = {};
static struct SimParams s_sim_params = {};

struct ViewParams {
    int view_width;
    int view_height;
    int show_vector_field_view;
    struct DVec4 rotation;
} s_view_params = {};

struct ViewPrograms {
    GLuint view;
    GLuint view_of;
    GLuint translate;
    GLuint vector_view;
} s_view_programs = {};

struct ViewFrames {
    frame_id main_view;
    frame_id sub_views[6];
    frame_id vector_view;
} s_view_frames = {};

void init_view_params() {
    s_view_params.rotation.x = 0.0;
    s_view_params.rotation.y = 0.0;
    s_view_params.rotation.z = 0.0;
    s_view_params.rotation.w = 1.0;
#ifdef __APPLE__
    s_view_params.view_width = 1024;
    s_view_params.view_height = 1024;
#else
    s_view_params.view_width = 512;
    s_view_params.view_height = 512;
#endif
}

void init_view_programs() {
    s_view_programs.view = make_quad_program("./shaders/view.frag");
    s_view_programs.view_of = make_quad_program("./shaders/view-of.frag");
    s_view_programs.translate = make_quad_program("./shaders/translate.frag");
    s_view_programs.vector_view = make_program("./shaders/vector-view.vert",
                                               "./shaders/vector-view.frag");
}

void init_view_frames() {
    s_view_frames.main_view = new_quad(NULL);
    struct TextureParams tex_params = {
        .type=GL_FLOAT,
        .width=s_sim_params.tex_dimensions.width_2d,
        .height=s_sim_params.tex_dimensions.height_2d,
        .generate_mipmap=1,
        .wrap_s=GL_CLAMP_TO_EDGE, .wrap_t=GL_CLAMP_TO_EDGE,
        .min_filter=GL_LINEAR, .mag_filter=GL_LINEAR
    };
    for (int i = 0; i < 6; i++) {
        s_view_frames.sub_views[i] = new_quad(&tex_params);
    }
    int sizeof_vertices = 2*sizeof(struct Vec4)*
                          s_sim_params.tex_dimensions.width_2d*
                          s_sim_params.tex_dimensions.height_2d;
    struct Vec4 *vertices = malloc(sizeof_vertices);
    for (int i = 0; i < s_sim_params.tex_dimensions.height_2d; i++) {
        for (int j = 0; j < s_sim_params.tex_dimensions.width_2d; j++) {
            int w = s_sim_params.tex_dimensions.width_2d;
            int h = s_sim_params.tex_dimensions.height_2d;
            vertices[2*(i*w + j)].x = ((float)j + 0.5)/(float)w;
            vertices[2*(i*w + j)].y = ((float)i + 0.5)/(float)h;
            vertices[2*(i*w + j)].z = 0.0;
            vertices[2*(i*w + j)].w = -0.5;
            vertices[2*(i*w + j) + 1].x = ((float)j + 0.5)/(float)w;
            vertices[2*(i*w + j) + 1].y = ((float)i + 0.5)/(float)h;
            vertices[2*(i*w + j) + 1].z = 0.0;
            vertices[2*(i*w + j) + 1].w = 0.5;
        }
    }
    s_view_frames.vector_view
         = new_frame(&tex_params, (float *)vertices, sizeof_vertices,
                     NULL, -1);

}

void init() {
    init_sim_params(&s_sim_params);
    init_sim_programs(&s_sim_programs);
    init_view_params();
    init_view_programs();
    init_view_frames();
    init_sim_frames(&s_sim_frames, &s_sim_params);
    // getchar();
    glViewport(0, 0,
               s_sim_params.tex_dimensions.width_2d,
               s_sim_params.tex_dimensions.height_2d);
    init_wave_func(&s_sim_frames, &s_sim_programs, &s_sim_params);
}

void view_of(frame_id dst, frame_id src, int offset_v, int offset_h) {
    bind_quad(dst, s_view_programs.view_of);
    set_sampler2D_uniform("tex", src);
    set_ivec2_uniform("outDimensions", s_sim_params.tex_dimensions.width_3d,
                      s_sim_params.tex_dimensions.height_3d);
    set_ivec2_uniform("texDimensions", s_sim_params.tex_dimensions.width_2d,
                      s_sim_params.tex_dimensions.height_2d);
    set_vec2_uniform("offset",
                     (float)offset_v*
                     (float)s_sim_params.tex_dimensions.width_3d/
                     (float)s_sim_params.tex_dimensions.width_2d,
                     (float)offset_h*
                     (float)s_sim_params.tex_dimensions.height_3d/
                     (float)s_sim_params.tex_dimensions.height_2d);
    draw_unbind_quad();
}

static struct DVec3 normalize(struct DVec3 r) {
    double norm = sqrt(r.x*r.x + r.y*r.y + r.z*r.z);
    struct DVec3 v = {.x=r.x/norm, .y=r.y/norm, .z=r.z/norm};
    return v;
}

double length(struct DVec3 r) {
    return sqrt(r.x*r.x + r.y*r.y + r.z*r.z);
}

static struct DVec4 quaternion_multiply(struct DVec4 q1, struct DVec4 q2) {
    struct DVec4 q3 = {
        .w = q1.w*q2.w - q1.x*q2.x - q1.y*q2.y - q1.z*q2.z,
        .x = q1.w*q2.x + q1.x*q2.w + q1.y*q2.z - q1.z*q2.y,
        .y = q1.w*q2.y + q1.y*q2.w + q1.z*q2.x - q1.x*q2.z,
        .z = q1.w*q2.z + q1.z*q2.w + q1.x*q2.y - q1.y*q2.x,
    };
    return q3;
}

static struct DVec3 cross_product(struct DVec3 r1, struct DVec3 r2) {
    struct DVec3 r3 = {
        .x = r1.y*r2.z - r1.z*r2.y,
        .y = - r1.x*r2.z + r1.z*r2.x,
        .z = r1.x*r2.y - r1.y*r2.x,
    };
    return r3;
}

static struct DVec4
rotation_axis_to_quaternion(double angle, struct DVec3 axis) {
    double norm = sqrt(axis.x*axis.x + axis.y*axis.y + axis.z*axis.z);
    for (int i = 0; i < 3; i++) {
        axis.ind[i] = axis.ind[i]/norm;
    }
    double c = cos(angle/2.0);
    double s = sin(angle/2.0);
    struct DVec4 res = {.x=s*axis.x, .y=s*axis.y, .z=s*axis.z, .w=c};
    return res;
}

void render(const struct RenderParams *render_params) {
    if (render_params->user_use &&
        (render_params->user_dx != 0.0 || render_params->user_dy != 0.0)) {
        double angle = 4.0*sqrt(
            render_params->user_dx*render_params->user_dx
             + render_params->user_dy*render_params->user_dy);
        struct DVec3 to_camera = {.x=0.0, .y=0.0, .z=1.0};
        struct DVec3 vel = {.x=render_params->user_dx,
                            .y=render_params->user_dy,
                            .z=0.0};
        struct DVec3 unorm_axis = cross_product(vel, to_camera);
        struct DVec3 axis = normalize(unorm_axis);
        if (length(axis) > (1.0 - 1e-10)
             && length(axis) < (1.0 + 1e-10)) {
            struct DVec4 q_axis
                     = rotation_axis_to_quaternion(angle, axis);
            struct DVec4 tmp
                = quaternion_multiply(s_view_params.rotation, q_axis);
            s_view_params.rotation.x = tmp.x;
            s_view_params.rotation.y = tmp.y;
            s_view_params.rotation.z = tmp.z;
            s_view_params.rotation.w = tmp.w;
        }

    }
    glViewport(0, 0,
               s_sim_params.tex_dimensions.width_2d,
               s_sim_params.tex_dimensions.height_2d);
    do_multiple_steps(&s_sim_frames, &s_sim_programs, &s_sim_params, 2);
    // s_view_params.show_vector_field_view = render_params->user_use2;
    s_view_params.show_vector_field_view = 0;
    if (s_view_params.show_vector_field_view) {
        bind_quad(s_view_frames.sub_views[0], s_view_programs.view);
        float angle = s_sim_params.t*s_sim_params.m
                      *s_sim_params.c*s_sim_params.c;
        set_vec2_uniform("constPhase", cos(angle), sin(angle));
        set_sampler2D_uniform("uTex", s_sim_frames.wave_func_x[0].ind[0]);
        set_sampler2D_uniform("vTex", s_sim_frames.wave_func_x[0].ind[1]);
        draw_unbind_quad();
        spin_expectation(s_sim_programs.spin_expectation,
                         s_view_frames.sub_views[1],
                         s_sim_frames.wave_func_x[0].ind[0]);
        glEnable(GL_DEPTH_TEST);
        bind_frame(s_view_frames.vector_view, s_view_programs.vector_view);
        struct VertexParam vertex_params[2] = {
            {.name="inputData", .size=4,
             .type=GL_FLOAT, .normalized=GL_FALSE,
             .stride=4*sizeof(float), .offset=0},
        };
        set_vertex_attributes(vertex_params, 0);
        set_float_uniform("scale", render_params->user_scroll);
        set_vec4_uniform("rotation",
                         s_view_params.rotation.x,
                         s_view_params.rotation.y,
                         s_view_params.rotation.z,
                         s_view_params.rotation.w);
        set_float_uniform("vecScale", 1.0);
        set_sampler2D_uniform("colTex", s_view_frames.sub_views[0]);
        set_sampler2D_uniform("vecTex", s_view_frames.sub_views[1]);
        set_ivec2_uniform("texelDimensions2D",
                          s_sim_params.tex_dimensions.width_2d,
                          s_sim_params.tex_dimensions.height_2d);
        set_ivec3_uniform("texelDimensions3D",
                          s_sim_params.tex_dimensions.width_3d,
                          s_sim_params.tex_dimensions.height_3d,
                          s_sim_params.tex_dimensions.length_3d);
        // print_user_defined_uniforms();
        glDrawArrays(GL_LINES, 0, 
                     2*s_sim_params.tex_dimensions.width_2d
                     *s_sim_params.tex_dimensions.height_2d);
        // glClear(GL_COLOR_BUFFER_BIT);
        // glClear(GL_DEPTH_BUFFER_BIT);
        // glClear(GL_STENCIL_BUFFER_BIT);
        unbind();
        glDisable(GL_DEPTH_TEST);
        glViewport(0, 0,
                   s_view_params.view_width,
                   s_view_params.view_height);
        bind_quad(s_view_frames.main_view, s_sim_programs.copy);
        set_sampler2D_uniform("tex", s_view_frames.vector_view);
        draw_unbind_quad();
    } else {
        for (int i = 0; i < 2; i++)
            transpose(s_sim_programs.transpose,
                      s_view_frames.sub_views[i],
                      s_sim_frames.wave_func_x[0].ind[i],
                      &s_sim_params.tex_dimensions, 0, 2, 1);
        int which = render_params->which;
        int w2d_w3d = (int)s_sim_params.tex_dimensions.width_2d/
                    (int)s_sim_params.tex_dimensions.width_3d;
        int h2d_h3d = (int)s_sim_params.tex_dimensions.height_2d/
                    (int)s_sim_params.tex_dimensions.height_3d;
        int w = which % w2d_w3d;
        int h = which / h2d_h3d;
        for (int i = 0; i < 2; i++)
            view_of(s_view_frames.sub_views[2+i],
                    s_view_frames.sub_views[i],
                    w, h);
        glViewport(0, 0,
                s_view_params.view_width,
                s_view_params.view_height);
        bind_quad(s_view_frames.main_view, s_view_programs.view);
        set_sampler2D_uniform("uTex", s_view_frames.sub_views[2]);
        set_sampler2D_uniform("vTex", s_view_frames.sub_views[3]);
        float angle = s_sim_params.t*s_sim_params.m
                      *s_sim_params.c*s_sim_params.c;
        set_vec2_uniform("constPhase", cos(angle), sin(angle));
        draw_unbind_quad();
    }

}
