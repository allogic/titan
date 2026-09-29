#ifndef TI_CL_FWD_H
#define TI_CL_FWD_H

typedef void (*on_create_proc_t)(void);
typedef void (*on_play_proc_t)(void);
typedef void (*on_stop_proc_t)(void);
typedef void (*on_destroy_proc_t)(void);

typedef struct cl_module_t {
  on_create_proc_t on_create_proc;
  on_play_proc_t on_play_proc;
  on_stop_proc_t on_stop_proc;
  on_destroy_proc_t on_destroy_proc;
} cl_module_t;

#endif // TI_CL_FWD_H
