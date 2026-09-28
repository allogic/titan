#ifndef TI_CLANG_H
#define TI_CLANG_H

#ifdef __cplusplus
extern "C" {
#endif

uint8_t clang_create(void);
uint8_t clang_compile(char const *source_code, void **buffer, uint64_t *buffer_size);
uint8_t clang_load(void *buffer, uint64_t buffer_size);
uint8_t clang_lookup(char const *symbol, void **function_ptr);
void clang_destroy(void);

#ifdef __cplusplus
}
#endif

#endif // TI_CLANG_H
