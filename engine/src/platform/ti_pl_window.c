#include <ti_pch.h>

static LRESULT window_message_proc(HWND window_handle, UINT window_message, WPARAM w_param, LPARAM l_param);

static char const *s_window_class = "TITAN_WND_CLASS";

void pl_window_create(pl_window_t *window) {
  window->is_first_frame = 1;
  window->window_border_width = 1;
  window->titlebar_height = 35;
  window->sidebar_width = 46;
  window->module_handle = GetModuleHandleA(0);

  WNDCLASSEX window_class_ex = {
    .cbSize = sizeof(WNDCLASSEX),
    .style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC,
    .lpfnWndProc = window_message_proc,
    .cbClsExtra = 0,
    .cbWndExtra = 0,
    .hInstance = window->module_handle,
    .hIcon = LoadIconA(0, IDI_APPLICATION),
    .hCursor = LoadCursorA(0, IDC_ARROW),
    .hbrBackground = (HBRUSH)(COLOR_WINDOW + 1),
    .lpszMenuName = 0,
    .lpszClassName = s_window_class,
    .hIconSm = LoadIconA(0, IDI_APPLICATION),
  };

  RegisterClassExA(&window_class_ex);

  INT screen_width = GetSystemMetrics(SM_CXSCREEN);
  INT screen_height = GetSystemMetrics(SM_CYSCREEN);
  INT window_position_x = (screen_width - window->window_width) / 2;
  INT window_position_y = (screen_height - window->window_height) / 2;

  window->window_handle = CreateWindowExA(
    0,
    s_window_class, window->window_title,
    WS_POPUP | WS_THICKFRAME,
    window_position_x, window_position_y,
    window->window_width, window->window_height,
    0,
    0,
    window->module_handle,
    window);

  ShowWindow(window->window_handle, SW_SHOW);

  vk_create();
}
void pl_window_run(pl_window_t *window) {
  QueryPerformanceFrequency(&window->time_freq);
  QueryPerformanceCounter(&window->time_prev);

  // TODO: refactor this as well..

  g_vk_main_framebuffer.width = 1;
  g_vk_main_framebuffer.height = 1;

  g_vk_imgui_framebuffer.width = g_pl_window.window_width;
  g_vk_imgui_framebuffer.height = g_pl_window.window_height;

  im_viewport_update(); // TODO: remove this and create a clear viewport API!

  while (window->is_running) {

    window->mouse_wheel_delta = 0;

    uint32_t keyboard_key_index = 0;
    uint32_t keyboard_key_count = KEYBOARD_KEY_COUNT;

    while (keyboard_key_index < keyboard_key_count) {

      if (window->keyboard_key_states[keyboard_key_index] == KEY_STATE_PRESSED) {
        window->keyboard_key_states[keyboard_key_index] = KEY_STATE_DOWN;
      } else if (window->keyboard_key_states[keyboard_key_index] == KEY_STATE_RELEASED) {
        window->keyboard_key_states[keyboard_key_index] = KEY_STATE_UP;
      }

      keyboard_key_index++;
    }

    uint32_t mouse_key_index = 0;
    uint32_t mouse_key_count = MOUSE_KEY_COUNT;

    while (mouse_key_index < mouse_key_count) {

      if (window->mouse_key_states[mouse_key_index] == KEY_STATE_PRESSED) {
        window->mouse_key_states[mouse_key_index] = KEY_STATE_DOWN;
      } else if (window->mouse_key_states[mouse_key_index] == KEY_STATE_RELEASED) {
        window->mouse_key_states[mouse_key_index] = KEY_STATE_UP;
      }

      mouse_key_index++;
    }

    MSG msg = {0};

    while (PeekMessageA(&msg, 0, 0, 0, PM_REMOVE)) {

      TranslateMessage(&msg);
      DispatchMessageA(&msg);
    }

    vk_renderer_draw(&g_vk_renderer);

    QueryPerformanceCounter(&window->time_curr);

    if (g_vk_main_framebuffer.is_dirty) {

      g_vk_main_framebuffer.is_dirty = 0;

      TI_VK_CHECK(vkQueueWaitIdle(g_vk_instance.primary_queue));
      TI_VK_CHECK(vkQueueWaitIdle(g_vk_instance.present_queue));

      vk_framebuffer_destroy(&g_vk_main_framebuffer);
      vk_framebuffer_create(&g_vk_main_framebuffer, &g_vk_main_renderpass, "asset/framebuffer/main.pak");

      im_viewport_update(); // TODO
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

      vk_update_surface_capabilities();

      vk_swapchain_create(&g_vk_swapchain, "asset/swapchain/main.pak");
      vk_renderer_create(&g_vk_renderer, "asset/renderer/main.pak");

      vk_renderpass_create(&g_vk_main_renderpass, "asset/renderpass/main.pak");
      vk_renderpass_create(&g_vk_imgui_renderpass, "asset/renderpass/imgui.pak");

      g_vk_imgui_framebuffer.width = g_pl_window.window_width;
      g_vk_imgui_framebuffer.height = g_pl_window.window_height;

      vk_framebuffer_create(&g_vk_main_framebuffer, &g_vk_main_renderpass, "asset/framebuffer/main.pak");
      vk_framebuffer_create(&g_vk_imgui_framebuffer, &g_vk_imgui_renderpass, "asset/framebuffer/imgui.pak");

      im_viewport_update(); // TODO
    }

    double time_freq = (double)window->time_freq.QuadPart;
    double time_prev = (double)window->time_prev.QuadPart;
    double time_curr = (double)window->time_curr.QuadPart;

    float delta_time = (float)((time_curr - time_prev) / time_freq);

    delta_time = clampf(delta_time, 0.0F, TI_WINDOW_MAX_DELTA_TIME);

    window->delta_time = delta_time;

    window->time_prev = window->time_curr;

    window->time += delta_time;
    window->elapsed_time_since_fps_count_update += delta_time;

    window->fps_counter++;

    g_vk_instance.frame_index++; // TODO: move this into vulkan instance..

    if ((window->elapsed_time_since_fps_count_update > 1.0F) || (window->is_first_frame)) {

      static char title_buffer[0x400] = {0};

      snprintf(title_buffer, sizeof(title_buffer), "%s %s.%s.%s (%s) - %d FPS",
               window->window_title,
               VERSION_MAJOR,
               VERSION_MINOR,
               VERSION_PATCH,
               GIT_VERSION_HASH,
               window->fps_counter);

      SetWindowTextA(window->window_handle, title_buffer);

      window->elapsed_time_since_fps_count_update = 0.0F;
      window->fps_counter = 0;
    }

    window->is_first_frame = 0;
  }
}
void pl_window_destroy(pl_window_t *window) {
  vk_destroy();

  DestroyWindow(window->window_handle);

  UnregisterClassA(s_window_class, window->module_handle);
}

static LRESULT window_message_proc(HWND window_handle, UINT window_message, WPARAM w_param, LPARAM l_param) {
  pl_window_t *window = (pl_window_t *)GetWindowLongPtr(window_handle, GWLP_USERDATA);

  im_message(window_handle, window_message, w_param, l_param);

  switch (window_message) {

    case WM_CREATE: {

      window->is_running = 1;

      break;
    }
    case WM_CLOSE: {

      window->is_running = 0;

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
          window->keyboard_key_states[KEYBOARD_KEY_LEFT_SHIFT] = ((window->keyboard_key_states[KEYBOARD_KEY_LEFT_SHIFT] == KEY_STATE_UP) || (window->keyboard_key_states[KEYBOARD_KEY_LEFT_SHIFT] == KEY_STATE_RELEASED)) ? KEY_STATE_PRESSED : KEY_STATE_DOWN;
          break;
        case KEYBOARD_KEY_RIGHT_SHIFT:
          window->keyboard_key_states[KEYBOARD_KEY_RIGHT_SHIFT] = ((window->keyboard_key_states[KEYBOARD_KEY_RIGHT_SHIFT] == KEY_STATE_UP) || (window->keyboard_key_states[KEYBOARD_KEY_RIGHT_SHIFT] == KEY_STATE_RELEASED)) ? KEY_STATE_PRESSED : KEY_STATE_DOWN;
          break;
        case KEYBOARD_KEY_LEFT_CONTROL:
          window->keyboard_key_states[KEYBOARD_KEY_LEFT_CONTROL] = ((window->keyboard_key_states[KEYBOARD_KEY_LEFT_CONTROL] == KEY_STATE_UP) || (window->keyboard_key_states[KEYBOARD_KEY_LEFT_CONTROL] == KEY_STATE_RELEASED)) ? KEY_STATE_PRESSED : KEY_STATE_DOWN;
          break;
        case KEYBOARD_KEY_RIGHT_CONTROL:
          window->keyboard_key_states[KEYBOARD_KEY_RIGHT_CONTROL] = ((window->keyboard_key_states[KEYBOARD_KEY_RIGHT_CONTROL] == KEY_STATE_UP) || (window->keyboard_key_states[KEYBOARD_KEY_RIGHT_CONTROL] == KEY_STATE_RELEASED)) ? KEY_STATE_PRESSED : KEY_STATE_DOWN;
          break;
        case KEYBOARD_KEY_LEFT_MENU:
          window->keyboard_key_states[KEYBOARD_KEY_LEFT_MENU] = ((window->keyboard_key_states[KEYBOARD_KEY_LEFT_MENU] == KEY_STATE_UP) || (window->keyboard_key_states[KEYBOARD_KEY_LEFT_MENU] == KEY_STATE_RELEASED)) ? KEY_STATE_PRESSED : KEY_STATE_DOWN;
          break;
        case KEYBOARD_KEY_RIGHT_MENU:
          window->keyboard_key_states[KEYBOARD_KEY_RIGHT_MENU] = ((window->keyboard_key_states[KEYBOARD_KEY_RIGHT_MENU] == KEY_STATE_UP) || (window->keyboard_key_states[KEYBOARD_KEY_RIGHT_MENU] == KEY_STATE_RELEASED)) ? KEY_STATE_PRESSED : KEY_STATE_DOWN;
          break;
        default:
          window->keyboard_key_states[virtual_key] = ((window->keyboard_key_states[virtual_key] == KEY_STATE_UP) || (window->keyboard_key_states[virtual_key] == KEY_STATE_RELEASED)) ? KEY_STATE_PRESSED : KEY_STATE_DOWN;
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
          window->keyboard_key_states[KEYBOARD_KEY_LEFT_SHIFT] = ((window->keyboard_key_states[KEYBOARD_KEY_LEFT_SHIFT] == KEY_STATE_DOWN) || (window->keyboard_key_states[KEYBOARD_KEY_LEFT_SHIFT] == KEY_STATE_PRESSED)) ? KEY_STATE_RELEASED : KEY_STATE_UP;
          break;
        case KEYBOARD_KEY_RIGHT_SHIFT:
          window->keyboard_key_states[KEYBOARD_KEY_RIGHT_SHIFT] = ((window->keyboard_key_states[KEYBOARD_KEY_RIGHT_SHIFT] == KEY_STATE_DOWN) || (window->keyboard_key_states[KEYBOARD_KEY_RIGHT_SHIFT] == KEY_STATE_PRESSED)) ? KEY_STATE_RELEASED : KEY_STATE_UP;
          break;
        case KEYBOARD_KEY_LEFT_CONTROL:
          window->keyboard_key_states[KEYBOARD_KEY_LEFT_CONTROL] = ((window->keyboard_key_states[KEYBOARD_KEY_LEFT_CONTROL] == KEY_STATE_DOWN) || (window->keyboard_key_states[KEYBOARD_KEY_LEFT_CONTROL] == KEY_STATE_PRESSED)) ? KEY_STATE_RELEASED : KEY_STATE_UP;
          break;
        case KEYBOARD_KEY_RIGHT_CONTROL:
          window->keyboard_key_states[KEYBOARD_KEY_RIGHT_CONTROL] = ((window->keyboard_key_states[KEYBOARD_KEY_RIGHT_CONTROL] == KEY_STATE_DOWN) || (window->keyboard_key_states[KEYBOARD_KEY_RIGHT_CONTROL] == KEY_STATE_PRESSED)) ? KEY_STATE_RELEASED : KEY_STATE_UP;
          break;
        case KEYBOARD_KEY_LEFT_MENU:
          window->keyboard_key_states[KEYBOARD_KEY_LEFT_MENU] = ((window->keyboard_key_states[KEYBOARD_KEY_LEFT_MENU] == KEY_STATE_DOWN) || (window->keyboard_key_states[KEYBOARD_KEY_LEFT_MENU] == KEY_STATE_PRESSED)) ? KEY_STATE_RELEASED : KEY_STATE_UP;
          break;
        case KEYBOARD_KEY_RIGHT_MENU:
          window->keyboard_key_states[KEYBOARD_KEY_RIGHT_MENU] = ((window->keyboard_key_states[KEYBOARD_KEY_RIGHT_MENU] == KEY_STATE_DOWN) || (window->keyboard_key_states[KEYBOARD_KEY_RIGHT_MENU] == KEY_STATE_PRESSED)) ? KEY_STATE_RELEASED : KEY_STATE_UP;
          break;
        default:
          window->keyboard_key_states[virtual_key] = ((window->keyboard_key_states[virtual_key] == KEY_STATE_DOWN) || (window->keyboard_key_states[virtual_key] == KEY_STATE_PRESSED)) ? KEY_STATE_RELEASED : KEY_STATE_UP;
          break;
      }

      break;
    }

    case WM_LBUTTONDOWN: {

      window->mouse_key_states[MOUSE_KEY_LEFT] = ((window->mouse_key_states[MOUSE_KEY_LEFT] == KEY_STATE_UP) || (window->mouse_key_states[MOUSE_KEY_LEFT] == KEY_STATE_RELEASED)) ? KEY_STATE_PRESSED : KEY_STATE_DOWN;

      break;
    }
    case WM_LBUTTONUP: {

      window->mouse_key_states[MOUSE_KEY_LEFT] = ((window->mouse_key_states[MOUSE_KEY_LEFT] == KEY_STATE_DOWN) || (window->mouse_key_states[MOUSE_KEY_LEFT] == KEY_STATE_PRESSED)) ? KEY_STATE_RELEASED : KEY_STATE_UP;

      break;
    }
    case WM_MBUTTONDOWN: {

      window->mouse_key_states[MOUSE_KEY_MIDDLE] = ((window->mouse_key_states[MOUSE_KEY_MIDDLE] == KEY_STATE_UP) || (window->mouse_key_states[MOUSE_KEY_MIDDLE] == KEY_STATE_RELEASED)) ? KEY_STATE_PRESSED : KEY_STATE_DOWN;

      break;
    }
    case WM_MBUTTONUP: {

      window->mouse_key_states[MOUSE_KEY_MIDDLE] = ((window->mouse_key_states[MOUSE_KEY_MIDDLE] == KEY_STATE_DOWN) || (window->mouse_key_states[MOUSE_KEY_MIDDLE] == KEY_STATE_PRESSED)) ? KEY_STATE_RELEASED : KEY_STATE_UP;

      break;
    }
    case WM_RBUTTONDOWN: {

      window->mouse_key_states[MOUSE_KEY_RIGHT] = ((window->mouse_key_states[MOUSE_KEY_RIGHT] == KEY_STATE_UP) || (window->mouse_key_states[MOUSE_KEY_RIGHT] == KEY_STATE_RELEASED)) ? KEY_STATE_PRESSED : KEY_STATE_DOWN;

      break;
    }
    case WM_RBUTTONUP: {

      window->mouse_key_states[MOUSE_KEY_RIGHT] = ((window->mouse_key_states[MOUSE_KEY_RIGHT] == KEY_STATE_DOWN) || (window->mouse_key_states[MOUSE_KEY_RIGHT] == KEY_STATE_PRESSED)) ? KEY_STATE_RELEASED : KEY_STATE_UP;

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

      window->mouse_position_x = mouse_x;
      window->mouse_position_y = mouse_y;

      break;
    }
    case WM_MOUSEWHEEL: {

      window->mouse_wheel_delta = GET_WHEEL_DELTA_WPARAM(w_param) / WHEEL_DELTA;

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

      BOOL left = mouse.x <= window->window_border_width;
      BOOL right = mouse.x >= (rect.right - window->window_border_width);
      BOOL top = (mouse.y <= window->window_border_width) || (mouse.y < border_size_y);
      BOOL bottom = mouse.y >= (rect.bottom - window->window_border_width);

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
