#include <ti_pch.h>

void fs_primitive_load(fs_primitive_t *primitive, fs_file *file) {
  memset(primitive, 0, sizeof(fs_primitive_t));

  fs_file_read(file, primitive->name, TI_PATH_SIZE, 0);

  fs_file_read(file, &primitive->position_count, sizeof(uint64_t), 0);
  fs_file_read(file, &primitive->position_stride, sizeof(uint64_t), 0);
  primitive->positions = TI_ALLOC(primitive->position_stride * primitive->position_count, 0, 0);
  fs_file_read(file, primitive->positions, primitive->position_stride * primitive->position_count, 0);

  fs_file_read(file, &primitive->normal_count, sizeof(uint64_t), 0);
  fs_file_read(file, &primitive->normal_stride, sizeof(uint64_t), 0);
  primitive->normals = TI_ALLOC(primitive->normal_stride * primitive->normal_count, 0, 0);
  fs_file_read(file, primitive->normals, primitive->normal_stride * primitive->normal_count, 0);

  fs_file_read(file, &primitive->tangent_count, sizeof(uint64_t), 0);
  fs_file_read(file, &primitive->tangent_stride, sizeof(uint64_t), 0);
  primitive->tangents = TI_ALLOC(primitive->tangent_stride * primitive->tangent_count, 0, 0);
  fs_file_read(file, primitive->tangents, primitive->tangent_stride * primitive->tangent_count, 0);

  fs_file_read(file, &primitive->texcoord_count, sizeof(uint64_t), 0);
  fs_file_read(file, &primitive->texcoord_stride, sizeof(uint64_t), 0);
  primitive->texcoords = TI_ALLOC(primitive->texcoord_stride * primitive->texcoord_count, 0, 0);
  fs_file_read(file, primitive->texcoords, primitive->texcoord_stride * primitive->texcoord_count, 0);

  fs_file_read(file, &primitive->color_count, sizeof(uint64_t), 0);
  fs_file_read(file, &primitive->color_stride, sizeof(uint64_t), 0);
  primitive->colors = TI_ALLOC(primitive->color_stride * primitive->color_count, 0, 0);
  fs_file_read(file, primitive->colors, primitive->color_stride * primitive->color_count, 0);

  fs_file_read(file, &primitive->joint_count, sizeof(uint64_t), 0);
  fs_file_read(file, &primitive->joint_stride, sizeof(uint64_t), 0);
  primitive->joints = TI_ALLOC(primitive->joint_stride * primitive->joint_count, 0, 0);
  fs_file_read(file, primitive->joints, primitive->joint_stride * primitive->joint_count, 0);

  fs_file_read(file, &primitive->weight_count, sizeof(uint64_t), 0);
  fs_file_read(file, &primitive->weight_stride, sizeof(uint64_t), 0);
  primitive->weights = TI_ALLOC(primitive->weight_stride * primitive->weight_count, 0, 0);
  fs_file_read(file, primitive->weights, primitive->weight_stride * primitive->weight_count, 0);
}
void fs_primitive_store(fs_primitive_t *primitive, fs_file *file) {
  fs_file_write(file, primitive->name, TI_PATH_SIZE, 0);

  fs_file_write(file, &primitive->position_count, sizeof(uint64_t), 0);
  fs_file_write(file, &primitive->position_stride, sizeof(uint64_t), 0);
  fs_file_write(file, primitive->positions, primitive->position_stride * primitive->position_count, 0);

  fs_file_write(file, &primitive->normal_count, sizeof(uint64_t), 0);
  fs_file_write(file, &primitive->normal_stride, sizeof(uint64_t), 0);
  fs_file_write(file, primitive->normals, primitive->normal_stride * primitive->normal_count, 0);

  fs_file_write(file, &primitive->tangent_count, sizeof(uint64_t), 0);
  fs_file_write(file, &primitive->tangent_stride, sizeof(uint64_t), 0);
  fs_file_write(file, primitive->tangents, primitive->tangent_stride * primitive->tangent_count, 0);

  fs_file_write(file, &primitive->texcoord_count, sizeof(uint64_t), 0);
  fs_file_write(file, &primitive->texcoord_stride, sizeof(uint64_t), 0);
  fs_file_write(file, primitive->texcoords, primitive->texcoord_stride * primitive->texcoord_count, 0);

  fs_file_write(file, &primitive->color_count, sizeof(uint64_t), 0);
  fs_file_write(file, &primitive->color_stride, sizeof(uint64_t), 0);
  fs_file_write(file, primitive->colors, primitive->color_stride * primitive->color_count, 0);

  fs_file_write(file, &primitive->joint_count, sizeof(uint64_t), 0);
  fs_file_write(file, &primitive->joint_stride, sizeof(uint64_t), 0);
  fs_file_write(file, primitive->joints, primitive->joint_stride * primitive->joint_count, 0);

  fs_file_write(file, &primitive->weight_count, sizeof(uint64_t), 0);
  fs_file_write(file, &primitive->weight_stride, sizeof(uint64_t), 0);
  fs_file_write(file, primitive->weights, primitive->weight_stride * primitive->weight_count, 0);
}
void fs_primitive_destroy(fs_primitive_t *primitive) {
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

  memset(primitive, 0, sizeof(fs_primitive_t));
}
