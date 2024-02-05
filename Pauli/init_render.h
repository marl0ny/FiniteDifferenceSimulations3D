#ifndef _INIT_RENDER_
#define _INIT_RENDER_

#ifdef __cplusplus
extern "C" {
#endif

struct RenderParams {
    int user_use;
    double user_dx, user_dy;
    double user_x, user_y;
    double user_scroll;
    int which;
};


void init();

void render(const struct RenderParams *render_params); 

#ifdef __cplusplus
}
#endif

#endif