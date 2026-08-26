#include <ti_pch.h>

#define CGLTF_IMPLEMENTATION
#include <cgltf.h>

#include <glslang/Include/glslang_c_interface.h>
#include <glslang/Public/resource_limits_c.h>

static cgltf_accessor *adb_find_accessor(cgltf_primitive *gltf_primitive, cgltf_attribute_type attribute_type);
static cgltf_node *adb_find_root_node(cgltf_skin *gltf_skin);

static void adb_read_float_accessor(fs_file *file, cgltf_accessor *gltf_accessor);
static void adb_read_uint_accessor(fs_file *file, cgltf_accessor *gltf_accessor);

static void adb_convert_gltf_model(fs_file *file, cgltf_data *gltf_data);
static void adb_convert_gltf_mesh(fs_file *file, cgltf_mesh *gltf_mesh, uint64_t mesh_index);
static void adb_convert_gltf_primitive(fs_file *file, cgltf_primitive *gltf_primitive, uint64_t primitive_index);

static void adb_convert_skeleton(fs_file *file, cgltf_data *gltf_data);
static void adb_convert_skin(fs_file *file, cgltf_skin *gltf_skin, uint64_t skin_index);
static void adb_convert_joint(fs_file *file, cgltf_node *gltf_node);

static uint8_t adb_compile_shader(char const *file_path, glslang_stage_t stage, uint32_t **word, uint64_t *word_count);

/*
void adb_load(adb_t *adb) {
  FILE *file = fopen(adb->file_path, "rb");

  if (file) {

    uint64_t magic = 0;
    uint64_t version = 0;
    uint64_t asset_index = 0;
    uint64_t asset_count = 0;

    fread(&magic, sizeof(uint64_t), 1, file);

    if (magic == TI_ADB_MAGIC) {

      fread(&version, sizeof(uint64_t), 1, file);
      fread(&asset_count, sizeof(uint64_t), 1, file);
      fread(adb->file_name, TI_PATH_SIZE, 1, file);
      fread(adb->file_path, TI_PATH_SIZE, 1, file);

      while (asset_index < asset_count) {

        adb_load_asset(file, &adb->assets);

        asset_index++;
      }
    }

    fclose(file);
  }
}
void adb_store(adb_t *adb) {
  FILE *file = fopen(adb->file_path, "wb");

  if (file) {

    uint64_t magic = TI_ADB_MAGIC;
    uint64_t version = 1;
    uint64_t asset_count = trie_count(&adb->assets);

    fwrite(&magic, sizeof(uint64_t), 1, file);
    fwrite(&version, sizeof(uint64_t), 1, file);
    fwrite(&asset_count, sizeof(uint64_t), 1, file);
    fwrite(adb->file_name, TI_PATH_SIZE, 1, file);
    fwrite(adb->file_path, TI_PATH_SIZE, 1, file);

    trie_store(&adb->assets, adb_store_asset, file);

    fclose(file);
  }
}
*/

uint8_t adb_import_model(char const *virtual_path, char const *model_file) {
  static char file_path[TI_PATH_SIZE] = {0};

  cgltf_options gltf_options = {0};
  cgltf_data *gltf_data = 0;

  LARGE_INTEGER freq = {0};
  LARGE_INTEGER t0 = {0};
  LARGE_INTEGER t1 = {0};
  LARGE_INTEGER t2 = {0};
  LARGE_INTEGER t3 = {0};
  LARGE_INTEGER t4 = {0};
  LARGE_INTEGER t5 = {0};
  LARGE_INTEGER t6 = {0};
  LARGE_INTEGER t7 = {0};

  QueryPerformanceFrequency(&freq);
  QueryPerformanceCounter(&t0);

  if (cgltf_parse_file(&gltf_options, model_file, &gltf_data) != cgltf_result_success) {

    return 1;
  }

  QueryPerformanceCounter(&t1);

  if (cgltf_load_buffers(&gltf_options, gltf_data, model_file) != cgltf_result_success) {

    cgltf_free(gltf_data);

    return 1;
  }

  QueryPerformanceCounter(&t2);

  if (cgltf_validate(gltf_data) != cgltf_result_success) {

    cgltf_free(gltf_data);

    return 1;
  }

  QueryPerformanceCounter(&t3);

  if (fs_mkdir_recursive(g_fs, virtual_path, FS_READ | FS_WRITE) != FS_SUCCESS) {

    cgltf_free(gltf_data);

    return 1;
  }

  QueryPerformanceCounter(&t4);

  {
    snprintf(file_path, TI_PATH_SIZE, "%s/%s", virtual_path, "model.bin");

    fs_file *file = 0;

    if (fs_file_open(g_fs, file_path, FS_WRITE, &file) == FS_SUCCESS) {

      adb_convert_gltf_model(file, gltf_data);

      fs_file_close(file);
    }
  }

  QueryPerformanceCounter(&t5);

  {
    snprintf(file_path, TI_PATH_SIZE, "%s/%s", virtual_path, "skeleton.bin");

    fs_file *file = 0;

    if (fs_file_open(g_fs, file_path, FS_WRITE, &file) == FS_SUCCESS) {

      adb_convert_skeleton(file, gltf_data);

      fs_file_close(file);
    }
  }

  QueryPerformanceCounter(&t6);

  cgltf_free(gltf_data);

  QueryPerformanceCounter(&t7);

  LONGLONG d0 = ((t1.QuadPart - t0.QuadPart) * 1000) / freq.QuadPart;
  LONGLONG d1 = ((t2.QuadPart - t1.QuadPart) * 1000) / freq.QuadPart;
  LONGLONG d2 = ((t3.QuadPart - t2.QuadPart) * 1000) / freq.QuadPart;
  LONGLONG d3 = ((t4.QuadPart - t3.QuadPart) * 1000) / freq.QuadPart;
  LONGLONG d4 = ((t5.QuadPart - t4.QuadPart) * 1000) / freq.QuadPart;
  LONGLONG d5 = ((t6.QuadPart - t5.QuadPart) * 1000) / freq.QuadPart;
  LONGLONG d6 = ((t7.QuadPart - t6.QuadPart) * 1000) / freq.QuadPart;
  LONGLONG dt = d0 + d1 + d2 + d3 + d4 + d5 + d6;

  printf("Importing %s\n", model_file);
  printf("  Parse file         %8lld ms\n", d0);
  printf("  Load buffer        %8lld ms\n", d1);
  printf("  Validate           %8lld ms\n", d2);
  printf("  Create directory   %8lld ms\n", d3);
  printf("  Convert model      %8lld ms\n", d4);
  printf("  Convert skeleton   %8lld ms\n", d5);
  printf("  Cleanup            %8lld ms\n", d6);
  printf("  Total              %8llu ms\n", dt);
  printf("\n");

  return 0;
}
uint8_t adb_import_dflt_shader(char const *virtual_path, char const *vertex_file, char const *fragment_file) {
  static char file_path[TI_PATH_SIZE] = {0};

  uint32_t *vertex_word = 0;
  uint32_t *fragment_word = 0;

  uint64_t vertex_word_count = 0;
  uint64_t fragment_word_count = 0;

  LARGE_INTEGER freq = {0};
  LARGE_INTEGER t0 = {0};
  LARGE_INTEGER t1 = {0};
  LARGE_INTEGER t2 = {0};
  LARGE_INTEGER t3 = {0};
  LARGE_INTEGER t4 = {0};
  LARGE_INTEGER t5 = {0};
  LARGE_INTEGER t6 = {0};

  QueryPerformanceFrequency(&freq);
  QueryPerformanceCounter(&t0);

  if (adb_compile_shader(vertex_file, GLSLANG_STAGE_VERTEX, &vertex_word, &vertex_word_count)) {

    return 1;
  }

  QueryPerformanceCounter(&t1);

  if (adb_compile_shader(fragment_file, GLSLANG_STAGE_FRAGMENT, &fragment_word, &fragment_word_count)) {

    TI_FREE(vertex_word);

    return 1;
  }

  QueryPerformanceCounter(&t2);

  if (fs_mkdir_recursive(g_fs, virtual_path, FS_READ | FS_WRITE) != FS_SUCCESS) {

    TI_FREE(vertex_word);
    TI_FREE(fragment_word);

    return 1;
  }

  QueryPerformanceCounter(&t3);

  {
    snprintf(file_path, TI_PATH_SIZE, "%s/%s", virtual_path, "vertex.spv");

    fs_file *file = 0;

    if (fs_file_open(g_fs, file_path, FS_WRITE, &file) == FS_SUCCESS) {

      fs_file_write(file, vertex_word, sizeof(uint32_t) * vertex_word_count, 0);
      fs_file_close(file);
    }
  }

  QueryPerformanceCounter(&t4);

  {
    snprintf(file_path, TI_PATH_SIZE, "%s/%s", virtual_path, "fragment.spv");

    fs_file *file = 0;

    if (fs_file_open(g_fs, file_path, FS_WRITE, &file) == FS_SUCCESS) {

      fs_file_write(file, fragment_word, sizeof(uint32_t) * fragment_word_count, 0);
      fs_file_close(file);
    }
  }

  QueryPerformanceCounter(&t5);

  TI_FREE(vertex_word);
  TI_FREE(fragment_word);

  QueryPerformanceCounter(&t6);

  LONGLONG d0 = ((t1.QuadPart - t0.QuadPart) * 1000) / freq.QuadPart;
  LONGLONG d1 = ((t2.QuadPart - t1.QuadPart) * 1000) / freq.QuadPart;
  LONGLONG d2 = ((t3.QuadPart - t2.QuadPart) * 1000) / freq.QuadPart;
  LONGLONG d3 = ((t4.QuadPart - t3.QuadPart) * 1000) / freq.QuadPart;
  LONGLONG d4 = ((t5.QuadPart - t4.QuadPart) * 1000) / freq.QuadPart;
  LONGLONG d5 = ((t6.QuadPart - t5.QuadPart) * 1000) / freq.QuadPart;
  LONGLONG dt = d0 + d1 + d2 + d3 + d4 + d5;

  printf("Importing %s %s\n", vertex_file, fragment_file);
  printf("  Compile vertex shader         %8lld ms\n", d0);
  printf("  Compile fragment shader       %8lld ms\n", d1);
  printf("  Create directory              %8lld ms\n", d2);
  printf("  Convert vertex shader         %8lld ms\n", d3);
  printf("  Convert fragment shader       %8lld ms\n", d4);
  printf("  Cleanup                       %8lld ms\n", d5);
  printf("  Total                         %8llu ms\n", dt);
  printf("\n");

  return 0;
}

// static void adb_load_asset(FILE *file, trie_t *assets) {
// static char name[TI_PATH_SIZE] = {0};
// static char physical_path[TI_PATH_SIZE] = {0};
// static char virtual_path[TI_PATH_SIZE] = {0};
//
// adb_node_type_t node_type = ADB_NODE_TYPE_NONE;
// adb_asset_type_t asset_type = ADB_ASSET_TYPE_NONE;
//
// fread(name, TI_PATH_SIZE, 1, file);
// fread(physical_path, TI_PATH_SIZE, 1, file);
// fread(virtual_path, TI_PATH_SIZE, 1, file);
// fread(&node_type, sizeof(adb_node_type_t), 1, file);
// fread(&asset_type, sizeof(adb_asset_type_t), 1, file);
//
// adb_asset_t *asset = (adb_asset_t *)trie_at(assets, virtual_path);
//
// strcpy(asset->name, name);
// strcpy(asset->physical_path, physical_path);
// strcpy(asset->virtual_path, virtual_path);
//
// asset->node_type = node_type;
// asset->asset_type = asset_type;
//
// switch (asset_type) {
//   case ADB_ASSET_TYPE_MODEL: {
//
//     asset->value = (adb_model_t *)TI_ALLOC(sizeof(adb_model_t), 0, 0);
//
//     adb_model_load(asset->value, file);
//
//     break;
//   }
//   case ADB_ASSET_TYPE_SKELETON: {
//
//     asset->value = (adb_skeleton_t *)TI_ALLOC(sizeof(adb_skeleton_t), 0, 0);
//
//     adb_skeleton_load(asset->value, file);
//
//     break;
//   }
// }
// }
// static void adb_store_asset(FILE *file, trie_node_t *node) {
// adb_asset_t *asset = node->value;
//
// fwrite(asset->name, TI_PATH_SIZE, 1, file);
// fwrite(asset->physical_path, TI_PATH_SIZE, 1, file);
// fwrite(asset->virtual_path, TI_PATH_SIZE, 1, file);
// fwrite(&asset->node_type, sizeof(adb_node_type_t), 1, file);
// fwrite(&asset->asset_type, sizeof(adb_asset_type_t), 1, file);
//
// switch (asset->asset_type) {
//   case ADB_ASSET_TYPE_MODEL: {
//
//     adb_model_store(asset->value, file);
//
//     break;
//   }
//   case ADB_ASSET_TYPE_SKELETON: {
//
//     adb_skeleton_store(asset->value, file);
//
//     break;
//   }
// }
// }

static cgltf_accessor *adb_find_accessor(cgltf_primitive *gltf_primitive, cgltf_attribute_type attribute_type) {
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
static cgltf_node *adb_find_root_node(cgltf_skin *gltf_skin) {
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

static void adb_read_float_accessor(fs_file *file, cgltf_accessor *gltf_accessor) {
  if (gltf_accessor == 0) {
    return;
  }

  uint64_t component_count = cgltf_num_components(gltf_accessor->type);

  uint64_t value_index = 0;
  uint64_t value_count = gltf_accessor->count;
  uint64_t value_stride = sizeof(float) * component_count;

  fs_file_write(file, &value_count, sizeof(uint64_t), 0);
  fs_file_write(file, &value_stride, sizeof(uint64_t), 0);

  float *float_buffer = (float *)TI_ALLOC(value_stride, 0, 0);

  while (value_index < value_count) {

    cgltf_accessor_read_float(gltf_accessor, value_index, float_buffer, component_count);

    fs_file_write(file, float_buffer, value_stride, 0);

    value_index++;
  }

  TI_FREE(float_buffer);
}
static void adb_read_uint_accessor(fs_file *file, cgltf_accessor *gltf_accessor) {
  if (gltf_accessor == 0) {
    return;
  }

  uint64_t component_count = cgltf_num_components(gltf_accessor->type);

  uint64_t value_index = 0;
  uint64_t value_count = gltf_accessor->count;
  uint64_t value_stride = sizeof(uint32_t) * component_count;

  uint32_t *int_buffer = (uint32_t *)TI_ALLOC(value_stride, 0, 0);

  while (value_index < value_count) {

    cgltf_accessor_read_uint(gltf_accessor, value_index, int_buffer, component_count);

    fs_file_write(file, int_buffer, value_stride, 0);

    value_index++;
  }

  TI_FREE(int_buffer);
}

static void adb_convert_gltf_model(fs_file *file, cgltf_data *gltf_data) {
  static char model_name[TI_PATH_SIZE] = {0};

  snprintf(model_name, TI_PATH_SIZE, "root");

  uint64_t mesh_index = 0;
  uint64_t mesh_count = gltf_data->meshes_count;

  fs_file_write(file, model_name, TI_PATH_SIZE, 0);
  fs_file_write(file, &mesh_count, sizeof(uint64_t), 0);

  while (mesh_index < mesh_count) {

    adb_convert_gltf_mesh(file, &gltf_data->meshes[mesh_index], mesh_index);

    mesh_index++;
  }
}
static void adb_convert_gltf_mesh(fs_file *file, cgltf_mesh *gltf_mesh, uint64_t mesh_index) {
  static char mesh_name[TI_PATH_SIZE] = {0};

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

    adb_convert_gltf_primitive(file, &gltf_mesh->primitives[primitive_index], primitive_index);

    primitive_index++;
  }
}
static void adb_convert_gltf_primitive(fs_file *file, cgltf_primitive *gltf_primitive, uint64_t primitive_index) {
  static char primitive_name[TI_PATH_SIZE] = {0};

  snprintf(primitive_name, TI_PATH_SIZE, "primitive %llu", primitive_index);

  fs_file_write(file, primitive_name, TI_PATH_SIZE, 0);

  adb_read_float_accessor(file, adb_find_accessor(gltf_primitive, cgltf_attribute_type_position));
  adb_read_float_accessor(file, adb_find_accessor(gltf_primitive, cgltf_attribute_type_normal));
  adb_read_float_accessor(file, adb_find_accessor(gltf_primitive, cgltf_attribute_type_tangent));
  adb_read_float_accessor(file, adb_find_accessor(gltf_primitive, cgltf_attribute_type_texcoord));
  adb_read_float_accessor(file, adb_find_accessor(gltf_primitive, cgltf_attribute_type_color));
  adb_read_uint_accessor(file, adb_find_accessor(gltf_primitive, cgltf_attribute_type_joints));
  adb_read_float_accessor(file, adb_find_accessor(gltf_primitive, cgltf_attribute_type_weights));
}

static void adb_convert_skeleton(fs_file *file, cgltf_data *gltf_data) {
  static char skeleton_name[TI_PATH_SIZE] = {0};

  snprintf(skeleton_name, TI_PATH_SIZE, "root");

  uint64_t skin_index = 0;
  uint64_t skin_count = gltf_data->skins_count;

  fs_file_write(file, skeleton_name, TI_PATH_SIZE, 0);
  fs_file_write(file, &skin_count, sizeof(uint64_t), 0);

  while (skin_index < skin_count) {

    adb_convert_skin(file, &gltf_data->skins[skin_index], skin_index);

    skin_index++;
  }
}
static void adb_convert_skin(fs_file *file, cgltf_skin *gltf_skin, uint64_t skin_index) {
  static char skin_name[TI_PATH_SIZE] = {0};

  if (gltf_skin->name) {
    strcpy(skin_name, gltf_skin->name);
  } else {
    snprintf(skin_name, TI_PATH_SIZE, "skin %llu", skin_index);
  }

  fs_file_write(file, skin_name, TI_PATH_SIZE, 0);

  adb_convert_joint(file, adb_find_root_node(gltf_skin));
}
static void adb_convert_joint(fs_file *file, cgltf_node *gltf_node) {
  static char joint_name[TI_PATH_SIZE] = {0};

  strcpy(joint_name, gltf_node->name);

  uint64_t child_index = 0;
  uint64_t child_count = gltf_node->children_count;

  fs_file_write(file, joint_name, TI_PATH_SIZE, 0);
  fs_file_write(file, &child_count, sizeof(uint64_t), 0);

  while (child_index < child_count) {

    adb_convert_joint(file, gltf_node->children[child_index]);

    child_index++;
  }
}

static uint8_t adb_compile_shader(char const *file_path, glslang_stage_t stage, uint32_t **word, uint64_t *word_count) {
  fs_file *file = 0;

  if (fs_file_open(g_fs, file_path, FS_READ, &file) != FS_SUCCESS) {

    return 1;
  }

  uint8_t *code = 0;
  uint64_t code_size = 0;

  if (fs_file_read_to_end(file, FS_FORMAT_TEXT, &code, &code_size) != FS_SUCCESS) {

    fs_file_close(file);

    return 1;
  }

  glslang_initialize_process();

  glslang_input_t stage_input = {
    .language = GLSLANG_SOURCE_GLSL,
    .stage = stage,
    .client = GLSLANG_CLIENT_VULKAN,
    .client_version = GLSLANG_TARGET_VULKAN_1_2,
    .target_language = GLSLANG_TARGET_SPV,
    .target_language_version = GLSLANG_TARGET_SPV_1_6,
    .code = code,
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

    glslang_shader_delete(shader);
    glslang_finalize_process();

    fs_free(code, 0);
    fs_file_close(file);

    return 1;
  }

  glslang_program_t *program = glslang_program_create();

  glslang_program_add_shader(program, shader);

  if (glslang_program_link(program, GLSLANG_MSG_SPV_RULES_BIT) == 0) {

    printf("Link Error %s\n", file_path);
    printf(glslang_program_get_info_log(program));
    printf("\n");

    glslang_program_delete(program);
    glslang_shader_delete(shader);
    glslang_finalize_process();

    fs_free(code, 0);
    fs_file_close(file);

    return 1;
  }

  glslang_program_SPIRV_generate(program, stage);

  *word_count = glslang_program_SPIRV_get_size(program);
  *word = (uint32_t *)TI_ALLOC(sizeof(uint32_t) * (*word_count), 0, 0);

  memcpy(*word, glslang_program_SPIRV_get_ptr(program), sizeof(uint32_t) * (*word_count));

  glslang_program_delete(program);
  glslang_shader_delete(shader);
  glslang_finalize_process();

  fs_free(code, 0);
  fs_file_close(file);

  return 0;
}
