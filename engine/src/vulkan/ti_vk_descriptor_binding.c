#include <ti_pch.h>

void vk_descriptor_binding_create(vk_descriptor_binding_t *descriptor_binding, char const *asset_path) {
  descriptor_binding->config = (fs_descriptor_binding_t *)fs_get(asset_path);

  // TODO
}
void vk_descriptor_binding_destroy(vk_descriptor_binding_t *descriptor_binding) {
  // TODO
}
