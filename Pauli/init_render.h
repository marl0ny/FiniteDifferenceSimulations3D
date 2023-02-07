#ifndef _INIT_RENDER_
#define _INIT_RENDER_


struct RenderParams {
    int user_use;
    double user_dx, user_dy;
    double user_x, user_y;
    double user_scroll;
    int which;
};


void init();

void render(const struct RenderParams *render_params); 


#endif