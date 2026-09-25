#include <ti_pch.h>

void fs_asset_create(fs_asset_t *asset) {
  switch (asset->type) {

    case FS_ASSET_TYPE_MODEL: {

      asset->instance = TI_ALLOC(sizeof(fs_model_t), 1, 0);

      break;
    }
    case FS_ASSET_TYPE_PIPELINE: {

      asset->instance = TI_ALLOC(sizeof(fs_pipeline_t), 1, 0);

      break;
    }
    case FS_ASSET_TYPE_FONT: {

      asset->instance = TI_ALLOC(sizeof(fs_font_t), 1, 0);

      break;
    }
    case FS_ASSET_TYPE_INPUT_VARIABLE: {

      asset->instance = TI_ALLOC(sizeof(fs_input_variable_t), 1, 0);

      break;
    }
    case FS_ASSET_TYPE_DESCRIPTOR_BINDING: {

      asset->instance = TI_ALLOC(sizeof(fs_descriptor_binding_t), 1, 0);

      break;
    }
    case FS_ASSET_TYPE_BUFFER: {

      asset->instance = TI_ALLOC(sizeof(fs_buffer_t), 1, 0);

      break;
    }
    case FS_ASSET_TYPE_IMAGE: {

      asset->instance = TI_ALLOC(sizeof(fs_image_t), 1, 0);

      break;
    }
    case FS_ASSET_TYPE_FRAMEBUFFER: {

      asset->instance = TI_ALLOC(sizeof(fs_framebuffer_t), 1, 0);

      break;
    }
    case FS_ASSET_TYPE_SWAPCHAIN: {

      asset->instance = TI_ALLOC(sizeof(fs_swapchain_t), 1, 0);

      break;
    }
    case FS_ASSET_TYPE_RENDERPASS: {

      asset->instance = TI_ALLOC(sizeof(fs_renderpass_t), 1, 0);

      break;
    }
    case FS_ASSET_TYPE_RENDERER: {

      asset->instance = TI_ALLOC(sizeof(fs_renderer_t), 1, 0);

      break;
    }
  }
}
uint8_t fs_asset_exists(fs_asset_t *asset) {
  fs_file_info info = {0};
  fs_result result = fs_info(g_fs, asset->path, 0, &info);

  return result != FS_DOES_NOT_EXIST;
}
void fs_asset_load(fs_asset_t *asset) {
  fs_file *file = 0;

  if (fs_file_open(g_fs, asset->path, FS_READ, &file) != FS_SUCCESS) {
    return;
  }

  fs_file_read(file, &asset->magic, sizeof(uint64_t), 0);

  if (asset->magic != TI_FS_ASSET_MAGIC) {
    return;
  }

  // TODO: additionally do checksums..

  fs_file_read(file, &asset->type, sizeof(fs_asset_type_t), 0);

  switch (asset->type) {

    case FS_ASSET_TYPE_MODEL: {

      asset->instance = TI_ALLOC(sizeof(fs_model_t), 1, 0);

      fs_model_load(asset->instance, file);

      break;
    }
    case FS_ASSET_TYPE_PIPELINE: {

      asset->instance = TI_ALLOC(sizeof(fs_pipeline_t), 1, 0);

      fs_pipeline_load(asset->instance, file);

      break;
    }
    case FS_ASSET_TYPE_FONT: {

      asset->instance = TI_ALLOC(sizeof(fs_font_t), 1, 0);

      fs_font_load(asset->instance, file);

      break;
    }
    case FS_ASSET_TYPE_INPUT_VARIABLE: {

      asset->instance = TI_ALLOC(sizeof(fs_input_variable_t), 1, 0);

      fs_input_variable_load(asset->instance, file);

      break;
    }
    case FS_ASSET_TYPE_DESCRIPTOR_BINDING: {

      asset->instance = TI_ALLOC(sizeof(fs_descriptor_binding_t), 1, 0);

      fs_descriptor_binding_load(asset->instance, file);

      break;
    }
    case FS_ASSET_TYPE_BUFFER: {

      asset->instance = TI_ALLOC(sizeof(fs_buffer_t), 1, 0);

      fs_buffer_load(asset->instance, file);

      break;
    }
    case FS_ASSET_TYPE_IMAGE: {

      asset->instance = TI_ALLOC(sizeof(fs_image_t), 1, 0);

      fs_image_load(asset->instance, file);

      break;
    }
    case FS_ASSET_TYPE_FRAMEBUFFER: {

      asset->instance = TI_ALLOC(sizeof(fs_framebuffer_t), 1, 0);

      fs_framebuffer_load(asset->instance, file);

      break;
    }
    case FS_ASSET_TYPE_SWAPCHAIN: {

      asset->instance = TI_ALLOC(sizeof(fs_swapchain_t), 1, 0);

      fs_swapchain_load(asset->instance, file);

      break;
    }
    case FS_ASSET_TYPE_RENDERPASS: {

      asset->instance = TI_ALLOC(sizeof(fs_renderpass_t), 1, 0);

      fs_renderpass_load(asset->instance, file);

      break;
    }
    case FS_ASSET_TYPE_RENDERER: {

      asset->instance = TI_ALLOC(sizeof(fs_renderer_t), 1, 0);

      fs_renderer_load(asset->instance, file);

      break;
    }
  }

  fs_file_close(file);
}
void fs_asset_store(fs_asset_t *asset) {
  fs_file *file = 0;

  if (fs_file_open(g_fs, asset->path, FS_WRITE, &file) != FS_SUCCESS) {
    return;
  }

  if (asset->magic != TI_FS_ASSET_MAGIC) {
    return;
  }

  fs_file_write(file, &asset->magic, sizeof(uint64_t), 0);
  fs_file_write(file, &asset->type, sizeof(fs_asset_type_t), 0);

  switch (asset->type) {

    case FS_ASSET_TYPE_MODEL: {

      fs_model_store(asset->instance, file);

      break;
    }
    case FS_ASSET_TYPE_PIPELINE: {

      fs_pipeline_store(asset->instance, file);

      break;
    }
    case FS_ASSET_TYPE_FONT: {

      fs_font_store(asset->instance, file);

      break;
    }
    case FS_ASSET_TYPE_INPUT_VARIABLE: {

      fs_input_variable_store(asset->instance, file);

      break;
    }
    case FS_ASSET_TYPE_DESCRIPTOR_BINDING: {

      fs_descriptor_binding_store(asset->instance, file);

      break;
    }
    case FS_ASSET_TYPE_BUFFER: {

      fs_buffer_store(asset->instance, file);

      break;
    }
    case FS_ASSET_TYPE_IMAGE: {

      fs_image_store(asset->instance, file);

      break;
    }
    case FS_ASSET_TYPE_FRAMEBUFFER: {

      fs_framebuffer_store(asset->instance, file);

      break;
    }
    case FS_ASSET_TYPE_SWAPCHAIN: {

      fs_swapchain_store(asset->instance, file);

      break;
    }
    case FS_ASSET_TYPE_RENDERPASS: {

      fs_renderpass_store(asset->instance, file);

      break;
    }
    case FS_ASSET_TYPE_RENDERER: {

      fs_renderer_store(asset->instance, file);

      break;
    }
  }

  fs_file_close(file);
}
void fs_asset_destroy(fs_asset_t *asset) {
  switch (asset->type) {

    case FS_ASSET_TYPE_MODEL: {

      fs_model_destroy(asset->instance);

      break;
    }
    case FS_ASSET_TYPE_PIPELINE: {

      fs_pipeline_destroy(asset->instance);

      break;
    }
    case FS_ASSET_TYPE_FONT: {

      fs_font_destroy(asset->instance);

      break;
    }
    case FS_ASSET_TYPE_INPUT_VARIABLE: {

      fs_input_variable_destroy(asset->instance);

      break;
    }
    case FS_ASSET_TYPE_DESCRIPTOR_BINDING: {

      fs_descriptor_binding_destroy(asset->instance);

      break;
    }
    case FS_ASSET_TYPE_BUFFER: {

      fs_buffer_destroy(asset->instance);

      break;
    }
    case FS_ASSET_TYPE_IMAGE: {

      fs_image_destroy(asset->instance);

      break;
    }
    case FS_ASSET_TYPE_FRAMEBUFFER: {

      fs_framebuffer_destroy(asset->instance);

      break;
    }
    case FS_ASSET_TYPE_SWAPCHAIN: {

      fs_swapchain_destroy(asset->instance);

      break;
    }
    case FS_ASSET_TYPE_RENDERPASS: {

      fs_renderpass_destroy(asset->instance);

      break;
    }
    case FS_ASSET_TYPE_RENDERER: {

      fs_renderer_destroy(asset->instance);

      break;
    }
  }

  TI_FREE(asset->instance);
}
