#ifndef TI_MATH_IVEC4_H
#define TI_MATH_IVEC4_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

__forceinline ivec4_t ivec4_negate(ivec4_t a);
__forceinline ivec4_t ivec4_add(ivec4_t a, ivec4_t b);
__forceinline ivec4_t ivec4_sub(ivec4_t a, ivec4_t b);
__forceinline ivec4_t ivec4_mul(ivec4_t a, ivec4_t b);
__forceinline ivec4_t ivec4_div(ivec4_t a, ivec4_t b);
__forceinline ivec4_t ivec4_adds(ivec4_t a, int32_t b);
__forceinline ivec4_t ivec4_subs(ivec4_t a, int32_t b);
__forceinline ivec4_t ivec4_muls(ivec4_t a, int32_t b);
__forceinline ivec4_t ivec4_divs(ivec4_t a, int32_t b);
__forceinline float ivec4_dot(ivec4_t a, ivec4_t b);
__forceinline float ivec4_length(ivec4_t a);
__forceinline float ivec4_length2(ivec4_t a);
__forceinline uint32_t ivec4_hash32(ivec4_t a);
__forceinline uint32_t iveczor4_equal(ivec4_t a, ivec4_t b);
__forceinline void ivec4_print(ivec4_t a);

#ifdef __cplusplus
}
#endif // __cplusplus

#include <math/ti_math_ivec4.inl>

#endif // TI_MATH_IVEC4_H
