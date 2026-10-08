#include <ti_adb.h>

static asset_type_t find_asset_type(char const *asset_type);

static map64_t s_assets = {0};

static enum_record_t s_asset_type_table[] = {
  {ASSET_TYPE_NONE, "ASSET_TYPE_NONE"},
  {ASSET_TYPE_INSTANCE, "ASSET_TYPE_INSTANCE"},
  {ASSET_TYPE_SWAPCHAIN, "ASSET_TYPE_SWAPCHAIN"},
  {ASSET_TYPE_BUFFER, "ASSET_TYPE_BUFFER"},
  {ASSET_TYPE_MODEL, "ASSET_TYPE_MODEL"},
  {ASSET_TYPE_PIPELINE, "ASSET_TYPE_PIPELINE"},
  {ASSET_TYPE_FONT, "ASSET_TYPE_FONT"},
  {ASSET_TYPE_DESCRIPTOR_BINDING, "ASSET_TYPE_DESCRIPTOR_BINDING"},
  {ASSET_TYPE_IMAGE, "ASSET_TYPE_IMAGE"},
  {ASSET_TYPE_FRAMEBUFFER, "ASSET_TYPE_FRAMEBUFFER"},
  {ASSET_TYPE_RENDERPASS, "ASSET_TYPE_RENDERPASS"},
  {ASSET_TYPE_RENDERER, "ASSET_TYPE_RENDERER"},
  {ASSET_TYPE_INPUT_VARIABLE, "ASSET_TYPE_INPUT_VARIABLE"},
  {ASSET_TYPE_SCRIPT, "ASSET_TYPE_SCRIPT"},
  {ASSET_TYPE_SOUND, "ASSET_TYPE_SOUND"},
};

void adb_create(void) {
  map64_create(&s_assets);
}
asset_handle_t *adb_handle(uint64_t parent, char const *asset_path) {
  uint64_t hash = fnv1a64(parent, (uint8_t *)asset_path, strlen(asset_path));

  if (map64_contains(&s_assets, hash) == 0) {

    asset_handle_t *handle = (asset_handle_t *)TI_ALLOC(sizeof(asset_handle_t), 1, 0);

    strcpy(handle->asset_path, asset_path);

    handle->hash = hash;

    if (fsutil_load_text(&handle->buffer, &handle->buffer_size, asset_path) == 0) {

      // TODO: We should never reach this!
      //       File not found..

      __debugbreak();
    }

    handle->json = cJSON_ParseWithLength(handle->buffer, handle->buffer_size);

    if (handle->json == 0) {

      // TODO: We should never reach this!
      //       JSON could not be parsed..

      __debugbreak();
    }

    handle->type = find_asset_type(TI_JSON_STRING(handle->json, "type"));

    switch (handle->type) {

      case ASSET_TYPE_INSTANCE: {

        handle->instance = TI_ALLOC(sizeof(vk_instance_t), 1, 0);

        ((vk_instance_t *)handle->instance)->hash = handle->hash;
        ((vk_instance_t *)handle->instance)->config = handle->json;

        vk_instance_create(handle->instance);

        break;
      }
      case ASSET_TYPE_SWAPCHAIN: {

        handle->instance = TI_ALLOC(sizeof(vk_swapchain_t), 1, 0);

        ((vk_swapchain_t *)handle->instance)->hash = handle->hash;
        ((vk_swapchain_t *)handle->instance)->config = handle->json;

        vk_swapchain_create(handle->instance);

        break;
      }
      case ASSET_TYPE_BUFFER: {

        handle->instance = TI_ALLOC(sizeof(vk_buffer_t), 1, 0);

        ((vk_buffer_t *)handle->instance)->hash = handle->hash;
        ((vk_buffer_t *)handle->instance)->config = handle->json;

        vk_buffer_create(handle->instance);

        break;
      }
      case ASSET_TYPE_MODEL: {

        handle->instance = TI_ALLOC(sizeof(vk_model_t), 1, 0);

        ((vk_model_t *)handle->instance)->hash = handle->hash;
        ((vk_model_t *)handle->instance)->config = handle->json;

        vk_model_create(handle->instance);

        break;
      }
      case ASSET_TYPE_PIPELINE: {

        handle->instance = TI_ALLOC(sizeof(vk_pipeline_t), 1, 0);

        ((vk_pipeline_t *)handle->instance)->hash = handle->hash;
        ((vk_pipeline_t *)handle->instance)->config = handle->json;

        vk_pipeline_create(handle->instance);

        break;
      }
      case ASSET_TYPE_FONT: {

        handle->instance = TI_ALLOC(sizeof(vk_font_t), 1, 0);

        ((vk_font_t *)handle->instance)->hash = handle->hash;
        ((vk_font_t *)handle->instance)->config = handle->json;

        vk_font_create(handle->instance);

        break;
      }
      case ASSET_TYPE_DESCRIPTOR_BINDING: {

        handle->instance = TI_ALLOC(sizeof(vk_descriptor_binding_t), 1, 0);

        ((vk_descriptor_binding_t *)handle->instance)->hash = handle->hash;
        ((vk_descriptor_binding_t *)handle->instance)->config = handle->json;

        vk_descriptor_binding_create(handle->instance);

        break;
      }
      case ASSET_TYPE_IMAGE: {

        handle->instance = TI_ALLOC(sizeof(vk_image_t), 1, 0);

        ((vk_image_t *)handle->instance)->hash = handle->hash;
        ((vk_image_t *)handle->instance)->config = handle->json;

        vk_image_create(handle->instance);

        break;
      }
      case ASSET_TYPE_FRAMEBUFFER: {

        handle->instance = TI_ALLOC(sizeof(vk_framebuffer_t), 1, 0);

        ((vk_framebuffer_t *)handle->instance)->hash = handle->hash;
        ((vk_framebuffer_t *)handle->instance)->config = handle->json;

        vk_framebuffer_create(handle->instance);

        break;
      }
      case ASSET_TYPE_RENDERPASS: {

        handle->instance = TI_ALLOC(sizeof(vk_renderpass_t), 1, 0);

        ((vk_renderpass_t *)handle->instance)->hash = handle->hash;
        ((vk_renderpass_t *)handle->instance)->config = handle->json;

        vk_renderpass_create(handle->instance);

        break;
      }
      case ASSET_TYPE_RENDERER: {

        handle->instance = TI_ALLOC(sizeof(vk_renderer_t), 1, 0);

        ((vk_renderer_t *)handle->instance)->hash = handle->hash;
        ((vk_renderer_t *)handle->instance)->config = handle->json;

        vk_renderer_create(handle->instance);

        break;
      }
      case ASSET_TYPE_INPUT_VARIABLE: {

        // TODO

        break;
      }
      case ASSET_TYPE_SCRIPT: {

        // TODO

        break;
      }
      case ASSET_TYPE_SOUND: {

        // TODO

        break;
      }

      default: {

        // TODO: We should never reach this!
        //       Invalid asset type..

        __debugbreak();
      }
    }

    if (map64_insert(&s_assets, hash, (uint64_t)handle) == 0) {

      // TODO: We should never reach this!
      //       Hash collision detected..

      __debugbreak();
    }
  };

  asset_handle_t *handle = *(asset_handle_t **)map64_at(&s_assets, hash);

  if (handle->instance == 0) {

    // TODO: We should never reach this!
    //       Instance was not created..

    __debugbreak();
  }

  return handle->instance;
}
void adb_destroy(void) {
  map64_iter_t it = map64_iter(&s_assets);

  while (map64_next(&it)) {

    // TODO
  }

  map64_destroy(&s_assets);
}

static asset_type_t find_asset_type(char const *asset_type) {
  uint64_t index = 0;
  uint64_t count = TI_ARRAY_COUNT(s_asset_type_table);

  while (index < count) {

    if (strcmp(asset_type, s_asset_type_table[index].name) == 0) {
      return s_asset_type_table[index].value;
    }

    index++;
  }

  // TODO: We should never reach this!
  //       Someone has made an oopsie..

  __debugbreak();

  return 0;
}
