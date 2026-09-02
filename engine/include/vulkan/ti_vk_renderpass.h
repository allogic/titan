#ifndef TI_VK_RENDERPASS_H
#define TI_VK_RENDERPASS_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

extern VkRenderPass g_renderpass;

void renderpass_create(void);
void renderpass_destroy(void);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // TI_VK_RENDERPASS_H
