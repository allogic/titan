#include <ti_pch.h>

void vk_font_create(vk_font_t *font, char const *asset_path) {
  font->config = (fs_font_t *)fs_get(asset_path);

  // TODO
}
void vk_font_destroy(vk_font_t *font) {
  // TODO
}
