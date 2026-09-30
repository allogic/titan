#ifndef TI_CP_VELOCITY_H
#define TI_CP_VELOCITY_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

void cp_velocity_init(cp_velocity_t *velocity);
void cp_velocity_update(cp_velocity_t *velocity, cp_transform_t *transform);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // TI_CP_VELOCITY_H
