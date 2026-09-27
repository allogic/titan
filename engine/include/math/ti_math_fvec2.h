#ifndef TI_MATH_FVEC2_H
#define TI_MATH_FVEC2_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

__forceinline fvec2_t fvec2_right(void);
__forceinline fvec2_t fvec2_up(void);
__forceinline fvec2_t fvec2_left(void);
__forceinline fvec2_t fvec2_down(void);
__forceinline fvec2_t fvec2_negate(fvec2_t a);
__forceinline fvec2_t fvec2_add(fvec2_t a, fvec2_t b);
__forceinline fvec2_t fvec2_sub(fvec2_t a, fvec2_t b);
__forceinline fvec2_t fvec2_mul(fvec2_t a, fvec2_t b);
__forceinline fvec2_t fvec2_div(fvec2_t a, fvec2_t b);
__forceinline fvec2_t fvec2_adds(fvec2_t a, float b);
__forceinline fvec2_t fvec2_subs(fvec2_t a, float b);
__forceinline fvec2_t fvec2_muls(fvec2_t a, float b);
__forceinline fvec2_t fvec2_divs(fvec2_t a, float b);
__forceinline fvec2_t fvec2_norm(fvec2_t a);
__forceinline fvec2_t fvec2_inv(fvec2_t a);
__forceinline fvec2_t fvec2_floor(fvec2_t a);
__forceinline fvec2_t fvec2_ceil(fvec2_t a);
__forceinline float fvec2_dot(fvec2_t a, fvec2_t b);
__forceinline float fvec2_length(fvec2_t a);
__forceinline float fvec2_length2(fvec2_t a);
__forceinline void fvec2_print(fvec2_t a);

#ifdef __cplusplus
}
#endif // __cplusplus

#include <math/ti_math_fvec2.inl>

#endif // TI_MATH_FVEC2_H
