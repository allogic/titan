#ifndef TI_ALLOC_DEBUG_H
#define TI_ALLOC_DEBUG_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

void dmalloc_create(void);
void *dmalloc_alloc(char const *file_name, char const *function_name, uint64_t line_number, uint64_t data_size, uint32_t zero_data, void const *data_ref);
void dmalloc_free(char const *file_name, char const *function_name, uint64_t line_number, void *data);
void dmalloc_destroy(void);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // TI_ALLOC_DEBUG_H
