#include <ti_pch.h>

void vk_model_create(vk_model_t *model, char const *asset_path) {
  model->asset.path = asset_path;

  fs_asset_load(&model->asset);

  fs_model_t *config = (fs_model_t *)model->asset.instance;

  // TODO
}
void vk_model_destroy(vk_model_t *model) {
  // TODO

  fs_asset_destroy(&model->asset);
}
