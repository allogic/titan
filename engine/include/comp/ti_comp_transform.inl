__forceinline fvec3_t transform_local_right(transform_t const *transform) {
  return fquat_right((fquat_t){transform->rotation_x, transform->rotation_y, transform->rotation_z, transform->rotation_w});
}
__forceinline fvec3_t transform_local_up(transform_t const *transform) {
  return fquat_up((fquat_t){transform->rotation_x, transform->rotation_y, transform->rotation_z, transform->rotation_w});
}
__forceinline fvec3_t transform_local_front(transform_t const *transform) {
  return fquat_front((fquat_t){transform->rotation_x, transform->rotation_y, transform->rotation_z, transform->rotation_w});
}
__forceinline fvec3_t transform_local_left(transform_t const *transform) {
  return fvec3_negate(fquat_right((fquat_t){transform->rotation_x, transform->rotation_y, transform->rotation_z, transform->rotation_w}));
}
__forceinline fvec3_t transform_local_down(transform_t const *transform) {
  return fvec3_negate(fquat_up((fquat_t){transform->rotation_x, transform->rotation_y, transform->rotation_z, transform->rotation_w}));
}
__forceinline fvec3_t transform_local_back(transform_t const *transform) {
  return fvec3_negate(fquat_front((fquat_t){transform->rotation_x, transform->rotation_y, transform->rotation_z, transform->rotation_w}));
}

// void transform_set_position(transform_t *transform, fvec3_t position) {
//   transform->position = position;
// }
// void transform_set_position_xyz(transform_t *transform, float x, float y, float z) {
//   transform->position.x = x;
//   transform->position.y = y;
//   transform->position.z = z;
// }
// void transform_set_relative_position(transform_t *transform, transform_t *reference, fvec3_t position) {
//   transform->position.x = reference->position.x + position.x;
//   transform->position.y = reference->position.y + position.y;
//   transform->position.z = reference->position.z + position.z;
// }
// void transform_set_relative_position_xyz(transform_t *transform, transform_t *reference, float x, float y, float z) {
//   transform->local_position.x = reference->world_position.x + x;
//   transform->local_position.y = reference->world_position.y + y;
//   transform->local_position.z = reference->world_position.z + z;
// }

void transform_set_rotation(transform_t *transform, fquat_t rotation) {
  fquat_t q = fquat_norm(rotation);

  transform->rotation_x = q.x;
  transform->rotation_y = q.y;
  transform->rotation_z = q.z;
  transform->rotation_w = q.w;
}
void transform_set_rotation_xyzw(transform_t *transform, float x, float y, float z, float w) {
  fquat_t q = fquat_norm((fquat_t){x, y, z, w});

  transform->rotation_x = q.x;
  transform->rotation_y = q.y;
  transform->rotation_z = q.z;
  transform->rotation_w = q.w;
}
// void transform_set_relative_rotation(transform_t *transform, transform_t *reference, fquat_t rotation) {
//   transform->rotation = fquat_norm(fquat_mul(reference->world_rotation, rotation));
// }
// void transform_set_relative_rotation_xyzw(transform_t *transform, transform_t *reference, float x, float y, float z, float w) {
//   transform->rotation = fquat_norm(fquat_mul(reference->world_rotation, (fquat_t){x, y, z, w}));
// }

void transform_set_euler_angles(transform_t *transform, fvec3_t euler_angles) {
  fquat_t qx = fquat_angle_axis(euler_angles.x, fvec3_right());
  fquat_t qy = fquat_angle_axis(euler_angles.y, fvec3_up());
  fquat_t qz = fquat_angle_axis(euler_angles.z, fvec3_front());

  fquat_t q = fquat_norm(fquat_mul(qy, fquat_mul(qx, qz)));

  transform->rotation_x = q.x;
  transform->rotation_y = q.y;
  transform->rotation_z = q.z;
  transform->rotation_w = q.w;
}
void transform_set_euler_angles_pyr(transform_t *transform, float p, float y, float r) {
  fquat_t qx = fquat_angle_axis(p, fvec3_right());
  fquat_t qy = fquat_angle_axis(y, fvec3_up());
  fquat_t qz = fquat_angle_axis(r, fvec3_front());

  fquat_t q = fquat_norm(fquat_mul(qy, fquat_mul(qx, qz)));

  transform->rotation_x = q.x;
  transform->rotation_y = q.y;
  transform->rotation_z = q.z;
  transform->rotation_w = q.w;
}
// void transform_set_relative_euler_angles(transform_t *transform, transform_t *reference, fvec3_t euler_angles) {
//   fquat_t qx = fquat_angle_axis(euler_angles.x, transform_local_right(reference));
//   fquat_t qy = fquat_angle_axis(euler_angles.y, transform_local_up(reference));
//   fquat_t qz = fquat_angle_axis(euler_angles.z, transform_local_front(reference));
//
//   transform->rotation = fquat_norm(fquat_mul(transform->rotation, fquat_mul(qy, fquat_mul(qx, qz))));
// }
// void transform_set_relative_euler_angles_pyr(transform_t *transform, transform_t *reference, float p, float y, float r) {
//   fquat_t qx = fquat_angle_axis(p, transform_local_right(reference));
//   fquat_t qy = fquat_angle_axis(y, transform_local_up(reference));
//   fquat_t qz = fquat_angle_axis(r, transform_local_front(reference));
//
//   transform->rotation = fquat_norm(fquat_mul(transform->rotation, fquat_mul(qy, fquat_mul(qx, qz))));
// }

// void transform_set_scale(transform_t *transform, fvec3_t scale) {
//   transform->local_scale = scale;
// }
// void transform_set_scale_xyz(transform_t *transform, float x, float y, float z) {
//   transform->local_scale.x = x;
//   transform->local_scale.y = y;
//   transform->local_scale.z = z;
// }
// void transform_set_relative_scale(transform_t *transform, transform_t *reference, fvec3_t scale) {
//   transform->local_scale.x = reference->world_scale.x * scale.x;
//   transform->local_scale.y = reference->world_scale.y * scale.y;
//   transform->local_scale.z = reference->world_scale.z * scale.z;
// }
// void transform_set_relative_scale_xyz(transform_t *transform, transform_t *reference, float x, float y, float z) {
//   transform->local_scale.x = reference->world_scale.x * x;
//   transform->local_scale.y = reference->world_scale.y * y;
//   transform->local_scale.z = reference->world_scale.z * z;
// }
