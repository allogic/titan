#ifndef TI_FS_SWAPCHAIN_H
#define TI_FS_SWAPCHAIN_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

void fs_swapchain_load(fs_swapchain_t *swapchain, fs_file *file);
void fs_swapchain_store(fs_swapchain_t *swapchain, fs_file *file);
void fs_swapchain_destroy(fs_swapchain_t *swapchain);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // TI_FS_SWAPCHAIN_H
