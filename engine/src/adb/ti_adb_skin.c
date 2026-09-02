#include <ti_pch.h>

void adb_skin_load(adb_skin_t *skin, fs_file *file) {
  memset(skin, 0, sizeof(adb_skin_t));

  fs_file_read(file, skin->name, TI_PATH_SIZE, 0);

  skin->root_joint = (adb_joint_t *)TI_ALLOC(sizeof(adb_joint_t), 0, 0);

  adb_joint_load(skin->root_joint, file);
}
void adb_skin_store(adb_skin_t *skin, fs_file *file) {
  fs_file_write(file, skin->name, TI_PATH_SIZE, 0);

  adb_joint_store(skin->root_joint, file);
}
void adb_skin_destroy(adb_skin_t *skin) {
  adb_joint_destroy(skin->root_joint);

  TI_FREE(skin->root_joint);

  memset(skin, 0, sizeof(adb_skin_t));
}
