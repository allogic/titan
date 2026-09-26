#include <ti_pch.h>

static void create_descriptor_pool(vk_pipeline_t *pipeline);
static void create_descriptor_set_layout(vk_pipeline_t *pipeline);
static void create_descriptor_set(vk_pipeline_t *pipeline);
static void create_pipeline_layout(vk_pipeline_t *pipeline);

static void create_sbt_buffer(vk_pipeline_t *pipeline);

static void create_default_pipeline(vk_pipeline_t *pipeline, vk_renderpass_t *renderpass);
static void create_mesh_pipeline(vk_pipeline_t *pipeline, vk_renderpass_t *renderpass);
static void create_ray_tracing_pipeline(vk_pipeline_t *pipeline);
static void create_compute_pipeline(vk_pipeline_t *pipeline);

static void destroy_sbt_buffer(vk_pipeline_t *pipeline);

void vk_pipeline_create(vk_pipeline_t *pipeline, vk_renderpass_t *renderpass, char const *asset_path) {
  pipeline->asset.path = asset_path;

  fs_asset_load(&pipeline->asset);

  fs_pipeline_t *config = (fs_pipeline_t *)pipeline->asset.instance;

  pipeline->descriptor_set_layout = (VkDescriptorSetLayout *)TI_ALLOC(sizeof(VkDescriptorSetLayout) * config->descriptor_set_count, 0, 0);
  pipeline->descriptor_set = (VkDescriptorSet *)TI_ALLOC(sizeof(VkDescriptorSet) * config->descriptor_set_count, 0, 0);

  create_descriptor_pool(pipeline);
  create_descriptor_set_layout(pipeline);
  create_descriptor_set(pipeline);
  create_pipeline_layout(pipeline);

  switch (config->pipeline_type) {

    case FS_PIPELINE_TYPE_DEFAULT: {

      create_default_pipeline(pipeline, renderpass);

      break;
    }
    case FS_PIPELINE_TYPE_MESH: {

      create_mesh_pipeline(pipeline, renderpass);

      break;
    }
    case FS_PIPELINE_TYPE_RAY_TRACING: {

      create_ray_tracing_pipeline(pipeline);

      create_sbt_buffer(pipeline);

      break;
    }
    case FS_PIPELINE_TYPE_COMPUTE: {

      create_compute_pipeline(pipeline);

      break;
    }
  }
}
void vk_pipeline_destroy(vk_pipeline_t *pipeline) {
  fs_pipeline_t *config = (fs_pipeline_t *)pipeline->asset.instance;

  switch (config->pipeline_type) {

    case FS_PIPELINE_TYPE_RAY_TRACING: {

      destroy_sbt_buffer(pipeline);

      break;
    }
  }

  vkDestroyDescriptorPool(g_vk_instance.device, pipeline->descriptor_pool, 0);
  vkDestroyDescriptorSetLayout(g_vk_instance.device, pipeline->descriptor_set_layout_base, 0);
  vkDestroyPipelineLayout(g_vk_instance.device, pipeline->pipeline_layout, 0);
  vkDestroyPipeline(g_vk_instance.device, pipeline->pipeline_handle, 0);

  TI_FREE(pipeline->descriptor_set_layout);
  TI_FREE(pipeline->descriptor_set);

  fs_asset_destroy(&pipeline->asset);
}

static void create_descriptor_pool(vk_pipeline_t *pipeline) {
  fs_pipeline_t *config = (fs_pipeline_t *)pipeline->asset.instance;

  uint32_t descriptor_pool_index = 0;
  uint32_t descriptor_pool_count = pipeline->descriptor_pool_size_count;

  while (descriptor_pool_index < descriptor_pool_count) {

    pipeline->descriptor_pool_size[descriptor_pool_index].descriptorCount *= config->descriptor_set_count;

    descriptor_pool_index++;
  }

  VkDescriptorPoolCreateInfo descriptor_pool_create_info = {
    .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
    .pPoolSizes = pipeline->descriptor_pool_size,
    .poolSizeCount = pipeline->descriptor_pool_size_count,
    .maxSets = config->descriptor_set_count,
  };

  TI_VK_CHECK(vkCreateDescriptorPool(g_vk_instance.device, &descriptor_pool_create_info, 0, &pipeline->descriptor_pool));
}
static void create_descriptor_set_layout(vk_pipeline_t *pipeline) {
  VkDescriptorSetLayoutCreateInfo descriptor_set_layout_create_info = {
    .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
    .pBindings = pipeline->descriptor_set_layout_binding,
    .bindingCount = pipeline->descriptor_set_layout_binding_count,
    .pNext = 0,
  };

  TI_VK_CHECK(vkCreateDescriptorSetLayout(g_vk_instance.device, &descriptor_set_layout_create_info, 0, &pipeline->descriptor_set_layout_base));
}
static void create_descriptor_set(vk_pipeline_t *pipeline) {
  fs_pipeline_t *config = (fs_pipeline_t *)pipeline->asset.instance;

  uint32_t descriptor_set_index = 0;
  uint32_t descriptor_set_count = config->descriptor_set_count;

  while (descriptor_set_index < descriptor_set_count) {

    pipeline->descriptor_set_layout[descriptor_set_index] = pipeline->descriptor_set_layout_base;

    descriptor_set_index++;
  }

  VkDescriptorSetAllocateInfo descriptor_set_allocate_info = {
    .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
    .descriptorSetCount = config->descriptor_set_count,
    .descriptorPool = pipeline->descriptor_pool,
    .pSetLayouts = pipeline->descriptor_set_layout,
  };

  TI_VK_CHECK(vkAllocateDescriptorSets(g_vk_instance.device, &descriptor_set_allocate_info, pipeline->descriptor_set));
}
static void create_pipeline_layout(vk_pipeline_t *pipeline) {
  VkPipelineLayoutCreateInfo pipeline_layout_create_info = {
    .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
    .setLayoutCount = 1,
    .pSetLayouts = &pipeline->descriptor_set_layout_base,
    .pPushConstantRanges = pipeline->push_constant_range,
    .pushConstantRangeCount = pipeline->push_constant_range_count,
  };

  TI_VK_CHECK(vkCreatePipelineLayout(g_vk_instance.device, &pipeline_layout_create_info, 0, &pipeline->pipeline_layout));
}

static void create_sbt_buffer(vk_pipeline_t *pipeline) {
  pipeline->ray_gen_group_count = 1;
  pipeline->ray_miss_group_count = 1;
  pipeline->ray_hit_group_count = 1;
  pipeline->callable_group_count = 0;

  uint32_t handle_size = g_physical_device_ray_tracing_pipeline_properties.shaderGroupHandleSize;
  uint32_t handle_alignment = g_physical_device_ray_tracing_pipeline_properties.shaderGroupHandleAlignment;
  uint32_t base_alignment = g_physical_device_ray_tracing_pipeline_properties.shaderGroupBaseAlignment;

  uint32_t aligned_handle_size = TI_ALIGN_UP_BY(handle_size, handle_alignment);

  uint64_t ray_gen_size = aligned_handle_size * pipeline->ray_gen_group_count;
  uint64_t ray_miss_size = aligned_handle_size * pipeline->ray_miss_group_count;
  uint64_t ray_hit_size = aligned_handle_size * pipeline->ray_hit_group_count;
  uint64_t callable_size = 0;

  uint64_t ray_gen_region_size = TI_ALIGN_UP_BY(ray_gen_size, base_alignment);
  uint64_t ray_miss_region_size = TI_ALIGN_UP_BY(ray_miss_size, base_alignment);
  uint64_t ray_hit_region_size = TI_ALIGN_UP_BY(ray_hit_size, base_alignment);
  uint64_t callable_region_size = TI_ALIGN_UP_BY(callable_size, base_alignment);

  uint64_t sbt_buffer_size = ray_gen_region_size + ray_miss_region_size + ray_hit_region_size + callable_region_size;

  VkBufferCreateInfo buffer_create_info = {
    .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
    .size = sbt_buffer_size,
    .usage = VK_BUFFER_USAGE_SHADER_BINDING_TABLE_BIT_KHR | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT,
    .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
  };

  TI_VK_CHECK(vkCreateBuffer(g_vk_instance.device, &buffer_create_info, 0, &pipeline->sbt_buffer_handle));

  VkMemoryRequirements memory_requirements = {0};

  vkGetBufferMemoryRequirements(g_vk_instance.device, pipeline->sbt_buffer_handle, &memory_requirements);

  uint32_t memory_type_index = vk_find_memory_type_index(memory_requirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

  VkMemoryAllocateFlagsInfo memory_allocate_flags_info = {
    .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_FLAGS_INFO,
    .flags = VK_MEMORY_ALLOCATE_DEVICE_ADDRESS_BIT,
  };

  VkMemoryAllocateInfo memory_allocate_info = {
    .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
    .pNext = &memory_allocate_flags_info,
    .allocationSize = memory_requirements.size,
    .memoryTypeIndex = memory_type_index,
  };

  TI_VK_CHECK(vkAllocateMemory(g_vk_instance.device, &memory_allocate_info, 0, &pipeline->sbt_device_memory));
  TI_VK_CHECK(vkBindBufferMemory(g_vk_instance.device, pipeline->sbt_buffer_handle, pipeline->sbt_device_memory, 0));

  uint8_t *sbt_device_data = 0;

  TI_VK_CHECK(vkMapMemory(g_vk_instance.device, pipeline->sbt_device_memory, 0, sbt_buffer_size, 0, &sbt_device_data));

  uint8_t *handles = (uint8_t *)TI_ALLOC(handle_size * 3, 0, 0);

  TI_VK_CHECK(vkGetRayTracingShaderGroupHandlesKHR_proc(g_vk_instance.device, pipeline->pipeline_handle, 0, 3, handle_size * 3, handles));

  memcpy(sbt_device_data, handles + handle_size * 0, handle_size);
  sbt_device_data += ray_gen_region_size;
  memcpy(sbt_device_data, handles + handle_size * 1, handle_size);
  sbt_device_data += ray_miss_region_size;
  memcpy(sbt_device_data, handles + handle_size * 2, handle_size);

  TI_FREE(handles);

  vkUnmapMemory(g_vk_instance.device, pipeline->sbt_device_memory);

  VkBufferDeviceAddressInfo buffer_device_address_info = {
    .sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO,
    .buffer = pipeline->sbt_buffer_handle,
  };

  pipeline->sbt_device_address = vkGetBufferDeviceAddress(g_vk_instance.device, &buffer_device_address_info);

  pipeline->ray_gen_region.deviceAddress = pipeline->sbt_device_address;
  pipeline->ray_gen_region.stride = aligned_handle_size;
  pipeline->ray_gen_region.size = aligned_handle_size;

  pipeline->ray_miss_region.deviceAddress = TI_ALIGN_UP_BY(pipeline->sbt_device_address + aligned_handle_size, base_alignment);
  pipeline->ray_miss_region.stride = aligned_handle_size;
  pipeline->ray_miss_region.size = ray_miss_region_size;

  pipeline->ray_hit_region.deviceAddress = TI_ALIGN_UP_BY(pipeline->sbt_device_address + aligned_handle_size, base_alignment) + ray_miss_region_size;
  pipeline->ray_hit_region.stride = aligned_handle_size;
  pipeline->ray_hit_region.size = ray_hit_region_size;

  pipeline->callable_region.deviceAddress = 0;
  pipeline->callable_region.stride = 0;
  pipeline->callable_region.size = 0;
}

static void create_default_pipeline(vk_pipeline_t *pipeline, vk_renderpass_t *renderpass) {
  fs_pipeline_t *config = (fs_pipeline_t *)pipeline->asset.instance;

  VkShaderModule vertex_module = 0;
  VkShaderModule fragment_module = 0;

  {
    VkShaderModuleCreateInfo shader_module_create_info = {
      .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
      .pCode = config->spirv_vertex_words,
      .codeSize = config->spirv_vertex_word_count * sizeof(uint32_t),
    };

    TI_VK_CHECK(vkCreateShaderModule(g_vk_instance.device, &shader_module_create_info, 0, &vertex_module));
  }

  {
    VkShaderModuleCreateInfo shader_module_create_info = {
      .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
      .pCode = config->spirv_fragment_words,
      .codeSize = config->spirv_fragment_word_count * sizeof(uint32_t),
    };

    TI_VK_CHECK(vkCreateShaderModule(g_vk_instance.device, &shader_module_create_info, 0, &fragment_module));
  }

  VkPipelineShaderStageCreateInfo pipeline_shader_stage_create_info[] = {
    {
      .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
      .stage = VK_SHADER_STAGE_VERTEX_BIT,
      .module = vertex_module,
      .pName = "main",
    },
    {
      .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
      .stage = VK_SHADER_STAGE_FRAGMENT_BIT,
      .module = fragment_module,
      .pName = "main",
    },
  };

  VkPipelineVertexInputStateCreateInfo pipeline_vertex_input_state_create_info = {
    .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
    .pVertexBindingDescriptions = pipeline->vertex_input_binding_description,
    .vertexBindingDescriptionCount = pipeline->vertex_input_binding_description_count,
    .pVertexAttributeDescriptions = pipeline->vertex_input_attribute_description,
    .vertexAttributeDescriptionCount = pipeline->vertex_input_attribute_description_count,
  };

  VkPipelineInputAssemblyStateCreateInfo pipeline_input_assembly_state_create_info = {
    .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
    .topology = g_vk_primitive_topology_table[config->primitive_topology_index].value,
    .primitiveRestartEnable = 0,
  };

  VkViewport viewport = {
    .x = 0.0F,
    .y = 0.0F,
    .width = (float)g_pl_window.window_width,   // TODO
    .height = (float)g_pl_window.window_height, // TODO
    .minDepth = 0.0F,
    .maxDepth = 1.0F,
  };

  VkRect2D scissor = {
    .offset.x = 0,
    .offset.y = 0,
    .extent = {
      .width = g_pl_window.window_width,   // TODO
      .height = g_pl_window.window_height, // TODO
    },
  };

  VkPipelineViewportStateCreateInfo pipeline_viewport_state_create_info = {
    .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
    .viewportCount = 1,
    .pViewports = &viewport,
    .scissorCount = 1,
    .pScissors = &scissor,
  };

  VkPipelineRasterizationStateCreateInfo pipeline_rasterization_state_create_info = {
    .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
    .depthClampEnable = 0,
    .rasterizerDiscardEnable = 0,
    .polygonMode = g_vk_polygon_mode_table[config->polygon_mode_index].value,
    .lineWidth = 1.0F,
    .cullMode = config->cull_mode_flags,
    .frontFace = VK_FRONT_FACE_CLOCKWISE,
    .depthBiasEnable = 0,
    .depthBiasConstantFactor = 0.0F,
    .depthBiasClamp = 0.0F,
    .depthBiasSlopeFactor = 0.0F,
  };

  VkPipelineMultisampleStateCreateInfo pipeline_multisample_state_create_info = {
    .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
    .sampleShadingEnable = 0,
    .rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
    .minSampleShading = 1.0F,
    .pSampleMask = 0,
    .alphaToCoverageEnable = 0,
    .alphaToOneEnable = 0,
  };

  VkPipelineColorBlendAttachmentState pipeline_color_blend_attachment_state = {
    .colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT,
    .blendEnable = config->enable_blending,
    .srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA,
    .dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
    .colorBlendOp = VK_BLEND_OP_ADD,
    .srcAlphaBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA,
    .dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
    .alphaBlendOp = VK_BLEND_OP_ADD,
  };

  VkPipelineDepthStencilStateCreateInfo pipeline_depth_stencil_state_create_info = {
    .sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
    .depthTestEnable = config->enable_depth_test,
    .depthWriteEnable = config->enable_depth_write,
    .depthCompareOp = VK_COMPARE_OP_LESS,
    .depthBoundsTestEnable = 0,
    .stencilTestEnable = 0,
  };

  VkPipelineColorBlendStateCreateInfo pipeline_color_blend_state_create_info = {
    .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
    .logicOpEnable = 0,
    .logicOp = VK_LOGIC_OP_COPY,
    .attachmentCount = 1,
    .pAttachments = &pipeline_color_blend_attachment_state,
    .blendConstants = {
      0.0F,
      0.0F,
      0.0F,
      0.0F,
    },
  };

  VkDynamicState dynamic_state[] = {
    VK_DYNAMIC_STATE_VIEWPORT,
    VK_DYNAMIC_STATE_SCISSOR,
  };

  VkPipelineDynamicStateCreateInfo pipeline_dynamic_state_create_info = {
    .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
    .pDynamicStates = dynamic_state,
    .dynamicStateCount = TI_ARRAY_COUNT(dynamic_state),
  };

  VkGraphicsPipelineCreateInfo graphics_pipeline_create_info = {
    .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
    .pStages = pipeline_shader_stage_create_info,
    .stageCount = TI_ARRAY_COUNT(pipeline_shader_stage_create_info),
    .pVertexInputState = &pipeline_vertex_input_state_create_info,
    .pInputAssemblyState = &pipeline_input_assembly_state_create_info,
    .pViewportState = &pipeline_viewport_state_create_info,
    .pRasterizationState = &pipeline_rasterization_state_create_info,
    .pMultisampleState = &pipeline_multisample_state_create_info,
    .pDepthStencilState = &pipeline_depth_stencil_state_create_info,
    .pColorBlendState = &pipeline_color_blend_state_create_info,
    .pDynamicState = &pipeline_dynamic_state_create_info,
    .layout = pipeline->pipeline_layout,
    .renderPass = renderpass->handle,
    .subpass = 0,
    .basePipelineHandle = 0,
  };

  TI_VK_CHECK(vkCreateGraphicsPipelines(g_vk_instance.device, 0, 1, &graphics_pipeline_create_info, 0, &pipeline->pipeline_handle));

  vkDestroyShaderModule(g_vk_instance.device, vertex_module, 0);
  vkDestroyShaderModule(g_vk_instance.device, fragment_module, 0);
}
static void create_mesh_pipeline(vk_pipeline_t *pipeline, vk_renderpass_t *renderpass) {
  fs_pipeline_t *config = (fs_pipeline_t *)pipeline->asset.instance;

  VkShaderModule task_module = 0;
  VkShaderModule mesh_module = 0;
  VkShaderModule fragment_module = 0;

  // TODO
  /*
  {
    uint8_t *shader_bytes = 0;
    uint64_t shader_size = 0;

    fsutil_load_binary(&shader_bytes, &shader_size, pipeline->task_shader);

    VkShaderModuleCreateInfo shader_module_create_info = {
      .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
      .pCode = (uint32_t const *)shader_bytes,
      .codeSize = shader_size,
    };

    TI_VK_CHECK(vkCreateShaderModule(g_pl_window.device, &shader_module_create_info, 0, &task_module));

    TI_FREE(shader_bytes);
  }

  {
    uint8_t *shader_bytes = 0;
    uint64_t shader_size = 0;

    fsutil_load_binary(&shader_bytes, &shader_size, pipeline->mesh_shader);

    VkShaderModuleCreateInfo shader_module_create_info = {
      .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
      .pCode = (uint32_t const *)shader_bytes,
      .codeSize = shader_size,
    };

    TI_VK_CHECK(vkCreateShaderModule(g_pl_window.device, &shader_module_create_info, 0, &mesh_module));

    TI_FREE(shader_bytes);
  }

  {
    uint8_t *shader_bytes = 0;
    uint64_t shader_size = 0;

    fsutil_load_binary(&shader_bytes, &shader_size, pipeline->fragment_shader);

    VkShaderModuleCreateInfo shader_module_create_info = {
      .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
      .pCode = (uint32_t const *)shader_bytes,
      .codeSize = shader_size,
    };

    TI_VK_CHECK(vkCreateShaderModule(g_pl_window.device, &shader_module_create_info, 0, &fragment_module));

    TI_FREE(shader_bytes);
  }
  */

  VkPipelineShaderStageCreateInfo pipeline_shader_stage_create_info[] = {
    {
      .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
      .stage = VK_SHADER_STAGE_TASK_BIT_EXT,
      .module = task_module,
      .pName = "main",
    },
    {
      .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
      .stage = VK_SHADER_STAGE_MESH_BIT_EXT,
      .module = mesh_module,
      .pName = "main",
    },
    {
      .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
      .stage = VK_SHADER_STAGE_FRAGMENT_BIT,
      .module = fragment_module,
      .pName = "main",
    },
  };

  VkViewport viewport = {
    .x = 0.0F,
    .y = 0.0F,
    .width = (float)g_pl_window.window_width,   // TODO
    .height = (float)g_pl_window.window_height, // TODO
    .minDepth = 0.0F,
    .maxDepth = 1.0F,
  };

  VkRect2D scissor = {
    .offset.x = 0,
    .offset.y = 0,
    .extent = {
      .width = g_pl_window.window_width,   // TODO
      .height = g_pl_window.window_height, // TODO
    },
  };

  VkPipelineViewportStateCreateInfo pipeline_viewport_state_create_info = {
    .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
    .viewportCount = 1,
    .pViewports = &viewport,
    .scissorCount = 1,
    .pScissors = &scissor,
  };

  VkPipelineRasterizationStateCreateInfo pipeline_rasterization_state_create_info = {
    .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
    .depthClampEnable = 0,
    .rasterizerDiscardEnable = 0,
    .polygonMode = g_vk_polygon_mode_table[config->polygon_mode_index].value,
    .lineWidth = 1.0F,
    .cullMode = config->cull_mode_flags,
    .frontFace = VK_FRONT_FACE_CLOCKWISE,
    .depthBiasEnable = 0,
    .depthBiasConstantFactor = 0.0F,
    .depthBiasClamp = 0.0F,
    .depthBiasSlopeFactor = 0.0F,
  };

  VkPipelineMultisampleStateCreateInfo pipeline_multisample_state_create_info = {
    .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
    .sampleShadingEnable = 0,
    .rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
    .minSampleShading = 1.0F,
    .pSampleMask = 0,
    .alphaToCoverageEnable = 0,
    .alphaToOneEnable = 0,
  };

  VkPipelineColorBlendAttachmentState pipeline_color_blend_attachment_state = {
    .colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT,
    .blendEnable = config->enable_blending,
    .srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA,
    .dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
    .colorBlendOp = VK_BLEND_OP_ADD,
    .srcAlphaBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA,
    .dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
    .alphaBlendOp = VK_BLEND_OP_ADD,
  };

  VkPipelineDepthStencilStateCreateInfo pipeline_depth_stencil_state_create_info = {
    .sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
    .depthTestEnable = config->enable_depth_test,
    .depthWriteEnable = config->enable_depth_write,
    .depthCompareOp = VK_COMPARE_OP_LESS,
    .depthBoundsTestEnable = 0,
    .stencilTestEnable = 0,
  };

  VkPipelineColorBlendStateCreateInfo pipeline_color_blend_state_create_info = {
    .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
    .logicOpEnable = 0,
    .logicOp = VK_LOGIC_OP_COPY,
    .attachmentCount = 1,
    .pAttachments = &pipeline_color_blend_attachment_state,
    .blendConstants = {
      0.0F,
      0.0F,
      0.0F,
      0.0F,
    },
  };

  VkDynamicState dynamic_state[] = {
    VK_DYNAMIC_STATE_VIEWPORT,
    VK_DYNAMIC_STATE_SCISSOR,
  };

  VkPipelineDynamicStateCreateInfo pipeline_dynamic_state_create_info = {
    .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
    .pDynamicStates = dynamic_state,
    .dynamicStateCount = TI_ARRAY_COUNT(dynamic_state),
  };

  VkGraphicsPipelineCreateInfo graphics_pipeline_create_info = {
    .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
    .pStages = pipeline_shader_stage_create_info,
    .stageCount = TI_ARRAY_COUNT(pipeline_shader_stage_create_info),
    .pVertexInputState = 0,
    .pInputAssemblyState = 0,
    .pViewportState = &pipeline_viewport_state_create_info,
    .pRasterizationState = &pipeline_rasterization_state_create_info,
    .pMultisampleState = &pipeline_multisample_state_create_info,
    .pDepthStencilState = &pipeline_depth_stencil_state_create_info,
    .pColorBlendState = &pipeline_color_blend_state_create_info,
    .pDynamicState = &pipeline_dynamic_state_create_info,
    .layout = pipeline->pipeline_layout,
    .renderPass = renderpass->handle,
    .subpass = 0,
    .basePipelineHandle = 0,
  };

  TI_VK_CHECK(vkCreateGraphicsPipelines(g_vk_instance.device, 0, 1, &graphics_pipeline_create_info, 0, &pipeline->pipeline_handle));

  vkDestroyShaderModule(g_vk_instance.device, task_module, 0);
  vkDestroyShaderModule(g_vk_instance.device, mesh_module, 0);
  vkDestroyShaderModule(g_vk_instance.device, fragment_module, 0);
}
static void create_ray_tracing_pipeline(vk_pipeline_t *pipeline) {
  VkShaderModule ray_gen_module = 0;
  VkShaderModule ray_miss_module = 0;
  VkShaderModule ray_intersect_module = 0;
  VkShaderModule ray_closest_hit_module = 0;

  // TODO
  /*
  {
    uint8_t *shader_bytes = 0;
    uint64_t shader_size = 0;

    fsutil_load_binary(&shader_bytes, &shader_size, pipeline->ray_gen_shader);

    VkShaderModuleCreateInfo shader_module_create_info = {
      .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
      .pCode = (uint32_t const *)shader_bytes,
      .codeSize = shader_size,
    };

    TI_VK_CHECK(vkCreateShaderModule(g_pl_window.device, &shader_module_create_info, 0, &ray_gen_module));

    TI_FREE(shader_bytes);
  }

  {
    uint8_t *shader_bytes = 0;
    uint64_t shader_size = 0;

    fsutil_load_binary(&shader_bytes, &shader_size, pipeline->ray_miss_shader);

    VkShaderModuleCreateInfo shader_module_create_info = {
      .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
      .pCode = (uint32_t const *)shader_bytes,
      .codeSize = shader_size,
    };

    TI_VK_CHECK(vkCreateShaderModule(g_pl_window.device, &shader_module_create_info, 0, &ray_miss_module));

    TI_FREE(shader_bytes);
  }

  {
    uint8_t *shader_bytes = 0;
    uint64_t shader_size = 0;

    fsutil_load_binary(&shader_bytes, &shader_size, pipeline->ray_intersect_shader);

    VkShaderModuleCreateInfo shader_module_create_info = {
      .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
      .pCode = (uint32_t const *)shader_bytes,
      .codeSize = shader_size,
    };

    TI_VK_CHECK(vkCreateShaderModule(g_pl_window.device, &shader_module_create_info, 0, &ray_intersect_module));

    TI_FREE(shader_bytes);
  }

  {
    uint8_t *shader_bytes = 0;
    uint64_t shader_size = 0;

    fsutil_load_binary(&shader_bytes, &shader_size, pipeline->ray_closest_hit_shader);

    VkShaderModuleCreateInfo shader_module_create_info = {
      .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
      .pCode = (uint32_t const *)shader_bytes,
      .codeSize = shader_size,
    };

    TI_VK_CHECK(vkCreateShaderModule(g_pl_window.device, &shader_module_create_info, 0, &ray_closest_hit_module));

    TI_FREE(shader_bytes);
  }
  */

  VkPipelineShaderStageCreateInfo pipeline_shader_stage_create_info[] = {
    {
      .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
      .stage = VK_SHADER_STAGE_RAYGEN_BIT_KHR,
      .module = ray_gen_module,
      .pName = "main",
    },
    {
      .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
      .stage = VK_SHADER_STAGE_MISS_BIT_KHR,
      .module = ray_miss_module,
      .pName = "main",
    },
    {
      .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
      .stage = VK_SHADER_STAGE_INTERSECTION_BIT_KHR,
      .module = ray_intersect_module,
      .pName = "main",
    },
    {
      .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
      .stage = VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR,
      .module = ray_closest_hit_module,
      .pName = "main",
    },
  };

  VkRayTracingShaderGroupCreateInfoKHR ray_tracing_shader_group_create_info[] = {
    {
      .sType = VK_STRUCTURE_TYPE_RAY_TRACING_SHADER_GROUP_CREATE_INFO_KHR,
      .type = VK_RAY_TRACING_SHADER_GROUP_TYPE_GENERAL_KHR,
      .generalShader = 0,
      .closestHitShader = VK_SHADER_UNUSED_KHR,
      .anyHitShader = VK_SHADER_UNUSED_KHR,
      .intersectionShader = VK_SHADER_UNUSED_KHR,
    },
    {
      .sType = VK_STRUCTURE_TYPE_RAY_TRACING_SHADER_GROUP_CREATE_INFO_KHR,
      .type = VK_RAY_TRACING_SHADER_GROUP_TYPE_GENERAL_KHR,
      .generalShader = 1,
      .closestHitShader = VK_SHADER_UNUSED_KHR,
      .anyHitShader = VK_SHADER_UNUSED_KHR,
      .intersectionShader = VK_SHADER_UNUSED_KHR,
    },
    {
      .sType = VK_STRUCTURE_TYPE_RAY_TRACING_SHADER_GROUP_CREATE_INFO_KHR,
      .type = VK_RAY_TRACING_SHADER_GROUP_TYPE_PROCEDURAL_HIT_GROUP_KHR,
      .generalShader = VK_SHADER_UNUSED_KHR,
      .closestHitShader = 3,
      .anyHitShader = VK_SHADER_UNUSED_KHR,
      .intersectionShader = 2,
    },
  };

  VkRayTracingPipelineCreateInfoKHR ray_tracing_pipeline_create_info = {
    .sType = VK_STRUCTURE_TYPE_RAY_TRACING_PIPELINE_CREATE_INFO_KHR,
    .pStages = pipeline_shader_stage_create_info,
    .stageCount = TI_ARRAY_COUNT(pipeline_shader_stage_create_info),
    .pGroups = ray_tracing_shader_group_create_info,
    .groupCount = TI_ARRAY_COUNT(ray_tracing_shader_group_create_info),
    .maxPipelineRayRecursionDepth = 1,
    .pLibraryInfo = 0,
    .pLibraryInterface = 0,
    .pDynamicState = 0,
    .layout = pipeline->pipeline_layout,
    .basePipelineHandle = 0,
    .basePipelineIndex = 0,
  };

  TI_VK_CHECK(vkCreateRayTracingPipelinesKHR_proc(g_vk_instance.device, 0, 0, 1, &ray_tracing_pipeline_create_info, 0, &pipeline->pipeline_handle));

  vkDestroyShaderModule(g_vk_instance.device, ray_gen_module, 0);
  vkDestroyShaderModule(g_vk_instance.device, ray_miss_module, 0);
  vkDestroyShaderModule(g_vk_instance.device, ray_intersect_module, 0);
  vkDestroyShaderModule(g_vk_instance.device, ray_closest_hit_module, 0);
}
static void create_compute_pipeline(vk_pipeline_t *pipeline) {
  VkShaderModule compute_module = 0;

  // TODO
  /*
  {
    uint8_t *shader_bytes = 0;
    uint64_t shader_size = 0;

    fsutil_load_binary(&shader_bytes, &shader_size, pipeline->compute_shader);

    VkShaderModuleCreateInfo shader_module_create_info = {
      .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
      .pCode = (uint32_t const *)shader_bytes,
      .codeSize = shader_size,
    };

    TI_VK_CHECK(vkCreateShaderModule(g_pl_window.device, &shader_module_create_info, 0, &compute_module));

    TI_FREE(shader_bytes);
  }
  */

  VkPipelineShaderStageCreateInfo pipeline_shader_stage_create_info = {
    .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
    .stage = VK_SHADER_STAGE_COMPUTE_BIT,
    .module = compute_module,
    .pName = "main",
  };

  VkComputePipelineCreateInfo compute_pipeline_create_info = {
    .sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO,
    .layout = pipeline->pipeline_layout,
    .stage = pipeline_shader_stage_create_info,
  };

  TI_VK_CHECK(vkCreateComputePipelines(g_vk_instance.device, 0, 1, &compute_pipeline_create_info, 0, &pipeline->pipeline_handle));

  vkDestroyShaderModule(g_vk_instance.device, compute_module, 0);
}

static void destroy_sbt_buffer(vk_pipeline_t *pipeline) {
  vkFreeMemory(g_vk_instance.device, pipeline->sbt_device_memory, 0);
  vkDestroyBuffer(g_vk_instance.device, pipeline->sbt_buffer_handle, 0);
}
