#ifndef TI_VK_FRAMEBUFFER_H
#define TI_VK_FRAMEBUFFER_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

void vk_framebuffer_create(vk_framebuffer_t *framebuffer, vk_renderpass_t *renderpass, uint32_t width, uint32_t height, char const *asset_path);
void vk_framebuffer_destroy(vk_framebuffer_t *framebuffer);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // TI_VK_FRAMEBUFFER_H
