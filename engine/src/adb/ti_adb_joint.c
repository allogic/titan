#include <ti_pch.h>

void adb_joint_create(adb_joint_t *joint) {
  memset(joint, 0, sizeof(adb_joint_t));
}
void adb_joint_load(adb_joint_t *joint, FILE *file) {
  adb_joint_create(joint);

  fread(joint->name, TI_PATH_SIZE, 1, file);
  fread(&joint->child_count, sizeof(uint64_t), 1, file);

  joint->children = (adb_joint_t *)TI_ALLOC(sizeof(adb_joint_t) * joint->child_count, 0, 0);

  uint64_t child_index = 0;
  uint64_t child_count = joint->child_count;

  while (child_index < child_count) {

    adb_joint_load(&joint->children[child_index], file);

    child_index++;
  }
}
void adb_joint_store(adb_joint_t *joint, FILE *file) {
  fwrite(joint->name, TI_PATH_SIZE, 1, file);
  fwrite(&joint->child_count, sizeof(uint64_t), 1, file);

  uint64_t child_index = 0;
  uint64_t child_count = joint->child_count;

  while (child_index < child_count) {

    adb_joint_store(&joint->children[child_index], file);

    child_index++;
  }
}
void adb_joint_destroy(adb_joint_t *joint) {
  uint64_t child_index = 0;
  uint64_t child_count = joint->child_count;

  while (child_index < child_count) {

    adb_joint_destroy(&joint->children[child_index]);

    child_index++;
  }

  TI_FREE(joint->children);

  memset(joint, 0, sizeof(adb_joint_t));
}
