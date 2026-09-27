#ifndef TI_CLANG_H
#define TI_CLANG_H

#ifdef __cplusplus
extern "C" {
#endif

void ti_clang_create(void);
void ti_clang_compile(char const *source_code);
void ti_clang_destroy(void);

#ifdef __cplusplus
}
#endif

#endif // TI_CLANG_H
