#ifndef TI_CP_CONST_H
#define TI_CP_CONST_H

#define TI_CP_TRANSFORM_DECL \
  {                          \
    float position_x;        \
    float position_y;        \
    float position_z;        \
    float rotation_x;        \
    float rotation_y;        \
    float rotation_z;        \
    float rotation_w;        \
    float scale_x;           \
    float scale_y;           \
    float scale_z;           \
  }

#define TI_CP_CAMERA_DECL     \
  {                           \
    uint8_t is_debug_enabled; \
    float fov;                \
    float near_z;             \
    float far_z;              \
  }

#define TI_CP_MATERIAL_DECL \
  {                         \
    char pipeline[256];     \
    char material[256];     \
  }

#define TI_CP_MESH_DECL \
  {                     \
    char mesh[256];     \
  }

#define TI_CP_SKELETON_DECL \
  {                         \
    char skeleton[256];     \
  }

#define TI_CP_TRANSFORM_DESC TI_STRINGIFY(TI_CP_TRANSFORM_DECL)
#define TI_CP_CAMERA_DESC TI_STRINGIFY(TI_CP_CAMERA_DECL)
#define TI_CP_MATERIAL_DESC TI_STRINGIFY(TI_CP_MATERIAL_DECL)
#define TI_CP_MESH_DESC TI_STRINGIFY(TI_CP_MESH_DECL)
#define TI_CP_SKELETON_DESC TI_STRINGIFY(TI_CP_SKELETON_DECL)

#endif // TI_CP_CONST_H
