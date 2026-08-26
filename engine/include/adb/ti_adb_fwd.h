#ifndef TI_ADB_FWD_H
#define TI_ADB_FWD_H

typedef struct adb_primitive_t {
  char name[TI_PATH_SIZE];
  uint64_t position_count;
  uint64_t normal_count;
  uint64_t tangent_count;
  uint64_t texcoord_count;
  uint64_t color_count;
  uint64_t joint_count;
  uint64_t weight_count;
  uint64_t position_stride;
  uint64_t normal_stride;
  uint64_t tangent_stride;
  uint64_t texcoord_stride;
  uint64_t color_stride;
  uint64_t joint_stride;
  uint64_t weight_stride;
  void *positions;
  void *normals;
  void *tangents;
  void *texcoords;
  void *colors;
  void *joints;
  void *weights;
} adb_primitive_t;
typedef struct adb_mesh_t {
  char name[TI_PATH_SIZE];
  uint64_t primitive_count;
  adb_primitive_t *primitives;
} adb_mesh_t;
typedef struct adb_model_t {
  char name[TI_PATH_SIZE];
  uint64_t mesh_count;
  adb_mesh_t *meshes;
} adb_model_t;

typedef struct adb_joint_t {
  char name[TI_PATH_SIZE];
  uint64_t child_count;
  struct adb_joint_t *children;
} adb_joint_t;
typedef struct adb_skin_t {
  char name[TI_PATH_SIZE];
  adb_joint_t *root_joint;
} adb_skin_t;
typedef struct adb_skeleton_t {
  char name[TI_PATH_SIZE];
  uint64_t skin_count;
  adb_skin_t *skins;
} adb_skeleton_t;

#endif // TI_ADB_FWD_H
