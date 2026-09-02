#ifndef TI_ADB_PIPELINE_H
#define TI_ADB_PIPELINE_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

void adb_pipeline_load(adb_pipeline_t *pipeline, fs_file *file);
void adb_pipeline_store(adb_pipeline_t *pipeline, fs_file *file);
void adb_pipeline_destroy(adb_pipeline_t *pipeline);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // TI_ADB_PIPELINE_H
