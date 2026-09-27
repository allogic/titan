#ifndef TI_MATH_FWD_H
#define TI_MATH_FWD_H

typedef struct fvec2_t {
  float x;
  float y;
} fvec2_t;
typedef struct ivec2_t {
  int32_t x;
  int32_t y;
} ivec2_t;
typedef struct fvec3_t {
  float x;
  float y;
  float z;
} fvec3_t;
typedef struct ivec3_t {
  int32_t x;
  int32_t y;
  int32_t z;
} ivec3_t;
typedef struct fvec4_t {
  float x;
  float y;
  float z;
  float w;
} fvec4_t;
typedef struct ivec4_t {
  int32_t x;
  int32_t y;
  int32_t z;
  int32_t w;
} ivec4_t;
typedef struct fquat_t {
  float x;
  float y;
  float z;
  float w;
} fquat_t;
typedef struct fmat4x4_t {
  float m00, m01, m02, m03;
  float m10, m11, m12, m13;
  float m20, m21, m22, m23;
  float m30, m31, m32, m33;
} fmat4x4_t;

#endif // TI_MATH_FWD_H
