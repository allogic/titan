#ifndef TI_MATH_FVEC3_H
#define TI_MATH_FVEC3_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

__forceinline fvec3_t fvec3_right(void);
__forceinline fvec3_t fvec3_up(void);
__forceinline fvec3_t fvec3_front(void);
__forceinline fvec3_t fvec3_left(void);
__forceinline fvec3_t fvec3_down(void);
__forceinline fvec3_t fvec3_back(void);
__forceinline fvec3_t fvec3_negate(fvec3_t a);
__forceinline fvec3_t fvec3_add(fvec3_t a, fvec3_t b);
__forceinline fvec3_t fvec3_sub(fvec3_t a, fvec3_t b);
__forceinline fvec3_t fvec3_mul(fvec3_t a, fvec3_t b);
__forceinline fvec3_t fvec3_div(fvec3_t a, fvec3_t b);
__forceinline fvec3_t fvec3_adds(fvec3_t a, float b);
__forceinline fvec3_t fvec3_subs(fvec3_t a, float b);
__forceinline fvec3_t fvec3_muls(fvec3_t a, float b);
__forceinline fvec3_t fvec3_divs(fvec3_t a, float b);
__forceinline fvec3_t fvec3_norm(fvec3_t a);
__forceinline fvec3_t fvec3_inv(fvec3_t a);
__forceinline fvec3_t fvec3_floor(fvec3_t a);
__forceinline fvec3_t fvec3_ceil(fvec3_t a);
__forceinline fvec3_t fvec3_cross(fvec3_t a, fvec3_t b);
__forceinline fvec3_t fvec3_rotate(fvec3_t a, fquat_t b);
__forceinline float fvec3_dot(fvec3_t a, fvec3_t b);
__forceinline float fvec3_length(fvec3_t a);
__forceinline float fvec3_length2(fvec3_t a);
__forceinline void fvec3_print(fvec3_t a);

#ifdef __cplusplus
}
#endif // __cplusplus

#include <math/ti_math_fvec3.inl>

#endif // TI_MATH_FVEC3_H
