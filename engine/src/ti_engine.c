#include <ti_pch.h>

// TODO: check all VkImageMemoryBarrier's and remove double transitions depending on current renderpass (VkAttachmentDescription)
// TODO: rename functions with their proper module name..
// TODO: refactor sound studio..
// TODO: refactor physic studio..
// TODO: make folder specifically for GLSLang..
// TODO: resolve asset references inside the gui and display it via TreeNodeEx..
// TODO: fix sound importer..
// TODO: check ECS transients for async asset loading..
// TODO: create a editor camera component and create an entity with it depending on wether we are in editor or runtime mode..
// TODO: handle events properly..
// TODO: create asset database..
// TODO: create a proper import dialog for every type..

static void import_default_assets(void);
static void create_default_assets(void);

int32_t main(int32_t argc, char **argv) {
  srand(GetTickCount()); // TODO

  dmalloc_init();

  __try {

    fs_create(ROOT_DIR "/static", ROOT_DIR "/asset");
    cl_compiler_create();
    audio_create();
    // TODO: Add missing ti_audio_demo_create()
    physic_create();
    scene_create(&g_scene);

    import_default_assets();
    create_default_assets();

    window_create();
    window_run();
    window_destroy();

    scene_destroy(&g_scene);
    ti_physic_demo_destroy(); // TODO
    physic_destroy();
    ti_audio_demo_destroy(); // TODO
    audio_destroy();
    cl_compiler_destroy();
    fs_destroy();

  } __except (EXCEPTION_EXECUTE_HANDLER) {

    printf("Something went wrong..\n"); // TODO
  }

  im_output_clear();
  dmalloc_cleanup();

  return 0;
}

static void import_default_assets(void) {
  // Sounds
  {
    uint32_t sound_index = 1;

    while (sound_index <= 10) {

      char asset_path[TI_PATH_SIZE] = {0};
      char source_path[TI_PATH_SIZE] = {0};

      snprintf(asset_path, TI_PATH_SIZE, "asset/sound/fart_%02u.pak", sound_index);
      snprintf(source_path, TI_PATH_SIZE, "static/sound/fart_%02u.wav", sound_index);

      fs_asset_t asset = {
        .magic = TI_FS_ASSET_MAGIC,
        .type = FS_ASSET_TYPE_SOUND,
        .path = asset_path,
      };

      if (fs_asset_exists(&asset) == 0) {

        fs_asset_create(&asset);

        if (fs_import_sound(&asset, source_path, 1) == 0) {
          fs_asset_store(&asset);
        }

        fs_asset_destroy(&asset);
      }

      sound_index++;
    }
  }

  // Models
  {
    fs_asset_t asset = {
      .magic = TI_FS_ASSET_MAGIC,
      .type = FS_ASSET_TYPE_MODEL,
      .path = "asset/character/veigar_greybeard.pak",
    };

    if (fs_asset_exists(&asset) == 0) {

      fs_asset_create(&asset);

      if (fs_import_model(&asset, "static/character/veigar_greybeard.glb") == 0) {

        fs_model_t *model = (fs_model_t *)asset.instance;

        fs_asset_store(&asset);
      }

      fs_asset_destroy(&asset);
    }
  }
  {
    fs_asset_t asset = {
      .magic = TI_FS_ASSET_MAGIC,
      .type = FS_ASSET_TYPE_MODEL,
      .path = "asset/character/viktor.pak",
    };

    if (fs_asset_exists(&asset) == 0) {

      fs_asset_create(&asset);

      if (fs_import_model(&asset, "static/character/viktor.glb") == 0) {

        fs_model_t *model = (fs_model_t *)asset.instance;

        fs_asset_store(&asset);
      }

      fs_asset_destroy(&asset);
    }
  }
  {
    fs_asset_t asset = {
      .magic = TI_FS_ASSET_MAGIC,
      .type = FS_ASSET_TYPE_MODEL,
      .path = "asset/character/demonblade_tryndamere.pak",
    };

    if (fs_asset_exists(&asset) == 0) {

      fs_asset_create(&asset);

      if (fs_import_model(&asset, "static/character/demonblade_tryndamere.glb") == 0) {

        fs_model_t *model = (fs_model_t *)asset.instance;

        fs_asset_store(&asset);
      }

      fs_asset_destroy(&asset);
    }
  }
  {
    fs_asset_t asset = {
      .magic = TI_FS_ASSET_MAGIC,
      .type = FS_ASSET_TYPE_MODEL,
      .path = "asset/character/spirit_blossom_springs_teemo.pak",
    };

    if (fs_asset_exists(&asset) == 0) {

      fs_asset_create(&asset);

      if (fs_import_model(&asset, "static/character/spirit_blossom_springs_teemo.glb") == 0) {

        fs_model_t *model = (fs_model_t *)asset.instance;

        fs_asset_store(&asset);
      }

      fs_asset_destroy(&asset);
    }
  }
  {
    fs_asset_t asset = {
      .magic = TI_FS_ASSET_MAGIC,
      .type = FS_ASSET_TYPE_MODEL,
      .path = "asset/character/prestige_pandemonium_shaco.pak",
    };

    if (fs_asset_exists(&asset) == 0) {

      fs_asset_create(&asset);

      if (fs_import_model(&asset, "static/character/prestige_pandemonium_shaco.glb") == 0) {

        fs_model_t *model = (fs_model_t *)asset.instance;

        fs_asset_store(&asset);
      }

      fs_asset_destroy(&asset);
    }
  }
  {
    fs_asset_t asset = {
      .magic = TI_FS_ASSET_MAGIC,
      .type = FS_ASSET_TYPE_MODEL,
      .path = "asset/character/old_god_malphite.pak",
    };

    if (fs_asset_exists(&asset) == 0) {

      fs_asset_create(&asset);

      if (fs_import_model(&asset, "static/character/old_god_malphite.glb") == 0) {

        fs_model_t *model = (fs_model_t *)asset.instance;

        fs_asset_store(&asset);
      }

      fs_asset_destroy(&asset);
    }
  }
  {
    fs_asset_t asset = {
      .magic = TI_FS_ASSET_MAGIC,
      .type = FS_ASSET_TYPE_MODEL,
      .path = "asset/character/high_noon_locke.pak",
    };

    if (fs_asset_exists(&asset) == 0) {

      fs_asset_create(&asset);

      if (fs_import_model(&asset, "static/character/high_noon_locke.glb") == 0) {

        fs_model_t *model = (fs_model_t *)asset.instance;

        fs_asset_store(&asset);
      }

      fs_asset_destroy(&asset);
    }
  }
  {
    fs_asset_t asset = {
      .magic = TI_FS_ASSET_MAGIC,
      .type = FS_ASSET_TYPE_MODEL,
      .path = "asset/character/immortalized_legend_kaisa.pak",
    };

    if (fs_asset_exists(&asset) == 0) {

      fs_asset_create(&asset);

      if (fs_import_model(&asset, "static/character/immortalized_legend_kaisa.glb") == 0) {

        fs_model_t *model = (fs_model_t *)asset.instance;

        fs_asset_store(&asset);
      }

      fs_asset_destroy(&asset);
    }
  }
  {
    fs_asset_t asset = {
      .magic = TI_FS_ASSET_MAGIC,
      .type = FS_ASSET_TYPE_MODEL,
      .path = "asset/character/old_god_mordekaiser.pak",
    };

    if (fs_asset_exists(&asset) == 0) {

      fs_asset_create(&asset);

      if (fs_import_model(&asset, "static/character/old_god_mordekaiser.glb") == 0) {

        fs_model_t *model = (fs_model_t *)asset.instance;

        fs_asset_store(&asset);
      }

      fs_asset_destroy(&asset);
    }
  }
  {
    fs_asset_t asset = {
      .magic = TI_FS_ASSET_MAGIC,
      .type = FS_ASSET_TYPE_MODEL,
      .path = "asset/character/steel_legion_garen.pak",
    };

    if (fs_asset_exists(&asset) == 0) {

      fs_asset_create(&asset);

      if (fs_import_model(&asset, "static/character/steel_legion_garen.glb") == 0) {

        fs_model_t *model = (fs_model_t *)asset.instance;

        fs_asset_store(&asset);
      }

      fs_asset_destroy(&asset);
    }
  }
  {
    fs_asset_t asset = {
      .magic = TI_FS_ASSET_MAGIC,
      .type = FS_ASSET_TYPE_MODEL,
      .path = "asset/character/pulsefire_caitlyn.pak",
    };

    if (fs_asset_exists(&asset) == 0) {

      fs_asset_create(&asset);

      if (fs_import_model(&asset, "static/character/pulsefire_caitlyn.glb") == 0) {

        fs_model_t *model = (fs_model_t *)asset.instance;

        fs_asset_store(&asset);
      }

      fs_asset_destroy(&asset);
    }
  }
  {
    fs_asset_t asset = {
      .magic = TI_FS_ASSET_MAGIC,
      .type = FS_ASSET_TYPE_MODEL,
      .path = "asset/character/grand_reckoning_sion.pak",
    };

    if (fs_asset_exists(&asset) == 0) {

      fs_asset_create(&asset);

      if (fs_import_model(&asset, "static/character/grand_reckoning_sion.glb") == 0) {

        fs_model_t *model = (fs_model_t *)asset.instance;

        fs_asset_store(&asset);
      }

      fs_asset_destroy(&asset);
    }
  }
  {
    fs_asset_t asset = {
      .magic = TI_FS_ASSET_MAGIC,
      .type = FS_ASSET_TYPE_MODEL,
      .path = "asset/character/marauder_kalista.pak",
    };

    if (fs_asset_exists(&asset) == 0) {

      fs_asset_create(&asset);

      if (fs_import_model(&asset, "static/character/marauder_kalista.glb") == 0) {

        fs_model_t *model = (fs_model_t *)asset.instance;

        fs_asset_store(&asset);
      }

      fs_asset_destroy(&asset);
    }
  }

  // Pipelines
  {
    fs_asset_t asset = {
      .magic = TI_FS_ASSET_MAGIC,
      .type = FS_ASSET_TYPE_PIPELINE,
      .path = "asset/pipeline/standard_brdf/main.pak",
    };

    if (fs_asset_exists(&asset) == 0) {

      fs_asset_create(&asset);

      if (fs_import_pipeline(&asset, FS_PIPELINE_TYPE_DEFAULT,
                             "static/shader/standard_brdf/main.vert",
                             "static/shader/standard_brdf/main.frag",
                             "", "", "", "", "", "", "") == 0) {

        fs_pipeline_t *pipeline = (fs_pipeline_t *)asset.instance;

        pipeline->pipeline_type = FS_PIPELINE_TYPE_DEFAULT;
        pipeline->primitive_topology_index = vk_find_primitive_topology_index(VK_PRIMITIVE_TOPOLOGY_LINE_LIST);
        pipeline->polygon_mode_index = vk_find_polygon_mode_index(VK_POLYGON_MODE_FILL);
        pipeline->cull_mode_flags = VK_CULL_MODE_BACK_BIT;
        pipeline->enable_blending = 1;
        pipeline->enable_depth_test = 1;
        pipeline->enable_depth_write = 1;
        pipeline->descriptor_set_count = 1;

        // TODO: create missing default BRDF pipeline..
        //         VkVertexInputBindingDescription
        //         VkVertexInputAttributeDescription
        //         VkDescriptorPoolSize
        //         VkDescriptorSetLayoutBinding

        fs_asset_store(&asset);
      }

      fs_asset_destroy(&asset);
    }
  }
  {
    fs_asset_t asset = {
      .magic = TI_FS_ASSET_MAGIC,
      .type = FS_ASSET_TYPE_PIPELINE,
      .path = "asset/pipeline/debug_line/main.pak",
    };

    if (fs_asset_exists(&asset) == 0) {

      fs_asset_create(&asset);

      if (fs_import_pipeline(&asset, FS_PIPELINE_TYPE_DEFAULT,
                             "static/shader/debug_line/main.vert",
                             "static/shader/debug_line/main.frag",
                             "", "", "", "", "", "", "") == 0) {

        fs_pipeline_t *pipeline = (fs_pipeline_t *)asset.instance;

        pipeline->pipeline_type = FS_PIPELINE_TYPE_DEFAULT;
        pipeline->primitive_topology_index = vk_find_primitive_topology_index(VK_PRIMITIVE_TOPOLOGY_LINE_LIST);
        pipeline->polygon_mode_index = vk_find_polygon_mode_index(VK_POLYGON_MODE_FILL);
        pipeline->cull_mode_flags = VK_CULL_MODE_BACK_BIT;
        pipeline->enable_blending = 1;
        pipeline->enable_depth_test = 1;
        pipeline->enable_depth_write = 1;
        pipeline->descriptor_set_count = 1;

        pipeline->vertex_input_binding_description_count = 1;
        pipeline->vertex_input_binding_description = (fs_vertex_input_binding_description_t *)TI_ALLOC(sizeof(fs_vertex_input_binding_description_t) * pipeline->vertex_input_binding_description_count, 1, 0);
        strcpy(pipeline->vertex_input_binding_description[0].name, "default");
        pipeline->vertex_input_binding_description[0].binding = 0;
        pipeline->vertex_input_binding_description[0].stride = sizeof(vk_debug_line_vertex_t); // TODO: remove this ref..
        pipeline->vertex_input_binding_description[0].input_rate_index = vk_find_vertex_input_rate_index(VK_VERTEX_INPUT_RATE_VERTEX);

        pipeline->vertex_input_attribute_description_count = 2;
        pipeline->vertex_input_attribute_description = (fs_vertex_input_attribute_description_t *)TI_ALLOC(sizeof(fs_vertex_input_attribute_description_t) * pipeline->vertex_input_attribute_description_count, 1, 0);
        strcpy(pipeline->vertex_input_attribute_description[0].name, "default");
        pipeline->vertex_input_attribute_description[0].location = 0;
        pipeline->vertex_input_attribute_description[0].binding = 0;
        pipeline->vertex_input_attribute_description[0].format_index = vk_find_format_index(VK_FORMAT_R32G32B32A32_SFLOAT);
        pipeline->vertex_input_attribute_description[0].offset = 0;
        pipeline->vertex_input_attribute_description[1].location = 1;
        pipeline->vertex_input_attribute_description[1].binding = 0;
        pipeline->vertex_input_attribute_description[1].format_index = vk_find_format_index(VK_FORMAT_R32G32B32A32_SFLOAT);
        pipeline->vertex_input_attribute_description[1].offset = TI_OFFSET_OF(vk_debug_line_vertex_t, color); // TODO: remove this ref..

        pipeline->descriptor_pool_size_count = 1;
        pipeline->descriptor_pool_size = (fs_descriptor_pool_size_t *)TI_ALLOC(sizeof(fs_descriptor_pool_size_t) * pipeline->descriptor_pool_size_count, 1, 0);
        strcpy(pipeline->descriptor_pool_size[0].name, "default");
        pipeline->descriptor_pool_size[0].type_index = vk_find_descriptor_type_index(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER);
        pipeline->descriptor_pool_size[0].descriptor_count = 1;

        pipeline->descriptor_set_layout_binding_count = 1;
        pipeline->descriptor_set_layout_binding = (fs_descriptor_set_layout_binding_t *)TI_ALLOC(sizeof(fs_descriptor_set_layout_binding_t) * pipeline->descriptor_set_layout_binding_count, 1, 0);
        strcpy(pipeline->descriptor_set_layout_binding[0].name, "default");
        pipeline->descriptor_set_layout_binding[0].binding = 0;
        pipeline->descriptor_set_layout_binding[0].descriptor_type_index = vk_find_descriptor_type_index(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER);
        pipeline->descriptor_set_layout_binding[0].descriptor_count = 1;
        pipeline->descriptor_set_layout_binding[0].stage_flags = VK_SHADER_STAGE_VERTEX_BIT;

        fs_asset_store(&asset);
      }

      fs_asset_destroy(&asset);
    }
  }

  // Fonts
  {
    fs_asset_t asset = {
      .magic = TI_FS_ASSET_MAGIC,
      .type = FS_ASSET_TYPE_FONT,
      .path = "asset/font/commit_mono_latin_400_normal.pak",
    };

    if (fs_asset_exists(&asset) == 0) {

      fs_asset_create(&asset);

      if (fs_import_font(&asset, "static/font/commit_mono_latin_400_normal.ttf") == 0) {

        fs_font_t *font = (fs_font_t *)asset.instance;

        fs_asset_store(&asset);
      }

      fs_asset_destroy(&asset);
    }
  }
  {
    fs_asset_t asset = {
      .magic = TI_FS_ASSET_MAGIC,
      .type = FS_ASSET_TYPE_FONT,
      .path = "asset/font/material_symbols_rounded_fill.pak",
    };

    if (fs_asset_exists(&asset) == 0) {

      fs_asset_create(&asset);

      if (fs_import_font(&asset, "static/font/material_symbols_rounded_fill.ttf") == 0) {

        fs_font_t *font = (fs_font_t *)asset.instance;

        fs_asset_store(&asset);
      }

      fs_asset_destroy(&asset);
    }
  }

  // Scripts
  {
    fs_asset_t asset = {
      .magic = TI_FS_ASSET_MAGIC,
      .type = FS_ASSET_TYPE_SCRIPT,
      .path = "asset/script/camera_controller.pak",
    };

    if (fs_asset_exists(&asset) == 0) {

      fs_asset_create(&asset);

      if (fs_import_script(&asset, "static/script/camera_controller.c") == 0) {

        fs_script_t *script = (fs_script_t *)asset.instance;

        fs_asset_store(&asset);
      }

      fs_asset_destroy(&asset);
    }
  }
}
static void create_default_assets(void) {
  // Globals instances
  {
    fs_asset_t asset = {
      .magic = TI_FS_ASSET_MAGIC,
      .type = FS_ASSET_TYPE_SWAPCHAIN,
      .path = "asset/swapchain/main.pak",
    };

    if (fs_asset_exists(&asset) == 0) {

      fs_asset_create(&asset);

      fs_swapchain_t *swapchain = (fs_swapchain_t *)asset.instance;

      swapchain->image_count = 2;

      fs_asset_store(&asset);
      fs_asset_destroy(&asset);
    }
  }
  {
    fs_asset_t asset = {
      .magic = TI_FS_ASSET_MAGIC,
      .type = FS_ASSET_TYPE_RENDERER,
      .path = "asset/renderer/main.pak",
    };

    if (fs_asset_exists(&asset) == 0) {

      fs_asset_create(&asset);

      fs_renderer_t *renderer = (fs_renderer_t *)asset.instance;

      strcpy(renderer->debug_line_vertex_buffer.reference_path, "asset/renderer/main/buffer/debug_line_vertex.pak");
      strcpy(renderer->debug_line_index_buffer.reference_path, "asset/renderer/main/buffer/debug_line_index.pak");
      strcpy(renderer->full_screen_vertex_buffer.reference_path, "asset/renderer/main/buffer/full_screen_vertex.pak");
      strcpy(renderer->full_screen_index_buffer.reference_path, "asset/renderer/main/buffer/full_screen_index.pak");

      fs_asset_store(&asset);
      fs_asset_destroy(&asset);
    }
  }

  // Renderpasses
  {
    fs_asset_t asset = {
      .magic = TI_FS_ASSET_MAGIC,
      .type = FS_ASSET_TYPE_RENDERPASS,
      .path = "asset/renderpass/main.pak",
    };

    if (fs_asset_exists(&asset) == 0) {

      fs_asset_create(&asset);

      fs_renderpass_t *renderpass = (fs_renderpass_t *)asset.instance;

      renderpass->initial_color_attachment_layout_index = vk_find_image_layout_index(VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
      renderpass->initial_depth_attachment_layout_index = vk_find_image_layout_index(VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL);
      renderpass->final_color_attachment_layout_index = vk_find_image_layout_index(VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
      renderpass->final_depth_attachment_layout_index = vk_find_image_layout_index(VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL);

      fs_asset_store(&asset);
      fs_asset_destroy(&asset);
    }
  }
  {
    fs_asset_t asset = {
      .magic = TI_FS_ASSET_MAGIC,
      .type = FS_ASSET_TYPE_RENDERPASS,
      .path = "asset/renderpass/imgui.pak",
    };

    if (fs_asset_exists(&asset) == 0) {

      fs_asset_create(&asset);

      fs_renderpass_t *renderpass = (fs_renderpass_t *)asset.instance;

      renderpass->initial_color_attachment_layout_index = vk_find_image_layout_index(VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
      renderpass->initial_depth_attachment_layout_index = vk_find_image_layout_index(VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_OPTIMAL);
      renderpass->final_color_attachment_layout_index = vk_find_image_layout_index(VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
      renderpass->final_depth_attachment_layout_index = vk_find_image_layout_index(VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_OPTIMAL);

      fs_asset_store(&asset);
      fs_asset_destroy(&asset);
    }
  }

  // Framebuffer
  {
    fs_asset_t asset = {
      .magic = TI_FS_ASSET_MAGIC,
      .type = FS_ASSET_TYPE_FRAMEBUFFER,
      .path = "asset/framebuffer/main.pak",
    };

    if (fs_asset_exists(&asset) == 0) {

      fs_asset_create(&asset);

      fs_framebuffer_t *framebuffer = (fs_framebuffer_t *)asset.instance;

      framebuffer->color_attachment_count = 1;
      framebuffer->color_attachment = (fs_asset_reference_t *)TI_ALLOC(sizeof(fs_asset_reference_t), 1, 0);

      strcpy(framebuffer->color_attachment[0].reference_path, "asset/framebuffer/main/attachment/color.pak");
      strcpy(framebuffer->depth_attachment.reference_path, "asset/framebuffer/main/attachment/depth.pak");

      fs_asset_store(&asset);
      fs_asset_destroy(&asset);
    }
  }
  {
    fs_asset_t asset = {
      .magic = TI_FS_ASSET_MAGIC,
      .type = FS_ASSET_TYPE_FRAMEBUFFER,
      .path = "asset/framebuffer/imgui.pak",
    };

    if (fs_asset_exists(&asset) == 0) {

      fs_asset_create(&asset);

      fs_framebuffer_t *framebuffer = (fs_framebuffer_t *)asset.instance;

      framebuffer->color_attachment_count = 1;
      framebuffer->color_attachment = (fs_asset_reference_t *)TI_ALLOC(sizeof(fs_asset_reference_t), 1, 0);

      strcpy(framebuffer->color_attachment[0].reference_path, "asset/framebuffer/imgui/attachment/color.pak");
      strcpy(framebuffer->depth_attachment.reference_path, "asset/framebuffer/imgui/attachment/depth.pak");

      fs_asset_store(&asset);
      fs_asset_destroy(&asset);
    }
  }

  // Global buffer
  {
    fs_asset_t asset = {
      .magic = TI_FS_ASSET_MAGIC,
      .type = FS_ASSET_TYPE_BUFFER,
      .path = "asset/buffer/time_info.pak",
    };

    if (fs_asset_exists(&asset) == 0) {

      fs_asset_create(&asset);

      fs_buffer_t *buffer = (fs_buffer_t *)asset.instance;

      buffer->zero_data = 0;
      buffer->size = sizeof(vk_time_info_t);
      buffer->buffer_usage_flags = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
      buffer->memory_property_flags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
      buffer->memory_allocate_flags = 0;

      fs_asset_store(&asset);
      fs_asset_destroy(&asset);
    }
  }
  {
    fs_asset_t asset = {
      .magic = TI_FS_ASSET_MAGIC,
      .type = FS_ASSET_TYPE_BUFFER,
      .path = "asset/buffer/screen_info.pak",
    };

    if (fs_asset_exists(&asset) == 0) {

      fs_asset_create(&asset);

      fs_buffer_t *buffer = (fs_buffer_t *)asset.instance;

      buffer->zero_data = 0;
      buffer->size = sizeof(vk_screen_info_t);
      buffer->buffer_usage_flags = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
      buffer->memory_property_flags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
      buffer->memory_allocate_flags = 0;

      fs_asset_store(&asset);
      fs_asset_destroy(&asset);
    }
  }
  {
    fs_asset_t asset = {
      .magic = TI_FS_ASSET_MAGIC,
      .type = FS_ASSET_TYPE_BUFFER,
      .path = "asset/buffer/mouse_info.pak",
    };

    if (fs_asset_exists(&asset) == 0) {

      fs_asset_create(&asset);

      fs_buffer_t *buffer = (fs_buffer_t *)asset.instance;

      buffer->zero_data = 0;
      buffer->size = sizeof(vk_mouse_info_t);
      buffer->buffer_usage_flags = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
      buffer->memory_property_flags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
      buffer->memory_allocate_flags = 0;

      fs_asset_store(&asset);
      fs_asset_destroy(&asset);
    }
  }
  {
    fs_asset_t asset = {
      .magic = TI_FS_ASSET_MAGIC,
      .type = FS_ASSET_TYPE_BUFFER,
      .path = "asset/buffer/camera_info.pak",
    };

    if (fs_asset_exists(&asset) == 0) {

      fs_asset_create(&asset);

      fs_buffer_t *buffer = (fs_buffer_t *)asset.instance;

      buffer->zero_data = 0;
      buffer->size = sizeof(vk_camera_info_t);
      buffer->buffer_usage_flags = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
      buffer->memory_property_flags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
      buffer->memory_allocate_flags = 0;

      fs_asset_store(&asset);
      fs_asset_destroy(&asset);
    }
  }

  // Main framebuffer attachments
  {
    fs_asset_t asset = {
      .magic = TI_FS_ASSET_MAGIC,
      .type = FS_ASSET_TYPE_IMAGE,
      .path = "asset/framebuffer/main/attachment/color.pak",
    };

    if (fs_asset_exists(&asset) == 0) {

      fs_asset_create(&asset);

      fs_image_t *image = (fs_image_t *)asset.instance;

      image->width = 0;  // TODO
      image->height = 0; // TODO
      image->depth = 1;  // TODO
      image->mip_levels = 1;
      image->format_index = vk_find_format_index(VK_FORMAT_R8G8B8A8_UNORM);
      image->image_layout_index = vk_find_image_layout_index(VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
      image->image_type_index = vk_find_image_type_index(VK_IMAGE_TYPE_2D);
      image->image_tiling_index = vk_find_image_tiling_index(VK_IMAGE_TILING_OPTIMAL);
      image->image_view_type_index = vk_find_image_view_type_index(VK_IMAGE_VIEW_TYPE_2D);
      image->image_usage_flags = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
      image->image_aspect_flags = VK_IMAGE_ASPECT_COLOR_BIT;
      image->memory_property_flags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
      image->memory_allocate_flags = 0;

      fs_asset_store(&asset);
      fs_asset_destroy(&asset);
    }
  }
  {
    fs_asset_t asset = {
      .magic = TI_FS_ASSET_MAGIC,
      .type = FS_ASSET_TYPE_IMAGE,
      .path = "asset/framebuffer/main/attachment/depth.pak",
    };

    if (fs_asset_exists(&asset) == 0) {

      fs_asset_create(&asset);

      fs_image_t *image = (fs_image_t *)asset.instance;

      image->width = 0;  // TODO
      image->height = 0; // TODO
      image->depth = 1;  // TODO
      image->mip_levels = 1;
      image->format_index = vk_find_format_index(VK_FORMAT_D32_SFLOAT);
      image->image_layout_index = vk_find_image_layout_index(VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL);
      image->image_type_index = vk_find_image_type_index(VK_IMAGE_TYPE_2D);
      image->image_tiling_index = vk_find_image_tiling_index(VK_IMAGE_TILING_OPTIMAL);
      image->image_view_type_index = vk_find_image_view_type_index(VK_IMAGE_VIEW_TYPE_2D);
      image->image_usage_flags = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
      image->image_aspect_flags = VK_IMAGE_ASPECT_DEPTH_BIT;
      image->memory_property_flags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
      image->memory_allocate_flags = 0;

      fs_asset_store(&asset);
      fs_asset_destroy(&asset);
    }
  }

  // Imgui framebuffer attachments
  {
    fs_asset_t asset = {
      .magic = TI_FS_ASSET_MAGIC,
      .type = FS_ASSET_TYPE_IMAGE,
      .path = "asset/framebuffer/imgui/attachment/color.pak",
    };

    if (fs_asset_exists(&asset) == 0) {

      fs_asset_create(&asset);

      fs_image_t *image = (fs_image_t *)asset.instance;

      image->width = 0;  // TODO
      image->height = 0; // TODO
      image->depth = 1;  // TODO
      image->mip_levels = 1;
      image->format_index = vk_find_format_index(VK_FORMAT_R8G8B8A8_UNORM);
      image->image_layout_index = vk_find_image_layout_index(VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
      image->image_type_index = vk_find_image_type_index(VK_IMAGE_TYPE_2D);
      image->image_tiling_index = vk_find_image_tiling_index(VK_IMAGE_TILING_OPTIMAL);
      image->image_view_type_index = vk_find_image_view_type_index(VK_IMAGE_VIEW_TYPE_2D);
      image->image_usage_flags = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
      image->image_aspect_flags = VK_IMAGE_ASPECT_COLOR_BIT;
      image->memory_property_flags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
      image->memory_allocate_flags = 0;

      fs_asset_store(&asset);
      fs_asset_destroy(&asset);
    }
  }
  {
    fs_asset_t asset = {
      .magic = TI_FS_ASSET_MAGIC,
      .type = FS_ASSET_TYPE_IMAGE,
      .path = "asset/framebuffer/imgui/attachment/depth.pak",
    };

    if (fs_asset_exists(&asset) == 0) {

      fs_asset_create(&asset);

      fs_image_t *image = (fs_image_t *)asset.instance;

      image->width = 0;  // TODO
      image->height = 0; // TODO
      image->depth = 1;  // TODO
      image->mip_levels = 1;
      image->format_index = vk_find_format_index(VK_FORMAT_D32_SFLOAT);
      image->image_layout_index = vk_find_image_layout_index(VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_OPTIMAL);
      image->image_type_index = vk_find_image_type_index(VK_IMAGE_TYPE_2D);
      image->image_tiling_index = vk_find_image_tiling_index(VK_IMAGE_TILING_OPTIMAL);
      image->image_view_type_index = vk_find_image_view_type_index(VK_IMAGE_VIEW_TYPE_2D);
      image->image_usage_flags = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
      image->image_aspect_flags = VK_IMAGE_ASPECT_DEPTH_BIT;
      image->memory_property_flags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
      image->memory_allocate_flags = 0;

      fs_asset_store(&asset);
      fs_asset_destroy(&asset);
    }
  }

  // Renderer buffer
  {
    fs_asset_t asset = {
      .magic = TI_FS_ASSET_MAGIC,
      .type = FS_ASSET_TYPE_BUFFER,
      .path = "asset/renderer/main/buffer/debug_line_vertex.pak",
    };

    if (fs_asset_exists(&asset) == 0) {

      fs_asset_create(&asset);

      fs_buffer_t *buffer = (fs_buffer_t *)asset.instance;

      buffer->zero_data = 0;
      buffer->size = sizeof(vk_debug_line_vertex_t) * TI_DEBUG_LINE_VERTEX_COUNT;
      buffer->buffer_usage_flags = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
      buffer->memory_property_flags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
      buffer->memory_allocate_flags = 0;

      fs_asset_store(&asset);
      fs_asset_destroy(&asset);
    }
  }
  {
    fs_asset_t asset = {
      .magic = TI_FS_ASSET_MAGIC,
      .type = FS_ASSET_TYPE_BUFFER,
      .path = "asset/renderer/main/buffer/debug_line_index.pak",
    };

    if (fs_asset_exists(&asset) == 0) {

      fs_asset_create(&asset);

      fs_buffer_t *buffer = (fs_buffer_t *)asset.instance;

      buffer->zero_data = 0;
      buffer->size = sizeof(vk_debug_line_index_t) * TI_DEBUG_LINE_INDEX_COUNT;
      buffer->buffer_usage_flags = VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
      buffer->memory_property_flags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
      buffer->memory_allocate_flags = 0;

      fs_asset_store(&asset);
      fs_asset_destroy(&asset);
    }
  }
  {
    fs_asset_t asset = {
      .magic = TI_FS_ASSET_MAGIC,
      .type = FS_ASSET_TYPE_BUFFER,
      .path = "asset/renderer/main/buffer/full_screen_vertex.pak",
    };

    if (fs_asset_exists(&asset) == 0) {

      fs_asset_create(&asset);

      fs_buffer_t *buffer = (fs_buffer_t *)asset.instance;

      buffer->zero_data = 0;
      buffer->size = sizeof(vk_full_screen_vertex_t) * 4;
      buffer->buffer_usage_flags = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
      buffer->memory_property_flags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
      buffer->memory_allocate_flags = 0;

      fs_asset_store(&asset);
      fs_asset_destroy(&asset);
    }
  }
  {
    fs_asset_t asset = {
      .magic = TI_FS_ASSET_MAGIC,
      .type = FS_ASSET_TYPE_BUFFER,
      .path = "asset/renderer/main/buffer/full_screen_index.pak",
    };

    if (fs_asset_exists(&asset) == 0) {

      fs_asset_create(&asset);

      fs_buffer_t *buffer = (fs_buffer_t *)asset.instance;

      buffer->zero_data = 0;
      buffer->size = sizeof(vk_full_screen_index_t) * 6;
      buffer->buffer_usage_flags = VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
      buffer->memory_property_flags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
      buffer->memory_allocate_flags = 0;

      fs_asset_store(&asset);
      fs_asset_destroy(&asset);
    }
  }
}
