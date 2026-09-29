#include <ti_pch.h>

void fs_renderpass_load(fs_renderpass_t *renderpass, fs_file *file) {
  memset(renderpass, 0, sizeof(fs_renderpass_t));

  fs_file_read(file, &renderpass->initial_color_attachment_layout_index, sizeof(uint64_t), 0);
  fs_file_read(file, &renderpass->initial_depth_attachment_layout_index, sizeof(uint64_t), 0);
  fs_file_read(file, &renderpass->final_color_attachment_layout_index, sizeof(uint64_t), 0);
  fs_file_read(file, &renderpass->final_depth_attachment_layout_index, sizeof(uint64_t), 0);
}
void fs_renderpass_store(fs_renderpass_t *renderpass, fs_file *file) {
  fs_file_write(file, &renderpass->initial_color_attachment_layout_index, sizeof(uint64_t), 0);
  fs_file_write(file, &renderpass->initial_depth_attachment_layout_index, sizeof(uint64_t), 0);
  fs_file_write(file, &renderpass->final_color_attachment_layout_index, sizeof(uint64_t), 0);
  fs_file_write(file, &renderpass->final_depth_attachment_layout_index, sizeof(uint64_t), 0);
}
void fs_renderpass_destroy(fs_renderpass_t *renderpass) {
  memset(renderpass, 0, sizeof(fs_renderpass_t));
}
