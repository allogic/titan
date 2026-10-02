#ifndef TI_PH_RAY_H
#define TI_PH_RAY_H

#include <ti_engine.h>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

uint32_t ray_intersect_aabb(ray_t ray, aabb_t aabb, float *t_enter, float *t_exit);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // TI_PH_RAY_H
