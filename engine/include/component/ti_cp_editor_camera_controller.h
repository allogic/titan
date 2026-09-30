#ifndef TI_CP_EDITOR_CAMERA_CONTROLLER_H
#define TI_CP_EDITOR_CAMERA_CONTROLLER_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

void cp_editor_camera_controller_init(cp_editor_camera_controller_t *editor_camera_controller);
void cp_editor_camera_controller_update(cp_editor_camera_controller_t *editor_camera_controller, cp_transform_t *transform, cp_velocity_t *velocity);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // TI_CP_EDITOR_CAMERA_CONTROLLER_H
