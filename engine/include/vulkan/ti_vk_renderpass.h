#ifndef TI_VK_RENDERPASS_H
#define TI_VK_RENDERPASS_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

void vk_renderpass_create(vk_renderpass_t *renderpass, char const *asset_path);
void vk_renderpass_destroy(vk_renderpass_t *renderpass);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // TI_VK_RENDERPASS_H
