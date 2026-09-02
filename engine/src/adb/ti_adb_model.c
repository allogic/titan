#include <ti_pch.h>

void adb_model_load(adb_model_t *model, fs_file *file) {
  memset(model, 0, sizeof(adb_model_t));

  fs_file_read(file, model->name, TI_PATH_SIZE, 0);
  fs_file_read(file, &model->mesh_count, sizeof(uint64_t), 0);

  model->meshes = (adb_mesh_t *)TI_ALLOC(sizeof(adb_mesh_t) * model->mesh_count, 0, 0);

  uint64_t mesh_index = 0;
  uint64_t mesh_count = model->mesh_count;

  while (mesh_index < mesh_count) {

    adb_mesh_load(&model->meshes[mesh_index], file);

    mesh_index++;
  }

  fs_file_read(file, &model->skin_count, sizeof(uint64_t), 0);

  model->skins = (adb_skin_t *)TI_ALLOC(sizeof(adb_skin_t) * model->skin_count, 0, 0);

  uint64_t skin_index = 0;
  uint64_t skin_count = model->skin_count;

  while (skin_index < skin_count) {

    adb_skin_load(&model->skins[skin_index], file);

    skin_index++;
  }
}
void adb_model_store(adb_model_t *model, fs_file *file) {
  fs_file_write(file, model->name, TI_PATH_SIZE, 0);
  fs_file_write(file, &model->mesh_count, sizeof(uint64_t), 0);

  uint64_t mesh_index = 0;
  uint64_t mesh_count = model->mesh_count;

  while (mesh_index < mesh_count) {

    adb_mesh_store(&model->meshes[mesh_index], file);

    mesh_index++;
  }

  fs_file_write(file, &model->skin_count, sizeof(uint64_t), 0);

  uint64_t skin_index = 0;
  uint64_t skin_count = model->skin_count;

  while (skin_index < skin_count) {

    adb_skin_store(&model->skins[skin_index], file);

    skin_index++;
  }
}
void adb_model_destroy(adb_model_t *model) {
  uint64_t mesh_index = 0;
  uint64_t mesh_count = model->mesh_count;

  while (mesh_index < mesh_count) {

    adb_mesh_destroy(&model->meshes[mesh_index]);

    mesh_index++;
  }

  uint64_t skin_index = 0;
  uint64_t skin_count = model->skin_count;

  while (skin_index < skin_count) {

    adb_skin_destroy(&model->skins[skin_index]);

    skin_index++;
  }

  TI_FREE(model->meshes);
  TI_FREE(model->skins);

  memset(model, 0, sizeof(adb_model_t));
}
