#include <ti_pch.h>

#include <imgui.h>

static void draw_background(void);
static void draw_file_controls(void);
static void draw_file(void);
static void draw_entity_controls(void);
static void draw_entity(void);

static inspector_type_t s_inspector_type = INSPECTOR_TYPE_NONE;
static comp_type_t s_selected_comp = COMP_TYPE_TRANSFORM;

static void *s_selected_data = 0;

static const char *s_component_names[] = {
  "Transform",
  "Camera",
  "Material",
  "Mesh",
  "Skeleton",
};

void inspector_draw(void) {
  ImGui::Begin("Inspector", 0, ImGuiWindowFlags_NoDecoration);

  draw_background();

  switch (s_inspector_type) {

    case INSPECTOR_TYPE_ENTITY: {

      draw_entity_controls();
      draw_entity();

      break;
    }
    case INSPECTOR_TYPE_FILE: {

      draw_file_controls();
      draw_file();

      break;
    }
  }

  ImGui::End();
}
void inspector_select(inspector_type_t type, void *selection) {
  s_inspector_type = type;
  s_selected_data = selection;
}
void inspector_reset(void) {
  s_inspector_type = INSPECTOR_TYPE_NONE;
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
static void draw_file_controls(void) {
  // TODO
}
static void draw_file(void) {
  fs_asset_t *asset = (fs_asset_t *)s_selected_data;

  switch (asset->type) {

    case FS_ASSET_TYPE_MODEL: {

      fs_model_t *model = (fs_model_t *)asset->fs_instance;

      ImGui::Text("%s", model->name);
      ImGui::Text("Mesh Count: %llu", model->mesh_count);

      ImGuiTreeNodeFlags tree_node_flags = ImGuiTreeNodeFlags_OpenOnArrow |
                                           ImGuiTreeNodeFlags_SpanFullWidth |
                                           ImGuiTreeNodeFlags_FramePadding;

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

      fs_pipeline_t *pipeline = (fs_pipeline_t *)asset->fs_instance;

      ImGui::Text("%s", pipeline->name);

      ImGui::SeparatorText("Input Variables");

      ImGuiTreeNodeFlags tree_node_flags = ImGuiTreeNodeFlags_DefaultOpen |
                                           ImGuiTreeNodeFlags_SpanFullWidth |
                                           ImGuiTreeNodeFlags_FramePadding;

      uint64_t input_variable_index = 0;
      uint64_t input_variable_count = pipeline->input_variable_count;

      while (input_variable_index < input_variable_count) {

        fs_input_variable_t *input_variable = &pipeline->input_variables[input_variable_index];

        if (ImGui::TreeNodeEx(input_variable->name, tree_node_flags)) {

          ImGui::Text("Location: %u", input_variable->location);
          ImGui::Text("Format: %u", input_variable->format);
          ImGui::Text("Built-In: %u", input_variable->built_in);

          ImGui::TreePop();
        }

        input_variable_index++;
      }

      ImGui::SeparatorText("Descriptor Bindings");

      uint64_t descriptor_binding_index = 0;
      uint64_t descriptor_binding_count = pipeline->descriptor_binding_count;

      while (descriptor_binding_index < descriptor_binding_count) {

        fs_descriptor_binding_t *descriptor_binding = &pipeline->descriptor_bindings[descriptor_binding_index];

        if (ImGui::TreeNodeEx(descriptor_binding->name, tree_node_flags)) {

          ImGui::Text("Set: %u", descriptor_binding->set);
          ImGui::Text("Binding: %u", descriptor_binding->binding);

          if ((descriptor_binding->descriptor_type == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER) ||
              (descriptor_binding->descriptor_type == VK_DESCRIPTOR_TYPE_STORAGE_BUFFER)) {

            ImGui::Text("Block Size: %d", descriptor_binding->block_size);
            ImGui::Text("Block Variables: %d", descriptor_binding->block_variable_count);

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

          ImGui::TreePop();
        }

        descriptor_binding_index++;
      }

      break;
    }
    case FS_ASSET_TYPE_FONT: {

      fs_font_t *font = (fs_font_t *)asset->fs_instance;

      ImGui::Text("%s", font->name);

      break;
    }
    case FS_ASSET_TYPE_DESCRIPTOR_BINDING: {

      fs_descriptor_binding_t *descriptor_binding = (fs_descriptor_binding_t *)asset->fs_instance;

      ImGui::Text("%s", descriptor_binding->name);

      break;
    }
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
