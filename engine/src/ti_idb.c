#include <ti_idb.h>

static map64_t s_instances = {0};

void idb_create(void) {
  map64_create(&s_instances);
}
handle_t *idb_reference(handle_t *source_handle, char const *asset_path) {
  uint64_t hash = fnv1a64(asset_path);

  if (map64_contains(&s_instances, hash) == 0) {

    handle_t *handle = (handle_t *)TI_ALLOC(sizeof(handle_t), 1, 0);

    strcpy(handle->asset_path, asset_path);

    handle->asset.path = handle->asset_path;

    fs_asset_load(&handle->asset); // TODO: check if asset is not available..

    switch (handle->asset.type) {

      case FS_ASSET_TYPE_MODEL: {

        handle->instance = TI_ALLOC(sizeof(vk_model_t), 1, 0);

        vk_model_create(handle->instance, handle->asset.instance);

        break;
      }
      case FS_ASSET_TYPE_PIPELINE: {

        handle->instance = TI_ALLOC(sizeof(vk_pipeline_t), 1, 0);

        vk_pipeline_create(handle->instance, handle->asset.instance);

        break;
      }
      case FS_ASSET_TYPE_FONT: {

        handle->instance = TI_ALLOC(sizeof(vk_font_t), 1, 0);

        vk_font_create(handle->instance, handle->asset.instance);

        break;
      }
      case FS_ASSET_TYPE_INPUT_VARIABLE: {

        // TODO

        break;
      }
      case FS_ASSET_TYPE_DESCRIPTOR_BINDING: {

        handle->instance = TI_ALLOC(sizeof(vk_descriptor_binding_t), 1, 0);

        vk_descriptor_binding_create(handle->instance, handle->asset.instance);

        break;
      }
      case FS_ASSET_TYPE_BUFFER: {

        handle->instance = TI_ALLOC(sizeof(vk_buffer_t), 1, 0);

        vk_buffer_create(handle->instance, handle->asset.instance);

        break;
      }
      case FS_ASSET_TYPE_IMAGE: {

        handle->instance = TI_ALLOC(sizeof(vk_image_t), 1, 0);

        vk_image_create(handle->instance, handle->asset.instance);

        break;
      }
      case FS_ASSET_TYPE_FRAMEBUFFER: {

        handle->instance = TI_ALLOC(sizeof(vk_framebuffer_t), 1, 0);

        vk_framebuffer_create(handle->instance, handle->asset.instance);

        break;
      }
      case FS_ASSET_TYPE_SWAPCHAIN: {

        handle->instance = TI_ALLOC(sizeof(vk_swapchain_t), 1, 0);

        vk_swapchain_create(handle->instance, handle->asset.instance);

        break;
      }
      case FS_ASSET_TYPE_RENDERPASS: {

        handle->instance = TI_ALLOC(sizeof(vk_renderpass_t), 1, 0);

        vk_renderpass_create(handle->instance, handle->asset.instance);

        break;
      }
      case FS_ASSET_TYPE_RENDERER: {

        handle->instance = TI_ALLOC(sizeof(vk_renderer_t), 1, 0);

        vk_renderer_create(handle->instance, handle->asset.instance);

        break;
      }
      case FS_ASSET_TYPE_SCRIPT: {

        // TODO

        break;
      }
      case FS_ASSET_TYPE_SOUND: {

        // TODO

        break;
      }
    }

    if (map64_insert(&s_instances, hash, (uint64_t)handle) == 0) {

      // TODO: We should never reach this!
      //       Hash collision detected..

      __debugbreak();
    }
  };

  handle_t *handle = *(handle_t **)map64_at(&s_instances, hash);

  if (handle->instance == 0) {

    // TODO: We should never reach this!
    //       Instance was not created..

    __debugbreak();
  }

  return handle->instance;
}
void idb_dereference(handle_t *handle) {
  // TODO
}
void idb_destroy(void) {
  map64_iter_t it = map64_iter(&s_instances);

  while (map64_next(&it)) {

    // TODO
  }

  map64_destroy(&s_instances);
}
