#include <ti_pch.h>

#include <imgui.h>

static void check_background_clicks(void);

static void draw_background(void);
static void draw_handles(void);

static uint64_t s_selected_handle = 0;

void im_handle_draw(void) {
  ImGui::Begin("Handle", 0, ImGuiWindowFlags_NoDecoration);

  draw_background();

  ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(5.0F, 3.0F));
  ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(5.0F, 0.0F));

  draw_handles();

  ImGui::PopStyleVar(2);

  ImGui::End();
}
void im_handle_refresh(void) {
  // TODO
}
void im_handle_reset(void) {
  s_selected_handle = 0;

  im_inspector_select(IM_INSPECTOR_TYPE_NONE, 0);
}

static void check_background_clicks(void) {
  if (ImGui::IsWindowHovered(ImGuiHoveredFlags_AllowWhenBlockedByActiveItem) &&
      ImGui::IsMouseClicked(ImGuiMouseButton_Left) &&
      ImGui::IsAnyItemHovered() == 0) {

    s_selected_handle = 0;

    im_inspector_select(IM_INSPECTOR_TYPE_NONE, 0);
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
static void draw_handles(void) {
  ImGuiTreeNodeFlags tree_node_flags = ImGuiTreeNodeFlags_OpenOnArrow |
                                       ImGuiTreeNodeFlags_SpanFullWidth |
                                       ImGuiTreeNodeFlags_FramePadding;

  map_iter_t asset_it = map_iter(&g_assets);

  if (ImGui::TreeNodeEx("Assets", tree_node_flags)) {

    while (map_next(&asset_it)) {

      fs_asset_t *asset = (fs_asset_t *)map_value(&asset_it);

      switch (asset->type) {

        case FS_ASSET_TYPE_MODEL: {

          fs_model_t *model = (fs_model_t *)asset->config;

          if (ImGui::TreeNodeEx(model->name, tree_node_flags | ImGuiTreeNodeFlags_Leaf)) {

            ImGui::TreePop();
          }

          break;
        }
        case FS_ASSET_TYPE_PIPELINE: {

          fs_pipeline_t *pipeline = (fs_pipeline_t *)asset->config;

          if (ImGui::TreeNodeEx(pipeline->name, tree_node_flags | ImGuiTreeNodeFlags_Leaf)) {

            ImGui::TreePop();
          }

          break;
        }
        case FS_ASSET_TYPE_FONT: {

          fs_font_t *font = (fs_font_t *)asset->config;

          if (ImGui::TreeNodeEx(font->name, tree_node_flags | ImGuiTreeNodeFlags_Leaf)) {

            ImGui::TreePop();
          }

          break;
        }
        case FS_ASSET_TYPE_DESCRIPTOR_BINDING: {

          fs_descriptor_binding_t *descriptor_binding = (fs_descriptor_binding_t *)asset->config;

          if (ImGui::TreeNodeEx(descriptor_binding->name, tree_node_flags | ImGuiTreeNodeFlags_Leaf)) {

            ImGui::TreePop();
          }

          break;
        }
      }
    }

    ImGui::TreePop();
  }
}
