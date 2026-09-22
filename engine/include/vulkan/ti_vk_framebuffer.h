#ifndef TI_VK_FRAMEBUFFER_H
#define TI_VK_FRAMEBUFFER_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

// TODO: remove renderpass dependency..
void vk_framebuffer_create(vk_renderpass_t *renderpass, vk_framebuffer_t *framebuffer, char const *asset_path);
void vk_framebuffer_destroy(vk_framebuffer_t *framebuffer);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // TI_VK_FRAMEBUFFER_H
