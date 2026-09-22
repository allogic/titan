#include <ti_pch.h>

static void create_sync_object(vk_renderer_t *renderer);
static void create_descriptor_info(vk_renderer_t *renderer);
static void create_debug_line_buffer(vk_renderer_t *renderer);
static void create_full_screen_buffer(vk_renderer_t *renderer);

static void update_debug_line_descriptor_set(vk_renderer_t *renderer);
static void update_coherent_buffer(vk_renderer_t *renderer);

static void record_pre_compute_pass(vk_renderer_t *renderer);
static void record_compute_pass(vk_renderer_t *renderer);
static void record_post_compute_pass(vk_renderer_t *renderer);

static void record_pre_main_pass(vk_renderer_t *renderer);
static void record_main_pass(vk_renderer_t *renderer);
static void record_post_main_pass(vk_renderer_t *renderer);

static void record_pre_ray_tracing_pass(vk_renderer_t *renderer);
static void record_ray_tracing_pass(vk_renderer_t *renderer);
static void record_post_ray_tracing_pass(vk_renderer_t *renderer);

static void record_pre_imgui_pass(vk_renderer_t *renderer);
static void record_imgui_pass(vk_renderer_t *renderer);
static void record_post_imgui_pass(vk_renderer_t *renderer);

static void destroy_sync_object(vk_renderer_t *renderer);
static void destroy_buffer(vk_renderer_t *renderer);

// TODO: refactor all of this!

static vk_full_screen_vertex_t s_full_screen_vertices[] = {
  {-1.0F, -1.0F, 0.0F, 1.0F},
  {1.0F, -1.0F, 0.0F, 1.0F},
  {-1.0F, 1.0F, 0.0F, 1.0F},
  {1.0F, 1.0F, 0.0F, 1.0F},
};
static vk_full_screen_index_t s_full_screen_indices[] = {
  0,
  1,
  2,
  3,
  2,
  1,
};

static VkVertexInputBindingDescription s_full_screen_vertex_input_binding_description[] = {
  {
    .binding = 0,
    .stride = sizeof(vk_full_screen_vertex_t),
    .inputRate = VK_VERTEX_INPUT_RATE_VERTEX,
  },
};
static VkVertexInputBindingDescription s_debug_line_vertex_input_binding_description[] = {
  {
    .binding = 0,
    .stride = sizeof(vk_debug_line_vertex_t),
    .inputRate = VK_VERTEX_INPUT_RATE_VERTEX,
  },
};

static VkVertexInputAttributeDescription s_full_screen_vertex_input_attribute_description[] = {
  {
    .location = 0,
    .binding = 0,
    .format = VK_FORMAT_R32G32B32A32_SFLOAT,
    .offset = 0,
  },
};
static VkVertexInputAttributeDescription s_debug_line_vertex_input_attribute_description[] = {
  {
    .location = 0,
    .binding = 0,
    .format = VK_FORMAT_R32G32B32A32_SFLOAT,
    .offset = 0,
  },
  {
    .location = 1,
    .binding = 0,
    .format = VK_FORMAT_R32G32B32A32_SFLOAT,
    .offset = TI_OFFSET_OF(vk_debug_line_vertex_t, color),
  },
};

static VkDescriptorPoolSize s_debug_line_renderer_descriptor_pool_size[] = {
  {
    .type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
    .descriptorCount = 1,
  },
};

static VkDescriptorSetLayoutBinding s_debug_line_renderer_descriptor_set_layout_binding[] = {
  {
    .binding = 0,
    .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
    .descriptorCount = 1,
    .stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
    .pImmutableSamplers = 0,
  },
};

// static pipeline_t s_debug_line_renderer_pipeline = {
//   .pipeline_type = PIPELINE_TYPE_DFLT,
//   .vertex_shader = "asset/shader/debug/line/vertex.spv",
//   .fragment_shader = "asset/shader/debug/line/fragment.spv",
//   .primitive_topology = VK_PRIMITIVE_TOPOLOGY_LINE_LIST,
//   .polygon_mode = VK_POLYGON_MODE_FILL,
//   .cull_mode = VK_CULL_MODE_BACK_BIT,
//   .enable_blending = 1,
//   .enable_depth_test = 1,
//   .enable_depth_write = 1,
//   .vertex_input_binding_description = s_debug_line_vertex_input_binding_description,
//   .vertex_input_binding_description_count = TI_ARRAY_COUNT(s_debug_line_vertex_input_binding_description),
//   .vertex_input_attribute_description = s_debug_line_vertex_input_attribute_description,
//   .vertex_input_attribute_description_count = TI_ARRAY_COUNT(s_debug_line_vertex_input_attribute_description),
//   .descriptor_pool_size = s_debug_line_renderer_descriptor_pool_size,
//   .descriptor_pool_size_count = TI_ARRAY_COUNT(s_debug_line_renderer_descriptor_pool_size),
//   .descriptor_set_layout_binding = s_debug_line_renderer_descriptor_set_layout_binding,
//   .descriptor_set_layout_binding_count = TI_ARRAY_COUNT(s_debug_line_renderer_descriptor_set_layout_binding),
//   .descriptor_set_count = 1,
//   .render_pass = &g_renderpass,
// };

void vk_renderer_create(vk_renderer_t *renderer, char const *asset_path) {
  renderer->config = (fs_renderer_t *)fs_get(asset_path);

  create_sync_object(renderer);
  create_descriptor_info(renderer);
  create_debug_line_buffer(renderer);
  create_full_screen_buffer(renderer);

  // TODO
  // pipeline_create(&s_debug_line_renderer_pipeline);

  update_debug_line_descriptor_set(renderer);
}
void vk_renderer_draw(vk_renderer_t *renderer) {
  TI_VK_CHECK(vkWaitForFences(g_vk_instance.device, 1, &renderer->frame_fence, 1, UINT64_MAX));
  TI_VK_CHECK(vkResetFences(g_vk_instance.device, 1, &renderer->frame_fence));

  TI_VK_CHECK(vkAcquireNextImageKHR(g_vk_instance.device, g_vk_swapchain.handle, UINT64_MAX, renderer->image_available_semaphore, 0, &renderer->image_index));

  update_coherent_buffer(renderer);

  VkCommandBufferBeginInfo command_buffer_begin_info = {
    .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
    .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
    .pInheritanceInfo = 0,
  };

  TI_VK_CHECK(vkResetCommandBuffer(g_vk_instance.command_buffer, 0));
  TI_VK_CHECK(vkBeginCommandBuffer(g_vk_instance.command_buffer, &command_buffer_begin_info));

  record_pre_compute_pass(renderer);
  record_compute_pass(renderer);
  record_post_compute_pass(renderer);

  record_pre_main_pass(renderer);
  record_main_pass(renderer);
  record_post_main_pass(renderer);

  record_pre_ray_tracing_pass(renderer);
  record_ray_tracing_pass(renderer);
  record_post_ray_tracing_pass(renderer);

  record_pre_imgui_pass(renderer);
  record_imgui_pass(renderer);
  record_post_imgui_pass(renderer);

  {
    VkImageMemoryBarrier image_memory_barrier = {
      .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
      .srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
      .dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT,
      .oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
      .newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
      .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
      .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
      .image = g_vk_imgui_framebuffer.color_attachment[renderer->image_index].handle,
      .subresourceRange = {
        .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
        .baseMipLevel = 0,
        .levelCount = 1,
        .baseArrayLayer = 0,
        .layerCount = 1,
      },
    };

    vkCmdPipelineBarrier(g_vk_instance.command_buffer, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, 0, 0, 0, 1, &image_memory_barrier);
  }

  {
    VkImageMemoryBarrier image_memory_barrier = {
      .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
      .srcAccessMask = VK_ACCESS_NONE,
      .dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT,
      .oldLayout = VK_IMAGE_LAYOUT_UNDEFINED,
      .newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
      .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
      .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
      .image = g_vk_swapchain.image[renderer->image_index],
      .subresourceRange = {
        .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
        .baseMipLevel = 0,
        .levelCount = 1,
        .baseArrayLayer = 0,
        .layerCount = 1,
      },
    };

    vkCmdPipelineBarrier(g_vk_instance.command_buffer, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, 0, 0, 0, 1, &image_memory_barrier);
  }

  VkImageCopy image_copy = {
    .srcSubresource = {
      .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
      .baseArrayLayer = 0,
      .layerCount = 1,
      .mipLevel = 0,
    },
    .srcOffset = {
      .x = 0,
      .y = 0,
      .z = 0,
    },
    .dstSubresource = {
      .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
      .baseArrayLayer = 0,
      .layerCount = 1,
      .mipLevel = 0,
    },
    .dstOffset = {
      .x = 0,
      .y = 0,
      .z = 0,
    },
    .extent = {
      .width = g_pl_window.window_width,
      .height = g_pl_window.window_height,
      .depth = 1,
    },
  };

  vkCmdCopyImage(g_vk_instance.command_buffer, g_vk_imgui_framebuffer.color_attachment[renderer->image_index].handle, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, g_vk_swapchain.image[renderer->image_index], VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &image_copy);

  {
    VkImageMemoryBarrier image_memory_barrier = {
      .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
      .srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT,
      .dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
      .oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
      .newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
      .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
      .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
      .image = g_vk_imgui_framebuffer.color_attachment[renderer->image_index].handle,
      .subresourceRange = {
        .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
        .baseMipLevel = 0,
        .levelCount = 1,
        .baseArrayLayer = 0,
        .layerCount = 1,
      },
    };

    vkCmdPipelineBarrier(g_vk_instance.command_buffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, 0, 0, 0, 0, 0, 1, &image_memory_barrier);
  }

  {
    VkImageMemoryBarrier image_memory_barrier = {
      .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
      .srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT,
      .dstAccessMask = VK_ACCESS_NONE,
      .oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
      .newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
      .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
      .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
      .image = g_vk_swapchain.image[renderer->image_index],
      .subresourceRange = {
        .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
        .baseMipLevel = 0,
        .levelCount = 1,
        .baseArrayLayer = 0,
        .layerCount = 1,
      },
    };

    vkCmdPipelineBarrier(g_vk_instance.command_buffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, 0, 0, 0, 0, 0, 1, &image_memory_barrier);
  }

  TI_VK_CHECK(vkEndCommandBuffer(g_vk_instance.command_buffer));

  VkPipelineStageFlags primary_wait_stages[] = {
    VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
    VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
    VK_PIPELINE_STAGE_RAY_TRACING_SHADER_BIT_KHR,
    VK_PIPELINE_STAGE_TRANSFER_BIT,
  };

  VkSubmitInfo primary_submit_info = {
    .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
    .pWaitSemaphores = &renderer->image_available_semaphore,
    .waitSemaphoreCount = 1,
    .pSignalSemaphores = &renderer->render_finished_semaphore[renderer->image_index],
    .signalSemaphoreCount = 1,
    .pCommandBuffers = &g_vk_instance.command_buffer,
    .commandBufferCount = 1,
    .pWaitDstStageMask = primary_wait_stages,
  };

  TI_VK_CHECK(vkQueueSubmit(g_vk_instance.primary_queue, 1, &primary_submit_info, renderer->frame_fence));

  VkPresentInfoKHR present_info = {
    .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
    .pWaitSemaphores = &renderer->render_finished_semaphore[renderer->image_index],
    .waitSemaphoreCount = 1,
    .pSwapchains = &g_vk_swapchain.handle,
    .swapchainCount = 1,
    .pImageIndices = &renderer->image_index,
  };

  VkResult result = vkQueuePresentKHR(g_vk_instance.present_queue, &present_info);

  switch (result) {

    case VK_SUBOPTIMAL_KHR:
    case VK_ERROR_OUT_OF_DATE_KHR: {

      g_vk_swapchain.is_dirty = 1;

      return;
    }
  }
}
void vk_renderer_destroy(vk_renderer_t *renderer) {
  // pipeline_destroy(&s_debug_line_renderer_pipeline);

  destroy_buffer(renderer);
  destroy_sync_object(renderer);
}

void vk_renderer_draw_debug_line(vk_renderer_t *renderer, fvec3_t from, fvec3_t to, fvec4_t color) {
  if (renderer->is_debug_enabled) {

    uint32_t vertex_offset = renderer->debug_line_vertex_offset;
    uint32_t index_offset = renderer->debug_line_index_offset;

    if ((vertex_offset + 2) < TI_DEBUG_LINE_VERTEX_COUNT &&
        (index_offset + 2) < TI_DEBUG_LINE_INDEX_COUNT) {

      vk_debug_line_vertex_t *vertices = (vk_debug_line_vertex_t *)renderer->debug_line_vertex_buffer.device_data;
      vk_debug_line_index_t *indices = (vk_debug_line_index_t *)renderer->debug_line_index_buffer.device_data;

      vertices[vertex_offset + 0].position = (fvec4_t){from.x, from.y, from.z, 1.0F};
      vertices[vertex_offset + 1].position = (fvec4_t){to.x, to.y, to.z, 1.0F};

      vertices[vertex_offset + 0].color = color;
      vertices[vertex_offset + 1].color = color;

      indices[index_offset + 0] = (vk_debug_line_index_t)(vertex_offset + 0);
      indices[index_offset + 1] = (vk_debug_line_index_t)(vertex_offset + 1);

      renderer->debug_line_vertex_offset += 2;
      renderer->debug_line_index_offset += 2;
    }
  }
}
void vk_renderer_draw_debug_box(vk_renderer_t *renderer, fvec3_t position, fvec3_t size, fvec4_t color) {
  if (renderer->is_debug_enabled) {

    uint32_t vertex_offset = renderer->debug_line_vertex_offset;
    uint32_t index_offset = renderer->debug_line_index_offset;

    if ((vertex_offset + 8) < TI_DEBUG_LINE_VERTEX_COUNT &&
        (index_offset + 24) < TI_DEBUG_LINE_INDEX_COUNT) {

      vk_debug_line_vertex_t *vertices = (vk_debug_line_vertex_t *)renderer->debug_line_vertex_buffer.device_data;
      vk_debug_line_index_t *indices = (vk_debug_line_index_t *)renderer->debug_line_index_buffer.device_data;

      vertices[vertex_offset + 0].position = (fvec4_t){position.x, position.y, position.z, 1.0F};
      vertices[vertex_offset + 1].position = (fvec4_t){position.x, position.y + size.y, position.z, 1.00F};
      vertices[vertex_offset + 2].position = (fvec4_t){position.x + size.x, position.y, position.z, 1.0F};
      vertices[vertex_offset + 3].position = (fvec4_t){position.x + size.x, position.y + size.y, position.z, 1.0F};
      vertices[vertex_offset + 4].position = (fvec4_t){position.x, position.y, position.z + size.z, 1.0F};
      vertices[vertex_offset + 5].position = (fvec4_t){position.x, position.y + size.y, position.z + size.z, 1.0F};
      vertices[vertex_offset + 6].position = (fvec4_t){position.x + size.x, position.y, position.z + size.z, 1.0F};
      vertices[vertex_offset + 7].position = (fvec4_t){position.x + size.x, position.y + size.y, position.z + size.z, 1.0F};

      vertices[vertex_offset + 0].color = color;
      vertices[vertex_offset + 1].color = color;
      vertices[vertex_offset + 2].color = color;
      vertices[vertex_offset + 3].color = color;
      vertices[vertex_offset + 4].color = color;
      vertices[vertex_offset + 5].color = color;
      vertices[vertex_offset + 6].color = color;
      vertices[vertex_offset + 7].color = color;

      indices[index_offset + 0] = (vk_debug_line_index_t)(vertex_offset + 0);
      indices[index_offset + 1] = (vk_debug_line_index_t)(vertex_offset + 1);
      indices[index_offset + 2] = (vk_debug_line_index_t)(vertex_offset + 1);
      indices[index_offset + 3] = (vk_debug_line_index_t)(vertex_offset + 3);
      indices[index_offset + 4] = (vk_debug_line_index_t)(vertex_offset + 3);
      indices[index_offset + 5] = (vk_debug_line_index_t)(vertex_offset + 2);
      indices[index_offset + 6] = (vk_debug_line_index_t)(vertex_offset + 2);
      indices[index_offset + 7] = (vk_debug_line_index_t)(vertex_offset + 0);
      indices[index_offset + 8] = (vk_debug_line_index_t)(vertex_offset + 4);
      indices[index_offset + 9] = (vk_debug_line_index_t)(vertex_offset + 5);
      indices[index_offset + 10] = (vk_debug_line_index_t)(vertex_offset + 5);
      indices[index_offset + 11] = (vk_debug_line_index_t)(vertex_offset + 7);
      indices[index_offset + 12] = (vk_debug_line_index_t)(vertex_offset + 7);
      indices[index_offset + 13] = (vk_debug_line_index_t)(vertex_offset + 6);
      indices[index_offset + 14] = (vk_debug_line_index_t)(vertex_offset + 6);
      indices[index_offset + 15] = (vk_debug_line_index_t)(vertex_offset + 4);
      indices[index_offset + 16] = (vk_debug_line_index_t)(vertex_offset + 0);
      indices[index_offset + 17] = (vk_debug_line_index_t)(vertex_offset + 4);
      indices[index_offset + 18] = (vk_debug_line_index_t)(vertex_offset + 1);
      indices[index_offset + 19] = (vk_debug_line_index_t)(vertex_offset + 5);
      indices[index_offset + 20] = (vk_debug_line_index_t)(vertex_offset + 2);
      indices[index_offset + 21] = (vk_debug_line_index_t)(vertex_offset + 6);
      indices[index_offset + 22] = (vk_debug_line_index_t)(vertex_offset + 3);
      indices[index_offset + 23] = (vk_debug_line_index_t)(vertex_offset + 7);

      renderer->debug_line_vertex_offset += 8;
      renderer->debug_line_index_offset += 24;
    }
  }
}

static void create_sync_object(vk_renderer_t *renderer) {
  VkSemaphoreCreateInfo semaphore_create_info = {
    .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
    .flags = 0,
  };

  VkFenceCreateInfo fence_create_info = {
    .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
    .flags = VK_FENCE_CREATE_SIGNALED_BIT,
  };

  uint32_t image_index = 0;
  uint32_t image_count = g_vk_swapchain.image_count;

  while (image_index < image_count) {

    TI_VK_CHECK(vkCreateSemaphore(g_vk_instance.device, &semaphore_create_info, 0, &renderer->render_finished_semaphore[image_index]));

    image_index++;
  }

  TI_VK_CHECK(vkCreateSemaphore(g_vk_instance.device, &semaphore_create_info, 0, &renderer->image_available_semaphore));
  TI_VK_CHECK(vkCreateFence(g_vk_instance.device, &fence_create_info, 0, &renderer->frame_fence));
}
static void create_descriptor_info(vk_renderer_t *renderer) {
  renderer->time_info_descriptor_buffer_info.offset = 0;
  renderer->time_info_descriptor_buffer_info.buffer = g_vk_time_info_buffer.buffer_handle;
  renderer->time_info_descriptor_buffer_info.range = VK_WHOLE_SIZE;

  renderer->screen_info_descriptor_buffer_info.offset = 0;
  renderer->screen_info_descriptor_buffer_info.buffer = g_vk_screen_info_buffer.buffer_handle;
  renderer->screen_info_descriptor_buffer_info.range = VK_WHOLE_SIZE;

  renderer->mouse_info_descriptor_buffer_info.offset = 0;
  renderer->mouse_info_descriptor_buffer_info.buffer = g_vk_mouse_info_buffer.buffer_handle;
  renderer->mouse_info_descriptor_buffer_info.range = VK_WHOLE_SIZE;

  renderer->camera_info_descriptor_buffer_info.offset = 0;
  renderer->camera_info_descriptor_buffer_info.buffer = g_vk_camera_info_buffer.buffer_handle;
  renderer->camera_info_descriptor_buffer_info.range = VK_WHOLE_SIZE;
}
static void create_debug_line_buffer(vk_renderer_t *renderer) {
  vk_buffer_create(&renderer->debug_line_vertex_buffer, renderer->config->debug_line_vertex_buffer);
  vk_buffer_create(&renderer->debug_line_index_buffer, renderer->config->debug_line_index_buffer);

  vk_buffer_map(&renderer->debug_line_vertex_buffer);
  vk_buffer_map(&renderer->debug_line_index_buffer);
}
static void create_full_screen_buffer(vk_renderer_t *renderer) {
  vk_buffer_create(&renderer->full_screen_vertex_buffer, renderer->config->full_screen_vertex_buffer);
  vk_buffer_create(&renderer->full_screen_index_buffer, renderer->config->full_screen_index_buffer);
}

static void update_debug_line_descriptor_set(vk_renderer_t *renderer) {
  // TODO
  // VkWriteDescriptorSet write_descriptor_set[] = {
  //   {
  //     .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
  //     .pNext = 0,
  //     .dstSet = s_debug_line_renderer_pipeline.descriptor_set[0],
  //     .dstBinding = 0,
  //     .dstArrayElement = 0,
  //     .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
  //     .descriptorCount = 1,
  //     .pImageInfo = 0,
  //     .pBufferInfo = &renderer->camera_info_descriptor_buffer_info,
  //     .pTexelBufferView = 0,
  //   },
  // };
  //
  // vkUpdateDescriptorSets(g_pl_window.device, TI_ARRAY_COUNT(write_descriptor_set), write_descriptor_set, 0, 0);
}
static void update_coherent_buffer(vk_renderer_t *renderer) {
  vk_time_info_t *time_info = (vk_time_info_t *)g_vk_time_info_buffer.device_data;
  vk_screen_info_t *screen_info = (vk_screen_info_t *)g_vk_screen_info_buffer.device_data;
  vk_mouse_info_t *mouse_info = (vk_mouse_info_t *)g_vk_mouse_info_buffer.device_data;
  vk_camera_info_t *camera_info = (vk_camera_info_t *)g_vk_camera_info_buffer.device_data;

  time_info->time = g_pl_window.time;
  time_info->delta_time = g_pl_window.delta_time;

  screen_info->resolution = (ivec2_t){g_pl_window.window_width, g_pl_window.window_height};

  mouse_info->position = (ivec2_t){g_pl_window.mouse_position_x, g_pl_window.mouse_position_y};

  fvec3_t camera_position = {0.0F, 0.0F, -10.0F}; // g_player.transform.world_position;
  fvec3_t camera_direction = {0.0F, 0.0F, 1.0F};  // quaternion_front(g_player.transform.world_rotation);

  camera_info->position = (fvec4_t){camera_position.x, camera_position.y, camera_position.z, 0.0F};
  camera_info->direction = (fvec4_t){camera_direction.x, camera_direction.y, camera_direction.z, 0.0F};
  camera_info->view = fmat4x4_identity();                // g_player.camera.view;
  camera_info->view_inv = fmat4x4_identity();            // g_player.camera.view_inv;
  camera_info->projection = fmat4x4_identity();          // g_player.camera.projection;
  camera_info->projection_inv = fmat4x4_identity();      // g_player.camera.projection_inv;
  camera_info->view_projection = fmat4x4_identity();     // g_player.camera.view_projection;
  camera_info->view_projection_inv = fmat4x4_identity(); // g_player.camera.view_projection_inv;
}

static void record_pre_compute_pass(vk_renderer_t *renderer) {
  // TODO
}
static void record_compute_pass(vk_renderer_t *renderer) {
  // TODO
}
static void record_post_compute_pass(vk_renderer_t *renderer) {
  // TODO
}

static void record_pre_main_pass(vk_renderer_t *renderer) {
  // TODO
}
static void record_main_pass(vk_renderer_t *renderer) {
  VkClearValue color_clear_value = {
    .color.float32 = {
      0.0F,
      0.0F,
      0.0F,
      1.0F,
    },
  };

  VkClearValue depth_clear_value = {
    .depthStencil = {
      .depth = 1.0F,
      .stencil = 0,
    },
  };

  VkClearValue clear_values[] = {
    color_clear_value,
    depth_clear_value,
  };

  VkRenderPassBeginInfo render_pass_create_info = {
    .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
    .renderPass = g_vk_main_renderpass.handle,
    .framebuffer = g_vk_main_framebuffer.handle[renderer->image_index],
    .renderArea = {
      .offset.x = 0,
      .offset.y = 0,
      .extent = {
        .width = g_vk_main_framebuffer.width,
        .height = g_vk_main_framebuffer.height,
      },
    },
    .pClearValues = clear_values,
    .clearValueCount = TI_ARRAY_COUNT(clear_values),
  };

  vkCmdBeginRenderPass(g_vk_instance.command_buffer, &render_pass_create_info, VK_SUBPASS_CONTENTS_INLINE);

  VkViewport viewport = {
    .x = 0.0F,
    .y = 0.0F,
    .width = (float)g_vk_main_framebuffer.width,
    .height = (float)g_vk_main_framebuffer.height,
    .minDepth = 0.0F,
    .maxDepth = 1.0F,
  };

  vkCmdSetViewport(g_vk_instance.command_buffer, 0, 1, &viewport);

  VkRect2D scissor = {
    .offset.x = 0,
    .offset.y = 0,
    .extent = {
      .width = g_vk_main_framebuffer.width,
      .height = g_vk_main_framebuffer.height,
    },
  };

  vkCmdSetScissor(g_vk_instance.command_buffer, 0, 1, &scissor);

  if (renderer->is_debug_enabled) {

    VkDeviceSize vertex_offset = 0;

    // TODO
    // vkCmdBindPipeline(g_vk_instance.command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, s_debug_line_renderer_pipeline.pipeline_handle);
    // vkCmdBindVertexBuffers(g_vk_instance.command_buffer, 0, 1, &s_debug_line_vertex_buffer.buffer_handle, &vertex_offset);
    // vkCmdBindIndexBuffer(g_vk_instance.command_buffer, s_debug_line_index_buffer.buffer_handle, 0, VK_INDEX_TYPE_UINT32);
    // vkCmdBindDescriptorSets(g_vk_instance.command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, s_debug_line_renderer_pipeline.pipeline_layout, 0, 1, &s_debug_line_renderer_pipeline.descriptor_set[0], 0, 0);
    // vkCmdDrawIndexed(g_vk_instance.command_buffer, s_debug_line_index_offset, 1, 0, 0, 0);

    renderer->debug_line_vertex_offset = 0;
    renderer->debug_line_index_offset = 0;
  }

  vkCmdEndRenderPass(g_vk_instance.command_buffer);
}
static void record_post_main_pass(vk_renderer_t *renderer) {
  // TODO
}

static void record_pre_ray_tracing_pass(vk_renderer_t *renderer) {
  {
    VkImageMemoryBarrier image_memory_barrier = {
      .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
      .srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
      .dstAccessMask = VK_ACCESS_SHADER_WRITE_BIT,
      .oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
      .newLayout = VK_IMAGE_LAYOUT_GENERAL,
      .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
      .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
      .image = g_vk_main_framebuffer.color_attachment[renderer->image_index].handle,
      .subresourceRange = {
        .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
        .baseMipLevel = 0,
        .levelCount = 1,
        .baseArrayLayer = 0,
        .layerCount = 1,
      },
    };

    vkCmdPipelineBarrier(g_vk_instance.command_buffer, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_PIPELINE_STAGE_RAY_TRACING_SHADER_BIT_KHR, 0, 0, 0, 0, 0, 1, &image_memory_barrier);
  }

  {
    VkImageMemoryBarrier image_memory_barrier = {
      .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
      .srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
      .dstAccessMask = VK_ACCESS_SHADER_READ_BIT,
      .oldLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
      .newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
      .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
      .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
      .image = g_vk_main_framebuffer.depth_attachment[renderer->image_index].handle,
      .subresourceRange = {
        .aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT,
        .baseMipLevel = 0,
        .levelCount = 1,
        .baseArrayLayer = 0,
        .layerCount = 1,
      },
    };

    vkCmdPipelineBarrier(g_vk_instance.command_buffer, VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT, VK_PIPELINE_STAGE_RAY_TRACING_SHADER_BIT_KHR, 0, 0, 0, 0, 0, 1, &image_memory_barrier);
  }
}
static void record_ray_tracing_pass(vk_renderer_t *renderer) {
  // TODO
}
static void record_post_ray_tracing_pass(vk_renderer_t *renderer) {
  {
    VkImageMemoryBarrier image_memory_barrier = {
      .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
      .srcAccessMask = VK_ACCESS_SHADER_WRITE_BIT,
      .dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
      .oldLayout = VK_IMAGE_LAYOUT_GENERAL,
      .newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
      .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
      .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
      .image = g_vk_main_framebuffer.color_attachment[renderer->image_index].handle,
      .subresourceRange = {
        .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
        .baseMipLevel = 0,
        .levelCount = 1,
        .baseArrayLayer = 0,
        .layerCount = 1,
      },
    };

    vkCmdPipelineBarrier(g_vk_instance.command_buffer, VK_PIPELINE_STAGE_RAY_TRACING_SHADER_BIT_KHR, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, 0, 0, 0, 0, 0, 1, &image_memory_barrier);
  }

  {
    VkImageMemoryBarrier image_memory_barrier = {
      .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
      .srcAccessMask = VK_ACCESS_SHADER_READ_BIT,
      .dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
      .oldLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
      .newLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
      .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
      .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
      .image = g_vk_main_framebuffer.depth_attachment[renderer->image_index].handle,
      .subresourceRange = {
        .aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT,
        .baseMipLevel = 0,
        .levelCount = 1,
        .baseArrayLayer = 0,
        .layerCount = 1,
      },
    };

    vkCmdPipelineBarrier(g_vk_instance.command_buffer, VK_PIPELINE_STAGE_RAY_TRACING_SHADER_BIT_KHR, VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT, 0, 0, 0, 0, 0, 1, &image_memory_barrier);
  }
}

static void record_pre_imgui_pass(vk_renderer_t *renderer) {
  {
    VkImageMemoryBarrier image_memory_barrier = {
      .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
      .srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
      .dstAccessMask = VK_ACCESS_SHADER_READ_BIT,
      .oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
      .newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
      .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
      .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
      .image = g_vk_main_framebuffer.color_attachment[g_vk_renderer.image_index].handle,
      .subresourceRange = {
        .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
        .baseMipLevel = 0,
        .levelCount = 1,
        .baseArrayLayer = 0,
        .layerCount = 1,
      },
    };

    vkCmdPipelineBarrier(g_vk_instance.command_buffer, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, 0, 0, 0, 1, &image_memory_barrier);
  }

  {
    VkImageMemoryBarrier image_memory_barrier = {
      .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
      .srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
      .dstAccessMask = VK_ACCESS_SHADER_READ_BIT,
      .oldLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
      .newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
      .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
      .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
      .image = g_vk_main_framebuffer.depth_attachment[g_vk_renderer.image_index].handle,
      .subresourceRange = {
        .aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT,
        .baseMipLevel = 0,
        .levelCount = 1,
        .baseArrayLayer = 0,
        .layerCount = 1,
      },
    };

    vkCmdPipelineBarrier(g_vk_instance.command_buffer, VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, 0, 0, 0, 1, &image_memory_barrier);
  }
}
static void record_imgui_pass(vk_renderer_t *renderer) {
  VkClearValue color_clear_value = {
    .color.float32 = {
      0.0F,
      0.0F,
      0.0F,
      1.0F,
    },
  };

  VkClearValue depth_clear_value = {
    .depthStencil = {
      .depth = 1.0F,
      .stencil = 0,
    },
  };

  VkClearValue clear_values[] = {
    color_clear_value,
    depth_clear_value,
  };

  VkRenderPassBeginInfo render_pass_create_info = {
    .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
    .renderPass = g_vk_imgui_renderpass.handle,
    .framebuffer = g_vk_imgui_framebuffer.handle[renderer->image_index],
    .renderArea = {
      .offset.x = 0,
      .offset.y = 0,
      .extent = {
        .width = g_vk_imgui_framebuffer.width,
        .height = g_vk_imgui_framebuffer.height,
      },
    },
    .pClearValues = clear_values,
    .clearValueCount = TI_ARRAY_COUNT(clear_values),
  };

  vkCmdBeginRenderPass(g_vk_instance.command_buffer, &render_pass_create_info, VK_SUBPASS_CONTENTS_INLINE);

  VkViewport viewport = {
    .x = 0.0F,
    .y = 0.0F,
    .width = (float)g_vk_imgui_framebuffer.width,
    .height = (float)g_vk_imgui_framebuffer.height,
    .minDepth = 0.0F,
    .maxDepth = 1.0F,
  };

  vkCmdSetViewport(g_vk_instance.command_buffer, 0, 1, &viewport);

  VkRect2D scissor = {
    .offset.x = 0,
    .offset.y = 0,
    .extent = {
      .width = g_vk_imgui_framebuffer.width,
      .height = g_vk_imgui_framebuffer.height,
    },
  };

  vkCmdSetScissor(g_vk_instance.command_buffer, 0, 1, &scissor);

  im_draw();

  vkCmdEndRenderPass(g_vk_instance.command_buffer);
}
static void record_post_imgui_pass(vk_renderer_t *renderer) {
  {
    VkImageMemoryBarrier image_memory_barrier = {
      .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
      .srcAccessMask = VK_ACCESS_SHADER_READ_BIT,
      .dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
      .oldLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
      .newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
      .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
      .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
      .image = g_vk_main_framebuffer.color_attachment[g_vk_renderer.image_index].handle,
      .subresourceRange = {
        .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
        .baseMipLevel = 0,
        .levelCount = 1,
        .baseArrayLayer = 0,
        .layerCount = 1,
      },
    };

    vkCmdPipelineBarrier(g_vk_instance.command_buffer, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, 0, 0, 0, 0, 0, 1, &image_memory_barrier);
  }

  {
    VkImageMemoryBarrier image_memory_barrier = {
      .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
      .srcAccessMask = VK_ACCESS_SHADER_READ_BIT,
      .dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
      .oldLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
      .newLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
      .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
      .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
      .image = g_vk_main_framebuffer.depth_attachment[renderer->image_index].handle,
      .subresourceRange = {
        .aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT,
        .baseMipLevel = 0,
        .levelCount = 1,
        .baseArrayLayer = 0,
        .layerCount = 1,
      },
    };

    vkCmdPipelineBarrier(g_vk_instance.command_buffer, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT, 0, 0, 0, 0, 0, 1, &image_memory_barrier);
  }
}

static void destroy_sync_object(vk_renderer_t *renderer) {
  uint32_t image_index = 0;
  uint32_t image_count = g_vk_swapchain.image_count;

  while (image_index < image_count) {

    vkDestroySemaphore(g_vk_instance.device, renderer->render_finished_semaphore[image_index], 0);

    image_index++;
  }

  vkDestroySemaphore(g_vk_instance.device, renderer->image_available_semaphore, 0);
  vkDestroyFence(g_vk_instance.device, renderer->frame_fence, 0);
}
static void destroy_buffer(vk_renderer_t *renderer) {
  vk_buffer_destroy(&renderer->debug_line_vertex_buffer);
  vk_buffer_destroy(&renderer->debug_line_index_buffer);
  vk_buffer_destroy(&renderer->full_screen_vertex_buffer);
  vk_buffer_destroy(&renderer->full_screen_index_buffer);
}
