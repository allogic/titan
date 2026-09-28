#include <ti_pch.h>

void fs_script_load(fs_script_t *script, fs_file *file) {
  memset(script, 0, sizeof(fs_script_t));

  fs_file_read(file, &script->buffer_size, sizeof(uint64_t), 0);
  script->buffer = TI_ALLOC(script->buffer_size, 0, 0);
  fs_file_read(file, script->buffer, script->buffer_size, 0);
}
void fs_script_store(fs_script_t *script, fs_file *file) {
  fs_file_write(file, &script->buffer_size, sizeof(uint64_t), 0);
  fs_file_write(file, script->buffer, script->buffer_size, 0);
}
void fs_script_destroy(fs_script_t *script) {
  TI_FREE(script->buffer);

  memset(script, 0, sizeof(fs_script_t));
}
