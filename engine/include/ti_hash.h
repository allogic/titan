#ifndef TI_HASH_H
#define TI_HASH_H

#include <ti_engine.h>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

uint64_t mix64(uint64_t v);
uint64_t fnv1a64(uint64_t value, uint8_t *buffer, uint64_t size);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // TI_HASH_H
