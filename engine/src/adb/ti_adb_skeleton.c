#include <ti_pch.h>

void adb_skeleton_create(adb_skeleton_t *skeleton) {
  memset(skeleton, 0, sizeof(adb_skeleton_t));
}
void adb_skeleton_load(adb_skeleton_t *skeleton, FILE *file) {
  adb_skeleton_create(skeleton);

  fread(skeleton->name, TI_PATH_SIZE, 1, file);
  fread(&skeleton->skin_count, sizeof(uint64_t), 1, file);

  skeleton->skins = (adb_skin_t *)TI_ALLOC(sizeof(adb_skin_t) * skeleton->skin_count, 0, 0);

  uint64_t skin_index = 0;
  uint64_t skin_count = skeleton->skin_count;

  while (skin_index < skin_count) {

    adb_skin_load(&skeleton->skins[skin_index], file);

    skin_index++;
  }
}
void adb_skeleton_store(adb_skeleton_t *skeleton, FILE *file) {
  fwrite(skeleton->name, TI_PATH_SIZE, 1, file);
  fwrite(&skeleton->skin_count, sizeof(uint64_t), 1, file);

  uint64_t skin_index = 0;
  uint64_t skin_count = skeleton->skin_count;

  while (skin_index < skin_count) {

    adb_skin_store(&skeleton->skins[skin_index], file);

    skin_index++;
  }
}
void adb_skeleton_destroy(adb_skeleton_t *skeleton) {
  uint64_t skin_index = 0;
  uint64_t skin_count = skeleton->skin_count;

  while (skin_index < skin_count) {

    adb_skin_destroy(&skeleton->skins[skin_index]);

    skin_index++;
  }

  TI_FREE(skeleton->skins);

  memset(skeleton, 0, sizeof(adb_skeleton_t));
}
