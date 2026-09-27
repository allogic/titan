#ifndef TI_PHYSIC_FWD_H
#define TI_PHYSIC_FWD_H

typedef struct ray_t {
  fvec3_t orig;
  fvec3_t dir;
} ray_t;
typedef struct aabb_t {
  fvec3_t min;
  fvec3_t max;
} aabb_t;
typedef struct plane_t {
  fvec3_t normal;
  float offset;
} plane_t;

#endif // TI_PHYSIC_FWD_H
