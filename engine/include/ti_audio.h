#ifndef TI_AUDIO_H
#define TI_AUDIO_H

#include <phonon.h>

#define TI_AUDIO_SAMPLE_RATE 48000
#define TI_AUDIO_FRAME_SIZE 512

typedef enum ti_audio_state_t {
  TI_AUDIO_STATE_STOPPED,
  TI_AUDIO_STATE_PLAYING,
  TI_AUDIO_STATE_PAUSED,
} ti_audio_state_t;

typedef struct ti_audio_t {
  IPLContext context;
  IPLSimulator simulator;
  IPLScene scene;
  int32_t device_available;
} ti_audio_t;

typedef struct ti_sound_t {
  float *samples;
  uint64_t frame_count;
  uint32_t channel_count;
  uint32_t source_count;
} ti_sound_t;

typedef struct ti_audio_source_t ti_audio_source_t;

#ifdef __cplusplus
extern "C" {
#endif

extern ti_audio_t g_audio;

void audio_create(void);
void audio_destroy(void);
int32_t audio_load(ti_sound_t **sound, char const *file_path);
int32_t audio_unload(ti_sound_t **sound);
int32_t audio_source_create(ti_audio_source_t **source, ti_sound_t *sound);
void audio_source_destroy(ti_audio_source_t **source);
void audio_source_play(ti_audio_source_t *source);
void audio_source_pause(ti_audio_source_t *source);
void audio_source_stop(ti_audio_source_t *source);
void audio_source_set(ti_audio_source_t *source, IPLVector3 *position, float gain, int32_t loop);
int32_t audio_source_state(ti_audio_source_t *source);
void audio_listener_set(IPLCoordinateSpace3 *listener);
void audio_update(void);
void audio_render(float *output, uint32_t frame_count);

#ifdef __cplusplus
}
#endif

#endif // TI_AUDIO_H
