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

#define TI_CP_VELOCITY_DECL \
  {                         \
    float linear_x;         \
    float linear_y;         \
    float linear_z;         \
    float angular_x;        \
    float angular_y;        \
    float angular_z;        \
    float linear_drag;      \
    float angular_drag;     \
  }

#define TI_CP_EDITOR_CAMERA_CONTROLLER_DECL \
  {                                         \
    float keyboard_speed_fast;              \
    float keyboard_speed_normal;            \
    float mouse_rotation_speed;             \
    float mouse_begin_x;                    \
    float mouse_begin_y;                    \
    float mouse_delta_x;                    \
    float mouse_delta_y;                    \
  }

#define TI_CP_MATERIAL_DECL  \
  {                          \
    void *pipeline_instance; \
    void *material_instance; \
  }

#define TI_CP_MESH_DECL  \
  {                      \
    void *mesh_instance; \
  }

#define TI_CP_SKELETON_DECL  \
  {                          \
    void *skeleton_instance; \
  }

#define TI_CP_SCRIPT_DECL  \
  {                        \
    void *module_instance; \
  }

#define TI_CP_MATERIAL_REF_DECL \
  {                             \
    char pipeline_path[256];    \
    char material_path[256];    \
  }

#define TI_CP_MESH_REF_DECL \
  {                         \
    char mesh_path[256];    \
  }

#define TI_CP_SKELETON_REF_DECL \
  {                             \
    char skeleton_path[256];    \
  }

#define TI_CP_SCRIPT_REF_DECL \
  {                           \
    char module_path[256];    \
  }

#define TI_CP_TRANSFORM_DESC TI_STRINGIFY(TI_CP_TRANSFORM_DECL)
#define TI_CP_CAMERA_DESC TI_STRINGIFY(TI_CP_CAMERA_DECL)
#define TI_CP_VELOCITY_DESC TI_STRINGIFY(TI_CP_VELOCITY_DECL)
#define TI_CP_EDITOR_CAMERA_CONTROLLER_DESC TI_STRINGIFY(TI_CP_EDITOR_CAMERA_CONTROLLER_DECL)
#define TI_CP_MATERIAL_DESC TI_STRINGIFY(TI_CP_MATERIAL_DECL)
#define TI_CP_MESH_DESC TI_STRINGIFY(TI_CP_MESH_DECL)
#define TI_CP_SKELETON_DESC TI_STRINGIFY(TI_CP_SKELETON_DECL)
#define TI_CP_SCRIPT_DESC TI_STRINGIFY(TI_CP_SCRIPT_DECL)

#define TI_CP_MATERIAL_REF_DESC TI_STRINGIFY(TI_CP_MATERIAL_REF_DECL)
#define TI_CP_MESH_REF_DESC TI_STRINGIFY(TI_CP_MESH_REF_DECL)
#define TI_CP_SKELETON_REF_DESC TI_STRINGIFY(TI_CP_SKELETON_REF_DECL)
#define TI_CP_SCRIPT_REF_DESC TI_STRINGIFY(TI_CP_SCRIPT_REF_DECL)

#endif // TI_CP_CONST_H
