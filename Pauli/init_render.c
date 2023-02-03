#include <stdio.h>

#include "gl_wrappers/gl_wrappers.h"
#include "init_render.h"
#include "simulation.h"


static struct SimFrames s_sim_frames = {};
static struct SimPrograms s_sim_programs = {};
static struct SimParams s_sim_params = {};

struct ViewParams {
    int view_width;
    int view_height;
} s_view_params = {};

struct ViewPrograms {
    GLuint view;
    GLuint view_of;
    GLuint translate;
} s_view_programs = {};

struct ViewFrames {
    frame_id main_view;
    frame_id sub_views[3];
} s_view_frames = {};

void init_view_params() {
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
    for (int i = 0; i < 3; i++) {
        s_view_frames.sub_views[i] = new_quad(&tex_params);
    }
}

void init() {
    init_sim_params(&s_sim_params);
    init_sim_programs(&s_sim_programs);
    init_view_params();
    init_view_programs();
    init_view_frames();
    init_sim_frames(&s_sim_frames, &s_sim_params);
    glViewport(0, 0,
               s_sim_params.tex_dimensions.width_2d,
               s_sim_params.tex_dimensions.height_2d);
    init_boundary(&s_sim_frames, &s_sim_programs, &s_sim_params);
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

void render(const struct RenderParams *render_params) {
    glViewport(0, 0,
               s_sim_params.tex_dimensions.width_2d,
               s_sim_params.tex_dimensions.height_2d);
    do_multiple_steps(&s_sim_frames, &s_sim_programs, &s_sim_params, 6);
    transpose(s_sim_programs.transpose,
              s_view_frames.sub_views[0], s_sim_frames.wave_func[0],
              &s_sim_params.tex_dimensions, 0, 2, 1);
    view_of(s_view_frames.sub_views[1], s_view_frames.sub_views[0], 4, 4);
    glViewport(0, 0,
               s_view_params.view_width,
               s_view_params.view_height);
    bind_quad(s_view_frames.main_view, s_view_programs.view);
    set_sampler2D_uniform("waveFuncTex", s_view_frames.sub_views[1]);
    draw_unbind_quad();

}
