#ifdef __cplusplus
extern "C" {
#endif

#ifndef _FFT_GL_
#define _FFT_GL_

#include "gl_wrappers/gl_wrappers.h"
#include "texture_dimensions.h"


void rev_bit_sort2(GLuint rev_bit_sort2_program, frame_id dst, frame_id src,
                   const struct TextureDimensions *tex_dimensions);

/* Returns the result in either fft_iter1, or fft_iter2. */
frame_id fft3d(GLuint rev_bit_sort2_program,
               GLuint fft_iter_program,
               frame_id init_frame, frame_id fft_iter1, frame_id fft_iter2,
               const struct TextureDimensions *tex_dimensions);

/* Returns the result in either fft_iter1, or fft_iter2. */
frame_id ifft3d(GLuint rev_bit_sort2_program,
                GLuint fft_iter_program,
                frame_id init_frame, frame_id fft_iter1, frame_id fft_iter2,
                const struct TextureDimensions *tex_dimensions);

/* Returns the result in fftshift1 */
frame_id fftshift3d(GLuint fftshift_program, GLuint init_frame,
                    frame_id fftshift1, frame_id fftshift2,
                    const struct TextureDimensions *tex_dimensions);


#endif

#ifdef __cplusplus
}
#endif
