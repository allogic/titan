#ifndef TI_VK_RENDERPASS_H
#define TI_VK_RENDERPASS_H

#include <ti_engine.h>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

void vk_renderpass_create(vk_renderpass_t *renderpass, fs_renderpass_t *config);
void vk_renderpass_destroy(vk_renderpass_t *renderpass);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // TI_VK_RENDERPASS_H
