#ifndef TI_FS_MODEL_H
#define TI_FS_MODEL_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

void fs_model_load(fs_model_t *model, fs_file *file);
void fs_model_store(fs_model_t *model, fs_file *file);
void fs_model_destroy(fs_model_t *model);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // TI_FS_MODEL_H
