#ifndef TI_FS_RENDERER_H
#define TI_FS_RENDERER_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

void fs_renderer_load(fs_renderer_t *renderer, fs_file *file);
void fs_renderer_store(fs_renderer_t *renderer, fs_file *file);
void fs_renderer_destroy(fs_renderer_t *renderer);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // TI_FS_RENDERER_H
