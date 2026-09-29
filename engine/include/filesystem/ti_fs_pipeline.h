#ifndef TI_FS_PIPELINE_H
#define TI_FS_PIPELINE_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

void fs_pipeline_load(fs_pipeline_t *pipeline, fs_file *file);
void fs_pipeline_store(fs_pipeline_t *pipeline, fs_file *file);
void fs_pipeline_destroy(fs_pipeline_t *pipeline);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // TI_FS_PIPELINE_H
