#ifndef TI_PHYSIC_RAY_H
#define TI_PHYSIC_RAY_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

uint32_t ray_intersect_aabb(ray_t ray, aabb_t aabb, float *t_enter, float *t_exit);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // TI_PHYSIC_RAY_H
