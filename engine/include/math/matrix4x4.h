#ifndef MATH_MATRIX4X4_H
#define MATH_MATRIX4X4_H

#include <fwd.h>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

__forceinline matrix4x4_t matrix4x4_zero(void);
__forceinline matrix4x4_t matrix4x4_identity(void);
__forceinline matrix4x4_t matrix4x4_inverse(matrix4x4_t a);
__forceinline vector3_t matrix4x4_position(matrix4x4_t a);
__forceinline quaternion_t matrix4x4_rotation(matrix4x4_t a);
__forceinline vector3_t matrix4x4_euler_angles(matrix4x4_t a);
__forceinline vector3_t matrix4x4_scale(matrix4x4_t a);
__forceinline matrix4x4_t matrix4x4_mul(matrix4x4_t a, matrix4x4_t b);
__forceinline vector4_t matrix4x4_mul_vector4(matrix4x4_t a, vector4_t b);
__forceinline matrix4x4_t matrix4x4_ortho(float left, float right, float bottom, float top, float near_z, float far_z);
__forceinline matrix4x4_t matrix4x4_persp(float fov, float aspect_ratio, float near_z, float far_z);
__forceinline matrix4x4_t matrix4x4_look_at(vector3_t eye, vector3_t center, vector3_t up);
__forceinline void matrix4x4_decompose(matrix4x4_t a, vector3_t *p, quaternion_t *r, vector3_t *s);
__forceinline void matrix4x4_print(matrix4x4_t a);

#ifdef __cplusplus
}
#endif // __cplusplus

#include <math/matrix4x4.inl>

#endif // MATH_MATRIX4X4_H
