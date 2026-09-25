#include <ti_pch.h>

void fs_framebuffer_load(fs_framebuffer_t *framebuffer, fs_file *file) {
  memset(framebuffer, 0, sizeof(fs_framebuffer_t));

  fs_file_read(file, &framebuffer->color_attachment_count, sizeof(uint64_t), 0);

  framebuffer->color_attachments = (fs_asset_reference_t *)TI_ALLOC(sizeof(fs_asset_reference_t) * framebuffer->color_attachment_count, 0, 0);

  uint64_t attachment_index = 0;
  uint64_t attachment_count = framebuffer->color_attachment_count;

  while (attachment_index < attachment_count) {

    fs_file_read(file, framebuffer->color_attachments[attachment_index].reference_path, TI_PATH_SIZE, 0);

    attachment_index++;
  }

  fs_file_read(file, framebuffer->depth_attachment.reference_path, TI_PATH_SIZE, 0);
}
void fs_framebuffer_store(fs_framebuffer_t *framebuffer, fs_file *file) {
  fs_file_write(file, &framebuffer->color_attachment_count, sizeof(uint64_t), 0);

  uint64_t attachment_index = 0;
  uint64_t attachment_count = framebuffer->color_attachment_count;

  while (attachment_index < attachment_count) {

    fs_file_write(file, framebuffer->color_attachments[attachment_index].reference_path, TI_PATH_SIZE, 0);

    attachment_index++;
  }

  fs_file_write(file, framebuffer->depth_attachment.reference_path, TI_PATH_SIZE, 0);
}
void fs_framebuffer_destroy(fs_framebuffer_t *framebuffer) {
  if (framebuffer->color_attachments) {

    TI_FREE(framebuffer->color_attachments);
  }

  memset(framebuffer, 0, sizeof(fs_framebuffer_t));
}
