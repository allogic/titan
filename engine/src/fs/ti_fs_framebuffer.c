#include <ti_pch.h>

void fs_framebuffer_load(fs_framebuffer_t *framebuffer, fs_file *file) {
  memset(framebuffer, 0, sizeof(fs_framebuffer_t));

  fs_file_read(file, framebuffer->name, TI_PATH_SIZE, 0);
  fs_file_read(file, framebuffer->color_attachment_image, TI_PATH_SIZE, 0);
  fs_file_read(file, framebuffer->depth_attachment_image, TI_PATH_SIZE, 0);
}
void fs_framebuffer_store(fs_framebuffer_t *framebuffer, fs_file *file) {
  fs_file_write(file, framebuffer->name, TI_PATH_SIZE, 0);
  fs_file_write(file, framebuffer->color_attachment_image, TI_PATH_SIZE, 0);
  fs_file_write(file, framebuffer->depth_attachment_image, TI_PATH_SIZE, 0);
}
void fs_framebuffer_destroy(fs_framebuffer_t *framebuffer) {
  memset(framebuffer, 0, sizeof(fs_framebuffer_t));
}
