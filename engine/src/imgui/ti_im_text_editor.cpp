#include <ti_pch.h>
#include <ti_clang.h>

#include <imgui.h>
#include <TextEditor.h>

static void draw_background(void);

static TextEditor s_text_editor = {};

static char s_asset_path[TI_PATH_SIZE] = {};
static fs_asset_t *s_asset = 0;

void im_text_editor_setup(void) {
  TextEditor::Palette palette = {{
    IM_COL32(224, 224, 224, 255), // text
    IM_COL32(197, 134, 192, 255), // keyword
    IM_COL32(90, 179, 155, 255),  // declaration
    IM_COL32(181, 206, 168, 255), // number
    IM_COL32(206, 145, 120, 255), // string
    IM_COL32(255, 255, 153, 255), // punctuation
    IM_COL32(64, 192, 128, 255),  // preprocessor
    IM_COL32(156, 220, 254, 255), // identifier
    IM_COL32(79, 193, 255, 255),  // known identifier
    IM_COL32(106, 153, 85, 255),  // comment
    TI_DARK_GREY,                 // background
    IM_COL32(224, 224, 224, 255), // cursor
    IM_COL32(32, 96, 160, 255),   // selection
    IM_COL32(80, 80, 80, 255),    // whitespace
    IM_COL32(70, 70, 70, 255),    // matchingBracketBackground
    IM_COL32(140, 140, 140, 255), // matchingBracketActive
    IM_COL32(246, 222, 36, 255),  // matchingBracketLevel1
    IM_COL32(66, 120, 198, 255),  // matchingBracketLevel2
    IM_COL32(213, 96, 213, 255),  // matchingBracketLevel3
    IM_COL32(198, 8, 32, 255),    // matchingBracketError
    IM_COL32(128, 128, 144, 255), // line number
    IM_COL32(224, 224, 240, 255), // current line number
    IM_COL32(255, 255, 255, 8),   // current line highlight
    IM_COL32(255, 255, 255, 16)   // current line highlight border
  }};

  s_text_editor.SetLanguage(TextEditor::Language::C());
  s_text_editor.SetPalette(palette);
  s_text_editor.SetTabSize(2);
  s_text_editor.SetText("");
  s_text_editor.SetShowMiniMapEnabled(true);
  s_text_editor.SetShowScrollbarMiniMapEnabled(true);
  s_text_editor.SetShowCurrentLineHighlightEnabled(true);
  s_text_editor.SetShowMatchingBrackets(true);
  s_text_editor.SetShowSpacesEnabled(true);
  s_text_editor.SetShowTabsEnabled(true);
}
void im_text_editor_open(char const *asset_path) {
  if (s_asset) {

    fs_asset_destroy(s_asset);

    TI_FREE(s_asset);
  }

  strcpy(s_asset_path, asset_path);

  s_asset = (fs_asset_t *)TI_ALLOC(sizeof(fs_asset_t), 0, 0);

  s_asset->path = s_asset_path;

  fs_asset_load(s_asset);

  switch (s_asset->type) {

    case FS_ASSET_TYPE_SCRIPT: {

      fs_script_t *script = (fs_script_t *)s_asset->instance;

      // TODO: find clean way without the need of if statements..

      if (script->buffer_size) {

        s_text_editor.SetText((char *)script->buffer);
      }

      break;
    }
  }
}
void im_text_editor_close(void) {
  fs_asset_destroy(s_asset);

  if (s_asset) {
    TI_FREE(s_asset);
  }
}
void im_text_editor_draw(void) {
  ImGui::Begin("Text Editor", 0, ImGuiWindowFlags_NoDecoration);

  draw_background();

  if (ImGui::Button("Save")) {

    fs_script_t *script = (fs_script_t *)s_asset->instance;

    if (script->buffer) {

      TI_FREE(script->buffer);
    }

    std::string source = s_text_editor.GetText();

    script->buffer_size = source.size() + 1;
    script->buffer = TI_ALLOC(script->buffer_size, 0, 0);

    memcpy(script->buffer, source.c_str(), script->buffer_size);

    ((char *)script->buffer)[script->buffer_size - 1] = 0;

    fs_asset_store(s_asset);
  }

  ImGui::SameLine();

  if (ImGui::Button("Compile")) {

    ti_clang_compile(s_text_editor.GetText().c_str());
  }

  ImGui::PushStyleColor(ImGuiCol_NavCursor, IM_COL32(0, 0, 0, 0));
  ImGui::PushStyleColor(ImGuiCol_ScrollbarBg, IM_COL32(0, 0, 0, 0));
  ImGui::PushStyleColor(ImGuiCol_ScrollbarGrab, IM_COL32(0, 0, 0, 0));
  ImGui::PushStyleColor(ImGuiCol_ScrollbarGrabHovered, IM_COL32(0, 0, 0, 0));
  ImGui::PushStyleColor(ImGuiCol_ScrollbarGrabActive, IM_COL32(0, 0, 0, 0));

  ImGui::PushStyleVar(ImGuiStyleVar_ScrollbarSize, 0.0F);

  s_text_editor.Render("##TextEditor", ImVec2(0.0F, 0.0F), ImGuiChildFlags_None, ImGuiWindowFlags_None);

  ImGui::PopStyleVar(1);
  ImGui::PopStyleColor(5);

  ImGui::End();
}
void im_text_editor_refresh(void) {
  // TODO
}
void im_text_editor_reset(void) {
  if (s_asset) {

    fs_asset_destroy(s_asset);

    TI_FREE(s_asset);
  }

  s_text_editor.ClearText();

  s_asset_path[0] = 0;
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
