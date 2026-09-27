#ifndef TI_MATH_FMAT4X4_H
#define TI_MATH_FMAT4X4_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

__forceinline fmat4x4_t fmat4x4_identity(void);
__forceinline fmat4x4_t fmat4x4_inverse(fmat4x4_t a);
__forceinline fvec3_t fmat4x4_position(fmat4x4_t a);
__forceinline fquat_t fmat4x4_rotation(fmat4x4_t a);
__forceinline fvec3_t fmat4x4_euler_angles(fmat4x4_t a);
__forceinline fvec3_t fmat4x4_scale(fmat4x4_t a);
__forceinline fmat4x4_t fmat4x4_mul(fmat4x4_t a, fmat4x4_t b);
__forceinline fvec4_t fmat4x4_mul_fvec4(fmat4x4_t a, fvec4_t b);
__forceinline fmat4x4_t fmat4x4_ortho(float left, float right, float bottom, float top, float near_z, float far_z);
__forceinline fmat4x4_t fmat4x4_persp(float fov, float aspect_ratio, float near_z, float far_z);
__forceinline fmat4x4_t fmat4x4_look_at(fvec3_t eye, fvec3_t center, fvec3_t up);
__forceinline void fmat4x4_decompose(fmat4x4_t a, fvec3_t *p, fquat_t *r, fvec3_t *s);
__forceinline void fmat4x4_print(fmat4x4_t a);

#ifdef __cplusplus
}
#endif // __cplusplus

#include <math/ti_math_fmat4x4.inl>

#endif // TI_MATH_FMAT4X4_H
