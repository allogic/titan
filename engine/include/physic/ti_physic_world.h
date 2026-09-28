#ifndef TI_PHYSIC_WORLD_H
#define TI_PHYSIC_WORLD_H

#ifdef __cplusplus
extern "C" {
#endif

extern ti_physic_t g_physic;

void ti_physic_create(void);
void ti_physic_destroy(void);
void ti_physic_update(float delta_time);
void ti_physic_step(void);

#ifdef __cplusplus
}
#endif

#endif // TI_PHYSIC_WORLD_H
