__forceinline fvec2_t fvec2_right(void) {
  fvec2_t v = {
    1.0F,
    0.0F,
  };

  return v;
}
__forceinline fvec2_t fvec2_up(void) {
  fvec2_t v = {
    0.0F,
    1.0F,
  };

  return v;
}
__forceinline fvec2_t fvec2_left(void) {
  fvec2_t v = {
    -1.0F,
    0.0F,
  };

  return v;
}
__forceinline fvec2_t fvec2_down(void) {
  fvec2_t v = {
    0.0F,
    -1.0F,
  };

  return v;
}
__forceinline fvec2_t fvec2_negate(fvec2_t a) {
  fvec2_t v = {
    -a.x,
    -a.y,
  };

  return v;
}
__forceinline fvec2_t fvec2_add(fvec2_t a, fvec2_t b) {
  fvec2_t v = {
    a.x + b.x,
    a.y + b.y,
  };

  return v;
}
__forceinline fvec2_t fvec2_sub(fvec2_t a, fvec2_t b) {
  fvec2_t v = {
    a.x - b.x,
    a.y - b.y,
  };

  return v;
}
__forceinline fvec2_t fvec2_mul(fvec2_t a, fvec2_t b) {
  fvec2_t v = {
    a.x * b.x,
    a.y * b.y,
  };

  return v;
}
__forceinline fvec2_t fvec2_div(fvec2_t a, fvec2_t b) {
  fvec2_t v = {
    a.x / b.x,
    a.y / b.y,
  };

  return v;
}
__forceinline fvec2_t fvec2_adds(fvec2_t a, float b) {
  fvec2_t v = {
    a.x + b,
    a.y + b,
  };

  return v;
}
__forceinline fvec2_t fvec2_subs(fvec2_t a, float b) {
  fvec2_t v = {
    a.x - b,
    a.y - b,
  };

  return v;
}
__forceinline fvec2_t fvec2_muls(fvec2_t a, float b) {
  fvec2_t v = {
    a.x * b,
    a.y * b,
  };

  return v;
}
__forceinline fvec2_t fvec2_divs(fvec2_t a, float b) {
  fvec2_t v = {
    a.x / b,
    a.y / b,
  };

  return v;
}
__forceinline fvec2_t fvec2_norm(fvec2_t a) {
  float l = fvec2_length(a);

  if (l > 0.0F) {
    return fvec2_muls(a, 1.0F / l);
  } else {
    return (fvec2_t){0.0F, 0.0F};
  }
}
__forceinline fvec2_t fvec2_inv(fvec2_t a) {
  fvec2_t v = {
    1.0F / a.x,
    1.0F / a.y,
  };

  return v;
}
__forceinline fvec2_t fvec2_floor(fvec2_t a) {
  fvec2_t v = {
    floorf(a.x),
    floorf(a.y),
  };

  return v;
}
__forceinline fvec2_t fvec2_ceil(fvec2_t a) {
  fvec2_t v = {
    ceilf(a.x),
    ceilf(a.y),
  };

  return v;
}
__forceinline float fvec2_dot(fvec2_t a, fvec2_t b) {
  return (a.x * b.x) + (a.y * b.y);
}
__forceinline float fvec2_length(fvec2_t a) {
  return sqrtf(fvec2_dot(a, a));
}
__forceinline float fvec2_length2(fvec2_t a) {
  return fvec2_dot(a, a);
}
__forceinline void fvec2_print(fvec2_t a) {
  printf("[%f, %f]\n", a.x, a.y);
}
