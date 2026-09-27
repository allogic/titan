#ifndef TI_FS_DESCRIPTOR_BINDING_H
#define TI_FS_DESCRIPTOR_BINDING_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

void fs_descriptor_binding_load(fs_descriptor_binding_t *descriptor_binding, fs_file *file);
void fs_descriptor_binding_store(fs_descriptor_binding_t *descriptor_binding, fs_file *file);
void fs_descriptor_binding_destroy(fs_descriptor_binding_t *descriptor_binding);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // TI_FS_DESCRIPTOR_BINDING_H
