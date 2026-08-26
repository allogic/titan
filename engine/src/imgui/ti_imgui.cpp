#include <ti_pch.h>

#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_vulkan.h>

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND window_handle, UINT window_message, WPARAM w_param, LPARAM l_param);

static int32_t Platform_CreateVkSurface(ImGuiViewport *vp, ImU64 vk_inst, const void *vk_allocators, ImU64 *out_vk_surface);

static VkDescriptorPool s_imgui_descriptor_pool = 0;

static VkDescriptorPoolSize s_imgui_descriptor_pool_sizes[] = {
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

void imgui_create(void) {
  VkDescriptorPoolCreateInfo descriptor_pool_create_info = {
    .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
    .flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT,
    .maxSets = 1000,
    .poolSizeCount = TI_ARRAY_COUNT(s_imgui_descriptor_pool_sizes),
    .pPoolSizes = s_imgui_descriptor_pool_sizes,
  };

  TI_VK_CHECK(vkCreateDescriptorPool(g_window.device, &descriptor_pool_create_info, 0, &s_imgui_descriptor_pool));

  IMGUI_CHECKVERSION();

  ImGui::CreateContext();

  ImGuiIO &io = ImGui::GetIO();
  ImGuiPlatformIO &platform_io = ImGui::GetPlatformIO();
  ImGuiStyle &style = ImGui::GetStyle();

  io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
  io.ConfigWindowsMoveFromTitleBarOnly = 1;

  platform_io.Platform_CreateVkSurface = Platform_CreateVkSurface;

  ImGui_ImplWin32_Init(g_window.window_handle);

  ImGui_ImplVulkan_InitInfo imgui_vulkan_init_info = {
    .Instance = g_window.instance,
    .PhysicalDevice = g_window.physical_device,
    .Device = g_window.device,
    .QueueFamily = g_window.primary_queue_index,
    .Queue = g_window.primary_queue,
    .DescriptorPool = s_imgui_descriptor_pool,
    .MinImageCount = g_window.min_image_count,
    .ImageCount = g_swapchain.image_count,
    .PipelineCache = 0,
    .PipelineInfoMain = {
      .RenderPass = g_renderpass,
      .Subpass = 0,
      .MSAASamples = VK_SAMPLE_COUNT_1_BIT,
    },
    .Allocator = 0,
    .CheckVkResultFn = 0,
  };

  ImGui_ImplVulkan_Init(&imgui_vulkan_init_info);
}
void imgui_draw(VkCommandBuffer command_buffer) {
  ImGui_ImplVulkan_NewFrame();
  ImGui_ImplWin32_NewFrame();

  ImGui::NewFrame();

  ImGuiViewport *viewport = ImGui::GetMainViewport();

  ImGui::SetNextWindowPos(viewport->WorkPos);
  ImGui::SetNextWindowSize(viewport->WorkSize);
  ImGui::SetNextWindowViewport(viewport->ID);

  ImGuiWindowFlags window_flags =
    ImGuiWindowFlags_NoDocking |
    ImGuiWindowFlags_NoTitleBar |
    ImGuiWindowFlags_NoCollapse |
    ImGuiWindowFlags_NoResize |
    ImGuiWindowFlags_NoMove |
    ImGuiWindowFlags_NoBringToFrontOnFocus |
    ImGuiWindowFlags_NoNavFocus;

  ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0F);
  ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0F);
  ImGui::Begin("dockspace", 0, window_flags);
  ImGui::PopStyleVar(2);

  ImGuiID dockspace_id = ImGui::GetID("main_dockspace");

  ImGui::DockSpace(dockspace_id);

  layout_build();

  fs_draw();
  hierarchy_draw();
  scene_draw();
  inspector_draw();

  ImGui::End();
  ImGui::Render();

  ImDrawData *draw_data = ImGui::GetDrawData();

  ImGui_ImplVulkan_RenderDrawData(draw_data, command_buffer);
}
void imgui_message(HWND window_handle, UINT window_message, WPARAM w_param, LPARAM l_param) {
  ImGui_ImplWin32_WndProcHandler(window_handle, window_message, w_param, l_param);
}
void imgui_destroy(void) {
  // TODO: obsolete..?
  // adb_reset();
  // hierarchy_reset();
  // scene_reset();
  // mesh_reset();
  // skeleton_reset();
  // inspector_reset();

  ImGui_ImplVulkan_Shutdown();
  ImGui_ImplWin32_Shutdown();

  ImGui::DestroyContext();

  vkDestroyDescriptorPool(g_window.device, s_imgui_descriptor_pool, 0);
}

static int32_t Platform_CreateVkSurface(ImGuiViewport *vp, ImU64 vk_inst, const void *vk_allocators, ImU64 *out_vk_surface) {
  return 0;
}
