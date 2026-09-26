#include <ti_pch.h>

#define CGLTF_IMPLEMENTATION
#include <cgltf.h>

#include <glslang/Include/glslang_c_interface.h>
#include <glslang/Public/resource_limits_c.h>

#include <spirv_reflect.h>

static cgltf_accessor *find_gltf_accessor(cgltf_primitive *gltf_primitive, cgltf_attribute_type attribute_type);
static cgltf_node *find_gltf_root_node(cgltf_skin *gltf_skin);

static void convert_gltf_float_accessor_by_attribute(fs_primitive_t *primitive, cgltf_primitive *gltf_primitive, cgltf_attribute_type gltf_attribute_type);
static void convert_gltf_uint_accessor_by_attribute(fs_primitive_t *primitive, cgltf_primitive *gltf_primitive, cgltf_attribute_type gltf_attribute_type);

static void convert_gltf_model(fs_model_t *model, cgltf_data *gltf_data);
static void convert_gltf_mesh(fs_mesh_t *mesh, cgltf_mesh *gltf_mesh, uint64_t mesh_index);
static void convert_gltf_primitive(fs_primitive_t *primitive, cgltf_primitive *gltf_primitive, uint64_t primitive_index);
static void convert_gltf_skin(fs_skin_t *skin, cgltf_skin *gltf_skin, uint64_t skin_index);
static void convert_gltf_joint(fs_joint_t *joint, cgltf_node *gltf_node);

static void convert_ttf_font(fs_font_t *font, void *buffer, uint64_t buffer_size);

static uint8_t compile_glsl_shader(fs_pipeline_t *pipeline, char const *file_path, glslang_stage_t stage);

static uint64_t count_spirv_input_variables(SpvReflectShaderModule *module);
static uint64_t count_spirv_descriptor_bindings(SpvReflectShaderModule *module);

static uint8_t convert_spirv_input_variables(fs_asset_t *asset, fs_pipeline_t *pipeline, uint64_t *input_variable_offset, SpvReflectShaderModule *module);
static uint8_t convert_spirv_descriptor_bindings(fs_asset_t *asset, fs_pipeline_t *pipeline, uint64_t *descriptor_binding_offset, SpvReflectShaderModule *module);

uint8_t fs_import_model(fs_asset_t *asset, char const *model_file) {
  uint8_t status = 0;

  fs_model_t *model = (fs_model_t *)asset->instance;

  uint64_t path_size = strlen(asset->path);

  const char *file_name = fs_path_file_name(asset->path, path_size);
  const char *file_ext = fs_path_extension(asset->path, path_size);

  void *gltf_buffer = 0;
  uint64_t gltf_buffer_size = 0;

  cgltf_options gltf_options = {0};
  cgltf_data *gltf_data = 0;

  LARGE_INTEGER freq = {0};
  LARGE_INTEGER t0 = {0};
  LARGE_INTEGER t1 = {0};
  LARGE_INTEGER t2 = {0};
  LARGE_INTEGER t3 = {0};
  LARGE_INTEGER t4 = {0};
  LARGE_INTEGER t5 = {0};

  QueryPerformanceFrequency(&freq);
  QueryPerformanceCounter(&t0);

  if (fs_file_open_and_read(g_fs, model_file, FS_FORMAT_BINARY, &gltf_buffer, &gltf_buffer_size) != FS_SUCCESS) {

    status = 1;

    goto error;
  }

  QueryPerformanceCounter(&t1);

  if (cgltf_parse(&gltf_options, gltf_buffer, gltf_buffer_size, &gltf_data) != cgltf_result_success) {

    status = 1;

    goto error;
  }

  QueryPerformanceCounter(&t2);

  if (cgltf_load_buffers(&gltf_options, gltf_data, model_file) != cgltf_result_success) {

    status = 1;

    goto error;
  }

  QueryPerformanceCounter(&t3);

  if (cgltf_validate(gltf_data) != cgltf_result_success) {

    status = 1;

    goto error;
  }

  QueryPerformanceCounter(&t4);

  if (file_name && file_ext) {
    memcpy(model->name, file_name, file_ext - file_name - 1);
  } else {
    snprintf(model->name, TI_PATH_SIZE, "<unnamed>");
  }

  convert_gltf_model(model, gltf_data);

  QueryPerformanceCounter(&t5);

  LONGLONG d0 = ((t1.QuadPart - t0.QuadPart) * 1000) / freq.QuadPart;
  LONGLONG d1 = ((t2.QuadPart - t1.QuadPart) * 1000) / freq.QuadPart;
  LONGLONG d2 = ((t3.QuadPart - t2.QuadPart) * 1000) / freq.QuadPart;
  LONGLONG d3 = ((t4.QuadPart - t3.QuadPart) * 1000) / freq.QuadPart;
  LONGLONG d4 = ((t5.QuadPart - t4.QuadPart) * 1000) / freq.QuadPart;
  LONGLONG dt = d0 + d1 + d2 + d3 + d4;

  printf("Importing %s\n", asset->path);
  printf("  Load file        %8lld ms\n", d0);
  printf("  Parse file       %8lld ms\n", d1);
  printf("  Load buffer      %8lld ms\n", d2);
  printf("  Validate         %8lld ms\n", d3);
  printf("  Convert model    %8lld ms\n", d4);
  printf("  Total            %8llu ms\n", dt);
  printf("\n");

error:

  if (gltf_data) {
    cgltf_free(gltf_data);
  }

  if (gltf_buffer) {
    fs_free(gltf_buffer, 0);
  }

  return status;
}
uint8_t fs_import_font(fs_asset_t *asset, char const *font_file) {
  uint8_t status = 0;

  fs_font_t *font = (fs_font_t *)asset->instance;

  uint64_t path_size = strlen(asset->path);

  const char *file_name = fs_path_file_name(asset->path, path_size);
  const char *file_ext = fs_path_extension(asset->path, path_size);

  void *buffer = 0;
  uint64_t buffer_size = 0;

  LARGE_INTEGER freq = {0};
  LARGE_INTEGER t0 = {0};
  LARGE_INTEGER t1 = {0};
  LARGE_INTEGER t2 = {0};

  QueryPerformanceFrequency(&freq);
  QueryPerformanceCounter(&t0);

  if (fs_file_open_and_read(g_fs, font_file, FS_FORMAT_BINARY, &buffer, &buffer_size) != FS_SUCCESS) {

    status = 1;

    goto error;
  }

  QueryPerformanceCounter(&t1);

  convert_ttf_font(font, buffer, buffer_size);

  QueryPerformanceCounter(&t2);

  LONGLONG d0 = ((t1.QuadPart - t0.QuadPart) * 1000) / freq.QuadPart;
  LONGLONG d1 = ((t2.QuadPart - t1.QuadPart) * 1000) / freq.QuadPart;
  LONGLONG dt = d0 + d1;

  printf("Importing %s\n", asset->path);
  printf("  Read font        %8lld ms\n", d0);
  printf("  Convert font     %8lld ms\n", d1);
  printf("  Total            %8llu ms\n", dt);
  printf("\n");

error:

  if (buffer) {
    fs_free(buffer, 0);
  }

  return status;
}
uint8_t fs_import_pipeline(fs_asset_t *asset, fs_pipeline_type_t pipeline_type,
                           char const *vertex_file,
                           char const *fragment_file,
                           char const *task_file,
                           char const *mesh_file,
                           char const *ray_gen_file,
                           char const *ray_miss_file,
                           char const *ray_intersect_file,
                           char const *ray_closest_hit_file,
                           char const *compute_file) {
  uint8_t status = 0;

  fs_pipeline_t *pipeline = (fs_pipeline_t *)asset->instance;

  SpvReflectShaderModule vertex_module = {0};
  SpvReflectShaderModule fragment_module = {0};
  SpvReflectShaderModule task_module = {0};
  SpvReflectShaderModule mesh_module = {0};
  SpvReflectShaderModule ray_gen_module = {0};
  SpvReflectShaderModule ray_miss_module = {0};
  SpvReflectShaderModule ray_intersect_module = {0};
  SpvReflectShaderModule ray_closest_hit_module = {0};
  SpvReflectShaderModule compute_module = {0};

  LARGE_INTEGER freq = {0};
  LARGE_INTEGER t0 = {0};
  LARGE_INTEGER t1 = {0};
  LARGE_INTEGER t2 = {0};
  LARGE_INTEGER t3 = {0};

  QueryPerformanceFrequency(&freq);
  QueryPerformanceCounter(&t0);

  switch (pipeline_type) {

    case FS_PIPELINE_TYPE_DEFAULT: {

      if (compile_glsl_shader(pipeline, vertex_file, GLSLANG_STAGE_VERTEX)) {

        status = 1;

        goto error;
      }
      if (compile_glsl_shader(pipeline, fragment_file, GLSLANG_STAGE_FRAGMENT)) {

        status = 1;

        goto error;
      }

      QueryPerformanceCounter(&t1);

      if (spvReflectCreateShaderModule(sizeof(uint32_t) * pipeline->spirv_vertex_word_count, pipeline->spirv_vertex_words, &vertex_module) != SPV_REFLECT_RESULT_SUCCESS) {

        status = 1;

        goto error;
      }
      if (spvReflectCreateShaderModule(sizeof(uint32_t) * pipeline->spirv_fragment_word_count, pipeline->spirv_fragment_words, &fragment_module) != SPV_REFLECT_RESULT_SUCCESS) {

        status = 1;

        goto error;
      }

      QueryPerformanceCounter(&t2);

      uint64_t input_variable_offset = 0;
      uint64_t input_variable_count = 0;

      uint64_t descriptor_binding_offset = 0;
      uint64_t descriptor_binding_count = 0;

      input_variable_count += count_spirv_input_variables(&vertex_module);

      descriptor_binding_count += count_spirv_descriptor_bindings(&vertex_module);
      descriptor_binding_count += count_spirv_descriptor_bindings(&fragment_module);

      pipeline->input_variable_count = input_variable_count;
      pipeline->input_variables = (fs_asset_reference_t *)TI_ALLOC(sizeof(fs_asset_reference_t) * input_variable_count, 0, 0);

      pipeline->descriptor_binding_count = descriptor_binding_count;
      pipeline->descriptor_bindings = (fs_asset_reference_t *)TI_ALLOC(sizeof(fs_asset_reference_t) * descriptor_binding_count, 0, 0);

      convert_spirv_input_variables(asset, pipeline, &input_variable_offset, &vertex_module);

      convert_spirv_descriptor_bindings(asset, pipeline, &descriptor_binding_offset, &vertex_module);
      convert_spirv_descriptor_bindings(asset, pipeline, &descriptor_binding_offset, &fragment_module);

      break;
    }
    case FS_PIPELINE_TYPE_MESH: {

      if (compile_glsl_shader(pipeline, task_file, GLSLANG_STAGE_TASK)) {

        status = 1;

        goto error;
      }
      if (compile_glsl_shader(pipeline, mesh_file, GLSLANG_STAGE_MESH)) {

        status = 1;

        goto error;
      }
      if (compile_glsl_shader(pipeline, fragment_file, GLSLANG_STAGE_FRAGMENT)) {

        status = 1;

        goto error;
      }

      QueryPerformanceCounter(&t1);

      if (spvReflectCreateShaderModule(sizeof(uint32_t) * pipeline->spirv_task_word_count, pipeline->spirv_task_words, &task_module) != SPV_REFLECT_RESULT_SUCCESS) {

        status = 1;

        goto error;
      }
      if (spvReflectCreateShaderModule(sizeof(uint32_t) * pipeline->spirv_mesh_word_count, pipeline->spirv_mesh_words, &mesh_module) != SPV_REFLECT_RESULT_SUCCESS) {

        status = 1;

        goto error;
      }
      if (spvReflectCreateShaderModule(sizeof(uint32_t) * pipeline->spirv_fragment_word_count, pipeline->spirv_fragment_words, &fragment_module) != SPV_REFLECT_RESULT_SUCCESS) {

        status = 1;

        goto error;
      }

      QueryPerformanceCounter(&t2);

      uint64_t input_variable_count = 0;

      uint64_t descriptor_binding_offset = 0;
      uint64_t descriptor_binding_count = 0;

      descriptor_binding_count += count_spirv_descriptor_bindings(&task_module);
      descriptor_binding_count += count_spirv_descriptor_bindings(&mesh_module);
      descriptor_binding_count += count_spirv_descriptor_bindings(&fragment_module);

      pipeline->input_variable_count = input_variable_count;
      pipeline->descriptor_binding_count = descriptor_binding_count;

      pipeline->input_variables = (fs_asset_reference_t *)TI_ALLOC(sizeof(fs_asset_reference_t) * input_variable_count, 0, 0);
      pipeline->descriptor_bindings = (fs_asset_reference_t *)TI_ALLOC(sizeof(fs_asset_reference_t) * descriptor_binding_count, 0, 0);

      convert_spirv_descriptor_bindings(asset, pipeline, &descriptor_binding_offset, &task_module);
      convert_spirv_descriptor_bindings(asset, pipeline, &descriptor_binding_offset, &mesh_module);
      convert_spirv_descriptor_bindings(asset, pipeline, &descriptor_binding_offset, &fragment_module);

      break;
    }
    case FS_PIPELINE_TYPE_RAY_TRACING: {

      if (compile_glsl_shader(pipeline, ray_gen_file, GLSLANG_STAGE_RAYGEN)) {

        status = 1;

        goto error;
      }
      if (compile_glsl_shader(pipeline, ray_miss_file, GLSLANG_STAGE_MISS)) {

        status = 1;

        goto error;
      }
      if (compile_glsl_shader(pipeline, ray_intersect_file, GLSLANG_STAGE_INTERSECT)) {

        status = 1;

        goto error;
      }
      if (compile_glsl_shader(pipeline, ray_closest_hit_file, GLSLANG_STAGE_CLOSESTHIT)) {

        status = 1;

        goto error;
      }

      QueryPerformanceCounter(&t1);

      if (spvReflectCreateShaderModule(sizeof(uint32_t) * pipeline->spirv_ray_gen_word_count, pipeline->spirv_ray_gen_words, &ray_gen_module) != SPV_REFLECT_RESULT_SUCCESS) {

        status = 1;

        goto error;
      }
      if (spvReflectCreateShaderModule(sizeof(uint32_t) * pipeline->spirv_ray_miss_word_count, pipeline->spirv_ray_miss_words, &ray_miss_module) != SPV_REFLECT_RESULT_SUCCESS) {

        status = 1;

        goto error;
      }
      if (spvReflectCreateShaderModule(sizeof(uint32_t) * pipeline->spirv_ray_intersect_word_count, pipeline->spirv_ray_intersect_words, &ray_intersect_module) != SPV_REFLECT_RESULT_SUCCESS) {

        status = 1;

        goto error;
      }
      if (spvReflectCreateShaderModule(sizeof(uint32_t) * pipeline->spirv_ray_closest_hit_word_count, pipeline->spirv_ray_closest_hit_words, &ray_closest_hit_module) != SPV_REFLECT_RESULT_SUCCESS) {

        status = 1;

        goto error;
      }

      QueryPerformanceCounter(&t2);

      uint64_t input_variable_count = 0;

      uint64_t descriptor_binding_offset = 0;
      uint64_t descriptor_binding_count = 0;

      descriptor_binding_count += count_spirv_descriptor_bindings(&ray_gen_module);
      descriptor_binding_count += count_spirv_descriptor_bindings(&ray_miss_module);
      descriptor_binding_count += count_spirv_descriptor_bindings(&ray_intersect_module);
      descriptor_binding_count += count_spirv_descriptor_bindings(&ray_closest_hit_module);

      pipeline->input_variable_count = input_variable_count;
      pipeline->descriptor_binding_count = descriptor_binding_count;

      pipeline->input_variables = (fs_asset_reference_t *)TI_ALLOC(sizeof(fs_asset_reference_t) * input_variable_count, 0, 0);
      pipeline->descriptor_bindings = (fs_asset_reference_t *)TI_ALLOC(sizeof(fs_asset_reference_t) * descriptor_binding_count, 0, 0);

      convert_spirv_descriptor_bindings(asset, pipeline, &descriptor_binding_offset, &ray_gen_module);
      convert_spirv_descriptor_bindings(asset, pipeline, &descriptor_binding_offset, &ray_miss_module);
      convert_spirv_descriptor_bindings(asset, pipeline, &descriptor_binding_offset, &ray_intersect_module);
      convert_spirv_descriptor_bindings(asset, pipeline, &descriptor_binding_offset, &ray_closest_hit_module);

      break;
    }
    case FS_PIPELINE_TYPE_COMPUTE: {

      if (compile_glsl_shader(pipeline, compute_file, GLSLANG_STAGE_COMPUTE)) {

        status = 1;

        goto error;
      }

      QueryPerformanceCounter(&t1);

      if (spvReflectCreateShaderModule(sizeof(uint32_t) * pipeline->spirv_compute_word_count, pipeline->spirv_compute_words, &compute_module) != SPV_REFLECT_RESULT_SUCCESS) {

        status = 1;

        goto error;
      }

      QueryPerformanceCounter(&t2);

      uint64_t input_variable_count = 0;

      uint64_t descriptor_binding_offset = 0;
      uint64_t descriptor_binding_count = 0;

      descriptor_binding_count += count_spirv_descriptor_bindings(&compute_module);

      pipeline->input_variable_count = input_variable_count;
      pipeline->descriptor_binding_count = descriptor_binding_count;

      pipeline->input_variables = (fs_asset_reference_t *)TI_ALLOC(sizeof(fs_asset_reference_t) * input_variable_count, 0, 0);
      pipeline->descriptor_bindings = (fs_asset_reference_t *)TI_ALLOC(sizeof(fs_asset_reference_t) * descriptor_binding_count, 0, 0);

      convert_spirv_descriptor_bindings(asset, pipeline, &descriptor_binding_offset, &compute_module);

      break;
    }
  }

  QueryPerformanceCounter(&t3);

  LONGLONG d0 = ((t1.QuadPart - t0.QuadPart) * 1000) / freq.QuadPart;
  LONGLONG d1 = ((t2.QuadPart - t1.QuadPart) * 1000) / freq.QuadPart;
  LONGLONG d2 = ((t3.QuadPart - t2.QuadPart) * 1000) / freq.QuadPart;
  LONGLONG dt = d0 + d1 + d2;

  printf("Importing %s\n", asset->path);
  printf("  Compile shaders         %8lld ms\n", d0);
  printf("  Reflect shaders         %8lld ms\n", d1);
  printf("  Convert shaders         %8lld ms\n", d2);
  printf("  Total                   %8llu ms\n", dt);
  printf("\n");

error:

  switch (pipeline_type) {

    case FS_PIPELINE_TYPE_DEFAULT: {

      spvReflectDestroyShaderModule(&vertex_module);
      spvReflectDestroyShaderModule(&fragment_module);

      break;
    }
    case FS_PIPELINE_TYPE_MESH: {

      spvReflectDestroyShaderModule(&mesh_module);
      spvReflectDestroyShaderModule(&task_module);
      spvReflectDestroyShaderModule(&fragment_module);

      break;
    }
    case FS_PIPELINE_TYPE_RAY_TRACING: {

      spvReflectDestroyShaderModule(&ray_gen_module);
      spvReflectDestroyShaderModule(&ray_miss_module);
      spvReflectDestroyShaderModule(&ray_intersect_module);
      spvReflectDestroyShaderModule(&ray_closest_hit_module);

      break;
    }
    case FS_PIPELINE_TYPE_COMPUTE: {

      spvReflectDestroyShaderModule(&compute_module);

      break;
    }
  }

  return status;
}

static cgltf_accessor *find_gltf_accessor(cgltf_primitive *gltf_primitive, cgltf_attribute_type attribute_type) {
  cgltf_accessor *gltf_accessor = 0;

  uint64_t attribute_index = 0;
  uint64_t attribute_count = gltf_primitive->attributes_count;

  while (attribute_index < attribute_count) {

    cgltf_attribute *attribute = &gltf_primitive->attributes[attribute_index];

    if (attribute->type == attribute_type) {

      gltf_accessor = attribute->data;

      break;
    }

    attribute_index++;
  }

  return gltf_accessor;
}
static cgltf_node *find_gltf_root_node(cgltf_skin *gltf_skin) {
  cgltf_node *gltf_root_joint = 0;

  uint64_t joint_index = 0;
  uint64_t joint_count = gltf_skin->joints_count;

  while (joint_index < joint_count) {

    cgltf_node *gltf_joint = gltf_skin->joints[joint_index];

    if (_stricmp("root", gltf_joint->name) == 0) {

      gltf_root_joint = gltf_joint;

      break;
    }

    joint_index++;
  }

  return gltf_root_joint;
}

static void convert_gltf_float_accessor_by_attribute(fs_primitive_t *primitive, cgltf_primitive *gltf_primitive, cgltf_attribute_type gltf_attribute_type) {
  uint64_t component_count = 0;
  uint64_t value_index = 0;
  uint64_t value_count = 0;
  uint64_t value_stride = 0;

  cgltf_accessor *gltf_accessor = find_gltf_accessor(gltf_primitive, gltf_attribute_type);

  if (gltf_accessor == 0) {
    return;
  }

  component_count = cgltf_num_components(gltf_accessor->type);
  value_index = 0;
  value_count = gltf_accessor->count;
  value_stride = sizeof(float) * component_count;

  switch (gltf_attribute_type) {

    case cgltf_attribute_type_position: {

      primitive->position_count = value_count;
      primitive->position_stride = value_stride;
      primitive->positions = TI_ALLOC(value_stride * value_count, 0, 0);

      break;
    }
    case cgltf_attribute_type_normal: {

      primitive->normal_count = value_count;
      primitive->normal_stride = value_stride;
      primitive->normals = TI_ALLOC(value_stride * value_count, 0, 0);

      break;
    }
    case cgltf_attribute_type_tangent: {

      primitive->tangent_count = value_count;
      primitive->tangent_stride = value_stride;
      primitive->tangents = TI_ALLOC(value_stride * value_count, 0, 0);

      break;
    }
    case cgltf_attribute_type_texcoord: {

      primitive->texcoord_count = value_count;
      primitive->texcoord_stride = value_stride;
      primitive->texcoords = TI_ALLOC(value_stride * value_count, 0, 0);

      break;
    }
    case cgltf_attribute_type_color: {

      primitive->color_count = value_count;
      primitive->color_stride = value_stride;
      primitive->colors = TI_ALLOC(value_stride * value_count, 0, 0);

      break;
    }
    case cgltf_attribute_type_joints: {

      primitive->joint_count = value_count;
      primitive->joint_stride = value_stride;
      primitive->joints = TI_ALLOC(value_stride * value_count, 0, 0);

      break;
    }
    case cgltf_attribute_type_weights: {

      primitive->weight_count = value_count;
      primitive->weight_stride = value_stride;
      primitive->weights = TI_ALLOC(value_stride * value_count, 0, 0);

      break;
    }
  }

  while (value_index < value_count) {

    switch (gltf_attribute_type) {

      case cgltf_attribute_type_position: {

        cgltf_accessor_read_float(gltf_accessor, value_index, primitive->positions, component_count);

        break;
      }
      case cgltf_attribute_type_normal: {

        cgltf_accessor_read_float(gltf_accessor, value_index, primitive->normals, component_count);

        break;
      }
      case cgltf_attribute_type_tangent: {

        cgltf_accessor_read_float(gltf_accessor, value_index, primitive->tangents, component_count);

        break;
      }
      case cgltf_attribute_type_texcoord: {

        cgltf_accessor_read_float(gltf_accessor, value_index, primitive->texcoords, component_count);

        break;
      }
      case cgltf_attribute_type_color: {

        cgltf_accessor_read_float(gltf_accessor, value_index, primitive->colors, component_count);

        break;
      }
      case cgltf_attribute_type_joints: {

        cgltf_accessor_read_float(gltf_accessor, value_index, primitive->joints, component_count);

        break;
      }
      case cgltf_attribute_type_weights: {

        cgltf_accessor_read_float(gltf_accessor, value_index, primitive->weights, component_count);

        break;
      }
    }

    value_index++;
  }
}
static void convert_gltf_uint_accessor_by_attribute(fs_primitive_t *primitive, cgltf_primitive *gltf_primitive, cgltf_attribute_type gltf_attribute_type) {
  uint64_t component_count = 0;
  uint64_t value_index = 0;
  uint64_t value_count = 0;
  uint64_t value_stride = 0;

  cgltf_accessor *gltf_accessor = find_gltf_accessor(gltf_primitive, gltf_attribute_type);

  if (gltf_accessor == 0) {
    return;
  }

  component_count = cgltf_num_components(gltf_accessor->type);
  value_count = gltf_accessor->count;
  value_stride = sizeof(uint32_t) * component_count;

  switch (gltf_attribute_type) {

    case cgltf_attribute_type_position: {

      primitive->position_count = value_count;
      primitive->position_stride = value_stride;
      primitive->positions = TI_ALLOC(value_stride * value_count, 0, 0);

      break;
    }
    case cgltf_attribute_type_normal: {

      primitive->normal_count = value_count;
      primitive->normal_stride = value_stride;
      primitive->normals = TI_ALLOC(value_stride * value_count, 0, 0);

      break;
    }
    case cgltf_attribute_type_tangent: {

      primitive->tangent_count = value_count;
      primitive->tangent_stride = value_stride;
      primitive->tangents = TI_ALLOC(value_stride * value_count, 0, 0);

      break;
    }
    case cgltf_attribute_type_texcoord: {

      primitive->texcoord_count = value_count;
      primitive->texcoord_stride = value_stride;
      primitive->texcoords = TI_ALLOC(value_stride * value_count, 0, 0);

      break;
    }
    case cgltf_attribute_type_color: {

      primitive->color_count = value_count;
      primitive->color_stride = value_stride;
      primitive->colors = TI_ALLOC(value_stride * value_count, 0, 0);

      break;
    }
    case cgltf_attribute_type_joints: {

      primitive->joint_count = value_count;
      primitive->joint_stride = value_stride;
      primitive->joints = TI_ALLOC(value_stride * value_count, 0, 0);

      break;
    }
    case cgltf_attribute_type_weights: {

      primitive->weight_count = value_count;
      primitive->weight_stride = value_stride;
      primitive->weights = TI_ALLOC(value_stride * value_count, 0, 0);

      break;
    }
  }

  while (value_index < value_count) {

    switch (gltf_attribute_type) {

      case cgltf_attribute_type_position: {

        cgltf_accessor_read_uint(gltf_accessor, value_index, primitive->positions, component_count);

        break;
      }
      case cgltf_attribute_type_normal: {

        cgltf_accessor_read_uint(gltf_accessor, value_index, primitive->normals, component_count);

        break;
      }
      case cgltf_attribute_type_tangent: {

        cgltf_accessor_read_uint(gltf_accessor, value_index, primitive->tangents, component_count);

        break;
      }
      case cgltf_attribute_type_texcoord: {

        cgltf_accessor_read_uint(gltf_accessor, value_index, primitive->texcoords, component_count);

        break;
      }
      case cgltf_attribute_type_color: {

        cgltf_accessor_read_uint(gltf_accessor, value_index, primitive->colors, component_count);

        break;
      }
      case cgltf_attribute_type_joints: {

        cgltf_accessor_read_uint(gltf_accessor, value_index, primitive->joints, component_count);

        break;
      }
      case cgltf_attribute_type_weights: {

        cgltf_accessor_read_uint(gltf_accessor, value_index, primitive->weights, component_count);

        break;
      }
    }

    value_index++;
  }
}

static void convert_gltf_model(fs_model_t *model, cgltf_data *gltf_data) {
  model->mesh_count = gltf_data->meshes_count;
  model->meshes = (fs_mesh_t *)TI_ALLOC(sizeof(fs_mesh_t) * gltf_data->meshes_count, 0, 0);

  uint64_t mesh_index = 0;
  uint64_t mesh_count = gltf_data->meshes_count;

  while (mesh_index < mesh_count) {

    convert_gltf_mesh(&model->meshes[mesh_index], &gltf_data->meshes[mesh_index], mesh_index);

    mesh_index++;
  }

  model->skin_count = gltf_data->skins_count;
  model->skins = (fs_skin_t *)TI_ALLOC(sizeof(fs_skin_t) * gltf_data->skins_count, 0, 0);

  uint64_t skin_index = 0;
  uint64_t skin_count = gltf_data->skins_count;

  while (skin_index < skin_count) {

    convert_gltf_skin(&model->skins[skin_index], &gltf_data->skins[skin_index], skin_index);

    skin_index++;
  }
}
static void convert_gltf_mesh(fs_mesh_t *mesh, cgltf_mesh *gltf_mesh, uint64_t mesh_index) {
  if (gltf_mesh->name) {
    strcpy(mesh->name, gltf_mesh->name);
  } else {
    snprintf(mesh->name, TI_PATH_SIZE, "mesh %llu", mesh_index);
  }

  mesh->primitive_count = gltf_mesh->primitives_count;
  mesh->primitives = (fs_primitive_t *)TI_ALLOC(sizeof(fs_primitive_t) * gltf_mesh->primitives_count, 0, 0);

  uint64_t primitive_index = 0;
  uint64_t primitive_count = gltf_mesh->primitives_count;

  while (primitive_index < primitive_count) {

    convert_gltf_primitive(&mesh->primitives[primitive_index], &gltf_mesh->primitives[primitive_index], primitive_index);

    primitive_index++;
  }
}
static void convert_gltf_primitive(fs_primitive_t *primitive, cgltf_primitive *gltf_primitive, uint64_t primitive_index) {
  snprintf(primitive->name, TI_PATH_SIZE, "primitive %llu", primitive_index);

  convert_gltf_float_accessor_by_attribute(primitive, gltf_primitive, cgltf_attribute_type_position);
  convert_gltf_float_accessor_by_attribute(primitive, gltf_primitive, cgltf_attribute_type_normal);
  convert_gltf_float_accessor_by_attribute(primitive, gltf_primitive, cgltf_attribute_type_tangent);
  convert_gltf_float_accessor_by_attribute(primitive, gltf_primitive, cgltf_attribute_type_texcoord);
  convert_gltf_float_accessor_by_attribute(primitive, gltf_primitive, cgltf_attribute_type_color);
  convert_gltf_uint_accessor_by_attribute(primitive, gltf_primitive, cgltf_attribute_type_joints);
  convert_gltf_float_accessor_by_attribute(primitive, gltf_primitive, cgltf_attribute_type_weights);
}
static void convert_gltf_skin(fs_skin_t *skin, cgltf_skin *gltf_skin, uint64_t skin_index) {
  if (gltf_skin->name) {
    strcpy(skin->name, gltf_skin->name);
  } else {
    snprintf(skin->name, TI_PATH_SIZE, "skin %llu", skin_index);
  }

  skin->root_joint = (fs_joint_t *)TI_ALLOC(sizeof(fs_joint_t), 1, 0);

  convert_gltf_joint(skin->root_joint, find_gltf_root_node(gltf_skin));
}
static void convert_gltf_joint(fs_joint_t *joint, cgltf_node *gltf_node) {
  strcpy(joint->name, gltf_node->name);

  joint->child_count = gltf_node->children_count;
  joint->children = (fs_joint_t *)TI_ALLOC(sizeof(fs_joint_t) * gltf_node->children_count, 1, 0);

  uint64_t child_index = 0;
  uint64_t child_count = gltf_node->children_count;

  while (child_index < child_count) {

    convert_gltf_joint(&joint->children[child_index], gltf_node->children[child_index]);

    child_index++;
  }
}

static void convert_ttf_font(fs_font_t *font, void *buffer, uint64_t buffer_size) {
  font->buffer = TI_ALLOC(buffer_size, 0, 0);
  memcpy(font->buffer, buffer, buffer_size);
  font->buffer_size = buffer_size;
}

static uint8_t compile_glsl_shader(fs_pipeline_t *pipeline, char const *file_path, glslang_stage_t stage) {
  uint8_t status = 0;

  void *buffer = 0;
  uint64_t buffer_size = 0;

  glslang_program_t *program = 0;
  glslang_shader_t *shader = 0;

  if (fs_file_open_and_read(g_fs, file_path, FS_FORMAT_TEXT, &buffer, &buffer_size) != FS_SUCCESS) {

    status = 1;

    goto error;
  }

  glslang_initialize_process();

  glslang_input_t stage_input = {
    .language = GLSLANG_SOURCE_GLSL,
    .stage = stage,
    .client = GLSLANG_CLIENT_VULKAN,
    .client_version = GLSLANG_TARGET_VULKAN_1_3,
    .target_language = GLSLANG_TARGET_SPV,
    .target_language_version = GLSLANG_TARGET_SPV_1_6,
    .code = buffer,
    .default_version = 460,
    .default_profile = GLSLANG_CORE_PROFILE,
    .force_default_version_and_profile = 1,
    .forward_compatible = 1,
    .messages = GLSLANG_MSG_DEFAULT_BIT,
    .resource = glslang_default_resource(),
  };

  shader = glslang_shader_create(&stage_input);

  if ((glslang_shader_preprocess(shader, &stage_input) == 0) ||
      (glslang_shader_parse(shader, &stage_input) == 0)) {

    printf("Compile Error %s\n", file_path);
    printf(glslang_shader_get_info_log(shader));
    printf("\n");

    status = 1;

    goto error;
  }

  program = glslang_program_create();

  glslang_program_add_shader(program, shader);

  if (glslang_program_link(program, GLSLANG_MSG_SPV_RULES_BIT) == 0) {

    printf("Link Error %s\n", file_path);
    printf(glslang_program_get_info_log(program));
    printf("\n");

    status = 1;

    goto error;
  }

  glslang_program_SPIRV_generate(program, stage);

  uint64_t word_count = glslang_program_SPIRV_get_size(program);
  uint32_t *words = glslang_program_SPIRV_get_ptr(program);

  switch (stage) {

    case GLSLANG_STAGE_VERTEX: {

      pipeline->spirv_vertex_word_count = word_count;
      pipeline->spirv_vertex_words = TI_ALLOC(sizeof(uint32_t) * word_count, 0, words);

      pipeline->glsl_vertex_shader_size = buffer_size;
      pipeline->glsl_vertex_shader = TI_ALLOC(buffer_size, 0, buffer);

      break;
    }
    case GLSLANG_STAGE_FRAGMENT: {

      pipeline->spirv_fragment_word_count = word_count;
      pipeline->spirv_fragment_words = TI_ALLOC(sizeof(uint32_t) * word_count, 0, words);

      pipeline->glsl_fragment_shader_size = buffer_size;
      pipeline->glsl_fragment_shader = TI_ALLOC(buffer_size, 0, buffer);

      break;
    }
    case GLSLANG_STAGE_TASK: {

      pipeline->spirv_task_word_count = word_count;
      pipeline->spirv_task_words = TI_ALLOC(sizeof(uint32_t) * word_count, 0, words);

      pipeline->glsl_task_shader_size = buffer_size;
      pipeline->glsl_task_shader = TI_ALLOC(buffer_size, 0, buffer);

      break;
    }
    case GLSLANG_STAGE_MESH: {

      pipeline->spirv_mesh_word_count = word_count;
      pipeline->spirv_mesh_words = TI_ALLOC(sizeof(uint32_t) * word_count, 0, words);

      pipeline->glsl_mesh_shader_size = buffer_size;
      pipeline->glsl_mesh_shader = TI_ALLOC(buffer_size, 0, buffer);

      break;
    }
    case GLSLANG_STAGE_RAYGEN: {

      pipeline->spirv_ray_gen_word_count = word_count;
      pipeline->spirv_ray_gen_words = TI_ALLOC(sizeof(uint32_t) * word_count, 0, words);

      pipeline->glsl_ray_gen_shader_size = buffer_size;
      pipeline->glsl_ray_gen_shader = TI_ALLOC(buffer_size, 0, buffer);

      break;
    }
    case GLSLANG_STAGE_MISS: {

      pipeline->spirv_ray_miss_word_count = word_count;
      pipeline->spirv_ray_miss_words = TI_ALLOC(sizeof(uint32_t) * word_count, 0, words);

      pipeline->glsl_ray_miss_shader_size = buffer_size;
      pipeline->glsl_ray_miss_shader = TI_ALLOC(buffer_size, 0, buffer);

      break;
    }
    case GLSLANG_STAGE_INTERSECT: {

      pipeline->spirv_ray_intersect_word_count = word_count;
      pipeline->spirv_ray_intersect_words = TI_ALLOC(sizeof(uint32_t) * word_count, 0, words);

      pipeline->glsl_ray_intersect_shader_size = buffer_size;
      pipeline->glsl_ray_intersect_shader = TI_ALLOC(buffer_size, 0, buffer);

      break;
    }
    case GLSLANG_STAGE_CLOSESTHIT: {

      pipeline->spirv_ray_closest_hit_word_count = word_count;
      pipeline->spirv_ray_closest_hit_words = TI_ALLOC(sizeof(uint32_t) * word_count, 0, words);

      pipeline->glsl_ray_closest_hit_shader_size = buffer_size;
      pipeline->glsl_ray_closest_hit_shader = TI_ALLOC(buffer_size, 0, buffer);

      break;
    }
  }

error:

  if (program) {
    glslang_program_delete(program);
  }

  if (shader) {
    glslang_shader_delete(shader);
  }

  glslang_finalize_process();

  if (buffer) {
    fs_free(buffer, 0);
  }

  return status;
}

static uint64_t count_spirv_input_variables(SpvReflectShaderModule *module) {
  uint32_t input_variable_count = 0;

  spvReflectEnumerateInputVariables(module, &input_variable_count, 0);

  return input_variable_count;
}
static uint64_t count_spirv_descriptor_bindings(SpvReflectShaderModule *module) {
  uint32_t descriptor_binding_count = 0;

  spvReflectEnumerateDescriptorBindings(module, &descriptor_binding_count, 0);

  return descriptor_binding_count;
}

static uint8_t convert_spirv_input_variables(fs_asset_t *asset, fs_pipeline_t *pipeline, uint64_t *input_variable_offset, SpvReflectShaderModule *module) {
  uint8_t status = 0;

  uint32_t input_variable_index = 0;
  uint32_t input_variable_count = 0;

  SpvReflectInterfaceVariable **spirv_input_variables = 0;

  if (spvReflectEnumerateInputVariables(module, &input_variable_count, 0) != SPV_REFLECT_RESULT_SUCCESS) {

    status = 1;

    goto error;
  }

  spirv_input_variables = (SpvReflectInterfaceVariable **)TI_ALLOC(sizeof(SpvReflectInterfaceVariable *) * input_variable_count, 0, 0);

  if (spvReflectEnumerateInputVariables(module, &input_variable_count, spirv_input_variables) != SPV_REFLECT_RESULT_SUCCESS) {

    status = 1;

    goto error;
  }

  while (input_variable_index < input_variable_count) {

    SpvReflectInterfaceVariable *spirv_input_variable = spirv_input_variables[input_variable_index];

    char input_variable_file_path[TI_PATH_SIZE] = {0};

    fs_path_parent(asset->path, input_variable_file_path);

    snprintf(input_variable_file_path, TI_PATH_SIZE, "%s/input_variable/%s.pak", input_variable_file_path, spirv_input_variable->name);

    strcpy(pipeline->input_variables[(*input_variable_offset) + input_variable_index].reference_path, input_variable_file_path);

    fs_asset_t input_variable_asset = {
      .magic = TI_FS_ASSET_MAGIC,
      .type = FS_ASSET_TYPE_INPUT_VARIABLE,
      .path = input_variable_file_path,
    };

    if (fs_asset_exists(&input_variable_asset) == 0) {

      fs_asset_create(&input_variable_asset);

      fs_input_variable_t *input_variable = (fs_input_variable_t *)input_variable_asset.instance;

      if (spirv_input_variable->name) {
        snprintf(input_variable->name, TI_PATH_SIZE, "%s", spirv_input_variable->name);
      } else {
        snprintf(input_variable->name, TI_PATH_SIZE, "<unnamed>");
      }

      input_variable->location = spirv_input_variable->location;
      input_variable->is_built_in = spirv_input_variable->built_in != -1;
      input_variable->format_index = vk_find_format_index(spirv_input_variable->format);

      fs_asset_store(&input_variable_asset);
      fs_asset_destroy(&input_variable_asset);
    }

    input_variable_index++;
  }

  *input_variable_offset += input_variable_count;

error:

  if (spirv_input_variables) {
    TI_FREE(spirv_input_variables);
  }

  return status;
}
static uint8_t convert_spirv_descriptor_bindings(fs_asset_t *asset, fs_pipeline_t *pipeline, uint64_t *descriptor_binding_offset, SpvReflectShaderModule *module) {
  uint8_t status = 0;

  uint32_t descriptor_binding_index = 0;
  uint32_t descriptor_binding_count = 0;

  SpvReflectDescriptorBinding **spirv_descriptor_bindings = 0;

  if (spvReflectEnumerateDescriptorBindings(module, &descriptor_binding_count, 0) != SPV_REFLECT_RESULT_SUCCESS) {

    status = 1;

    goto error;
  }

  spirv_descriptor_bindings = (SpvReflectDescriptorBinding **)TI_ALLOC(sizeof(SpvReflectDescriptorBinding *) * descriptor_binding_count, 0, 0);

  if (spvReflectEnumerateDescriptorBindings(module, &descriptor_binding_count, spirv_descriptor_bindings) != SPV_REFLECT_RESULT_SUCCESS) {

    status = 1;

    goto error;
  }

  while (descriptor_binding_index < descriptor_binding_count) {

    SpvReflectDescriptorBinding *spriv_descriptor_binding = spirv_descriptor_bindings[descriptor_binding_index];

    char descriptor_binding_file_path[TI_PATH_SIZE] = {0};

    fs_path_parent(asset->path, descriptor_binding_file_path);

    snprintf(descriptor_binding_file_path, TI_PATH_SIZE, "%s/descriptor_binding/%s.pak", descriptor_binding_file_path, spriv_descriptor_binding->name);

    strcpy(pipeline->descriptor_bindings[(*descriptor_binding_offset) + descriptor_binding_index].reference_path, descriptor_binding_file_path);

    fs_asset_t descriptor_binding_asset = {
      .magic = TI_FS_ASSET_MAGIC,
      .type = FS_ASSET_TYPE_DESCRIPTOR_BINDING,
      .path = descriptor_binding_file_path,
    };

    if (fs_asset_exists(&descriptor_binding_asset) == 0) {

      fs_asset_create(&descriptor_binding_asset);

      fs_descriptor_binding_t *descriptor_binding = (fs_descriptor_binding_t *)descriptor_binding_asset.instance;

      if (spriv_descriptor_binding->name) {
        snprintf(descriptor_binding->name, TI_PATH_SIZE, "%s", spriv_descriptor_binding->name);
      } else {
        snprintf(descriptor_binding->name, TI_PATH_SIZE, "<unnamed>");
      }

      descriptor_binding->set = spriv_descriptor_binding->set;
      descriptor_binding->binding = spriv_descriptor_binding->binding;
      descriptor_binding->descriptor_type_index = vk_find_descriptor_type_index(spriv_descriptor_binding->descriptor_type);

      if ((spriv_descriptor_binding->descriptor_type == SPV_REFLECT_DESCRIPTOR_TYPE_UNIFORM_BUFFER) ||
          (spriv_descriptor_binding->descriptor_type == SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_BUFFER)) {

        descriptor_binding->block_size = spriv_descriptor_binding->block.size;
        descriptor_binding->block_variable_count = spriv_descriptor_binding->block.member_count;
        descriptor_binding->block_variables = TI_ALLOC(sizeof(fs_block_variable_t) * spriv_descriptor_binding->block.member_count, 0, 0);

        uint32_t block_variable_index = 0;
        uint32_t block_variable_count = spriv_descriptor_binding->block.member_count;

        while (block_variable_index < block_variable_count) {

          fs_block_variable_t *block_variable = &descriptor_binding->block_variables[block_variable_index];
          SpvReflectBlockVariable *spriv_block_variable = &spriv_descriptor_binding->block.members[block_variable_index];

          if (spriv_block_variable->name) {
            snprintf(block_variable->name, TI_PATH_SIZE, "%s", spriv_block_variable->name);
          } else {
            snprintf(block_variable->name, TI_PATH_SIZE, "<unnamed>");
          }

          block_variable->offset = spriv_block_variable->offset;
          block_variable->size = spriv_block_variable->size;

          block_variable_index++;
        }
      }

      fs_asset_store(&descriptor_binding_asset);
      fs_asset_destroy(&descriptor_binding_asset);
    }

    descriptor_binding_index++;
  }

  *descriptor_binding_offset += descriptor_binding_count;

error:

  if (spirv_descriptor_bindings) {
    TI_FREE(spirv_descriptor_bindings);
  }

  return status;
}
