#include <ti_pch.h>

void fs_model_load(fs_model_t *model, fs_file *file) {
  memset(model, 0, sizeof(fs_model_t));

  fs_file_read(file, model->name, TI_PATH_SIZE, 0);
  fs_file_read(file, &model->mesh_count, sizeof(uint64_t), 0);

  model->meshes = (fs_mesh_t *)TI_ALLOC(sizeof(fs_mesh_t) * model->mesh_count, 0, 0);

  uint64_t mesh_index = 0;
  uint64_t mesh_count = model->mesh_count;

  while (mesh_index < mesh_count) {

    fs_mesh_load(&model->meshes[mesh_index], file);

    mesh_index++;
  }

  fs_file_read(file, &model->skin_count, sizeof(uint64_t), 0);

  model->skins = (fs_skin_t *)TI_ALLOC(sizeof(fs_skin_t) * model->skin_count, 0, 0);

  uint64_t skin_index = 0;
  uint64_t skin_count = model->skin_count;

  while (skin_index < skin_count) {

    fs_skin_load(&model->skins[skin_index], file);

    skin_index++;
  }
}
void fs_model_store(fs_model_t *model, fs_file *file) {
  fs_file_write(file, model->name, TI_PATH_SIZE, 0);
  fs_file_write(file, &model->mesh_count, sizeof(uint64_t), 0);

  uint64_t mesh_index = 0;
  uint64_t mesh_count = model->mesh_count;

  while (mesh_index < mesh_count) {

    fs_mesh_store(&model->meshes[mesh_index], file);

    mesh_index++;
  }

  fs_file_write(file, &model->skin_count, sizeof(uint64_t), 0);

  uint64_t skin_index = 0;
  uint64_t skin_count = model->skin_count;

  while (skin_index < skin_count) {

    fs_skin_store(&model->skins[skin_index], file);

    skin_index++;
  }
}
void fs_model_destroy(fs_model_t *model) {
  uint64_t mesh_index = 0;
  uint64_t mesh_count = model->mesh_count;

  while (mesh_index < mesh_count) {

    fs_mesh_destroy(&model->meshes[mesh_index]);

    mesh_index++;
  }

  uint64_t skin_index = 0;
  uint64_t skin_count = model->skin_count;

  while (skin_index < skin_count) {

    fs_skin_destroy(&model->skins[skin_index]);

    skin_index++;
  }

  TI_FREE(model->meshes);
  TI_FREE(model->skins);

  memset(model, 0, sizeof(fs_model_t));
}
