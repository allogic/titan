#include <ti_pch.h>

#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_vulkan.h>
#include <imgui_internal.h>

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND window_handle, UINT window_message, WPARAM w_param, LPARAM l_param);

static int32_t Platform_CreateVkSurface(ImGuiViewport *vp, ImU64 vk_inst, const void *vk_allocators, ImU64 *out_vk_surface);

static void build_layout(void);

static VkDescriptorPool s_descriptor_pool = 0;

static VkDescriptorPoolSize s_descriptor_pool_sizes[] = {
  {VK_DESCRIPTOR_TYPE_SAMPLER, 1000},
  {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000},
  {VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000},
  {VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000},
  {VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000},
  {VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000},
  {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000},
  {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000},
  {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000},
  {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000},
  {VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000},
};

static uint8_t s_layout_init = 1;

uint8_t g_im_show_left_panel = 1;
uint8_t g_im_show_right_panel = 1;

void *g_im_font_default_16 = 0;

void *g_im_font_symbols_16 = 0;
void *g_im_font_symbols_18 = 0;
void *g_im_font_symbols_22 = 0;
void *g_im_font_symbols_32 = 0;

void im_create(void) {
  VkDescriptorPoolCreateInfo descriptor_pool_create_info = {
    .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
    .flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT,
    .maxSets = 1000,
    .poolSizeCount = TI_ARRAY_COUNT(s_descriptor_pool_sizes),
    .pPoolSizes = s_descriptor_pool_sizes,
  };

  TI_VK_CHECK(vkCreateDescriptorPool(g_vk_instance.device, &descriptor_pool_create_info, 0, &s_descriptor_pool));

  IMGUI_CHECKVERSION();

  ImGui::CreateContext();

  ImGuiIO &io = ImGui::GetIO();
  ImGuiPlatformIO &platform_io = ImGui::GetPlatformIO();
  ImGuiStyle &style = ImGui::GetStyle();

  ImFontConfig font_config = {};

  font_config.FontDataOwnedByAtlas = 0;
  font_config.OversampleH = 2;
  font_config.OversampleV = 2;
  font_config.PixelSnapH = 1;

  ImWchar icon_glyph_ranges[] = {
    (ImWchar)ICON_MIN_MS,
    (ImWchar)ICON_MAX_MS,
    0,
  };

  fs_asset_t commit_mono_asset = {
    .path = "asset/font/commit_mono_latin_400_normal.pak",
  };
  fs_asset_t material_symbol_asset = {
    .path = "asset/font/material_symbols_rounded_fill.pak",
  };

  fs_asset_load(&commit_mono_asset);
  fs_asset_load(&material_symbol_asset);

  fs_font_t *commit_mono = (fs_font_t *)commit_mono_asset.instance;
  fs_font_t *material_symbols = (fs_font_t *)material_symbol_asset.instance;

  if (commit_mono) {

    g_im_font_default_16 = io.Fonts->AddFontFromMemoryTTF(commit_mono->buffer, (int32_t)commit_mono->buffer_size, 16.0F, &font_config, 0);
  }

  if (material_symbols) {

    g_im_font_symbols_16 = io.Fonts->AddFontFromMemoryTTF(material_symbols->buffer, (int32_t)material_symbols->buffer_size, 16.0F, &font_config, icon_glyph_ranges);
    g_im_font_symbols_18 = io.Fonts->AddFontFromMemoryTTF(material_symbols->buffer, (int32_t)material_symbols->buffer_size, 18.0F, &font_config, icon_glyph_ranges);
    g_im_font_symbols_22 = io.Fonts->AddFontFromMemoryTTF(material_symbols->buffer, (int32_t)material_symbols->buffer_size, 26.0F, &font_config, icon_glyph_ranges);
    g_im_font_symbols_32 = io.Fonts->AddFontFromMemoryTTF(material_symbols->buffer, (int32_t)material_symbols->buffer_size, 32.0F, &font_config, icon_glyph_ranges);
  }

  io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
  io.ConfigWindowsMoveFromTitleBarOnly = 1;

  platform_io.Platform_CreateVkSurface = Platform_CreateVkSurface;

  style.Alpha = 1.0F;
  style.DisabledAlpha = 1.0F;
  style.WindowPadding = ImVec2(5.0F, 5.0F);
  style.WindowRounding = 0.0F;
  style.WindowBorderSize = 0.0F;
  style.WindowTitleAlign = ImVec2(0.0F, 0.5F);
  style.WindowMenuButtonPosition = ImGuiDir_None;
  style.ChildRounding = 10.0F;
  style.ChildBorderSize = 0.0F;
  style.PopupRounding = 0.0F;
  style.ItemSpacing = ImVec2(5.0F, 5.0F);
  style.ItemInnerSpacing = ImVec2(5.0F, 5.0F);
  style.FrameRounding = 1.0F;
  style.FramePadding = ImVec2(5.0F, 5.0F);
  style.ScrollbarSize = 12.0F;
  style.ScrollbarRounding = 2.5F;
  style.GrabMinSize = 20.0F;
  style.GrabRounding = 0.0F;
  style.TabRounding = 0.0F;
  style.TabBorderSize = 0.0F;
  style.TabCloseButtonMinWidthSelected = -1.0F;
  style.TabCloseButtonMinWidthUnselected = -1.0F;
  style.TabBarBorderSize = 0.0F;
  style.TabBarOverlineSize = 0.0F;
  style.DockingSeparatorSize = 5.0F;

  ImGui::PushStyleColor(ImGuiCol_WindowBg, TI_LIGHT_GREY);
  ImGui::PushStyleColor(ImGuiCol_Border, TI_LIGHT_GREY);
  ImGui::PushStyleColor(ImGuiCol_ChildBg, TI_SHALLOW_GREY);
  ImGui::PushStyleColor(ImGuiCol_PopupBg, TI_DARK_GREY);
  ImGui::PushStyleColor(ImGuiCol_FrameBg, TI_LIGHT_GREY);
  ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, TI_HOVER_GREY);
  ImGui::PushStyleColor(ImGuiCol_FrameBgActive, TI_ACTIVE_GREY);
  ImGui::PushStyleColor(ImGuiCol_TitleBg, TI_LIGHT_GREY);
  ImGui::PushStyleColor(ImGuiCol_TitleBgCollapsed, TI_LIGHT_GREY);
  ImGui::PushStyleColor(ImGuiCol_TitleBgActive, TI_LIGHT_GREY);
  ImGui::PushStyleColor(ImGuiCol_ResizeGrip, TI_DARK_GREY);
  ImGui::PushStyleColor(ImGuiCol_ResizeGripHovered, TI_DARK_GREY);
  ImGui::PushStyleColor(ImGuiCol_ResizeGripActive, TI_DARK_GREY);
  ImGui::PushStyleColor(ImGuiCol_DockingEmptyBg, TI_LIGHT_GREY);
  ImGui::PushStyleColor(ImGuiCol_DockingPreview, TI_LIGHT_GREY);
  ImGui::PushStyleColor(ImGuiCol_Tab, TI_SHALLOW_GREY);
  ImGui::PushStyleColor(ImGuiCol_TabHovered, TI_SHALLOW_GREY);
  ImGui::PushStyleColor(ImGuiCol_TabActive, TI_SHALLOW_GREY);
  ImGui::PushStyleColor(ImGuiCol_TabUnfocused, TI_SHALLOW_GREY);
  ImGui::PushStyleColor(ImGuiCol_TabUnfocusedActive, TI_SHALLOW_GREY);
  ImGui::PushStyleColor(ImGuiCol_Text, TI_TEXT_GREY);
  ImGui::PushStyleColor(ImGuiCol_Separator, TI_LIGHT_GREY);
  ImGui::PushStyleColor(ImGuiCol_SeparatorActive, TI_LIGHT_GREY);
  ImGui::PushStyleColor(ImGuiCol_SeparatorHovered, TI_LIGHT_GREY);
  ImGui::PushStyleColor(ImGuiCol_ScrollbarBg, TI_SHALLOW_GREY);
  ImGui::PushStyleColor(ImGuiCol_ScrollbarGrab, TI_LIGHT_GREY);
  ImGui::PushStyleColor(ImGuiCol_ScrollbarGrabHovered, TI_HOVER_GREY);
  ImGui::PushStyleColor(ImGuiCol_ScrollbarGrabActive, TI_ACTIVE_GREY);
  ImGui::PushStyleColor(ImGuiCol_Button, TI_LIGHT_GREY);
  ImGui::PushStyleColor(ImGuiCol_ButtonHovered, TI_HOVER_GREY);
  ImGui::PushStyleColor(ImGuiCol_ButtonActive, TI_ACTIVE_GREY);
  ImGui::PushStyleColor(ImGuiCol_Header, TI_LIGHT_GREY);
  ImGui::PushStyleColor(ImGuiCol_HeaderHovered, TI_HOVER_GREY);
  ImGui::PushStyleColor(ImGuiCol_HeaderActive, TI_ACTIVE_GREY);

  ImGui_ImplWin32_Init(g_pl_window.window_handle);

  ImGui_ImplVulkan_InitInfo imgui_vulkan_init_info = {
    .Instance = g_vk_instance.instance,
    .PhysicalDevice = g_vk_instance.physical_device,
    .Device = g_vk_instance.device,
    .QueueFamily = g_vk_instance.primary_queue_index,
    .Queue = g_vk_instance.primary_queue,
    .DescriptorPool = s_descriptor_pool,
    .MinImageCount = g_vk_instance.min_image_count,
    .ImageCount = g_vk_swapchain.image_count,
    .PipelineCache = 0,
    .PipelineInfoMain = {
      .RenderPass = g_vk_imgui_renderpass.handle,
      .Subpass = 0,
      .MSAASamples = VK_SAMPLE_COUNT_1_BIT,
    },
    .Allocator = 0,
    .CheckVkResultFn = 0,
  };

  ImGui_ImplVulkan_Init(&imgui_vulkan_init_info);
}
void im_draw(void) {
  ImGui_ImplVulkan_NewFrame();
  ImGui_ImplWin32_NewFrame();

  ImGui::NewFrame();

  im_titlebar_draw();
  im_sidebar_draw();

  ImGui::SetNextWindowPos(ImVec2((float)g_pl_window.sidebar_width, (float)g_pl_window.titlebar_height));
  ImGui::SetNextWindowSize(ImVec2((float)g_pl_window.window_width - (float)g_pl_window.sidebar_width, (float)g_pl_window.window_height - g_pl_window.titlebar_height));

  ImGuiWindowFlags window_flags =
    ImGuiWindowFlags_NoDocking |
    ImGuiWindowFlags_NoTitleBar |
    ImGuiWindowFlags_NoCollapse |
    ImGuiWindowFlags_NoResize |
    ImGuiWindowFlags_NoMove |
    ImGuiWindowFlags_NoBringToFrontOnFocus |
    ImGuiWindowFlags_NoNavFocus;

  ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(5.0F, 5.0F));
  ImGui::Begin("dockspace", 0, window_flags);
  ImGui::PopStyleVar(1);

  ImGuiDockNodeFlags dock_flags =
    ImGuiDockNodeFlags_NoDockingSplit |
    ImGuiDockNodeFlags_AutoHideTabBar |
    ImGuiDockNodeFlags_NoUndocking |
    ImGuiDockNodeFlags_NoTabBar;

  ImGui::DockSpace(ImGui::GetID("main_dockspace"), ImVec2(0.0F, 0.0F), dock_flags);

  build_layout();

  ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 10.0F);

  if (g_im_show_left_panel) {
    switch (g_sidebar_tab) {

      case IM_SIDEBAR_TAB_HIERARCHY: {

        im_hierarchy_draw();

        break;
      }
      case IM_SIDEBAR_TAB_FILESYSTEM: {

        im_fs_draw();

        break;
      }
      case IM_SIDEBAR_TAB_HANDLE: {

        im_handle_draw();

        break;
      }
      case IM_SIDEBAR_TAB_RENDERER: {

        im_renderer_draw();

        break;
      }
    }
  }

  im_viewport_draw();

  if (g_im_show_right_panel) {
    im_inspector_draw();
  }

  ImGui::PopStyleVar(1);

  ImGui::End();
  // ImGui::ShowDemoWindow();
  // ImGui::ShowStyleEditor();
  ImGui::Render();

  ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), g_vk_instance.command_buffer);
}
void im_message(HWND window_handle, UINT window_message, WPARAM w_param, LPARAM l_param) {
  ImGui_ImplWin32_WndProcHandler(window_handle, window_message, w_param, l_param);
}
void im_destroy(void) {
  im_fs_reset();
  im_handle_reset();
  im_hierarchy_reset();
  im_inspector_reset();
  im_sidebar_reset();
  im_titlebar_reset();
  im_viewport_reset();
  im_renderer_reset();

  ImGui_ImplVulkan_Shutdown();
  ImGui_ImplWin32_Shutdown();

  ImGuiIO &io = ImGui::GetIO();

  io.Fonts->RemoveFont((ImFont *)g_im_font_default_16);

  io.Fonts->RemoveFont((ImFont *)g_im_font_symbols_16);
  io.Fonts->RemoveFont((ImFont *)g_im_font_symbols_18);
  io.Fonts->RemoveFont((ImFont *)g_im_font_symbols_22);
  io.Fonts->RemoveFont((ImFont *)g_im_font_symbols_32);

  ImGui::DestroyContext();

  vkDestroyDescriptorPool(g_vk_instance.device, s_descriptor_pool, 0);
}

static int32_t Platform_CreateVkSurface(ImGuiViewport *vp, ImU64 vk_inst, const void *vk_allocators, ImU64 *out_vk_surface) {
  return 0;
}

static void build_layout(void) {
  if (s_layout_init) {

    s_layout_init = 0;

    ImGuiViewport *viewport = ImGui::GetMainViewport();

    ImGuiID dockspace_id = ImGui::GetID("main_dockspace");

    ImGui::DockBuilderRemoveNode(dockspace_id);
    ImGui::DockBuilderAddNode(dockspace_id, ImGuiDockNodeFlags_DockSpace);
    ImGui::DockBuilderSetNodeSize(dockspace_id, viewport->WorkSize);

    ImGuiID dock_main = dockspace_id;
    ImGuiID dock_left = 0;
    ImGuiID dock_right = 0;

    dock_left = ImGui::DockBuilderSplitNode(dock_main, ImGuiDir_Left, 0.15F, 0, &dock_main);
    dock_right = ImGui::DockBuilderSplitNode(dock_main, ImGuiDir_Right, 0.25F, 0, &dock_main);

    ImGui::DockBuilderDockWindow("Viewport", dock_main);
    ImGui::DockBuilderDockWindow("Hierarchy", dock_left);
    ImGui::DockBuilderDockWindow("Filesystem", dock_left);
    ImGui::DockBuilderDockWindow("Handle", dock_left);
    ImGui::DockBuilderDockWindow("Renderer", dock_left);
    ImGui::DockBuilderDockWindow("Inspector", dock_right);

    ImGui::DockBuilderFinish(dockspace_id);
  }
}
