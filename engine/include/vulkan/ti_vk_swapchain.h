#ifndef TI_VK_SWAPCHAIN_H
#define TI_VK_SWAPCHAIN_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

extern vk_swapchain_t g_swapchain;

void swapchain_create(void);
void swapchain_destroy(void);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // TI_VK_SWAPCHAIN_H
