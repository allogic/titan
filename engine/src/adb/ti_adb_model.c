#include <ti_pch.h>

void adb_model_create(adb_model_t *model) {
  memset(model, 0, sizeof(adb_model_t));
}
void adb_model_load(adb_model_t *model, FILE *file) {
  adb_model_create(model);

  fread(model->name, TI_PATH_SIZE, 1, file);
  fread(&model->mesh_count, sizeof(uint64_t), 1, file);

  model->meshes = (adb_mesh_t *)TI_ALLOC(sizeof(adb_mesh_t) * model->mesh_count, 0, 0);

  uint64_t mesh_index = 0;
  uint64_t mesh_count = model->mesh_count;

  while (mesh_index < mesh_count) {

    adb_mesh_load(&model->meshes[mesh_index], file);

    mesh_index++;
  }
}
void adb_model_store(adb_model_t *model, FILE *file) {
  fwrite(model->name, TI_PATH_SIZE, 1, file);
  fwrite(&model->mesh_count, sizeof(uint64_t), 1, file);

  uint64_t mesh_index = 0;
  uint64_t mesh_count = model->mesh_count;

  while (mesh_index < mesh_count) {

    adb_mesh_store(&model->meshes[mesh_index], file);

    mesh_index++;
  }
}
void adb_model_destroy(adb_model_t *model) {
  uint64_t mesh_index = 0;
  uint64_t mesh_count = model->mesh_count;

  while (mesh_index < mesh_count) {

    adb_mesh_destroy(&model->meshes[mesh_index]);

    mesh_index++;
  }

  TI_FREE(model->meshes);

  memset(model, 0, sizeof(adb_model_t));
}
