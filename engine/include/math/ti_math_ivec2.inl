__forceinline ivec2_t ivec2_right(void) {
  ivec2_t v = {
    1,
    0,
  };

  return v;
}
__forceinline ivec2_t ivec2_up(void) {
  ivec2_t v = {
    0,
    1,
  };

  return v;
}
__forceinline ivec2_t ivec2_left(void) {
  ivec2_t v = {
    -1,
    0,
  };

  return v;
}
__forceinline ivec2_t ivec2_down(void) {
  ivec2_t v = {
    0,
    -1,
  };

  return v;
}
__forceinline ivec2_t ivec2_negate(ivec2_t a) {
  ivec2_t v = {
    -a.x,
    -a.y,
  };

  return v;
}
__forceinline ivec2_t ivec2_add(ivec2_t a, ivec2_t b) {
  ivec2_t v = {
    a.x + b.x,
    a.y + b.y,
  };

  return v;
}
__forceinline ivec2_t ivec2_sub(ivec2_t a, ivec2_t b) {
  ivec2_t v = {
    a.x - b.x,
    a.y - b.y,
  };

  return v;
}
__forceinline ivec2_t ivec2_mul(ivec2_t a, ivec2_t b) {
  ivec2_t v = {
    a.x * b.x,
    a.y * b.y,
  };

  return v;
}
__forceinline ivec2_t ivec2_div(ivec2_t a, ivec2_t b) {
  ivec2_t v = {
    a.x / b.x,
    a.y / b.y,
  };

  return v;
}
__forceinline ivec2_t ivec2_adds(ivec2_t a, int32_t b) {
  ivec2_t v = {
    a.x + b,
    a.y + b,
  };

  return v;
}
__forceinline ivec2_t ivec2_subs(ivec2_t a, int32_t b) {
  ivec2_t v = {
    a.x - b,
    a.y - b,
  };

  return v;
}
__forceinline ivec2_t ivec2_muls(ivec2_t a, int32_t b) {
  ivec2_t v = {
    a.x * b,
    a.y * b,
  };

  return v;
}
__forceinline ivec2_t ivec2_divs(ivec2_t a, int32_t b) {
  ivec2_t v = {
    a.x / b,
    a.y / b,
  };

  return v;
}
__forceinline float ivec2_dot(ivec2_t a, ivec2_t b) {
  return (float)((a.x * b.x) + (a.y * b.y));
}
__forceinline float ivec2_length(ivec2_t a) {
  return sqrtf(ivec2_dot(a, a));
}
__forceinline float ivec2_length2(ivec2_t a) {
  return ivec2_dot(a, a);
}
__forceinline uint32_t ivec2_hash32(ivec2_t a) {
  uint32_t h = 2166136261U;

  h = (h ^ a.x) * 16777619U;
  h = (h ^ a.y) * 16777619U;

  return h;
}
__forceinline uint32_t iveczor2_equal(ivec2_t a, ivec2_t b) {
  return (a.x == b.x) && (a.y == b.y);
}
__forceinline void ivec2_print(ivec2_t a) {
  printf("[%d, %d]\n", a.x, a.y);
}
