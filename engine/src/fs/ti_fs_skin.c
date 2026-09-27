#include <ti_pch.h>

void fs_skin_load(fs_skin_t *skin, fs_file *file) {
  memset(skin, 0, sizeof(fs_skin_t));

  fs_file_read(file, skin->name, TI_PATH_SIZE, 0);

  skin->root_joint = (fs_joint_t *)TI_ALLOC(sizeof(fs_joint_t), 0, 0);

  fs_joint_load(skin->root_joint, file);
}
void fs_skin_store(fs_skin_t *skin, fs_file *file) {
  fs_file_write(file, skin->name, TI_PATH_SIZE, 0);

  fs_joint_store(skin->root_joint, file);
}
void fs_skin_destroy(fs_skin_t *skin) {
  fs_joint_destroy(skin->root_joint);

  TI_FREE(skin->root_joint);

  memset(skin, 0, sizeof(fs_skin_t));
}
