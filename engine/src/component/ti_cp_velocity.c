#include <ti_pch.h>

static void handle_linear_velocity(cp_velocity_t *velocity, cp_transform_t *transform);
static void handle_angular_velocity(cp_velocity_t *velocity, cp_transform_t *transform);

void cp_velocity_init(cp_velocity_t *velocity) {
  velocity->linear_x = 0.0F;
  velocity->linear_y = 0.0F;
  velocity->linear_z = 0.0F;
  velocity->angular_x = 0.0F;
  velocity->angular_y = 0.0F;
  velocity->angular_z = 0.0F;
  velocity->linear_drag = 10.0F;
  velocity->angular_drag = 10.0F;
}
void cp_velocity_update(cp_velocity_t *velocity, cp_transform_t *transform) {
  handle_linear_velocity(velocity, transform);
  handle_angular_velocity(velocity, transform);
}

static void handle_linear_velocity(cp_velocity_t *velocity, cp_transform_t *transform) {
  float delta_time = g_window.delta_time;
  float linear_damping = expf(-velocity->linear_drag * delta_time);

  transform->position_x = transform->position_x + velocity->linear_x * delta_time;
  transform->position_y = transform->position_y + velocity->linear_y * delta_time;
  transform->position_z = transform->position_z + velocity->linear_z * delta_time;

  velocity->linear_x *= linear_damping;
  velocity->linear_y *= linear_damping;
  velocity->linear_z *= linear_damping;
}
static void handle_angular_velocity(cp_velocity_t *velocity, cp_transform_t *transform) {
  float delta_time = g_window.delta_time;
  float angular_damping = expf(-velocity->angular_drag * delta_time);

  fquat_t qp = fquat_angle_axis(velocity->angular_x * delta_time, cp_transform_local_right(transform));
  fquat_t qy = fquat_angle_axis(velocity->angular_y * delta_time, fvec3_up());
  fquat_t q = fquat_mul(qy, qp);

  fquat_t cr = {transform->rotation_x, transform->rotation_y, transform->rotation_z, transform->rotation_w};
  fquat_t r = fquat_norm(fquat_mul(q, cr));

  transform->rotation_x = r.x;
  transform->rotation_y = r.y;
  transform->rotation_z = r.z;
  transform->rotation_w = r.w;

  velocity->angular_x *= angular_damping;
  velocity->angular_y *= angular_damping;
  velocity->angular_z *= angular_damping;
}
