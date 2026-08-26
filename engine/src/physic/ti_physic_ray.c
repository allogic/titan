#include <ti_pch.h>

uint32_t ray_intersect_aabb(ray_t ray, aabb_t aabb, float *t_enter, float *t_exit) {
  float t_min = -FLT_MAX;
  float t_max = FLT_MAX;

  if (fabsf(ray.dir.x) < TI_EPSILON_6) {

    if (ray.orig.x < aabb.min.x || ray.orig.x > aabb.max.x) {
      return 0;
    }

  } else {

    float ray_direction_inv = 1.0F / ray.dir.x;
    float t1 = (aabb.min.x - ray.orig.x) * ray_direction_inv;
    float t2 = (aabb.max.x - ray.orig.x) * ray_direction_inv;

    if (t1 > t2) {
      float tmp = t1;
      t1 = t2;
      t2 = tmp;
    }

    if (t1 > t_min) {
      t_min = t1;
    }

    if (t2 < t_max) {
      t_max = t2;
    }

    if (t_min > t_max) {
      return 0;
    }
  }

  if (fabsf(ray.dir.y) < TI_EPSILON_6) {

    if (ray.orig.y < aabb.min.y || ray.orig.y > aabb.max.y) {
      return 0;
    }

  } else {

    float ray_direction_inv = 1.0F / ray.dir.y;
    float t1 = (aabb.min.y - ray.orig.y) * ray_direction_inv;
    float t2 = (aabb.max.y - ray.orig.y) * ray_direction_inv;

    if (t1 > t2) {
      float tmp = t1;
      t1 = t2;
      t2 = tmp;
    }

    if (t1 > t_min) {
      t_min = t1;
    }

    if (t2 < t_max) {
      t_max = t2;
    }

    if (t_min > t_max) {
      return 0;
    }
  }

  if (fabsf(ray.dir.z) < TI_EPSILON_6) {

    if (ray.orig.z < aabb.min.z || ray.orig.z > aabb.max.z) {
      return 0;
    }

  } else {

    float ray_direction_inv = 1.0F / ray.dir.z;
    float t1 = (aabb.min.z - ray.orig.z) * ray_direction_inv;
    float t2 = (aabb.max.z - ray.orig.z) * ray_direction_inv;

    if (t1 > t2) {
      float tmp = t1;
      t1 = t2;
      t2 = tmp;
    }

    if (t1 > t_min) {
      t_min = t1;
    }

    if (t2 < t_max) {
      t_max = t2;
    }

    if (t_min > t_max) {
      return 0;
    }
  }

  *t_enter = t_min;
  *t_exit = t_max;

  return 1;
}
