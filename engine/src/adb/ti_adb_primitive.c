#include <ti_pch.h>

void adb_primitive_create(adb_primitive_t *primitive) {
  memset(primitive, 0, sizeof(adb_primitive_t));
}
void adb_primitive_load(adb_primitive_t *primitive, FILE *file) {
  fread(primitive->name, TI_PATH_SIZE, 1, file);

  fread(&primitive->position_count, sizeof(uint64_t), 1, file);
  fread(&primitive->normal_count, sizeof(uint64_t), 1, file);
  fread(&primitive->tangent_count, sizeof(uint64_t), 1, file);
  fread(&primitive->texcoord_count, sizeof(uint64_t), 1, file);
  fread(&primitive->color_count, sizeof(uint64_t), 1, file);
  fread(&primitive->joint_count, sizeof(uint64_t), 1, file);
  fread(&primitive->weight_count, sizeof(uint64_t), 1, file);

  fread(&primitive->position_stride, sizeof(uint64_t), 1, file);
  fread(&primitive->normal_stride, sizeof(uint64_t), 1, file);
  fread(&primitive->tangent_stride, sizeof(uint64_t), 1, file);
  fread(&primitive->texcoord_stride, sizeof(uint64_t), 1, file);
  fread(&primitive->color_stride, sizeof(uint64_t), 1, file);
  fread(&primitive->joint_stride, sizeof(uint64_t), 1, file);
  fread(&primitive->weight_stride, sizeof(uint64_t), 1, file);

  primitive->positions = TI_ALLOC(primitive->position_stride * primitive->position_count, 0, 0);
  primitive->normals = TI_ALLOC(primitive->normal_stride * primitive->normal_count, 0, 0);
  primitive->tangents = TI_ALLOC(primitive->tangent_stride * primitive->tangent_count, 0, 0);
  primitive->texcoords = TI_ALLOC(primitive->texcoord_stride * primitive->texcoord_count, 0, 0);
  primitive->colors = TI_ALLOC(primitive->color_stride * primitive->color_count, 0, 0);
  primitive->joints = TI_ALLOC(primitive->joint_stride * primitive->joint_count, 0, 0);
  primitive->weights = TI_ALLOC(primitive->weight_stride * primitive->weight_count, 0, 0);

  fread(primitive->positions, primitive->position_stride, primitive->position_count, file);
  fread(primitive->normals, primitive->normal_stride, primitive->normal_count, file);
  fread(primitive->tangents, primitive->tangent_stride, primitive->tangent_count, file);
  fread(primitive->texcoords, primitive->texcoord_stride, primitive->texcoord_count, file);
  fread(primitive->colors, primitive->color_stride, primitive->color_count, file);
  fread(primitive->joints, primitive->joint_stride, primitive->joint_count, file);
  fread(primitive->weights, primitive->weight_stride, primitive->weight_count, file);
}
void adb_primitive_store(adb_primitive_t *primitive, FILE *file) {
  fwrite(primitive->name, TI_PATH_SIZE, 1, file);

  fwrite(&primitive->position_count, sizeof(uint64_t), 1, file);
  fwrite(&primitive->normal_count, sizeof(uint64_t), 1, file);
  fwrite(&primitive->tangent_count, sizeof(uint64_t), 1, file);
  fwrite(&primitive->texcoord_count, sizeof(uint64_t), 1, file);
  fwrite(&primitive->color_count, sizeof(uint64_t), 1, file);
  fwrite(&primitive->joint_count, sizeof(uint64_t), 1, file);
  fwrite(&primitive->weight_count, sizeof(uint64_t), 1, file);

  fwrite(&primitive->position_stride, sizeof(uint64_t), 1, file);
  fwrite(&primitive->normal_stride, sizeof(uint64_t), 1, file);
  fwrite(&primitive->tangent_stride, sizeof(uint64_t), 1, file);
  fwrite(&primitive->texcoord_stride, sizeof(uint64_t), 1, file);
  fwrite(&primitive->color_stride, sizeof(uint64_t), 1, file);
  fwrite(&primitive->joint_stride, sizeof(uint64_t), 1, file);
  fwrite(&primitive->weight_stride, sizeof(uint64_t), 1, file);

  fwrite(primitive->positions, primitive->position_stride, primitive->position_count, file);
  fwrite(primitive->normals, primitive->normal_stride, primitive->normal_count, file);
  fwrite(primitive->tangents, primitive->tangent_stride, primitive->tangent_count, file);
  fwrite(primitive->texcoords, primitive->texcoord_stride, primitive->texcoord_count, file);
  fwrite(primitive->colors, primitive->color_stride, primitive->color_count, file);
  fwrite(primitive->joints, primitive->joint_stride, primitive->joint_count, file);
  fwrite(primitive->weights, primitive->weight_stride, primitive->weight_count, file);
}
void adb_primitive_destroy(adb_primitive_t *primitive) {
  if (primitive->positions) {
    TI_FREE(primitive->positions);
  }
  if (primitive->normals) {
    TI_FREE(primitive->normals);
  }
  if (primitive->tangents) {
    TI_FREE(primitive->tangents);
  }
  if (primitive->texcoords) {
    TI_FREE(primitive->texcoords);
  }
  if (primitive->colors) {
    TI_FREE(primitive->colors);
  }
  if (primitive->joints) {
    TI_FREE(primitive->joints);
  }
  if (primitive->weights) {
    TI_FREE(primitive->weights);
  }

  memset(primitive, 0, sizeof(adb_primitive_t));
}
