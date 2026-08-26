__forceinline fquat_t fquat_identity(void) {
  fquat_t q = {
    0.0F,
    0.0F,
    0.0F,
    1.0F,
  };

  return q;
}
__forceinline fquat_t fquat_add(fquat_t a, fquat_t b) {
  fquat_t q = {
    a.x + b.x,
    a.y + b.y,
    a.z + b.z,
    a.w + b.w,
  };

  return q;
}
__forceinline fquat_t fquat_adds(fquat_t a, float b) {
  fquat_t q = {
    a.x + b,
    a.y + b,
    a.z + b,
    a.w + b,
  };

  return q;
}
__forceinline fquat_t fquat_mul(fquat_t a, fquat_t b) {
  fquat_t q = {
    (a.w * b.x) + (a.x * b.w) + (a.y * b.z) - (a.z * b.y),
    (a.w * b.y) - (a.x * b.z) + (a.y * b.w) + (a.z * b.x),
    (a.w * b.z) + (a.x * b.y) - (a.y * b.x) + (a.z * b.w),
    (a.w * b.w) - (a.x * b.x) - (a.y * b.y) - (a.z * b.z),
  };

  return q;
}
__forceinline fquat_t fquat_muls(fquat_t a, float b) {
  fquat_t q = {
    a.x * b,
    a.y * b,
    a.z * b,
    a.w * b,
  };

  return q;
}
__forceinline fquat_t fquat_conjugate(fquat_t a) {
  fquat_t q = {
    -a.x,
    -a.y,
    -a.z,
    a.w,
  };

  return q;
}
__forceinline fvec3_t fquat_right(fquat_t a) {
  fvec3_t v = {
    1.0F - 2.0F * ((a.y * a.y) + (a.z * a.z)),
    2.0F * ((a.x * a.y) + (a.w * a.z)),
    2.0F * ((a.x * a.z) - (a.w * a.y)),
  };

  return v;
}
__forceinline fvec3_t fquat_up(fquat_t a) {
  fvec3_t v = {
    2.0F * ((a.x * a.y) - (a.w * a.z)),
    1.0F - 2.0F * ((a.x * a.x) + (a.z * a.z)),
    2.0F * ((a.y * a.z) + (a.w * a.x)),
  };

  return v;
}
__forceinline fvec3_t fquat_front(fquat_t a) {
  fvec3_t v = {
    2.0F * ((a.x * a.z) + (a.w * a.y)),
    2.0F * ((a.y * a.z) - (a.w * a.x)),
    1.0F - 2.0F * ((a.x * a.x) + (a.y * a.y)),
  };

  return v;
}
__forceinline fvec3_t fquat_left(fquat_t a) {
  fvec3_t v = {
    -(1.0F - 2.0F * ((a.y * a.y) + (a.z * a.z))),
    -(2.0F * ((a.x * a.y) + (a.w * a.z))),
    -(2.0F * ((a.x * a.z) - (a.w * a.y))),
  };

  return v;
}
__forceinline fvec3_t fquat_down(fquat_t a) {
  fvec3_t v = {
    -(2.0F * ((a.x * a.y) - (a.w * a.z))),
    -(1.0F - 2.0F * ((a.x * a.x) + (a.z * a.z))),
    -(2.0F * ((a.y * a.z) + (a.w * a.x))),
  };

  return v;
}
__forceinline fvec3_t fquat_back(fquat_t a) {
  fvec3_t v = {
    -(2.0F * ((a.x * a.z) + (a.w * a.y))),
    -(2.0F * ((a.y * a.z) - (a.w * a.x))),
    -(1.0F - 2.0F * ((a.x * a.x) + (a.y * a.y))),
  };

  return v;
}
__forceinline fvec3_t fquat_to_euler_angles(fquat_t a) {
  float sinp = 2.0F * (a.w * a.x - a.y * a.z);

  sinp = fmaxf(-1.0F, fminf(1.0F, sinp));

  fvec3_t v = {
    asinf(sinp),
    atan2f(2.0F * (a.w * a.y + a.x * a.z), 1.0F - 2.0F * (a.x * a.x + a.y * a.y)),
    atan2f(2.0F * (a.w * a.z + a.x * a.y), 1.0F - 2.0F * (a.x * a.x + a.z * a.z)),
  };

  return v;
}
__forceinline fvec3_t fquat_to_euler_angles_xyzw(float x, float y, float z, float w) {
  float sinp = 2.0F * (w * x - y * z);

  sinp = fmaxf(-1.0F, fminf(1.0F, sinp));

  fvec3_t v = {
    asinf(sinp),
    atan2f(2.0F * (w * y + x * z), 1.0F - 2.0F * (x * x + y * y)),
    atan2f(2.0F * (w * z + x * y), 1.0F - 2.0F * (x * x + z * z)),
  };

  return v;
}
__forceinline fquat_t fquat_from_euler_angles(fvec3_t a) {
  float sp = sinf(a.x * 0.5F);
  float sy = sinf(a.y * 0.5F);
  float sr = sinf(a.z * 0.5F);

  float cp = cosf(a.x * 0.5F);
  float cy = cosf(a.y * 0.5F);
  float cr = cosf(a.z * 0.5F);

  fquat_t q = {
    sr * cp * cy - cr * sp * sy,
    cr * sp * cy + sr * cp * sy,
    cr * cp * sy - sr * sp * cy,
    cr * cp * cy + sr * sp * sy,
  };

  return q;
}
__forceinline fquat_t fquat_from_euler_angles_pyr(float p, float y, float r) {
  float sp = sinf(p * 0.5F);
  float sy = sinf(y * 0.5F);
  float sr = sinf(r * 0.5F);

  float cp = cosf(p * 0.5F);
  float cy = cosf(y * 0.5F);
  float cr = cosf(r * 0.5F);

  fquat_t q = {
    sr * cp * cy - cr * sp * sy,
    cr * sp * cy + sr * cp * sy,
    cr * cp * sy - sr * sp * cy,
    cr * cp * cy + sr * sp * sy,
  };

  return q;
}
__forceinline fquat_t fquat_angle_axis(float a, fvec3_t b) {
  fvec3_t n = fvec3_norm(b);

  float a_half = a * 0.5F;
  float s = sinf(a_half);

  fquat_t q = {
    n.x * s,
    n.y * s,
    n.z * s,
    cosf(a_half),
  };

  return q;
}
__forceinline fquat_t fquat_norm(fquat_t a) {
  float l = fquat_length(a);

  if (l > 0.0F) {
    return fquat_muls(a, 1.0F / l);
  } else {
    return fquat_identity();
  }
}
__forceinline float fquat_dot(fquat_t a, fquat_t b) {
  return (a.x * b.x) + (a.y * b.y) + (a.z * b.z) + (a.w * b.w);
}
__forceinline float fquat_length(fquat_t a) {
  return sqrtf(fquat_dot(a, a));
}
__forceinline float fquat_length2(fquat_t a) {
  return fquat_dot(a, a);
}
__forceinline void fquat_print(fquat_t a) {
  printf("[%f, %f, %f, %f]\n", a.x, a.y, a.z, a.w);
}
