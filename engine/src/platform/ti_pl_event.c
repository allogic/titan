#include <ti_pch.h>

uint8_t is_keyboard_key_pressed(pl_keyboard_key_t key) {
  return g_pl_window.keyboard_key_states[key] == KEY_STATE_PRESSED;
}
uint8_t is_keyboard_key_held(pl_keyboard_key_t key) {
  return (g_pl_window.keyboard_key_states[key] == KEY_STATE_DOWN) || (g_pl_window.keyboard_key_states[key] == KEY_STATE_PRESSED);
}
uint8_t is_keyboard_key_released(pl_keyboard_key_t key) {
  return g_pl_window.keyboard_key_states[key] == KEY_STATE_RELEASED;
}

uint8_t is_mouse_key_pressed(pl_mouse_key_t key) {
  return g_pl_window.mouse_key_states[key] == KEY_STATE_PRESSED;
}
uint8_t is_mouse_key_held(pl_mouse_key_t key) {
  return (g_pl_window.mouse_key_states[key] == KEY_STATE_DOWN) || (g_pl_window.mouse_key_states[key] == KEY_STATE_PRESSED);
}
uint8_t is_mouse_key_released(pl_mouse_key_t key) {
  return g_pl_window.mouse_key_states[key] == KEY_STATE_RELEASED;
}
