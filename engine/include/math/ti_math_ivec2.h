#ifndef TI_MATH_IVEC2_H
#define TI_MATH_IVEC2_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

__forceinline ivec2_t ivec2_right(void);
__forceinline ivec2_t ivec2_up(void);
__forceinline ivec2_t ivec2_left(void);
__forceinline ivec2_t ivec2_down(void);
__forceinline ivec2_t ivec2_negate(ivec2_t a);
__forceinline ivec2_t ivec2_add(ivec2_t a, ivec2_t b);
__forceinline ivec2_t ivec2_sub(ivec2_t a, ivec2_t b);
__forceinline ivec2_t ivec2_mul(ivec2_t a, ivec2_t b);
__forceinline ivec2_t ivec2_div(ivec2_t a, ivec2_t b);
__forceinline ivec2_t ivec2_adds(ivec2_t a, int32_t b);
__forceinline ivec2_t ivec2_subs(ivec2_t a, int32_t b);
__forceinline ivec2_t ivec2_muls(ivec2_t a, int32_t b);
__forceinline ivec2_t ivec2_divs(ivec2_t a, int32_t b);
__forceinline float ivec2_dot(ivec2_t a, ivec2_t b);
__forceinline float ivec2_length(ivec2_t a);
__forceinline float ivec2_length2(ivec2_t a);
__forceinline uint32_t ivec2_hash32(ivec2_t a);
__forceinline uint32_t ivec2_equal(ivec2_t a, ivec2_t b);
__forceinline void ivec2_print(ivec2_t a);

#ifdef __cplusplus
}
#endif // __cplusplus

#include <math/ti_math_ivec2.inl>

#endif // TI_MATH_IVEC2_H
