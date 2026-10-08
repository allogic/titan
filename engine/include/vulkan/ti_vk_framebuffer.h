#ifndef TI_VK_FRAMEBUFFER_H
#define TI_VK_FRAMEBUFFER_H

#include <ti_engine.h>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

void vk_framebuffer_create(vk_framebuffer_t *framebuffer);
void vk_framebuffer_destroy(vk_framebuffer_t *framebuffer);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // TI_VK_FRAMEBUFFER_H
