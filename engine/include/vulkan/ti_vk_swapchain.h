#ifndef TI_VK_SWAPCHAIN_H
#define TI_VK_SWAPCHAIN_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

void vk_swapchain_create(vk_swapchain_t *swapchain, char const *asset_path);
void vk_swapchain_destroy(vk_swapchain_t *swapchain);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // TI_VK_SWAPCHAIN_H
