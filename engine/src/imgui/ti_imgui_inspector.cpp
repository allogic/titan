#include <ti_pch.h>

#include <imgui.h>

static void inspector_draw_file_controls(void);
static void inspector_draw_entity_controls(void);

static void inspector_draw_file(void);
static void inspector_draw_entity(void);

static comp_type_t s_selected_comp = COMP_TYPE_TRANSFORM;

static const char *s_component_names[] = {
  "Transform",
  "Camera",
  "Material",
  "Mesh",
  "Skeleton",
};

inspector_type_t g_inspector_type = INSPECTOR_TYPE_NONE;

void inspector_draw(void) {
  ImGui::Begin("Inspector");

  switch (g_inspector_type) {
    case INSPECTOR_TYPE_FILE: {

      if (g_selected_file) {

        inspector_draw_file_controls();
        inspector_draw_file();
      }

      break;
    }
    case INSPECTOR_TYPE_ENTITY: {

      if (g_selected_entity) {

        inspector_draw_entity_controls();
        inspector_draw_entity();
      }

      break;
    }
  }

  ImGui::End();
}
void inspector_reset(void) {
  s_selected_comp = COMP_TYPE_TRANSFORM;

  g_inspector_type = INSPECTOR_TYPE_NONE;
}

static void inspector_draw_file_controls(void) {
  /*
  adb_asset_t *asset = (adb_asset_t *)g_selected_asset->value;

  switch (asset->node_type) {
    case ADB_NODE_TYPE_PHYSICAL: {

      ImGui::Text(asset->name);
      ImGui::Text(asset->physical_path);

      if (ImGui::Button("Re-Import")) {
        // TODO
      }

      break;
    }
    case ADB_NODE_TYPE_VIRTUAL: {

      switch (asset->asset_type) {
        case ADB_ASSET_TYPE_MODEL: {

          adb_model_t *model = (adb_model_t *)asset->value;

          break;
        }
        case ADB_ASSET_TYPE_SKELETON: {

          adb_skeleton_t *skeleton = (adb_skeleton_t *)asset->value;

          break;
        }
      }

      break;
    }
  }
  */
}
static void inspector_draw_entity_controls(void) {
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

  if (ImGui::Button("+")) {

    switch (s_selected_comp) {
      case COMP_TYPE_TRANSFORM: {

        ecs_add(g_scene.world, g_selected_entity, transform_t);

        transform_t *transform = ecs_get_mut(g_scene.world, g_selected_entity, transform_t);

        transform_init(transform);

        break;
      }
      case COMP_TYPE_CAMERA: {

        ecs_add(g_scene.world, g_selected_entity, camera_t);

        camera_t *camera = ecs_get_mut(g_scene.world, g_selected_entity, camera_t);

        camera_init(camera);

        break;
      }
      case COMP_TYPE_MATERIAL: {

        ecs_add(g_scene.world, g_selected_entity, material_t);

        material_t *material = ecs_get_mut(g_scene.world, g_selected_entity, material_t);

        material_init(material);

        break;
      }
      case COMP_TYPE_MESH: {

        ecs_add(g_scene.world, g_selected_entity, mesh_t);

        mesh_t *mesh = ecs_get_mut(g_scene.world, g_selected_entity, mesh_t);

        mesh_init(mesh);

        break;
      }
      case COMP_TYPE_SKELETON: {

        ecs_add(g_scene.world, g_selected_entity, skeleton_t);

        skeleton_t *skeleton = ecs_get_mut(g_scene.world, g_selected_entity, skeleton_t);

        skeleton_init(skeleton);

        break;
      }
    }
  }

  ImGui::SameLine();

  if (ImGui::Button("-")) {

    switch (s_selected_comp) {
      case COMP_TYPE_TRANSFORM: {

        ecs_remove(g_scene.world, g_selected_entity, transform_t);

        break;
      }
      case COMP_TYPE_CAMERA: {

        ecs_remove(g_scene.world, g_selected_entity, camera_t);

        break;
      }
      case COMP_TYPE_MATERIAL: {

        ecs_remove(g_scene.world, g_selected_entity, material_t);

        break;
      }
      case COMP_TYPE_MESH: {

        ecs_remove(g_scene.world, g_selected_entity, mesh_t);

        break;
      }
      case COMP_TYPE_SKELETON: {

        ecs_remove(g_scene.world, g_selected_entity, skeleton_t);

        break;
      }
    }
  }
}

static void inspector_draw_file(void) {
  /*
  adb_asset_t *asset = (adb_asset_t *)g_selected_asset->value;

  if (asset->node_type == ADB_NODE_TYPE_VIRTUAL) {

    switch (asset->asset_type) {
      case ADB_ASSET_TYPE_MODEL: {

        adb_model_t *model = (adb_model_t *)asset->value;

        ImGui::Text("%s", model->name);
        ImGui::Text("Mesh Count: %llu", model->mesh_count);

        uint32_t tree_node_flags = ImGuiTreeNodeFlags_OpenOnArrow |
                                   ImGuiTreeNodeFlags_SpanFullWidth |
                                   ImGuiTreeNodeFlags_FramePadding;

        uint64_t mesh_index = 0;
        uint64_t mesh_count = model->mesh_count;

        while (mesh_index < mesh_count) {

          adb_mesh_t *mesh = &model->meshes[mesh_index];

          if (ImGui::TreeNodeEx(mesh->name, tree_node_flags)) {

            ImGui::Text("Primitive Count: %llu", mesh->primitive_count);

            uint64_t primitive_index = 0;
            uint64_t primitive_count = mesh->primitive_count;

            while (primitive_index < primitive_count) {

              adb_primitive_t *primitive = &mesh->primitives[primitive_index];

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
      case ADB_ASSET_TYPE_SKELETON: {
        break;
      }
    }
  }
  */
}
static void inspector_draw_entity(void) {
  uint32_t tree_node_flags = ImGuiTreeNodeFlags_OpenOnArrow |
                             ImGuiTreeNodeFlags_SpanFullWidth |
                             ImGuiTreeNodeFlags_FramePadding;

  transform_t *transform = ecs_get_mut(g_scene.world, g_selected_entity, transform_t);
  camera_t *camera = ecs_get_mut(g_scene.world, g_selected_entity, camera_t);
  material_t *material = ecs_get_mut(g_scene.world, g_selected_entity, material_t);
  mesh_t *mesh = ecs_get_mut(g_scene.world, g_selected_entity, mesh_t);
  skeleton_t *skeleton = ecs_get_mut(g_scene.world, g_selected_entity, skeleton_t);

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

      if (ImGui::InputInt("Shader Ref", (int32_t *)&material->shader_ref)) {
        // TODO
      }

      if (ImGui::InputInt("Material Ref", (int32_t *)&material->material_ref)) {
        // TODO
      }

      ImGui::TreePop();
    }
  }

  if (mesh) {

    if (ImGui::TreeNodeEx("Mesh", tree_node_flags)) {

      if (ImGui::InputInt("Mesh Ref", (int32_t *)&mesh->mesh_ref)) {
        // TODO
      }

      ImGui::TreePop();
    }
  }

  if (skeleton) {

    if (ImGui::TreeNodeEx("Skeleton", tree_node_flags)) {

      if (ImGui::InputInt("Skeleton Ref", (int32_t *)&skeleton->skeleton_ref)) {
        // TODO
      }

      ImGui::TreePop();
    }
  }
}
