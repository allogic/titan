__forceinline ivec4_t ivec4_negate(ivec4_t a) {
  ivec4_t v = {
    -a.x,
    -a.y,
    -a.z,
    -a.w,
  };

  return v;
}
__forceinline ivec4_t ivec4_add(ivec4_t a, ivec4_t b) {
  ivec4_t v = {
    a.x + b.x,
    a.y + b.y,
    a.z + b.z,
    a.w + b.w,
  };

  return v;
}
__forceinline ivec4_t ivec4_sub(ivec4_t a, ivec4_t b) {
  ivec4_t v = {
    a.x - b.x,
    a.y - b.y,
    a.z - b.z,
    a.w - b.w,
  };

  return v;
}
__forceinline ivec4_t ivec4_mul(ivec4_t a, ivec4_t b) {
  ivec4_t v = {
    a.x * b.x,
    a.y * b.y,
    a.z * b.z,
    a.w * b.w,
  };

  return v;
}
__forceinline ivec4_t ivec4_div(ivec4_t a, ivec4_t b) {
  ivec4_t v = {
    a.x / b.x,
    a.y / b.y,
    a.z / b.z,
    a.w / b.w,
  };

  return v;
}
__forceinline ivec4_t ivec4_adds(ivec4_t a, int32_t b) {
  ivec4_t v = {
    a.x + b,
    a.y + b,
    a.z + b,
    a.w + b,
  };

  return v;
}
__forceinline ivec4_t ivec4_subs(ivec4_t a, int32_t b) {
  ivec4_t v = {
    a.x - b,
    a.y - b,
    a.z - b,
    a.w - b,
  };

  return v;
}
__forceinline ivec4_t ivec4_muls(ivec4_t a, int32_t b) {
  ivec4_t v = {
    a.x * b,
    a.y * b,
    a.z * b,
    a.w * b,
  };

  return v;
}
__forceinline ivec4_t ivec4_divs(ivec4_t a, int32_t b) {
  ivec4_t v = {
    a.x / b,
    a.y / b,
    a.z / b,
    a.w / b,
  };

  return v;
}
__forceinline float ivec4_dot(ivec4_t a, ivec4_t b) {
  return (float)((a.x * b.x) + (a.y * b.y) + (a.z * b.z) + (a.w * b.w));
}
__forceinline float ivec4_length(ivec4_t a) {
  return sqrtf(ivec4_dot(a, a));
}
__forceinline float ivec4_length2(ivec4_t a) {
  return ivec4_dot(a, a);
}
__forceinline uint32_t ivec4_hash32(ivec4_t a) {
  uint32_t h = 2166136261U;

  h = (h ^ a.x) * 16777619U;
  h = (h ^ a.y) * 16777619U;
  h = (h ^ a.z) * 16777619U;
  h = (h ^ a.w) * 16777619U;

  return h;
}
__forceinline uint32_t iveczor4_equal(ivec4_t a, ivec4_t b) {
  return (a.x == b.x) && (a.y == b.y) && (a.z == b.z) && (a.w == b.w);
}
__forceinline void ivec4_print(ivec4_t a) {
  printf("[%d, %d, %d, %d]\n", a.x, a.y, a.z, a.w);
}
