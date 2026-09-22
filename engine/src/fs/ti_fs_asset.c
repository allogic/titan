#include <ti_pch.h>

void fs_asset_create(fs_asset_t *asset) {
  switch (asset->type) {

    case FS_ASSET_TYPE_MODEL: {

      asset->config = TI_ALLOC(sizeof(fs_model_t), 1, 0);

      break;
    }
    case FS_ASSET_TYPE_PIPELINE: {

      asset->config = TI_ALLOC(sizeof(fs_pipeline_t), 1, 0);

      break;
    }
    case FS_ASSET_TYPE_FONT: {

      asset->config = TI_ALLOC(sizeof(fs_font_t), 1, 0);

      break;
    }
    case FS_ASSET_TYPE_DESCRIPTOR_BINDING: {

      asset->config = TI_ALLOC(sizeof(fs_descriptor_binding_t), 1, 0);

      break;
    }
    case FS_ASSET_TYPE_BUFFER: {

      asset->config = TI_ALLOC(sizeof(fs_buffer_t), 1, 0);

      break;
    }
    case FS_ASSET_TYPE_IMAGE: {

      asset->config = TI_ALLOC(sizeof(fs_image_t), 1, 0);

      break;
    }
    case FS_ASSET_TYPE_FRAMEBUFFER: {

      asset->config = TI_ALLOC(sizeof(fs_framebuffer_t), 1, 0);

      break;
    }
    case FS_ASSET_TYPE_SWAPCHAIN: {

      asset->config = TI_ALLOC(sizeof(fs_swapchain_t), 1, 0);

      break;
    }
    case FS_ASSET_TYPE_RENDERPASS: {

      asset->config = TI_ALLOC(sizeof(fs_renderpass_t), 1, 0);

      break;
    }
    case FS_ASSET_TYPE_RENDERER: {

      asset->config = TI_ALLOC(sizeof(fs_renderer_t), 1, 0);

      break;
    }
  }
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
  fs_file_read(file, asset->path, TI_PATH_SIZE, 0);

  switch (asset->type) {

    case FS_ASSET_TYPE_MODEL: {

      asset->config = TI_ALLOC(sizeof(fs_model_t), 1, 0);

      fs_model_load(asset->config, file);

      break;
    }
    case FS_ASSET_TYPE_PIPELINE: {

      asset->config = TI_ALLOC(sizeof(fs_pipeline_t), 1, 0);

      fs_pipeline_load(asset->config, file);

      break;
    }
    case FS_ASSET_TYPE_FONT: {

      asset->config = TI_ALLOC(sizeof(fs_font_t), 1, 0);

      fs_font_load(asset->config, file);

      break;
    }
    case FS_ASSET_TYPE_DESCRIPTOR_BINDING: {

      asset->config = TI_ALLOC(sizeof(fs_descriptor_binding_t), 1, 0);

      fs_descriptor_binding_load(asset->config, file);

      break;
    }
    case FS_ASSET_TYPE_BUFFER: {

      asset->config = TI_ALLOC(sizeof(fs_buffer_t), 1, 0);

      fs_buffer_load(asset->config, file);

      break;
    }
    case FS_ASSET_TYPE_IMAGE: {

      asset->config = TI_ALLOC(sizeof(fs_image_t), 1, 0);

      fs_image_load(asset->config, file);

      break;
    }
    case FS_ASSET_TYPE_FRAMEBUFFER: {

      asset->config = TI_ALLOC(sizeof(fs_framebuffer_t), 1, 0);

      fs_framebuffer_load(asset->config, file);

      break;
    }
    case FS_ASSET_TYPE_SWAPCHAIN: {

      asset->config = TI_ALLOC(sizeof(fs_swapchain_t), 1, 0);

      fs_swapchain_load(asset->config, file);

      break;
    }
    case FS_ASSET_TYPE_RENDERPASS: {

      asset->config = TI_ALLOC(sizeof(fs_renderpass_t), 1, 0);

      fs_renderpass_load(asset->config, file);

      break;
    }
    case FS_ASSET_TYPE_RENDERER: {

      asset->config = TI_ALLOC(sizeof(fs_renderer_t), 1, 0);

      fs_renderer_load(asset->config, file);

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
  fs_file_write(file, asset->path, TI_PATH_SIZE, 0);

  switch (asset->type) {

    case FS_ASSET_TYPE_MODEL: {

      fs_model_store(asset->config, file);

      break;
    }
    case FS_ASSET_TYPE_PIPELINE: {

      fs_pipeline_store(asset->config, file);

      break;
    }
    case FS_ASSET_TYPE_FONT: {

      fs_font_store(asset->config, file);

      break;
    }
    case FS_ASSET_TYPE_DESCRIPTOR_BINDING: {

      fs_descriptor_binding_store(asset->config, file);

      break;
    }
    case FS_ASSET_TYPE_BUFFER: {

      fs_buffer_store(asset->config, file);

      break;
    }
    case FS_ASSET_TYPE_IMAGE: {

      fs_image_store(asset->config, file);

      break;
    }
    case FS_ASSET_TYPE_FRAMEBUFFER: {

      fs_framebuffer_store(asset->config, file);

      break;
    }
    case FS_ASSET_TYPE_SWAPCHAIN: {

      fs_swapchain_store(asset->config, file);

      break;
    }
    case FS_ASSET_TYPE_RENDERPASS: {

      fs_renderpass_store(asset->config, file);

      break;
    }
    case FS_ASSET_TYPE_RENDERER: {

      fs_renderer_store(asset->config, file);

      break;
    }
  }

  fs_file_close(file);
}
void fs_asset_destroy(fs_asset_t *asset) {
  switch (asset->type) {

    case FS_ASSET_TYPE_MODEL: {

      fs_model_destroy(asset->config);

      break;
    }
    case FS_ASSET_TYPE_PIPELINE: {

      fs_pipeline_destroy(asset->config);

      break;
    }
    case FS_ASSET_TYPE_FONT: {

      fs_font_destroy(asset->config);

      break;
    }
    case FS_ASSET_TYPE_DESCRIPTOR_BINDING: {

      fs_descriptor_binding_destroy(asset->config);

      break;
    }
    case FS_ASSET_TYPE_BUFFER: {

      fs_buffer_destroy(asset->config);

      break;
    }
    case FS_ASSET_TYPE_IMAGE: {

      fs_image_destroy(asset->config);

      break;
    }
    case FS_ASSET_TYPE_FRAMEBUFFER: {

      fs_framebuffer_destroy(asset->config);

      break;
    }
    case FS_ASSET_TYPE_SWAPCHAIN: {

      fs_swapchain_destroy(asset->config);

      break;
    }
    case FS_ASSET_TYPE_RENDERPASS: {

      fs_renderpass_destroy(asset->config);

      break;
    }
    case FS_ASSET_TYPE_RENDERER: {

      fs_renderer_destroy(asset->config);

      break;
    }
  }

  TI_FREE(asset->config);
}
