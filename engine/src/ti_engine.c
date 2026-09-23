#include <ti_pch.h>

// TODO: Check all VkImageMemoryBarrier's and remove double transitions depending on current renderpass (VkAttachmentDescription)
// TODO: Rename functions with their proper module name..

static void import_dflt_assets(void);
static void create_dflt_assets(void);

int32_t main(int32_t argc, char **argv) {
  dmalloc_init();

  __try {

    fs_create(ROOT_DIR "/static", ROOT_DIR "/asset");
    scene_create(&g_scene, "test", "asset/scene/test.pak");

    import_dflt_assets();
    create_dflt_assets();

    pl_window_create(&g_pl_window);
    pl_window_run(&g_pl_window);
    pl_window_destroy(&g_pl_window);

    scene_destroy(&g_scene);
    fs_destroy();

  } __except (EXCEPTION_EXECUTE_HANDLER) {

    printf("Something went wrong..\n"); // TODO
  }

  dmalloc_cleanup();

  return 0;
}

static void import_dflt_assets(void) {
  fs_import_font("asset/font/commit_mono_latin_400_normal.pak", "static/font/commit_mono_latin_400_normal.ttf");
  fs_import_font("asset/font/material_symbols_rounded_fill.pak", "static/font/material_symbols_rounded_fill.ttf");

  fs_import_pipeline(FS_PIPELINE_TYPE_DFLT, "asset/pipeline/standard/brdf.pak", "static/shader/standard/brdf.vert", "static/shader/standard/brdf.frag");
  fs_import_pipeline(FS_PIPELINE_TYPE_DFLT, "asset/pipeline/debug/line.pak", "static/shader/debug/line.vert", "static/shader/debug/line.frag");
}
static void create_dflt_assets(void) {
  fs_file_info info = {0};

  {
    fs_asset_t asset = {
      .magic = TI_FS_ASSET_MAGIC,
      .type = FS_ASSET_TYPE_SWAPCHAIN,
    };

    strcpy(asset.path, "asset/swapchain/main.pak");

    fs_asset_create(&asset);

    fs_swapchain_t *swapchain = (fs_swapchain_t *)asset.config;

    strcpy(swapchain->name, "main");

    if (fs_info(g_fs, asset.path, 0, &info) == FS_DOES_NOT_EXIST) {
      fs_asset_store(&asset);
    }

    fs_asset_destroy(&asset);
  }
  {
    fs_asset_t asset = {
      .magic = TI_FS_ASSET_MAGIC,
      .type = FS_ASSET_TYPE_RENDERER,
    };

    strcpy(asset.path, "asset/renderer/main.pak");

    fs_asset_create(&asset);

    fs_renderer_t *renderer = (fs_renderer_t *)asset.config;

    strcpy(renderer->name, "main");
    strcpy(renderer->debug_line_vertex_buffer, "asset/renderer/main/debug_line_vertex_buffer.pak");
    strcpy(renderer->debug_line_index_buffer, "asset/renderer/main/debug_line_index_buffer.pak");
    strcpy(renderer->full_screen_vertex_buffer, "asset/renderer/main/full_screen_vertex_buffer.pak");
    strcpy(renderer->full_screen_index_buffer, "asset/renderer/main/full_screen_index_buffer.pak");

    if (fs_info(g_fs, asset.path, 0, &info) == FS_DOES_NOT_EXIST) {
      fs_asset_store(&asset);
    }

    fs_asset_destroy(&asset);
  }

  {
    fs_asset_t asset = {
      .magic = TI_FS_ASSET_MAGIC,
      .type = FS_ASSET_TYPE_RENDERPASS,
    };

    strcpy(asset.path, "asset/renderpass/main.pak");

    fs_asset_create(&asset);

    fs_renderpass_t *renderpass = (fs_renderpass_t *)asset.config;

    strcpy(renderpass->name, "main");

    renderpass->initial_color_attachment_layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    renderpass->initial_depth_attachment_layout = VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL;
    renderpass->final_color_attachment_layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    renderpass->final_depth_attachment_layout = VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL;

    if (fs_info(g_fs, asset.path, 0, &info) == FS_DOES_NOT_EXIST) {
      fs_asset_store(&asset);
    }

    fs_asset_destroy(&asset);
  }
  {
    fs_asset_t asset = {
      .magic = TI_FS_ASSET_MAGIC,
      .type = FS_ASSET_TYPE_RENDERPASS,
    };

    strcpy(asset.path, "asset/renderpass/imgui.pak");

    fs_asset_create(&asset);

    fs_renderpass_t *renderpass = (fs_renderpass_t *)asset.config;

    strcpy(renderpass->name, "imgui");

    renderpass->initial_color_attachment_layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    renderpass->initial_depth_attachment_layout = VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_OPTIMAL;
    renderpass->final_color_attachment_layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    renderpass->final_depth_attachment_layout = VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_OPTIMAL;

    if (fs_info(g_fs, asset.path, 0, &info) == FS_DOES_NOT_EXIST) {
      fs_asset_store(&asset);
    }

    fs_asset_destroy(&asset);
  }

  {
    fs_asset_t asset = {
      .magic = TI_FS_ASSET_MAGIC,
      .type = FS_ASSET_TYPE_FRAMEBUFFER,
    };

    strcpy(asset.path, "asset/framebuffer/main.pak");

    fs_asset_create(&asset);

    fs_framebuffer_t *framebuffer = (fs_framebuffer_t *)asset.config;

    strcpy(framebuffer->name, "main");
    strcpy(framebuffer->color_attachment_image, "asset/framebuffer/main/attachments/color.pak");
    strcpy(framebuffer->depth_attachment_image, "asset/framebuffer/main/attachments/depth.pak");

    if (fs_info(g_fs, asset.path, 0, &info) == FS_DOES_NOT_EXIST) {
      fs_asset_store(&asset);
    }

    fs_asset_destroy(&asset);
  }
  {
    fs_asset_t asset = {
      .magic = TI_FS_ASSET_MAGIC,
      .type = FS_ASSET_TYPE_FRAMEBUFFER,
    };

    strcpy(asset.path, "asset/framebuffer/imgui.pak");

    fs_asset_create(&asset);

    fs_framebuffer_t *framebuffer = (fs_framebuffer_t *)asset.config;

    strcpy(framebuffer->name, "main");
    strcpy(framebuffer->color_attachment_image, "asset/framebuffer/imgui/attachments/color.pak");
    strcpy(framebuffer->depth_attachment_image, "asset/framebuffer/imgui/attachments/depth.pak");

    if (fs_info(g_fs, asset.path, 0, &info) == FS_DOES_NOT_EXIST) {
      fs_asset_store(&asset);
    }

    fs_asset_destroy(&asset);
  }

  {
    fs_asset_t asset = {
      .magic = TI_FS_ASSET_MAGIC,
      .type = FS_ASSET_TYPE_BUFFER,
    };

    strcpy(asset.path, "asset/buffer/time_info.pak");

    fs_asset_create(&asset);

    fs_buffer_t *buffer = (fs_buffer_t *)asset.config;

    strcpy(buffer->name, "main");

    buffer->zero_data = 0;
    buffer->size = sizeof(vk_time_info_t);
    buffer->buffer_usage_flags = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
    buffer->memory_property_flags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
    buffer->memory_allocate_flags = 0;

    if (fs_info(g_fs, asset.path, 0, &info) == FS_DOES_NOT_EXIST) {
      fs_asset_store(&asset);
    }

    fs_asset_destroy(&asset);
  }
  {
    fs_asset_t asset = {
      .magic = TI_FS_ASSET_MAGIC,
      .type = FS_ASSET_TYPE_BUFFER,
    };

    strcpy(asset.path, "asset/buffer/screen_info.pak");

    fs_asset_create(&asset);

    fs_buffer_t *buffer = (fs_buffer_t *)asset.config;

    strcpy(buffer->name, "main");

    buffer->zero_data = 0;
    buffer->size = sizeof(vk_screen_info_t);
    buffer->buffer_usage_flags = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
    buffer->memory_property_flags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
    buffer->memory_allocate_flags = 0;

    if (fs_info(g_fs, asset.path, 0, &info) == FS_DOES_NOT_EXIST) {
      fs_asset_store(&asset);
    }

    fs_asset_destroy(&asset);
  }
  {
    fs_asset_t asset = {
      .magic = TI_FS_ASSET_MAGIC,
      .type = FS_ASSET_TYPE_BUFFER,
    };

    strcpy(asset.path, "asset/buffer/mouse_info.pak");

    fs_asset_create(&asset);

    fs_buffer_t *buffer = (fs_buffer_t *)asset.config;

    strcpy(buffer->name, "main");

    buffer->zero_data = 0;
    buffer->size = sizeof(vk_mouse_info_t);
    buffer->buffer_usage_flags = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
    buffer->memory_property_flags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
    buffer->memory_allocate_flags = 0;

    if (fs_info(g_fs, asset.path, 0, &info) == FS_DOES_NOT_EXIST) {
      fs_asset_store(&asset);
    }

    fs_asset_destroy(&asset);
  }
  {
    fs_asset_t asset = {
      .magic = TI_FS_ASSET_MAGIC,
      .type = FS_ASSET_TYPE_BUFFER,
    };

    strcpy(asset.path, "asset/buffer/camera_info.pak");

    fs_asset_create(&asset);

    fs_buffer_t *buffer = (fs_buffer_t *)asset.config;

    strcpy(buffer->name, "main");

    buffer->zero_data = 0;
    buffer->size = sizeof(vk_camera_info_t);
    buffer->buffer_usage_flags = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
    buffer->memory_property_flags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
    buffer->memory_allocate_flags = 0;

    if (fs_info(g_fs, asset.path, 0, &info) == FS_DOES_NOT_EXIST) {
      fs_asset_store(&asset);
    }

    fs_asset_destroy(&asset);
  }

  {
    fs_asset_t asset = {
      .magic = TI_FS_ASSET_MAGIC,
      .type = FS_ASSET_TYPE_IMAGE,
    };

    strcpy(asset.path, "asset/framebuffer/main/attachments/color.pak");

    fs_asset_create(&asset);

    fs_image_t *image = (fs_image_t *)asset.config;

    strcpy(image->name, "color");

    image->width = 0;  // TODO
    image->height = 0; // TODO
    image->depth = 1;  // TODO
    image->mip_levels = 1;
    image->format = VK_FORMAT_R8G8B8A8_UNORM;
    image->image_layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    image->image_type = VK_IMAGE_TYPE_2D;
    image->image_tiling = VK_IMAGE_TILING_OPTIMAL;
    image->image_view_type = VK_IMAGE_VIEW_TYPE_2D;
    image->image_usage_flags = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
    image->image_aspect_flags = VK_IMAGE_ASPECT_COLOR_BIT;
    image->memory_property_flags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
    image->memory_allocate_flags = 0;

    if (fs_info(g_fs, asset.path, 0, &info) == FS_DOES_NOT_EXIST) {
      fs_asset_store(&asset);
    }

    fs_asset_destroy(&asset);
  }
  {
    fs_asset_t asset = {
      .magic = TI_FS_ASSET_MAGIC,
      .type = FS_ASSET_TYPE_IMAGE,
    };

    strcpy(asset.path, "asset/framebuffer/main/attachments/depth.pak");

    fs_asset_create(&asset);

    fs_image_t *image = (fs_image_t *)asset.config;

    strcpy(image->name, "depth");

    image->width = 0;  // TODO
    image->height = 0; // TODO
    image->depth = 1;  // TODO
    image->mip_levels = 1;
    image->format = VK_FORMAT_D32_SFLOAT;
    image->image_layout = VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL;
    image->image_type = VK_IMAGE_TYPE_2D;
    image->image_tiling = VK_IMAGE_TILING_OPTIMAL;
    image->image_view_type = VK_IMAGE_VIEW_TYPE_2D;
    image->image_usage_flags = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
    image->image_aspect_flags = VK_IMAGE_ASPECT_DEPTH_BIT;
    image->memory_property_flags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
    image->memory_allocate_flags = 0;

    if (fs_info(g_fs, asset.path, 0, &info) == FS_DOES_NOT_EXIST) {
      fs_asset_store(&asset);
    }

    fs_asset_destroy(&asset);
  }

  {
    fs_asset_t asset = {
      .magic = TI_FS_ASSET_MAGIC,
      .type = FS_ASSET_TYPE_IMAGE,
    };

    strcpy(asset.path, "asset/framebuffer/imgui/attachments/color.pak");

    fs_asset_create(&asset);

    fs_image_t *image = (fs_image_t *)asset.config;

    strcpy(image->name, "color");

    image->width = 0;  // TODO
    image->height = 0; // TODO
    image->depth = 1;  // TODO
    image->mip_levels = 1;
    image->format = VK_FORMAT_R8G8B8A8_UNORM;
    image->image_layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    image->image_type = VK_IMAGE_TYPE_2D;
    image->image_tiling = VK_IMAGE_TILING_OPTIMAL;
    image->image_view_type = VK_IMAGE_VIEW_TYPE_2D;
    image->image_usage_flags = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
    image->image_aspect_flags = VK_IMAGE_ASPECT_COLOR_BIT;
    image->memory_property_flags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
    image->memory_allocate_flags = 0;

    if (fs_info(g_fs, asset.path, 0, &info) == FS_DOES_NOT_EXIST) {
      fs_asset_store(&asset);
    }

    fs_asset_destroy(&asset);
  }
  {
    fs_asset_t asset = {
      .magic = TI_FS_ASSET_MAGIC,
      .type = FS_ASSET_TYPE_IMAGE,
    };

    strcpy(asset.path, "asset/framebuffer/imgui/attachments/depth.pak");

    fs_asset_create(&asset);

    fs_image_t *image = (fs_image_t *)asset.config;

    strcpy(image->name, "depth");

    image->width = 0;  // TODO
    image->height = 0; // TODO
    image->depth = 1;  // TODO
    image->mip_levels = 1;
    image->format = VK_FORMAT_D32_SFLOAT;
    image->image_layout = VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_OPTIMAL;
    image->image_type = VK_IMAGE_TYPE_2D;
    image->image_tiling = VK_IMAGE_TILING_OPTIMAL;
    image->image_view_type = VK_IMAGE_VIEW_TYPE_2D;
    image->image_usage_flags = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
    image->image_aspect_flags = VK_IMAGE_ASPECT_DEPTH_BIT;
    image->memory_property_flags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
    image->memory_allocate_flags = 0;

    if (fs_info(g_fs, asset.path, 0, &info) == FS_DOES_NOT_EXIST) {
      fs_asset_store(&asset);
    }

    fs_asset_destroy(&asset);
  }

  {
    fs_asset_t asset = {
      .magic = TI_FS_ASSET_MAGIC,
      .type = FS_ASSET_TYPE_BUFFER,
    };

    strcpy(asset.path, "asset/renderer/main/debug_line_vertex_buffer.pak");

    fs_asset_create(&asset);

    fs_buffer_t *buffer = (fs_buffer_t *)asset.config;

    strcpy(buffer->name, "main");

    buffer->zero_data = 0;
    buffer->size = sizeof(vk_debug_line_vertex_t) * TI_DEBUG_LINE_VERTEX_COUNT;
    buffer->buffer_usage_flags = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
    buffer->memory_property_flags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
    buffer->memory_allocate_flags = 0;

    if (fs_info(g_fs, asset.path, 0, &info) == FS_DOES_NOT_EXIST) {
      fs_asset_store(&asset);
    }

    fs_asset_destroy(&asset);
  }
  {
    fs_asset_t asset = {
      .magic = TI_FS_ASSET_MAGIC,
      .type = FS_ASSET_TYPE_BUFFER,
    };

    strcpy(asset.path, "asset/renderer/main/debug_line_index_buffer.pak");

    fs_asset_create(&asset);

    fs_buffer_t *buffer = (fs_buffer_t *)asset.config;

    strcpy(buffer->name, "main");

    buffer->zero_data = 0;
    buffer->size = sizeof(vk_debug_line_index_t) * TI_DEBUG_LINE_INDEX_COUNT;
    buffer->buffer_usage_flags = VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
    buffer->memory_property_flags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
    buffer->memory_allocate_flags = 0;

    if (fs_info(g_fs, asset.path, 0, &info) == FS_DOES_NOT_EXIST) {
      fs_asset_store(&asset);
    }

    fs_asset_destroy(&asset);
  }
  {
    fs_asset_t asset = {
      .magic = TI_FS_ASSET_MAGIC,
      .type = FS_ASSET_TYPE_BUFFER,
    };

    strcpy(asset.path, "asset/renderer/main/full_screen_vertex_buffer.pak");

    fs_asset_create(&asset);

    fs_buffer_t *buffer = (fs_buffer_t *)asset.config;

    strcpy(buffer->name, "main");

    buffer->zero_data = 0;
    buffer->size = sizeof(vk_full_screen_vertex_t) * 4;
    buffer->buffer_usage_flags = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
    buffer->memory_property_flags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
    buffer->memory_allocate_flags = 0;

    if (fs_info(g_fs, asset.path, 0, &info) == FS_DOES_NOT_EXIST) {
      fs_asset_store(&asset);
    }

    fs_asset_destroy(&asset);
  }
  {
    fs_asset_t asset = {
      .magic = TI_FS_ASSET_MAGIC,
      .type = FS_ASSET_TYPE_BUFFER,
    };

    strcpy(asset.path, "asset/renderer/main/full_screen_index_buffer.pak");

    fs_asset_create(&asset);

    fs_buffer_t *buffer = (fs_buffer_t *)asset.config;

    strcpy(buffer->name, "main");

    buffer->zero_data = 0;
    buffer->size = sizeof(vk_full_screen_index_t) * 6;
    buffer->buffer_usage_flags = VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
    buffer->memory_property_flags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
    buffer->memory_allocate_flags = 0;

    if (fs_info(g_fs, asset.path, 0, &info) == FS_DOES_NOT_EXIST) {
      fs_asset_store(&asset);
    }

    fs_asset_destroy(&asset);
  }
}
