#include <platform/windows/ti_window.h>

static LRESULT message_proc(HWND window_handle, UINT window_message, WPARAM w_param, LPARAM l_param);

// TODO: config this bitch..
window_t g_window = {
  .width = 1920,
  .height = 1080,
  .title = "TITAN",
  .class_name = "TITAN_WND_CLASS",
  .h_border_padding = 7,
  .v_border_padding = 10,
  .titlebar_height = 35,
};

void window_create(void) {
  g_window.is_first_frame = 1;
  g_window.window_border_width = 1;
  g_window.titlebar_height = 35;
  g_window.sidebar_width = 46;
  g_window.module_handle = GetModuleHandleA(0);

  WNDCLASSEX window_class_ex = {
    .cbSize = sizeof(WNDCLASSEX),
    .style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC,
    .lpfnWndProc = message_proc,
    .cbClsExtra = 0,
    .cbWndExtra = 0,
    .hInstance = g_window.module_handle,
    .hIcon = LoadIconA(0, IDI_APPLICATION),
    .hCursor = LoadCursorA(0, IDC_ARROW),
    .hbrBackground = (HBRUSH)(COLOR_WINDOW + 1),
    .lpszMenuName = 0,
    .lpszClassName = g_window.class_name,
    .hIconSm = LoadIconA(0, IDI_APPLICATION),
  };

  RegisterClassExA(&window_class_ex);

  INT screen_width = GetSystemMetrics(SM_CXSCREEN);
  INT screen_height = GetSystemMetrics(SM_CYSCREEN);
  INT window_position_x = (screen_width - g_window.width) / 2;
  INT window_position_y = (screen_height - g_window.height) / 2;

  g_window.window_handle = CreateWindowExA(
    0,
    g_window.class_name,
    g_window.title,
    WS_POPUP | WS_THICKFRAME,
    window_position_x, window_position_y,
    g_window.width, g_window.height,
    0,
    0,
    g_window.module_handle,
    0);

  ShowWindow(g_window.window_handle, SW_SHOW);

  vk_context_create();
}
void window_run(void) {
  QueryPerformanceFrequency((PLARGE_INTEGER)&g_window.time_freq);
  QueryPerformanceCounter((PLARGE_INTEGER)&g_window.time_prev);

  if (g_window.editor_viewport_update_proc) {
    g_window.editor_viewport_update_proc(&g_vk_viewport);
  }

  while (g_window.is_running) {

    g_window.mouse_wheel_delta = 0;

    uint32_t keyboard_key_index = 0;
    uint32_t keyboard_key_count = KEYBOARD_KEY_COUNT;

    while (keyboard_key_index < keyboard_key_count) {

      if (g_window.keyboard_key_states[keyboard_key_index] == KEY_STATE_PRESSED) {
        g_window.keyboard_key_states[keyboard_key_index] = KEY_STATE_DOWN;
      } else if (g_window.keyboard_key_states[keyboard_key_index] == KEY_STATE_RELEASED) {
        g_window.keyboard_key_states[keyboard_key_index] = KEY_STATE_UP;
      }

      keyboard_key_index++;
    }

    uint32_t mouse_key_index = 0;
    uint32_t mouse_key_count = MOUSE_KEY_COUNT;

    while (mouse_key_index < mouse_key_count) {

      if (g_window.mouse_key_states[mouse_key_index] == KEY_STATE_PRESSED) {
        g_window.mouse_key_states[mouse_key_index] = KEY_STATE_DOWN;
      } else if (g_window.mouse_key_states[mouse_key_index] == KEY_STATE_RELEASED) {
        g_window.mouse_key_states[mouse_key_index] = KEY_STATE_UP;
      }

      mouse_key_index++;
    }

    MSG msg = {0};

    while (PeekMessageA(&msg, 0, 0, 0, PM_REMOVE)) {

      TranslateMessage(&msg);
      DispatchMessageA(&msg);
    }

    scene_update(&g_scene); // TODO
    audio_update();         // TODO
    ph_world_update(g_window.delta_time);

    fvec3_t center = {0.0F, 0.0F, 0.0F};
    fvec3_t right = {1.0F, 0.0F, 0.0F};
    fvec3_t up = {0.0F, 1.0F, 0.0F};
    fvec3_t forward = {0.0F, 0.0F, 1.0F};
    fvec4_t red = {1.0F, 0.0F, 0.0F, 1.0F};
    fvec4_t green = {0.0F, 1.0F, 0.0F, 1.0F};
    fvec4_t blue = {0.0F, 0.0F, 1.0F, 1.0F};

    vk_renderer_draw_debug_line(&g_vk_renderer, center, right, red);
    vk_renderer_draw_debug_line(&g_vk_renderer, center, up, green);
    vk_renderer_draw_debug_line(&g_vk_renderer, center, forward, blue);

    vk_renderer_draw(&g_vk_renderer);

    QueryPerformanceCounter((PLARGE_INTEGER)&g_window.time_curr);

    if (g_vk_main_framebuffer.is_dirty) {

      g_vk_main_framebuffer.is_dirty = 0;

      TI_VK_CHECK(vkQueueWaitIdle(g_vk_instance.primary_queue));
      TI_VK_CHECK(vkQueueWaitIdle(g_vk_instance.present_queue));

      vk_framebuffer_destroy(&g_vk_main_framebuffer);
      vk_framebuffer_create(&g_vk_main_framebuffer, &g_vk_main_renderpass, g_vk_viewport.width, g_vk_viewport.height, "asset/framebuffer/main.pak");

      if (g_window.editor_viewport_update_proc) {
        g_window.editor_viewport_update_proc(&g_vk_viewport);
      }
    }

    if (g_vk_swapchain.is_dirty) {

      g_vk_swapchain.is_dirty = 0;

      TI_VK_CHECK(vkQueueWaitIdle(g_vk_instance.primary_queue));
      TI_VK_CHECK(vkQueueWaitIdle(g_vk_instance.present_queue));

      vk_framebuffer_destroy(&g_vk_imgui_framebuffer);
      vk_framebuffer_destroy(&g_vk_main_framebuffer);

      vk_renderpass_destroy(&g_vk_imgui_renderpass);
      vk_renderpass_destroy(&g_vk_main_renderpass);

      vk_renderer_destroy(&g_vk_renderer);
      vk_swapchain_destroy(&g_vk_swapchain);

      vk_context_update_surface_capabilities();

      vk_swapchain_create(&g_vk_swapchain, "asset/swapchain/main.pak");

      vk_renderpass_create(&g_vk_main_renderpass, "asset/renderpass/main.pak");
      vk_renderpass_create(&g_vk_imgui_renderpass, "asset/renderpass/imgui.pak");

      vk_renderer_create(&g_vk_renderer, "asset/renderer/main.pak");

      vk_framebuffer_create(&g_vk_main_framebuffer, &g_vk_main_renderpass, g_vk_viewport.width, g_vk_viewport.height, "asset/framebuffer/main.pak");
      vk_framebuffer_create(&g_vk_imgui_framebuffer, &g_vk_imgui_renderpass, g_window.width, g_window.height, "asset/framebuffer/imgui.pak");

      if (g_window.editor_viewport_update_proc) {
        g_window.editor_viewport_update_proc(&g_vk_viewport);
      }
    }

    double time_freq = (double)g_window.time_freq;
    double time_prev = (double)g_window.time_prev;
    double time_curr = (double)g_window.time_curr;

    float delta_time = (float)((time_curr - time_prev) / time_freq);

    delta_time = clampf(delta_time, 0.0F, TI_WINDOW_MAX_DELTA_TIME);

    g_window.delta_time = delta_time;
    g_window.time_prev = g_window.time_curr;
    g_window.time += delta_time;
    g_window.elapsed_time_since_fps_count_update += delta_time;
    g_window.fps_counter++;

    if ((g_window.elapsed_time_since_fps_count_update > 1.0F) || (g_window.is_first_frame)) {

      g_window.elapsed_time_since_fps_count_update = 0.0F;
      g_window.final_fps_counter = g_window.fps_counter;
      g_window.fps_counter = 0;
    }

    g_window.is_first_frame = 0;
  }
}
void window_destroy(void) {
  vk_context_destroy();

  DestroyWindow(g_window.window_handle);

  UnregisterClassA(g_window.class_name, g_window.module_handle);
}

uint8_t is_keyboard_key_pressed(keyboard_key_t key) {
  return g_window.keyboard_key_states[key] == KEY_STATE_PRESSED;
}
uint8_t is_keyboard_key_held(keyboard_key_t key) {
  return (g_window.keyboard_key_states[key] == KEY_STATE_DOWN) || (g_window.keyboard_key_states[key] == KEY_STATE_PRESSED);
}
uint8_t is_keyboard_key_released(keyboard_key_t key) {
  return g_window.keyboard_key_states[key] == KEY_STATE_RELEASED;
}

uint8_t is_mouse_key_pressed(mouse_key_t key) {
  return g_window.mouse_key_states[key] == KEY_STATE_PRESSED;
}
uint8_t is_mouse_key_held(mouse_key_t key) {
  return (g_window.mouse_key_states[key] == KEY_STATE_DOWN) || (g_window.mouse_key_states[key] == KEY_STATE_PRESSED);
}
uint8_t is_mouse_key_released(mouse_key_t key) {
  return g_window.mouse_key_states[key] == KEY_STATE_RELEASED;
}

static LRESULT message_proc(HWND window_handle, UINT window_message, WPARAM w_param, LPARAM l_param) {
  if (g_window.editor_message_proc) {
    g_window.editor_message_proc(window_handle, window_message, w_param, l_param);
  }

  switch (window_message) {

    case WM_CREATE: {

      g_window.is_running = 1;

      break;
    }
    case WM_CLOSE: {

      g_window.is_running = 0;

      break;
    }

    case WM_NCCREATE: {

      SetWindowLongPtr(window_handle, GWLP_USERDATA, (LONG_PTR)((CREATESTRUCT *)l_param)->lpCreateParams);

      return 1;
    }
    case WM_NCDESTROY: {

      SetWindowLongPtr(window_handle, GWLP_USERDATA, 0);

      break;
    }

    case WM_KEYDOWN:
    case WM_SYSKEYDOWN: {

      UINT scan_code = MapVirtualKeyA((UINT)w_param, MAPVK_VK_TO_VSC);
      UINT virtual_key = MapVirtualKeyExA(scan_code, MAPVK_VSC_TO_VK_EX, GetKeyboardLayout(0));

      switch (virtual_key) {

        case KEYBOARD_KEY_LEFT_SHIFT:
          g_window.keyboard_key_states[KEYBOARD_KEY_LEFT_SHIFT] = ((g_window.keyboard_key_states[KEYBOARD_KEY_LEFT_SHIFT] == KEY_STATE_UP) || (g_window.keyboard_key_states[KEYBOARD_KEY_LEFT_SHIFT] == KEY_STATE_RELEASED)) ? KEY_STATE_PRESSED : KEY_STATE_DOWN;
          break;
        case KEYBOARD_KEY_RIGHT_SHIFT:
          g_window.keyboard_key_states[KEYBOARD_KEY_RIGHT_SHIFT] = ((g_window.keyboard_key_states[KEYBOARD_KEY_RIGHT_SHIFT] == KEY_STATE_UP) || (g_window.keyboard_key_states[KEYBOARD_KEY_RIGHT_SHIFT] == KEY_STATE_RELEASED)) ? KEY_STATE_PRESSED : KEY_STATE_DOWN;
          break;
        case KEYBOARD_KEY_LEFT_CONTROL:
          g_window.keyboard_key_states[KEYBOARD_KEY_LEFT_CONTROL] = ((g_window.keyboard_key_states[KEYBOARD_KEY_LEFT_CONTROL] == KEY_STATE_UP) || (g_window.keyboard_key_states[KEYBOARD_KEY_LEFT_CONTROL] == KEY_STATE_RELEASED)) ? KEY_STATE_PRESSED : KEY_STATE_DOWN;
          break;
        case KEYBOARD_KEY_RIGHT_CONTROL:
          g_window.keyboard_key_states[KEYBOARD_KEY_RIGHT_CONTROL] = ((g_window.keyboard_key_states[KEYBOARD_KEY_RIGHT_CONTROL] == KEY_STATE_UP) || (g_window.keyboard_key_states[KEYBOARD_KEY_RIGHT_CONTROL] == KEY_STATE_RELEASED)) ? KEY_STATE_PRESSED : KEY_STATE_DOWN;
          break;
        case KEYBOARD_KEY_LEFT_MENU:
          g_window.keyboard_key_states[KEYBOARD_KEY_LEFT_MENU] = ((g_window.keyboard_key_states[KEYBOARD_KEY_LEFT_MENU] == KEY_STATE_UP) || (g_window.keyboard_key_states[KEYBOARD_KEY_LEFT_MENU] == KEY_STATE_RELEASED)) ? KEY_STATE_PRESSED : KEY_STATE_DOWN;
          break;
        case KEYBOARD_KEY_RIGHT_MENU:
          g_window.keyboard_key_states[KEYBOARD_KEY_RIGHT_MENU] = ((g_window.keyboard_key_states[KEYBOARD_KEY_RIGHT_MENU] == KEY_STATE_UP) || (g_window.keyboard_key_states[KEYBOARD_KEY_RIGHT_MENU] == KEY_STATE_RELEASED)) ? KEY_STATE_PRESSED : KEY_STATE_DOWN;
          break;
        default:
          g_window.keyboard_key_states[virtual_key] = ((g_window.keyboard_key_states[virtual_key] == KEY_STATE_UP) || (g_window.keyboard_key_states[virtual_key] == KEY_STATE_RELEASED)) ? KEY_STATE_PRESSED : KEY_STATE_DOWN;
          break;
      }

      break;
    }

    case WM_KEYUP:
    case WM_SYSKEYUP: {

      UINT scan_code = MapVirtualKeyA((UINT)w_param, MAPVK_VK_TO_VSC);
      UINT virtual_key = MapVirtualKeyExA(scan_code, MAPVK_VSC_TO_VK_EX, GetKeyboardLayout(0));

      switch (virtual_key) {

        case KEYBOARD_KEY_LEFT_SHIFT:
          g_window.keyboard_key_states[KEYBOARD_KEY_LEFT_SHIFT] = ((g_window.keyboard_key_states[KEYBOARD_KEY_LEFT_SHIFT] == KEY_STATE_DOWN) || (g_window.keyboard_key_states[KEYBOARD_KEY_LEFT_SHIFT] == KEY_STATE_PRESSED)) ? KEY_STATE_RELEASED : KEY_STATE_UP;
          break;
        case KEYBOARD_KEY_RIGHT_SHIFT:
          g_window.keyboard_key_states[KEYBOARD_KEY_RIGHT_SHIFT] = ((g_window.keyboard_key_states[KEYBOARD_KEY_RIGHT_SHIFT] == KEY_STATE_DOWN) || (g_window.keyboard_key_states[KEYBOARD_KEY_RIGHT_SHIFT] == KEY_STATE_PRESSED)) ? KEY_STATE_RELEASED : KEY_STATE_UP;
          break;
        case KEYBOARD_KEY_LEFT_CONTROL:
          g_window.keyboard_key_states[KEYBOARD_KEY_LEFT_CONTROL] = ((g_window.keyboard_key_states[KEYBOARD_KEY_LEFT_CONTROL] == KEY_STATE_DOWN) || (g_window.keyboard_key_states[KEYBOARD_KEY_LEFT_CONTROL] == KEY_STATE_PRESSED)) ? KEY_STATE_RELEASED : KEY_STATE_UP;
          break;
        case KEYBOARD_KEY_RIGHT_CONTROL:
          g_window.keyboard_key_states[KEYBOARD_KEY_RIGHT_CONTROL] = ((g_window.keyboard_key_states[KEYBOARD_KEY_RIGHT_CONTROL] == KEY_STATE_DOWN) || (g_window.keyboard_key_states[KEYBOARD_KEY_RIGHT_CONTROL] == KEY_STATE_PRESSED)) ? KEY_STATE_RELEASED : KEY_STATE_UP;
          break;
        case KEYBOARD_KEY_LEFT_MENU:
          g_window.keyboard_key_states[KEYBOARD_KEY_LEFT_MENU] = ((g_window.keyboard_key_states[KEYBOARD_KEY_LEFT_MENU] == KEY_STATE_DOWN) || (g_window.keyboard_key_states[KEYBOARD_KEY_LEFT_MENU] == KEY_STATE_PRESSED)) ? KEY_STATE_RELEASED : KEY_STATE_UP;
          break;
        case KEYBOARD_KEY_RIGHT_MENU:
          g_window.keyboard_key_states[KEYBOARD_KEY_RIGHT_MENU] = ((g_window.keyboard_key_states[KEYBOARD_KEY_RIGHT_MENU] == KEY_STATE_DOWN) || (g_window.keyboard_key_states[KEYBOARD_KEY_RIGHT_MENU] == KEY_STATE_PRESSED)) ? KEY_STATE_RELEASED : KEY_STATE_UP;
          break;
        default:
          g_window.keyboard_key_states[virtual_key] = ((g_window.keyboard_key_states[virtual_key] == KEY_STATE_DOWN) || (g_window.keyboard_key_states[virtual_key] == KEY_STATE_PRESSED)) ? KEY_STATE_RELEASED : KEY_STATE_UP;
          break;
      }

      break;
    }

    case WM_LBUTTONDOWN: {

      g_window.mouse_key_states[MOUSE_KEY_LEFT] = ((g_window.mouse_key_states[MOUSE_KEY_LEFT] == KEY_STATE_UP) || (g_window.mouse_key_states[MOUSE_KEY_LEFT] == KEY_STATE_RELEASED)) ? KEY_STATE_PRESSED : KEY_STATE_DOWN;

      break;
    }
    case WM_LBUTTONUP: {

      g_window.mouse_key_states[MOUSE_KEY_LEFT] = ((g_window.mouse_key_states[MOUSE_KEY_LEFT] == KEY_STATE_DOWN) || (g_window.mouse_key_states[MOUSE_KEY_LEFT] == KEY_STATE_PRESSED)) ? KEY_STATE_RELEASED : KEY_STATE_UP;

      break;
    }
    case WM_MBUTTONDOWN: {

      g_window.mouse_key_states[MOUSE_KEY_MIDDLE] = ((g_window.mouse_key_states[MOUSE_KEY_MIDDLE] == KEY_STATE_UP) || (g_window.mouse_key_states[MOUSE_KEY_MIDDLE] == KEY_STATE_RELEASED)) ? KEY_STATE_PRESSED : KEY_STATE_DOWN;

      break;
    }
    case WM_MBUTTONUP: {

      g_window.mouse_key_states[MOUSE_KEY_MIDDLE] = ((g_window.mouse_key_states[MOUSE_KEY_MIDDLE] == KEY_STATE_DOWN) || (g_window.mouse_key_states[MOUSE_KEY_MIDDLE] == KEY_STATE_PRESSED)) ? KEY_STATE_RELEASED : KEY_STATE_UP;

      break;
    }
    case WM_RBUTTONDOWN: {

      g_window.mouse_key_states[MOUSE_KEY_RIGHT] = ((g_window.mouse_key_states[MOUSE_KEY_RIGHT] == KEY_STATE_UP) || (g_window.mouse_key_states[MOUSE_KEY_RIGHT] == KEY_STATE_RELEASED)) ? KEY_STATE_PRESSED : KEY_STATE_DOWN;

      break;
    }
    case WM_RBUTTONUP: {

      g_window.mouse_key_states[MOUSE_KEY_RIGHT] = ((g_window.mouse_key_states[MOUSE_KEY_RIGHT] == KEY_STATE_DOWN) || (g_window.mouse_key_states[MOUSE_KEY_RIGHT] == KEY_STATE_PRESSED)) ? KEY_STATE_RELEASED : KEY_STATE_UP;

      break;
    }
    case WM_LBUTTONDBLCLK: {

      break;
    }
    case WM_MBUTTONDBLCLK: {

      break;
    }
    case WM_RBUTTONDBLCLK: {

      break;
    }

    case WM_MOUSEMOVE: {

      INT mouse_x = LOWORD(l_param);
      INT mouse_y = HIWORD(l_param);

      g_window.mouse_position_x = mouse_x;
      g_window.mouse_position_y = mouse_y;

      break;
    }
    case WM_MOUSEWHEEL: {

      g_window.mouse_wheel_delta = GET_WHEEL_DELTA_WPARAM(w_param) / WHEEL_DELTA;

      break;
    }

    case WM_NCCALCSIZE: {

      INT border_size_x = GetSystemMetrics(SM_CXFRAME);
      INT border_size_y = GetSystemMetrics(SM_CYFRAME);

      NCCALCSIZE_PARAMS *calc_size_params = (NCCALCSIZE_PARAMS *)l_param;
      RECT *requested_client_rect = calc_size_params->rgrc;

      requested_client_rect->right -= border_size_x;
      requested_client_rect->left += border_size_x;
      requested_client_rect->bottom -= border_size_y;
      requested_client_rect->top += 0;

      return WVR_ALIGNLEFT | WVR_ALIGNTOP;
    }
    case WM_NCHITTEST: {

      INT mouse_x = LOWORD(l_param);
      INT mouse_y = HIWORD(l_param);
      INT border_size_y = GetSystemMetrics(SM_CYFRAME);

      POINT mouse = {mouse_x, mouse_y};
      ScreenToClient(window_handle, &mouse);

      RECT rect = {0};
      GetClientRect(window_handle, &rect);

      BOOL left = mouse.x <= g_window.window_border_width;
      BOOL right = mouse.x >= (rect.right - g_window.window_border_width);
      BOOL top = (mouse.y <= g_window.window_border_width) || (mouse.y < border_size_y);
      BOOL bottom = mouse.y >= (rect.bottom - g_window.window_border_width);

      if (top && left) {
        return HTTOPLEFT;
      }
      if (top && right) {
        return HTTOPRIGHT;
      }
      if (bottom && left) {
        return HTBOTTOMLEFT;
      }
      if (bottom && right) {
        return HTBOTTOMRIGHT;
      }

      if (left) {
        return HTLEFT;
      }
      if (right) {
        return HTRIGHT;
      }
      if (top) {
        return HTTOP;
      }
      if (bottom) {
        return HTBOTTOM;
      }

      return HTCLIENT;
    }

    default: {

      return DefWindowProcA(window_handle, window_message, w_param, l_param);
    }
  }

  return 1;
}
