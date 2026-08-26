#ifndef TI_COMP_CONST_H
#define TI_COMP_CONST_H

#define TI_TRANSFORM_DECL \
  {                       \
    float position_x;     \
    float position_y;     \
    float position_z;     \
    float rotation_x;     \
    float rotation_y;     \
    float rotation_z;     \
    float rotation_w;     \
    float scale_x;        \
    float scale_y;        \
    float scale_z;        \
  }

#define TI_CAMERA_DECL        \
  {                           \
    uint8_t is_debug_enabled; \
    float fov;                \
    float near_z;             \
    float far_z;              \
  }

#define TI_MATERIAL_DECL   \
  {                        \
    uint64_t shader_ref;   \
    uint64_t material_ref; \
  }

#define TI_MESH_DECL   \
  {                    \
    uint64_t mesh_ref; \
  }

#define TI_SKELETON_DECL   \
  {                        \
    uint64_t skeleton_ref; \
  }

#define TI_TRANSFORM_DESC TI_STRINGIFY(TI_TRANSFORM_DECL)
#define TI_CAMERA_DESC TI_STRINGIFY(TI_CAMERA_DECL)
#define TI_MATERIAL_DESC TI_STRINGIFY(TI_MATERIAL_DECL)
#define TI_MESH_DESC TI_STRINGIFY(TI_MESH_DECL)
#define TI_SKELETON_DESC TI_STRINGIFY(TI_SKELETON_DECL)

#endif // TI_COMP_CONST_H
