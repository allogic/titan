#ifndef TI_COMP_FWD_H
#define TI_COMP_FWD_H

typedef enum frustum_plane_t {
  FRUSTUM_PLANE_LEFT = 0,
  FRUSTUM_PLANE_RIGHT,
  FRUSTUM_PLANE_BOTTOM,
  FRUSTUM_PLANE_TOP,
  FRUSTUM_PLANE_NEAR,
  FRUSTUM_PLANE_FAR,
  FRUSTUM_PLANE_COUNT,
} frustum_plane_t;
typedef enum frustum_corner_t {
  FRUSTUM_CORNER_NEAR_V0 = 0,
  FRUSTUM_CORNER_NEAR_V1,
  FRUSTUM_CORNER_NEAR_V2,
  FRUSTUM_CORNER_NEAR_V3,
  FRUSTUM_CORNER_FAR_V0,
  FRUSTUM_CORNER_FAR_V1,
  FRUSTUM_CORNER_FAR_V2,
  FRUSTUM_CORNER_FAR_V3,
  FRUSTUM_CORNER_COUNT,
} frustum_corner_t;

typedef enum comp_type_t {
  COMP_TYPE_TRANSFORM = 0,
  COMP_TYPE_CAMERA,
  COMP_TYPE_MATERIAL,
  COMP_TYPE_MESH,
  COMP_TYPE_SKELETON,
  COMP_TYPE_COUNT,
} comp_type_t;

ECS_STRUCT_TYPE(transform_t, TI_TRANSFORM_DECL);
ECS_STRUCT_TYPE(camera_t, TI_CAMERA_DECL);
ECS_STRUCT_TYPE(material_t, TI_MATERIAL_DECL);
ECS_STRUCT_TYPE(mesh_t, TI_MESH_DECL);
ECS_STRUCT_TYPE(skeleton_t, TI_SKELETON_DECL);

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

extern ECS_COMPONENT_DECLARE(transform_t);
extern ECS_COMPONENT_DECLARE(camera_t);
extern ECS_COMPONENT_DECLARE(material_t);
extern ECS_COMPONENT_DECLARE(mesh_t);
extern ECS_COMPONENT_DECLARE(skeleton_t);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // TI_COMP_FWD_H
