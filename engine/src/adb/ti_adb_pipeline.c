#include <ti_pch.h>

static void read_input_variables(adb_pipeline_t *pipeline, fs_file *file);
static void read_descriptor_bindings(adb_pipeline_t *pipeline, fs_file *file);

void adb_pipeline_load(adb_pipeline_t *pipeline, fs_file *file) {
  memset(pipeline, 0, sizeof(adb_pipeline_t));

  fs_file_read(file, pipeline->name, TI_PATH_SIZE, 0);
  fs_file_read(file, &pipeline->pipeline_type, sizeof(pipeline_type_t), 0);

  switch (pipeline->pipeline_type) {

    case PIPELINE_TYPE_DFLT: {

      read_input_variables(pipeline, file);
      read_descriptor_bindings(pipeline, file);

      fs_file_read(file, &pipeline->spirv_vertex_word_count, sizeof(uint64_t), 0);
      pipeline->spirv_vertex_words = (uint32_t *)TI_ALLOC(sizeof(uint32_t) * pipeline->spirv_vertex_word_count, 0, 0);
      fs_file_read(file, pipeline->spirv_vertex_words, sizeof(uint32_t) * pipeline->spirv_vertex_word_count, 0);

      fs_file_read(file, &pipeline->spirv_fragment_word_count, sizeof(uint64_t), 0);
      pipeline->spirv_fragment_words = (uint32_t *)TI_ALLOC(sizeof(uint32_t) * pipeline->spirv_fragment_word_count, 0, 0);
      fs_file_read(file, pipeline->spirv_fragment_words, sizeof(uint32_t) * pipeline->spirv_fragment_word_count, 0);

      break;
    }
    case PIPELINE_TYPE_MESH: {

      // TODO

      break;
    }
    case PIPELINE_TYPE_RAY: {

      // TODO

      break;
    }
    case PIPELINE_TYPE_COMP: {

      // TODO

      break;
    }
  }
}
void adb_pipeline_store(adb_pipeline_t *pipeline, fs_file *file) {
  // TODO
}
void adb_pipeline_destroy(adb_pipeline_t *pipeline) {
  // TODO
}

static void read_input_variables(adb_pipeline_t *pipeline, fs_file *file) {
  fs_file_read(file, &pipeline->input_variable_count, sizeof(pipeline_type_t), 0);

  pipeline->input_variables = (adb_input_variable_t *)TI_ALLOC(sizeof(adb_input_variable_t) * pipeline->input_variable_count, 0, 0);

  uint64_t input_variable_index = 0;
  uint64_t input_variable_count = pipeline->input_variable_count;

  while (input_variable_index < input_variable_count) {

    adb_input_variable_t *input_variable = &pipeline->input_variables[input_variable_index];

    fs_file_read(file, input_variable->name, TI_PATH_SIZE, 0);
    fs_file_read(file, &input_variable->location, sizeof(int32_t), 0);
    fs_file_read(file, &input_variable->format, sizeof(int32_t), 0);
    fs_file_read(file, &input_variable->built_in, sizeof(int32_t), 0);

    input_variable_index++;
  }
}
static void read_descriptor_bindings(adb_pipeline_t *pipeline, fs_file *file) {
  fs_file_read(file, &pipeline->descriptor_binding_count, sizeof(pipeline_type_t), 0);

  pipeline->descriptor_bindings = (adb_descriptor_binding_t *)TI_ALLOC(sizeof(adb_descriptor_binding_t) * pipeline->descriptor_binding_count, 0, 0);

  uint64_t descriptor_binding_index = 0;
  uint64_t descriptor_binding_count = pipeline->descriptor_binding_count;

  while (descriptor_binding_index < descriptor_binding_count) {

    adb_descriptor_binding_t *descriptor_binding = &pipeline->descriptor_bindings[descriptor_binding_index];

    fs_file_read(file, descriptor_binding->name, TI_PATH_SIZE, 0);
    fs_file_read(file, &descriptor_binding->set, sizeof(uint32_t), 0);
    fs_file_read(file, &descriptor_binding->binding, sizeof(uint32_t), 0);
    fs_file_read(file, &descriptor_binding->descriptor_type, sizeof(int32_t), 0);

    if ((descriptor_binding->descriptor_type == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER) ||
        (descriptor_binding->descriptor_type == VK_DESCRIPTOR_TYPE_STORAGE_BUFFER)) {

      fs_file_read(file, &descriptor_binding->block_size, sizeof(uint32_t), 0);
      fs_file_read(file, &descriptor_binding->block_variable_count, sizeof(uint32_t), 0);

      descriptor_binding->block_variables = (adb_block_variable_t *)TI_ALLOC(sizeof(adb_block_variable_t) * descriptor_binding->block_variable_count, 0, 0);

      uint64_t block_variable_index = 0;
      uint64_t block_variable_count = descriptor_binding->block_variable_count;

      while (block_variable_index < block_variable_count) {

        adb_block_variable_t *block_variable = &descriptor_binding->block_variables[block_variable_index];

        fs_file_read(file, block_variable->name, TI_PATH_SIZE, 0);
        fs_file_read(file, &block_variable->offset, sizeof(uint32_t), 0);
        fs_file_read(file, &block_variable->size, sizeof(uint32_t), 0);

        block_variable_index++;
      }
    }

    descriptor_binding_index++;
  }
}
