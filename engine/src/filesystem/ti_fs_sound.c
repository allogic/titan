#include <filesystem/ti_fs_sound.h>

void fs_sound_load(fs_sound_t *sound, fs_file *file) {
  memset(sound, 0, sizeof(fs_sound_t));

  fs_file_read(file, &sound->channel_count, sizeof(uint32_t), 0);
  fs_file_read(file, &sound->sample_rate, sizeof(uint32_t), 0);
  fs_file_read(file, &sound->buffer_size, sizeof(uint64_t), 0);
  sound->buffer = TI_ALLOC(sound->buffer_size, 0, 0);
  fs_file_read(file, sound->buffer, sound->buffer_size, 0);
}
void fs_sound_store(fs_sound_t *sound, fs_file *file) {
  fs_file_write(file, &sound->channel_count, sizeof(uint32_t), 0);
  fs_file_write(file, &sound->sample_rate, sizeof(uint32_t), 0);
  fs_file_write(file, &sound->buffer_size, sizeof(uint64_t), 0);
  fs_file_write(file, sound->buffer, sound->buffer_size, 0);
}
void fs_sound_destroy(fs_sound_t *sound) {
  TI_FREE(sound->buffer);

  memset(sound, 0, sizeof(fs_sound_t));
}
