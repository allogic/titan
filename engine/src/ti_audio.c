#include <ti_pch.h>

#include <miniaudio.h>

struct ti_audio_source_t {
  ti_sound_t *sound;
  ti_audio_source_t *next;
  IPLSource simulation;
  IPLDirectEffect direct_effect;
  IPLBinauralEffect binaural_effect;
  IPLDirectEffectParams direct;
  IPLVector3 position;
  IPLVector3 direction;
  uint64_t cursor;
  float gain;
  int loop;
  int state;
  IPLAudioEffectState binaural_tail;
};

ti_audio_t g_audio = {0};

static SRWLOCK s_lock = SRWLOCK_INIT;
static ma_device s_device;
static IPLHRTF s_hrtf;
static ti_audio_source_t *s_sources;
static IPLCoordinateSpace3 s_listener;
static float s_mix[TI_AUDIO_FRAME_SIZE * 2];
static uint32_t s_mix_cursor = TI_AUDIO_FRAME_SIZE;

static void audio_callback(ma_device *device, void *output, void const *input, ma_uint32 frame_count) {
  audio_render(output, frame_count);
}

void audio_create(void) {
  IPLContextSettings context_settings = {
    .version = STEAMAUDIO_VERSION,
    .simdLevel = IPL_SIMDLEVEL_AVX2,
  };
  IPLAudioSettings audio_settings = {TI_AUDIO_SAMPLE_RATE, TI_AUDIO_FRAME_SIZE};
  IPLHRTFSettings hrtf_settings = {.type = IPL_HRTFTYPE_DEFAULT, .volume = 1.0F};
  IPLSceneSettings scene_settings = {.type = IPL_SCENETYPE_DEFAULT};
  IPLSimulationSettings simulation_settings = {
    .flags = IPL_SIMULATIONFLAGS_DIRECT,
    .sceneType = IPL_SCENETYPE_DEFAULT,
    .maxNumOcclusionSamples = 1,
    .numThreads = 1,
    .samplingRate = TI_AUDIO_SAMPLE_RATE,
    .frameSize = TI_AUDIO_FRAME_SIZE,
  };

  s_listener = (IPLCoordinateSpace3){.right = {1, 0, 0}, .up = {0, 1, 0}, .ahead = {0, 0, -1}};
  s_mix_cursor = TI_AUDIO_FRAME_SIZE;

  if (iplContextCreate(&context_settings, &g_audio.context) != IPL_STATUS_SUCCESS ||
      iplHRTFCreate(g_audio.context, &audio_settings, &hrtf_settings, &s_hrtf) != IPL_STATUS_SUCCESS ||
      iplSceneCreate(g_audio.context, &scene_settings, &g_audio.scene) != IPL_STATUS_SUCCESS ||
      iplSimulatorCreate(g_audio.context, &simulation_settings, &g_audio.simulator) != IPL_STATUS_SUCCESS) {
    audio_destroy();
    return;
  }

  iplSceneCommit(g_audio.scene);
  iplSimulatorSetScene(g_audio.simulator, g_audio.scene);
  iplSimulatorCommit(g_audio.simulator);

  ma_device_config device_config = ma_device_config_init(ma_device_type_playback);
  device_config.playback.format = ma_format_f32;
  device_config.playback.channels = 2;
  device_config.sampleRate = TI_AUDIO_SAMPLE_RATE;
  device_config.periodSizeInFrames = TI_AUDIO_FRAME_SIZE;
  device_config.dataCallback = audio_callback;

  if (ma_device_init(0, &device_config, &s_device) == MA_SUCCESS) {
    if (s_device.pContext->backend != ma_backend_null && ma_device_start(&s_device) == MA_SUCCESS) {
      g_audio.device_available = 1;
    } else {
      ma_device_uninit(&s_device);
    }
  }
}
void audio_destroy(void) {
  if (g_audio.device_available != 0) {
    ma_device_uninit(&s_device);
    g_audio.device_available = 0;
  }

  while (s_sources != 0) {
    ti_audio_source_t *source = s_sources;
    audio_source_destroy(&source);
  }

  iplSimulatorRelease(&g_audio.simulator);
  iplSceneRelease(&g_audio.scene);
  iplHRTFRelease(&s_hrtf);
  iplContextRelease(&g_audio.context);
  s_mix_cursor = TI_AUDIO_FRAME_SIZE;
}
int audio_load(ti_sound_t **sound, char const *file_path) {
  fs_asset_t asset = {.path = file_path};
  ti_sound_t *loaded = 0;
  int result = 1;

  fs_asset_load(&asset);

  if (asset.type != FS_ASSET_TYPE_SOUND || asset.instance == 0) {
    goto cleanup;
  }

  fs_sound_t *config = asset.instance;

  if (config->buffer == 0) {
    goto cleanup;
  }

  uint64_t input_frames = config->buffer_size / (config->channel_count * sizeof(int16_t));
  uint64_t output_frames = ma_convert_frames(0, 0, ma_format_f32, config->channel_count, TI_AUDIO_SAMPLE_RATE,
                                             config->buffer, input_frames, ma_format_s16, config->channel_count, config->sample_rate);

  if (output_frames == 0 || output_frames > SIZE_MAX / (config->channel_count * sizeof(float))) {
    goto cleanup;
  }

  loaded = TI_ALLOC(sizeof(ti_sound_t), 1, 0);

  if (loaded == 0) {
    goto cleanup;
  }

  loaded->samples = TI_ALLOC(output_frames * config->channel_count * sizeof(float), 0, 0);

  if (loaded->samples == 0) {
    goto cleanup;
  }

  loaded->channel_count = config->channel_count;
  loaded->frame_count = ma_convert_frames(loaded->samples, output_frames, ma_format_f32,
                                          config->channel_count, TI_AUDIO_SAMPLE_RATE, config->buffer, input_frames,
                                          ma_format_s16, config->channel_count, config->sample_rate);

  if (loaded->frame_count == 0) {
    goto cleanup;
  }

  if (audio_unload(sound) != 0) {
    goto cleanup;
  }

  *sound = loaded;
  loaded = 0;
  result = 0;

cleanup:
  audio_unload(&loaded);

  if (asset.instance != 0) {
    fs_asset_destroy(&asset);
  }

  return result;
}
int audio_unload(ti_sound_t **sound) {
  ti_sound_t *removed = 0;
  int result = 0;

  AcquireSRWLockExclusive(&s_lock);

  if (*sound != 0) {
    if ((*sound)->source_count == 0) {
      removed = *sound;
      *sound = 0;
    } else {
      result = 1;
    }
  }

  ReleaseSRWLockExclusive(&s_lock);

  if (removed != 0) {
    TI_FREE(removed->samples);
    TI_FREE(removed);
  }

  return result;
}
int audio_source_create(ti_audio_source_t **source, ti_sound_t *sound) {
  if (g_audio.simulator == 0 || sound == 0 || *source != 0) {
    return 1;
  }

  ti_audio_source_t *created = TI_ALLOC(sizeof(ti_audio_source_t), 1, 0);

  if (created == 0) {
    return 1;
  }

  created->sound = sound;
  created->gain = 1.0F;
  created->position.z = -1.0F;
  created->direction.z = -1.0F;
  created->binaural_tail = IPL_AUDIOEFFECTSTATE_TAILCOMPLETE;
  created->direct = (IPLDirectEffectParams){
    .flags = IPL_DIRECTEFFECTFLAGS_APPLYDISTANCEATTENUATION | IPL_DIRECTEFFECTFLAGS_APPLYOCCLUSION | IPL_DIRECTEFFECTFLAGS_APPLYTRANSMISSION,
    .transmissionType = IPL_TRANSMISSIONTYPE_FREQDEPENDENT,
    .distanceAttenuation = 1.0F,
    .occlusion = 1.0F,
    .transmission = {1.0F, 1.0F, 1.0F},
  };

  if (sound->channel_count == 1) {
    IPLAudioSettings audio_settings = {TI_AUDIO_SAMPLE_RATE, TI_AUDIO_FRAME_SIZE};
    IPLDirectEffectSettings direct_settings = {.numChannels = 1};
    IPLBinauralEffectSettings binaural_settings = {.hrtf = s_hrtf};
    IPLSourceSettings source_settings = {.flags = IPL_SIMULATIONFLAGS_DIRECT};

    if (iplDirectEffectCreate(g_audio.context, &audio_settings, &direct_settings, &created->direct_effect) != IPL_STATUS_SUCCESS ||
        iplBinauralEffectCreate(g_audio.context, &audio_settings, &binaural_settings, &created->binaural_effect) != IPL_STATUS_SUCCESS ||
        iplSourceCreate(g_audio.simulator, &source_settings, &created->simulation) != IPL_STATUS_SUCCESS) {
      iplSourceRelease(&created->simulation);
      iplBinauralEffectRelease(&created->binaural_effect);
      iplDirectEffectRelease(&created->direct_effect);
      TI_FREE(created);
      return 1;
    }

    iplSourceAdd(created->simulation, g_audio.simulator);
    iplSimulatorCommit(g_audio.simulator);
  }

  AcquireSRWLockExclusive(&s_lock);

  sound->source_count++;
  created->next = s_sources;
  s_sources = created;
  *source = created;

  ReleaseSRWLockExclusive(&s_lock);

  return 0;
}
void audio_source_destroy(ti_audio_source_t **source) {
  if (*source == 0) {
    return;
  }

  AcquireSRWLockExclusive(&s_lock);

  ti_audio_source_t *removed = *source;
  ti_audio_source_t **entry = &s_sources;

  while (*entry != removed) {
    entry = &(*entry)->next;
  }

  *entry = removed->next;
  removed->sound->source_count--;
  *source = 0;

  ReleaseSRWLockExclusive(&s_lock);

  if (removed->simulation != 0) {
    iplSourceRemove(removed->simulation, g_audio.simulator);
    iplSimulatorCommit(g_audio.simulator);
  }

  iplSourceRelease(&removed->simulation);
  iplBinauralEffectRelease(&removed->binaural_effect);
  iplDirectEffectRelease(&removed->direct_effect);
  TI_FREE(removed);
}
void audio_source_play(ti_audio_source_t *source) {
  if (source->simulation != 0) {
    audio_update();
  }

  AcquireSRWLockExclusive(&s_lock);

  if (source->state != TI_AUDIO_STATE_PAUSED) {
    source->cursor = 0;
    source->binaural_tail = IPL_AUDIOEFFECTSTATE_TAILCOMPLETE;

    if (source->direct_effect != 0) {
      iplDirectEffectReset(source->direct_effect);
      iplBinauralEffectReset(source->binaural_effect);
    }
  }

  source->state = TI_AUDIO_STATE_PLAYING;

  ReleaseSRWLockExclusive(&s_lock);
}
void audio_source_pause(ti_audio_source_t *source) {
  AcquireSRWLockExclusive(&s_lock);

  if (source->state == TI_AUDIO_STATE_PLAYING) {
    source->state = TI_AUDIO_STATE_PAUSED;
  }

  ReleaseSRWLockExclusive(&s_lock);
}
void audio_source_stop(ti_audio_source_t *source) {
  AcquireSRWLockExclusive(&s_lock);

  source->state = TI_AUDIO_STATE_STOPPED;
  source->cursor = 0;

  ReleaseSRWLockExclusive(&s_lock);
}
void audio_source_set(ti_audio_source_t *source, IPLVector3 *position, float gain, int loop) {
  AcquireSRWLockExclusive(&s_lock);

  source->position = *position;
  source->gain = gain;
  source->loop = loop;

  ReleaseSRWLockExclusive(&s_lock);
}
int audio_source_state(ti_audio_source_t *source) {
  AcquireSRWLockShared(&s_lock);

  int state = source->state;

  ReleaseSRWLockShared(&s_lock);

  return state;
}
void audio_listener_set(IPLCoordinateSpace3 *listener) {
  s_listener = *listener;
}
void audio_update(void) {
  if (g_audio.simulator == 0) {
    return;
  }

  IPLSimulationSharedInputs shared = {.listener = s_listener};
  iplSimulatorSetSharedInputs(g_audio.simulator, IPL_SIMULATIONFLAGS_DIRECT, &shared);

  ti_audio_source_t *source = s_sources;

  while (source != 0) {
    if (source->simulation != 0) {
      IPLSimulationInputs inputs = {
        .flags = IPL_SIMULATIONFLAGS_DIRECT,
        .directFlags = IPL_DIRECTSIMULATIONFLAGS_DISTANCEATTENUATION | IPL_DIRECTSIMULATIONFLAGS_OCCLUSION | IPL_DIRECTSIMULATIONFLAGS_TRANSMISSION,
        .source = {.right = {1, 0, 0}, .up = {0, 1, 0}, .ahead = {0, 0, -1}, .origin = source->position},
        .distanceAttenuationModel = {.type = IPL_DISTANCEATTENUATIONTYPE_DEFAULT},
        .occlusionType = IPL_OCCLUSIONTYPE_RAYCAST,
        .numOcclusionSamples = 1,
        .numTransmissionRays = 1,
      };

      iplSourceSetInputs(source->simulation, IPL_SIMULATIONFLAGS_DIRECT, &inputs);
    }

    source = source->next;
  }

  iplSimulatorRunDirect(g_audio.simulator);

  AcquireSRWLockExclusive(&s_lock);

  source = s_sources;

  while (source != 0) {
    if (source->simulation != 0) {
      IPLSimulationOutputs outputs = {0};
      iplSourceGetOutputs(source->simulation, IPL_SIMULATIONFLAGS_DIRECT, &outputs);

      source->direction = iplCalculateRelativeDirection(g_audio.context, source->position,
                                                        s_listener.origin, s_listener.ahead, s_listener.up);
      source->direct.distanceAttenuation = outputs.direct.distanceAttenuation;
      source->direct.occlusion = outputs.direct.occlusion;
      memcpy(source->direct.transmission, outputs.direct.transmission, sizeof(source->direct.transmission));
    }

    source = source->next;
  }

  ReleaseSRWLockExclusive(&s_lock);
}
void audio_render(float *output, uint32_t frame_count) {
  float input[TI_AUDIO_FRAME_SIZE];
  float filtered[TI_AUDIO_FRAME_SIZE];
  float left[TI_AUDIO_FRAME_SIZE];
  float right[TI_AUDIO_FRAME_SIZE];

  AcquireSRWLockExclusive(&s_lock);

  uint32_t written = 0;

  while (written < frame_count) {
    if (s_mix_cursor == TI_AUDIO_FRAME_SIZE) {
      memset(s_mix, 0, sizeof(s_mix));

      ti_audio_source_t *source = s_sources;

      while (source != 0) {
        if (source->state == TI_AUDIO_STATE_PLAYING) {
          ti_sound_t *sound = source->sound;
          uint32_t frame = 0;

          if (sound->channel_count == 2) {
            while (frame < TI_AUDIO_FRAME_SIZE) {
              if (source->cursor == sound->frame_count) {
                if (source->loop != 0) {
                  source->cursor = 0;
                } else {
                  source->state = TI_AUDIO_STATE_STOPPED;
                  break;
                }
              }

              s_mix[frame * 2] += sound->samples[source->cursor * 2] * source->gain;
              s_mix[frame * 2 + 1] += sound->samples[source->cursor * 2 + 1] * source->gain;
              source->cursor++;
              frame++;
            }
          } else {
            float *input_channels[] = {input};
            float *filtered_channels[] = {filtered};
            float *output_channels[] = {left, right};
            IPLAudioBuffer input_buffer = {1, TI_AUDIO_FRAME_SIZE, input_channels};
            IPLAudioBuffer filtered_buffer = {1, TI_AUDIO_FRAME_SIZE, filtered_channels};
            IPLAudioBuffer output_buffer = {2, TI_AUDIO_FRAME_SIZE, output_channels};
            IPLBinauralEffectParams params = {
              .direction = source->direction,
              .interpolation = IPL_HRTFINTERPOLATION_BILINEAR,
              .spatialBlend = 1.0F,
              .hrtf = s_hrtf,
            };

            if (source->cursor < sound->frame_count || source->loop != 0) {
              memset(input, 0, sizeof(input));

              while (frame < TI_AUDIO_FRAME_SIZE) {
                if (source->cursor == sound->frame_count) {
                  if (source->loop != 0) {
                    source->cursor = 0;
                  } else {
                    break;
                  }
                }

                input[frame] = sound->samples[source->cursor];
                source->cursor++;
                frame++;
              }

              iplDirectEffectApply(source->direct_effect, &source->direct, &input_buffer, &filtered_buffer);
              source->binaural_tail = iplBinauralEffectApply(source->binaural_effect, &params, &filtered_buffer, &output_buffer);
            } else if (source->binaural_tail == IPL_AUDIOEFFECTSTATE_TAILREMAINING) {
              source->binaural_tail = iplBinauralEffectGetTail(source->binaural_effect, &output_buffer);
            } else {
              source->state = TI_AUDIO_STATE_STOPPED;
            }

            if (source->state == TI_AUDIO_STATE_PLAYING) {
              frame = 0;

              while (frame < TI_AUDIO_FRAME_SIZE) {
                s_mix[frame * 2] += left[frame] * source->gain;
                s_mix[frame * 2 + 1] += right[frame] * source->gain;
                frame++;
              }
            }
          }
        }

        source = source->next;
      }

      s_mix_cursor = 0;
    }

    uint32_t count = TI_AUDIO_FRAME_SIZE - s_mix_cursor;

    if (count > frame_count - written) {
      count = frame_count - written;
    }

    memcpy(output + written * 2, s_mix + s_mix_cursor * 2, count * 2 * sizeof(float));
    s_mix_cursor += count;
    written += count;
  }

  ReleaseSRWLockExclusive(&s_lock);
}
