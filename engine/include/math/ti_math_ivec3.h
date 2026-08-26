#ifndef TI_MATH_IVEC3_H
#define TI_MATH_IVEC3_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

__forceinline ivec3_t ivec3_right(void);
__forceinline ivec3_t ivec3_up(void);
__forceinline ivec3_t ivec3_front(void);
__forceinline ivec3_t ivec3_left(void);
__forceinline ivec3_t ivec3_down(void);
__forceinline ivec3_t ivec3_back(void);
__forceinline ivec3_t ivec3_negate(ivec3_t a);
__forceinline ivec3_t ivec3_add(ivec3_t a, ivec3_t b);
__forceinline ivec3_t ivec3_sub(ivec3_t a, ivec3_t b);
__forceinline ivec3_t ivec3_mul(ivec3_t a, ivec3_t b);
__forceinline ivec3_t ivec3_div(ivec3_t a, ivec3_t b);
__forceinline ivec3_t ivec3_adds(ivec3_t a, int32_t b);
__forceinline ivec3_t ivec3_subs(ivec3_t a, int32_t b);
__forceinline ivec3_t ivec3_muls(ivec3_t a, int32_t b);
__forceinline ivec3_t ivec3_divs(ivec3_t a, int32_t b);
__forceinline float ivec3_dot(ivec3_t a, ivec3_t b);
__forceinline float ivec3_length(ivec3_t a);
__forceinline float ivec3_length2(ivec3_t a);
__forceinline uint32_t ivec3_hash32(ivec3_t a);
__forceinline uint32_t ivec3_equal(ivec3_t a, ivec3_t b);
__forceinline void ivec3_print(ivec3_t a);

#ifdef __cplusplus
}
#endif // __cplusplus

#include <math/ti_math_ivec3.inl>

#endif // TI_MATH_IVEC3_H
