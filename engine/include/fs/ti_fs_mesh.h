#ifndef TI_FS_MESH_H
#define TI_FS_MESH_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

void fs_mesh_load(fs_mesh_t *mesh, fs_file *file);
void fs_mesh_store(fs_mesh_t *mesh, fs_file *file);
void fs_mesh_destroy(fs_mesh_t *mesh);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // TI_FS_MESH_H
