#include <ti_pch.h>

void vk_model_create(vk_model_t *model, char const *asset_path) {
  model->config = (fs_model_t *)fs_get(asset_path);

  // TODO
}
void vk_model_destroy(vk_model_t *model) {
  // TODO
}
