#include <ti_pch.h>

void fs_input_variable_load(fs_input_variable_t *input_variable, fs_file *file) {
  memset(input_variable, 0, sizeof(fs_input_variable_t));

  fs_file_read(file, input_variable->name, TI_PATH_SIZE, 0);
  fs_file_read(file, &input_variable->is_built_in, sizeof(uint8_t), 0);
  fs_file_read(file, &input_variable->format_index, sizeof(uint64_t), 0);
}
void fs_input_variable_store(fs_input_variable_t *input_variable, fs_file *file) {
  fs_file_write(file, input_variable->name, TI_PATH_SIZE, 0);
  fs_file_write(file, &input_variable->is_built_in, sizeof(uint8_t), 0);
  fs_file_write(file, &input_variable->format_index, sizeof(uint64_t), 0);
}
void fs_input_variable_destroy(fs_input_variable_t *input_variable) {
  memset(input_variable, 0, sizeof(fs_input_variable_t));
}
