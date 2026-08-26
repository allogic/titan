#ifndef TI_VULKAN_FRAMEBUFFER_H
#define TI_VULKAN_FRAMEBUFFER_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

extern framebuffer_t g_framebuffer;

void framebuffer_create(void);
void framebuffer_destroy(void);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // TI_VULKAN_FRAMEBUFFER_H
