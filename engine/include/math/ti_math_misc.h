#ifndef TI_MATH_MISC_H
#define TI_MATH_MISC_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

__forceinline float deg_to_rad(float a);
__forceinline float rad_to_deg(float a);

__forceinline double clamp(double a, double min, double max);
__forceinline float clampf(float a, float min, float max);
__forceinline int32_t clampi(int32_t a, int32_t min, int32_t max);
__forceinline uint32_t clampu(uint32_t a, uint32_t min, uint32_t max);

__forceinline float signf(float a);

__forceinline int32_t floor_div32(float x);

#ifdef __cplusplus
}
#endif // __cplusplus

#include <math/ti_math_misc.inl>

#endif // TI_MATH_MISC_H
