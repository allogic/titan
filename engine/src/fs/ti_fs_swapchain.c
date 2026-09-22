#include <ti_pch.h>

void fs_swapchain_load(fs_swapchain_t *swapchain, fs_file *file) {
  memset(swapchain, 0, sizeof(fs_swapchain_t));

  fs_file_read(file, swapchain->name, TI_PATH_SIZE, 0);
}
void fs_swapchain_store(fs_swapchain_t *swapchain, fs_file *file) {
  fs_file_write(file, swapchain->name, TI_PATH_SIZE, 0);
}
void fs_swapchain_destroy(fs_swapchain_t *swapchain) {
  memset(swapchain, 0, sizeof(fs_swapchain_t));
}
