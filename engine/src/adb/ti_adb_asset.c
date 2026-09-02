#include <ti_pch.h>

// TODO
// static void vk_model_create(vk_model_t *vk_model, fs_model_t *fs_model);
// static void vk_pipeline_create(vk_pipeline_t *vk_pipeline, fs_pipeline_t *fs_pipeline);
// static void vk_font_create(vk_font_t *vk_font, fs_font_t *fs_font);

void adb_asset_load(adb_asset_t *asset, fs_file *file) {
  fs_file_read(file, &asset->magic, sizeof(uint64_t), 0);

  if (asset->magic != TI_ADB_ASSET_MAGIC) {
    return;
  }

  // TODO: additionally do checksums..

  fs_file_read(file, &asset->type, sizeof(adb_asset_type_t), 0);

  switch (asset->type) {

    case ADB_ASSET_TYPE_MODEL: {

      asset->fs_instance = TI_ALLOC(sizeof(adb_model_t), 1, 0);

      adb_model_load((adb_model_t *)asset->fs_instance, file);
      // vk_model_create((vk_model_t *)asset->vk_instance, (adb_model_t *)asset->fs_instance);

      break;
    }
    case ADB_ASSET_TYPE_PIPELINE: {

      asset->fs_instance = TI_ALLOC(sizeof(adb_pipeline_t), 1, 0);

      adb_pipeline_load((adb_pipeline_t *)asset->fs_instance, file);
      // vk_pipeline_create((vk_pipeline_t *)asset->vk_instance, (adb_pipeline_t *)asset->fs_instance);

      // pipeline_t pipeline = {
      //   .pipeline_type = PIPELINE_TYPE_DFLT,
      //   .vertex_shader = "asset/shader/debug/line/vertex.spv",
      //   .fragment_shader = "asset/shader/debug/line/fragment.spv",
      //   .primitive_topology = VK_PRIMITIVE_TOPOLOGY_LINE_LIST,
      //   .polygon_mode = VK_POLYGON_MODE_FILL,
      //   .cull_mode = VK_CULL_MODE_BACK_BIT,
      //   .enable_blending = 1,
      //   .enable_depth_test = 1,
      //   .enable_depth_write = 1,
      //   .vertex_input_binding_description = s_debug_line_vertex_input_binding_description,
      //   .vertex_input_binding_description_count = TI_ARRAY_COUNT(s_debug_line_vertex_input_binding_description),
      //   .vertex_input_attribute_description = s_debug_line_vertex_input_attribute_description,
      //   .vertex_input_attribute_description_count = TI_ARRAY_COUNT(s_debug_line_vertex_input_attribute_description),
      //   .descriptor_pool_size = s_debug_line_renderer_descriptor_pool_size,
      //   .descriptor_pool_size_count = TI_ARRAY_COUNT(s_debug_line_renderer_descriptor_pool_size),
      //   .descriptor_set_layout_binding = s_debug_line_renderer_descriptor_set_layout_binding,
      //   .descriptor_set_layout_binding_count = TI_ARRAY_COUNT(s_debug_line_renderer_descriptor_set_layout_binding),
      //   .descriptor_set_count = 1,
      //   .render_pass = &g_renderpass,
      // };
      //
      // pipeline_create(&pipeline);

      break;
    }
    case ADB_ASSET_TYPE_FONT: {

      asset->fs_instance = TI_ALLOC(sizeof(adb_font_t), 1, 0);

      adb_font_load((adb_font_t *)asset->fs_instance, file);
      // vk_font_create((vk_font_t *)asset->vk_instance, (adb_font_t *)asset->fs_instance);

      break;
    }
  }
}
void adb_asset_store(adb_asset_t *asset, fs_file *file) {
}
void adb_asset_destroy(adb_asset_t *asset) {
}
