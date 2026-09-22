#include <ti_pch.h>

#include <imgui.h>

static void draw_background(void);
static void draw_asset_controls(void);
static int draw_asset(void);
static int draw_asset_buffer_usage_flags(VkBufferUsageFlags *flags);
static int draw_asset_image_usage_flags(VkImageUsageFlags *flags);
static int draw_asset_image_aspect_flags(VkImageAspectFlags *flags);
static int draw_asset_memory_property_flags(VkMemoryPropertyFlags *flags);
static int draw_asset_memory_allocate_flags(VkMemoryAllocateFlags *flags);
static int draw_asset_attribute(char const *name, uint64_t count, uint64_t stride, void *data);
static int draw_asset_joint(fs_joint_t *joint);
static int draw_asset_descriptor_binding(fs_descriptor_binding_t *descriptor_binding);
static void draw_entity_controls(void);
static void draw_entity(void);

static im_inspector_type_t s_inspector_type = IM_INSPECTOR_TYPE_NONE;
static comp_type_t s_selected_comp = COMP_TYPE_TRANSFORM;

static void *s_selected_data = 0;

static const char *s_component_names[] = {
  "Transform",
  "Camera",
  "Material",
  "Mesh",
  "Skeleton",
};

void im_inspector_draw(void) {
  ImGui::Begin("Inspector", 0, ImGuiWindowFlags_NoDecoration);

  draw_background();

  switch (s_inspector_type) {

    case IM_INSPECTOR_TYPE_ENTITY: {

      draw_entity_controls();
      draw_entity();

      break;
    }
    case IM_INSPECTOR_TYPE_ASSET: {

      draw_asset_controls();
      draw_asset();

      break;
    }
  }

  ImGui::End();
}
void im_inspector_select(im_inspector_type_t type, void *selection) {
  s_inspector_type = type;
  s_selected_data = selection;
}
void im_inspector_reset(void) {
  s_inspector_type = IM_INSPECTOR_TYPE_NONE;
  s_selected_comp = COMP_TYPE_TRANSFORM;

  s_selected_data = 0;
}

static void draw_background(void) {
  ImDrawList *draw = ImGui::GetWindowDrawList();

  ImVec2 position = ImGui::GetWindowPos();
  ImVec2 size = ImGui::GetWindowSize();

  draw->AddRectFilled(
    position,
    ImVec2(position.x + size.x, position.y + size.y),
    TI_DARK_GREY,
    5.0F,
    ImDrawFlags_RoundCornersAll);
}
static void draw_asset_controls(void) {
  // TODO
}

static int draw_asset(void) {
  fs_asset_t *asset = (fs_asset_t *)s_selected_data;

  if (asset == 0) {

    ImGui::TextUnformatted("No asset selected.");

    return 0;
  }

  ImGui::Text("Magic: 0x%016llX", asset->magic);
  ImGui::Text("Asset Type: %d", asset->type);
  ImGui::Text("Config: %p", asset->config);

  if (asset->config == 0) {

    ImGui::TextUnformatted("Asset configuration is unavailable.");

    return 0;
  }

  ImGuiTreeNodeFlags tree_node_flags = ImGuiTreeNodeFlags_OpenOnArrow |
                                       ImGuiTreeNodeFlags_SpanFullWidth |
                                       ImGuiTreeNodeFlags_FramePadding;

  ImGui::PushID(asset);

  switch (asset->type) {

    case FS_ASSET_TYPE_MODEL: {

      fs_model_t *model = (fs_model_t *)asset->config;

      ImGui::Text("Name: %s", model->name);
      ImGui::Text("Mesh Count: %llu", model->mesh_count);
      ImGui::Text("Meshes: %p", model->meshes);
      ImGui::Text("Skin Count: %llu", model->skin_count);
      ImGui::Text("Skins: %p", model->skins);

      ImGui::SeparatorText("Meshes");

      if (model->meshes) {

        uint64_t mesh_index = 0;

        while (mesh_index < model->mesh_count) {

          fs_mesh_t *mesh = &model->meshes[mesh_index];

          if (ImGui::TreeNodeEx(mesh, tree_node_flags, "Mesh %llu: %s", mesh_index, mesh->name)) {

            ImGui::Text("Name: %s", mesh->name);
            ImGui::Text("Primitive Count: %llu", mesh->primitive_count);
            ImGui::Text("Primitives: %p", mesh->primitives);

            if (mesh->primitives) {

              uint64_t primitive_index = 0;

              while (primitive_index < mesh->primitive_count) {

                fs_primitive_t *primitive = &mesh->primitives[primitive_index];

                if (ImGui::TreeNodeEx(primitive, tree_node_flags, "Primitive %llu: %s", primitive_index, primitive->name)) {

                  ImGui::Text("Name: %s", primitive->name);

                  ImGuiTableFlags attribute_flags = ImGuiTableFlags_Borders |
                                                    ImGuiTableFlags_RowBg |
                                                    ImGuiTableFlags_Resizable;

                  if (ImGui::BeginTable("Attributes", 4, attribute_flags)) {

                    ImGui::TableSetupColumn("Attribute");
                    ImGui::TableSetupColumn("Count");
                    ImGui::TableSetupColumn("Stride (bytes)");
                    ImGui::TableSetupColumn("Data");

                    ImGui::TableHeadersRow();

                    draw_asset_attribute("Positions", primitive->position_count, primitive->position_stride, primitive->positions);
                    draw_asset_attribute("Normals", primitive->normal_count, primitive->normal_stride, primitive->normals);
                    draw_asset_attribute("Tangents", primitive->tangent_count, primitive->tangent_stride, primitive->tangents);
                    draw_asset_attribute("Texcoords", primitive->texcoord_count, primitive->texcoord_stride, primitive->texcoords);
                    draw_asset_attribute("Colors", primitive->color_count, primitive->color_stride, primitive->colors);
                    draw_asset_attribute("Joints", primitive->joint_count, primitive->joint_stride, primitive->joints);
                    draw_asset_attribute("Weights", primitive->weight_count, primitive->weight_stride, primitive->weights);

                    ImGui::EndTable();
                  }

                  ImGui::TreePop();
                }

                primitive_index++;
              }
            } else if (mesh->primitive_count > 0) {

              ImGui::TextUnformatted("Primitive data is unavailable.");
            }

            ImGui::TreePop();
          }

          mesh_index++;
        }
      } else if (model->mesh_count > 0) {

        ImGui::TextUnformatted("Mesh data is unavailable.");
      }

      ImGui::SeparatorText("Skins");

      if (model->skins) {

        uint64_t skin_index = 0;

        while (skin_index < model->skin_count) {

          fs_skin_t *skin = &model->skins[skin_index];

          if (ImGui::TreeNodeEx(skin, tree_node_flags, "Skin %llu: %s", skin_index, skin->name)) {

            ImGui::Text("Name: %s", skin->name);
            ImGui::Text("Root Joint: %p", skin->root_joint);

            draw_asset_joint(skin->root_joint);

            ImGui::TreePop();
          }

          skin_index++;
        }
      } else if (model->skin_count > 0) {

        ImGui::TextUnformatted("Skin data is unavailable.");
      }

      break;
    }
    case FS_ASSET_TYPE_PIPELINE: {

      fs_pipeline_t *pipeline = (fs_pipeline_t *)asset->config;

      tree_node_flags |= ImGuiTreeNodeFlags_DefaultOpen;

      ImGui::Text("Name: %s", pipeline->name);
      ImGui::Text("Pipeline Type: %d", pipeline->pipeline_type);
      ImGui::Text("Input Variable Count: %llu", pipeline->input_variable_count);
      ImGui::Text("Input Variables: %p", pipeline->input_variables);
      ImGui::Text("Descriptor Binding Count: %llu", pipeline->descriptor_binding_count);
      ImGui::Text("Descriptor Bindings: %p", pipeline->descriptor_bindings);
      ImGui::Text("Vertex SPIR-V Word Count: %llu", pipeline->spirv_vertex_word_count);
      ImGui::Text("Vertex SPIR-V Words: %p", pipeline->spirv_vertex_words);
      ImGui::Text("Fragment SPIR-V Word Count: %llu", pipeline->spirv_fragment_word_count);
      ImGui::Text("Fragment SPIR-V Words: %p", pipeline->spirv_fragment_words);

      ImGui::SeparatorText("Input Variables");

      if (pipeline->input_variables) {

        uint64_t input_variable_index = 0;

        while (input_variable_index < pipeline->input_variable_count) {

          fs_input_variable_t *input_variable = &pipeline->input_variables[input_variable_index];

          if (ImGui::TreeNodeEx(input_variable, tree_node_flags, "Input %llu: %s", input_variable_index, input_variable->name)) {

            ImGui::Text("Name: %s", input_variable->name);
            ImGui::Text("Location: %u", input_variable->location);
            ImGui::Text("Format: %u", input_variable->format);
            ImGui::Text("Built-In: %u", input_variable->built_in);

            ImGui::TreePop();
          }

          input_variable_index++;
        }
      } else if (pipeline->input_variable_count > 0) {

        ImGui::TextUnformatted("Input variable data is unavailable.");
      }

      ImGui::SeparatorText("Descriptor Bindings");

      if (pipeline->descriptor_bindings) {

        uint64_t descriptor_binding_index = 0;

        while (descriptor_binding_index < pipeline->descriptor_binding_count) {

          fs_descriptor_binding_t *descriptor_binding = &pipeline->descriptor_bindings[descriptor_binding_index];

          if (ImGui::TreeNodeEx(descriptor_binding, tree_node_flags, "Binding %llu: %s", descriptor_binding_index, descriptor_binding->name)) {

            draw_asset_descriptor_binding(descriptor_binding);

            ImGui::TreePop();
          }

          descriptor_binding_index++;
        }
      } else if (pipeline->descriptor_binding_count > 0) {

        ImGui::TextUnformatted("Descriptor binding data is unavailable.");
      }

      break;
    }
    case FS_ASSET_TYPE_FONT: {

      fs_font_t *font = (fs_font_t *)asset->config;

      ImGui::Text("Name: %s", font->name);
      ImGui::Text("Buffer: %p", font->buffer);
      ImGui::Text("Buffer Size (bytes): %llu", font->buffer_size);

      break;
    }
    case FS_ASSET_TYPE_DESCRIPTOR_BINDING: {

      fs_descriptor_binding_t *descriptor_binding = (fs_descriptor_binding_t *)asset->config;

      draw_asset_descriptor_binding(descriptor_binding);

      break;
    }
    case FS_ASSET_TYPE_BUFFER: {

      fs_buffer_t *buffer = (fs_buffer_t *)asset->config;

      ImGui::Text("Name: %s", buffer->name);
      ImGui::Text("Zero Data: %d", buffer->zero_data);
      ImGui::Text("Size (bytes): %llu", buffer->size);

      draw_asset_buffer_usage_flags(&buffer->buffer_usage_flags);
      draw_asset_memory_property_flags(&buffer->memory_property_flags);
      draw_asset_memory_allocate_flags(&buffer->memory_allocate_flags);

      break;
    }
    case FS_ASSET_TYPE_IMAGE: {

      fs_image_t *image = (fs_image_t *)asset->config;

      ImGui::Text("Name: %s", image->name);
      ImGui::Text("Width: %u", image->width);
      ImGui::Text("Height: %u", image->height);
      ImGui::Text("Depth: %u", image->depth);
      ImGui::Text("Mip Levels: %u", image->mip_levels);
      ImGui::Text("Format: %d", image->format);
      ImGui::Text("Image Layout: %d", image->image_layout);
      ImGui::Text("Image Type: %d", image->image_type);
      ImGui::Text("Image Tiling: %d", image->image_tiling);
      ImGui::Text("Image View Type: %d", image->image_view_type);

      draw_asset_image_usage_flags(&image->image_usage_flags);
      draw_asset_image_aspect_flags(&image->image_aspect_flags);
      draw_asset_memory_property_flags(&image->memory_property_flags);
      draw_asset_memory_allocate_flags(&image->memory_allocate_flags);

      break;
    }
    case FS_ASSET_TYPE_FRAMEBUFFER: {

      fs_framebuffer_t *framebuffer = (fs_framebuffer_t *)asset->config;

      ImGui::Text("Name: %s", framebuffer->name);
      ImGui::Text("Width: %u", framebuffer->width);
      ImGui::Text("Height: %u", framebuffer->height);
      ImGui::Text("Color Attachment Image: %s", framebuffer->color_attachment_image);
      ImGui::Text("Depth Attachment Image: %s", framebuffer->depth_attachment_image);

      if (ImGui::Button("Add Color Attachment")) {
        // TODO
      }

      break;
    }
    case FS_ASSET_TYPE_SWAPCHAIN: {

      fs_swapchain_t *swapchain = (fs_swapchain_t *)asset->config;

      ImGui::Text("Name: %s", swapchain->name);

      break;
    }
    case FS_ASSET_TYPE_RENDERPASS: {

      fs_renderpass_t *renderpass = (fs_renderpass_t *)asset->config;

      ImGui::Text("Name: %s", renderpass->name);
      ImGui::Text("Initial Color Attachment Layout: %d", renderpass->initial_color_attachment_layout);
      ImGui::Text("Initial Depth Attachment Layout: %d", renderpass->initial_depth_attachment_layout);
      ImGui::Text("Final Color Attachment Layout: %d", renderpass->final_color_attachment_layout);
      ImGui::Text("Final Depth Attachment Layout: %d", renderpass->final_depth_attachment_layout);

      break;
    }
    case FS_ASSET_TYPE_RENDERER: {

      fs_renderer_t *renderer = (fs_renderer_t *)asset->config;

      ImGui::Text("Name: %s", renderer->name);
      ImGui::Text("Debug Line Vertex Buffer: %s", renderer->debug_line_vertex_buffer);
      ImGui::Text("Debug Line Index Buffer: %s", renderer->debug_line_index_buffer);
      ImGui::Text("Full Screen Vertex Buffer: %s", renderer->full_screen_vertex_buffer);
      ImGui::Text("Full Screen Index Buffer: %s", renderer->full_screen_index_buffer);

      break;
    }
  }

  ImGui::PopID();

  return 1;
}

static int draw_asset_buffer_usage_flags(VkBufferUsageFlags *flags) {
  ImGui::PushID(flags);
  ImGui::SeparatorText("Buffer Usage Flags");

  ImGui::CheckboxFlags("Transfer Source", flags, VK_BUFFER_USAGE_TRANSFER_SRC_BIT);
  ImGui::CheckboxFlags("Transfer Destination", flags, VK_BUFFER_USAGE_TRANSFER_DST_BIT);
  ImGui::CheckboxFlags("Uniform Texel Buffer", flags, VK_BUFFER_USAGE_UNIFORM_TEXEL_BUFFER_BIT);
  ImGui::CheckboxFlags("Storage Texel Buffer", flags, VK_BUFFER_USAGE_STORAGE_TEXEL_BUFFER_BIT);
  ImGui::CheckboxFlags("Uniform Buffer", flags, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);
  ImGui::CheckboxFlags("Storage Buffer", flags, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT);
  ImGui::CheckboxFlags("Index Buffer", flags, VK_BUFFER_USAGE_INDEX_BUFFER_BIT);
  ImGui::CheckboxFlags("Vertex Buffer", flags, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);
  ImGui::CheckboxFlags("Indirect Buffer", flags, VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT);
  ImGui::CheckboxFlags("Shader Device Address", flags, VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT);

  ImGui::Text("Mask: 0x%08X", *flags);
  ImGui::PopID();

  return 1;
}

static int draw_asset_image_usage_flags(VkImageUsageFlags *flags) {
  ImGui::PushID(flags);
  ImGui::SeparatorText("Image Usage Flags");

  ImGui::CheckboxFlags("Transfer Source", flags, VK_IMAGE_USAGE_TRANSFER_SRC_BIT);
  ImGui::CheckboxFlags("Transfer Destination", flags, VK_IMAGE_USAGE_TRANSFER_DST_BIT);
  ImGui::CheckboxFlags("Sampled", flags, VK_IMAGE_USAGE_SAMPLED_BIT);
  ImGui::CheckboxFlags("Storage", flags, VK_IMAGE_USAGE_STORAGE_BIT);
  ImGui::CheckboxFlags("Color Attachment", flags, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT);
  ImGui::CheckboxFlags("Depth Stencil Attachment", flags, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT);
  ImGui::CheckboxFlags("Transient Attachment", flags, VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT);
  ImGui::CheckboxFlags("Input Attachment", flags, VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT);
  ImGui::CheckboxFlags("Host Transfer", flags, VK_IMAGE_USAGE_HOST_TRANSFER_BIT);

  ImGui::Text("Mask: 0x%08X", *flags);
  ImGui::PopID();

  return 1;
}

static int draw_asset_image_aspect_flags(VkImageAspectFlags *flags) {
  ImGui::PushID(flags);
  ImGui::SeparatorText("Image Aspect Flags");

  ImGui::CheckboxFlags("Color", flags, VK_IMAGE_ASPECT_COLOR_BIT);
  ImGui::CheckboxFlags("Depth", flags, VK_IMAGE_ASPECT_DEPTH_BIT);
  ImGui::CheckboxFlags("Stencil", flags, VK_IMAGE_ASPECT_STENCIL_BIT);
  ImGui::CheckboxFlags("Metadata", flags, VK_IMAGE_ASPECT_METADATA_BIT);
  ImGui::CheckboxFlags("Plane 0", flags, VK_IMAGE_ASPECT_PLANE_0_BIT);
  ImGui::CheckboxFlags("Plane 1", flags, VK_IMAGE_ASPECT_PLANE_1_BIT);
  ImGui::CheckboxFlags("Plane 2", flags, VK_IMAGE_ASPECT_PLANE_2_BIT);

  ImGui::Text("Mask: 0x%08X", *flags);
  ImGui::PopID();

  return 1;
}

static int draw_asset_memory_property_flags(VkMemoryPropertyFlags *flags) {
  ImGui::PushID(flags);
  ImGui::SeparatorText("Memory Property Flags");

  ImGui::CheckboxFlags("Device Local", flags, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
  ImGui::CheckboxFlags("Host Visible", flags, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
  ImGui::CheckboxFlags("Host Coherent", flags, VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
  ImGui::CheckboxFlags("Host Cached", flags, VK_MEMORY_PROPERTY_HOST_CACHED_BIT);
  ImGui::CheckboxFlags("Lazily Allocated", flags, VK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT);
  ImGui::CheckboxFlags("Protected", flags, VK_MEMORY_PROPERTY_PROTECTED_BIT);

  ImGui::Text("Mask: 0x%08X", *flags);
  ImGui::PopID();

  return 1;
}

static int draw_asset_memory_allocate_flags(VkMemoryAllocateFlags *flags) {
  ImGui::PushID(flags);
  ImGui::SeparatorText("Memory Allocate Flags");

  ImGui::CheckboxFlags("Device Mask", flags, VK_MEMORY_ALLOCATE_DEVICE_MASK_BIT);
  ImGui::CheckboxFlags("Device Address", flags, VK_MEMORY_ALLOCATE_DEVICE_ADDRESS_BIT);
  ImGui::CheckboxFlags("Device Address Capture Replay", flags, VK_MEMORY_ALLOCATE_DEVICE_ADDRESS_CAPTURE_REPLAY_BIT);

  ImGui::Text("Mask: 0x%08X", *flags);
  ImGui::PopID();

  return 1;
}

static int draw_asset_attribute(char const *name, uint64_t count, uint64_t stride, void *data) {
  ImGui::TableNextRow();

  ImGui::TableNextColumn();
  ImGui::TextUnformatted(name);

  ImGui::TableNextColumn();
  ImGui::Text("%llu", count);

  ImGui::TableNextColumn();
  ImGui::Text("%llu", stride);

  ImGui::TableNextColumn();
  ImGui::Text("%p", data);

  return 1;
}

static int draw_asset_joint(fs_joint_t *joint) {
  if (joint == 0) {

    ImGui::TextUnformatted("Joint data is unavailable.");

    return 0;
  }

  ImGuiTreeNodeFlags tree_node_flags = ImGuiTreeNodeFlags_OpenOnArrow |
                                       ImGuiTreeNodeFlags_SpanFullWidth |
                                       ImGuiTreeNodeFlags_FramePadding;

  if (ImGui::TreeNodeEx(joint, tree_node_flags, "Joint: %s", joint->name)) {

    ImGui::Text("Name: %s", joint->name);
    ImGui::Text("Child Count: %llu", joint->child_count);
    ImGui::Text("Children: %p", joint->children);

    if (joint->children) {

      uint64_t child_index = 0;

      while (child_index < joint->child_count) {

        draw_asset_joint(&joint->children[child_index]);

        child_index++;
      }
    } else if (joint->child_count > 0) {

      ImGui::TextUnformatted("Child joint data is unavailable.");
    }

    ImGui::TreePop();
  }

  return 1;
}

static int draw_asset_descriptor_binding(fs_descriptor_binding_t *descriptor_binding) {
  ImGui::Text("Name: %s", descriptor_binding->name);
  ImGui::Text("Set: %u", descriptor_binding->set);
  ImGui::Text("Binding: %u", descriptor_binding->binding);
  ImGui::Text("Descriptor Type: %d", descriptor_binding->descriptor_type);
  ImGui::Text("Block Size (bytes): %d", descriptor_binding->block_size);
  ImGui::Text("Block Variable Count: %d", descriptor_binding->block_variable_count);
  ImGui::Text("Block Variables: %p", descriptor_binding->block_variables);

  if (descriptor_binding->block_variable_count < 0) {

    ImGui::TextUnformatted("Invalid block variable count.");

    return 0;
  }

  if (descriptor_binding->block_variable_count == 0) {
    return 1;
  }

  if (descriptor_binding->block_variables == 0) {

    ImGui::TextUnformatted("Block variable data is unavailable.");

    return 0;
  }

  ImGuiTableFlags block_variable_flags = ImGuiTableFlags_Borders |
                                         ImGuiTableFlags_RowBg |
                                         ImGuiTableFlags_Resizable;

  ImGui::PushID(descriptor_binding);

  if (ImGui::BeginTable("Block Variables", 3, block_variable_flags)) {

    ImGui::TableSetupColumn("Name");
    ImGui::TableSetupColumn("Offset (bytes)");
    ImGui::TableSetupColumn("Size (bytes)");

    ImGui::TableHeadersRow();

    int32_t block_variable_index = 0;

    while (block_variable_index < descriptor_binding->block_variable_count) {

      fs_block_variable_t *block_variable = &descriptor_binding->block_variables[block_variable_index];

      ImGui::TableNextRow();

      ImGui::TableNextColumn();
      ImGui::Text("%s", block_variable->name);

      ImGui::TableNextColumn();
      ImGui::Text("%u", block_variable->offset);

      ImGui::TableNextColumn();
      ImGui::Text("%u", block_variable->size);

      block_variable_index++;
    }

    ImGui::EndTable();
  }

  ImGui::PopID();

  return 1;
}

static void draw_entity_controls(void) {
  if (ImGui::BeginCombo("##Component", s_component_names[s_selected_comp])) {

    uint64_t comp_index = 0;
    uint64_t comp_count = TI_ARRAY_COUNT(s_component_names);

    while (comp_index < comp_count) {

      bool selected = (comp_index == s_selected_comp);

      if (ImGui::Selectable(s_component_names[comp_index], selected)) {
        s_selected_comp = (comp_type_t)comp_index;
      }

      if (selected) {
        ImGui::SetItemDefaultFocus();
      }

      comp_index++;
    }

    ImGui::EndCombo();
  }

  ImGui::SameLine();

  ecs_entity_t entity = (ecs_entity_t)s_selected_data;

  if (ImGui::Button("Add")) {

    switch (s_selected_comp) {

      case COMP_TYPE_TRANSFORM: {

        ecs_add(g_scene.world, entity, transform_t);

        transform_t *transform = ecs_get_mut(g_scene.world, entity, transform_t);

        transform_init(transform);

        break;
      }
      case COMP_TYPE_CAMERA: {

        ecs_add(g_scene.world, entity, camera_t);

        camera_t *camera = ecs_get_mut(g_scene.world, entity, camera_t);

        camera_init(camera);

        break;
      }
      case COMP_TYPE_MATERIAL: {

        ecs_add(g_scene.world, entity, material_t);

        material_t *material = ecs_get_mut(g_scene.world, entity, material_t);

        material_init(material);

        break;
      }
      case COMP_TYPE_MESH: {

        ecs_add(g_scene.world, entity, mesh_t);

        mesh_t *mesh = ecs_get_mut(g_scene.world, entity, mesh_t);

        mesh_init(mesh);

        break;
      }
      case COMP_TYPE_SKELETON: {

        ecs_add(g_scene.world, entity, skeleton_t);

        skeleton_t *skeleton = ecs_get_mut(g_scene.world, entity, skeleton_t);

        skeleton_init(skeleton);

        break;
      }
    }
  }

  ImGui::SameLine();

  if (ImGui::Button("Remove")) {

    switch (s_selected_comp) {

      case COMP_TYPE_TRANSFORM: {

        ecs_remove(g_scene.world, entity, transform_t);

        break;
      }
      case COMP_TYPE_CAMERA: {

        ecs_remove(g_scene.world, entity, camera_t);

        break;
      }
      case COMP_TYPE_MATERIAL: {

        ecs_remove(g_scene.world, entity, material_t);

        break;
      }
      case COMP_TYPE_MESH: {

        ecs_remove(g_scene.world, entity, mesh_t);

        break;
      }
      case COMP_TYPE_SKELETON: {

        ecs_remove(g_scene.world, entity, skeleton_t);

        break;
      }
    }
  }
}
static void draw_entity(void) {
  ecs_entity_t entity = (ecs_entity_t)s_selected_data;

  ImGuiTreeNodeFlags tree_node_flags = ImGuiTreeNodeFlags_OpenOnArrow |
                                       ImGuiTreeNodeFlags_SpanFullWidth |
                                       ImGuiTreeNodeFlags_FramePadding;

  transform_t *transform = ecs_get_mut(g_scene.world, entity, transform_t);
  camera_t *camera = ecs_get_mut(g_scene.world, entity, camera_t);
  material_t *material = ecs_get_mut(g_scene.world, entity, material_t);
  mesh_t *mesh = ecs_get_mut(g_scene.world, entity, mesh_t);
  skeleton_t *skeleton = ecs_get_mut(g_scene.world, entity, skeleton_t);

  if (transform) {

    if (ImGui::TreeNodeEx("Transform", tree_node_flags)) {

      fvec3_t p = {transform->position_x, transform->position_y, transform->position_z};
      fvec4_t r = {transform->rotation_x, transform->rotation_y, transform->rotation_z, transform->rotation_w};
      fvec3_t s = {transform->scale_x, transform->scale_y, transform->scale_z};

      if (ImGui::DragFloat3("Position", (float *)&p, 0.01F, 0.0F, 0.0F, "%.3F", 0)) {

        transform->position_x = p.x;
        transform->position_y = p.y;
        transform->position_z = p.z;

        // TODO
      }

      if (ImGui::DragFloat4("Rotation", (float *)&r, 0.01F, 0.0F, 0.0F, "%.3F", 0)) {

        transform->rotation_x = r.x;
        transform->rotation_y = r.y;
        transform->rotation_z = r.z;
        transform->rotation_w = r.w;

        // TODO
      }

      if (ImGui::DragFloat3("Scale", (float *)&s, 0.01F, 0.0F, 0.0F, "%.3F", 0)) {

        transform->scale_x = s.x;
        transform->scale_y = s.y;
        transform->scale_z = s.z;

        // TODO
      }

      ImGui::TreePop();
    }
  }

  if (camera) {

    if (ImGui::TreeNodeEx("Camera", tree_node_flags)) {

      if (ImGui::Checkbox("Enable Debug", (bool *)&camera->is_debug_enabled)) {
        // TODO
      }

      if (ImGui::DragFloat("Fov", &camera->fov, 0.01F, 0.0F, 0.0F, "%.3F", 0)) {
        // TODO
      }

      if (ImGui::DragFloat("Near Z", &camera->near_z, 0.01F, 0.0F, 0.0F, "%.3F", 0)) {
        // TODO
      }

      if (ImGui::DragFloat("Far Z", &camera->far_z, 0.01F, 0.0F, 0.0F, "%.3F", 0)) {
        // TODO
      }

      ImGui::TreePop();
    }
  }

  if (material) {

    if (ImGui::TreeNodeEx("Material", tree_node_flags)) {

      if (ImGui::InputText("Pipeline", material->pipeline, TI_PATH_SIZE)) {
        // TODO
      }

      if (ImGui::InputText("Material", material->material, TI_PATH_SIZE)) {
        // TODO
      }

      ImGui::TreePop();
    }
  }

  if (mesh) {

    if (ImGui::TreeNodeEx("Mesh", tree_node_flags)) {

      if (ImGui::InputText("Mesh", mesh->mesh, TI_PATH_SIZE)) {
        // TODO
      }

      ImGui::TreePop();
    }
  }

  if (skeleton) {

    if (ImGui::TreeNodeEx("Skeleton", tree_node_flags)) {

      if (ImGui::InputText("Skeleton", skeleton->skeleton, TI_PATH_SIZE)) {
        // TODO
      }

      ImGui::TreePop();
    }
  }
}
