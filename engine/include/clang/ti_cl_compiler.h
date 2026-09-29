#ifndef TI_CL_COMPILER_H
#define TI_CL_COMPILER_H

#ifdef __cplusplus
extern "C" {
#endif

extern map_t g_cl_loaded_modules;

uint8_t cl_compiler_create(void);
uint8_t cl_compiler_compile(char const *source_code, void **buffer, uint64_t *buffer_size);
uint8_t cl_compiler_load(cl_module_t *module, void *buffer, uint64_t buffer_size);
uint8_t cl_compiler_unload(cl_module_t *module);
void cl_compiler_destroy(void);

#ifdef __cplusplus
}
#endif

#endif // TI_CL_COMPILER_H
