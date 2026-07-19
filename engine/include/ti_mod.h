#ifndef MOD_H
#define MOD_H

#include <ti_heap.h>

#define DECLARE_MOD(REFERENCE)                                                 \
  mod_instance_t *mod_create(void) {                                           \
    return (mod_instance_t *)HEAP_ALLOC(sizeof(mod_instance_t), 0, REFERENCE); \
  }                                                                            \
  void mod_destroy(mod_instance_t *instance) {                                 \
    HEAP_FREE(instance);                                                       \
  }

typedef void (*on_create_proc_t)(void);
typedef void (*on_destroy_proc_t)(void);
typedef void (*on_update_proc_t)(float delta_time);

typedef struct mod_instance_t {
  on_create_proc_t on_create;
  on_destroy_proc_t on_destroy;
  on_update_proc_t on_update;
} mod_instance_t;

typedef mod_instance_t *(*mod_create_proc_t)(void);
typedef void (*mod_destroy_proc_t)(mod_instance_t *);

typedef struct mod_t {
  char const *file_path;
  void *module;
  mod_create_proc_t create_proc;
  mod_destroy_proc_t destroy_proc;
  mod_instance_t *instance;
} mod_t;

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

__declspec(dllexport) mod_instance_t *mod_create(void);
__declspec(dllexport) void mod_destroy(mod_instance_t *mod_instance);

void mod_load(mod_t *mod);
void mod_unload(mod_t *mod);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // MOD_H
