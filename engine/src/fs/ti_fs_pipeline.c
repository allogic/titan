#include <ti_pch.h>

static void read_input_variables(fs_pipeline_t *pipeline, fs_file *file);
static void read_descriptor_bindings(fs_pipeline_t *pipeline, fs_file *file);

static void write_input_variables(fs_pipeline_t *pipeline, fs_file *file);
static void write_descriptor_bindings(fs_pipeline_t *pipeline, fs_file *file);

void fs_pipeline_load(fs_pipeline_t *pipeline, fs_file *file) {
  memset(pipeline, 0, sizeof(fs_pipeline_t));

  fs_file_read(file, &pipeline->pipeline_type, sizeof(int32_t), 0);
  fs_file_read(file, &pipeline->descriptor_set_count, sizeof(uint32_t), 0);

  read_descriptor_bindings(pipeline, file);

  switch (pipeline->pipeline_type) {

    case FS_PIPELINE_TYPE_DEFAULT: {

      read_input_variables(pipeline, file);

      fs_file_read(file, &pipeline->enable_blending, sizeof(uint8_t), 0);
      fs_file_read(file, &pipeline->enable_depth_test, sizeof(uint8_t), 0);
      fs_file_read(file, &pipeline->enable_depth_write, sizeof(uint8_t), 0);

      fs_file_read(file, &pipeline->primitive_topology_index, sizeof(uint64_t), 0);
      fs_file_read(file, &pipeline->polygon_mode_index, sizeof(uint64_t), 0);
      fs_file_read(file, &pipeline->cull_mode_flags, sizeof(VkCullModeFlags), 0);

      fs_file_read(file, &pipeline->spirv_vertex_word_count, sizeof(uint64_t), 0);
      pipeline->spirv_vertex_words = TI_ALLOC(sizeof(uint32_t) * pipeline->spirv_vertex_word_count, 0, 0);
      fs_file_read(file, pipeline->spirv_vertex_words, sizeof(uint32_t) * pipeline->spirv_vertex_word_count, 0);

      fs_file_read(file, &pipeline->spirv_fragment_word_count, sizeof(uint64_t), 0);
      pipeline->spirv_fragment_words = TI_ALLOC(sizeof(uint32_t) * pipeline->spirv_fragment_word_count, 0, 0);
      fs_file_read(file, pipeline->spirv_fragment_words, sizeof(uint32_t) * pipeline->spirv_fragment_word_count, 0);

      fs_file_read(file, &pipeline->glsl_vertex_shader_size, sizeof(uint64_t), 0);
      pipeline->glsl_vertex_shader = TI_ALLOC(pipeline->glsl_vertex_shader_size, 0, 0);
      fs_file_read(file, &pipeline->glsl_vertex_shader, pipeline->glsl_vertex_shader_size, 0);

      fs_file_read(file, &pipeline->glsl_fragment_shader_size, sizeof(uint64_t), 0);
      pipeline->glsl_fragment_shader = TI_ALLOC(pipeline->glsl_fragment_shader_size, 0, 0);
      fs_file_read(file, &pipeline->glsl_fragment_shader, pipeline->glsl_fragment_shader_size, 0);

      break;
    }
    case FS_PIPELINE_TYPE_MESH: {

      fs_file_read(file, &pipeline->spirv_task_word_count, sizeof(uint64_t), 0);
      pipeline->spirv_task_words = TI_ALLOC(sizeof(uint32_t) * pipeline->spirv_task_word_count, 0, 0);
      fs_file_read(file, pipeline->spirv_task_words, sizeof(uint32_t) * pipeline->spirv_task_word_count, 0);

      fs_file_read(file, &pipeline->spirv_mesh_word_count, sizeof(uint64_t), 0);
      pipeline->spirv_mesh_words = TI_ALLOC(sizeof(uint32_t) * pipeline->spirv_mesh_word_count, 0, 0);
      fs_file_read(file, pipeline->spirv_mesh_words, sizeof(uint32_t) * pipeline->spirv_mesh_word_count, 0);

      fs_file_read(file, &pipeline->spirv_fragment_word_count, sizeof(uint64_t), 0);
      pipeline->spirv_fragment_words = TI_ALLOC(sizeof(uint32_t) * pipeline->spirv_fragment_word_count, 0, 0);
      fs_file_read(file, pipeline->spirv_fragment_words, sizeof(uint32_t) * pipeline->spirv_fragment_word_count, 0);

      fs_file_read(file, &pipeline->glsl_task_shader_size, sizeof(uint64_t), 0);
      pipeline->glsl_task_shader = TI_ALLOC(pipeline->glsl_task_shader_size, 0, 0);
      fs_file_read(file, &pipeline->glsl_task_shader, pipeline->glsl_task_shader_size, 0);

      fs_file_read(file, &pipeline->glsl_mesh_shader_size, sizeof(uint64_t), 0);
      pipeline->glsl_mesh_shader = TI_ALLOC(pipeline->glsl_mesh_shader_size, 0, 0);
      fs_file_read(file, &pipeline->glsl_mesh_shader, pipeline->glsl_mesh_shader_size, 0);

      fs_file_read(file, &pipeline->glsl_fragment_shader_size, sizeof(uint64_t), 0);
      pipeline->glsl_fragment_shader = TI_ALLOC(pipeline->glsl_fragment_shader_size, 0, 0);
      fs_file_read(file, &pipeline->glsl_fragment_shader, pipeline->glsl_fragment_shader_size, 0);

      break;
    }
    case FS_PIPELINE_TYPE_RAY_TRACING: {

      fs_file_read(file, &pipeline->spirv_ray_gen_word_count, sizeof(uint64_t), 0);
      pipeline->spirv_ray_gen_words = TI_ALLOC(sizeof(uint32_t) * pipeline->spirv_ray_gen_word_count, 0, 0);
      fs_file_read(file, pipeline->spirv_ray_gen_words, sizeof(uint32_t) * pipeline->spirv_ray_gen_word_count, 0);

      fs_file_read(file, &pipeline->spirv_ray_miss_word_count, sizeof(uint64_t), 0);
      pipeline->spirv_ray_miss_words = TI_ALLOC(sizeof(uint32_t) * pipeline->spirv_ray_miss_word_count, 0, 0);
      fs_file_read(file, pipeline->spirv_ray_miss_words, sizeof(uint32_t) * pipeline->spirv_ray_miss_word_count, 0);

      fs_file_read(file, &pipeline->spirv_ray_intersect_word_count, sizeof(uint64_t), 0);
      pipeline->spirv_ray_intersect_words = TI_ALLOC(sizeof(uint32_t) * pipeline->spirv_ray_intersect_word_count, 0, 0);
      fs_file_read(file, pipeline->spirv_ray_intersect_words, sizeof(uint32_t) * pipeline->spirv_ray_intersect_word_count, 0);

      fs_file_read(file, &pipeline->spirv_ray_closest_hit_word_count, sizeof(uint64_t), 0);
      pipeline->spirv_ray_closest_hit_words = TI_ALLOC(sizeof(uint32_t) * pipeline->spirv_ray_closest_hit_word_count, 0, 0);
      fs_file_read(file, pipeline->spirv_ray_closest_hit_words, sizeof(uint32_t) * pipeline->spirv_ray_closest_hit_word_count, 0);

      fs_file_read(file, &pipeline->glsl_ray_gen_shader_size, sizeof(uint64_t), 0);
      pipeline->glsl_ray_gen_shader = TI_ALLOC(pipeline->glsl_ray_gen_shader_size, 0, 0);
      fs_file_read(file, &pipeline->glsl_ray_gen_shader, pipeline->glsl_ray_gen_shader_size, 0);

      fs_file_read(file, &pipeline->glsl_ray_miss_shader_size, sizeof(uint64_t), 0);
      pipeline->glsl_ray_miss_shader = TI_ALLOC(pipeline->glsl_ray_miss_shader_size, 0, 0);
      fs_file_read(file, &pipeline->glsl_ray_miss_shader, pipeline->glsl_ray_miss_shader_size, 0);

      fs_file_read(file, &pipeline->glsl_ray_intersect_shader_size, sizeof(uint64_t), 0);
      pipeline->glsl_ray_intersect_shader = TI_ALLOC(pipeline->glsl_ray_intersect_shader_size, 0, 0);
      fs_file_read(file, &pipeline->glsl_ray_intersect_shader, pipeline->glsl_ray_intersect_shader_size, 0);

      fs_file_read(file, &pipeline->glsl_ray_closest_hit_shader_size, sizeof(uint64_t), 0);
      pipeline->glsl_ray_closest_hit_shader = TI_ALLOC(pipeline->glsl_ray_closest_hit_shader_size, 0, 0);
      fs_file_read(file, &pipeline->glsl_ray_closest_hit_shader, pipeline->glsl_ray_closest_hit_shader_size, 0);

      break;
    }
    case FS_PIPELINE_TYPE_COMPUTE: {

      fs_file_read(file, &pipeline->spirv_compute_word_count, sizeof(uint64_t), 0);
      pipeline->spirv_compute_words = TI_ALLOC(sizeof(uint32_t) * pipeline->spirv_compute_word_count, 0, 0);
      fs_file_read(file, pipeline->spirv_compute_words, sizeof(uint32_t) * pipeline->spirv_compute_word_count, 0);

      fs_file_read(file, &pipeline->glsl_compute_shader_size, sizeof(uint64_t), 0);
      pipeline->glsl_compute_shader = TI_ALLOC(pipeline->glsl_compute_shader_size, 0, 0);
      fs_file_read(file, &pipeline->glsl_compute_shader, pipeline->glsl_compute_shader_size, 0);

      break;
    }
  }
}
void fs_pipeline_store(fs_pipeline_t *pipeline, fs_file *file) {
  fs_file_write(file, &pipeline->pipeline_type, sizeof(fs_pipeline_type_t), 0);
  fs_file_write(file, &pipeline->descriptor_set_count, sizeof(uint32_t), 0);

  write_descriptor_bindings(pipeline, file);

  switch (pipeline->pipeline_type) {

    case FS_PIPELINE_TYPE_DEFAULT: {

      write_input_variables(pipeline, file);

      fs_file_write(file, &pipeline->enable_blending, sizeof(uint8_t), 0);
      fs_file_write(file, &pipeline->enable_depth_test, sizeof(uint8_t), 0);
      fs_file_write(file, &pipeline->enable_depth_write, sizeof(uint8_t), 0);

      fs_file_write(file, &pipeline->primitive_topology_index, sizeof(uint64_t), 0);
      fs_file_write(file, &pipeline->polygon_mode_index, sizeof(uint64_t), 0);
      fs_file_write(file, &pipeline->cull_mode_flags, sizeof(VkCullModeFlags), 0);

      fs_file_write(file, &pipeline->spirv_vertex_word_count, sizeof(uint64_t), 0);
      fs_file_write(file, pipeline->spirv_vertex_words, sizeof(uint32_t) * pipeline->spirv_vertex_word_count, 0);

      fs_file_write(file, &pipeline->spirv_fragment_word_count, sizeof(uint64_t), 0);
      fs_file_write(file, pipeline->spirv_fragment_words, sizeof(uint32_t) * pipeline->spirv_fragment_word_count, 0);

      fs_file_write(file, &pipeline->glsl_vertex_shader_size, sizeof(uint64_t), 0);
      fs_file_write(file, pipeline->glsl_vertex_shader, pipeline->glsl_vertex_shader_size, 0);

      fs_file_write(file, &pipeline->glsl_fragment_shader_size, sizeof(uint64_t), 0);
      fs_file_write(file, pipeline->glsl_fragment_shader, pipeline->glsl_fragment_shader_size, 0);

      break;
    }
    case FS_PIPELINE_TYPE_MESH: {

      fs_file_write(file, &pipeline->spirv_task_word_count, sizeof(uint64_t), 0);
      fs_file_write(file, pipeline->spirv_task_words, sizeof(uint32_t) * pipeline->spirv_task_word_count, 0);

      fs_file_write(file, &pipeline->spirv_mesh_word_count, sizeof(uint64_t), 0);
      fs_file_write(file, pipeline->spirv_mesh_words, sizeof(uint32_t) * pipeline->spirv_mesh_word_count, 0);

      fs_file_write(file, &pipeline->spirv_fragment_word_count, sizeof(uint64_t), 0);
      fs_file_write(file, pipeline->spirv_fragment_words, sizeof(uint32_t) * pipeline->spirv_fragment_word_count, 0);

      fs_file_write(file, &pipeline->glsl_task_shader_size, sizeof(uint64_t), 0);
      fs_file_write(file, pipeline->glsl_task_shader, pipeline->glsl_task_shader_size, 0);

      fs_file_write(file, &pipeline->glsl_mesh_shader_size, sizeof(uint64_t), 0);
      fs_file_write(file, pipeline->glsl_mesh_shader, pipeline->glsl_mesh_shader_size, 0);

      fs_file_write(file, &pipeline->glsl_fragment_shader_size, sizeof(uint64_t), 0);
      fs_file_write(file, pipeline->glsl_fragment_shader, pipeline->glsl_fragment_shader_size, 0);

      break;
    }
    case FS_PIPELINE_TYPE_RAY_TRACING: {

      fs_file_write(file, &pipeline->spirv_ray_gen_word_count, sizeof(uint64_t), 0);
      fs_file_write(file, pipeline->spirv_ray_gen_words, sizeof(uint32_t) * pipeline->spirv_ray_gen_word_count, 0);

      fs_file_write(file, &pipeline->spirv_ray_miss_word_count, sizeof(uint64_t), 0);
      fs_file_write(file, pipeline->spirv_ray_miss_words, sizeof(uint32_t) * pipeline->spirv_ray_miss_word_count, 0);

      fs_file_write(file, &pipeline->spirv_ray_intersect_word_count, sizeof(uint64_t), 0);
      fs_file_write(file, pipeline->spirv_ray_intersect_words, sizeof(uint32_t) * pipeline->spirv_ray_intersect_word_count, 0);

      fs_file_write(file, &pipeline->spirv_ray_closest_hit_word_count, sizeof(uint64_t), 0);
      fs_file_write(file, pipeline->spirv_ray_closest_hit_words, sizeof(uint32_t) * pipeline->spirv_ray_closest_hit_word_count, 0);

      fs_file_write(file, &pipeline->glsl_ray_gen_shader_size, sizeof(uint64_t), 0);
      fs_file_write(file, pipeline->glsl_ray_gen_shader, pipeline->glsl_ray_gen_shader_size, 0);

      fs_file_write(file, &pipeline->glsl_ray_miss_shader_size, sizeof(uint64_t), 0);
      fs_file_write(file, pipeline->glsl_ray_miss_shader, pipeline->glsl_ray_miss_shader_size, 0);

      fs_file_write(file, &pipeline->glsl_ray_intersect_shader_size, sizeof(uint64_t), 0);
      fs_file_write(file, pipeline->glsl_ray_intersect_shader, pipeline->glsl_ray_intersect_shader_size, 0);

      fs_file_write(file, &pipeline->glsl_ray_closest_hit_shader_size, sizeof(uint64_t), 0);
      fs_file_write(file, pipeline->glsl_ray_closest_hit_shader, pipeline->glsl_ray_closest_hit_shader_size, 0);

      break;
    }
    case FS_PIPELINE_TYPE_COMPUTE: {

      fs_file_write(file, &pipeline->spirv_compute_word_count, sizeof(uint64_t), 0);
      fs_file_write(file, pipeline->spirv_compute_words, sizeof(uint32_t) * pipeline->spirv_compute_word_count, 0);

      fs_file_write(file, &pipeline->glsl_compute_shader_size, sizeof(uint64_t), 0);
      fs_file_write(file, pipeline->glsl_compute_shader, pipeline->glsl_compute_shader_size, 0);

      break;
    }
  }
}
void fs_pipeline_destroy(fs_pipeline_t *pipeline) {
  TI_FREE(pipeline->descriptor_bindings);

  switch (pipeline->pipeline_type) {

    case FS_PIPELINE_TYPE_DEFAULT: {

      TI_FREE(pipeline->input_variables);

      TI_FREE(pipeline->spirv_vertex_words);
      TI_FREE(pipeline->spirv_fragment_words);

      TI_FREE(pipeline->glsl_vertex_shader);
      TI_FREE(pipeline->glsl_fragment_shader);

      break;
    }
    case FS_PIPELINE_TYPE_MESH: {

      TI_FREE(pipeline->spirv_task_words);
      TI_FREE(pipeline->spirv_mesh_words);
      TI_FREE(pipeline->spirv_fragment_words);

      TI_FREE(pipeline->glsl_task_shader);
      TI_FREE(pipeline->glsl_mesh_shader);
      TI_FREE(pipeline->glsl_fragment_shader);

      break;
    }
    case FS_PIPELINE_TYPE_RAY_TRACING: {

      TI_FREE(pipeline->spirv_ray_gen_words);
      TI_FREE(pipeline->spirv_ray_miss_words);
      TI_FREE(pipeline->spirv_ray_intersect_words);
      TI_FREE(pipeline->spirv_ray_closest_hit_words);

      TI_FREE(pipeline->glsl_ray_gen_shader);
      TI_FREE(pipeline->glsl_ray_miss_shader);
      TI_FREE(pipeline->glsl_ray_intersect_shader);
      TI_FREE(pipeline->glsl_ray_closest_hit_shader);

      break;
    }
    case FS_PIPELINE_TYPE_COMPUTE: {

      TI_FREE(pipeline->spirv_compute_words);

      TI_FREE(pipeline->glsl_compute_shader);

      break;
    }
  }
}

static void read_input_variables(fs_pipeline_t *pipeline, fs_file *file) {
  fs_file_read(file, &pipeline->input_variable_count, sizeof(uint64_t), 0);

  pipeline->input_variables = (fs_asset_reference_t *)TI_ALLOC(sizeof(fs_asset_reference_t) * pipeline->input_variable_count, 0, 0);

  uint64_t input_variable_index = 0;
  uint64_t input_variable_count = pipeline->input_variable_count;

  while (input_variable_index < input_variable_count) {

    fs_asset_reference_t *input_variable = &pipeline->input_variables[input_variable_index];

    fs_file_read(file, input_variable->reference_path, TI_PATH_SIZE, 0);

    input_variable_index++;
  }
}
static void read_descriptor_bindings(fs_pipeline_t *pipeline, fs_file *file) {
  fs_file_read(file, &pipeline->descriptor_binding_count, sizeof(uint64_t), 0);

  pipeline->descriptor_bindings = (fs_asset_reference_t *)TI_ALLOC(sizeof(fs_asset_reference_t) * pipeline->descriptor_binding_count, 0, 0);

  uint64_t descriptor_binding_index = 0;
  uint64_t descriptor_binding_count = pipeline->descriptor_binding_count;

  while (descriptor_binding_index < descriptor_binding_count) {

    fs_asset_reference_t *descriptor_binding = &pipeline->descriptor_bindings[descriptor_binding_index];

    fs_file_read(file, descriptor_binding->reference_path, TI_PATH_SIZE, 0);

    descriptor_binding_index++;
  }
}

static void write_input_variables(fs_pipeline_t *pipeline, fs_file *file) {
  fs_file_write(file, &pipeline->input_variable_count, sizeof(uint64_t), 0);

  uint64_t input_variable_index = 0;
  uint64_t input_variable_count = pipeline->input_variable_count;

  while (input_variable_index < input_variable_count) {

    fs_asset_reference_t *input_variable = &pipeline->input_variables[input_variable_index];

    fs_file_write(file, input_variable->reference_path, TI_PATH_SIZE, 0);

    input_variable_index++;
  }
}
static void write_descriptor_bindings(fs_pipeline_t *pipeline, fs_file *file) {
  fs_file_write(file, &pipeline->descriptor_binding_count, sizeof(uint64_t), 0);

  uint64_t descriptor_binding_index = 0;
  uint64_t descriptor_binding_count = pipeline->descriptor_binding_count;

  while (descriptor_binding_index < descriptor_binding_count) {

    fs_asset_reference_t *descriptor_binding = &pipeline->descriptor_bindings[descriptor_binding_index];

    fs_file_write(file, descriptor_binding->reference_path, TI_PATH_SIZE, 0);

    descriptor_binding_index++;
  }
}
