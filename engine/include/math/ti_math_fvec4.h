#ifndef TI_MATH_FVEC4_H
#define TI_MATH_FVEC4_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

__forceinline fvec4_t fvec4_negate(fvec4_t a);
__forceinline fvec4_t fvec4_add(fvec4_t a, fvec4_t b);
__forceinline fvec4_t fvec4_sub(fvec4_t a, fvec4_t b);
__forceinline fvec4_t fvec4_mul(fvec4_t a, fvec4_t b);
__forceinline fvec4_t fvec4_div(fvec4_t a, fvec4_t b);
__forceinline fvec4_t fvec4_adds(fvec4_t a, float b);
__forceinline fvec4_t fvec4_subs(fvec4_t a, float b);
__forceinline fvec4_t fvec4_muls(fvec4_t a, float b);
__forceinline fvec4_t fvec4_divs(fvec4_t a, float b);
__forceinline fvec4_t fvec4_norm(fvec4_t a);
__forceinline fvec4_t fvec4_inv(fvec4_t a);
__forceinline fvec4_t fvec4_floor(fvec4_t a);
__forceinline fvec4_t fvec4_ceil(fvec4_t a);
__forceinline float fvec4_dot(fvec4_t a, fvec4_t b);
__forceinline float fvec4_length(fvec4_t a);
__forceinline float fvec4_length2(fvec4_t a);
__forceinline void fvec4_print(fvec4_t a);

#ifdef __cplusplus
}
#endif // __cplusplus

#include <math/ti_math_fvec4.inl>

#endif // TI_MATH_FVEC4_H
