#ifndef TI_PL_WINDOW_H
#define TI_PL_WINDOW_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

extern pl_window_t g_window;

void window_create(uint32_t width, uint32_t height, char const *title);
void window_run(void);
void window_destroy(void);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // TI_PL_WINDOW_H
