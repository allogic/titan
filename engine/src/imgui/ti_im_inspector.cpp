#include <ti_pch.h>

#include <imgui.h>

static void draw_background(void);
static void draw_asset_controls(void);
static void draw_asset(void);
static void draw_entity_controls(void);
static void draw_entity(void);

static bool draw_buffer_usage_flags(VkBufferUsageFlags *flags);
static bool draw_memory_property_flags(VkMemoryPropertyFlags *flags);
static bool draw_memory_allocate_flags(VkMemoryAllocateFlags *flags);
static bool draw_image_usage_flags(VkImageUsageFlags *flags);
static bool draw_image_aspect_flags(VkImageAspectFlags *flags);
static bool draw_cull_mode_flags(VkCullModeFlags *flags);
static bool draw_shader_stage_flags(VkShaderStageFlags *flags);

static bool draw_vulkan_enum_dropdown(char const *label, uint64_t *selected_index, vk_enum_record_t *table, uint64_t table_count);

static im_inspector_type_t s_selected_type = IM_INSPECTOR_TYPE_NONE;
static cp_component_type_t s_selected_comp = CP_COMPONENT_TYPE_TRANSFORM;
static ecs_entity_t s_selected_entity = 0;
static fs_asset_t *s_selected_asset = 0;

static const char *s_component_name[] = {
  "Transform",
  "Camera",
  "Material",
  "Mesh",
  "Skeleton",
};

static char s_asset_path[TI_PATH_SIZE] = {0};

static char s_vertex_input_binding_description_name[TI_PATH_SIZE] = {0};
static char s_vertex_input_attribute_description_name[TI_PATH_SIZE] = {0};
static char s_descriptor_pool_size_name[TI_PATH_SIZE] = {0};
static char s_descriptor_set_layout_binding_name[TI_PATH_SIZE] = {0};

void im_inspector_draw(void) {
  ImGui::Begin("Inspector", 0, ImGuiWindowFlags_NoDecoration);

  draw_background();

  switch (s_selected_type) {

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
void im_inspector_select(im_inspector_type_t type, void *data) {
  switch (type) {

    case IM_INSPECTOR_TYPE_ENTITY: {

      s_selected_type = type;
      s_selected_entity = (ecs_entity_t)data;

      break;
    }
    case IM_INSPECTOR_TYPE_ASSET: {

      if (s_selected_asset) {

        fs_asset_destroy(s_selected_asset);

        TI_FREE(s_selected_asset);
      }

      strcpy(s_asset_path, (char const *)data);

      s_selected_asset = (fs_asset_t *)TI_ALLOC(sizeof(fs_asset_t), 0, 0);

      s_selected_asset->path = s_asset_path;

      fs_asset_load(s_selected_asset);

      s_selected_type = type;

      break;
    }
  }
}
void im_inspector_reset(void) {
  if (s_selected_asset) {

    fs_asset_destroy(s_selected_asset);

    TI_FREE(s_selected_asset);
  }

  s_asset_path[0] = 0;

  s_selected_type = IM_INSPECTOR_TYPE_NONE;
  s_selected_comp = CP_COMPONENT_TYPE_TRANSFORM;
  s_selected_entity = 0;
  s_selected_asset = 0;
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
static void draw_asset(void) {
  bool dirty = false;

  ImGuiTreeNodeFlags tree_node_flags = ImGuiTreeNodeFlags_OpenOnArrow |
                                       ImGuiTreeNodeFlags_SpanFullWidth |
                                       ImGuiTreeNodeFlags_FramePadding |
                                       ImGuiTreeNodeFlags_Framed;

  switch (s_selected_asset->type) {

    case FS_ASSET_TYPE_MODEL: {

      fs_model_t *model = (fs_model_t *)s_selected_asset->instance;

      ImGui::Text("Mesh Count: %llu", model->mesh_count);

      uint64_t mesh_index = 0;
      uint64_t mesh_count = model->mesh_count;

      while (mesh_index < mesh_count) {

        fs_mesh_t *mesh = &model->meshes[mesh_index];

        if (ImGui::TreeNodeEx(mesh->name, tree_node_flags)) {

          ImGui::Text("Primitive Count: %llu", mesh->primitive_count);

          uint64_t primitive_index = 0;
          uint64_t primitive_count = mesh->primitive_count;

          while (primitive_index < primitive_count) {

            fs_primitive_t *primitive = &mesh->primitives[primitive_index];

            if (ImGui::TreeNodeEx(primitive->name, tree_node_flags)) {

              ImGui::Text("Position Count: %llu", primitive->position_count);
              ImGui::Text("Normal Count: %llu", primitive->normal_count);
              ImGui::Text("Tangent Count: %llu", primitive->tangent_count);
              ImGui::Text("Texcoord Count: %llu", primitive->texcoord_count);
              ImGui::Text("Color Count: %llu", primitive->color_count);
              ImGui::Text("Joint Count: %llu", primitive->joint_count);
              ImGui::Text("Weight Count: %llu", primitive->weight_count);

              ImGui::TreePop();
            }

            primitive_index++;
          }

          ImGui::TreePop();
        }

        mesh_index++;
      }

      break;
    }
    case FS_ASSET_TYPE_PIPELINE: {

      fs_pipeline_t *pipeline = (fs_pipeline_t *)s_selected_asset->instance;

      if (ImGui::Button("Vertex Shader")) {

        // pipeline->

        // TODO: im_text_editor_load();
      }

      ImGui::SameLine();

      if (ImGui::Button("Fragment Shader")) {

        // TODO: im_text_editor_load();
      }

      dirty |= ImGui::Checkbox("Enable Blending", (bool *)&pipeline->enable_blending);
      dirty |= ImGui::Checkbox("Enable Depth Test", (bool *)&pipeline->enable_depth_test);
      dirty |= ImGui::Checkbox("Enable Depth Write", (bool *)&pipeline->enable_depth_write);

      dirty |= ImGui::InputScalar("Descriptor Set Count", ImGuiDataType_U32, &pipeline->descriptor_set_count, 0, 0, "%lu", ImGuiInputTextFlags_EnterReturnsTrue);

      ImGui::SeparatorText("Cull Mode Flags");
      dirty |= draw_cull_mode_flags(&pipeline->cull_mode_flags);

      ImGui::SeparatorText("Input Variables");

      uint64_t input_variable_index = 0;
      uint64_t input_variable_count = pipeline->input_variable_count;

      while (input_variable_index < input_variable_count) {

        fs_asset_reference_t *input_variable = &pipeline->input_variable[input_variable_index];

        ImGui::PushID(input_variable);

        dirty |= ImGui::InputText("##Input Variable", input_variable->reference_path, TI_PATH_SIZE, ImGuiInputTextFlags_EnterReturnsTrue);

        ImGui::PopID();

        input_variable_index++;
      }

      ImGui::PushID("Input Variables");
      ImGui::PushFont((ImFont *)g_im_font_symbols_18);
      if (ImGui::Button(ICON_MS_ADD)) {

        uint64_t old_input_variable_count = pipeline->input_variable_count;
        uint64_t new_input_variable_count = pipeline->input_variable_count + 1;

        fs_asset_reference_t *old_input_variable = pipeline->input_variable;
        fs_asset_reference_t *new_input_variable = (fs_asset_reference_t *)TI_ALLOC(sizeof(fs_asset_reference_t) * new_input_variable_count, 0, 0);

        memcpy(new_input_variable, old_input_variable, sizeof(fs_asset_reference_t) * old_input_variable_count);

        TI_FREE(old_input_variable);

        pipeline->input_variable_count = new_input_variable_count;
        pipeline->input_variable = new_input_variable;

        dirty = true;
      }
      ImGui::PopFont();
      ImGui::PopID();

      ImGui::SeparatorText("Descriptor Bindings");

      uint64_t descriptor_binding_index = 0;
      uint64_t descriptor_binding_count = pipeline->descriptor_binding_count;

      while (descriptor_binding_index < descriptor_binding_count) {

        fs_asset_reference_t *descriptor_binding = &pipeline->descriptor_binding[descriptor_binding_index];

        ImGui::PushID(descriptor_binding);

        dirty |= ImGui::InputText("##Descriptor Binding", descriptor_binding->reference_path, TI_PATH_SIZE, ImGuiInputTextFlags_EnterReturnsTrue);

        ImGui::PopID();

        descriptor_binding_index++;
      }

      ImGui::PushID("Descriptor Bindings");
      ImGui::PushFont((ImFont *)g_im_font_symbols_18);
      if (ImGui::Button(ICON_MS_ADD)) {

        uint64_t old_descriptor_binding_count = pipeline->descriptor_binding_count;
        uint64_t new_descriptor_binding_count = pipeline->descriptor_binding_count + 1;

        fs_asset_reference_t *old_descriptor_binding = pipeline->descriptor_binding;
        fs_asset_reference_t *new_descriptor_binding = (fs_asset_reference_t *)TI_ALLOC(sizeof(fs_asset_reference_t) * new_descriptor_binding_count, 0, 0);

        memcpy(new_descriptor_binding, old_descriptor_binding, sizeof(fs_asset_reference_t) * old_descriptor_binding_count);

        TI_FREE(old_descriptor_binding);

        pipeline->descriptor_binding_count = new_descriptor_binding_count;
        pipeline->descriptor_binding = new_descriptor_binding;

        dirty = true;
      }
      ImGui::PopFont();
      ImGui::PopID();

      ImGui::SeparatorText("Descriptor Pool Size");

      uint64_t descriptor_pool_size_index = 0;
      uint64_t descriptor_pool_size_count = pipeline->descriptor_pool_size_count;

      while (descriptor_pool_size_index < descriptor_pool_size_count) {

        fs_descriptor_pool_size_t *descriptor_pool_size = &pipeline->descriptor_pool_size[descriptor_pool_size_index];

        if (descriptor_pool_size->name[0] != 0) {

          ImGui::PushID(descriptor_pool_size);
          if (ImGui::TreeNodeEx(descriptor_pool_size->name, tree_node_flags)) {

            dirty |= draw_vulkan_enum_dropdown("Type", &descriptor_pool_size->type_index, g_vk_descriptor_type_table, TI_ARRAY_COUNT(g_vk_descriptor_type_table));

            dirty |= ImGui::InputScalar("Descriptor Count", ImGuiDataType_U32, &descriptor_pool_size->descriptor_count, 0, 0, "%lu", ImGuiInputTextFlags_EnterReturnsTrue);

            ImGui::TreePop();
          }
          ImGui::PopID();
        }

        descriptor_pool_size_index++;
      }

      ImGui::PushID("Descriptor Pool Size");
      ImGui::PushFont((ImFont *)g_im_font_symbols_18);
      if (ImGui::Button(ICON_MS_ADD)) {

        if (s_descriptor_pool_size_name[0] != 0) {

          uint64_t old_descriptor_pool_size_count = pipeline->descriptor_pool_size_count;
          uint64_t new_descriptor_pool_size_count = pipeline->descriptor_pool_size_count + 1;

          fs_descriptor_pool_size_t *old_descriptor_pool_size = pipeline->descriptor_pool_size;
          fs_descriptor_pool_size_t *new_descriptor_pool_size = (fs_descriptor_pool_size_t *)TI_ALLOC(sizeof(fs_descriptor_pool_size_t) * new_descriptor_pool_size_count, 0, 0);

          memcpy(new_descriptor_pool_size, old_descriptor_pool_size, sizeof(fs_descriptor_pool_size_t) * old_descriptor_pool_size_count);

          TI_FREE(old_descriptor_pool_size);

          pipeline->descriptor_pool_size_count = new_descriptor_pool_size_count;
          pipeline->descriptor_pool_size = new_descriptor_pool_size;

          strcpy(pipeline->descriptor_pool_size[pipeline->descriptor_pool_size_count - 1].name, s_descriptor_pool_size_name);

          dirty = true;
        }
      }
      ImGui::PopFont();
      ImGui::SameLine();
      ImGui::InputText("##Input", s_descriptor_pool_size_name, TI_PATH_SIZE);
      ImGui::PopID();

      ImGui::SeparatorText("Descriptor Set Layout Binding");

      uint64_t descriptor_set_layout_binding_index = 0;
      uint64_t descriptor_set_layout_binding_count = pipeline->descriptor_set_layout_binding_count;

      while (descriptor_set_layout_binding_index < descriptor_set_layout_binding_count) {

        fs_descriptor_set_layout_binding_t *descriptor_set_layout_binding = &pipeline->descriptor_set_layout_binding[descriptor_set_layout_binding_index];

        if (descriptor_set_layout_binding->name[0] != 0) {

          ImGui::PushID(descriptor_set_layout_binding);
          if (ImGui::TreeNodeEx(descriptor_set_layout_binding->name, tree_node_flags)) {

            dirty |= draw_vulkan_enum_dropdown("Type", &descriptor_set_layout_binding->descriptor_type_index, g_vk_descriptor_type_table, TI_ARRAY_COUNT(g_vk_descriptor_type_table));

            dirty |= ImGui::InputScalar("Binding", ImGuiDataType_U32, &descriptor_set_layout_binding->binding, 0, 0, "%lu", ImGuiInputTextFlags_EnterReturnsTrue);
            dirty |= ImGui::InputScalar("Descriptor Count", ImGuiDataType_U32, &descriptor_set_layout_binding->descriptor_count, 0, 0, "%lu", ImGuiInputTextFlags_EnterReturnsTrue);

            dirty |= draw_shader_stage_flags(&descriptor_set_layout_binding->stage_flags);

            ImGui::TreePop();
          }
          ImGui::PopID();
        }

        descriptor_set_layout_binding_index++;
      }

      ImGui::PushID("Descriptor Set Layout Binding");
      ImGui::PushFont((ImFont *)g_im_font_symbols_18);
      if (ImGui::Button(ICON_MS_ADD)) {

        if (s_descriptor_set_layout_binding_name[0] != 0) {

          uint64_t old_descriptor_set_layout_binding_count = pipeline->descriptor_set_layout_binding_count;
          uint64_t new_descriptor_set_layout_binding_count = pipeline->descriptor_set_layout_binding_count + 1;

          fs_descriptor_set_layout_binding_t *old_descriptor_set_layout_binding = pipeline->descriptor_set_layout_binding;
          fs_descriptor_set_layout_binding_t *new_descriptor_set_layout_binding = (fs_descriptor_set_layout_binding_t *)TI_ALLOC(sizeof(fs_descriptor_set_layout_binding_t) * new_descriptor_set_layout_binding_count, 0, 0);

          memcpy(new_descriptor_set_layout_binding, old_descriptor_set_layout_binding, sizeof(fs_descriptor_set_layout_binding_t) * old_descriptor_set_layout_binding_count);

          TI_FREE(old_descriptor_set_layout_binding);

          pipeline->descriptor_set_layout_binding_count = new_descriptor_set_layout_binding_count;
          pipeline->descriptor_set_layout_binding = new_descriptor_set_layout_binding;

          strcpy(pipeline->descriptor_set_layout_binding[pipeline->descriptor_set_layout_binding_count - 1].name, s_descriptor_set_layout_binding_name);

          dirty = true;
        }
      }
      ImGui::PopFont();
      ImGui::SameLine();
      ImGui::InputText("##Input", s_descriptor_set_layout_binding_name, TI_PATH_SIZE);
      ImGui::PopID();

      switch (pipeline->pipeline_type) {

        case FS_PIPELINE_TYPE_DEFAULT: {

          ImGui::SeparatorText("Vertex Input Binding Description");

          uint64_t vertex_input_binding_description_index = 0;
          uint64_t vertex_input_binding_description_count = pipeline->vertex_input_binding_description_count;

          while (vertex_input_binding_description_index < vertex_input_binding_description_count) {

            fs_vertex_input_binding_description_t *vertex_input_binding_description = &pipeline->vertex_input_binding_description[vertex_input_binding_description_index];

            if (vertex_input_binding_description->name[0] != 0) {

              ImGui::PushID(vertex_input_binding_description);
              if (ImGui::TreeNodeEx(vertex_input_binding_description->name, tree_node_flags)) {

                dirty |= ImGui::InputScalar("Binding", ImGuiDataType_U32, &vertex_input_binding_description->binding, 0, 0, "%lu", ImGuiInputTextFlags_EnterReturnsTrue);
                dirty |= ImGui::InputScalar("Stride", ImGuiDataType_U32, &vertex_input_binding_description->stride, 0, 0, "%lu", ImGuiInputTextFlags_EnterReturnsTrue);

                dirty |= draw_vulkan_enum_dropdown("Input Rate", &vertex_input_binding_description->input_rate_index, g_vk_vertex_input_rate_table, TI_ARRAY_COUNT(g_vk_vertex_input_rate_table));

                ImGui::TreePop();
              }
              ImGui::PopID();
            }

            vertex_input_binding_description_index++;
          }

          ImGui::PushID("Vertex Input Binding Description");
          ImGui::PushFont((ImFont *)g_im_font_symbols_18);
          if (ImGui::Button(ICON_MS_ADD)) {

            if (s_vertex_input_binding_description_name[0] != 0) {

              uint64_t old_vertex_input_binding_description_count = pipeline->vertex_input_binding_description_count;
              uint64_t new_vertex_input_binding_description_count = pipeline->vertex_input_binding_description_count + 1;

              fs_vertex_input_binding_description_t *old_vertex_input_binding_description = pipeline->vertex_input_binding_description;
              fs_vertex_input_binding_description_t *new_vertex_input_binding_description = (fs_vertex_input_binding_description_t *)TI_ALLOC(sizeof(fs_vertex_input_binding_description_t) * new_vertex_input_binding_description_count, 0, 0);

              memcpy(new_vertex_input_binding_description, old_vertex_input_binding_description, sizeof(fs_vertex_input_binding_description_t) * old_vertex_input_binding_description_count);

              TI_FREE(old_vertex_input_binding_description);

              pipeline->vertex_input_binding_description_count = new_vertex_input_binding_description_count;
              pipeline->vertex_input_binding_description = new_vertex_input_binding_description;

              strcpy(pipeline->vertex_input_binding_description[pipeline->vertex_input_binding_description_count - 1].name, s_vertex_input_binding_description_name);

              dirty = true;
            }
          }
          ImGui::PopFont();
          ImGui::SameLine();
          ImGui::InputText("##Input", s_vertex_input_binding_description_name, TI_PATH_SIZE);
          ImGui::PopID();

          ImGui::SeparatorText("Vertex Input Attribute Description");

          uint64_t vertex_input_attribute_description_index = 0;
          uint64_t vertex_input_attribute_description_count = pipeline->vertex_input_attribute_description_count;

          while (vertex_input_attribute_description_index < vertex_input_attribute_description_count) {

            fs_vertex_input_attribute_description_t *vertex_input_attribute_description = &pipeline->vertex_input_attribute_description[vertex_input_attribute_description_index];

            if (vertex_input_attribute_description->name[0] != 0) {

              ImGui::PushID(vertex_input_attribute_description);
              if (ImGui::TreeNodeEx(vertex_input_attribute_description->name, tree_node_flags)) {

                dirty |= ImGui::InputScalar("Location", ImGuiDataType_U32, &vertex_input_attribute_description->location, 0, 0, "%lu", ImGuiInputTextFlags_EnterReturnsTrue);
                dirty |= ImGui::InputScalar("Binding", ImGuiDataType_U32, &vertex_input_attribute_description->binding, 0, 0, "%lu", ImGuiInputTextFlags_EnterReturnsTrue);
                dirty |= ImGui::InputScalar("Offset", ImGuiDataType_U32, &vertex_input_attribute_description->offset, 0, 0, "%lu", ImGuiInputTextFlags_EnterReturnsTrue);

                dirty |= draw_vulkan_enum_dropdown("Format", &vertex_input_attribute_description->format_index, g_vk_format_table, TI_ARRAY_COUNT(g_vk_format_table));

                ImGui::TreePop();
              }
              ImGui::PopID();
            }

            vertex_input_attribute_description_index++;
          }

          ImGui::PushID("Vertex Input Attribute Description");
          ImGui::PushFont((ImFont *)g_im_font_symbols_18);
          if (ImGui::Button(ICON_MS_ADD)) {

            if (s_vertex_input_attribute_description_name[0] != 0) {

              uint64_t old_vertex_input_attribute_description_count = pipeline->vertex_input_attribute_description_count;
              uint64_t new_vertex_input_attribute_description_count = pipeline->vertex_input_attribute_description_count + 1;

              fs_vertex_input_attribute_description_t *old_vertex_input_attribute_description = pipeline->vertex_input_attribute_description;
              fs_vertex_input_attribute_description_t *new_vertex_input_attribute_description = (fs_vertex_input_attribute_description_t *)TI_ALLOC(sizeof(fs_vertex_input_attribute_description_t) * new_vertex_input_attribute_description_count, 0, 0);

              memcpy(new_vertex_input_attribute_description, old_vertex_input_attribute_description, sizeof(fs_vertex_input_attribute_description_t) * old_vertex_input_attribute_description_count);

              TI_FREE(old_vertex_input_attribute_description);

              pipeline->vertex_input_attribute_description_count = new_vertex_input_attribute_description_count;
              pipeline->vertex_input_attribute_description = new_vertex_input_attribute_description;

              strcpy(pipeline->vertex_input_attribute_description[pipeline->vertex_input_attribute_description_count - 1].name, s_vertex_input_attribute_description_name);

              dirty = true;
            }
          }
          ImGui::PopFont();
          ImGui::SameLine();
          ImGui::InputText("##Input", s_vertex_input_attribute_description_name, TI_PATH_SIZE);
          ImGui::PopID();

          break;
        }
      }

      break;
    }
    case FS_ASSET_TYPE_FONT: {

      fs_font_t *font = (fs_font_t *)s_selected_asset->instance;

      // TODO

      break;
    }
    case FS_ASSET_TYPE_INPUT_VARIABLE: {

      fs_input_variable_t *input_variable = (fs_input_variable_t *)s_selected_asset->instance;

      dirty |= ImGui::InputText("Name", input_variable->name, TI_PATH_SIZE, ImGuiInputTextFlags_EnterReturnsTrue);
      dirty |= ImGui::InputScalar("Location", ImGuiDataType_U32, &input_variable->location, 0, 0, "%lu", ImGuiInputTextFlags_EnterReturnsTrue);
      dirty |= ImGui::Checkbox("Built In", (bool *)&input_variable->is_built_in);

      dirty |= draw_vulkan_enum_dropdown("Format", &input_variable->format_index, g_vk_format_table, TI_ARRAY_COUNT(g_vk_format_table));

      break;
    }
    case FS_ASSET_TYPE_DESCRIPTOR_BINDING: {

      fs_descriptor_binding_t *descriptor_binding = (fs_descriptor_binding_t *)s_selected_asset->instance;

      dirty |= ImGui::InputText("Name", descriptor_binding->name, TI_PATH_SIZE, ImGuiInputTextFlags_EnterReturnsTrue);
      dirty |= ImGui::InputScalar("Set", ImGuiDataType_U32, &descriptor_binding->set, 0, 0, "%lu", ImGuiInputTextFlags_EnterReturnsTrue);
      dirty |= ImGui::InputScalar("Binding", ImGuiDataType_U32, &descriptor_binding->binding, 0, 0, "%lu", ImGuiInputTextFlags_EnterReturnsTrue);

      dirty |= draw_vulkan_enum_dropdown("Descriptor Type", &descriptor_binding->descriptor_type_index, g_vk_descriptor_type_table, TI_ARRAY_COUNT(g_vk_descriptor_type_table));

      if ((g_vk_descriptor_type_table[descriptor_binding->descriptor_type_index].value == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER) ||
          (g_vk_descriptor_type_table[descriptor_binding->descriptor_type_index].value == VK_DESCRIPTOR_TYPE_STORAGE_BUFFER)) {

        dirty |= ImGui::InputScalar("Block Size", ImGuiDataType_S32, &descriptor_binding->block_size, 0, 0, "%ld", ImGuiInputTextFlags_EnterReturnsTrue);
        dirty |= ImGui::InputScalar("Block Variable Count", ImGuiDataType_S32, &descriptor_binding->block_variable_count, 0, 0, "%ld", ImGuiInputTextFlags_EnterReturnsTrue);

        // TODO: add the possability to add/remove/edit the block variables..

        ImGuiTableFlags block_variable_flags = ImGuiTableFlags_Borders |
                                               ImGuiTableFlags_RowBg |
                                               ImGuiTableFlags_Resizable;

        if (ImGui::BeginTable("Block Variables", 3, block_variable_flags)) {

          ImGui::TableSetupColumn("Name");
          ImGui::TableSetupColumn("Offset");
          ImGui::TableSetupColumn("Size");

          ImGui::TableHeadersRow();

          uint64_t block_variable_index = 0;
          uint64_t block_variable_count = descriptor_binding->block_variable_count;

          while (block_variable_index < block_variable_count) {

            fs_block_variable_t *block_variable = &descriptor_binding->block_variables[block_variable_index];

            ImGui::TableNextRow();

            ImGui::TableNextColumn();
            ImGui::Text(block_variable->name);

            ImGui::TableNextColumn();
            ImGui::Text("%u", block_variable->offset);

            ImGui::TableNextColumn();
            ImGui::Text("%u", block_variable->size);

            block_variable_index++;
          }

          ImGui::EndTable();
        }
      }

      break;
    }
    case FS_ASSET_TYPE_FRAMEBUFFER: {

      fs_framebuffer_t *framebuffer = (fs_framebuffer_t *)s_selected_asset->instance;

      ImGui::SeparatorText("Color Attachment");

      uint64_t attachment_index_to_destroy = -1;
      uint64_t attachment_index = 0;
      uint64_t attachment_count = framebuffer->color_attachment_count;

      while (attachment_index < attachment_count) {

        fs_asset_reference_t *color_attachment_reference = &framebuffer->color_attachment[attachment_index];

        ImGui::PushID(color_attachment_reference);

        ImGui::PushFont((ImFont *)g_im_font_symbols_18);
        if (ImGui::Button(ICON_MS_DELETE)) {

          attachment_index_to_destroy = attachment_index;
        }
        ImGui::PopFont();

        ImGui::SameLine();

        dirty |= ImGui::InputText("Color Attachment", color_attachment_reference->reference_path, TI_PATH_SIZE, ImGuiInputTextFlags_EnterReturnsTrue);

        ImGui::PopID();

        attachment_index++;
      }

      if (attachment_index_to_destroy != -1) {

        fs_asset_reference_t *src_color_attachment = &framebuffer->color_attachment[framebuffer->color_attachment_count - 1];
        fs_asset_reference_t *dst_color_attachment = &framebuffer->color_attachment[attachment_index_to_destroy];

        memcpy(dst_color_attachment, src_color_attachment, sizeof(fs_asset_reference_t));

        uint64_t old_color_attachment_count = framebuffer->color_attachment_count;
        uint64_t new_color_attachment_count = framebuffer->color_attachment_count - 1;

        fs_asset_reference_t *old_color_attachment = framebuffer->color_attachment;
        fs_asset_reference_t *new_color_attachment = (fs_asset_reference_t *)TI_ALLOC(sizeof(fs_asset_reference_t) * new_color_attachment_count, 0, 0);

        memcpy(new_color_attachment, old_color_attachment, sizeof(fs_asset_reference_t) * new_color_attachment_count);

        TI_FREE(old_color_attachment);

        framebuffer->color_attachment_count = new_color_attachment_count;
        framebuffer->color_attachment = new_color_attachment;

        dirty = true;
      }

      ImGui::PushFont((ImFont *)g_im_font_symbols_18);
      if (ImGui::Button(ICON_MS_ADD)) {

        uint64_t old_color_attachment_count = framebuffer->color_attachment_count;
        uint64_t new_color_attachment_count = framebuffer->color_attachment_count + 1;

        fs_asset_reference_t *old_color_attachment = framebuffer->color_attachment;
        fs_asset_reference_t *new_color_attachment = (fs_asset_reference_t *)TI_ALLOC(sizeof(fs_asset_reference_t) * new_color_attachment_count, 0, 0);

        memcpy(new_color_attachment, old_color_attachment, sizeof(fs_asset_reference_t) * old_color_attachment_count);

        TI_FREE(old_color_attachment);

        framebuffer->color_attachment_count = new_color_attachment_count;
        framebuffer->color_attachment = new_color_attachment;

        dirty = true;
      }
      ImGui::PopFont();

      ImGui::SeparatorText("Depth Attachment");

      dirty |= ImGui::InputText("Depth Attachment", framebuffer->depth_attachment.reference_path, TI_PATH_SIZE, ImGuiInputTextFlags_EnterReturnsTrue);

      break;
    }
    case FS_ASSET_TYPE_BUFFER: {

      fs_buffer_t *buffer = (fs_buffer_t *)s_selected_asset->instance;

      dirty |= ImGui::Checkbox("Zero Data", (bool *)&buffer->zero_data);
      dirty |= ImGui::InputScalar("Size", ImGuiDataType_U64, &buffer->size, 0, 0, "%llu", ImGuiInputTextFlags_EnterReturnsTrue);

      ImGui::SeparatorText("Buffer Usage Flags");
      dirty |= draw_buffer_usage_flags(&buffer->buffer_usage_flags);

      ImGui::SeparatorText("Memory Property Flags");
      dirty |= draw_memory_property_flags(&buffer->memory_property_flags);

      ImGui::SeparatorText("Memory Allocate Flags");
      dirty |= draw_memory_allocate_flags(&buffer->memory_allocate_flags);

      break;
    }
    case FS_ASSET_TYPE_IMAGE: {

      fs_image_t *image = (fs_image_t *)s_selected_asset->instance;

      dirty |= ImGui::InputScalar("Width", ImGuiDataType_U32, &image->width, 0, 0, "%lu", ImGuiInputTextFlags_EnterReturnsTrue);
      dirty |= ImGui::InputScalar("Height", ImGuiDataType_U32, &image->height, 0, 0, "%lu", ImGuiInputTextFlags_EnterReturnsTrue);
      dirty |= ImGui::InputScalar("Depth", ImGuiDataType_U32, &image->depth, 0, 0, "%lu", ImGuiInputTextFlags_EnterReturnsTrue);
      dirty |= ImGui::InputScalar("Mip Levels", ImGuiDataType_U32, &image->mip_levels, 0, 0, "%lu", ImGuiInputTextFlags_EnterReturnsTrue);

      dirty |= draw_vulkan_enum_dropdown("Format", &image->format_index, g_vk_format_table, TI_ARRAY_COUNT(g_vk_format_table));
      dirty |= draw_vulkan_enum_dropdown("Image Layout", &image->image_layout_index, g_vk_image_layout_table, TI_ARRAY_COUNT(g_vk_image_layout_table));
      dirty |= draw_vulkan_enum_dropdown("Image Type", &image->image_type_index, g_vk_image_type_table, TI_ARRAY_COUNT(g_vk_image_type_table));
      dirty |= draw_vulkan_enum_dropdown("Image Tiling", &image->image_tiling_index, g_vk_image_tiling_table, TI_ARRAY_COUNT(g_vk_image_tiling_table));
      dirty |= draw_vulkan_enum_dropdown("Image View Type", &image->image_view_type_index, g_vk_image_view_type_table, TI_ARRAY_COUNT(g_vk_image_view_type_table));

      ImGui::SeparatorText("Image Usage Flags");
      dirty |= draw_image_usage_flags(&image->image_usage_flags);

      ImGui::SeparatorText("Image Aspect Flags");
      dirty |= draw_image_aspect_flags(&image->image_aspect_flags);

      ImGui::SeparatorText("Memory Property Flags");
      dirty |= draw_memory_property_flags(&image->memory_property_flags);

      ImGui::SeparatorText("Memory Allocate Flags");
      dirty |= draw_memory_allocate_flags(&image->memory_allocate_flags);

      break;
    }
    case FS_ASSET_TYPE_SWAPCHAIN: {

      fs_swapchain_t *swapchain = (fs_swapchain_t *)s_selected_asset->instance;

      dirty |= ImGui::InputScalar("Image Count", ImGuiDataType_U32, &swapchain->image_count, 0, 0, "%lu", ImGuiInputTextFlags_EnterReturnsTrue);

      break;
    }
    case FS_ASSET_TYPE_RENDERPASS: {

      fs_renderpass_t *renderpass = (fs_renderpass_t *)s_selected_asset->instance;

      dirty |= draw_vulkan_enum_dropdown("Initial Color Attachment Layout", &renderpass->initial_color_attachment_layout_index, g_vk_image_layout_table, TI_ARRAY_COUNT(g_vk_image_layout_table));
      dirty |= draw_vulkan_enum_dropdown("Initial Depth Attachment Layout", &renderpass->initial_depth_attachment_layout_index, g_vk_image_layout_table, TI_ARRAY_COUNT(g_vk_image_layout_table));
      dirty |= draw_vulkan_enum_dropdown("Final Color Attachment Layout", &renderpass->final_color_attachment_layout_index, g_vk_image_layout_table, TI_ARRAY_COUNT(g_vk_image_layout_table));
      dirty |= draw_vulkan_enum_dropdown("Final Depth Attachment Layout", &renderpass->final_depth_attachment_layout_index, g_vk_image_layout_table, TI_ARRAY_COUNT(g_vk_image_layout_table));

      break;
    }
    case FS_ASSET_TYPE_RENDERER: {

      fs_renderer_t *renderer = (fs_renderer_t *)s_selected_asset->instance;

      dirty |= ImGui::InputText("Debug Line Vertex Buffer", renderer->debug_line_vertex_buffer.reference_path, TI_PATH_SIZE, ImGuiInputTextFlags_EnterReturnsTrue);
      dirty |= ImGui::InputText("Debug Line Index Buffer", renderer->debug_line_index_buffer.reference_path, TI_PATH_SIZE, ImGuiInputTextFlags_EnterReturnsTrue);
      dirty |= ImGui::InputText("Full Screen Vertex Buffer", renderer->full_screen_vertex_buffer.reference_path, TI_PATH_SIZE, ImGuiInputTextFlags_EnterReturnsTrue);
      dirty |= ImGui::InputText("Full Screen Index Buffer", renderer->full_screen_index_buffer.reference_path, TI_PATH_SIZE, ImGuiInputTextFlags_EnterReturnsTrue);

      break;
    }
    case FS_ASSET_TYPE_SCRIPT: {

      fs_script_t *script = (fs_script_t *)s_selected_asset->instance;

      // TODO

      break;
    }
    case FS_ASSET_TYPE_SOUND: {

      fs_sound_t *sound = (fs_sound_t *)s_selected_asset->instance;

      // TODO

      break;
    }
  }

  if (dirty) {
    fs_asset_store(s_selected_asset);
  }
}
static void draw_entity_controls(void) {
  if (ImGui::BeginCombo("##Component", s_component_name[s_selected_comp])) {

    uint64_t comp_index = 0;
    uint64_t comp_count = TI_ARRAY_COUNT(s_component_name);

    while (comp_index < comp_count) {

      bool selected = (comp_index == s_selected_comp);

      if (ImGui::Selectable(s_component_name[comp_index], selected)) {
        s_selected_comp = (cp_component_type_t)comp_index;
      }

      if (selected) {
        ImGui::SetItemDefaultFocus();
      }

      comp_index++;
    }

    ImGui::EndCombo();
  }

  ImGui::SameLine();

  ImGui::PushFont((ImFont *)g_im_font_symbols_18);
  if (ImGui::Button(ICON_MS_ADD)) {

    switch (s_selected_comp) {

      case CP_COMPONENT_TYPE_TRANSFORM: {

        ecs_add(g_scene.world, s_selected_entity, cp_transform_t);

        cp_transform_t *transform = ecs_get_mut(g_scene.world, s_selected_entity, cp_transform_t);

        cp_transform_init(transform);

        break;
      }
      case CP_COMPONENT_TYPE_CAMERA: {

        ecs_add(g_scene.world, s_selected_entity, cp_camera_t);

        cp_camera_t *camera = ecs_get_mut(g_scene.world, s_selected_entity, cp_camera_t);

        cp_camera_init(camera);

        break;
      }
      case CP_COMPONENT_TYPE_MATERIAL: {

        ecs_add(g_scene.world, s_selected_entity, cp_material_t);

        cp_material_t *material = ecs_get_mut(g_scene.world, s_selected_entity, cp_material_t);

        cp_material_init(material);

        break;
      }
      case CP_COMPONENT_TYPE_MESH: {

        ecs_add(g_scene.world, s_selected_entity, cp_mesh_t);

        cp_mesh_t *mesh = ecs_get_mut(g_scene.world, s_selected_entity, cp_mesh_t);

        cp_mesh_init(mesh);

        break;
      }
      case CP_COMPONENT_TYPE_SKELETON: {

        ecs_add(g_scene.world, s_selected_entity, cp_skeleton_t);

        cp_skeleton_t *skeleton = ecs_get_mut(g_scene.world, s_selected_entity, cp_skeleton_t);

        cp_skeleton_init(skeleton);

        break;
      }
    }
  }
  ImGui::PopFont();

  ImGui::SameLine();

  ImGui::PushFont((ImFont *)g_im_font_symbols_18);
  if (ImGui::Button(ICON_MS_REMOVE)) {

    switch (s_selected_comp) {

      case CP_COMPONENT_TYPE_TRANSFORM: {

        ecs_remove(g_scene.world, s_selected_entity, cp_transform_t);

        break;
      }
      case CP_COMPONENT_TYPE_CAMERA: {

        ecs_remove(g_scene.world, s_selected_entity, cp_camera_t);

        break;
      }
      case CP_COMPONENT_TYPE_MATERIAL: {

        ecs_remove(g_scene.world, s_selected_entity, cp_material_t);

        break;
      }
      case CP_COMPONENT_TYPE_MESH: {

        ecs_remove(g_scene.world, s_selected_entity, cp_mesh_t);

        break;
      }
      case CP_COMPONENT_TYPE_SKELETON: {

        ecs_remove(g_scene.world, s_selected_entity, cp_skeleton_t);

        break;
      }
    }
  }
  ImGui::PopFont();
}
static void draw_entity(void) {
  bool dirty = false;

  ImGuiTreeNodeFlags tree_node_flags = ImGuiTreeNodeFlags_OpenOnArrow |
                                       ImGuiTreeNodeFlags_SpanFullWidth |
                                       ImGuiTreeNodeFlags_FramePadding |
                                       ImGuiTreeNodeFlags_Framed;

  cp_transform_t *transform = ecs_get_mut(g_scene.world, s_selected_entity, cp_transform_t);
  cp_camera_t *camera = ecs_get_mut(g_scene.world, s_selected_entity, cp_camera_t);
  cp_material_t *material = ecs_get_mut(g_scene.world, s_selected_entity, cp_material_t);
  cp_mesh_t *mesh = ecs_get_mut(g_scene.world, s_selected_entity, cp_mesh_t);
  cp_skeleton_t *skeleton = ecs_get_mut(g_scene.world, s_selected_entity, cp_skeleton_t);
  cp_script_t *script = ecs_get_mut(g_scene.world, s_selected_entity, cp_script_t);

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

      ImGui::Text("Pipeline Handle %lu", material->pipeline);
      ImGui::Text("Material Handle %lu", material->material);

      ImGui::TreePop();
    }
  }

  if (mesh) {

    if (ImGui::TreeNodeEx("Mesh", tree_node_flags)) {

      ImGui::Text("Mesh Handle %lu", mesh->mesh);

      ImGui::TreePop();
    }
  }

  if (skeleton) {

    if (ImGui::TreeNodeEx("Skeleton", tree_node_flags)) {

      ImGui::Text("Skeleton Handle %lu", skeleton->skeleton);

      ImGui::TreePop();
    }
  }

  if (script) {

    if (ImGui::TreeNodeEx("Script", tree_node_flags)) {

      ImGui::Text("Script Handle %lu", script->script);

      ImGui::TreePop();
    }
  }

  if (dirty) {
    // TODO
  }
}

static bool draw_buffer_usage_flags(VkBufferUsageFlags *flags) {
  bool dirty = false;

  dirty |= ImGui::CheckboxFlags("VK_BUFFER_USAGE_TRANSFER_SRC_BIT", flags, VK_BUFFER_USAGE_TRANSFER_SRC_BIT);
  dirty |= ImGui::CheckboxFlags("VK_BUFFER_USAGE_TRANSFER_DST_BIT", flags, VK_BUFFER_USAGE_TRANSFER_DST_BIT);
  dirty |= ImGui::CheckboxFlags("VK_BUFFER_USAGE_UNIFORM_TEXEL_BUFFER_BIT", flags, VK_BUFFER_USAGE_UNIFORM_TEXEL_BUFFER_BIT);
  dirty |= ImGui::CheckboxFlags("VK_BUFFER_USAGE_STORAGE_TEXEL_BUFFER_BIT", flags, VK_BUFFER_USAGE_STORAGE_TEXEL_BUFFER_BIT);
  dirty |= ImGui::CheckboxFlags("VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT", flags, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);
  dirty |= ImGui::CheckboxFlags("VK_BUFFER_USAGE_STORAGE_BUFFER_BIT", flags, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT);
  dirty |= ImGui::CheckboxFlags("VK_BUFFER_USAGE_INDEX_BUFFER_BIT", flags, VK_BUFFER_USAGE_INDEX_BUFFER_BIT);
  dirty |= ImGui::CheckboxFlags("VK_BUFFER_USAGE_VERTEX_BUFFER_BIT", flags, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);
  dirty |= ImGui::CheckboxFlags("VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT", flags, VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT);
  dirty |= ImGui::CheckboxFlags("VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT", flags, VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT);
  dirty |= ImGui::CheckboxFlags("VK_BUFFER_USAGE_VIDEO_DECODE_SRC_BIT_KHR", flags, VK_BUFFER_USAGE_VIDEO_DECODE_SRC_BIT_KHR);
  dirty |= ImGui::CheckboxFlags("VK_BUFFER_USAGE_VIDEO_DECODE_DST_BIT_KHR", flags, VK_BUFFER_USAGE_VIDEO_DECODE_DST_BIT_KHR);
  dirty |= ImGui::CheckboxFlags("VK_BUFFER_USAGE_TRANSFORM_FEEDBACK_BUFFER_BIT_EXT", flags, VK_BUFFER_USAGE_TRANSFORM_FEEDBACK_BUFFER_BIT_EXT);
  dirty |= ImGui::CheckboxFlags("VK_BUFFER_USAGE_TRANSFORM_FEEDBACK_COUNTER_BUFFER_BIT_EXT", flags, VK_BUFFER_USAGE_TRANSFORM_FEEDBACK_COUNTER_BUFFER_BIT_EXT);
  dirty |= ImGui::CheckboxFlags("VK_BUFFER_USAGE_CONDITIONAL_RENDERING_BIT_EXT", flags, VK_BUFFER_USAGE_CONDITIONAL_RENDERING_BIT_EXT);
  dirty |= ImGui::CheckboxFlags("VK_BUFFER_USAGE_DESCRIPTOR_HEAP_BIT_EXT", flags, VK_BUFFER_USAGE_DESCRIPTOR_HEAP_BIT_EXT);
  dirty |= ImGui::CheckboxFlags("VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR", flags, VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR);
  dirty |= ImGui::CheckboxFlags("VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_STORAGE_BIT_KHR", flags, VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_STORAGE_BIT_KHR);
  dirty |= ImGui::CheckboxFlags("VK_BUFFER_USAGE_SHADER_BINDING_TABLE_BIT_KHR", flags, VK_BUFFER_USAGE_SHADER_BINDING_TABLE_BIT_KHR);
  dirty |= ImGui::CheckboxFlags("VK_BUFFER_USAGE_VIDEO_ENCODE_DST_BIT_KHR", flags, VK_BUFFER_USAGE_VIDEO_ENCODE_DST_BIT_KHR);
  dirty |= ImGui::CheckboxFlags("VK_BUFFER_USAGE_VIDEO_ENCODE_SRC_BIT_KHR", flags, VK_BUFFER_USAGE_VIDEO_ENCODE_SRC_BIT_KHR);
  dirty |= ImGui::CheckboxFlags("VK_BUFFER_USAGE_SAMPLER_DESCRIPTOR_BUFFER_BIT_EXT", flags, VK_BUFFER_USAGE_SAMPLER_DESCRIPTOR_BUFFER_BIT_EXT);
  dirty |= ImGui::CheckboxFlags("VK_BUFFER_USAGE_RESOURCE_DESCRIPTOR_BUFFER_BIT_EXT", flags, VK_BUFFER_USAGE_RESOURCE_DESCRIPTOR_BUFFER_BIT_EXT);
  dirty |= ImGui::CheckboxFlags("VK_BUFFER_USAGE_PUSH_DESCRIPTORS_DESCRIPTOR_BUFFER_BIT_EXT", flags, VK_BUFFER_USAGE_PUSH_DESCRIPTORS_DESCRIPTOR_BUFFER_BIT_EXT);
  dirty |= ImGui::CheckboxFlags("VK_BUFFER_USAGE_MICROMAP_BUILD_INPUT_READ_ONLY_BIT_EXT", flags, VK_BUFFER_USAGE_MICROMAP_BUILD_INPUT_READ_ONLY_BIT_EXT);
  dirty |= ImGui::CheckboxFlags("VK_BUFFER_USAGE_MICROMAP_STORAGE_BIT_EXT", flags, VK_BUFFER_USAGE_MICROMAP_STORAGE_BIT_EXT);
  dirty |= ImGui::CheckboxFlags("VK_BUFFER_USAGE_TILE_MEMORY_BIT_QCOM", flags, VK_BUFFER_USAGE_TILE_MEMORY_BIT_QCOM);

  return dirty;
}
static bool draw_memory_property_flags(VkMemoryPropertyFlags *flags) {
  bool dirty = false;

  dirty |= ImGui::CheckboxFlags("VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT", flags, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
  dirty |= ImGui::CheckboxFlags("VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT", flags, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
  dirty |= ImGui::CheckboxFlags("VK_MEMORY_PROPERTY_HOST_COHERENT_BIT", flags, VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
  dirty |= ImGui::CheckboxFlags("VK_MEMORY_PROPERTY_HOST_CACHED_BIT", flags, VK_MEMORY_PROPERTY_HOST_CACHED_BIT);
  dirty |= ImGui::CheckboxFlags("VK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT", flags, VK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT);
  dirty |= ImGui::CheckboxFlags("VK_MEMORY_PROPERTY_PROTECTED_BIT", flags, VK_MEMORY_PROPERTY_PROTECTED_BIT);
  dirty |= ImGui::CheckboxFlags("VK_MEMORY_PROPERTY_DEVICE_COHERENT_BIT_AMD", flags, VK_MEMORY_PROPERTY_DEVICE_COHERENT_BIT_AMD);
  dirty |= ImGui::CheckboxFlags("VK_MEMORY_PROPERTY_DEVICE_UNCACHED_BIT_AMD", flags, VK_MEMORY_PROPERTY_DEVICE_UNCACHED_BIT_AMD);
  dirty |= ImGui::CheckboxFlags("VK_MEMORY_PROPERTY_RDMA_CAPABLE_BIT_NV", flags, VK_MEMORY_PROPERTY_RDMA_CAPABLE_BIT_NV);

  return dirty;
}
static bool draw_memory_allocate_flags(VkMemoryAllocateFlags *flags) {
  bool dirty = false;

  dirty |= ImGui::CheckboxFlags("VK_MEMORY_ALLOCATE_DEVICE_MASK_BIT", flags, VK_MEMORY_ALLOCATE_DEVICE_MASK_BIT);
  dirty |= ImGui::CheckboxFlags("VK_MEMORY_ALLOCATE_DEVICE_ADDRESS_BIT", flags, VK_MEMORY_ALLOCATE_DEVICE_ADDRESS_BIT);
  dirty |= ImGui::CheckboxFlags("VK_MEMORY_ALLOCATE_DEVICE_ADDRESS_CAPTURE_REPLAY_BIT", flags, VK_MEMORY_ALLOCATE_DEVICE_ADDRESS_CAPTURE_REPLAY_BIT);
  dirty |= ImGui::CheckboxFlags("VK_MEMORY_ALLOCATE_ZERO_INITIALIZE_BIT_EXT", flags, VK_MEMORY_ALLOCATE_ZERO_INITIALIZE_BIT_EXT);

  return dirty;
}
static bool draw_image_usage_flags(VkImageUsageFlags *flags) {
  bool dirty = false;

  dirty |= ImGui::CheckboxFlags("VK_IMAGE_USAGE_TRANSFER_SRC_BIT", flags, VK_IMAGE_USAGE_TRANSFER_SRC_BIT);
  dirty |= ImGui::CheckboxFlags("VK_IMAGE_USAGE_TRANSFER_DST_BIT", flags, VK_IMAGE_USAGE_TRANSFER_DST_BIT);
  dirty |= ImGui::CheckboxFlags("VK_IMAGE_USAGE_SAMPLED_BIT", flags, VK_IMAGE_USAGE_SAMPLED_BIT);
  dirty |= ImGui::CheckboxFlags("VK_IMAGE_USAGE_STORAGE_BIT", flags, VK_IMAGE_USAGE_STORAGE_BIT);
  dirty |= ImGui::CheckboxFlags("VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT", flags, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT);
  dirty |= ImGui::CheckboxFlags("VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT", flags, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT);
  dirty |= ImGui::CheckboxFlags("VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT", flags, VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT);
  dirty |= ImGui::CheckboxFlags("VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT", flags, VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT);
  dirty |= ImGui::CheckboxFlags("VK_IMAGE_USAGE_HOST_TRANSFER_BIT", flags, VK_IMAGE_USAGE_HOST_TRANSFER_BIT);
  dirty |= ImGui::CheckboxFlags("VK_IMAGE_USAGE_VIDEO_DECODE_DST_BIT_KHR", flags, VK_IMAGE_USAGE_VIDEO_DECODE_DST_BIT_KHR);
  dirty |= ImGui::CheckboxFlags("VK_IMAGE_USAGE_VIDEO_DECODE_SRC_BIT_KHR", flags, VK_IMAGE_USAGE_VIDEO_DECODE_SRC_BIT_KHR);
  dirty |= ImGui::CheckboxFlags("VK_IMAGE_USAGE_VIDEO_DECODE_DPB_BIT_KHR", flags, VK_IMAGE_USAGE_VIDEO_DECODE_DPB_BIT_KHR);
  dirty |= ImGui::CheckboxFlags("VK_IMAGE_USAGE_FRAGMENT_DENSITY_MAP_BIT_EXT", flags, VK_IMAGE_USAGE_FRAGMENT_DENSITY_MAP_BIT_EXT);
  dirty |= ImGui::CheckboxFlags("VK_IMAGE_USAGE_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR", flags, VK_IMAGE_USAGE_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR);
  dirty |= ImGui::CheckboxFlags("VK_IMAGE_USAGE_VIDEO_ENCODE_DST_BIT_KHR", flags, VK_IMAGE_USAGE_VIDEO_ENCODE_DST_BIT_KHR);
  dirty |= ImGui::CheckboxFlags("VK_IMAGE_USAGE_VIDEO_ENCODE_SRC_BIT_KHR", flags, VK_IMAGE_USAGE_VIDEO_ENCODE_SRC_BIT_KHR);
  dirty |= ImGui::CheckboxFlags("VK_IMAGE_USAGE_VIDEO_ENCODE_DPB_BIT_KHR", flags, VK_IMAGE_USAGE_VIDEO_ENCODE_DPB_BIT_KHR);
  dirty |= ImGui::CheckboxFlags("VK_IMAGE_USAGE_ATTACHMENT_FEEDBACK_LOOP_BIT_EXT", flags, VK_IMAGE_USAGE_ATTACHMENT_FEEDBACK_LOOP_BIT_EXT);
  dirty |= ImGui::CheckboxFlags("VK_IMAGE_USAGE_INVOCATION_MASK_BIT_HUAWEI", flags, VK_IMAGE_USAGE_INVOCATION_MASK_BIT_HUAWEI);
  dirty |= ImGui::CheckboxFlags("VK_IMAGE_USAGE_SAMPLE_WEIGHT_BIT_QCOM", flags, VK_IMAGE_USAGE_SAMPLE_WEIGHT_BIT_QCOM);
  dirty |= ImGui::CheckboxFlags("VK_IMAGE_USAGE_SAMPLE_BLOCK_MATCH_BIT_QCOM", flags, VK_IMAGE_USAGE_SAMPLE_BLOCK_MATCH_BIT_QCOM);
  dirty |= ImGui::CheckboxFlags("VK_IMAGE_USAGE_TENSOR_ALIASING_BIT_ARM", flags, VK_IMAGE_USAGE_TENSOR_ALIASING_BIT_ARM);
  dirty |= ImGui::CheckboxFlags("VK_IMAGE_USAGE_TILE_MEMORY_BIT_QCOM", flags, VK_IMAGE_USAGE_TILE_MEMORY_BIT_QCOM);
  dirty |= ImGui::CheckboxFlags("VK_IMAGE_USAGE_VIDEO_ENCODE_QUANTIZATION_DELTA_MAP_BIT_KHR", flags, VK_IMAGE_USAGE_VIDEO_ENCODE_QUANTIZATION_DELTA_MAP_BIT_KHR);
  dirty |= ImGui::CheckboxFlags("VK_IMAGE_USAGE_VIDEO_ENCODE_EMPHASIS_MAP_BIT_KHR", flags, VK_IMAGE_USAGE_VIDEO_ENCODE_EMPHASIS_MAP_BIT_KHR);

  return dirty;
}
static bool draw_image_aspect_flags(VkImageAspectFlags *flags) {
  bool dirty = false;

  dirty |= ImGui::CheckboxFlags("VK_IMAGE_ASPECT_COLOR_BIT", flags, VK_IMAGE_ASPECT_COLOR_BIT);
  dirty |= ImGui::CheckboxFlags("VK_IMAGE_ASPECT_DEPTH_BIT", flags, VK_IMAGE_ASPECT_DEPTH_BIT);
  dirty |= ImGui::CheckboxFlags("VK_IMAGE_ASPECT_STENCIL_BIT", flags, VK_IMAGE_ASPECT_STENCIL_BIT);
  dirty |= ImGui::CheckboxFlags("VK_IMAGE_ASPECT_METADATA_BIT", flags, VK_IMAGE_ASPECT_METADATA_BIT);
  dirty |= ImGui::CheckboxFlags("VK_IMAGE_ASPECT_PLANE_0_BIT", flags, VK_IMAGE_ASPECT_PLANE_0_BIT);
  dirty |= ImGui::CheckboxFlags("VK_IMAGE_ASPECT_PLANE_1_BIT", flags, VK_IMAGE_ASPECT_PLANE_1_BIT);
  dirty |= ImGui::CheckboxFlags("VK_IMAGE_ASPECT_PLANE_2_BIT", flags, VK_IMAGE_ASPECT_PLANE_2_BIT);
  dirty |= ImGui::CheckboxFlags("VK_IMAGE_ASPECT_MEMORY_PLANE_0_BIT_EXT", flags, VK_IMAGE_ASPECT_MEMORY_PLANE_0_BIT_EXT);
  dirty |= ImGui::CheckboxFlags("VK_IMAGE_ASPECT_MEMORY_PLANE_1_BIT_EXT", flags, VK_IMAGE_ASPECT_MEMORY_PLANE_1_BIT_EXT);
  dirty |= ImGui::CheckboxFlags("VK_IMAGE_ASPECT_MEMORY_PLANE_2_BIT_EXT", flags, VK_IMAGE_ASPECT_MEMORY_PLANE_2_BIT_EXT);
  dirty |= ImGui::CheckboxFlags("VK_IMAGE_ASPECT_MEMORY_PLANE_3_BIT_EXT", flags, VK_IMAGE_ASPECT_MEMORY_PLANE_3_BIT_EXT);

  return dirty;
}
static bool draw_cull_mode_flags(VkCullModeFlags *flags) {
  bool dirty = false;

  dirty |= ImGui::CheckboxFlags("VK_CULL_MODE_FRONT_BIT", flags, VK_CULL_MODE_FRONT_BIT);
  dirty |= ImGui::CheckboxFlags("VK_CULL_MODE_BACK_BIT", flags, VK_CULL_MODE_BACK_BIT);
  dirty |= ImGui::CheckboxFlags("VK_CULL_MODE_FRONT_AND_BACK", flags, VK_CULL_MODE_FRONT_AND_BACK);

  return dirty;
}
static bool draw_shader_stage_flags(VkShaderStageFlags *flags) {
  bool dirty = false;

  dirty |= ImGui::CheckboxFlags("VK_SHADER_STAGE_VERTEX_BIT", flags, VK_SHADER_STAGE_VERTEX_BIT);
  dirty |= ImGui::CheckboxFlags("VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT", flags, VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT);
  dirty |= ImGui::CheckboxFlags("VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT", flags, VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT);
  dirty |= ImGui::CheckboxFlags("VK_SHADER_STAGE_GEOMETRY_BIT", flags, VK_SHADER_STAGE_GEOMETRY_BIT);
  dirty |= ImGui::CheckboxFlags("VK_SHADER_STAGE_FRAGMENT_BIT", flags, VK_SHADER_STAGE_FRAGMENT_BIT);
  dirty |= ImGui::CheckboxFlags("VK_SHADER_STAGE_COMPUTE_BIT", flags, VK_SHADER_STAGE_COMPUTE_BIT);
  dirty |= ImGui::CheckboxFlags("VK_SHADER_STAGE_ALL_GRAPHICS", flags, VK_SHADER_STAGE_ALL_GRAPHICS);
  dirty |= ImGui::CheckboxFlags("VK_SHADER_STAGE_ALL", flags, VK_SHADER_STAGE_ALL);
  dirty |= ImGui::CheckboxFlags("VK_SHADER_STAGE_RAYGEN_BIT_KHR", flags, VK_SHADER_STAGE_RAYGEN_BIT_KHR);
  dirty |= ImGui::CheckboxFlags("VK_SHADER_STAGE_ANY_HIT_BIT_KHR", flags, VK_SHADER_STAGE_ANY_HIT_BIT_KHR);
  dirty |= ImGui::CheckboxFlags("VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR", flags, VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR);
  dirty |= ImGui::CheckboxFlags("VK_SHADER_STAGE_MISS_BIT_KHR", flags, VK_SHADER_STAGE_MISS_BIT_KHR);
  dirty |= ImGui::CheckboxFlags("VK_SHADER_STAGE_INTERSECTION_BIT_KHR", flags, VK_SHADER_STAGE_INTERSECTION_BIT_KHR);
  dirty |= ImGui::CheckboxFlags("VK_SHADER_STAGE_CALLABLE_BIT_KHR", flags, VK_SHADER_STAGE_CALLABLE_BIT_KHR);
  dirty |= ImGui::CheckboxFlags("VK_SHADER_STAGE_TASK_BIT_EXT", flags, VK_SHADER_STAGE_TASK_BIT_EXT);
  dirty |= ImGui::CheckboxFlags("VK_SHADER_STAGE_MESH_BIT_EXT", flags, VK_SHADER_STAGE_MESH_BIT_EXT);
  dirty |= ImGui::CheckboxFlags("VK_SHADER_STAGE_SUBPASS_SHADING_BIT_HUAWEI", flags, VK_SHADER_STAGE_SUBPASS_SHADING_BIT_HUAWEI);
  dirty |= ImGui::CheckboxFlags("VK_SHADER_STAGE_CLUSTER_CULLING_BIT_HUAWEI", flags, VK_SHADER_STAGE_CLUSTER_CULLING_BIT_HUAWEI);

  return dirty;
}
static bool draw_vulkan_enum_dropdown(char const *label, uint64_t *selected_index, vk_enum_record_t *table, uint64_t table_count) {
  bool dirty = false;

  if (ImGui::BeginCombo(label, table[*selected_index].name)) {

    uint64_t index = 0;
    uint64_t count = table_count;

    while (index < count) {

      bool selected = (index == *selected_index);

      if (ImGui::Selectable(table[index].name, selected)) {

        if (*selected_index != index) {

          *selected_index = index;

          dirty = true;
        }
      }

      if (selected) {
        ImGui::SetItemDefaultFocus();
      }

      index++;
    }

    ImGui::EndCombo();
  }

  return dirty;
}
