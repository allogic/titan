#ifndef TI_FSUTIL_H
#define TI_FSUTIL_H

#include <ti_engine.h>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

bool32_t fsutil_load_text(uint8_t **buffer, uint64_t *buffer_size, char const *file_path);
bool32_t fsutil_load_binary(uint8_t **buffer, uint64_t *buffer_size, char const *file_path);

bool32_t fsutil_store_text(uint8_t *buffer, uint64_t buffer_size, char const *file_path);
bool32_t fsutil_store_binary(uint8_t *buffer, uint64_t buffer_size, char const *file_path);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // TI_FSUTIL_H
