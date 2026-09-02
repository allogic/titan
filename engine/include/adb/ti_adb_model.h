#ifndef TI_ADB_MODEL_H
#define TI_ADB_MODEL_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

void adb_model_load(adb_model_t *model, fs_file *file);
void adb_model_store(adb_model_t *model, fs_file *file);
void adb_model_destroy(adb_model_t *model);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // TI_ADB_MODEL_H
