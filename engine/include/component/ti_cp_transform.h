#ifndef TI_CP_TRANSFORM_H
#define TI_CP_TRANSFORM_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

void cp_transform_init(cp_transform_t *transform);
// fmat4x4_t cp_transform_matrix(cp_transform_t *transform);

// void cp_transform_compute_world_position(cp_transform_t *transform);
// void cp_transform_compute_world_rotation(cp_transform_t *transform);
// void cp_transform_compute_world_scale(cp_transform_t *transform);

__forceinline fvec3_t cp_transform_local_right(cp_transform_t *transform);
__forceinline fvec3_t cp_transform_local_up(cp_transform_t *transform);
__forceinline fvec3_t cp_transform_local_front(cp_transform_t *transform);
__forceinline fvec3_t cp_transform_local_left(cp_transform_t *transform);
__forceinline fvec3_t cp_transform_local_down(cp_transform_t *transform);
__forceinline fvec3_t cp_transform_local_back(cp_transform_t *transform);

// __forceinline void cp_transform_set_position(cp_transform_t *transform, fvec3_t position);
// __forceinline void cp_transform_set_position_xyz(cp_transform_t *transform, float x, float y, float z);
// __forceinline void cp_transform_set_relative_position(cp_transform_t *transform, cp_transform_t *reference, fvec3_t position);
// __forceinline void cp_transform_set_relative_position_xyz(cp_transform_t *transform, cp_transform_t *reference, float x, float y, float z);

__forceinline void cp_transform_set_rotation(cp_transform_t *transform, fquat_t rotation);
__forceinline void cp_transform_set_rotation_xyzw(cp_transform_t *transform, float x, float y, float z, float w);
// __forceinline void cp_transform_set_relative_rotation(cp_transform_t *transform, cp_transform_t *reference, fquat_t rotation);
// __forceinline void cp_transform_set_relative_rotation_xyzw(cp_transform_t *transform, cp_transform_t *reference, float x, float y, float z, float w);

__forceinline void cp_transform_set_euler_angles(cp_transform_t *transform, fvec3_t euler_angles);
__forceinline void cp_transform_set_euler_angles_pyr(cp_transform_t *transform, float p, float y, float r);
// __forceinline void cp_transform_set_relative_euler_angles(cp_transform_t *transform, cp_transform_t *reference, fvec3_t euler_angles);
// __forceinline void cp_transform_set_relative_euler_angles_pyr(cp_transform_t *transform, cp_transform_t *reference, float p, float y, float r);

// __forceinline void cp_transform_set_scale(cp_transform_t *transform, fvec3_t scale);
// __forceinline void cp_transform_set_scale_xyz(cp_transform_t *transform, float x, float y, float z);
// __forceinline void cp_transform_set_relative_scale(cp_transform_t *transform, cp_transform_t *reference, fvec3_t scale);
// __forceinline void cp_transform_set_relative_scale_xyz(cp_transform_t *transform, cp_transform_t *reference, float x, float y, float z);

#ifdef __cplusplus
}
#endif // __cplusplus

#include <component/ti_cp_transform.inl>

#endif // TI_CP_TRANSFORM_H
