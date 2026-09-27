#include <ti_pch.h>

void fs_script_load(fs_script_t *script, fs_file *file) {
  memset(script, 0, sizeof(fs_script_t));
}
void fs_script_store(fs_script_t *script, fs_file *file) {
  // TODO
}
void fs_script_destroy(fs_script_t *script) {
  memset(script, 0, sizeof(fs_script_t));
}
