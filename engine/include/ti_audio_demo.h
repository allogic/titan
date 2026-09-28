#ifndef TI_AUDIO_DEMO_H
#define TI_AUDIO_DEMO_H

#ifdef __cplusplus
extern "C" {
#endif

extern int32_t g_audio_demo_available;
extern int32_t g_audio_demo_door_open;

void ti_audio_demo_create(void);
void ti_audio_demo_destroy(void);
void ti_audio_demo_door(int32_t open);

#ifdef __cplusplus
}
#endif

#endif // TI_AUDIO_DEMO_H
