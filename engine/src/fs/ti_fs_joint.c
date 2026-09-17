#include <ti_pch.h>

void fs_joint_load(fs_joint_t *joint, fs_file *file) {
  memset(joint, 0, sizeof(fs_joint_t));

  fs_file_read(file, joint->name, TI_PATH_SIZE, 0);
  fs_file_read(file, &joint->child_count, sizeof(uint64_t), 0);

  joint->children = (fs_joint_t *)TI_ALLOC(sizeof(fs_joint_t) * joint->child_count, 0, 0);

  uint64_t child_index = 0;
  uint64_t child_count = joint->child_count;

  while (child_index < child_count) {

    fs_joint_load(&joint->children[child_index], file);

    child_index++;
  }
}
void fs_joint_store(fs_joint_t *joint, fs_file *file) {
  fs_file_write(file, joint->name, TI_PATH_SIZE, 0);
  fs_file_write(file, &joint->child_count, sizeof(uint64_t), 0);

  uint64_t child_index = 0;
  uint64_t child_count = joint->child_count;

  while (child_index < child_count) {

    fs_joint_store(&joint->children[child_index], file);

    child_index++;
  }
}
void fs_joint_destroy(fs_joint_t *joint) {
  uint64_t child_index = 0;
  uint64_t child_count = joint->child_count;

  while (child_index < child_count) {

    fs_joint_destroy(&joint->children[child_index]);

    child_index++;
  }

  TI_FREE(joint->children);

  memset(joint, 0, sizeof(fs_joint_t));
}
