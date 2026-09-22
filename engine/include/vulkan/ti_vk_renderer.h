#ifndef TI_VK_RENDERER_H
#define TI_VK_RENDERER_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

void vk_renderer_create(vk_renderer_t *renderer, char const *asset_path);
void vk_renderer_draw(vk_renderer_t *renderer);
void vk_renderer_destroy(vk_renderer_t *renderer);

void vk_renderer_draw_debug_line(vk_renderer_t *renderer, fvec3_t from, fvec3_t to, fvec4_t color);
void vk_renderer_draw_debug_box(vk_renderer_t *renderer, fvec3_t position, fvec3_t size, fvec4_t color);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // TI_VK_RENDERER_H
