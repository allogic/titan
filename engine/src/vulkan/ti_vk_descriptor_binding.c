#include <ti_pch.h>

void vk_descriptor_binding_create(vk_descriptor_binding_t *descriptor_binding, char const *asset_path) {
  descriptor_binding->asset.path = asset_path;

  fs_asset_load(&descriptor_binding->asset);

  fs_descriptor_binding_t *config = (fs_descriptor_binding_t *)descriptor_binding->asset.instance;

  // TODO
}
void vk_descriptor_binding_destroy(vk_descriptor_binding_t *descriptor_binding) {
  // TODO

  fs_asset_destroy(&descriptor_binding->asset);
}
