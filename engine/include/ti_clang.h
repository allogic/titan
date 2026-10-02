#ifndef TI_CLANG_H
#define TI_CLANG_H

#include <ti_engine.h>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

uint8_t clang_create(void);
uint8_t clang_compile(char const *source_code, void **buffer, uint64_t *buffer_size);
uint8_t clang_load(cl_module_t *module, void *buffer, uint64_t buffer_size);
uint8_t clang_unload(cl_module_t *module);
void clang_destroy(void);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // TI_CLANG_H
