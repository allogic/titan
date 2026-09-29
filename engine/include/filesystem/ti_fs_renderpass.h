#ifndef TI_FS_RENDERPASS_H
#define TI_FS_RENDERPASS_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

void fs_renderpass_load(fs_renderpass_t *renderpass, fs_file *file);
void fs_renderpass_store(fs_renderpass_t *renderpass, fs_file *file);
void fs_renderpass_destroy(fs_renderpass_t *renderpass);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // TI_FS_RENDERPASS_H
