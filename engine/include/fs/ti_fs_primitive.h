#ifndef TI_FS_PRIMITIVE_H
#define TI_FS_PRIMITIVE_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

void fs_primitive_load(fs_primitive_t *primitive, fs_file *file);
void fs_primitive_store(fs_primitive_t *primitive, fs_file *file);
void fs_primitive_destroy(fs_primitive_t *primitive);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // TI_FS_PRIMITIVE_H
