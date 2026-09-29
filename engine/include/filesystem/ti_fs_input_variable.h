#ifndef TI_FS_INPUT_VARIABLE_H
#define TI_FS_INPUT_VARIABLE_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

void fs_input_variable_load(fs_input_variable_t *input_variable, fs_file *file);
void fs_input_variable_store(fs_input_variable_t *input_variable, fs_file *file);
void fs_input_variable_destroy(fs_input_variable_t *input_variable);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // TI_FS_INPUT_VARIABLE_H
