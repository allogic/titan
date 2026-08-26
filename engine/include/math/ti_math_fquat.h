#ifndef TI_MATH_FQUAT_H
#define TI_MATH_FQUAT_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

__forceinline fquat_t fquat_identity(void);
__forceinline fquat_t fquat_add(fquat_t a, fquat_t b);
__forceinline fquat_t fquat_adds(fquat_t a, float b);
__forceinline fquat_t fquat_mul(fquat_t a, fquat_t b);
__forceinline fquat_t fquat_muls(fquat_t a, float b);
__forceinline fquat_t fquat_conjugate(fquat_t a);
__forceinline fvec3_t fquat_right(fquat_t a);
__forceinline fvec3_t fquat_up(fquat_t a);
__forceinline fvec3_t fquat_front(fquat_t a);
__forceinline fvec3_t fquat_left(fquat_t a);
__forceinline fvec3_t fquat_down(fquat_t a);
__forceinline fvec3_t fquat_back(fquat_t a);
__forceinline fvec3_t fquat_to_euler_angles(fquat_t a);
__forceinline fvec3_t fquat_to_euler_angles_xyzw(float x, float y, float z, float w);
__forceinline fquat_t fquat_from_euler_angles(fvec3_t a);
__forceinline fquat_t fquat_from_euler_angles_pyr(float p, float y, float r);
__forceinline fquat_t fquat_angle_axis(float a, fvec3_t b);
__forceinline fquat_t fquat_norm(fquat_t a);
__forceinline float fquat_dot(fquat_t a, fquat_t b);
__forceinline float fquat_length(fquat_t a);
__forceinline float fquat_length2(fquat_t a);
__forceinline void fquat_print(fquat_t a);

#ifdef __cplusplus
}
#endif // __cplusplus

#include <math/ti_math_fquat.inl>

#endif // TI_MATH_FQUAT_H
