#include <ti_pch.h>

#define CGLTF_IMPLEMENTATION
#include <cgltf.h>

#include <glslang/Include/glslang_c_interface.h>
#include <glslang/Public/resource_limits_c.h>

#include <spirv_reflect.h>

static cgltf_accessor *find_gltf_accessor(cgltf_primitive *gltf_primitive, cgltf_attribute_type attribute_type);
static cgltf_node *find_gltf_root_node(cgltf_skin *gltf_skin);

static void convert_gltf_float_accessor(fs_file *file, cgltf_accessor *gltf_accessor);
static void convert_gltf_uint_accessor(fs_file *file, cgltf_accessor *gltf_accessor);

static void convert_gltf_model(fs_file *file, cgltf_data *gltf_data);
static void convert_gltf_mesh(fs_file *file, cgltf_mesh *gltf_mesh, uint64_t mesh_index);
static void convert_gltf_primitive(fs_file *file, cgltf_primitive *gltf_primitive, uint64_t primitive_index);
static void convert_gltf_skin(fs_file *file, cgltf_skin *gltf_skin, uint64_t skin_index);
static void convert_gltf_joint(fs_file *file, cgltf_node *gltf_node);

static uint8_t compile_glsl_shader(char const *file_path, glslang_stage_t stage, uint32_t **words, uint64_t *word_count);

static uint8_t convert_spirv_input_variables(fs_file *file, SpvReflectShaderModule *module);
static uint8_t convert_spirv_descriptor_bindings(fs_file *file, SpvReflectShaderModule *module);

uint8_t fs_import_model(char const *asset_file, char const *model_file) {
  uint8_t status = 0;

  void *gltf_buffer = 0;
  uint64_t gltf_buffer_size = 0;

  cgltf_options gltf_options = {0};
  cgltf_data *gltf_data = 0;

  fs_file *file = 0;

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

  if (fs_file_open(g_fs, asset_file, FS_WRITE, &file) == FS_SUCCESS) {

    uint64_t path_size = strlen(asset_file);

    const char *file_name = fs_path_file_name(asset_file, path_size);
    const char *file_ext = fs_path_extension(asset_file, path_size);

    char model_name[TI_PATH_SIZE] = {0};

    if (file_name && file_ext) {
      memcpy(model_name, file_name, file_ext - file_name - 1);
    } else {
      snprintf(model_name, TI_PATH_SIZE, "<unnamed>");
    }

    uint64_t asset_magic = TI_FS_ASSET_MAGIC;
    fs_asset_type_t asset_type = FS_ASSET_TYPE_MODEL;

    fs_file_write(file, &asset_magic, sizeof(uint64_t), 0);
    fs_file_write(file, &asset_type, sizeof(fs_asset_type_t), 0);
    fs_file_write(file, model_name, TI_PATH_SIZE, 0);

    convert_gltf_model(file, gltf_data);

    fs_file_close(file);
  }

  QueryPerformanceCounter(&t5);

  LONGLONG d0 = ((t1.QuadPart - t0.QuadPart) * 1000) / freq.QuadPart;
  LONGLONG d1 = ((t2.QuadPart - t1.QuadPart) * 1000) / freq.QuadPart;
  LONGLONG d2 = ((t3.QuadPart - t2.QuadPart) * 1000) / freq.QuadPart;
  LONGLONG d3 = ((t4.QuadPart - t3.QuadPart) * 1000) / freq.QuadPart;
  LONGLONG d4 = ((t5.QuadPart - t4.QuadPart) * 1000) / freq.QuadPart;
  LONGLONG dt = d0 + d1 + d2 + d3 + d4;

  printf("Importing %s\n", asset_file);
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
uint8_t fs_import_pipeline(fs_pipeline_type_t pipeline_type, char const *asset_file, char const *vertex_file, char const *fragment_file) {
  uint8_t status = 0;

  uint32_t *spirv_vertex_words = 0;
  uint32_t *spirv_fragment_words = 0;

  uint64_t spirv_vertex_word_count = 0;
  uint64_t spirv_fragment_word_count = 0;

  SpvReflectShaderModule vertex_module = {0};
  SpvReflectShaderModule fragment_module = {0};

  fs_file *file = 0;

  LARGE_INTEGER freq = {0};
  LARGE_INTEGER t0 = {0};
  LARGE_INTEGER t1 = {0};
  LARGE_INTEGER t2 = {0};
  LARGE_INTEGER t3 = {0};
  LARGE_INTEGER t4 = {0};

  QueryPerformanceFrequency(&freq);
  QueryPerformanceCounter(&t0);

  if (compile_glsl_shader(vertex_file, GLSLANG_STAGE_VERTEX, &spirv_vertex_words, &spirv_vertex_word_count)) {

    status = 1;

    goto error;
  }

  QueryPerformanceCounter(&t1);

  if (compile_glsl_shader(fragment_file, GLSLANG_STAGE_FRAGMENT, &spirv_fragment_words, &spirv_fragment_word_count)) {

    status = 1;

    goto error;
  }

  QueryPerformanceCounter(&t2);

  if (spvReflectCreateShaderModule(sizeof(uint32_t) * spirv_vertex_word_count, spirv_vertex_words, &vertex_module) != SPV_REFLECT_RESULT_SUCCESS) {

    status = 1;

    goto error;
  }
  if (spvReflectCreateShaderModule(sizeof(uint32_t) * spirv_fragment_word_count, spirv_fragment_words, &fragment_module) != SPV_REFLECT_RESULT_SUCCESS) {

    status = 1;

    goto error;
  }

  QueryPerformanceCounter(&t3);

  if (fs_file_open(g_fs, asset_file, FS_WRITE, &file) == FS_SUCCESS) {

    uint64_t path_size = strlen(asset_file);

    const char *file_name = fs_path_file_name(asset_file, path_size);
    const char *file_ext = fs_path_extension(asset_file, path_size);

    char program_name[TI_PATH_SIZE] = {0};

    if (file_name && file_ext) {
      memcpy(program_name, file_name, file_ext - file_name - 1);
    } else {
      snprintf(program_name, TI_PATH_SIZE, "<unnamed>");
    }

    uint64_t asset_magic = TI_FS_ASSET_MAGIC;
    fs_asset_type_t asset_type = FS_ASSET_TYPE_PIPELINE;

    fs_file_write(file, &asset_magic, sizeof(uint64_t), 0);
    fs_file_write(file, &asset_type, sizeof(fs_asset_type_t), 0);
    fs_file_write(file, program_name, TI_PATH_SIZE, 0);
    fs_file_write(file, &pipeline_type, sizeof(fs_pipeline_type_t), 0);

    switch (pipeline_type) {

      case FS_PIPELINE_TYPE_DFLT: {

        convert_spirv_input_variables(file, &vertex_module);
        convert_spirv_descriptor_bindings(file, &vertex_module);

        fs_file_write(file, &spirv_vertex_word_count, sizeof(uint64_t), 0);
        fs_file_write(file, spirv_vertex_words, sizeof(uint32_t) * spirv_vertex_word_count, 0);

        fs_file_write(file, &spirv_fragment_word_count, sizeof(uint64_t), 0);
        fs_file_write(file, spirv_fragment_words, sizeof(uint32_t) * spirv_fragment_word_count, 0);

        break;
      }
      case FS_PIPELINE_TYPE_MESH: {

        // TODO

        break;
      }
      case FS_PIPELINE_TYPE_RAY: {

        // TODO

        break;
      }
      case FS_PIPELINE_TYPE_COMP: {

        // TODO

        break;
      }
    }

    fs_file_close(file);
  }

  QueryPerformanceCounter(&t4);

  LONGLONG d0 = ((t1.QuadPart - t0.QuadPart) * 1000) / freq.QuadPart;
  LONGLONG d1 = ((t2.QuadPart - t1.QuadPart) * 1000) / freq.QuadPart;
  LONGLONG d2 = ((t3.QuadPart - t2.QuadPart) * 1000) / freq.QuadPart;
  LONGLONG d3 = ((t4.QuadPart - t3.QuadPart) * 1000) / freq.QuadPart;
  LONGLONG dt = d0 + d1 + d2 + d3;

  printf("Importing %s\n", asset_file);
  printf("  Compile vertex shader   %8lld ms\n", d0);
  printf("  Compile fragment shader %8lld ms\n", d1);
  printf("  Convert vertex shader   %8lld ms\n", d2);
  printf("  Convert fragment shader %8lld ms\n", d3);
  printf("  Total                   %8llu ms\n", dt);
  printf("\n");

error:

  spvReflectDestroyShaderModule(&vertex_module);
  spvReflectDestroyShaderModule(&fragment_module);

  if (spirv_vertex_words) {
    TI_FREE(spirv_vertex_words);
  }

  if (spirv_fragment_words) {
    TI_FREE(spirv_fragment_words);
  }

  return status;
}
uint8_t fs_import_font(char const *asset_file, char const *font_file) {
  uint8_t status = 0;

  void *buffer = 0;
  uint64_t buffer_size = 0;

  fs_file *file = 0;

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

  if (fs_file_open(g_fs, asset_file, FS_WRITE, &file) == FS_SUCCESS) {

    uint64_t path_size = strlen(asset_file);

    const char *file_name = fs_path_file_name(asset_file, path_size);
    const char *file_ext = fs_path_extension(asset_file, path_size);

    char font_name[TI_PATH_SIZE] = {0};

    if (file_name && file_ext) {
      memcpy(font_name, file_name, file_ext - file_name - 1);
    } else {
      snprintf(font_name, TI_PATH_SIZE, "<unnamed>");
    }

    uint64_t asset_magic = TI_FS_ASSET_MAGIC;
    fs_asset_type_t asset_type = FS_ASSET_TYPE_FONT;

    fs_file_write(file, &asset_magic, sizeof(uint64_t), 0);
    fs_file_write(file, &asset_type, sizeof(fs_asset_type_t), 0);
    fs_file_write(file, font_name, TI_PATH_SIZE, 0);
    fs_file_write(file, &buffer_size, sizeof(uint64_t), 0);
    fs_file_write(file, buffer, buffer_size, 0);

    fs_file_close(file);
  }

  QueryPerformanceCounter(&t2);

  LONGLONG d0 = ((t1.QuadPart - t0.QuadPart) * 1000) / freq.QuadPart;
  LONGLONG d1 = ((t2.QuadPart - t1.QuadPart) * 1000) / freq.QuadPart;
  LONGLONG dt = d0 + d1;

  printf("Importing %s\n", asset_file);
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

static void convert_gltf_float_accessor(fs_file *file, cgltf_accessor *gltf_accessor) {
  uint64_t component_count = 0;
  uint64_t value_index = 0;
  uint64_t value_count = 0;
  uint64_t value_stride = 0;

  float *float_buffer = 0;

  if (gltf_accessor) {

    component_count = cgltf_num_components(gltf_accessor->type);
    value_index = 0;
    value_count = gltf_accessor->count;
    value_stride = sizeof(float) * component_count;

    fs_file_write(file, &value_count, sizeof(uint64_t), 0);
    fs_file_write(file, &value_stride, sizeof(uint64_t), 0);

    float_buffer = (float *)TI_ALLOC(value_stride, 0, 0);

    while (value_index < value_count) {

      cgltf_accessor_read_float(gltf_accessor, value_index, float_buffer, component_count);

      fs_file_write(file, float_buffer, value_stride, 0);

      value_index++;
    }

    TI_FREE(float_buffer);

  } else {

    fs_file_write(file, &value_count, sizeof(uint64_t), 0);
    fs_file_write(file, &value_stride, sizeof(uint64_t), 0);
  }
}
static void convert_gltf_uint_accessor(fs_file *file, cgltf_accessor *gltf_accessor) {
  uint64_t component_count = 0;
  uint64_t value_index = 0;
  uint64_t value_count = 0;
  uint64_t value_stride = 0;

  uint32_t *int_buffer = 0;

  if (gltf_accessor) {

    component_count = cgltf_num_components(gltf_accessor->type);
    value_count = gltf_accessor->count;
    value_stride = sizeof(uint32_t) * component_count;

    fs_file_write(file, &value_count, sizeof(uint64_t), 0);
    fs_file_write(file, &value_stride, sizeof(uint64_t), 0);

    int_buffer = (uint32_t *)TI_ALLOC(value_stride, 0, 0);

    while (value_index < value_count) {

      cgltf_accessor_read_uint(gltf_accessor, value_index, int_buffer, component_count);

      fs_file_write(file, int_buffer, value_stride, 0);

      value_index++;
    }

    TI_FREE(int_buffer);

  } else {

    fs_file_write(file, &value_count, sizeof(uint64_t), 0);
    fs_file_write(file, &value_stride, sizeof(uint64_t), 0);
  }
}

static void convert_gltf_model(fs_file *file, cgltf_data *gltf_data) {
  uint64_t mesh_index = 0;
  uint64_t mesh_count = gltf_data->meshes_count;

  fs_file_write(file, &mesh_count, sizeof(uint64_t), 0);

  while (mesh_index < mesh_count) {

    convert_gltf_mesh(file, &gltf_data->meshes[mesh_index], mesh_index);

    mesh_index++;
  }

  uint64_t skin_index = 0;
  uint64_t skin_count = gltf_data->skins_count;

  fs_file_write(file, &skin_count, sizeof(uint64_t), 0);

  while (skin_index < skin_count) {

    convert_gltf_skin(file, &gltf_data->skins[skin_index], skin_index);

    skin_index++;
  }
}
static void convert_gltf_mesh(fs_file *file, cgltf_mesh *gltf_mesh, uint64_t mesh_index) {
  char mesh_name[TI_PATH_SIZE] = {0};

  if (gltf_mesh->name) {
    strcpy(mesh_name, gltf_mesh->name);
  } else {
    snprintf(mesh_name, TI_PATH_SIZE, "mesh %llu", mesh_index);
  }

  uint64_t primitive_index = 0;
  uint64_t primitive_count = gltf_mesh->primitives_count;

  fs_file_write(file, mesh_name, TI_PATH_SIZE, 0);
  fs_file_write(file, &primitive_count, sizeof(uint64_t), 0);

  while (primitive_index < primitive_count) {

    convert_gltf_primitive(file, &gltf_mesh->primitives[primitive_index], primitive_index);

    primitive_index++;
  }
}
static void convert_gltf_primitive(fs_file *file, cgltf_primitive *gltf_primitive, uint64_t primitive_index) {
  char primitive_name[TI_PATH_SIZE] = {0};

  snprintf(primitive_name, TI_PATH_SIZE, "primitive %llu", primitive_index);

  fs_file_write(file, primitive_name, TI_PATH_SIZE, 0);

  convert_gltf_float_accessor(file, find_gltf_accessor(gltf_primitive, cgltf_attribute_type_position));
  convert_gltf_float_accessor(file, find_gltf_accessor(gltf_primitive, cgltf_attribute_type_normal));
  convert_gltf_float_accessor(file, find_gltf_accessor(gltf_primitive, cgltf_attribute_type_tangent));
  convert_gltf_float_accessor(file, find_gltf_accessor(gltf_primitive, cgltf_attribute_type_texcoord));
  convert_gltf_float_accessor(file, find_gltf_accessor(gltf_primitive, cgltf_attribute_type_color));
  convert_gltf_uint_accessor(file, find_gltf_accessor(gltf_primitive, cgltf_attribute_type_joints));
  convert_gltf_float_accessor(file, find_gltf_accessor(gltf_primitive, cgltf_attribute_type_weights));
}
static void convert_gltf_skin(fs_file *file, cgltf_skin *gltf_skin, uint64_t skin_index) {
  char skin_name[TI_PATH_SIZE] = {0};

  if (gltf_skin->name) {
    strcpy(skin_name, gltf_skin->name);
  } else {
    snprintf(skin_name, TI_PATH_SIZE, "skin %llu", skin_index);
  }

  fs_file_write(file, skin_name, TI_PATH_SIZE, 0);

  convert_gltf_joint(file, find_gltf_root_node(gltf_skin));
}
static void convert_gltf_joint(fs_file *file, cgltf_node *gltf_node) {
  char joint_name[TI_PATH_SIZE] = {0};

  strcpy(joint_name, gltf_node->name);

  uint64_t child_index = 0;
  uint64_t child_count = gltf_node->children_count;

  fs_file_write(file, joint_name, TI_PATH_SIZE, 0);
  fs_file_write(file, &child_count, sizeof(uint64_t), 0);

  while (child_index < child_count) {

    convert_gltf_joint(file, gltf_node->children[child_index]);

    child_index++;
  }
}

static uint8_t compile_glsl_shader(char const *file_path, glslang_stage_t stage, uint32_t **words, uint64_t *word_count) {
  uint8_t status = 0;

  void *buffer = 0;
  uint64_t buffer_size = 0;

  if (fs_file_open_and_read(g_fs, file_path, FS_FORMAT_TEXT, &buffer, &buffer_size) != FS_SUCCESS) {

    status = 1;

    goto error;
  }

  glslang_initialize_process();

  glslang_input_t stage_input = {
    .language = GLSLANG_SOURCE_GLSL,
    .stage = stage,
    .client = GLSLANG_CLIENT_VULKAN,
    .client_version = GLSLANG_TARGET_VULKAN_1_2,
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

  glslang_shader_t *shader = glslang_shader_create(&stage_input);

  if ((glslang_shader_preprocess(shader, &stage_input) == 0) ||
      (glslang_shader_parse(shader, &stage_input) == 0)) {

    printf("Compile Error %s\n", file_path);
    printf(glslang_shader_get_info_log(shader));
    printf("\n");

    status = 1;

    goto error;
  }

  glslang_program_t *program = glslang_program_create();

  glslang_program_add_shader(program, shader);

  if (glslang_program_link(program, GLSLANG_MSG_SPV_RULES_BIT) == 0) {

    printf("Link Error %s\n", file_path);
    printf(glslang_program_get_info_log(program));
    printf("\n");

    status = 1;

    goto error;
  }

  glslang_program_SPIRV_generate(program, stage);

  *word_count = glslang_program_SPIRV_get_size(program);
  *words = (uint32_t *)TI_ALLOC(sizeof(uint32_t) * (*word_count), 0, 0);

  memcpy(*words, glslang_program_SPIRV_get_ptr(program), sizeof(uint32_t) * (*word_count));

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

static uint8_t convert_spirv_input_variables(fs_file *file, SpvReflectShaderModule *module) {
  uint8_t status = 0;

  char variable_name[TI_PATH_SIZE] = {0};

  uint32_t input_variable_index = 0;
  uint32_t input_variable_count = 0;

  SpvReflectInterfaceVariable **input_variables = 0;

  if (spvReflectEnumerateInputVariables(module, &input_variable_count, 0) != SPV_REFLECT_RESULT_SUCCESS) {

    status = 1;

    goto error;
  }

  input_variables = (SpvReflectInterfaceVariable **)TI_ALLOC(sizeof(SpvReflectInterfaceVariable *) * input_variable_count, 0, 0);

  if (spvReflectEnumerateInputVariables(module, &input_variable_count, input_variables) != SPV_REFLECT_RESULT_SUCCESS) {

    status = 1;

    goto error;
  }

  fs_file_write(file, &input_variable_count, sizeof(uint32_t), 0);

  while (input_variable_index < input_variable_count) {

    SpvReflectInterfaceVariable *input_variable = input_variables[input_variable_index];

    if (input_variable->name) {
      snprintf(variable_name, TI_PATH_SIZE, "%s", input_variable->name);
    } else {
      snprintf(variable_name, TI_PATH_SIZE, "<unnamed>");
    }

    fs_file_write(file, variable_name, TI_PATH_SIZE, 0);
    fs_file_write(file, &input_variable->location, sizeof(uint32_t), 0);
    fs_file_write(file, &input_variable->format, sizeof(int32_t), 0);
    fs_file_write(file, &input_variable->built_in, sizeof(int32_t), 0);

    input_variable_index++;
  }

error:

  if (input_variables) {
    TI_FREE(input_variables);
  }

  return status;
}
static uint8_t convert_spirv_descriptor_bindings(fs_file *file, SpvReflectShaderModule *module) {
  uint8_t status = 0;

  char binding_name[TI_PATH_SIZE] = {0};
  char variable_name[TI_PATH_SIZE] = {0};

  uint32_t descriptor_binding_index = 0;
  uint32_t descriptor_binding_count = 0;

  SpvReflectDescriptorBinding **descriptor_bindings = 0;

  if (spvReflectEnumerateDescriptorBindings(module, &descriptor_binding_count, 0) != SPV_REFLECT_RESULT_SUCCESS) {

    status = 1;

    goto error;
  }

  descriptor_bindings = (SpvReflectDescriptorBinding **)TI_ALLOC(sizeof(SpvReflectDescriptorBinding *) * descriptor_binding_count, 0, 0);

  if (spvReflectEnumerateDescriptorBindings(module, &descriptor_binding_count, descriptor_bindings) != SPV_REFLECT_RESULT_SUCCESS) {

    status = 1;

    goto error;
  }

  fs_file_write(file, &descriptor_binding_count, sizeof(uint32_t), 0);

  while (descriptor_binding_index < descriptor_binding_count) {

    SpvReflectDescriptorBinding *descriptor_binding = descriptor_bindings[descriptor_binding_index];

    if (descriptor_binding->name) {
      snprintf(binding_name, TI_PATH_SIZE, "%s", descriptor_binding->name);
    } else {
      snprintf(binding_name, TI_PATH_SIZE, "<unnamed>");
    }

    fs_file_write(file, binding_name, TI_PATH_SIZE, 0);
    fs_file_write(file, &descriptor_binding->set, sizeof(uint32_t), 0);
    fs_file_write(file, &descriptor_binding->binding, sizeof(uint32_t), 0);
    fs_file_write(file, &descriptor_binding->descriptor_type, sizeof(int32_t), 0);

    if ((descriptor_binding->descriptor_type == SPV_REFLECT_DESCRIPTOR_TYPE_UNIFORM_BUFFER) ||
        (descriptor_binding->descriptor_type == SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_BUFFER)) {

      fs_file_write(file, &descriptor_binding->block.size, sizeof(uint32_t), 0);
      fs_file_write(file, &descriptor_binding->block.member_count, sizeof(uint32_t), 0);

      uint32_t block_variable_index = 0;
      uint32_t block_variable_count = descriptor_binding->block.member_count;

      while (block_variable_index < block_variable_count) {

        SpvReflectBlockVariable *block_variable = &descriptor_binding->block.members[block_variable_index];

        if (block_variable->name) {
          snprintf(variable_name, TI_PATH_SIZE, "%s", block_variable->name);
        } else {
          snprintf(variable_name, TI_PATH_SIZE, "<unnamed>");
        }

        fs_file_write(file, variable_name, TI_PATH_SIZE, 0);
        fs_file_write(file, &block_variable->offset, sizeof(uint32_t), 0);
        fs_file_write(file, &block_variable->size, sizeof(uint32_t), 0);

        block_variable_index++;
      }
    }

    descriptor_binding_index++;
  }

error:

  if (descriptor_bindings) {
    TI_FREE(descriptor_bindings);
  }

  return status;
}
