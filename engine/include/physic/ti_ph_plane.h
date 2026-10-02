#ifndef TI_PH_PLANE_H
#define TI_PH_PLANE_H

#include <ti_engine.h>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

uint32_t plane_intersect_plane3(plane_t p1, plane_t p2, plane_t p3, fvec3_t *intersection_point);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // TI_PH_PLANE_H
