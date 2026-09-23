#include <ti_pch.h>

#include <imgui.h>

static void draw_background(void);
static void draw_asset_controls(void);
static void draw_asset(void);
static void draw_asset_joint(fs_asset_t *asset, fs_joint_t *joint);
static void draw_asset_descriptor_binding(fs_asset_t *asset, fs_descriptor_binding_t *descriptor_binding);
static void resize_model_meshes(fs_model_t *model, uint64_t count);
static void resize_model_skins(fs_model_t *model, uint64_t count);
static void resize_mesh_primitives(fs_mesh_t *mesh, uint64_t count);
static void resize_joint_children(fs_joint_t *joint, uint64_t count);
static void resize_binding_variables(fs_descriptor_binding_t *binding, int32_t count);
static void resize_buffer(void **buffer, uint64_t old_size, uint64_t new_size);
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

static void draw_asset(void) {
  fs_asset_t *asset = (fs_asset_t *)s_selected_data;

  if ((asset == 0) || (asset->config == 0)) {
    return;
  }

  ImGui::Text("Magic: 0x%016llX", asset->magic);
  ImGui::Text("Type: %d", asset->type);
  ImGui::Text("Path: %s", asset->path);
  ImGui::Text("Config: %p", asset->config);

  ImGuiTreeNodeFlags tree_node_flags = ImGuiTreeNodeFlags_OpenOnArrow |
                                       ImGuiTreeNodeFlags_SpanFullWidth |
                                       ImGuiTreeNodeFlags_FramePadding;

  ImGui::PushID(asset);
  ImGui::PushItemWidth(-FLT_MIN);

  switch (asset->type) {

    case FS_ASSET_TYPE_MODEL: {

      fs_model_t *model = (fs_model_t *)asset->config;

      ImGui::TextUnformatted("Name:");
      ImGui::SameLine();

      if (ImGui::InputText("##Name", model->name, TI_PATH_SIZE, ImGuiInputTextFlags_EnterReturnsTrue)) {
        fs_asset_store(asset);
      }

      uint64_t new_mesh_count = model->mesh_count;

      ImGui::TextUnformatted("Mesh Count:");
      ImGui::SameLine();

      if (ImGui::InputScalar("##Mesh Count", ImGuiDataType_U64, &new_mesh_count, 0, 0, "%llu", ImGuiInputTextFlags_EnterReturnsTrue)) {
        resize_model_meshes(model, new_mesh_count);
        fs_asset_store(asset);
      }

      ImGui::Text("Meshes: %p", model->meshes);

      uint64_t new_skin_count = model->skin_count;

      ImGui::TextUnformatted("Skin Count:");
      ImGui::SameLine();

      if (ImGui::InputScalar("##Skin Count", ImGuiDataType_U64, &new_skin_count, 0, 0, "%llu", ImGuiInputTextFlags_EnterReturnsTrue)) {
        resize_model_skins(model, new_skin_count);
        fs_asset_store(asset);
      }

      ImGui::Text("Skins: %p", model->skins);

      uint64_t mesh_index = 0;

      while (mesh_index < model->mesh_count) {

        fs_mesh_t *mesh = &model->meshes[mesh_index];

        if (ImGui::TreeNodeEx(mesh, tree_node_flags, "Mesh %llu: %s", mesh_index, mesh->name)) {

          ImGui::TextUnformatted("Name:");
          ImGui::SameLine();

          if (ImGui::InputText("##Name", mesh->name, TI_PATH_SIZE, ImGuiInputTextFlags_EnterReturnsTrue)) {
            fs_asset_store(asset);
          }

          uint64_t new_primitive_count = mesh->primitive_count;

          ImGui::TextUnformatted("Primitive Count:");
          ImGui::SameLine();

          if (ImGui::InputScalar("##Primitive Count", ImGuiDataType_U64, &new_primitive_count, 0, 0, "%llu", ImGuiInputTextFlags_EnterReturnsTrue)) {
            resize_mesh_primitives(mesh, new_primitive_count);
            fs_asset_store(asset);
          }

          ImGui::Text("Primitives: %p", mesh->primitives);

          uint64_t primitive_index = 0;

          while (primitive_index < mesh->primitive_count) {

            fs_primitive_t *primitive = &mesh->primitives[primitive_index];

            if (ImGui::TreeNodeEx(primitive, tree_node_flags, "Primitive %llu: %s", primitive_index, primitive->name)) {

              ImGui::TextUnformatted("Name:");
              ImGui::SameLine();

              if (ImGui::InputText("##Name", primitive->name, TI_PATH_SIZE, ImGuiInputTextFlags_EnterReturnsTrue)) {
                fs_asset_store(asset);
              }

              uint64_t new_position_count = primitive->position_count;

              ImGui::TextUnformatted("Position Count:");
              ImGui::SameLine();

              if (ImGui::InputScalar("##Position Count", ImGuiDataType_U64, &new_position_count, 0, 0, "%llu", ImGuiInputTextFlags_EnterReturnsTrue)) {
                resize_buffer(&primitive->positions, primitive->position_count * primitive->position_stride, new_position_count * primitive->position_stride);
                primitive->position_count = new_position_count;
                fs_asset_store(asset);
              }

              uint64_t new_position_stride = primitive->position_stride;

              ImGui::TextUnformatted("Position Stride:");
              ImGui::SameLine();

              if (ImGui::InputScalar("##Position Stride", ImGuiDataType_U64, &new_position_stride, 0, 0, "%llu", ImGuiInputTextFlags_EnterReturnsTrue)) {
                resize_buffer(&primitive->positions, primitive->position_count * primitive->position_stride, primitive->position_count * new_position_stride);
                primitive->position_stride = new_position_stride;
                fs_asset_store(asset);
              }

              ImGui::Text("Positions: %p", primitive->positions);

              uint64_t new_normal_count = primitive->normal_count;

              ImGui::TextUnformatted("Normal Count:");
              ImGui::SameLine();

              if (ImGui::InputScalar("##Normal Count", ImGuiDataType_U64, &new_normal_count, 0, 0, "%llu", ImGuiInputTextFlags_EnterReturnsTrue)) {
                resize_buffer(&primitive->normals, primitive->normal_count * primitive->normal_stride, new_normal_count * primitive->normal_stride);
                primitive->normal_count = new_normal_count;
                fs_asset_store(asset);
              }

              uint64_t new_normal_stride = primitive->normal_stride;

              ImGui::TextUnformatted("Normal Stride:");
              ImGui::SameLine();

              if (ImGui::InputScalar("##Normal Stride", ImGuiDataType_U64, &new_normal_stride, 0, 0, "%llu", ImGuiInputTextFlags_EnterReturnsTrue)) {
                resize_buffer(&primitive->normals, primitive->normal_count * primitive->normal_stride, primitive->normal_count * new_normal_stride);
                primitive->normal_stride = new_normal_stride;
                fs_asset_store(asset);
              }

              ImGui::Text("Normals: %p", primitive->normals);

              uint64_t new_tangent_count = primitive->tangent_count;

              ImGui::TextUnformatted("Tangent Count:");
              ImGui::SameLine();

              if (ImGui::InputScalar("##Tangent Count", ImGuiDataType_U64, &new_tangent_count, 0, 0, "%llu", ImGuiInputTextFlags_EnterReturnsTrue)) {
                resize_buffer(&primitive->tangents, primitive->tangent_count * primitive->tangent_stride, new_tangent_count * primitive->tangent_stride);
                primitive->tangent_count = new_tangent_count;
                fs_asset_store(asset);
              }

              uint64_t new_tangent_stride = primitive->tangent_stride;

              ImGui::TextUnformatted("Tangent Stride:");
              ImGui::SameLine();

              if (ImGui::InputScalar("##Tangent Stride", ImGuiDataType_U64, &new_tangent_stride, 0, 0, "%llu", ImGuiInputTextFlags_EnterReturnsTrue)) {
                resize_buffer(&primitive->tangents, primitive->tangent_count * primitive->tangent_stride, primitive->tangent_count * new_tangent_stride);
                primitive->tangent_stride = new_tangent_stride;
                fs_asset_store(asset);
              }

              ImGui::Text("Tangents: %p", primitive->tangents);

              uint64_t new_texcoord_count = primitive->texcoord_count;

              ImGui::TextUnformatted("Texcoord Count:");
              ImGui::SameLine();

              if (ImGui::InputScalar("##Texcoord Count", ImGuiDataType_U64, &new_texcoord_count, 0, 0, "%llu", ImGuiInputTextFlags_EnterReturnsTrue)) {
                resize_buffer(&primitive->texcoords, primitive->texcoord_count * primitive->texcoord_stride, new_texcoord_count * primitive->texcoord_stride);
                primitive->texcoord_count = new_texcoord_count;
                fs_asset_store(asset);
              }

              uint64_t new_texcoord_stride = primitive->texcoord_stride;

              ImGui::TextUnformatted("Texcoord Stride:");
              ImGui::SameLine();

              if (ImGui::InputScalar("##Texcoord Stride", ImGuiDataType_U64, &new_texcoord_stride, 0, 0, "%llu", ImGuiInputTextFlags_EnterReturnsTrue)) {
                resize_buffer(&primitive->texcoords, primitive->texcoord_count * primitive->texcoord_stride, primitive->texcoord_count * new_texcoord_stride);
                primitive->texcoord_stride = new_texcoord_stride;
                fs_asset_store(asset);
              }

              ImGui::Text("Texcoords: %p", primitive->texcoords);

              uint64_t new_color_count = primitive->color_count;

              ImGui::TextUnformatted("Color Count:");
              ImGui::SameLine();

              if (ImGui::InputScalar("##Color Count", ImGuiDataType_U64, &new_color_count, 0, 0, "%llu", ImGuiInputTextFlags_EnterReturnsTrue)) {
                resize_buffer(&primitive->colors, primitive->color_count * primitive->color_stride, new_color_count * primitive->color_stride);
                primitive->color_count = new_color_count;
                fs_asset_store(asset);
              }

              uint64_t new_color_stride = primitive->color_stride;

              ImGui::TextUnformatted("Color Stride:");
              ImGui::SameLine();

              if (ImGui::InputScalar("##Color Stride", ImGuiDataType_U64, &new_color_stride, 0, 0, "%llu", ImGuiInputTextFlags_EnterReturnsTrue)) {
                resize_buffer(&primitive->colors, primitive->color_count * primitive->color_stride, primitive->color_count * new_color_stride);
                primitive->color_stride = new_color_stride;
                fs_asset_store(asset);
              }

              ImGui::Text("Colors: %p", primitive->colors);

              uint64_t new_joint_count = primitive->joint_count;

              ImGui::TextUnformatted("Joint Count:");
              ImGui::SameLine();

              if (ImGui::InputScalar("##Joint Count", ImGuiDataType_U64, &new_joint_count, 0, 0, "%llu", ImGuiInputTextFlags_EnterReturnsTrue)) {
                resize_buffer(&primitive->joints, primitive->joint_count * primitive->joint_stride, new_joint_count * primitive->joint_stride);
                primitive->joint_count = new_joint_count;
                fs_asset_store(asset);
              }

              uint64_t new_joint_stride = primitive->joint_stride;

              ImGui::TextUnformatted("Joint Stride:");
              ImGui::SameLine();

              if (ImGui::InputScalar("##Joint Stride", ImGuiDataType_U64, &new_joint_stride, 0, 0, "%llu", ImGuiInputTextFlags_EnterReturnsTrue)) {
                resize_buffer(&primitive->joints, primitive->joint_count * primitive->joint_stride, primitive->joint_count * new_joint_stride);
                primitive->joint_stride = new_joint_stride;
                fs_asset_store(asset);
              }

              ImGui::Text("Joints: %p", primitive->joints);

              uint64_t new_weight_count = primitive->weight_count;

              ImGui::TextUnformatted("Weight Count:");
              ImGui::SameLine();

              if (ImGui::InputScalar("##Weight Count", ImGuiDataType_U64, &new_weight_count, 0, 0, "%llu", ImGuiInputTextFlags_EnterReturnsTrue)) {
                resize_buffer(&primitive->weights, primitive->weight_count * primitive->weight_stride, new_weight_count * primitive->weight_stride);
                primitive->weight_count = new_weight_count;
                fs_asset_store(asset);
              }

              uint64_t new_weight_stride = primitive->weight_stride;

              ImGui::TextUnformatted("Weight Stride:");
              ImGui::SameLine();

              if (ImGui::InputScalar("##Weight Stride", ImGuiDataType_U64, &new_weight_stride, 0, 0, "%llu", ImGuiInputTextFlags_EnterReturnsTrue)) {
                resize_buffer(&primitive->weights, primitive->weight_count * primitive->weight_stride, primitive->weight_count * new_weight_stride);
                primitive->weight_stride = new_weight_stride;
                fs_asset_store(asset);
              }

              ImGui::Text("Weights: %p", primitive->weights);

              ImGui::TreePop();
            }

            primitive_index++;
          }

          ImGui::TreePop();
        }

        mesh_index++;
      }

      uint64_t skin_index = 0;

      while (skin_index < model->skin_count) {

        fs_skin_t *skin = &model->skins[skin_index];

        if (ImGui::TreeNodeEx(skin, tree_node_flags, "Skin %llu: %s", skin_index, skin->name)) {

          ImGui::TextUnformatted("Name:");
          ImGui::SameLine();

          if (ImGui::InputText("##Name", skin->name, TI_PATH_SIZE, ImGuiInputTextFlags_EnterReturnsTrue)) {
            fs_asset_store(asset);
          }

          ImGui::Text("Root Joint: %p", skin->root_joint);

          draw_asset_joint(asset, skin->root_joint);

          ImGui::TreePop();
        }

        skin_index++;
      }

      break;
    }
    case FS_ASSET_TYPE_PIPELINE: {

      fs_pipeline_t *pipeline = (fs_pipeline_t *)asset->config;

      ImGui::TextUnformatted("Pipeline saving is not implemented.");

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

      ImGui::SeparatorText("Input Variables:");

      tree_node_flags |= ImGuiTreeNodeFlags_DefaultOpen;

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

      ImGui::SeparatorText("Descriptor Bindings:");

      uint64_t descriptor_binding_index = 0;

      while (descriptor_binding_index < pipeline->descriptor_binding_count) {

        fs_descriptor_binding_t *descriptor_binding = &pipeline->descriptor_bindings[descriptor_binding_index];

        if (ImGui::TreeNodeEx(descriptor_binding, tree_node_flags, "Binding %llu: %s", descriptor_binding_index, descriptor_binding->name)) {

          ImGui::BeginDisabled();
          draw_asset_descriptor_binding(asset, descriptor_binding);
          ImGui::EndDisabled();

          ImGui::TreePop();
        }

        descriptor_binding_index++;
      }

      break;
    }
    case FS_ASSET_TYPE_FONT: {

      fs_font_t *font = (fs_font_t *)asset->config;

      ImGui::TextUnformatted("Name:");
      ImGui::SameLine();

      if (ImGui::InputText("##Name", font->name, TI_PATH_SIZE, ImGuiInputTextFlags_EnterReturnsTrue)) {
        fs_asset_store(asset);
      }

      ImGui::Text("Buffer: %p", font->buffer);

      int buffer_in_use = 0;
      int source_index = 0;
      ImFontAtlas *atlas = ImGui::GetIO().Fonts;

      while (source_index < atlas->Sources.Size) {

        ImFontConfig *source = &atlas->Sources[source_index];

        if (font->buffer && (source->FontData == font->buffer)) {
          buffer_in_use = 1;
        }

        source_index++;
      }

      ImGui::BeginDisabled(buffer_in_use);

      uint64_t new_buffer_size = font->buffer_size;

      ImGui::TextUnformatted("Buffer Size:");
      ImGui::SameLine();

      if (ImGui::InputScalar("##Buffer Size", ImGuiDataType_U64, &new_buffer_size, 0, 0, "%llu", ImGuiInputTextFlags_EnterReturnsTrue)) {
        resize_buffer(&font->buffer, font->buffer_size, new_buffer_size);
        font->buffer_size = new_buffer_size;
        fs_asset_store(asset);
      }

      ImGui::EndDisabled();

      break;
    }
    case FS_ASSET_TYPE_DESCRIPTOR_BINDING: {

      fs_descriptor_binding_t *descriptor_binding = (fs_descriptor_binding_t *)asset->config;

      draw_asset_descriptor_binding(asset, descriptor_binding);

      break;
    }
    case FS_ASSET_TYPE_BUFFER: {

      fs_buffer_t *buffer = (fs_buffer_t *)asset->config;

      ImGui::TextUnformatted("Name:");
      ImGui::SameLine();

      if (ImGui::InputText("##Name", buffer->name, TI_PATH_SIZE, ImGuiInputTextFlags_EnterReturnsTrue)) {
        fs_asset_store(asset);
      }

      bool zero_data = buffer->zero_data > 0;

      ImGui::TextUnformatted("Zero Data:");
      ImGui::SameLine();

      if (ImGui::Checkbox("##Zero Data", &zero_data)) {
        buffer->zero_data = zero_data;
        fs_asset_store(asset);
      }

      ImGui::TextUnformatted("Size:");
      ImGui::SameLine();

      if (ImGui::InputScalar("##Size", ImGuiDataType_U64, &buffer->size, 0, 0, "%llu", ImGuiInputTextFlags_EnterReturnsTrue)) {
        fs_asset_store(asset);
      }

      ImGui::SeparatorText("Buffer Usage Flags:");
      ImGui::PushID(&buffer->buffer_usage_flags);

      ImGui::TextUnformatted("Transfer Source:");
      ImGui::SameLine();

      if (ImGui::CheckboxFlags("##Transfer Source", &buffer->buffer_usage_flags, VK_BUFFER_USAGE_TRANSFER_SRC_BIT)) {
        fs_asset_store(asset);
      }

      ImGui::TextUnformatted("Transfer Destination:");
      ImGui::SameLine();

      if (ImGui::CheckboxFlags("##Transfer Destination", &buffer->buffer_usage_flags, VK_BUFFER_USAGE_TRANSFER_DST_BIT)) {
        fs_asset_store(asset);
      }

      ImGui::TextUnformatted("Uniform Texel Buffer:");
      ImGui::SameLine();

      if (ImGui::CheckboxFlags("##Uniform Texel Buffer", &buffer->buffer_usage_flags, VK_BUFFER_USAGE_UNIFORM_TEXEL_BUFFER_BIT)) {
        fs_asset_store(asset);
      }

      ImGui::TextUnformatted("Storage Texel Buffer:");
      ImGui::SameLine();

      if (ImGui::CheckboxFlags("##Storage Texel Buffer", &buffer->buffer_usage_flags, VK_BUFFER_USAGE_STORAGE_TEXEL_BUFFER_BIT)) {
        fs_asset_store(asset);
      }

      ImGui::TextUnformatted("Uniform Buffer:");
      ImGui::SameLine();

      if (ImGui::CheckboxFlags("##Uniform Buffer", &buffer->buffer_usage_flags, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT)) {
        fs_asset_store(asset);
      }

      ImGui::TextUnformatted("Storage Buffer:");
      ImGui::SameLine();

      if (ImGui::CheckboxFlags("##Storage Buffer", &buffer->buffer_usage_flags, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT)) {
        fs_asset_store(asset);
      }

      ImGui::TextUnformatted("Index Buffer:");
      ImGui::SameLine();

      if (ImGui::CheckboxFlags("##Index Buffer", &buffer->buffer_usage_flags, VK_BUFFER_USAGE_INDEX_BUFFER_BIT)) {
        fs_asset_store(asset);
      }

      ImGui::TextUnformatted("Vertex Buffer:");
      ImGui::SameLine();

      if (ImGui::CheckboxFlags("##Vertex Buffer", &buffer->buffer_usage_flags, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT)) {
        fs_asset_store(asset);
      }

      ImGui::TextUnformatted("Indirect Buffer:");
      ImGui::SameLine();

      if (ImGui::CheckboxFlags("##Indirect Buffer", &buffer->buffer_usage_flags, VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT)) {
        fs_asset_store(asset);
      }

      ImGui::TextUnformatted("Shader Device Address:");
      ImGui::SameLine();

      if (ImGui::CheckboxFlags("##Shader Device Address", &buffer->buffer_usage_flags, VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT)) {
        fs_asset_store(asset);
      }

      ImGui::Text("Mask: %u", buffer->buffer_usage_flags);
      ImGui::PopID();

      ImGui::SeparatorText("Memory Property Flags:");
      ImGui::PushID(&buffer->memory_property_flags);

      ImGui::TextUnformatted("Device Local:");
      ImGui::SameLine();

      if (ImGui::CheckboxFlags("##Device Local", &buffer->memory_property_flags, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)) {
        fs_asset_store(asset);
      }

      ImGui::TextUnformatted("Host Visible:");
      ImGui::SameLine();

      if (ImGui::CheckboxFlags("##Host Visible", &buffer->memory_property_flags, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT)) {
        fs_asset_store(asset);
      }

      ImGui::TextUnformatted("Host Coherent:");
      ImGui::SameLine();

      if (ImGui::CheckboxFlags("##Host Coherent", &buffer->memory_property_flags, VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)) {
        fs_asset_store(asset);
      }

      ImGui::TextUnformatted("Host Cached:");
      ImGui::SameLine();

      if (ImGui::CheckboxFlags("##Host Cached", &buffer->memory_property_flags, VK_MEMORY_PROPERTY_HOST_CACHED_BIT)) {
        fs_asset_store(asset);
      }

      ImGui::TextUnformatted("Lazily Allocated:");
      ImGui::SameLine();

      if (ImGui::CheckboxFlags("##Lazily Allocated", &buffer->memory_property_flags, VK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT)) {
        fs_asset_store(asset);
      }

      ImGui::TextUnformatted("Protected:");
      ImGui::SameLine();

      if (ImGui::CheckboxFlags("##Protected", &buffer->memory_property_flags, VK_MEMORY_PROPERTY_PROTECTED_BIT)) {
        fs_asset_store(asset);
      }

      ImGui::Text("Mask: %u", buffer->memory_property_flags);
      ImGui::PopID();

      ImGui::SeparatorText("Memory Allocate Flags:");
      ImGui::PushID(&buffer->memory_allocate_flags);

      ImGui::TextUnformatted("Device Mask:");
      ImGui::SameLine();

      if (ImGui::CheckboxFlags("##Device Mask", &buffer->memory_allocate_flags, VK_MEMORY_ALLOCATE_DEVICE_MASK_BIT)) {
        fs_asset_store(asset);
      }

      ImGui::TextUnformatted("Device Address:");
      ImGui::SameLine();

      if (ImGui::CheckboxFlags("##Device Address", &buffer->memory_allocate_flags, VK_MEMORY_ALLOCATE_DEVICE_ADDRESS_BIT)) {
        fs_asset_store(asset);
      }

      ImGui::TextUnformatted("Device Address Capture Replay:");
      ImGui::SameLine();

      if (ImGui::CheckboxFlags("##Device Address Capture Replay", &buffer->memory_allocate_flags, VK_MEMORY_ALLOCATE_DEVICE_ADDRESS_CAPTURE_REPLAY_BIT)) {
        fs_asset_store(asset);
      }

      ImGui::Text("Mask: %u", buffer->memory_allocate_flags);
      ImGui::PopID();

      break;
    }
    case FS_ASSET_TYPE_IMAGE: {

      fs_image_t *image = (fs_image_t *)asset->config;

      ImGui::TextUnformatted("Name:");
      ImGui::SameLine();

      if (ImGui::InputText("##Name", image->name, TI_PATH_SIZE, ImGuiInputTextFlags_EnterReturnsTrue)) {
        fs_asset_store(asset);
      }

      ImGui::TextUnformatted("Width:");
      ImGui::SameLine();

      if (ImGui::InputScalar("##Width", ImGuiDataType_U32, &image->width, 0, 0, "%u", ImGuiInputTextFlags_EnterReturnsTrue)) {
        fs_asset_store(asset);
      }

      ImGui::TextUnformatted("Height:");
      ImGui::SameLine();

      if (ImGui::InputScalar("##Height", ImGuiDataType_U32, &image->height, 0, 0, "%u", ImGuiInputTextFlags_EnterReturnsTrue)) {
        fs_asset_store(asset);
      }

      ImGui::TextUnformatted("Depth:");
      ImGui::SameLine();

      if (ImGui::InputScalar("##Depth", ImGuiDataType_U32, &image->depth, 0, 0, "%u", ImGuiInputTextFlags_EnterReturnsTrue)) {
        fs_asset_store(asset);
      }

      ImGui::TextUnformatted("Mip Levels:");
      ImGui::SameLine();

      if (ImGui::InputScalar("##Mip Levels", ImGuiDataType_U32, &image->mip_levels, 0, 0, "%u", ImGuiInputTextFlags_EnterReturnsTrue)) {
        fs_asset_store(asset);
      }

      ImGui::TextUnformatted("Format:");
      ImGui::SameLine();

      if (ImGui::InputInt("##Format", (int32_t *)&image->format, 0, 0, ImGuiInputTextFlags_EnterReturnsTrue)) {
        fs_asset_store(asset);
      }

      ImGui::TextUnformatted("Image Layout:");
      ImGui::SameLine();

      if (ImGui::InputInt("##Image Layout", (int32_t *)&image->image_layout, 0, 0, ImGuiInputTextFlags_EnterReturnsTrue)) {
        fs_asset_store(asset);
      }

      ImGui::TextUnformatted("Image Type:");
      ImGui::SameLine();

      if (ImGui::InputInt("##Image Type", (int32_t *)&image->image_type, 0, 0, ImGuiInputTextFlags_EnterReturnsTrue)) {
        fs_asset_store(asset);
      }

      ImGui::TextUnformatted("Image Tiling:");
      ImGui::SameLine();

      if (ImGui::InputInt("##Image Tiling", (int32_t *)&image->image_tiling, 0, 0, ImGuiInputTextFlags_EnterReturnsTrue)) {
        fs_asset_store(asset);
      }

      ImGui::TextUnformatted("Image View Type:");
      ImGui::SameLine();

      if (ImGui::InputInt("##Image View Type", (int32_t *)&image->image_view_type, 0, 0, ImGuiInputTextFlags_EnterReturnsTrue)) {
        fs_asset_store(asset);
      }

      ImGui::SeparatorText("Image Usage Flags:");
      ImGui::PushID(&image->image_usage_flags);

      ImGui::TextUnformatted("Transfer Source:");
      ImGui::SameLine();

      if (ImGui::CheckboxFlags("##Transfer Source", &image->image_usage_flags, VK_IMAGE_USAGE_TRANSFER_SRC_BIT)) {
        fs_asset_store(asset);
      }

      ImGui::TextUnformatted("Transfer Destination:");
      ImGui::SameLine();

      if (ImGui::CheckboxFlags("##Transfer Destination", &image->image_usage_flags, VK_IMAGE_USAGE_TRANSFER_DST_BIT)) {
        fs_asset_store(asset);
      }

      ImGui::TextUnformatted("Sampled:");
      ImGui::SameLine();

      if (ImGui::CheckboxFlags("##Sampled", &image->image_usage_flags, VK_IMAGE_USAGE_SAMPLED_BIT)) {
        fs_asset_store(asset);
      }

      ImGui::TextUnformatted("Storage:");
      ImGui::SameLine();

      if (ImGui::CheckboxFlags("##Storage", &image->image_usage_flags, VK_IMAGE_USAGE_STORAGE_BIT)) {
        fs_asset_store(asset);
      }

      ImGui::TextUnformatted("Color Attachment:");
      ImGui::SameLine();

      if (ImGui::CheckboxFlags("##Color Attachment", &image->image_usage_flags, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT)) {
        fs_asset_store(asset);
      }

      ImGui::TextUnformatted("Depth Stencil Attachment:");
      ImGui::SameLine();

      if (ImGui::CheckboxFlags("##Depth Stencil Attachment", &image->image_usage_flags, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT)) {
        fs_asset_store(asset);
      }

      ImGui::TextUnformatted("Transient Attachment:");
      ImGui::SameLine();

      if (ImGui::CheckboxFlags("##Transient Attachment", &image->image_usage_flags, VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT)) {
        fs_asset_store(asset);
      }

      ImGui::TextUnformatted("Input Attachment:");
      ImGui::SameLine();

      if (ImGui::CheckboxFlags("##Input Attachment", &image->image_usage_flags, VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT)) {
        fs_asset_store(asset);
      }

      ImGui::TextUnformatted("Host Transfer:");
      ImGui::SameLine();

      if (ImGui::CheckboxFlags("##Host Transfer", &image->image_usage_flags, VK_IMAGE_USAGE_HOST_TRANSFER_BIT)) {
        fs_asset_store(asset);
      }

      ImGui::Text("Mask: %u", image->image_usage_flags);
      ImGui::PopID();

      ImGui::SeparatorText("Image Aspect Flags:");
      ImGui::PushID(&image->image_aspect_flags);

      ImGui::TextUnformatted("Color:");
      ImGui::SameLine();

      if (ImGui::CheckboxFlags("##Color", &image->image_aspect_flags, VK_IMAGE_ASPECT_COLOR_BIT)) {
        fs_asset_store(asset);
      }

      ImGui::TextUnformatted("Depth:");
      ImGui::SameLine();

      if (ImGui::CheckboxFlags("##Depth", &image->image_aspect_flags, VK_IMAGE_ASPECT_DEPTH_BIT)) {
        fs_asset_store(asset);
      }

      ImGui::TextUnformatted("Stencil:");
      ImGui::SameLine();

      if (ImGui::CheckboxFlags("##Stencil", &image->image_aspect_flags, VK_IMAGE_ASPECT_STENCIL_BIT)) {
        fs_asset_store(asset);
      }

      ImGui::TextUnformatted("Metadata:");
      ImGui::SameLine();

      if (ImGui::CheckboxFlags("##Metadata", &image->image_aspect_flags, VK_IMAGE_ASPECT_METADATA_BIT)) {
        fs_asset_store(asset);
      }

      ImGui::TextUnformatted("Plane 0:");
      ImGui::SameLine();

      if (ImGui::CheckboxFlags("##Plane 0", &image->image_aspect_flags, VK_IMAGE_ASPECT_PLANE_0_BIT)) {
        fs_asset_store(asset);
      }

      ImGui::TextUnformatted("Plane 1:");
      ImGui::SameLine();

      if (ImGui::CheckboxFlags("##Plane 1", &image->image_aspect_flags, VK_IMAGE_ASPECT_PLANE_1_BIT)) {
        fs_asset_store(asset);
      }

      ImGui::TextUnformatted("Plane 2:");
      ImGui::SameLine();

      if (ImGui::CheckboxFlags("##Plane 2", &image->image_aspect_flags, VK_IMAGE_ASPECT_PLANE_2_BIT)) {
        fs_asset_store(asset);
      }

      ImGui::Text("Mask: %u", image->image_aspect_flags);
      ImGui::PopID();

      ImGui::SeparatorText("Memory Property Flags:");
      ImGui::PushID(&image->memory_property_flags);

      ImGui::TextUnformatted("Device Local:");
      ImGui::SameLine();

      if (ImGui::CheckboxFlags("##Device Local", &image->memory_property_flags, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)) {
        fs_asset_store(asset);
      }

      ImGui::TextUnformatted("Host Visible:");
      ImGui::SameLine();

      if (ImGui::CheckboxFlags("##Host Visible", &image->memory_property_flags, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT)) {
        fs_asset_store(asset);
      }

      ImGui::TextUnformatted("Host Coherent:");
      ImGui::SameLine();

      if (ImGui::CheckboxFlags("##Host Coherent", &image->memory_property_flags, VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)) {
        fs_asset_store(asset);
      }

      ImGui::TextUnformatted("Host Cached:");
      ImGui::SameLine();

      if (ImGui::CheckboxFlags("##Host Cached", &image->memory_property_flags, VK_MEMORY_PROPERTY_HOST_CACHED_BIT)) {
        fs_asset_store(asset);
      }

      ImGui::TextUnformatted("Lazily Allocated:");
      ImGui::SameLine();

      if (ImGui::CheckboxFlags("##Lazily Allocated", &image->memory_property_flags, VK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT)) {
        fs_asset_store(asset);
      }

      ImGui::TextUnformatted("Protected:");
      ImGui::SameLine();

      if (ImGui::CheckboxFlags("##Protected", &image->memory_property_flags, VK_MEMORY_PROPERTY_PROTECTED_BIT)) {
        fs_asset_store(asset);
      }

      ImGui::Text("Mask: %u", image->memory_property_flags);
      ImGui::PopID();

      ImGui::SeparatorText("Memory Allocate Flags:");
      ImGui::PushID(&image->memory_allocate_flags);

      ImGui::TextUnformatted("Device Mask:");
      ImGui::SameLine();

      if (ImGui::CheckboxFlags("##Device Mask", &image->memory_allocate_flags, VK_MEMORY_ALLOCATE_DEVICE_MASK_BIT)) {
        fs_asset_store(asset);
      }

      ImGui::TextUnformatted("Device Address:");
      ImGui::SameLine();

      if (ImGui::CheckboxFlags("##Device Address", &image->memory_allocate_flags, VK_MEMORY_ALLOCATE_DEVICE_ADDRESS_BIT)) {
        fs_asset_store(asset);
      }

      ImGui::TextUnformatted("Device Address Capture Replay:");
      ImGui::SameLine();

      if (ImGui::CheckboxFlags("##Device Address Capture Replay", &image->memory_allocate_flags, VK_MEMORY_ALLOCATE_DEVICE_ADDRESS_CAPTURE_REPLAY_BIT)) {
        fs_asset_store(asset);
      }

      ImGui::Text("Mask: %u", image->memory_allocate_flags);
      ImGui::PopID();

      break;
    }
    case FS_ASSET_TYPE_FRAMEBUFFER: {

      fs_framebuffer_t *framebuffer = (fs_framebuffer_t *)asset->config;

      ImGui::TextUnformatted("Name:");
      ImGui::SameLine();

      if (ImGui::InputText("##Name", framebuffer->name, TI_PATH_SIZE, ImGuiInputTextFlags_EnterReturnsTrue)) {
        fs_asset_store(asset);
      }

      ImGui::TextUnformatted("Color Attachment Image:");
      ImGui::SameLine();

      if (ImGui::InputText("##Color Attachment Image", framebuffer->color_attachment_image, TI_PATH_SIZE, ImGuiInputTextFlags_EnterReturnsTrue)) {
        fs_asset_store(asset);
      }

      ImGui::TextUnformatted("Depth Attachment Image:");
      ImGui::SameLine();

      if (ImGui::InputText("##Depth Attachment Image", framebuffer->depth_attachment_image, TI_PATH_SIZE, ImGuiInputTextFlags_EnterReturnsTrue)) {
        fs_asset_store(asset);
      }

      if (ImGui::Button("Add Color Attachment")) {
      }

      break;
    }
    case FS_ASSET_TYPE_SWAPCHAIN: {

      fs_swapchain_t *swapchain = (fs_swapchain_t *)asset->config;

      ImGui::TextUnformatted("Name:");
      ImGui::SameLine();

      if (ImGui::InputText("##Name", swapchain->name, TI_PATH_SIZE, ImGuiInputTextFlags_EnterReturnsTrue)) {
        fs_asset_store(asset);
      }

      break;
    }
    case FS_ASSET_TYPE_RENDERPASS: {

      fs_renderpass_t *renderpass = (fs_renderpass_t *)asset->config;

      ImGui::TextUnformatted("Name:");
      ImGui::SameLine();

      if (ImGui::InputText("##Name", renderpass->name, TI_PATH_SIZE, ImGuiInputTextFlags_EnterReturnsTrue)) {
        fs_asset_store(asset);
      }

      ImGui::TextUnformatted("Initial Color Attachment Layout:");
      ImGui::SameLine();

      if (ImGui::InputInt("##Initial Color Attachment Layout", (int32_t *)&renderpass->initial_color_attachment_layout, 0, 0, ImGuiInputTextFlags_EnterReturnsTrue)) {
        fs_asset_store(asset);
      }

      ImGui::TextUnformatted("Initial Depth Attachment Layout:");
      ImGui::SameLine();

      if (ImGui::InputInt("##Initial Depth Attachment Layout", (int32_t *)&renderpass->initial_depth_attachment_layout, 0, 0, ImGuiInputTextFlags_EnterReturnsTrue)) {
        fs_asset_store(asset);
      }

      ImGui::TextUnformatted("Final Color Attachment Layout:");
      ImGui::SameLine();

      if (ImGui::InputInt("##Final Color Attachment Layout", (int32_t *)&renderpass->final_color_attachment_layout, 0, 0, ImGuiInputTextFlags_EnterReturnsTrue)) {
        fs_asset_store(asset);
      }

      ImGui::TextUnformatted("Final Depth Attachment Layout:");
      ImGui::SameLine();

      if (ImGui::InputInt("##Final Depth Attachment Layout", (int32_t *)&renderpass->final_depth_attachment_layout, 0, 0, ImGuiInputTextFlags_EnterReturnsTrue)) {
        fs_asset_store(asset);
      }

      break;
    }
    case FS_ASSET_TYPE_RENDERER: {

      fs_renderer_t *renderer = (fs_renderer_t *)asset->config;

      ImGui::TextUnformatted("Name:");
      ImGui::SameLine();

      if (ImGui::InputText("##Name", renderer->name, TI_PATH_SIZE, ImGuiInputTextFlags_EnterReturnsTrue)) {
        fs_asset_store(asset);
      }

      ImGui::TextUnformatted("Debug Line Vertex Buffer:");
      ImGui::SameLine();

      if (ImGui::InputText("##Debug Line Vertex Buffer", renderer->debug_line_vertex_buffer, TI_PATH_SIZE, ImGuiInputTextFlags_EnterReturnsTrue)) {
        fs_asset_store(asset);
      }

      ImGui::TextUnformatted("Debug Line Index Buffer:");
      ImGui::SameLine();

      if (ImGui::InputText("##Debug Line Index Buffer", renderer->debug_line_index_buffer, TI_PATH_SIZE, ImGuiInputTextFlags_EnterReturnsTrue)) {
        fs_asset_store(asset);
      }

      ImGui::TextUnformatted("Full Screen Vertex Buffer:");
      ImGui::SameLine();

      if (ImGui::InputText("##Full Screen Vertex Buffer", renderer->full_screen_vertex_buffer, TI_PATH_SIZE, ImGuiInputTextFlags_EnterReturnsTrue)) {
        fs_asset_store(asset);
      }

      ImGui::TextUnformatted("Full Screen Index Buffer:");
      ImGui::SameLine();

      if (ImGui::InputText("##Full Screen Index Buffer", renderer->full_screen_index_buffer, TI_PATH_SIZE, ImGuiInputTextFlags_EnterReturnsTrue)) {
        fs_asset_store(asset);
      }

      break;
    }
  }

  ImGui::PopItemWidth();
  ImGui::PopID();
}

static void draw_asset_joint(fs_asset_t *asset, fs_joint_t *joint) {
  if (joint == 0) {
    return;
  }

  ImGuiTreeNodeFlags tree_node_flags = ImGuiTreeNodeFlags_OpenOnArrow |
                                       ImGuiTreeNodeFlags_SpanFullWidth |
                                       ImGuiTreeNodeFlags_FramePadding;

  if (ImGui::TreeNodeEx(joint, tree_node_flags, "Joint: %s", joint->name)) {

    ImGui::TextUnformatted("Name:");
    ImGui::SameLine();

    if (ImGui::InputText("##Name", joint->name, TI_PATH_SIZE, ImGuiInputTextFlags_EnterReturnsTrue)) {
      fs_asset_store(asset);
    }

    uint64_t new_child_count = joint->child_count;

    ImGui::TextUnformatted("Child Count:");
    ImGui::SameLine();

    if (ImGui::InputScalar("##Child Count", ImGuiDataType_U64, &new_child_count, 0, 0, "%llu", ImGuiInputTextFlags_EnterReturnsTrue)) {
      resize_joint_children(joint, new_child_count);
      fs_asset_store(asset);
    }

    ImGui::Text("Children: %p", joint->children);

    uint64_t child_index = 0;

    while (child_index < joint->child_count) {

      draw_asset_joint(asset, &joint->children[child_index]);

      child_index++;
    }

    ImGui::TreePop();
  }
}

static void draw_asset_descriptor_binding(fs_asset_t *asset, fs_descriptor_binding_t *descriptor_binding) {
  ImGui::TextUnformatted("Name:");
  ImGui::SameLine();

  if (ImGui::InputText("##Name", descriptor_binding->name, TI_PATH_SIZE, ImGuiInputTextFlags_EnterReturnsTrue)) {
    fs_asset_store(asset);
  }

  ImGui::TextUnformatted("Set:");
  ImGui::SameLine();

  if (ImGui::InputScalar("##Set", ImGuiDataType_U32, &descriptor_binding->set, 0, 0, "%u", ImGuiInputTextFlags_EnterReturnsTrue)) {
    fs_asset_store(asset);
  }

  ImGui::TextUnformatted("Binding:");
  ImGui::SameLine();

  if (ImGui::InputScalar("##Binding", ImGuiDataType_U32, &descriptor_binding->binding, 0, 0, "%u", ImGuiInputTextFlags_EnterReturnsTrue)) {
    fs_asset_store(asset);
  }

  ImGui::TextUnformatted("Descriptor Type:");
  ImGui::SameLine();

  if (ImGui::InputInt("##Descriptor Type", &descriptor_binding->descriptor_type, 0, 0, ImGuiInputTextFlags_EnterReturnsTrue)) {
    fs_asset_store(asset);
  }

  ImGui::BeginDisabled((descriptor_binding->descriptor_type != VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER) &&
                       (descriptor_binding->descriptor_type != VK_DESCRIPTOR_TYPE_STORAGE_BUFFER));

  ImGui::TextUnformatted("Block Size:");
  ImGui::SameLine();

  if (ImGui::InputInt("##Block Size", &descriptor_binding->block_size, 0, 0, ImGuiInputTextFlags_EnterReturnsTrue)) {
    fs_asset_store(asset);
  }

  int32_t new_block_variable_count = descriptor_binding->block_variable_count;

  ImGui::TextUnformatted("Block Variable Count:");
  ImGui::SameLine();

  if (ImGui::InputInt("##Block Variable Count", &new_block_variable_count, 0, 0, ImGuiInputTextFlags_EnterReturnsTrue)) {
    resize_binding_variables(descriptor_binding, new_block_variable_count);
    fs_asset_store(asset);
  }

  ImGui::Text("Block Variables: %p", descriptor_binding->block_variables);

  ImGuiTableFlags table_flags = ImGuiTableFlags_Borders |
                               ImGuiTableFlags_RowBg |
                               ImGuiTableFlags_Resizable;

  if (ImGui::BeginTable("Block Variables", 3, table_flags)) {

    ImGui::TableSetupColumn("Name:");
    ImGui::TableSetupColumn("Offset:");
    ImGui::TableSetupColumn("Size:");
    ImGui::TableHeadersRow();

    int32_t block_variable_index = 0;

    while (block_variable_index < descriptor_binding->block_variable_count) {

      fs_block_variable_t *block_variable = &descriptor_binding->block_variables[block_variable_index];

      ImGui::PushID(block_variable);
      ImGui::TableNextRow();
      ImGui::TableNextColumn();
      if (ImGui::InputText("##Name", block_variable->name, TI_PATH_SIZE, ImGuiInputTextFlags_EnterReturnsTrue)) {
        fs_asset_store(asset);
      }

      ImGui::TableNextColumn();
      if (ImGui::InputScalar("##Offset", ImGuiDataType_U32, &block_variable->offset, 0, 0, "%u", ImGuiInputTextFlags_EnterReturnsTrue)) {
        fs_asset_store(asset);
      }

      ImGui::TableNextColumn();
      if (ImGui::InputScalar("##Size", ImGuiDataType_U32, &block_variable->size, 0, 0, "%u", ImGuiInputTextFlags_EnterReturnsTrue)) {
        fs_asset_store(asset);
      }

      ImGui::PopID();

      block_variable_index++;
    }

    ImGui::EndTable();
  }

  ImGui::EndDisabled();
}

static void resize_model_meshes(fs_model_t *model, uint64_t count) {
  if (count != model->mesh_count) {
    fs_mesh_t *meshes = (fs_mesh_t *)TI_ALLOC(count * sizeof(fs_mesh_t), 1, 0);
    memcpy(meshes, model->meshes, TI_MIN(model->mesh_count, count) * sizeof(fs_mesh_t));

    uint64_t index = count;

    while (index < model->mesh_count) {
      fs_mesh_destroy(&model->meshes[index]);
      index++;
    }

    TI_FREE(model->meshes);
    model->meshes = meshes;
    model->mesh_count = count;
  }
}

static void resize_mesh_primitives(fs_mesh_t *mesh, uint64_t count) {
  if (count != mesh->primitive_count) {
    fs_primitive_t *primitives = (fs_primitive_t *)TI_ALLOC(count * sizeof(fs_primitive_t), 1, 0);
    memcpy(primitives, mesh->primitives, TI_MIN(mesh->primitive_count, count) * sizeof(fs_primitive_t));

    uint64_t index = count;

    while (index < mesh->primitive_count) {
      fs_primitive_destroy(&mesh->primitives[index]);
      index++;
    }

    TI_FREE(mesh->primitives);
    mesh->primitives = primitives;
    mesh->primitive_count = count;
  }
}

static void resize_joint_children(fs_joint_t *joint, uint64_t count) {
  if (count != joint->child_count) {
    fs_joint_t *children = (fs_joint_t *)TI_ALLOC(count * sizeof(fs_joint_t), 1, 0);
    memcpy(children, joint->children, TI_MIN(joint->child_count, count) * sizeof(fs_joint_t));

    uint64_t index = count;

    while (index < joint->child_count) {
      fs_joint_destroy(&joint->children[index]);
      index++;
    }

    TI_FREE(joint->children);
    joint->children = children;
    joint->child_count = count;
  }
}

static void resize_model_skins(fs_model_t *model, uint64_t count) {
  if (count != model->skin_count) {
    fs_skin_t *skins = (fs_skin_t *)TI_ALLOC(count * sizeof(fs_skin_t), 1, 0);
    memcpy(skins, model->skins, TI_MIN(model->skin_count, count) * sizeof(fs_skin_t));

    uint64_t index = count;

    while (index < model->skin_count) {
      fs_skin_destroy(&model->skins[index]);
      index++;
    }

    index = model->skin_count;

    while (index < count) {
      skins[index].root_joint = (fs_joint_t *)TI_ALLOC(sizeof(fs_joint_t), 1, 0);
      index++;
    }

    TI_FREE(model->skins);
    model->skins = skins;
    model->skin_count = count;
  }
}

static void resize_binding_variables(fs_descriptor_binding_t *binding, int32_t count) {
  if (count < 0) {
    return;
  }

  if (count != binding->block_variable_count) {
    fs_block_variable_t *variables = (fs_block_variable_t *)TI_ALLOC(count * sizeof(fs_block_variable_t), 1, 0);
    memcpy(variables, binding->block_variables, TI_MIN(binding->block_variable_count, count) * sizeof(fs_block_variable_t));

    TI_FREE(binding->block_variables);
    binding->block_variables = variables;
    binding->block_variable_count = count;
  }
}

static void resize_buffer(void **buffer, uint64_t old_size, uint64_t new_size) {
  if (new_size != old_size) {
    void *new_buffer = TI_ALLOC(new_size, 0, 0);
    memcpy(new_buffer, *buffer, TI_MIN(old_size, new_size));

    TI_FREE(*buffer);
    *buffer = new_buffer;
  }
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
