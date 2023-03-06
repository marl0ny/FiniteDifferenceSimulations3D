#include "fft_gl.h"
#include <stdio.h>

void swap2(frame_id *f1, frame_id *f2) {
    frame_id tmp = *f2;
    *f2 = *f1;
    *f1 = tmp;
}

const int ORIENTATION_0 = 0;
const int ORIENTATION_1 = 1;
const int ORIENTATION_2 = 2;
const int IS_INVERSE = 1;
const int NOT_IS_INVERSE = 0;
frame_id fft_iter(int fft_iter_program, 
                  frame_id fft_iter1, frame_id fft_iter2,
                  int orientation, int is_inverse,
                  const struct TextureDimensions *tex_dimensions) {
    frame_id iter[2] = {fft_iter1, fft_iter2};
    int size = 0;
    switch(orientation) {
        case ORIENTATION_0:
        size = tex_dimensions->width_3d;
        break;
        case ORIENTATION_1:
        size = tex_dimensions->height_3d;
        break;
        case ORIENTATION_2:
        size = tex_dimensions->length_3d;
        break;
    }
    for (float block_size = 2.0; block_size <= size; block_size *= 2.0) {
        bind_quad(iter[1], fft_iter_program);
        set_ivec2_uniform("texelDimensions2D",
                          tex_dimensions->width_2d, tex_dimensions->height_2d);
        set_ivec3_uniform("texelDimensions3D",
                          tex_dimensions->width_3d, tex_dimensions->height_3d, 
                          tex_dimensions->length_3d);
        set_sampler2D_uniform("tex", iter[0]);
        set_int_uniform("orientation", orientation);
        set_float_uniform("blockSize", block_size/(float)size);
        set_float_uniform("size", (float)size);
        if (is_inverse)
            set_float_uniform("angleSign", 1.0);
        else
            set_float_uniform("angleSign", -1.0);
        if (is_inverse && block_size == size)
            set_float_uniform("scale", 1.0/(float)size);
        else
            set_float_uniform("scale", 1.0);
        draw_unbind_quad();
        swap2(&iter[0], &iter[1]);
    }
    return iter[0];
}


void rev_bit_sort2(GLuint rev_bit_sort2_program, frame_id dst, frame_id src,
                   const struct TextureDimensions *tex_dimensions) {
    bind_quad(dst, rev_bit_sort2_program);
    set_ivec2_uniform("texelDimensions2D",
                      tex_dimensions->width_2d, tex_dimensions->height_2d);
    set_ivec3_uniform("texelDimensions3D",
                      tex_dimensions->width_3d, tex_dimensions->height_3d, 
                      tex_dimensions->length_3d);       
    set_sampler2D_uniform("tex", src);
    draw_unbind_quad();
}

frame_id fft3d(GLuint rev_bit_sort2_program,
               GLuint fft_iter_program,
               frame_id init_frame, frame_id fft_iter1, frame_id fft_iter2,
               const struct TextureDimensions *tex_dimensions) {
    frame_id fft_iters[2] = {fft_iter1, fft_iter2};
    rev_bit_sort2(rev_bit_sort2_program,
                  fft_iters[0], init_frame, tex_dimensions);
    int res_x = fft_iter(fft_iter_program, fft_iters[0], fft_iters[1],
                         ORIENTATION_0, NOT_IS_INVERSE, tex_dimensions);
    if (res_x != fft_iters[0])
        swap2(&fft_iters[0], &fft_iters[1]);
    int res_y = fft_iter(fft_iter_program, fft_iters[0], fft_iters[1],
                         ORIENTATION_1, NOT_IS_INVERSE, tex_dimensions);
    if (res_y != fft_iters[0])
        swap2(&fft_iters[0], &fft_iters[1]);
    int res_z = fft_iter(fft_iter_program, fft_iters[0], fft_iters[1],
                         ORIENTATION_2, NOT_IS_INVERSE, tex_dimensions);
    return res_z;
}

frame_id ifft3d(GLuint rev_bit_sort2_program,
                GLuint fft_iter_program,
                frame_id init_frame, frame_id fft_iter1, frame_id fft_iter2,
                const struct TextureDimensions *tex_dimensions) {
    if (init_frame == fft_iter1 || init_frame == fft_iter2) {
        fprintf(stderr, "init_frame cannot equal fft_iter1 or fft_iter2.");
        return -1;
    }
    frame_id fft_iters[2] = {fft_iter1, fft_iter2};
    rev_bit_sort2(rev_bit_sort2_program,
                  fft_iters[0], init_frame, tex_dimensions);
    int res_x = fft_iter(fft_iter_program, fft_iters[0], fft_iters[1],
                         ORIENTATION_0, IS_INVERSE, tex_dimensions);
    if (res_x != fft_iters[0])
        swap2(&fft_iters[0], &fft_iters[1]);
    int res_y = fft_iter(fft_iter_program, fft_iters[0], fft_iters[1],
                         ORIENTATION_1, IS_INVERSE, tex_dimensions);
    if (res_y != fft_iters[0])
        swap2(&fft_iters[0], &fft_iters[1]);
    int res_z = fft_iter(fft_iter_program, fft_iters[0], fft_iters[1],
                         ORIENTATION_2, IS_INVERSE, tex_dimensions);
    return res_z;
}

frame_id fftshift3d(GLuint fftshift_program, GLuint init_frame,
                    frame_id fftshift1, frame_id fftshift2,
                    const struct TextureDimensions *tex_dimensions) {
    if (init_frame == fftshift1 || init_frame == fftshift2) {
        fprintf(stderr, "init_frame cannot equal fftshift1 or fftshift2");
        return -1;
    }
    frame_id fft_shift_frames[3] = {fftshift1, fftshift2, fftshift1};
    for (int i = 0; i < 3; i++) {
        bind_quad(fft_shift_frames[i], fftshift_program);
        if (i == 0)
            set_sampler2D_uniform("tex", init_frame);
        else
            set_sampler2D_uniform("tex", fft_shift_frames[i-1]);
        set_ivec2_uniform("texelDimensions2D",
                        tex_dimensions->width_2d, tex_dimensions->height_2d);
        set_ivec3_uniform("texelDimensions3D",
                        tex_dimensions->width_3d, tex_dimensions->height_3d, 
                        tex_dimensions->length_3d);
        set_int_uniform("orientation", i);
        draw_unbind_quad();
    }
    return fftshift1;
}
