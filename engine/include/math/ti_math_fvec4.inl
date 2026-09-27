__forceinline fvec4_t fvec4_negate(fvec4_t a) {
  fvec4_t v = {
    -a.x,
    -a.y,
    -a.z,
    -a.w,
  };

  return v;
}
__forceinline fvec4_t fvec4_add(fvec4_t a, fvec4_t b) {
  fvec4_t v = {
    a.x + b.x,
    a.y + b.y,
    a.z + b.z,
    a.w + b.w,
  };

  return v;
}
__forceinline fvec4_t fvec4_sub(fvec4_t a, fvec4_t b) {
  fvec4_t v = {
    a.x - b.x,
    a.y - b.y,
    a.z - b.z,
    a.w - b.w,
  };

  return v;
}
__forceinline fvec4_t fvec4_mul(fvec4_t a, fvec4_t b) {
  fvec4_t v = {
    a.x * b.x,
    a.y * b.y,
    a.z * b.z,
    a.w * b.w,
  };

  return v;
}
__forceinline fvec4_t fvec4_div(fvec4_t a, fvec4_t b) {
  fvec4_t v = {
    a.x / b.x,
    a.y / b.y,
    a.z / b.z,
    a.w / b.w,
  };

  return v;
}
__forceinline fvec4_t fvec4_adds(fvec4_t a, float b) {
  fvec4_t v = {
    a.x + b,
    a.y + b,
    a.z + b,
    a.w + b,
  };

  return v;
}
__forceinline fvec4_t fvec4_subs(fvec4_t a, float b) {
  fvec4_t v = {
    a.x - b,
    a.y - b,
    a.z - b,
    a.w - b,
  };

  return v;
}
__forceinline fvec4_t fvec4_muls(fvec4_t a, float b) {
  fvec4_t v = {
    a.x * b,
    a.y * b,
    a.z * b,
    a.w * b,
  };

  return v;
}
__forceinline fvec4_t fvec4_divs(fvec4_t a, float b) {
  fvec4_t v = {
    a.x / b,
    a.y / b,
    a.z / b,
    a.w / b,
  };

  return v;
}
__forceinline fvec4_t fvec4_norm(fvec4_t a) {
  float l = fvec4_length(a);

  if (l > 0.0F) {
    return fvec4_muls(a, 1.0F / l);
  } else {
    return (fvec4_t){0.0F, 0.0F, 0.0F};
  }
}
__forceinline fvec4_t fvec4_inv(fvec4_t a) {
  fvec4_t v = {
    1.0F / a.x,
    1.0F / a.y,
    1.0F / a.z,
    1.0F / a.w,
  };

  return v;
}
__forceinline fvec4_t fvec4_floor(fvec4_t a) {
  fvec4_t v = {
    floorf(a.x),
    floorf(a.y),
    floorf(a.z),
    floorf(a.w),
  };

  return v;
}
__forceinline fvec4_t fvec4_ceil(fvec4_t a) {
  fvec4_t v = {
    ceilf(a.x),
    ceilf(a.y),
    ceilf(a.z),
    ceilf(a.w),
  };

  return v;
}
__forceinline float fvec4_dot(fvec4_t a, fvec4_t b) {
  return (a.x * b.x) + (a.y * b.y) + (a.z * b.z) + (a.w * b.w);
}
__forceinline float fvec4_length(fvec4_t a) {
  return sqrtf(fvec4_dot(a, a));
}
__forceinline float fvec4_length2(fvec4_t a) {
  return fvec4_dot(a, a);
}
__forceinline void fvec4_print(fvec4_t a) {
  printf("[%f, %f, %f, %f]\n", a.x, a.y, a.z, a.w);
}
