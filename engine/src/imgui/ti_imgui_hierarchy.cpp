#include <ti_pch.h>

#include <imgui.h>

static void hierarchy_check_background_clicks(void);

static void hierarchy_draw_controls(void);
static void hierarchy_draw_tree(ecs_entity_t entity);

static char s_entity_parent[TI_PATH_SIZE] = {0};
static char s_entity_name[TI_PATH_SIZE] = {0};

ecs_entity_t g_selected_entity = 0;

void hierarchy_draw(void) {
  ImGui::Begin("Hierarchy");

  hierarchy_check_background_clicks();

  hierarchy_draw_controls();

  ecs_defer_begin(g_scene.world);

  ecs_query_t *root_entites = scene_root_children(&g_scene, g_scene.root_entity);
  ecs_iter_t root_it = ecs_query_iter(g_scene.world, root_entites);

  while (ecs_query_next(&root_it)) {

    uint32_t entity_index = 0;
    uint32_t entity_count = root_it.count;

    while (entity_index < entity_count) {

      hierarchy_draw_tree(root_it.entities[entity_index]);

      entity_index++;
    }
  }

  ecs_defer_end(g_scene.world);

  ImGui::End();
}
void hierarchy_reset(void) {
  g_selected_entity = 0;
}

static void hierarchy_check_background_clicks(void) {
  if (ImGui::IsWindowHovered(ImGuiHoveredFlags_AllowWhenBlockedByActiveItem) &&
      ImGui::IsMouseClicked(ImGuiMouseButton_Left) &&
      ImGui::IsAnyItemHovered() == false) {

    g_selected_entity = 0;
  }
}

static void hierarchy_draw_controls(void) {
  ImGui::InputText("Parent ID", s_entity_parent, TI_PATH_SIZE);
  ImGui::InputText("Entity Name", s_entity_name, TI_PATH_SIZE);

  if (ImGui::Button("Create Entity")) {

    ecs_entity_t parent = g_selected_entity ? g_selected_entity : strtoull(s_entity_parent, 0, 10);

    entity_create(&g_scene, s_entity_name, parent);
  }
}
static void hierarchy_draw_tree(ecs_entity_t entity) {
  ImGui::PushID((int32_t)entity);

  ecs_iter_t child_it = ecs_children(g_scene.world, entity);

  ecs_children_next(&child_it);

  uint32_t tree_node_flags = ImGuiTreeNodeFlags_OpenOnArrow |
                             ImGuiTreeNodeFlags_SpanFullWidth |
                             ImGuiTreeNodeFlags_FramePadding;

  if (child_it.count == 0) {
    tree_node_flags |= ImGuiTreeNodeFlags_Leaf;
  }

  if (entity == g_selected_entity) {
    tree_node_flags |= ImGuiTreeNodeFlags_Selected;
  }

  const char *entity_name = ecs_get_name(g_scene.world, entity);

  uint8_t opened = ImGui::TreeNodeEx(entity_name, tree_node_flags);

  if (ImGui::IsItemClicked(0) || ImGui::IsItemClicked(1)) {

    g_selected_entity = entity;
    g_selected_file = 0;

    g_inspector_type = INSPECTOR_TYPE_ENTITY;
  }

  if (ImGui::BeginPopupContextItem("hierarchy_context_menu")) {

    if (ImGui::MenuItem("Remove")) {
      if (g_selected_entity) {

        entity_destroy(&g_scene, g_selected_entity);

        g_selected_entity = 0;
      }
    }

    ImGui::Separator();

    if (ImGui::MenuItem("Rename")) {
      // TODO
    }

    ImGui::EndPopup();
  }

  if (opened) {

    do {

      uint32_t child_index = 0;
      uint32_t child_count = child_it.count;

      while (child_index < child_count) {

        hierarchy_draw_tree(child_it.entities[child_index]);

        child_index++;
      }

    } while (ecs_children_next(&child_it));

    ImGui::TreePop();
  }

  ImGui::PopID();
}
