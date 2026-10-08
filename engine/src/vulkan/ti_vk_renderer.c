#include <vulkan/ti_vk_renderer.h>

static void create_sync_object(vk_renderer_t *renderer);

static void update_descriptor_info(vk_renderer_t *renderer);
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

void vk_renderer_create(vk_renderer_t *renderer) {
  renderer->is_debug_enabled = 1; // TODO

  create_sync_object(renderer);

  renderer->main_renderpass_hdl = adb_handle(renderer->hash, TI_JSON_STRING(renderer->config, "main_renderpass"));
  renderer->imgui_renderpass_hdl = adb_handle(renderer->hash, TI_JSON_STRING(renderer->config, "imgui_renderpass"));
  renderer->main_framebuffer_hdl = adb_handle(renderer->hash, TI_JSON_STRING(renderer->config, "main_framebuffer"));
  renderer->imgui_framebuffer_hdl = adb_handle(renderer->hash, TI_JSON_STRING(renderer->config, "imgui_framebuffer"));
  renderer->debug_line_vertex_buffer_hdl = adb_handle(renderer->hash, TI_JSON_STRING(renderer->config, "debug_line_vertex_buffer"));
  renderer->debug_line_index_buffer_hdl = adb_handle(renderer->hash, TI_JSON_STRING(renderer->config, "debug_line_index_buffer"));
  renderer->full_screen_vertex_buffer_hdl = adb_handle(renderer->hash, TI_JSON_STRING(renderer->config, "full_screen_vertex_buffer"));
  renderer->full_screen_index_buffer_hdl = adb_handle(renderer->hash, TI_JSON_STRING(renderer->config, "full_screen_index_buffer"));
  renderer->debug_line_pipeline_hdl = adb_handle(renderer->hash, TI_JSON_STRING(renderer->config, "debug_line_pipeline"));

  update_descriptor_info(renderer);
  update_debug_line_descriptor_set(renderer);
}
void vk_renderer_draw(vk_renderer_t *renderer) {
  VkResult result = VK_SUCCESS;

  TI_VK_CHECK(vkWaitForFences(g_vk_instance->device, 1, &renderer->frame_fence, 1, UINT64_MAX));

  result = vkAcquireNextImageKHR(g_vk_instance->device, g_vk_swapchain->swapchain, UINT64_MAX, renderer->image_available_semaphore, 0, &renderer->image_index);

  switch (result) {

    case VK_SUBOPTIMAL_KHR:
    case VK_ERROR_OUT_OF_DATE_KHR: {

      g_vk_swapchain->is_dirty = 1;

      return;
    }
  }

  update_coherent_buffer(renderer);

  VkCommandBufferBeginInfo command_buffer_begin_info = {
    .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
    .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
    .pInheritanceInfo = 0,
  };

  TI_VK_CHECK(vkResetCommandBuffer(g_vk_instance->command_buffer, 0));
  TI_VK_CHECK(vkBeginCommandBuffer(g_vk_instance->command_buffer, &command_buffer_begin_info));

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

  vk_framebuffer_t *imgui_framebuffer = (vk_framebuffer_t *)renderer->imgui_framebuffer_hdl->instance;
  vk_image_t *color_attachment = (vk_image_t *)imgui_framebuffer->color_attachment_hdl[0]->instance; // TODO: use renderer->image_index if double buffering is enabled..

  {
    VkImageMemoryBarrier image_memory_barrier = {
      .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
      .srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
      .dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT,
      .oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
      .newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
      .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
      .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
      .image = color_attachment->image,
      .subresourceRange = {
        .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
        .baseMipLevel = 0,
        .levelCount = 1,
        .baseArrayLayer = 0,
        .layerCount = 1,
      },
    };

    vkCmdPipelineBarrier(g_vk_instance->command_buffer, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, 0, 0, 0, 1, &image_memory_barrier);
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
      .image = g_vk_swapchain->image[renderer->image_index],
      .subresourceRange = {
        .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
        .baseMipLevel = 0,
        .levelCount = 1,
        .baseArrayLayer = 0,
        .layerCount = 1,
      },
    };

    vkCmdPipelineBarrier(g_vk_instance->command_buffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, 0, 0, 0, 1, &image_memory_barrier);
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
      .width = g_window.width,
      .height = g_window.height,
      .depth = 1,
    },
  };

  vkCmdCopyImage(g_vk_instance->command_buffer, color_attachment->image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, g_vk_swapchain->image[renderer->image_index], VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &image_copy);

  {
    VkImageMemoryBarrier image_memory_barrier = {
      .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
      .srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT,
      .dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
      .oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
      .newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
      .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
      .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
      .image = color_attachment->image,
      .subresourceRange = {
        .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
        .baseMipLevel = 0,
        .levelCount = 1,
        .baseArrayLayer = 0,
        .layerCount = 1,
      },
    };

    vkCmdPipelineBarrier(g_vk_instance->command_buffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, 0, 0, 0, 0, 0, 1, &image_memory_barrier);
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
      .image = g_vk_swapchain->image[renderer->image_index],
      .subresourceRange = {
        .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
        .baseMipLevel = 0,
        .levelCount = 1,
        .baseArrayLayer = 0,
        .layerCount = 1,
      },
    };

    vkCmdPipelineBarrier(g_vk_instance->command_buffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, 0, 0, 0, 0, 0, 1, &image_memory_barrier);
  }

  TI_VK_CHECK(vkEndCommandBuffer(g_vk_instance->command_buffer));

  VkPipelineStageFlags primary_wait_stages[] = {
    VK_PIPELINE_STAGE_TRANSFER_BIT,
  };

  VkSubmitInfo primary_submit_info = {
    .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
    .pWaitSemaphores = &renderer->image_available_semaphore,
    .waitSemaphoreCount = 1,
    .pSignalSemaphores = &renderer->render_finished_semaphore[renderer->image_index],
    .signalSemaphoreCount = 1,
    .pCommandBuffers = &g_vk_instance->command_buffer,
    .commandBufferCount = 1,
    .pWaitDstStageMask = primary_wait_stages,
  };

  TI_VK_CHECK(vkResetFences(g_vk_instance->device, 1, &renderer->frame_fence));
  TI_VK_CHECK(vkQueueSubmit(g_vk_instance->primary_queue, 1, &primary_submit_info, renderer->frame_fence));

  VkPresentInfoKHR present_info = {
    .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
    .pWaitSemaphores = &renderer->render_finished_semaphore[renderer->image_index],
    .waitSemaphoreCount = 1,
    .pSwapchains = &g_vk_swapchain->swapchain,
    .swapchainCount = 1,
    .pImageIndices = &renderer->image_index,
  };

  result = vkQueuePresentKHR(g_vk_instance->present_queue, &present_info);

  switch (result) {

    case VK_SUBOPTIMAL_KHR:
    case VK_ERROR_OUT_OF_DATE_KHR: {

      g_vk_swapchain->is_dirty = 1;

      return;
    }
  }
}
void vk_renderer_destroy(vk_renderer_t *renderer) {
  destroy_sync_object(renderer);
}

void vk_renderer_draw_debug_line(vk_renderer_t *renderer, fvec3_t from, fvec3_t to, fvec4_t color) {
  if (renderer->is_debug_enabled) {

    uint32_t vertex_offset = renderer->debug_line_vertex_offset;
    uint32_t index_offset = renderer->debug_line_index_offset;

    if ((vertex_offset + 2) < TI_DEBUG_LINE_VERTEX_COUNT &&
        (index_offset + 2) < TI_DEBUG_LINE_INDEX_COUNT) {

      vk_buffer_t *vertex_buffer = (vk_buffer_t *)renderer->debug_line_vertex_buffer_hdl->instance;
      vk_buffer_t *index_buffer = (vk_buffer_t *)renderer->debug_line_index_buffer_hdl->instance;

      vk_debug_line_vertex_t *vertices = (vk_debug_line_vertex_t *)vertex_buffer->device_data;
      vk_debug_line_index_t *indices = (vk_debug_line_index_t *)index_buffer->device_data;

      vertices[vertex_offset + 0].position = (fvec4_t){from.x, from.y, from.z, 1.0F};
      vertices[vertex_offset + 1].position = (fvec4_t){to.x, to.y, to.z, 1.0F};

      vertices[vertex_offset + 0].color = color;
      vertices[vertex_offset + 1].color = color;

      indices[index_offset + 0] = (vk_debug_line_index_t){vertex_offset + 0};
      indices[index_offset + 1] = (vk_debug_line_index_t){vertex_offset + 1};

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

      vk_buffer_t *vertex_buffer = (vk_buffer_t *)renderer->debug_line_vertex_buffer_hdl->instance;
      vk_buffer_t *index_buffer = (vk_buffer_t *)renderer->debug_line_index_buffer_hdl->instance;

      vk_debug_line_vertex_t *vertices = (vk_debug_line_vertex_t *)vertex_buffer->device_data;
      vk_debug_line_index_t *indices = (vk_debug_line_index_t *)index_buffer->device_data;

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

      indices[index_offset + 0] = (vk_debug_line_index_t){vertex_offset + 0};
      indices[index_offset + 1] = (vk_debug_line_index_t){vertex_offset + 1};
      indices[index_offset + 2] = (vk_debug_line_index_t){vertex_offset + 1};
      indices[index_offset + 3] = (vk_debug_line_index_t){vertex_offset + 3};
      indices[index_offset + 4] = (vk_debug_line_index_t){vertex_offset + 3};
      indices[index_offset + 5] = (vk_debug_line_index_t){vertex_offset + 2};
      indices[index_offset + 6] = (vk_debug_line_index_t){vertex_offset + 2};
      indices[index_offset + 7] = (vk_debug_line_index_t){vertex_offset + 0};
      indices[index_offset + 8] = (vk_debug_line_index_t){vertex_offset + 4};
      indices[index_offset + 9] = (vk_debug_line_index_t){vertex_offset + 5};
      indices[index_offset + 10] = (vk_debug_line_index_t){vertex_offset + 5};
      indices[index_offset + 11] = (vk_debug_line_index_t){vertex_offset + 7};
      indices[index_offset + 12] = (vk_debug_line_index_t){vertex_offset + 7};
      indices[index_offset + 13] = (vk_debug_line_index_t){vertex_offset + 6};
      indices[index_offset + 14] = (vk_debug_line_index_t){vertex_offset + 6};
      indices[index_offset + 15] = (vk_debug_line_index_t){vertex_offset + 4};
      indices[index_offset + 16] = (vk_debug_line_index_t){vertex_offset + 0};
      indices[index_offset + 17] = (vk_debug_line_index_t){vertex_offset + 4};
      indices[index_offset + 18] = (vk_debug_line_index_t){vertex_offset + 1};
      indices[index_offset + 19] = (vk_debug_line_index_t){vertex_offset + 5};
      indices[index_offset + 20] = (vk_debug_line_index_t){vertex_offset + 2};
      indices[index_offset + 21] = (vk_debug_line_index_t){vertex_offset + 6};
      indices[index_offset + 22] = (vk_debug_line_index_t){vertex_offset + 3};
      indices[index_offset + 23] = (vk_debug_line_index_t){vertex_offset + 7};

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
  uint32_t image_count = g_vk_swapchain->image_count;

  while (image_index < image_count) {

    TI_VK_CHECK(vkCreateSemaphore(g_vk_instance->device, &semaphore_create_info, 0, &renderer->render_finished_semaphore[image_index]));

    image_index++;
  }

  TI_VK_CHECK(vkCreateSemaphore(g_vk_instance->device, &semaphore_create_info, 0, &renderer->image_available_semaphore));
  TI_VK_CHECK(vkCreateFence(g_vk_instance->device, &fence_create_info, 0, &renderer->frame_fence));
}

static void update_descriptor_info(vk_renderer_t *renderer) {
  renderer->time_info_descriptor_buffer_info.offset = 0;
  renderer->time_info_descriptor_buffer_info.buffer = g_vk_time_info_buffer->buffer;
  renderer->time_info_descriptor_buffer_info.range = VK_WHOLE_SIZE;

  renderer->screen_info_descriptor_buffer_info.offset = 0;
  renderer->screen_info_descriptor_buffer_info.buffer = g_vk_screen_info_buffer->buffer;
  renderer->screen_info_descriptor_buffer_info.range = VK_WHOLE_SIZE;

  renderer->mouse_info_descriptor_buffer_info.offset = 0;
  renderer->mouse_info_descriptor_buffer_info.buffer = g_vk_mouse_info_buffer->buffer;
  renderer->mouse_info_descriptor_buffer_info.range = VK_WHOLE_SIZE;

  renderer->camera_info_descriptor_buffer_info.offset = 0;
  renderer->camera_info_descriptor_buffer_info.buffer = g_vk_camera_info_buffer->buffer;
  renderer->camera_info_descriptor_buffer_info.range = VK_WHOLE_SIZE;
}
static void update_debug_line_descriptor_set(vk_renderer_t *renderer) {
  vk_pipeline_t *debug_line_pipeline = (vk_pipeline_t *)renderer->debug_line_pipeline_hdl->instance;

  VkWriteDescriptorSet write_descriptor_set[] = {
    {
      .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
      .pNext = 0,
      .dstSet = debug_line_pipeline->descriptor_set[0],
      .dstBinding = 0,
      .dstArrayElement = 0,
      .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
      .descriptorCount = 1,
      .pImageInfo = 0,
      .pBufferInfo = &renderer->camera_info_descriptor_buffer_info,
      .pTexelBufferView = 0,
    },
  };

  vkUpdateDescriptorSets(g_vk_instance->device, TI_ARRAY_COUNT(write_descriptor_set), write_descriptor_set, 0, 0);
}
static void update_coherent_buffer(vk_renderer_t *renderer) {
  // TODO: use ecs_valid or something..
  if (g_scene.main_camera_entity) {

    cp_transform_t const *transform = ecs_get(g_scene.world, g_scene.main_camera_entity, cp_transform_t);
    cp_camera_t const *camera = ecs_get(g_scene.world, g_scene.main_camera_entity, cp_camera_t);

    if (transform && camera) {

      vk_time_info_t *time_info = (vk_time_info_t *)g_vk_time_info_buffer->device_data;
      vk_screen_info_t *screen_info = (vk_screen_info_t *)g_vk_screen_info_buffer->device_data;
      vk_mouse_info_t *mouse_info = (vk_mouse_info_t *)g_vk_mouse_info_buffer->device_data;
      vk_camera_info_t *camera_info = (vk_camera_info_t *)g_vk_camera_info_buffer->device_data;

      time_info->time = g_window.time;
      time_info->delta_time = g_window.delta_time;

      screen_info->resolution = (ivec2_t){g_window.width, g_window.height};

      mouse_info->position = (ivec2_t){g_window.mouse_position_x, g_window.mouse_position_y};

      fvec3_t camera_position = {transform->position_x, transform->position_y, transform->position_z};
      fquat_t camera_rotation = {transform->rotation_x, transform->rotation_y, transform->rotation_z, transform->rotation_w};

      fvec3_t camera_direction = fquat_front(camera_rotation);

      float aspect_ratio = (float)g_window.width / (float)g_window.height;

      camera_info->position = (fvec4_t){camera_position.x, camera_position.y, camera_position.z, 0.0F};
      camera_info->direction = (fvec4_t){camera_direction.x, camera_direction.y, camera_direction.z, 0.0F};
      camera_info->view = fmat4x4_look_at(camera_position, fvec3_add(camera_position, camera_direction), fvec3_up());
      camera_info->view_inv = fmat4x4_inverse(camera_info->view);
      camera_info->projection = fmat4x4_persp(deg_to_rad(-camera->fov), aspect_ratio, camera->near_z, camera->far_z);
      camera_info->projection_inv = fmat4x4_inverse(camera_info->projection);
      camera_info->view_projection = fmat4x4_mul(camera_info->view, camera_info->projection);
      camera_info->view_projection_inv = fmat4x4_inverse(camera_info->view_projection);
    }
  }
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

  vk_renderpass_t *main_renderpass = (vk_renderpass_t *)renderer->main_renderpass_hdl->instance;
  vk_framebuffer_t *main_framebuffer = (vk_framebuffer_t *)renderer->main_framebuffer_hdl->instance;

  VkRenderPassBeginInfo render_pass_create_info = {
    .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
    .renderPass = main_renderpass->renderpass,
    .framebuffer = main_framebuffer->framebuffer,
    .renderArea = {
      .offset.x = 0,
      .offset.y = 0,
      .extent = {
        .width = g_vk_viewport->width,
        .height = g_vk_viewport->height,
      },
    },
    .pClearValues = clear_values,
    .clearValueCount = TI_ARRAY_COUNT(clear_values),
  };

  vkCmdBeginRenderPass(g_vk_instance->command_buffer, &render_pass_create_info, VK_SUBPASS_CONTENTS_INLINE);

  VkViewport viewport = {
    .x = 0.0F,
    .y = 0.0F,
    .width = (float)g_vk_viewport->width,
    .height = (float)g_vk_viewport->height,
    .minDepth = 0.0F,
    .maxDepth = 1.0F,
  };

  vkCmdSetViewport(g_vk_instance->command_buffer, 0, 1, &viewport);

  VkRect2D scissor = {
    .offset.x = 0,
    .offset.y = 0,
    .extent = {
      .width = g_vk_viewport->width,
      .height = g_vk_viewport->height,
    },
  };

  vkCmdSetScissor(g_vk_instance->command_buffer, 0, 1, &scissor);

  if (renderer->is_debug_enabled) {

    VkDeviceSize vertex_offset = 0;

    // TODO: abstract this even more..

    vk_buffer_t *vertex_buffer = (vk_buffer_t *)renderer->debug_line_vertex_buffer_hdl->instance;
    vk_buffer_t *index_buffer = (vk_buffer_t *)renderer->debug_line_index_buffer_hdl->instance;
    vk_pipeline_t *debug_line_pipeline = (vk_pipeline_t *)renderer->debug_line_pipeline_hdl->instance;

    vkCmdBindPipeline(g_vk_instance->command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, debug_line_pipeline->pipeline);
    vkCmdBindVertexBuffers(g_vk_instance->command_buffer, 0, 1, &vertex_buffer->buffer, &vertex_offset);
    vkCmdBindIndexBuffer(g_vk_instance->command_buffer, index_buffer->buffer, 0, VK_INDEX_TYPE_UINT32);
    vkCmdBindDescriptorSets(g_vk_instance->command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, debug_line_pipeline->pipeline_layout, 0, 1, &debug_line_pipeline->descriptor_set[0], 0, 0);
    vkCmdDrawIndexed(g_vk_instance->command_buffer, renderer->debug_line_index_offset, 1, 0, 0, 0);

    renderer->debug_line_vertex_offset = 0;
    renderer->debug_line_index_offset = 0;
  }

  vkCmdEndRenderPass(g_vk_instance->command_buffer);
}
static void record_post_main_pass(vk_renderer_t *renderer) {
  // TODO
}

static void record_pre_ray_tracing_pass(vk_renderer_t *renderer) {
  vk_framebuffer_t *main_framebuffer = (vk_framebuffer_t *)renderer->main_framebuffer_hdl->instance;
  vk_image_t *color_image = (vk_image_t *)main_framebuffer->color_attachment_hdl[0]->instance; // TODO
  vk_image_t *depth_image = (vk_image_t *)main_framebuffer->depth_attachment_hdl->instance;

  {
    VkImageMemoryBarrier image_memory_barrier = {
      .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
      .srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
      .dstAccessMask = VK_ACCESS_SHADER_WRITE_BIT,
      .oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
      .newLayout = VK_IMAGE_LAYOUT_GENERAL,
      .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
      .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
      .image = color_image->image,
      .subresourceRange = {
        .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
        .baseMipLevel = 0,
        .levelCount = 1,
        .baseArrayLayer = 0,
        .layerCount = 1,
      },
    };

    vkCmdPipelineBarrier(g_vk_instance->command_buffer, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_PIPELINE_STAGE_RAY_TRACING_SHADER_BIT_KHR, 0, 0, 0, 0, 0, 1, &image_memory_barrier);
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
      .image = depth_image->image,
      .subresourceRange = {
        .aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT,
        .baseMipLevel = 0,
        .levelCount = 1,
        .baseArrayLayer = 0,
        .layerCount = 1,
      },
    };

    vkCmdPipelineBarrier(g_vk_instance->command_buffer, VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT, VK_PIPELINE_STAGE_RAY_TRACING_SHADER_BIT_KHR, 0, 0, 0, 0, 0, 1, &image_memory_barrier);
  }
}
static void record_ray_tracing_pass(vk_renderer_t *renderer) {
  // TODO
}
static void record_post_ray_tracing_pass(vk_renderer_t *renderer) {
  vk_framebuffer_t *main_framebuffer = (vk_framebuffer_t *)renderer->main_framebuffer_hdl->instance;
  vk_image_t *color_image = (vk_image_t *)main_framebuffer->color_attachment_hdl[0]->instance; // TODO
  vk_image_t *depth_image = (vk_image_t *)main_framebuffer->depth_attachment_hdl->instance;

  {
    VkImageMemoryBarrier image_memory_barrier = {
      .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
      .srcAccessMask = VK_ACCESS_SHADER_WRITE_BIT,
      .dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
      .oldLayout = VK_IMAGE_LAYOUT_GENERAL,
      .newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
      .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
      .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
      .image = color_image->image,
      .subresourceRange = {
        .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
        .baseMipLevel = 0,
        .levelCount = 1,
        .baseArrayLayer = 0,
        .layerCount = 1,
      },
    };

    vkCmdPipelineBarrier(g_vk_instance->command_buffer, VK_PIPELINE_STAGE_RAY_TRACING_SHADER_BIT_KHR, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, 0, 0, 0, 0, 0, 1, &image_memory_barrier);
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
      .image = depth_image->image,
      .subresourceRange = {
        .aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT,
        .baseMipLevel = 0,
        .levelCount = 1,
        .baseArrayLayer = 0,
        .layerCount = 1,
      },
    };

    vkCmdPipelineBarrier(g_vk_instance->command_buffer, VK_PIPELINE_STAGE_RAY_TRACING_SHADER_BIT_KHR, VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT, 0, 0, 0, 0, 0, 1, &image_memory_barrier);
  }
}

static void record_pre_imgui_pass(vk_renderer_t *renderer) {
  vk_framebuffer_t *main_framebuffer = (vk_framebuffer_t *)renderer->main_framebuffer_hdl->instance;
  vk_image_t *color_image = (vk_image_t *)main_framebuffer->color_attachment_hdl[0]->instance; // TODO
  vk_image_t *depth_image = (vk_image_t *)main_framebuffer->depth_attachment_hdl->instance;

  {
    VkImageMemoryBarrier image_memory_barrier = {
      .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
      .srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
      .dstAccessMask = VK_ACCESS_SHADER_READ_BIT,
      .oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
      .newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
      .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
      .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
      .image = color_image->image,
      .subresourceRange = {
        .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
        .baseMipLevel = 0,
        .levelCount = 1,
        .baseArrayLayer = 0,
        .layerCount = 1,
      },
    };

    vkCmdPipelineBarrier(g_vk_instance->command_buffer, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, 0, 0, 0, 1, &image_memory_barrier);
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
      .image = depth_image->image,
      .subresourceRange = {
        .aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT,
        .baseMipLevel = 0,
        .levelCount = 1,
        .baseArrayLayer = 0,
        .layerCount = 1,
      },
    };

    vkCmdPipelineBarrier(g_vk_instance->command_buffer, VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, 0, 0, 0, 1, &image_memory_barrier);
  }
}
static void record_imgui_pass(vk_renderer_t *renderer) {
  VkClearValue color_clear_value = {
    .color.float32 = {
      0.2352941176470588F, // 60 / 255
      0.2352941176470588F, // 60 / 255
      0.2352941176470588F, // 60 / 255
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

  vk_renderpass_t *imgui_renderpass = (vk_renderpass_t *)renderer->imgui_renderpass_hdl->instance;
  vk_framebuffer_t *imgui_framebuffer = (vk_framebuffer_t *)renderer->imgui_framebuffer_hdl->instance;

  VkRenderPassBeginInfo render_pass_create_info = {
    .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
    .renderPass = imgui_renderpass->renderpass,
    .framebuffer = imgui_framebuffer->framebuffer,
    .renderArea = {
      .offset.x = 0,
      .offset.y = 0,
      .extent = {
        .width = g_window.width,
        .height = g_window.height,
      },
    },
    .pClearValues = clear_values,
    .clearValueCount = TI_ARRAY_COUNT(clear_values),
  };

  vkCmdBeginRenderPass(g_vk_instance->command_buffer, &render_pass_create_info, VK_SUBPASS_CONTENTS_INLINE);

  VkViewport viewport = {
    .x = 0.0F,
    .y = 0.0F,
    .width = (float)g_window.width,
    .height = (float)g_window.height,
    .minDepth = 0.0F,
    .maxDepth = 1.0F,
  };

  vkCmdSetViewport(g_vk_instance->command_buffer, 0, 1, &viewport);

  VkRect2D scissor = {
    .offset.x = 0,
    .offset.y = 0,
    .extent = {
      .width = g_window.width,
      .height = g_window.height,
    },
  };

  vkCmdSetScissor(g_vk_instance->command_buffer, 0, 1, &scissor);

  if (g_window.editor_draw_proc) {
    g_window.editor_draw_proc();
  }

  vkCmdEndRenderPass(g_vk_instance->command_buffer);
}
static void record_post_imgui_pass(vk_renderer_t *renderer) {
  vk_framebuffer_t *main_framebuffer = (vk_framebuffer_t *)renderer->main_framebuffer_hdl->instance;
  vk_image_t *color_image = (vk_image_t *)main_framebuffer->color_attachment_hdl[0]->instance; // TODO
  vk_image_t *depth_image = (vk_image_t *)main_framebuffer->depth_attachment_hdl->instance;

  {
    VkImageMemoryBarrier image_memory_barrier = {
      .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
      .srcAccessMask = VK_ACCESS_SHADER_READ_BIT,
      .dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
      .oldLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
      .newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
      .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
      .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
      .image = color_image->image,
      .subresourceRange = {
        .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
        .baseMipLevel = 0,
        .levelCount = 1,
        .baseArrayLayer = 0,
        .layerCount = 1,
      },
    };

    vkCmdPipelineBarrier(g_vk_instance->command_buffer, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, 0, 0, 0, 0, 0, 1, &image_memory_barrier);
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
      .image = depth_image->image,
      .subresourceRange = {
        .aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT,
        .baseMipLevel = 0,
        .levelCount = 1,
        .baseArrayLayer = 0,
        .layerCount = 1,
      },
    };

    vkCmdPipelineBarrier(g_vk_instance->command_buffer, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT, 0, 0, 0, 0, 0, 1, &image_memory_barrier);
  }
}

static void destroy_sync_object(vk_renderer_t *renderer) {
  uint32_t image_index = 0;
  uint32_t image_count = g_vk_swapchain->image_count;

  while (image_index < image_count) {

    vkDestroySemaphore(g_vk_instance->device, renderer->render_finished_semaphore[image_index], 0);

    image_index++;
  }

  vkDestroySemaphore(g_vk_instance->device, renderer->image_available_semaphore, 0);
  vkDestroyFence(g_vk_instance->device, renderer->frame_fence, 0);
}
