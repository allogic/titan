#ifndef TI_ADB_MESH_H
#define TI_ADB_MESH_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

void adb_mesh_load(adb_mesh_t *mesh, fs_file *file);
void adb_mesh_store(adb_mesh_t *mesh, fs_file *file);
void adb_mesh_destroy(adb_mesh_t *mesh);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // TI_ADB_MESH_H
