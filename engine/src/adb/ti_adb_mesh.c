#include <ti_pch.h>

void adb_mesh_load(adb_mesh_t *mesh, fs_file *file) {
  memset(mesh, 0, sizeof(adb_mesh_t));

  fs_file_read(file, mesh->name, TI_PATH_SIZE, 0);
  fs_file_read(file, &mesh->primitive_count, sizeof(uint64_t), 0);

  mesh->primitives = (adb_primitive_t *)TI_ALLOC(sizeof(adb_primitive_t) * mesh->primitive_count, 0, 0);

  uint64_t primitive_index = 0;
  uint64_t primitive_count = mesh->primitive_count;

  while (primitive_index < primitive_count) {

    adb_primitive_load(&mesh->primitives[primitive_index], file);

    primitive_index++;
  }
}
void adb_mesh_store(adb_mesh_t *mesh, fs_file *file) {
  fs_file_write(file, mesh->name, TI_PATH_SIZE, 0);
  fs_file_write(file, &mesh->primitive_count, sizeof(uint64_t), 0);

  uint64_t primitive_index = 0;
  uint64_t primitive_count = mesh->primitive_count;

  while (primitive_index < primitive_count) {

    adb_primitive_store(&mesh->primitives[primitive_index], file);

    primitive_index++;
  }
}
void adb_mesh_destroy(adb_mesh_t *mesh) {
  uint64_t primitive_index = 0;
  uint64_t primitive_count = mesh->primitive_count;

  while (primitive_index < primitive_count) {

    adb_primitive_destroy(&mesh->primitives[primitive_index]);

    primitive_index++;
  }

  TI_FREE(mesh->primitives);

  memset(mesh, 0, sizeof(adb_mesh_t));
}
