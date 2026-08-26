#include <ti_pch.h>

uint32_t plane_intersect_plane3(plane_t p1, plane_t p2, plane_t p3, fvec3_t *intersection_point) {
  fvec3_t n1 = {p1.normal.x, p1.normal.y, p1.normal.z};
  fvec3_t n2 = {p2.normal.x, p2.normal.y, p2.normal.z};
  fvec3_t n3 = {p3.normal.x, p3.normal.y, p3.normal.z};

  fvec3_t n2xn3 = fvec3_cross(n2, n3);
  fvec3_t n3xn1 = fvec3_cross(n3, n1);
  fvec3_t n1xn2 = fvec3_cross(n1, n2);

  float denom = fvec3_dot(n1, n2xn3);

  if (fabsf(denom) < TI_EPSILON_6) {
    return 0;
  }

  intersection_point->x = (-p1.offset * n2xn3.x - p2.offset * n3xn1.x - p3.offset * n1xn2.x) / denom;
  intersection_point->y = (-p1.offset * n2xn3.y - p2.offset * n3xn1.y - p3.offset * n1xn2.y) / denom;
  intersection_point->z = (-p1.offset * n2xn3.z - p2.offset * n3xn1.z - p3.offset * n1xn2.z) / denom;

  return 1;
}
