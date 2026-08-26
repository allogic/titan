#include <ti_pch.h>

void camera_init(camera_t *camera) {
  camera->is_debug_enabled = 0;
  camera->fov = deg_to_rad(45.0F);
  camera->near_z = 0.001F;
  camera->far_z = 100000.0F;
}
