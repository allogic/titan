#include <vulkan/ti_vk_instance.h>

#ifdef BUILD_DEBUG
static VkBool32 message_proc(VkDebugUtilsMessageSeverityFlagBitsEXT message_severity, VkDebugUtilsMessageTypeFlagsEXT message_type, VkDebugUtilsMessengerCallbackDataEXT const *callback_data, void *user_data);
#endif // BUILD_DEBUG

static void create_instance(vk_instance_t *instance);
static void create_surface(vk_instance_t *instance);
static void create_device(vk_instance_t *instance);
static void create_command_pool(vk_instance_t *instance);
static void create_command_buffer(vk_instance_t *instance);

static void find_physical_device(vk_instance_t *instance);
static void find_physical_device_queue_families(vk_instance_t *instance);

static void check_physical_device_extensions(vk_instance_t *instance);
static void check_physical_device_features(void);

static void destroy_instance(vk_instance_t *instance);
static void destroy_surface(vk_instance_t *instance);
static void destroy_device(vk_instance_t *instance);
static void destroy_command_pool(vk_instance_t *instance);
static void destroy_command_buffer(vk_instance_t *instance);

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

void vk_instance_create(vk_instance_t *instance) {
  create_instance(instance);
  create_surface(instance);

  find_physical_device(instance);
  find_physical_device_queue_families(instance);

  check_physical_device_extensions(instance);
  check_physical_device_features();

  create_device(instance);
  create_command_pool(instance);
  create_command_buffer(instance);
}
void vk_instance_destroy(vk_instance_t *instance) {
  destroy_command_buffer(instance);
  destroy_command_pool(instance);
  destroy_device(instance);
  destroy_surface(instance);
  destroy_instance(instance);
}

#ifdef BUILD_DEBUG
static VkBool32 message_proc(VkDebugUtilsMessageSeverityFlagBitsEXT message_severity, VkDebugUtilsMessageTypeFlagsEXT message_type, VkDebugUtilsMessengerCallbackDataEXT const *callback_data, void *user_data) {
  printf("%s\n", callback_data->pMessage);

  __debugbreak();

  return 0;
}
#endif // BUILD_DEBUG

static void create_instance(vk_instance_t *instance) {
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
    .pfnUserCallback = message_proc,
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

  TI_VK_CHECK(vkCreateInstance(&instance_create_info, 0, &instance->instance));

#ifdef BUILD_DEBUG
  instance->create_debug_utils_messenger_ext_proc = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance->instance, "vkCreateDebugUtilsMessengerEXT");
  instance->destroy_debug_utils_messenger_ext_proc = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance->instance, "vkDestroyDebugUtilsMessengerEXT");

  TI_VK_CHECK(instance->create_debug_utils_messenger_ext_proc(instance->instance, &debug_utils_messenger_create_info, 0, &instance->debug_utils_messenger));

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
static void create_surface(vk_instance_t *instance) {
  VkWin32SurfaceCreateInfoKHR win32_surface_create_info = {
    .sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR,
    .hwnd = g_window.window_handle,
    .hinstance = g_window.module_handle,
  };

  TI_VK_CHECK(vkCreateWin32SurfaceKHR(instance->instance, &win32_surface_create_info, 0, &instance->surface));
}
static void create_device(vk_instance_t *instance) {
  float queue_priority = 1.0F;

  VkDeviceQueueCreateInfo device_queue_create_infos[2] = {
    {
      .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
      .queueFamilyIndex = instance->primary_queue_index,
      .queueCount = 1,
      .pQueuePriorities = &queue_priority,
    },
    {
      .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
      .queueFamilyIndex = instance->present_queue_index,
      .queueCount = 1,
      .pQueuePriorities = &queue_priority,
    },
  };

  VkDeviceCreateInfo device_create_info = {
    .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
    .pNext = &instance->physical_device_features2,
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

  TI_VK_CHECK(vkCreateDevice(instance->physical_device, &device_create_info, 0, &instance->device));

  vkGetDeviceQueue(instance->device, instance->primary_queue_index, 0, &instance->primary_queue);
  vkGetDeviceQueue(instance->device, instance->present_queue_index, 0, &instance->present_queue);

  instance->cmd_draw_mesh_tasks_ext_proc = (PFN_vkCmdDrawMeshTasksEXT)vkGetDeviceProcAddr(instance->device, "vkCmdDrawMeshTasksEXT");
  instance->cmd_trace_rays_khr_proc = (PFN_vkCmdTraceRaysKHR)vkGetDeviceProcAddr(instance->device, "vkCmdTraceRaysKHR");
  instance->cmd_build_acceleration_structures_khr_proc = (PFN_vkCmdBuildAccelerationStructuresKHR)vkGetDeviceProcAddr(instance->device, "vkCmdBuildAccelerationStructuresKHR");

  instance->create_acceleration_structure_khr_proc = (PFN_vkCreateAccelerationStructureKHR)vkGetDeviceProcAddr(instance->device, "vkCreateAccelerationStructureKHR");
  instance->create_ray_tracing_pipelines_khr_proc = (PFN_vkCreateRayTracingPipelinesKHR)vkGetDeviceProcAddr(instance->device, "vkCreateRayTracingPipelinesKHR");

  instance->get_acceleration_structure_build_sizes_khr_proc = (PFN_vkGetAccelerationStructureBuildSizesKHR)vkGetDeviceProcAddr(instance->device, "vkGetAccelerationStructureBuildSizesKHR");
  instance->get_acceleration_structure_device_address_khr_proc = (PFN_vkGetAccelerationStructureDeviceAddressKHR)vkGetDeviceProcAddr(instance->device, "vkGetAccelerationStructureDeviceAddressKHR");
  instance->get_ray_tracing_shader_group_handles_khr_proc = (PFN_vkGetRayTracingShaderGroupHandlesKHR)vkGetDeviceProcAddr(instance->device, "vkGetRayTracingShaderGroupHandlesKHR");

  instance->destroy_acceleration_structure_khr_proc = (PFN_vkDestroyAccelerationStructureKHR)vkGetDeviceProcAddr(instance->device, "vkDestroyAccelerationStructureKHR");
}
static void create_command_pool(vk_instance_t *instance) {
  VkCommandPoolCreateInfo command_pool_create_info = {
    .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
    .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
    .queueFamilyIndex = instance->primary_queue_index,
  };

  TI_VK_CHECK(vkCreateCommandPool(instance->device, &command_pool_create_info, 0, &instance->command_pool));
}
static void create_command_buffer(vk_instance_t *instance) {
  VkCommandBufferAllocateInfo command_buffer_allocate_info = {
    .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
    .commandPool = instance->command_pool,
    .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
    .commandBufferCount = 1,
  };

  TI_VK_CHECK(vkAllocateCommandBuffers(instance->device, &command_buffer_allocate_info, &instance->command_buffer));
}

static void find_physical_device(vk_instance_t *instance) {
  uint32_t physical_device_index = 0;
  uint32_t physical_device_count = 0;

  static VkPhysicalDevice physical_devices[TI_INSTANCE_MAX_PHYSICAL_DEVICES] = {0};

  TI_VK_CHECK(vkEnumeratePhysicalDevices(instance->instance, &physical_device_count, 0));
  TI_VK_CHECK(vkEnumeratePhysicalDevices(instance->instance, &physical_device_count, physical_devices));

  while (physical_device_index < physical_device_count) {

    VkPhysicalDevice physical_device = physical_devices[physical_device_index];

    instance->physical_device_features2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    instance->physical_device_features2.pNext = &g_physical_device_mesh_shader_features;

    instance->physical_device_properties2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    instance->physical_device_properties2.pNext = &g_physical_device_ray_tracing_pipeline_properties;

    instance->physical_device_memory_properties2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MEMORY_PROPERTIES_2;
    instance->physical_device_memory_properties2.pNext = 0;

    vkGetPhysicalDeviceFeatures2(physical_device, &instance->physical_device_features2);
    vkGetPhysicalDeviceProperties2(physical_device, &instance->physical_device_properties2);
    vkGetPhysicalDeviceMemoryProperties2(physical_device, &instance->physical_device_memory_properties2);

    if (instance->physical_device_properties2.properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {

      instance->physical_device = physical_device;

      break;
    }

    physical_device_index++;
  }

#ifdef BUILD_DEBUG
  VkPhysicalDeviceProperties *props = &instance->physical_device_properties2.properties;

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
static void find_physical_device_queue_families(vk_instance_t *instance) {
  int32_t primary_queue_index = -1;
  int32_t present_queue_index = -1;

  uint32_t queue_family_property_index = 0;
  uint32_t queue_family_property_count = 0;

  static VkQueueFamilyProperties queue_family_properties[TI_INSTANCE_MAX_QUEUE_FAMILY_PROPERTIES_COUNT] = {0};

  vkGetPhysicalDeviceQueueFamilyProperties(instance->physical_device, &queue_family_property_count, 0);
  vkGetPhysicalDeviceQueueFamilyProperties(instance->physical_device, &queue_family_property_count, queue_family_properties);

  while (queue_family_property_index < queue_family_property_count) {

    VkQueueFamilyProperties queue_family_property = queue_family_properties[queue_family_property_index];

    VkBool32 graphics_support = queue_family_property.queueFlags & VK_QUEUE_GRAPHICS_BIT;
    VkBool32 compute_support = queue_family_property.queueFlags & VK_QUEUE_COMPUTE_BIT;
    VkBool32 present_support = 0;

    TI_VK_CHECK(vkGetPhysicalDeviceSurfaceSupportKHR(instance->physical_device, (uint32_t)queue_family_property_index, instance->surface, &present_support));

    if (graphics_support && compute_support && (primary_queue_index == -1)) {

      primary_queue_index = queue_family_property_index;

    } else if (present_support && (present_queue_index == -1)) {

      present_queue_index = queue_family_property_index;
    }

    if ((primary_queue_index != -1) && (present_queue_index != -1)) {

      instance->primary_queue_index = (uint32_t)primary_queue_index;
      instance->present_queue_index = (uint32_t)present_queue_index;

      break;
    }

    queue_family_property_index++;
  }

#ifdef BUILD_DEBUG
  printf("Selected Physical Queues\n");
  printf("  Primary Queue Index: %d\n", instance->primary_queue_index);
  printf("  Present Queue Index: %d\n", instance->present_queue_index);
  printf("\n");
#endif // BUILD_DEBUG
}

static void check_physical_device_extensions(vk_instance_t *instance) {
  uint32_t available_device_extension_count = 0;

  static VkExtensionProperties available_extension_properties[TI_INSTANCE_MAX_EXTENSION_PROPERTIES_COUNT] = {0};

  TI_VK_CHECK(vkEnumerateDeviceExtensionProperties(instance->physical_device, 0, &available_device_extension_count, 0));
  TI_VK_CHECK(vkEnumerateDeviceExtensionProperties(instance->physical_device, 0, &available_device_extension_count, available_extension_properties));

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

static void destroy_instance(vk_instance_t *instance) {
#ifdef BUILD_DEBUG
  instance->destroy_debug_utils_messenger_ext_proc(instance->instance, instance->debug_utils_messenger, 0);
#endif // BUILD_DEBUG

  vkDestroyInstance(instance->instance, 0);
}
static void destroy_surface(vk_instance_t *instance) {
  vkDestroySurfaceKHR(instance->instance, instance->surface, 0);
}
static void destroy_device(vk_instance_t *instance) {
  vkDestroyDevice(instance->device, 0);
}
static void destroy_command_pool(vk_instance_t *instance) {
  vkDestroyCommandPool(instance->device, instance->command_pool, 0);
}
static void destroy_command_buffer(vk_instance_t *instance) {
  vkFreeCommandBuffers(instance->device, instance->command_pool, 1, &instance->command_buffer);
}
