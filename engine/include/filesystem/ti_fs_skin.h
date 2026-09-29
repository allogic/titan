#ifndef TI_FS_SKIN_H
#define TI_FS_SKIN_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

void fs_skin_load(fs_skin_t *skin, fs_file *file);
void fs_skin_store(fs_skin_t *skin, fs_file *file);
void fs_skin_destroy(fs_skin_t *skin);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // TI_FS_SKIN_H
