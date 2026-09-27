#include <ti_pch.h>

void fs_font_load(fs_font_t *font, fs_file *file) {
  memset(font, 0, sizeof(fs_font_t));

  fs_file_read(file, &font->buffer_size, sizeof(uint64_t), 0);

  font->buffer = TI_ALLOC(font->buffer_size, 0, 0);

  fs_file_read(file, font->buffer, font->buffer_size, 0);
}
void fs_font_store(fs_font_t *font, fs_file *file) {
  fs_file_write(file, &font->buffer_size, sizeof(uint64_t), 0);
  fs_file_write(file, font->buffer, font->buffer_size, 0);
}
void fs_font_destroy(fs_font_t *font) {
  if (font->buffer) {
    TI_FREE(font->buffer);
  }

  memset(font, 0, sizeof(fs_font_t));
}
