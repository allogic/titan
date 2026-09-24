#include <ti_pch.h>

void vk_swapchain_create(vk_swapchain_t *swapchain, char const *asset_path) {
  swapchain->asset.path = asset_path;

  fs_asset_load(&swapchain->asset);

  fs_swapchain_t *config = (fs_swapchain_t *)swapchain->asset.instance;

  swapchain->image_count = clampu(TI_SWAPCHAIN_IMAGE_COUNT, g_vk_instance.min_image_count, g_vk_instance.max_image_count);

  uint32_t queue_families[2] = {
    g_vk_instance.primary_queue_index,
    g_vk_instance.present_queue_index,
  };

  VkSwapchainCreateInfoKHR swapchain_create_info = {
    .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
    .surface = g_vk_instance.surface,
    .minImageCount = swapchain->image_count,
    .imageFormat = VK_FORMAT_R8G8B8A8_UNORM,
    .imageColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR,
    .imageExtent.width = g_pl_window.window_width,
    .imageExtent.height = g_pl_window.window_height,
    .imageArrayLayers = 1,
    .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT,
    .preTransform = g_vk_instance.surface_transform,
    .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
    .presentMode = VK_PRESENT_MODE_MAILBOX_KHR,
    .clipped = 1,
    .oldSwapchain = 0,
    .imageSharingMode = VK_SHARING_MODE_CONCURRENT,
    .pQueueFamilyIndices = queue_families,
    .queueFamilyIndexCount = TI_ARRAY_COUNT(queue_families),
  };

  TI_VK_CHECK(vkCreateSwapchainKHR(g_vk_instance.device, &swapchain_create_info, 0, &swapchain->handle));
  TI_VK_CHECK(vkGetSwapchainImagesKHR(g_vk_instance.device, swapchain->handle, &swapchain->image_count, swapchain->image));
}
void vk_swapchain_destroy(vk_swapchain_t *swapchain) {
  vkDestroySwapchainKHR(g_vk_instance.device, swapchain->handle, 0);

  fs_asset_destroy(&swapchain->asset);
}
