#include <ti_pch.h>

static void handle_position(cp_editor_camera_controller_t *editor_camera_controller, cp_transform_t *transform, cp_velocity_t *velocity);
static void handle_rotation(cp_editor_camera_controller_t *editor_camera_controller, cp_transform_t *transform, cp_velocity_t *velocity);

void cp_editor_camera_controller_init(cp_editor_camera_controller_t *editor_camera_controller) {
  editor_camera_controller->keyboard_speed_fast = 2.0F;
  editor_camera_controller->keyboard_speed_normal = 0.2F;
  editor_camera_controller->mouse_rotation_speed = 2.5F;
  editor_camera_controller->mouse_begin_x = 0.0F;
  editor_camera_controller->mouse_begin_y = 0.0F;
  editor_camera_controller->mouse_delta_x = 0.0F;
  editor_camera_controller->mouse_delta_y = 0.0F;
}
void cp_editor_camera_controller_update(cp_editor_camera_controller_t *editor_camera_controller, cp_transform_t *transform, cp_velocity_t *velocity) {
  handle_position(editor_camera_controller, transform, velocity);
  handle_rotation(editor_camera_controller, transform, velocity);
}

static void handle_position(cp_editor_camera_controller_t *editor_camera_controller, cp_transform_t *transform, cp_velocity_t *velocity) {
  float speed = is_keyboard_key_held(KEYBOARD_KEY_LEFT_SHIFT)
                  ? editor_camera_controller->keyboard_speed_fast
                  : editor_camera_controller->keyboard_speed_normal;

  fvec3_t v = {velocity->linear_x, velocity->linear_y, velocity->linear_z};

  if (is_keyboard_key_held(KEYBOARD_KEY_D)) {
    v = fvec3_add(v, fvec3_muls(cp_transform_local_right(transform), speed));
  }
  if (is_keyboard_key_held(KEYBOARD_KEY_A)) {
    v = fvec3_add(v, fvec3_muls(cp_transform_local_left(transform), speed));
  }
  if (is_keyboard_key_held(KEYBOARD_KEY_E)) {
    v = fvec3_add(v, fvec3_muls(cp_transform_local_up(transform), speed));
  }
  if (is_keyboard_key_held(KEYBOARD_KEY_Q)) {
    v = fvec3_add(v, fvec3_muls(cp_transform_local_down(transform), speed));
  }
  if (is_keyboard_key_held(KEYBOARD_KEY_W)) {
    v = fvec3_add(v, fvec3_muls(cp_transform_local_front(transform), speed));
  }
  if (is_keyboard_key_held(KEYBOARD_KEY_S)) {
    v = fvec3_add(v, fvec3_muls(cp_transform_local_back(transform), speed));
  }

  velocity->linear_x = v.x;
  velocity->linear_y = v.y;
  velocity->linear_z = v.z;
}
static void handle_rotation(cp_editor_camera_controller_t *editor_camera_controller, cp_transform_t *transform, cp_velocity_t *velocity) {
  float mouse_position_x = (float)g_window.mouse_position_x;
  float mouse_position_y = (float)g_window.mouse_position_y;

  if (is_mouse_key_pressed(MOUSE_KEY_RIGHT)) {

    editor_camera_controller->mouse_begin_x = mouse_position_x;
    editor_camera_controller->mouse_begin_y = mouse_position_y;

    editor_camera_controller->mouse_delta_x = 0.0F;
    editor_camera_controller->mouse_delta_y = 0.0F;
  }

  if (is_mouse_key_held(MOUSE_KEY_RIGHT)) {

    fvec3_t v = {velocity->angular_x, velocity->angular_y, velocity->angular_z};

    editor_camera_controller->mouse_delta_x = mouse_position_x - editor_camera_controller->mouse_begin_x;
    editor_camera_controller->mouse_delta_y = mouse_position_y - editor_camera_controller->mouse_begin_y;

    editor_camera_controller->mouse_begin_x = mouse_position_x;
    editor_camera_controller->mouse_begin_y = mouse_position_y;

    v.x = editor_camera_controller->mouse_delta_y * editor_camera_controller->mouse_rotation_speed;
    v.y = editor_camera_controller->mouse_delta_x * editor_camera_controller->mouse_rotation_speed;

    velocity->angular_x = v.x;
    velocity->angular_y = v.y;
    velocity->angular_z = v.z;
  }
}
