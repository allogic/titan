#ifndef TI_IM_VIEWPORT_H
#define TI_IM_VIEWPORT_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

void im_viewport_update(vk_viewport_t *viewport);
void im_viewport_draw(vk_viewport_t *viewport);
void im_viewport_refresh(vk_viewport_t *viewport);
void im_viewport_reset(vk_viewport_t *viewport);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // TI_IM_VIEWPORT_H
