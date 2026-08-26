#include <ti_pch.h>

#include <imgui.h>

static void fs_check_background_clicks(void);

static void fs_draw_controls(void);
static void fs_draw_tree(void);

static fs_entry *s_files = 0;

static uint64_t s_file_count = 0;

char g_curr_dir[TI_PATH_SIZE] = "";

uint64_t g_selected_file = 0;

uint8_t g_refresh = 0;

void fs_draw(void) {
  ImGui::Begin("Filesystem");

  fs_check_background_clicks();

  fs_draw_controls();
  fs_draw_tree();

  ImGui::End();
}
void fs_reset(void) {
  if (s_files) {
    TI_FREE(s_files);
  }

  s_files = 0;
  s_file_count = 0;

  g_selected_file = 0;
  g_refresh = 0;
}

static void fs_check_background_clicks(void) {
  if (ImGui::IsWindowHovered(ImGuiHoveredFlags_AllowWhenBlockedByActiveItem) &&
      ImGui::IsMouseClicked(ImGuiMouseButton_Left) &&
      ImGui::IsAnyItemHovered() == false) {

    g_selected_file = 0;
  }
}

static void fs_draw_controls(void) {
  if (ImGui::Button("Import Models")) {
    adb_import_model("asset/character/veigar_greybeard", ROOT_DIR "/static/model/veigar_greybeard.glb");
    adb_import_model("asset/character/viktor", ROOT_DIR "/static/model/viktor.glb");
    adb_import_model("asset/character/demonblade_tryndamere", ROOT_DIR "/static/model/demonblade_tryndamere.glb");
    adb_import_model("asset/character/spirit_blossom_springs_teemo", ROOT_DIR "/static/model/spirit_blossom_springs_teemo.glb");
    adb_import_model("asset/character/prestige_pandemonium_shaco", ROOT_DIR "/static/model/prestige_pandemonium_shaco.glb");
    adb_import_model("asset/character/old_god_malphite", ROOT_DIR "/static/model/old_god_malphite.glb");
    adb_import_model("asset/character/high_noon_locke", ROOT_DIR "/static/model/high_noon_locke.glb");
    adb_import_model("asset/character/immortalized_legend_kaisa", ROOT_DIR "/static/model/immortalized_legend_kaisa.glb");
    adb_import_model("asset/character/old_god_mordekaiser", ROOT_DIR "/static/model/old_god_mordekaiser.glb");
    adb_import_model("asset/character/steel_legion_garen", ROOT_DIR "/static/model/steel_legion_garen.glb");
    adb_import_model("asset/character/pulsefire_caitlyn", ROOT_DIR "/static/model/pulsefire_caitlyn.glb");
    adb_import_model("asset/character/grand_reckoning_sion", ROOT_DIR "/static/model/grand_reckoning_sion.glb");
    adb_import_model("asset/character/marauder_kalista", ROOT_DIR "/static/model/marauder_kalista.glb");
  }

  ImGui::SameLine();

  if (ImGui::Button("Import Shader")) {
    adb_import_dflt_shader("asset/shader/debug/line", ROOT_DIR "/static/shader/debug/line.vert", ROOT_DIR "/static/shader/debug/line.frag");
  }
}
static void fs_draw_tree(void) {
  if (g_refresh) {

    g_refresh = 0;

    if (s_files) {
      TI_FREE(s_files);
    }

    fs_iterator *dir_it = fs_first(g_fs, g_curr_dir, FS_READ);

    while (dir_it) {

      dir_it = fs_next(dir_it);
    }
  }

  uint64_t file_index = 0;
  uint64_t file_count = s_file_count;

  while (file_index < file_count) {

    fs_entry *file = &s_files[file_index];

    ImGui::Text(file->name);

    file_index++;
  }
}
