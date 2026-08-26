#include <ti_pch.h>

void adb_skin_create(adb_skin_t *skin) {
  memset(skin, 0, sizeof(adb_skin_t));

  skin->root_joint = (adb_joint_t *)TI_ALLOC(sizeof(adb_joint_t), 0, 0);

  adb_joint_create(skin->root_joint);
}
void adb_skin_load(adb_skin_t *skin, FILE *file) {
  fread(skin->name, TI_PATH_SIZE, 1, file);

  skin->root_joint = (adb_joint_t *)TI_ALLOC(sizeof(adb_joint_t), 0, 0);

  adb_joint_load(skin->root_joint, file);
}
void adb_skin_store(adb_skin_t *skin, FILE *file) {
  fwrite(skin->name, TI_PATH_SIZE, 1, file);

  adb_joint_store(skin->root_joint, file);
}
void adb_skin_destroy(adb_skin_t *skin) {
  adb_joint_destroy(skin->root_joint);

  TI_FREE(skin->root_joint);

  memset(skin, 0, sizeof(adb_skin_t));
}
