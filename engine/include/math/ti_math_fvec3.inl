__forceinline fvec3_t fvec3_right(void) {
  fvec3_t v = {
    1.0F,
    0.0F,
    0.0F,
  };

  return v;
}
__forceinline fvec3_t fvec3_up(void) {
  fvec3_t v = {
    0.0F,
    1.0F,
    0.0F,
  };

  return v;
}
__forceinline fvec3_t fvec3_front(void) {
  fvec3_t v = {
    0.0F,
    0.0F,
    1.0F,
  };

  return v;
}
__forceinline fvec3_t fvec3_left(void) {
  fvec3_t v = {
    -1.0F,
    0.0F,
    0.0F,
  };

  return v;
}
__forceinline fvec3_t fvec3_down(void) {
  fvec3_t v = {
    0.0F,
    -1.0F,
    0.0F,
  };

  return v;
}
__forceinline fvec3_t fvec3_back(void) {
  fvec3_t v = {
    0.0F,
    0.0F,
    -1.0F,
  };

  return v;
}
__forceinline fvec3_t fvec3_negate(fvec3_t a) {
  fvec3_t v = {
    -a.x,
    -a.y,
    -a.z,
  };

  return v;
}
__forceinline fvec3_t fvec3_add(fvec3_t a, fvec3_t b) {
  fvec3_t v = {
    a.x + b.x,
    a.y + b.y,
    a.z + b.z,
  };

  return v;
}
__forceinline fvec3_t fvec3_sub(fvec3_t a, fvec3_t b) {
  fvec3_t v = {
    a.x - b.x,
    a.y - b.y,
    a.z - b.z,
  };

  return v;
}
__forceinline fvec3_t fvec3_mul(fvec3_t a, fvec3_t b) {
  fvec3_t v = {
    a.x * b.x,
    a.y * b.y,
    a.z * b.z,
  };

  return v;
}
__forceinline fvec3_t fvec3_div(fvec3_t a, fvec3_t b) {
  fvec3_t v = {
    a.x / b.x,
    a.y / b.y,
    a.z / b.z,
  };

  return v;
}
__forceinline fvec3_t fvec3_adds(fvec3_t a, float b) {
  fvec3_t v = {
    a.x + b,
    a.y + b,
    a.z + b,
  };

  return v;
}
__forceinline fvec3_t fvec3_subs(fvec3_t a, float b) {
  fvec3_t v = {
    a.x - b,
    a.y - b,
    a.z - b,
  };

  return v;
}
__forceinline fvec3_t fvec3_muls(fvec3_t a, float b) {
  fvec3_t v = {
    a.x * b,
    a.y * b,
    a.z * b,
  };

  return v;
}
__forceinline fvec3_t fvec3_divs(fvec3_t a, float b) {
  fvec3_t v = {
    a.x / b,
    a.y / b,
    a.z / b,
  };

  return v;
}
__forceinline fvec3_t fvec3_norm(fvec3_t a) {
  float l = fvec3_length(a);

  if (l > 0.0F) {
    return fvec3_muls(a, 1.0F / l);
  } else {
    return (fvec3_t){0.0F, 0.0F, 0.0F};
  }
}
__forceinline fvec3_t fvec3_inv(fvec3_t a) {
  fvec3_t v = {
    1.0F / a.x,
    1.0F / a.y,
    1.0F / a.z,
  };

  return v;
}
__forceinline fvec3_t fvec3_floor(fvec3_t a) {
  fvec3_t v = {
    floorf(a.x),
    floorf(a.y),
    floorf(a.z),
  };

  return v;
}
__forceinline fvec3_t fvec3_ceil(fvec3_t a) {
  fvec3_t v = {
    ceilf(a.x),
    ceilf(a.y),
    ceilf(a.z),
  };

  return v;
}
__forceinline fvec3_t fvec3_cross(fvec3_t a, fvec3_t b) {
  fvec3_t v = {
    (a.y * b.z) - (a.z * b.y),
    (a.z * b.x) - (a.x * b.z),
    (a.x * b.y) - (a.y * b.x),
  };

  return v;
}
__forceinline fvec3_t fvec3_rotate(fvec3_t a, fquat_t b) {
  float xx = b.x * b.x;
  float yy = b.y * b.y;
  float zz = b.z * b.z;

  float xy = b.x * b.y;
  float xz = b.x * b.z;
  float yz = b.y * b.z;

  float wx = b.w * b.x;
  float wy = b.w * b.y;
  float wz = b.w * b.z;

  fvec3_t v = {
    (1.0F - 2.0F * (yy + zz)) * a.x + 2.0F * (xy - wz) * a.y + 2.0F * (xz + wy) * a.z,
    2.0F * (xy + wz) * a.x + (1.0F - 2.0F * (xx + zz)) * a.y + 2.0F * (yz - wx) * a.z,
    2.0F * (xz - wy) * a.x + 2.0F * (yz + wx) * a.y + (1.0F - 2.0F * (xx + yy)) * a.z,
  };

  return v;
}
__forceinline float fvec3_dot(fvec3_t a, fvec3_t b) {
  return (a.x * b.x) + (a.y * b.y) + (a.z * b.z);
}
__forceinline float fvec3_length(fvec3_t a) {
  return sqrtf(fvec3_dot(a, a));
}
__forceinline float fvec3_length2(fvec3_t a) {
  return fvec3_dot(a, a);
}
__forceinline void fvec3_print(fvec3_t a) {
  printf("[%f, %f, %f]\n", a.x, a.y, a.z);
}
