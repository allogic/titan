#include <ti_pch.h>

void fs_sound_load(fs_sound_t *sound, fs_file *file) {
  memset(sound, 0, sizeof(fs_sound_t));
}
void fs_sound_store(fs_sound_t *sound, fs_file *file) {
  // TODO
}
void fs_sound_destroy(fs_sound_t *sound) {
  memset(sound, 0, sizeof(fs_sound_t));
}
