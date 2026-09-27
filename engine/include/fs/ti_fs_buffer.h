#ifndef TI_FS_BUFFER_H
#define TI_FS_BUFFER_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

void fs_buffer_load(fs_buffer_t *buffer, fs_file *file);
void fs_buffer_store(fs_buffer_t *buffer, fs_file *file);
void fs_buffer_destroy(fs_buffer_t *buffer);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // TI_FS_BUFFER_H
