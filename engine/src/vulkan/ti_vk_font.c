#include <ti_pch.h>

void vk_font_create(vk_font_t *font, char const *asset_path) {
  font->asset.path = asset_path;

  fs_asset_load(&font->asset);

  fs_font_t *config = (fs_font_t *)font->asset.instance;

  // TODO
}
void vk_font_destroy(vk_font_t *font) {
  // TODO

  fs_asset_destroy(&font->asset);
}
