#ifndef TI_COMP_TRANSFORM_H
#define TI_COMP_TRANSFORM_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

void transform_init(transform_t *transform);
// fmat4x4_t transform_matrix(transform_t *transform);

// void transform_compute_world_position(transform_t *transform);
// void transform_compute_world_rotation(transform_t *transform);
// void transform_compute_world_scale(transform_t *transform);

__forceinline fvec3_t transform_local_right(transform_t const *transform);
__forceinline fvec3_t transform_local_up(transform_t const *transform);
__forceinline fvec3_t transform_local_front(transform_t const *transform);
__forceinline fvec3_t transform_local_left(transform_t const *transform);
__forceinline fvec3_t transform_local_down(transform_t const *transform);
__forceinline fvec3_t transform_local_back(transform_t const *transform);

// __forceinline void transform_set_position(transform_t *transform, fvec3_t position);
// __forceinline void transform_set_position_xyz(transform_t *transform, float x, float y, float z);
// __forceinline void transform_set_relative_position(transform_t *transform, transform_t *reference, fvec3_t position);
// __forceinline void transform_set_relative_position_xyz(transform_t *transform, transform_t *reference, float x, float y, float z);

__forceinline void transform_set_rotation(transform_t *transform, fquat_t rotation);
__forceinline void transform_set_rotation_xyzw(transform_t *transform, float x, float y, float z, float w);
// __forceinline void transform_set_relative_rotation(transform_t *transform, transform_t *reference, fquat_t rotation);
// __forceinline void transform_set_relative_rotation_xyzw(transform_t *transform, transform_t *reference, float x, float y, float z, float w);

__forceinline void transform_set_euler_angles(transform_t *transform, fvec3_t euler_angles);
__forceinline void transform_set_euler_angles_pyr(transform_t *transform, float p, float y, float r);
// __forceinline void transform_set_relative_euler_angles(transform_t *transform, transform_t *reference, fvec3_t euler_angles);
// __forceinline void transform_set_relative_euler_angles_pyr(transform_t *transform, transform_t *reference, float p, float y, float r);

// __forceinline void transform_set_scale(transform_t *transform, fvec3_t scale);
// __forceinline void transform_set_scale_xyz(transform_t *transform, float x, float y, float z);
// __forceinline void transform_set_relative_scale(transform_t *transform, transform_t *reference, fvec3_t scale);
// __forceinline void transform_set_relative_scale_xyz(transform_t *transform, transform_t *reference, float x, float y, float z);

#ifdef __cplusplus
}
#endif // __cplusplus

#include <comp/ti_comp_transform.inl>

#endif // TI_COMP_TRANSFORM_H
