#ifndef TI_IM_OUTPUT_H
#define TI_IM_OUTPUT_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

void im_output_push(char const *format, ...);
void im_output_clear(void);
void im_output_draw(void);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // TI_IM_OUTPUT_H
