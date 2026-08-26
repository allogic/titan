#ifndef TI_RENDERER_H
#define TI_RENDERER_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

extern renderer_t g_renderer;

void renderer_create(void);
void renderer_draw(void);
void renderer_destroy(void);

void renderer_draw_debug_line(fvec3_t from, fvec3_t to, fvec4_t color);
void renderer_draw_debug_box(fvec3_t position, fvec3_t size, fvec4_t color);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // TI_RENDERER_H
