#ifndef TI_VULKAN_SWAPCHAIN_H
#define TI_VULKAN_SWAPCHAIN_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

extern swapchain_t g_swapchain;

void swapchain_create(void);
void swapchain_destroy(void);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // TI_VULKAN_SWAPCHAIN_H
