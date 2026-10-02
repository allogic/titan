#ifndef TI_PH_WORLD_H
#define TI_PH_WORLD_H

#include <ti_engine.h>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

extern ti_physic_t g_ph_world;

void ph_world_create(void);
void ph_world_destroy(void);
void ph_world_update(float delta_time);
void ph_world_step(void);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // TI_PH_WORLD_H
