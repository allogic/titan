#ifndef TI_PL_EVENT_H
#define TI_PL_EVENT_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

uint8_t is_keyboard_key_pressed(pl_keyboard_key_t key);
uint8_t is_keyboard_key_held(pl_keyboard_key_t key);
uint8_t is_keyboard_key_released(pl_keyboard_key_t key);

uint8_t is_mouse_key_pressed(pl_mouse_key_t key);
uint8_t is_mouse_key_held(pl_mouse_key_t key);
uint8_t is_mouse_key_released(pl_mouse_key_t key);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // TI_PL_EVENT_H
