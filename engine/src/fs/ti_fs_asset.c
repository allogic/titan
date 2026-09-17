#include <ti_pch.h>

void fs_asset_load(fs_asset_t *asset, fs_file *file) {
  fs_file_read(file, &asset->magic, sizeof(uint64_t), 0);

  if (asset->magic != TI_FS_ASSET_MAGIC) {
    return;
  }

  // TODO: additionally do checksums..

  fs_file_read(file, &asset->type, sizeof(fs_asset_type_t), 0);

  switch (asset->type) {

    case FS_ASSET_TYPE_MODEL: {

      asset->fs_instance = TI_ALLOC(sizeof(fs_model_t), 1, 0);
      asset->vk_instance = TI_ALLOC(sizeof(vk_model_t), 1, 0);

      ((vk_model_t *)asset->vk_instance)->asset = asset;

      fs_model_load(asset->fs_instance, file);
      // vk_model_create(asset->vk_instance);

      break;
    }
    case FS_ASSET_TYPE_PIPELINE: {

      asset->fs_instance = TI_ALLOC(sizeof(fs_pipeline_t), 1, 0);
      asset->vk_instance = TI_ALLOC(sizeof(vk_pipeline_t), 1, 0);

      ((vk_pipeline_t *)asset->vk_instance)->asset = asset;

      fs_pipeline_load(asset->fs_instance, file);
      // vk_pipeline_create(asset->vk_instance);

      break;
    }
    case FS_ASSET_TYPE_FONT: {

      asset->fs_instance = TI_ALLOC(sizeof(fs_font_t), 1, 0);
      asset->vk_instance = TI_ALLOC(sizeof(vk_font_t), 1, 0);

      ((vk_font_t *)asset->vk_instance)->asset = asset;

      fs_font_load(asset->fs_instance, file);
      // vk_font_create(asset->vk_instance);

      break;
    }
    case FS_ASSET_TYPE_DESCRIPTOR_BINDING: {

      asset->fs_instance = TI_ALLOC(sizeof(fs_descriptor_binding_t), 1, 0);
      asset->vk_instance = TI_ALLOC(sizeof(vk_descriptor_binding_t), 1, 0);

      ((vk_descriptor_binding_t *)asset->vk_instance)->asset = asset;

      fs_descriptor_binding_load(asset->fs_instance, file);
      // vk_descriptor_binding_create(asset->vk_instance);

      break;
    }
  }
}
void fs_asset_store(fs_asset_t *asset, fs_file *file) {
}
void fs_asset_destroy(fs_asset_t *asset) {
  switch (asset->type) {

    case FS_ASSET_TYPE_MODEL: {

      fs_model_destroy(asset->fs_instance);
      vk_model_destroy(asset->vk_instance);

      break;
    }
    case FS_ASSET_TYPE_PIPELINE: {

      fs_pipeline_destroy(asset->fs_instance);
      vk_pipeline_destroy(asset->vk_instance);

      break;
    }
    case FS_ASSET_TYPE_FONT: {

      fs_font_destroy(asset->fs_instance);
      vk_font_destroy(asset->vk_instance);

      break;
    }
    case FS_ASSET_TYPE_DESCRIPTOR_BINDING: {

      fs_descriptor_binding_destroy(asset->fs_instance);
      vk_descriptor_binding_destroy(asset->vk_instance);

      break;
    }
  }

  TI_FREE(asset->fs_instance);
  TI_FREE(asset->vk_instance);
}
