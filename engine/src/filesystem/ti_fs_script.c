#include <ti_pch.h>

void fs_script_load(fs_script_t *script, fs_file *file) {
  memset(script, 0, sizeof(fs_script_t));

  fs_file_read(file, &script->c_buffer_size, sizeof(uint64_t), 0);
  script->c_buffer = TI_ALLOC(script->c_buffer_size, 0, 0);
  fs_file_read(file, script->c_buffer, script->c_buffer_size, 0);

  fs_file_read(file, &script->obj_buffer_size, sizeof(uint64_t), 0);
  script->obj_buffer = TI_ALLOC(script->obj_buffer_size, 0, 0);
  fs_file_read(file, script->obj_buffer, script->obj_buffer_size, 0);
}
void fs_script_store(fs_script_t *script, fs_file *file) {
  fs_file_write(file, &script->c_buffer_size, sizeof(uint64_t), 0);
  fs_file_write(file, script->c_buffer, script->c_buffer_size, 0);

  fs_file_write(file, &script->obj_buffer_size, sizeof(uint64_t), 0);
  fs_file_write(file, script->obj_buffer, script->obj_buffer_size, 0);
}
void fs_script_destroy(fs_script_t *script) {
  TI_FREE(script->c_buffer);
  TI_FREE(script->obj_buffer);

  memset(script, 0, sizeof(fs_script_t));
}
