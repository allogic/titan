#include <filesystem/ti_fs_script.h>

void fs_script_load(fs_script_t *script, fs_file *file) {
  memset(script, 0, sizeof(fs_script_t));

  fs_file_read(file, &script->source_buffer_size, sizeof(uint64_t), 0);
  script->source_buffer = TI_ALLOC(script->source_buffer_size, 0, 0);
  fs_file_read(file, script->source_buffer, script->source_buffer_size, 0);

  fs_file_read(file, &script->object_buffer_size, sizeof(uint64_t), 0);
  script->object_buffer = TI_ALLOC(script->object_buffer_size, 0, 0);
  fs_file_read(file, script->object_buffer, script->object_buffer_size, 0);
}
void fs_script_store(fs_script_t *script, fs_file *file) {
  fs_file_write(file, &script->source_buffer_size, sizeof(uint64_t), 0);
  fs_file_write(file, script->source_buffer, script->source_buffer_size, 0);

  fs_file_write(file, &script->object_buffer_size, sizeof(uint64_t), 0);
  fs_file_write(file, script->object_buffer, script->object_buffer_size, 0);
}
void fs_script_destroy(fs_script_t *script) {
  TI_FREE(script->source_buffer);
  TI_FREE(script->object_buffer);

  memset(script, 0, sizeof(fs_script_t));
}
