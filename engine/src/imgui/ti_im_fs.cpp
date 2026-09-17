#include <ti_pch.h>

#include <imgui.h>

static void check_background_clicks(void);

static void draw_background(void);
static void draw_tree(fs_entry_t *file);
static void draw_context_menu(fs_entry_t *file);
static void draw_text_input(fs_entry_t *file);

static void collect_children(fs_entry_t *file);
static void destroy_children(fs_entry_t *file);

static uint8_t s_refresh_entries = 1;

static fs_entry_t *s_root_file = 0;
static fs_entry_t *s_selected_file = 0;

static fs_file_type_t s_new_file_type = FS_FILE_TYPE_NONE;

static char s_new_file_name[TI_PATH_SIZE] = {0};

void fs_draw(void) {
  if (s_refresh_entries) {

    s_refresh_entries = 0;

    if (s_root_file) {

      destroy_children(s_root_file);

      TI_FREE(s_root_file);
    }

    s_root_file = (fs_entry_t *)TI_ALLOC(sizeof(fs_entry_t), 1, 0);

    strcpy(s_root_file->path, "asset");
    strcpy(s_root_file->name, "asset://");

    s_root_file->is_directory = 1;
    s_root_file->is_open = 1;

    collect_children(s_root_file);
  }

  ImGui::Begin("Filesystem", 0, ImGuiWindowFlags_NoDecoration);

  check_background_clicks();

  draw_background();

  ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(5.0F, 3.0F));
  ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(5.0F, 0.0F));

  draw_tree(s_root_file);

  ImGui::PopStyleVar(2);

  ImGui::End();
}
void fs_refresh(void) {
  s_refresh_entries = 1;
}
void fs_reset(void) {
  if (s_root_file) {

    destroy_children(s_root_file);

    TI_FREE(s_root_file);
  }

  s_root_file = 0;
  s_selected_file = 0;

  s_new_file_type = FS_FILE_TYPE_NONE;
  s_new_file_name[0] = 0;

  inspector_select(INSPECTOR_TYPE_NONE, 0);
}

static void check_background_clicks(void) {
  if (ImGui::IsWindowHovered(ImGuiHoveredFlags_AllowWhenBlockedByActiveItem) &&
      ImGui::IsMouseClicked(ImGuiMouseButton_Left) &&
      ImGui::IsAnyItemHovered() == 0) {

    s_selected_file = 0;

    s_new_file_type = FS_FILE_TYPE_NONE;
    s_new_file_name[0] = 0;

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
static void draw_tree(fs_entry_t *file) {
  ImGui::PushID(file->name);

  ImGuiTreeNodeFlags tree_node_flags = ImGuiTreeNodeFlags_OpenOnArrow |
                                       ImGuiTreeNodeFlags_SpanFullWidth |
                                       ImGuiTreeNodeFlags_FramePadding;

  if (file->is_directory == 0) {
    tree_node_flags |= ImGuiTreeNodeFlags_Leaf;
  }

  if (file == s_selected_file) {
    tree_node_flags |= ImGuiTreeNodeFlags_Selected;
  }

  if (file->is_open) {
    tree_node_flags |= ImGuiTreeNodeFlags_DefaultOpen;
  }

  if (file->should_open) {

    file->should_open = 0;

    ImGui::SetNextItemOpen(1);
  }

  file->is_open = (uint8_t)ImGui::TreeNodeEx(file->name, tree_node_flags);

  if (ImGui::IsItemClicked(ImGuiMouseButton_Left)) {

    s_selected_file = file;

    s_new_file_type = FS_FILE_TYPE_NONE;
    s_new_file_name[0] = 0;

    if ((file->is_directory == 0) && (file->is_symlink == 0)) {

      inspector_select(INSPECTOR_TYPE_FILE, fs_asset(file->path));
    }
  }

  draw_context_menu(file);

  if (file->is_open) {

    if (file->is_directory) {

      if (file->children == 0) {

        collect_children(file);
      }
    }

    draw_text_input(file);

    uint64_t child_index = 0;
    uint64_t child_count = file->child_count;

    while (child_index < child_count) {

      draw_tree(&file->children[child_index]);

      child_index++;
    }

    ImGui::TreePop();
  }

  ImGui::PopID();
}
static void draw_context_menu(fs_entry_t *file) {
  ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(10.0F, 10.0F));
  ImGui::PushStyleVar(ImGuiStyleVar_PopupRounding, 5.0F);
  ImGui::PushStyleVar(ImGuiStyleVar_PopupBorderSize, 1.0F);
  ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(10.0F, 5.0F));
  ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(10.0F, 5.0F));

  if (ImGui::BeginPopupContextItem("fs_context_menu")) {

    s_selected_file = file;

    if (file->is_directory) {

      if (ImGui::BeginMenu("Add")) {

        if (ImGui::MenuItem("File")) {

          s_selected_file = file;

          file->should_open = 1;

          s_new_file_type = FS_FILE_TYPE_FILE;
          s_new_file_name[0] = 0;
        }

        if (ImGui::MenuItem("Folder")) {

          s_selected_file = file;

          file->should_open = 1;

          s_new_file_type = FS_FILE_TYPE_DIRECTORY;
          s_new_file_name[0] = 0;
        }

        ImGui::Separator();

        if (ImGui::MenuItem("Model")) {

          // TODO
        }

        if (ImGui::MenuItem("Pipeline")) {

          // TODO
        }

        if (ImGui::MenuItem("Font")) {

          // TODO
        }

        ImGui::Separator();

        if (ImGui::MenuItem("Descriptor Binding")) {

          // TODO
        }

        ImGui::EndMenu();
      }

      ImGui::Separator();

      if (ImGui::BeginMenu("Import")) {

        if (ImGui::MenuItem("Model")) {

          import_model("asset/character/veigar_greybeard.pak", "static/character/veigar_greybeard.glb");
          import_model("asset/character/viktor.pak", "static/character/viktor.glb");
          import_model("asset/character/demonblade_tryndamere.pak", "static/character/demonblade_tryndamere.glb");
          import_model("asset/character/spirit_blossom_springs_teemo.pak", "static/character/spirit_blossom_springs_teemo.glb");
          import_model("asset/character/prestige_pandemonium_shaco.pak", "static/character/prestige_pandemonium_shaco.glb");
          import_model("asset/character/old_god_malphite.pak", "static/character/old_god_malphite.glb");
          import_model("asset/character/high_noon_locke.pak", "static/character/high_noon_locke.glb");
          import_model("asset/character/immortalized_legend_kaisa.pak", "static/character/immortalized_legend_kaisa.glb");
          import_model("asset/character/old_god_mordekaiser.pak", "static/character/old_god_mordekaiser.glb");
          import_model("asset/character/steel_legion_garen.pak", "static/character/steel_legion_garen.glb");
          import_model("asset/character/pulsefire_caitlyn.pak", "static/character/pulsefire_caitlyn.glb");
          import_model("asset/character/grand_reckoning_sion.pak", "static/character/grand_reckoning_sion.glb");
          import_model("asset/character/marauder_kalista.pak", "static/character/marauder_kalista.glb");

          fs_refresh();
        }

        if (ImGui::MenuItem("Pipeline")) {

          import_pipeline(VK_PIPELINE_TYPE_DFLT, "asset/shader/standard/brdf.pak", "static/shader/standard/brdf.vert", "static/shader/standard/brdf.frag");
          import_pipeline(VK_PIPELINE_TYPE_DFLT, "asset/shader/debug/line.pak", "static/shader/debug/line.vert", "static/shader/debug/line.frag");

          fs_refresh();
        }

        if (ImGui::MenuItem("Font")) {

          import_font("asset/font/commit_mono_latin_400_normal.pak", "static/font/commit_mono_latin_400_normal.ttf");
          import_font("asset/font/material_symbols_rounded_fill.pak", "static/font/material_symbols_rounded_fill.ttf");

          fs_refresh();
        }

        ImGui::EndMenu();
      }

      ImGui::Separator();
    }

    if (ImGui::MenuItem("Refresh")) {

      fs_refresh();
    }

    ImGui::Separator();

    if (ImGui::MenuItem("Rename")) {

      // TODO
    }

    ImGui::Separator();

    if (ImGui::MenuItem("Remove")) {

      if (s_selected_file) {

        fs_remove_recursive(g_fs, file->path);

        s_selected_file = 0;

        inspector_select(INSPECTOR_TYPE_NONE, 0);

        fs_refresh();
      }
    }

    ImGui::EndPopup();
  }

  ImGui::PopStyleVar(5);
}
static void draw_text_input(fs_entry_t *file) {
  if ((file == s_selected_file) &&
      (s_new_file_type != FS_FILE_TYPE_NONE)) {

    float width = ImGui::GetContentRegionAvail().x;

    ImGui::SetNextItemWidth(width);
    ImGui::SetKeyboardFocusHere();

    if (ImGui::InputText("##file_name", s_new_file_name, TI_PATH_SIZE, ImGuiInputTextFlags_EnterReturnsTrue)) {

      // create_entry();

      s_new_file_type = FS_FILE_TYPE_NONE;
      s_new_file_name[0] = 0;
    }

    if (ImGui::IsKeyPressed(ImGuiKey_Escape)) {

      s_new_file_type = FS_FILE_TYPE_NONE;
      s_new_file_name[0] = 0;
    }
  }
}

static void collect_children(fs_entry_t *file) {
  fs_iterator *dir_it = 0;

  dir_it = fs_first(g_fs, file->path, FS_READ);

  while (dir_it) {

    file->child_count++;

    dir_it = fs_next(dir_it);
  }

  file->children = (fs_entry_t *)TI_ALLOC(sizeof(fs_entry_t) * file->child_count, 1, 0);

  dir_it = fs_first(g_fs, file->path, FS_READ);

  uint64_t child_index = 0;
  uint64_t child_count = file->child_count;

  while (dir_it && (child_index < child_count)) {

    fs_entry_t *child = &file->children[child_index];

    fs_path_append(child->path, TI_PATH_SIZE, file->path, strlen(file->path), dir_it->pName, strlen(dir_it->pName));

    strcpy(child->name, dir_it->pName);

    child->size = dir_it->info.size;
    child->child_count = 0;
    child->is_directory = dir_it->info.directory;
    child->is_symlink = dir_it->info.symlink;
    child->is_open = 0;
    child->parent = file;
    child->children = 0;

    dir_it = fs_next(dir_it);

    child_index++;
  }
}
static void destroy_children(fs_entry_t *file) {
  uint64_t child_index = 0;
  uint64_t child_count = file->child_count;

  while (child_index < child_count) {

    destroy_children(&file->children[child_index]);

    child_index++;
  }

  TI_FREE(file->children);
}
