#include <ti_pch.h>

void fs_renderer_load(fs_renderer_t *renderer, fs_file *file) {
  memset(renderer, 0, sizeof(fs_renderer_t));

  fs_file_read(file, renderer->debug_line_vertex_buffer.reference_path, TI_PATH_SIZE, 0);
  fs_file_read(file, renderer->debug_line_index_buffer.reference_path, TI_PATH_SIZE, 0);
  fs_file_read(file, renderer->full_screen_vertex_buffer.reference_path, TI_PATH_SIZE, 0);
  fs_file_read(file, renderer->full_screen_index_buffer.reference_path, TI_PATH_SIZE, 0);
}
void fs_renderer_store(fs_renderer_t *renderer, fs_file *file) {
  fs_file_write(file, renderer->debug_line_vertex_buffer.reference_path, TI_PATH_SIZE, 0);
  fs_file_write(file, renderer->debug_line_index_buffer.reference_path, TI_PATH_SIZE, 0);
  fs_file_write(file, renderer->full_screen_vertex_buffer.reference_path, TI_PATH_SIZE, 0);
  fs_file_write(file, renderer->full_screen_index_buffer.reference_path, TI_PATH_SIZE, 0);
}
void fs_renderer_destroy(fs_renderer_t *renderer) {
  memset(renderer, 0, sizeof(fs_renderer_t));
}
