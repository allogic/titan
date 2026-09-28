#ifndef TI_CLANG_H
#define TI_CLANG_H

#ifdef __cplusplus
extern "C" {
#endif

void clang_create(void);
void clang_compile(char const *source_code);
void clang_destroy(void);

#ifdef __cplusplus
}
#endif

#endif // TI_CLANG_H
