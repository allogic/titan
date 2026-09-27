__forceinline ivec3_t ivec3_right(void) {
  ivec3_t v = {
    1,
    0,
    0,
  };

  return v;
}
__forceinline ivec3_t ivec3_up(void) {
  ivec3_t v = {
    0,
    1,
    0,
  };

  return v;
}
__forceinline ivec3_t ivec3_front(void) {
  ivec3_t v = {
    0,
    0,
    1,
  };

  return v;
}
__forceinline ivec3_t ivec3_left(void) {
  ivec3_t v = {
    -1,
    0,
    0,
  };

  return v;
}
__forceinline ivec3_t ivec3_down(void) {
  ivec3_t v = {
    0,
    -1,
    0,
  };

  return v;
}
__forceinline ivec3_t ivec3_back(void) {
  ivec3_t v = {
    0,
    0,
    -1,
  };

  return v;
}
__forceinline ivec3_t ivec3_negate(ivec3_t a) {
  ivec3_t v = {
    -a.x,
    -a.y,
    -a.z,
  };

  return v;
}
__forceinline ivec3_t ivec3_add(ivec3_t a, ivec3_t b) {
  ivec3_t v = {
    a.x + b.x,
    a.y + b.y,
    a.z + b.z,
  };

  return v;
}
__forceinline ivec3_t ivec3_sub(ivec3_t a, ivec3_t b) {
  ivec3_t v = {
    a.x - b.x,
    a.y - b.y,
    a.z - b.z,
  };

  return v;
}
__forceinline ivec3_t ivec3_mul(ivec3_t a, ivec3_t b) {
  ivec3_t v = {
    a.x * b.x,
    a.y * b.y,
    a.z * b.z,
  };

  return v;
}
__forceinline ivec3_t ivec3_div(ivec3_t a, ivec3_t b) {
  ivec3_t v = {
    a.x / b.x,
    a.y / b.y,
    a.z / b.z,
  };

  return v;
}
__forceinline ivec3_t ivec3_adds(ivec3_t a, int32_t b) {
  ivec3_t v = {
    a.x + b,
    a.y + b,
    a.z + b,
  };

  return v;
}
__forceinline ivec3_t ivec3_subs(ivec3_t a, int32_t b) {
  ivec3_t v = {
    a.x - b,
    a.y - b,
    a.z - b,
  };

  return v;
}
__forceinline ivec3_t ivec3_muls(ivec3_t a, int32_t b) {
  ivec3_t v = {
    a.x * b,
    a.y * b,
    a.z * b,
  };

  return v;
}
__forceinline ivec3_t ivec3_divs(ivec3_t a, int32_t b) {
  ivec3_t v = {
    a.x / b,
    a.y / b,
    a.z / b,
  };

  return v;
}
__forceinline float ivec3_dot(ivec3_t a, ivec3_t b) {
  return (float)((a.x * b.x) + (a.y * b.y) + (a.z * b.z));
}
__forceinline float ivec3_length(ivec3_t a) {
  return sqrtf(ivec3_dot(a, a));
}
__forceinline float ivec3_length2(ivec3_t a) {
  return ivec3_dot(a, a);
}
__forceinline uint32_t ivec3_hash32(ivec3_t a) {
  uint32_t h = 2166136261U;

  h = (h ^ a.x) * 16777619U;
  h = (h ^ a.y) * 16777619U;
  h = (h ^ a.z) * 16777619U;

  return h;
}
__forceinline uint32_t ivec3_equal(ivec3_t a, ivec3_t b) {
  return (a.x == b.x) && (a.y == b.y) && (a.z == b.z);
}
__forceinline void ivec3_print(ivec3_t a) {
  printf("[%d, %d, %d]\n", a.x, a.y, a.z);
}
