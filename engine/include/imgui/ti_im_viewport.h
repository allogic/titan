#ifndef TI_IM_VIEWPORT_H
#define TI_IM_VIEWPORT_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

extern uint32_t g_viewport_width;
extern uint32_t g_viewport_height;

void im_viewport_update(void);
void im_viewport_draw(void);
void im_viewport_refresh(void);
void im_viewport_reset(void);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // TI_IM_VIEWPORT_H
