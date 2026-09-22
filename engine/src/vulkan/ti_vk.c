#include <ti_pch.h>

#ifdef BUILD_DEBUG
static VkBool32 vulkan_message_proc(VkDebugUtilsMessageSeverityFlagBitsEXT message_severity, VkDebugUtilsMessageTypeFlagsEXT message_type, VkDebugUtilsMessengerCallbackDataEXT const *callback_data, void *user_data);
#endif // BUILD_DEBUG

static void create_instance(void);
static void create_surface(void);
static void create_device(void);
static void create_command_pool(void);
static void create_command_buffer(void);

static void find_physical_device(void);
static void find_physical_device_queue_families(void);

static void check_physical_device_extensions(void);
static void check_physical_device_features(void);

static void destroy_instance(void);
static void destroy_surface(void);
static void destroy_device(void);
static void destroy_command_pool(void);
static void destroy_command_buffer(void);

#ifdef BUILD_DEBUG
static char const *s_instance_layer[] = {
  "VK_LAYER_KHRONOS_validation",
};
#endif // BUILD_DEBUG

static char const *s_instance_extension[] = {
  "VK_KHR_surface",
  "VK_KHR_win32_surface",
#ifdef BUILD_DEBUG
  "VK_EXT_debug_utils",
#endif // BUILD_DEBUG
};

static char const *s_device_extension[] = {
  "VK_KHR_swapchain",
  "VK_KHR_ray_tracing_pipeline",
  "VK_KHR_acceleration_structure",
  "VK_KHR_deferred_host_operations",
  "VK_EXT_mesh_shader", // TODO: do we really need task/mesh shaders..
};

VkPhysicalDeviceRayTracingPipelinePropertiesKHR g_physical_device_ray_tracing_pipeline_properties = {
  .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PIPELINE_PROPERTIES_KHR,
  .pNext = 0,
};

VkPhysicalDeviceVulkan12Features g_physical_device_vulkan_12_features = {
  .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES,
  .pNext = 0,
};
VkPhysicalDeviceRayTracingPipelineFeaturesKHR g_physical_device_ray_tracing_pipeline_features = {
  .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PIPELINE_FEATURES_KHR,
  .pNext = &g_physical_device_vulkan_12_features,
};
VkPhysicalDeviceAccelerationStructureFeaturesKHR g_physical_device_acceleration_structure_features = {
  .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ACCELERATION_STRUCTURE_FEATURES_KHR,
  .pNext = &g_physical_device_ray_tracing_pipeline_features,
};
VkPhysicalDeviceMaintenance4Features g_physical_device_maintenance_4_features = {
  .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MAINTENANCE_4_FEATURES,
  .pNext = &g_physical_device_acceleration_structure_features,
};
VkPhysicalDeviceFragmentShadingRateFeaturesKHR g_physical_device_fragment_shading_rate_features = {
  .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_SHADING_RATE_FEATURES_KHR,
  .pNext = &g_physical_device_maintenance_4_features,
};
VkPhysicalDeviceMultiviewFeatures g_physical_device_multiview_features = {
  .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MULTIVIEW_FEATURES,
  .pNext = &g_physical_device_fragment_shading_rate_features,
};
VkPhysicalDeviceMeshShaderFeaturesEXT g_physical_device_mesh_shader_features = {
  .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MESH_SHADER_FEATURES_EXT,
  .pNext = &g_physical_device_multiview_features,
};

vk_instance_t g_vk_instance = {0};
vk_swapchain_t g_vk_swapchain = {0};
vk_renderer_t g_vk_renderer = {0};

vk_renderpass_t g_vk_main_renderpass = {0};
vk_renderpass_t g_vk_imgui_renderpass = {0};

vk_framebuffer_t g_vk_main_framebuffer = {0};
vk_framebuffer_t g_vk_imgui_framebuffer = {0};

vk_buffer_t g_vk_time_info_buffer = {0};
vk_buffer_t g_vk_screen_info_buffer = {0};
vk_buffer_t g_vk_mouse_info_buffer = {0};
vk_buffer_t g_vk_camera_info_buffer = {0};

#ifdef BUILD_DEBUG
PFN_vkCreateDebugUtilsMessengerEXT vkCreateDebugUtilsMessengerEXT_proc = 0;
PFN_vkDestroyDebugUtilsMessengerEXT vkDestroyDebugUtilsMessengerEXT_proc = 0;
#endif // BUILD_DEBUG

PFN_vkCmdDrawMeshTasksEXT vkCmdDrawMeshTasksEXT_proc = 0;
PFN_vkCmdTraceRaysKHR vkCmdTraceRaysKHR_proc = 0;
PFN_vkCmdBuildAccelerationStructuresKHR vkCmdBuildAccelerationStructuresKHR_proc = 0;

PFN_vkCreateAccelerationStructureKHR vkCreateAccelerationStructureKHR_proc = 0;
PFN_vkCreateRayTracingPipelinesKHR vkCreateRayTracingPipelinesKHR_proc = 0;

PFN_vkGetAccelerationStructureBuildSizesKHR vkGetAccelerationStructureBuildSizesKHR_proc = 0;
PFN_vkGetAccelerationStructureDeviceAddressKHR vkGetAccelerationStructureDeviceAddressKHR_proc = 0;
PFN_vkGetRayTracingShaderGroupHandlesKHR vkGetRayTracingShaderGroupHandlesKHR_proc = 0;

PFN_vkDestroyAccelerationStructureKHR vkDestroyAccelerationStructureKHR_proc = 0;

void vk_create(void) {
  create_instance();
  create_surface();

  find_physical_device();
  find_physical_device_queue_families();

  check_physical_device_extensions();
  check_physical_device_features();

  create_device();
  create_command_pool();
  create_command_buffer();

  vk_update_surface_capabilities();

  vk_swapchain_create(&g_vk_swapchain, "asset/swapchain/main.pak");
  vk_renderer_create(&g_vk_renderer, "asset/renderer/main.pak");

  vk_renderpass_create(&g_vk_main_renderpass, "asset/renderpass/main.pak");
  vk_renderpass_create(&g_vk_imgui_renderpass, "asset/renderpass/imgui.pak");

  g_vk_main_framebuffer.width = 1;
  g_vk_main_framebuffer.height = 1;

  g_vk_imgui_framebuffer.width = g_pl_window.window_width;
  g_vk_imgui_framebuffer.height = g_pl_window.window_height;

  vk_framebuffer_create(&g_vk_main_framebuffer, &g_vk_main_renderpass, "asset/framebuffer/main.pak");
  vk_framebuffer_create(&g_vk_imgui_framebuffer, &g_vk_imgui_renderpass, "asset/framebuffer/imgui.pak");

  vk_buffer_create(&g_vk_time_info_buffer, "asset/buffer/time_info.pak");
  vk_buffer_create(&g_vk_screen_info_buffer, "asset/buffer/screen_info.pak");
  vk_buffer_create(&g_vk_mouse_info_buffer, "asset/buffer/mouse_info.pak");
  vk_buffer_create(&g_vk_camera_info_buffer, "asset/buffer/camera_info.pak");

  vk_buffer_map(&g_vk_time_info_buffer);
  vk_buffer_map(&g_vk_screen_info_buffer);
  vk_buffer_map(&g_vk_mouse_info_buffer);
  vk_buffer_map(&g_vk_camera_info_buffer);

  im_create();
}
void vk_destroy(void) {
  TI_VK_CHECK(vkQueueWaitIdle(g_vk_instance.primary_queue));
  TI_VK_CHECK(vkQueueWaitIdle(g_vk_instance.present_queue));

  im_destroy();

  vk_buffer_unmap(&g_vk_time_info_buffer);
  vk_buffer_unmap(&g_vk_screen_info_buffer);
  vk_buffer_unmap(&g_vk_mouse_info_buffer);
  vk_buffer_unmap(&g_vk_camera_info_buffer);

  vk_buffer_destroy(&g_vk_time_info_buffer);
  vk_buffer_destroy(&g_vk_screen_info_buffer);
  vk_buffer_destroy(&g_vk_mouse_info_buffer);
  vk_buffer_destroy(&g_vk_camera_info_buffer);

  vk_framebuffer_destroy(&g_vk_imgui_framebuffer);
  vk_framebuffer_destroy(&g_vk_main_framebuffer);

  vk_renderpass_destroy(&g_vk_imgui_renderpass);
  vk_renderpass_destroy(&g_vk_main_renderpass);

  vk_renderer_destroy(&g_vk_renderer);
  vk_swapchain_destroy(&g_vk_swapchain);

  destroy_command_buffer();
  destroy_command_pool();
  destroy_device();
  destroy_surface();
  destroy_instance();
}

void vk_update_surface_capabilities(void) {
  TI_VK_CHECK(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(g_vk_instance.physical_device, g_vk_instance.surface, &g_vk_instance.surface_capabilities));

  g_pl_window.window_width = g_vk_instance.surface_capabilities.currentExtent.width;
  g_pl_window.window_height = g_vk_instance.surface_capabilities.currentExtent.height;

  g_vk_instance.min_image_count = g_vk_instance.surface_capabilities.minImageCount;
  g_vk_instance.max_image_count = g_vk_instance.surface_capabilities.maxImageCount;
  g_vk_instance.surface_transform = g_vk_instance.surface_capabilities.currentTransform;
}

uint32_t vk_find_memory_type_index(uint32_t type_filter, VkMemoryPropertyFlags memory_property_flags) {
  uint32_t memory_type_index = 0;
  uint32_t memory_type_count = g_vk_instance.physical_device_memory_properties2.memoryProperties.memoryTypeCount;

  while (memory_type_index < memory_type_count) {

    if ((type_filter & (1 << memory_type_index)) && ((g_vk_instance.physical_device_memory_properties2.memoryProperties.memoryTypes[memory_type_index].propertyFlags & memory_property_flags) == memory_property_flags)) {
      return memory_type_index;
    }

    memory_type_index++;
  }

  return UINT32_MAX;
}

VkCommandBuffer vk_primary_command_buffer_record_immediate(void) {
  VkCommandBuffer command_buffer = 0;

  VkCommandBufferAllocateInfo command_buffer_allocate_info = {
    .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
    .commandPool = g_vk_instance.command_pool,
    .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
    .commandBufferCount = 1,
  };

  TI_VK_CHECK(vkAllocateCommandBuffers(g_vk_instance.device, &command_buffer_allocate_info, &command_buffer));

  VkCommandBufferBeginInfo command_buffer_begin_info = {
    .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
    .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
  };

  TI_VK_CHECK(vkBeginCommandBuffer(command_buffer, &command_buffer_begin_info));

  return command_buffer;
}
void vk_primary_command_buffer_submit_immediate(VkCommandBuffer command_buffer) {
  TI_VK_CHECK(vkEndCommandBuffer(command_buffer));

  VkSubmitInfo submit_info = {
    .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
    .commandBufferCount = 1,
    .pCommandBuffers = &command_buffer,
  };

  TI_VK_CHECK(vkQueueSubmit(g_vk_instance.primary_queue, 1, &submit_info, 0));
  TI_VK_CHECK(vkQueueWaitIdle(g_vk_instance.primary_queue));

  vkFreeCommandBuffers(g_vk_instance.device, g_vk_instance.command_pool, 1, &command_buffer);
}

#ifdef BUILD_DEBUG
static VkBool32 vulkan_message_proc(VkDebugUtilsMessageSeverityFlagBitsEXT message_severity, VkDebugUtilsMessageTypeFlagsEXT message_type, VkDebugUtilsMessengerCallbackDataEXT const *callback_data, void *user_data) {
  printf("%s\n", callback_data->pMessage);

  __debugbreak();

  return 0;
}
#endif // BUILD_DEBUG

static void create_instance(void) {
  VkApplicationInfo application_info = {
    .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
    .pApplicationName = "TITAN_APPLICATION",
    .applicationVersion = VK_MAKE_API_VERSION(0, 1, 0, 0),
    .pEngineName = "TITAN_ENGINE",
    .engineVersion = VK_MAKE_API_VERSION(0, 1, 0, 0),
    .apiVersion = VK_API_VERSION_1_3, // TODO
  };

#ifdef BUILD_DEBUG
  VkValidationFeatureEnableEXT validation_feature_enable[] = {
    VK_VALIDATION_FEATURE_ENABLE_GPU_ASSISTED_EXT,
    VK_VALIDATION_FEATURE_ENABLE_BEST_PRACTICES_EXT,
    VK_VALIDATION_FEATURE_ENABLE_SYNCHRONIZATION_VALIDATION_EXT,
  };

  VkValidationFeaturesEXT validation_features = {
    .sType = VK_STRUCTURE_TYPE_VALIDATION_FEATURES_EXT,
    .pNext = 0,
    .enabledValidationFeatureCount = 0, // ARRAY_COUNT(validation_feature_enable),
    .pEnabledValidationFeatures = 0,    // validation_feature_enable,
    .disabledValidationFeatureCount = 0,
    .pDisabledValidationFeatures = 0,
  };

  VkDebugUtilsMessengerCreateInfoEXT debug_utils_messenger_create_info = {
    .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
    .pNext = 0,
    .flags = 0,
    .messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
    .messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
    .pfnUserCallback = vulkan_message_proc,
    .pUserData = 0,
  };
#endif // BUILD_DEBUG

  VkInstanceCreateInfo instance_create_info = {
    .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
    .pApplicationInfo = &application_info,
    .ppEnabledExtensionNames = s_instance_extension,
    .enabledExtensionCount = TI_ARRAY_COUNT(s_instance_extension),
#ifdef BUILD_DEBUG
    .pNext = &debug_utils_messenger_create_info,
    .ppEnabledLayerNames = s_instance_layer,
    .enabledLayerCount = TI_ARRAY_COUNT(s_instance_layer),
#endif // BUILD_DEBUG
  };

  TI_VK_CHECK(vkCreateInstance(&instance_create_info, 0, &g_vk_instance.instance));

#ifdef BUILD_DEBUG
  vkCreateDebugUtilsMessengerEXT_proc = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(g_vk_instance.instance, "vkCreateDebugUtilsMessengerEXT");
  vkDestroyDebugUtilsMessengerEXT_proc = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(g_vk_instance.instance, "vkDestroyDebugUtilsMessengerEXT");

  TI_VK_CHECK(vkCreateDebugUtilsMessengerEXT_proc(g_vk_instance.instance, &debug_utils_messenger_create_info, 0, &g_vk_instance.debug_utils_messenger));

  uint32_t instance_version = 0;

  VkResult result = vkEnumerateInstanceVersion(&instance_version);

  if (result == VK_SUCCESS) {

    uint32_t major = VK_VERSION_MAJOR(instance_version);
    uint32_t minor = VK_VERSION_MINOR(instance_version);
    uint32_t patch = VK_VERSION_PATCH(instance_version);

    printf("Vulkan Runtime Version: %d.%d.%d\n", major, minor, patch);

  } else {

    printf("vkEnumerateInstanceVersion not supported, default to 1.0\n");
  }

  printf("Vulkan Header Version: %d\n", VK_HEADER_VERSION);
  printf("\n");
#endif // BUILD_DEBUG
}
static void create_surface(void) {
  VkWin32SurfaceCreateInfoKHR win32_surface_create_info = {
    .sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR,
    .hwnd = g_pl_window.window_handle,
    .hinstance = g_pl_window.module_handle,
  };

  TI_VK_CHECK(vkCreateWin32SurfaceKHR(g_vk_instance.instance, &win32_surface_create_info, 0, &g_vk_instance.surface));
}
static void create_device(void) {
  float queue_priority = 1.0F;

  VkDeviceQueueCreateInfo device_queue_create_infos[2] = {
    {
      .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
      .queueFamilyIndex = g_vk_instance.primary_queue_index,
      .queueCount = 1,
      .pQueuePriorities = &queue_priority,
    },
    {
      .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
      .queueFamilyIndex = g_vk_instance.present_queue_index,
      .queueCount = 1,
      .pQueuePriorities = &queue_priority,
    },
  };

  VkDeviceCreateInfo device_create_info = {
    .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
    .pNext = &g_vk_instance.physical_device_features2,
    .pQueueCreateInfos = device_queue_create_infos,
    .queueCreateInfoCount = TI_ARRAY_COUNT(device_queue_create_infos),
    .pEnabledFeatures = 0,
    .ppEnabledExtensionNames = s_device_extension,
    .enabledExtensionCount = TI_ARRAY_COUNT(s_device_extension),
#ifdef BUILD_DEBUG
    .ppEnabledLayerNames = 0,
    .enabledLayerCount = 0,
#endif // BUILD_DEBUG
  };

  TI_VK_CHECK(vkCreateDevice(g_vk_instance.physical_device, &device_create_info, 0, &g_vk_instance.device));

  vkGetDeviceQueue(g_vk_instance.device, g_vk_instance.primary_queue_index, 0, &g_vk_instance.primary_queue);
  vkGetDeviceQueue(g_vk_instance.device, g_vk_instance.present_queue_index, 0, &g_vk_instance.present_queue);

  vkCmdDrawMeshTasksEXT_proc = (PFN_vkCmdDrawMeshTasksEXT)vkGetDeviceProcAddr(g_vk_instance.device, "vkCmdDrawMeshTasksEXT");
  vkCmdTraceRaysKHR_proc = (PFN_vkCmdTraceRaysKHR)vkGetDeviceProcAddr(g_vk_instance.device, "vkCmdTraceRaysKHR");
  vkCmdBuildAccelerationStructuresKHR_proc = (PFN_vkCmdBuildAccelerationStructuresKHR)vkGetDeviceProcAddr(g_vk_instance.device, "vkCmdBuildAccelerationStructuresKHR");

  vkCreateAccelerationStructureKHR_proc = (PFN_vkCreateAccelerationStructureKHR)vkGetDeviceProcAddr(g_vk_instance.device, "vkCreateAccelerationStructureKHR");
  vkCreateRayTracingPipelinesKHR_proc = (PFN_vkCreateRayTracingPipelinesKHR)vkGetDeviceProcAddr(g_vk_instance.device, "vkCreateRayTracingPipelinesKHR");

  vkGetAccelerationStructureBuildSizesKHR_proc = (PFN_vkGetAccelerationStructureBuildSizesKHR)vkGetDeviceProcAddr(g_vk_instance.device, "vkGetAccelerationStructureBuildSizesKHR");
  vkGetAccelerationStructureDeviceAddressKHR_proc = (PFN_vkGetAccelerationStructureDeviceAddressKHR)vkGetDeviceProcAddr(g_vk_instance.device, "vkGetAccelerationStructureDeviceAddressKHR");
  vkGetRayTracingShaderGroupHandlesKHR_proc = (PFN_vkGetRayTracingShaderGroupHandlesKHR)vkGetDeviceProcAddr(g_vk_instance.device, "vkGetRayTracingShaderGroupHandlesKHR");

  vkDestroyAccelerationStructureKHR_proc = (PFN_vkDestroyAccelerationStructureKHR)vkGetDeviceProcAddr(g_vk_instance.device, "vkDestroyAccelerationStructureKHR");
}
static void create_command_pool(void) {
  VkCommandPoolCreateInfo command_pool_create_info = {
    .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
    .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
    .queueFamilyIndex = g_vk_instance.primary_queue_index,
  };

  TI_VK_CHECK(vkCreateCommandPool(g_vk_instance.device, &command_pool_create_info, 0, &g_vk_instance.command_pool));
}
static void create_command_buffer(void) {
  VkCommandBufferAllocateInfo command_buffer_allocate_info = {
    .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
    .commandPool = g_vk_instance.command_pool,
    .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
    .commandBufferCount = 1,
  };

  TI_VK_CHECK(vkAllocateCommandBuffers(g_vk_instance.device, &command_buffer_allocate_info, &g_vk_instance.command_buffer));
}

static void find_physical_device(void) {
  uint32_t physical_device_index = 0;
  uint32_t physical_device_count = 0;

  static VkPhysicalDevice physical_devices[TI_WINDOW_MAX_PHYSICAL_DEVICES] = {0};

  TI_VK_CHECK(vkEnumeratePhysicalDevices(g_vk_instance.instance, &physical_device_count, 0));
  TI_VK_CHECK(vkEnumeratePhysicalDevices(g_vk_instance.instance, &physical_device_count, physical_devices));

  while (physical_device_index < physical_device_count) {

    VkPhysicalDevice physical_device = physical_devices[physical_device_index];

    g_vk_instance.physical_device_features2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    g_vk_instance.physical_device_features2.pNext = &g_physical_device_mesh_shader_features;

    g_vk_instance.physical_device_properties2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    g_vk_instance.physical_device_properties2.pNext = &g_physical_device_ray_tracing_pipeline_properties;

    g_vk_instance.physical_device_memory_properties2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MEMORY_PROPERTIES_2;
    g_vk_instance.physical_device_memory_properties2.pNext = 0;

    vkGetPhysicalDeviceFeatures2(physical_device, &g_vk_instance.physical_device_features2);
    vkGetPhysicalDeviceProperties2(physical_device, &g_vk_instance.physical_device_properties2);
    vkGetPhysicalDeviceMemoryProperties2(physical_device, &g_vk_instance.physical_device_memory_properties2);

    if (g_vk_instance.physical_device_properties2.properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {

      g_vk_instance.physical_device = physical_device;

      break;
    }

    physical_device_index++;
  }

#ifdef BUILD_DEBUG
  VkPhysicalDeviceProperties *props = &g_vk_instance.physical_device_properties2.properties;

  char const *vendor_name = 0;

  switch (props->vendorID) {

    case TI_VENDOR_ID_NVIDIA:
      vendor_name = "NVIDIA";
      break;
    case TI_VENDOR_ID_AMD:
      vendor_name = "AMD";
      break;
    case TI_VENDOR_ID_INTEL:
      vendor_name = "INTEL";
      break;
    default:
      vendor_name = "UNKNOWN";
      break;
  }

  printf("Selected Physical Device\n");
  printf("  Device Index: %d\n", physical_device_index);
  printf("  Device Name: %s\n", props->deviceName);
  printf("  Vulkan Version: %u.%u.%u\n", VK_VERSION_MAJOR(props->apiVersion), VK_VERSION_MINOR(props->apiVersion), VK_VERSION_PATCH(props->apiVersion));

  switch (props->vendorID) {

    case TI_VENDOR_ID_NVIDIA:
      printf("  Driver Version: %u.%u.%u\n", (props->driverVersion >> 22) & 0x3FF, (props->driverVersion >> 14) & 0xFF, (props->driverVersion >> 0) & 0x3FFF);
      break;
    case TI_VENDOR_ID_AMD:
      printf("  Driver Version: %u.%u.%u\n", (props->driverVersion >> 22) & 0x3FF, (props->driverVersion >> 12) & 0x3FF, (props->driverVersion >> 0) & 0xFFF);
      break;
    case TI_VENDOR_ID_INTEL:
      printf("  Driver Version: %u\n", props->driverVersion);
      break;
    default:
      printf("Driver Version: %u\n", props->driverVersion);
      break;
  }

  printf("  Vendor Name: %s\n", vendor_name);
  printf("  Vendor ID: 0x%X\n", props->vendorID);
  printf("  Device ID: 0x%X\n", props->deviceID);
  printf("  Device Type: %d\n", props->deviceType);
  printf("\n");
#endif // BUILD_DEBUG
}
static void find_physical_device_queue_families(void) {
  int32_t primary_queue_index = -1;
  int32_t present_queue_index = -1;

  uint32_t queue_family_property_index = 0;
  uint32_t queue_family_property_count = 0;

  static VkQueueFamilyProperties queue_family_properties[TI_WINDOW_MAX_QUEUE_FAMILY_PROPERTIES_COUNT] = {0};

  vkGetPhysicalDeviceQueueFamilyProperties(g_vk_instance.physical_device, &queue_family_property_count, 0);
  vkGetPhysicalDeviceQueueFamilyProperties(g_vk_instance.physical_device, &queue_family_property_count, queue_family_properties);

  while (queue_family_property_index < queue_family_property_count) {

    VkQueueFamilyProperties queue_family_property = queue_family_properties[queue_family_property_index];

    VkBool32 graphics_support = queue_family_property.queueFlags & VK_QUEUE_GRAPHICS_BIT;
    VkBool32 compute_support = queue_family_property.queueFlags & VK_QUEUE_COMPUTE_BIT;
    VkBool32 present_support = 0;

    TI_VK_CHECK(vkGetPhysicalDeviceSurfaceSupportKHR(g_vk_instance.physical_device, (uint32_t)queue_family_property_index, g_vk_instance.surface, &present_support));

    if (graphics_support && compute_support && (primary_queue_index == -1)) {

      primary_queue_index = queue_family_property_index;

    } else if (present_support && (present_queue_index == -1)) {

      present_queue_index = queue_family_property_index;
    }

    if ((primary_queue_index != -1) && (present_queue_index != -1)) {

      g_vk_instance.primary_queue_index = (uint32_t)primary_queue_index;
      g_vk_instance.present_queue_index = (uint32_t)present_queue_index;

      break;
    }

    queue_family_property_index++;
  }

#ifdef BUILD_DEBUG
  printf("Selected Physical Queues\n");
  printf("  Primary Queue Index: %d\n", g_vk_instance.primary_queue_index);
  printf("  Present Queue Index: %d\n", g_vk_instance.present_queue_index);
  printf("\n");
#endif // BUILD_DEBUG
}

static void check_physical_device_extensions(void) {
  uint32_t available_device_extension_count = 0;

  static VkExtensionProperties available_extension_properties[TI_WINDOW_MAX_EXTENSION_PROPERTIES_COUNT] = {0};

  TI_VK_CHECK(vkEnumerateDeviceExtensionProperties(g_vk_instance.physical_device, 0, &available_device_extension_count, 0));
  TI_VK_CHECK(vkEnumerateDeviceExtensionProperties(g_vk_instance.physical_device, 0, &available_device_extension_count, available_extension_properties));

#ifdef BUILD_DEBUG
  printf("Required Device Extensions\n");
#endif // BUILD_DEBUG

  uint32_t device_extension_index = 0;
  uint32_t device_extension_count = TI_ARRAY_COUNT(s_device_extension);

  while (device_extension_index < device_extension_count) {

    uint32_t device_extensions_available = 0;

    uint32_t available_device_extension_index = 0;

    while (available_device_extension_index < available_device_extension_count) {

      if (strcmp(s_device_extension[device_extension_index], available_extension_properties[available_device_extension_index].extensionName) == 0) {

#ifdef BUILD_DEBUG
        printf("  %s: 1\n", s_device_extension[device_extension_index]);
#endif // BUILD_DEBUG

        device_extensions_available = 1;

        break;
      }

      available_device_extension_index++;
    }

    if (device_extensions_available == 0) {

#ifdef BUILD_DEBUG
      printf("  %s: 0\n", s_device_extension[device_extension_index]);
#endif // BUILD_DEBUG

      break;
    }

    device_extension_index++;
  }

#ifdef BUILD_DEBUG
  printf("\n");
#endif // BUILD_DEBUG
}
static void check_physical_device_features(void) {
#ifdef BUILD_DEBUG
  printf("Required Device Features\n");

  printf("  VkPhysicalDeviceMeshShaderFeaturesEXT::taskShader: %d\n", g_physical_device_mesh_shader_features.taskShader);
  printf("  VkPhysicalDeviceMeshShaderFeaturesEXT::meshShader: %d\n", g_physical_device_mesh_shader_features.meshShader);

  printf("  VkPhysicalDeviceRayTracingPipelineFeaturesKHR::rayTracingPipeline: %d\n", g_physical_device_ray_tracing_pipeline_features.rayTracingPipeline);

  printf("  VkPhysicalDeviceAccelerationStructureFeaturesKHR::accelerationStructure: %d\n", g_physical_device_acceleration_structure_features.accelerationStructure);

  printf("  VkPhysicalDeviceVulkan12Features::runtimeDescriptorArray: %d\n", g_physical_device_vulkan_12_features.runtimeDescriptorArray);
  printf("  VkPhysicalDeviceVulkan12Features::bufferDeviceAddress: %d\n", g_physical_device_vulkan_12_features.bufferDeviceAddress);
  printf("  VkPhysicalDeviceVulkan12Features::shaderSampledImageArrayNonUniformIndexing %d\n", g_physical_device_vulkan_12_features.shaderSampledImageArrayNonUniformIndexing);
  printf("  VkPhysicalDeviceVulkan12Features::descriptorBindingSampledImageUpdateAfterBind %d\n", g_physical_device_vulkan_12_features.descriptorBindingSampledImageUpdateAfterBind);
  printf("  VkPhysicalDeviceVulkan12Features::shaderUniformBufferArrayNonUniformIndexing %d\n", g_physical_device_vulkan_12_features.shaderUniformBufferArrayNonUniformIndexing);
  printf("  VkPhysicalDeviceVulkan12Features::descriptorBindingUniformBufferUpdateAfterBind %d\n", g_physical_device_vulkan_12_features.descriptorBindingUniformBufferUpdateAfterBind);
  printf("  VkPhysicalDeviceVulkan12Features::shaderStorageBufferArrayNonUniformIndexing %d\n", g_physical_device_vulkan_12_features.shaderStorageBufferArrayNonUniformIndexing);
  printf("  VkPhysicalDeviceVulkan12Features::descriptorBindingStorageBufferUpdateAfterBind %d\n", g_physical_device_vulkan_12_features.descriptorBindingStorageBufferUpdateAfterBind);

  printf("\n");

  assert(g_physical_device_mesh_shader_features.taskShader);
  assert(g_physical_device_mesh_shader_features.meshShader);

  assert(g_physical_device_ray_tracing_pipeline_features.rayTracingPipeline);

  assert(g_physical_device_acceleration_structure_features.accelerationStructure);

  assert(g_physical_device_vulkan_12_features.runtimeDescriptorArray);
  assert(g_physical_device_vulkan_12_features.bufferDeviceAddress);
  assert(g_physical_device_vulkan_12_features.shaderSampledImageArrayNonUniformIndexing);
  assert(g_physical_device_vulkan_12_features.descriptorBindingSampledImageUpdateAfterBind);
  assert(g_physical_device_vulkan_12_features.shaderUniformBufferArrayNonUniformIndexing);
  assert(g_physical_device_vulkan_12_features.descriptorBindingUniformBufferUpdateAfterBind);
  assert(g_physical_device_vulkan_12_features.shaderStorageBufferArrayNonUniformIndexing);
  assert(g_physical_device_vulkan_12_features.descriptorBindingStorageBufferUpdateAfterBind);
#endif // BUILD_DEBUG
}

static void destroy_instance(void) {
#ifdef BUILD_DEBUG
  vkDestroyDebugUtilsMessengerEXT_proc(g_vk_instance.instance, g_vk_instance.debug_utils_messenger, 0);
#endif // BUILD_DEBUG

  vkDestroyInstance(g_vk_instance.instance, 0);
}
static void destroy_surface(void) {
  vkDestroySurfaceKHR(g_vk_instance.instance, g_vk_instance.surface, 0);
}
static void destroy_device(void) {
  vkDestroyDevice(g_vk_instance.device, 0);
}
static void destroy_command_pool(void) {
  vkDestroyCommandPool(g_vk_instance.device, g_vk_instance.command_pool, 0);
}
static void destroy_command_buffer(void) {
  vkFreeCommandBuffers(g_vk_instance.device, g_vk_instance.command_pool, 1, &g_vk_instance.command_buffer);
}
