#ifndef TI_PL_WINDOW_H
#define TI_PL_WINDOW_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

void pl_window_create(pl_window_t *window);
void pl_window_run(pl_window_t *window);
void pl_window_destroy(pl_window_t *window);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // TI_PL_WINDOW_H
