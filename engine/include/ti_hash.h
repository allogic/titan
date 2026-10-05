#ifndef TI_HASH_H
#define TI_HASH_H

#include <ti_engine.h>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

uint64_t mix64(uint64_t v);
uint64_t fnv1a64(char const *v);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // TI_HASH_H
