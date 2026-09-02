#include <ti_pch.h>

void adb_font_load(adb_font_t *font, fs_file *file) {
  memset(font, 0, sizeof(adb_font_t));

  fs_file_read(file, font->name, TI_PATH_SIZE, 0);
  fs_file_read(file, &font->buffer_size, sizeof(uint64_t), 0);

  font->buffer = TI_ALLOC(font->buffer_size, 0, 0);

  fs_file_read(file, font->buffer, font->buffer_size, 0);
}
void adb_font_store(adb_font_t *font, fs_file *file) {
  fs_file_write(file, font->name, TI_PATH_SIZE, 0);
  fs_file_write(file, &font->buffer_size, sizeof(uint64_t), 0);
  fs_file_write(file, font->buffer, font->buffer_size, 0);
}
void adb_font_destroy(adb_font_t *font) {
  if (font->buffer) {
    TI_FREE(font->buffer);
  }

  memset(font, 0, sizeof(adb_font_t));
}
