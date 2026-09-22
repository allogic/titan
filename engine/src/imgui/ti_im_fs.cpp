#include <ti_pch.h>

#include <imgui.h>

static void check_background_clicks(void);

static void draw_background(void);
static void draw_tree(im_file_entry_t *file_entry);
static void draw_context_menu(im_file_entry_t *file_entry);
static void draw_text_input(im_file_entry_t *file_entry);

static void collect_children(im_file_entry_t *file_entry);
static void destroy_children(im_file_entry_t *file_entry);

static uint8_t s_refresh_entries = 1;

static im_file_entry_t *s_root_file = 0;
static im_file_entry_t *s_selected_file = 0;

static fs_asset_type_t s_new_asset_type = FS_ASSET_TYPE_NONE;
static im_file_type_t s_new_file_type = IM_FILE_TYPE_NONE;
static char s_new_file_name[TI_PATH_SIZE] = {0};

void im_fs_draw(void) {
  if (s_refresh_entries) {

    s_refresh_entries = 0;

    if (s_root_file) {

      destroy_children(s_root_file);

      TI_FREE(s_root_file);
    }

    s_root_file = (im_file_entry_t *)TI_ALLOC(sizeof(im_file_entry_t), 1, 0);

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
void im_fs_refresh(void) {
  s_refresh_entries = 1;
}
void im_fs_reset(void) {
  if (s_root_file) {

    destroy_children(s_root_file);

    TI_FREE(s_root_file);
  }

  s_root_file = 0;
  s_selected_file = 0;

  s_new_asset_type = FS_ASSET_TYPE_NONE;
  s_new_file_type = IM_FILE_TYPE_NONE;
  s_new_file_name[0] = 0;

  im_inspector_select(IM_INSPECTOR_TYPE_NONE, 0);
}

static void check_background_clicks(void) {
  if (ImGui::IsWindowHovered(ImGuiHoveredFlags_AllowWhenBlockedByActiveItem) &&
      ImGui::IsMouseClicked(ImGuiMouseButton_Left) &&
      ImGui::IsAnyItemHovered() == 0) {

    s_selected_file = 0;

    s_new_asset_type = FS_ASSET_TYPE_NONE;
    s_new_file_type = IM_FILE_TYPE_NONE;
    s_new_file_name[0] = 0;

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
static void draw_tree(im_file_entry_t *file_entry) {
  ImGui::PushID(file_entry->name);

  ImGuiTreeNodeFlags tree_node_flags = ImGuiTreeNodeFlags_OpenOnArrow |
                                       ImGuiTreeNodeFlags_SpanFullWidth |
                                       ImGuiTreeNodeFlags_FramePadding;

  if (file_entry->is_directory == 0) {
    tree_node_flags |= ImGuiTreeNodeFlags_Leaf;
  }

  if (file_entry == s_selected_file) {
    tree_node_flags |= ImGuiTreeNodeFlags_Selected;
  }

  if (file_entry->is_open) {
    tree_node_flags |= ImGuiTreeNodeFlags_DefaultOpen;
  }

  if (file_entry->should_open) {

    file_entry->should_open = 0;

    ImGui::SetNextItemOpen(1);
  }

  file_entry->is_open = (uint8_t)ImGui::TreeNodeEx(file_entry->name, tree_node_flags);

  if (ImGui::IsItemClicked(ImGuiMouseButton_Left)) {

    s_selected_file = file_entry;

    s_new_asset_type = FS_ASSET_TYPE_NONE;
    s_new_file_type = IM_FILE_TYPE_NONE;
    s_new_file_name[0] = 0;

    if (file_entry->is_directory || file_entry->is_symlink) {
      im_inspector_select(IM_INSPECTOR_TYPE_NONE, 0);
    } else {
      im_inspector_select(IM_INSPECTOR_TYPE_ASSET, fs_asset(file_entry->path));
    }
  }

  draw_context_menu(file_entry);

  if (file_entry->is_open) {

    if (file_entry->is_directory) {

      if (file_entry->children == 0) {

        collect_children(file_entry);
      }
    }

    draw_text_input(file_entry);

    uint64_t child_index = 0;
    uint64_t child_count = file_entry->child_count;

    while (child_index < child_count) {

      draw_tree(&file_entry->children[child_index]);

      child_index++;
    }

    ImGui::TreePop();
  }

  ImGui::PopID();
}
static void draw_context_menu(im_file_entry_t *file_entry) {
  ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(10.0F, 10.0F));
  ImGui::PushStyleVar(ImGuiStyleVar_PopupRounding, 5.0F);
  ImGui::PushStyleVar(ImGuiStyleVar_PopupBorderSize, 1.0F);
  ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(10.0F, 5.0F));
  ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(10.0F, 5.0F));

  if (ImGui::BeginPopupContextItem("fs_context_menu")) {

    s_selected_file = file_entry;

    if (file_entry->is_directory) {

      if (ImGui::BeginMenu("Add")) {

        if (ImGui::MenuItem("Folder")) {

          s_selected_file = file_entry;

          file_entry->should_open = 1;

          s_new_asset_type = FS_ASSET_TYPE_NONE;
          s_new_file_type = IM_FILE_TYPE_FOLDER;
          s_new_file_name[0] = 0;
        }

        ImGui::Separator();

        if (ImGui::MenuItem("Model")) {

          s_selected_file = file_entry;

          file_entry->should_open = 1;

          s_new_asset_type = FS_ASSET_TYPE_MODEL;
          s_new_file_type = IM_FILE_TYPE_ASSET;
          s_new_file_name[0] = 0;
        }

        if (ImGui::MenuItem("Pipeline")) {

          s_selected_file = file_entry;

          file_entry->should_open = 1;

          s_new_asset_type = FS_ASSET_TYPE_PIPELINE;
          s_new_file_type = IM_FILE_TYPE_ASSET;
          s_new_file_name[0] = 0;
        }

        if (ImGui::MenuItem("Font")) {

          s_selected_file = file_entry;

          file_entry->should_open = 1;

          s_new_asset_type = FS_ASSET_TYPE_FONT;
          s_new_file_type = IM_FILE_TYPE_ASSET;
          s_new_file_name[0] = 0;
        }

        ImGui::Separator();

        if (ImGui::MenuItem("Descriptor Binding")) {

          s_selected_file = file_entry;

          file_entry->should_open = 1;

          s_new_asset_type = FS_ASSET_TYPE_DESCRIPTOR_BINDING;
          s_new_file_type = IM_FILE_TYPE_ASSET;
          s_new_file_name[0] = 0;
        }

        if (ImGui::MenuItem("Buffer")) {

          s_selected_file = file_entry;

          file_entry->should_open = 1;

          s_new_asset_type = FS_ASSET_TYPE_BUFFER;
          s_new_file_type = IM_FILE_TYPE_ASSET;
          s_new_file_name[0] = 0;
        }

        if (ImGui::MenuItem("Image")) {

          s_selected_file = file_entry;

          file_entry->should_open = 1;

          s_new_asset_type = FS_ASSET_TYPE_IMAGE;
          s_new_file_type = IM_FILE_TYPE_ASSET;
          s_new_file_name[0] = 0;
        }

        if (ImGui::MenuItem("Framebuffer")) {

          s_selected_file = file_entry;

          file_entry->should_open = 1;

          s_new_asset_type = FS_ASSET_TYPE_FRAMEBUFFER;
          s_new_file_type = IM_FILE_TYPE_ASSET;
          s_new_file_name[0] = 0;
        }

        ImGui::EndMenu();
      }

      ImGui::Separator();

      if (ImGui::BeginMenu("Import")) {

        if (ImGui::MenuItem("Model")) {

          fs_import_model("asset/character/veigar_greybeard.pak", "static/character/veigar_greybeard.glb");
          fs_import_model("asset/character/viktor.pak", "static/character/viktor.glb");
          fs_import_model("asset/character/demonblade_tryndamere.pak", "static/character/demonblade_tryndamere.glb");
          fs_import_model("asset/character/spirit_blossom_springs_teemo.pak", "static/character/spirit_blossom_springs_teemo.glb");
          fs_import_model("asset/character/prestige_pandemonium_shaco.pak", "static/character/prestige_pandemonium_shaco.glb");
          fs_import_model("asset/character/old_god_malphite.pak", "static/character/old_god_malphite.glb");
          fs_import_model("asset/character/high_noon_locke.pak", "static/character/high_noon_locke.glb");
          fs_import_model("asset/character/immortalized_legend_kaisa.pak", "static/character/immortalized_legend_kaisa.glb");
          fs_import_model("asset/character/old_god_mordekaiser.pak", "static/character/old_god_mordekaiser.glb");
          fs_import_model("asset/character/steel_legion_garen.pak", "static/character/steel_legion_garen.glb");
          fs_import_model("asset/character/pulsefire_caitlyn.pak", "static/character/pulsefire_caitlyn.glb");
          fs_import_model("asset/character/grand_reckoning_sion.pak", "static/character/grand_reckoning_sion.glb");
          fs_import_model("asset/character/marauder_kalista.pak", "static/character/marauder_kalista.glb");

          im_fs_refresh();
        }

        if (ImGui::MenuItem("Pipeline")) {

          fs_import_pipeline(FS_PIPELINE_TYPE_DFLT, "asset/shader/standard/brdf.pak", "static/shader/standard/brdf.vert", "static/shader/standard/brdf.frag");
          fs_import_pipeline(FS_PIPELINE_TYPE_DFLT, "asset/shader/debug/line.pak", "static/shader/debug/line.vert", "static/shader/debug/line.frag");

          im_fs_refresh();
        }

        if (ImGui::MenuItem("Font")) {

          fs_import_font("asset/font/commit_mono_latin_400_normal.pak", "static/font/commit_mono_latin_400_normal.ttf");
          fs_import_font("asset/font/material_symbols_rounded_fill.pak", "static/font/material_symbols_rounded_fill.ttf");

          im_fs_refresh();
        }

        if (ImGui::MenuItem("Texture")) {

          // TODO
        }

        ImGui::EndMenu();
      }

      ImGui::Separator();
    }

    if (ImGui::MenuItem("Refresh")) {

      im_fs_refresh();
    }

    ImGui::Separator();

    if (ImGui::MenuItem("Rename")) {

      // TODO
    }

    ImGui::Separator();

    if (ImGui::MenuItem("Remove")) {

      if (s_selected_file) {

        fs_remove_recursive(g_fs, file_entry->path);

        s_selected_file = 0;

        im_inspector_select(IM_INSPECTOR_TYPE_NONE, 0);

        im_fs_refresh();
      }
    }

    ImGui::EndPopup();
  }

  ImGui::PopStyleVar(5);
}
static void draw_text_input(im_file_entry_t *file_entry) {
  if ((file_entry == s_selected_file) && (s_new_file_type != FS_ASSET_TYPE_NONE)) {

    float width = ImGui::GetContentRegionAvail().x;

    ImGui::SetNextItemWidth(width);
    ImGui::SetKeyboardFocusHere();

    if (ImGui::InputText("##file_name", s_new_file_name, TI_PATH_SIZE, ImGuiInputTextFlags_EnterReturnsTrue)) {

      char file_path[TI_PATH_SIZE] = {0};

      snprintf(file_path, TI_PATH_SIZE, "%s/%s", file_entry->path, s_new_file_name);

      fs_file *file = 0;

      switch (s_new_file_type) {

        case IM_FILE_TYPE_FOLDER: {

          if (fs_mkdir_recursive(g_fs, file_path, FS_READ | FS_WRITE) == FS_SUCCESS) {

            // TODO
          }

          break;
        }
        case IM_FILE_TYPE_ASSET: {

          if (fs_file_open(g_fs, file_path, FS_WRITE, &file) == FS_SUCCESS) {

            fs_asset_t asset = {
              .magic = TI_FS_ASSET_MAGIC,
              .type = s_new_asset_type,
            };

            fs_asset_create(&asset);
            fs_asset_store(&asset, file);
            fs_asset_destroy(&asset);

            fs_file_close(file);
          }

          break;
        }
      }

      s_new_asset_type = FS_ASSET_TYPE_NONE;
      s_new_file_type = IM_FILE_TYPE_NONE;
      s_new_file_name[0] = 0;
    }

    if (ImGui::IsKeyPressed(ImGuiKey_Escape)) {

      s_new_asset_type = FS_ASSET_TYPE_NONE;
      s_new_file_type = IM_FILE_TYPE_NONE;
      s_new_file_name[0] = 0;
    }
  }

  im_fs_refresh();
}

static void collect_children(im_file_entry_t *file_entry) {
  fs_iterator *dir_it = 0;

  dir_it = fs_first(g_fs, file_entry->path, FS_READ);

  while (dir_it) {

    file_entry->child_count++;

    dir_it = fs_next(dir_it);
  }

  file_entry->children = (im_file_entry_t *)TI_ALLOC(sizeof(im_file_entry_t) * file_entry->child_count, 1, 0);

  dir_it = fs_first(g_fs, file_entry->path, FS_READ);

  uint64_t child_index = 0;
  uint64_t child_count = file_entry->child_count;

  while (dir_it && (child_index < child_count)) {

    im_file_entry_t *child = &file_entry->children[child_index];

    fs_path_append(child->path, TI_PATH_SIZE, file_entry->path, strlen(file_entry->path), dir_it->pName, strlen(dir_it->pName));

    strcpy(child->name, dir_it->pName);

    child->size = dir_it->info.size;
    child->child_count = 0;
    child->is_directory = dir_it->info.directory;
    child->is_symlink = dir_it->info.symlink;
    child->is_open = 0;
    child->parent = file_entry;
    child->children = 0;

    dir_it = fs_next(dir_it);

    child_index++;
  }
}
static void destroy_children(im_file_entry_t *file_entry) {
  uint64_t child_index = 0;
  uint64_t child_count = file_entry->child_count;

  while (child_index < child_count) {

    destroy_children(&file_entry->children[child_index]);

    child_index++;
  }

  TI_FREE(file_entry->children);
}
