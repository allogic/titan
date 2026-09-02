#include <ti_pch.h>

#include <imgui.h>

static void check_background_clicks(void);

static void draw_background(void);
static void draw_controls(void);
static void draw_root_entities(void);
static void draw_tree(ecs_entity_t entity);

static char s_entity_name[TI_PATH_SIZE] = {0};

static ecs_entity_t s_selected_entity = 0;

void hierarchy_draw(void) {
  ImGui::Begin("Hierarchy", 0, ImGuiWindowFlags_NoDecoration);

  check_background_clicks();

  draw_background();
  draw_controls();

  ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(5.0F, 3.0F));
  ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(5.0F, 0.0F));

  draw_root_entities();

  ImGui::PopStyleVar(2);

  ImGui::End();
}
void hierarchy_refresh(void) {
  // TODO
}
void hierarchy_reset(void) {
  memset(s_entity_name, 0, TI_PATH_SIZE);

  s_selected_entity = 0;

  inspector_select(INSPECTOR_TYPE_NONE, 0);
}

static void check_background_clicks(void) {
  if (ImGui::IsWindowHovered(ImGuiHoveredFlags_AllowWhenBlockedByActiveItem) &&
      ImGui::IsMouseClicked(ImGuiMouseButton_Left) &&
      ImGui::IsAnyItemHovered() == 0) {

    s_selected_entity = 0;

    inspector_select(INSPECTOR_TYPE_NONE, 0);
  }
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
static void draw_controls(void) {
  ImGui::InputText("Entity Name", s_entity_name, TI_PATH_SIZE);

  if (ImGui::Button("Create Entity")) {

    entity_create(&g_scene, s_entity_name, s_selected_entity);
  }
}
static void draw_root_entities(void) {
  ecs_defer_begin(g_scene.world);

  ecs_query_t *root_entites = scene_root_children(&g_scene, g_scene.root_entity);
  ecs_iter_t root_it = ecs_query_iter(g_scene.world, root_entites);

  while (ecs_query_next(&root_it)) {

    uint32_t entity_index = 0;
    uint32_t entity_count = root_it.count;

    while (entity_index < entity_count) {

      draw_tree(root_it.entities[entity_index]);

      entity_index++;
    }
  }

  ecs_defer_end(g_scene.world);
}
static void draw_tree(ecs_entity_t entity) {
  ImGui::PushID((int32_t)entity);

  ecs_iter_t child_it = ecs_children(g_scene.world, entity);

  ecs_children_next(&child_it);

  ImGuiTreeNodeFlags tree_node_flags = ImGuiTreeNodeFlags_OpenOnArrow |
                                       ImGuiTreeNodeFlags_SpanFullWidth |
                                       ImGuiTreeNodeFlags_FramePadding;

  if (child_it.count == 0) {
    tree_node_flags |= ImGuiTreeNodeFlags_Leaf;
  }

  if (entity == s_selected_entity) {
    tree_node_flags |= ImGuiTreeNodeFlags_Selected;
  }

  const char *entity_name = ecs_get_name(g_scene.world, entity);

  uint8_t opened = ImGui::TreeNodeEx(entity_name, tree_node_flags);

  if (ImGui::IsItemClicked(0) || ImGui::IsItemClicked(1)) {

    s_selected_entity = entity;

    inspector_select(INSPECTOR_TYPE_ENTITY, (void *)entity);
  }

  if (ImGui::BeginPopupContextItem("hierarchy_context_menu")) {

    if (ImGui::MenuItem("Remove")) {

      if (s_selected_entity) {

        entity_destroy(&g_scene, s_selected_entity);

        s_selected_entity = 0;

        inspector_select(INSPECTOR_TYPE_NONE, 0);
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

        draw_tree(child_it.entities[child_index]);

        child_index++;
      }

    } while (ecs_children_next(&child_it));

    ImGui::TreePop();
  }

  ImGui::PopID();
}
