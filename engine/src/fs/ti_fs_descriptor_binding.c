#include <ti_pch.h>

void fs_descriptor_binding_load(fs_descriptor_binding_t *descriptor_binding, fs_file *file) {
  memset(descriptor_binding, 0, sizeof(fs_descriptor_binding_t));

  fs_file_read(file, descriptor_binding->name, TI_PATH_SIZE, 0);
  fs_file_read(file, &descriptor_binding->set, sizeof(uint32_t), 0);
  fs_file_read(file, &descriptor_binding->binding, sizeof(uint32_t), 0);
  fs_file_read(file, &descriptor_binding->descriptor_type_index, sizeof(uint64_t), 0);

  if ((g_vk_descriptor_type_table[descriptor_binding->descriptor_type_index].value == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER) ||
      (g_vk_descriptor_type_table[descriptor_binding->descriptor_type_index].value == VK_DESCRIPTOR_TYPE_STORAGE_BUFFER)) {

    fs_file_read(file, &descriptor_binding->block_size, sizeof(uint32_t), 0);
    fs_file_read(file, &descriptor_binding->block_variable_count, sizeof(uint32_t), 0);

    descriptor_binding->block_variables = (fs_block_variable_t *)TI_ALLOC(sizeof(fs_block_variable_t) * descriptor_binding->block_variable_count, 0, 0);

    uint64_t block_variable_index = 0;
    uint64_t block_variable_count = descriptor_binding->block_variable_count;

    while (block_variable_index < block_variable_count) {

      fs_block_variable_t *block_variable = &descriptor_binding->block_variables[block_variable_index];

      fs_file_read(file, block_variable->name, TI_PATH_SIZE, 0);
      fs_file_read(file, &block_variable->offset, sizeof(uint32_t), 0);
      fs_file_read(file, &block_variable->size, sizeof(uint32_t), 0);

      block_variable_index++;
    }
  }
}
void fs_descriptor_binding_store(fs_descriptor_binding_t *descriptor_binding, fs_file *file) {
  fs_file_write(file, descriptor_binding->name, TI_PATH_SIZE, 0);
  fs_file_write(file, &descriptor_binding->set, sizeof(uint32_t), 0);
  fs_file_write(file, &descriptor_binding->binding, sizeof(uint32_t), 0);
  fs_file_write(file, &descriptor_binding->descriptor_type_index, sizeof(uint64_t), 0);

  if ((g_vk_descriptor_type_table[descriptor_binding->descriptor_type_index].value == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER) ||
      (g_vk_descriptor_type_table[descriptor_binding->descriptor_type_index].value == VK_DESCRIPTOR_TYPE_STORAGE_BUFFER)) {

    fs_file_write(file, &descriptor_binding->block_size, sizeof(uint32_t), 0);
    fs_file_write(file, &descriptor_binding->block_variable_count, sizeof(uint32_t), 0);

    uint32_t block_variable_index = 0;
    uint32_t block_variable_count = descriptor_binding->block_variable_count;

    while (block_variable_index < block_variable_count) {

      fs_block_variable_t *block_variable = &descriptor_binding->block_variables[block_variable_index];

      fs_file_write(file, block_variable->name, TI_PATH_SIZE, 0);
      fs_file_write(file, &block_variable->offset, sizeof(uint32_t), 0);
      fs_file_write(file, &block_variable->size, sizeof(uint32_t), 0);

      block_variable_index++;
    }
  }
}
void fs_descriptor_binding_destroy(fs_descriptor_binding_t *descriptor_binding) {
  if (descriptor_binding->block_variables) {
    TI_FREE(descriptor_binding->block_variables);
  }

  memset(descriptor_binding, 0, sizeof(fs_descriptor_binding_t));
}
