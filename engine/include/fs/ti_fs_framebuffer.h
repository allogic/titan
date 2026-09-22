#ifndef TI_FS_FRAMEBUFFER_H
#define TI_FS_FRAMEBUFFER_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

void fs_framebuffer_load(fs_framebuffer_t *framebuffer, fs_file *file);
void fs_framebuffer_store(fs_framebuffer_t *framebuffer, fs_file *file);
void fs_framebuffer_destroy(fs_framebuffer_t *framebuffer);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // TI_FS_FRAMEBUFFER_H
