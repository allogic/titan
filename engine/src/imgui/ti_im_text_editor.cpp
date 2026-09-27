#include <ti_pch.h>
#include <ti_clang.h>

#include <imgui.h>
#include <TextEditor.h>

static void draw_background(void);

static TextEditor s_text_editor = {};

void im_text_editor_setup(char const *source_code) {
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
    IM_COL32(30, 30, 30, 255),    // background
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
  s_text_editor.SetText(source_code);
  s_text_editor.SetFocus();
}
void im_text_editor_draw(void) {
  ImGui::Begin("Text Editor", 0, ImGuiWindowFlags_NoDecoration);

  draw_background();

  if (ImGui::Button("Compile and Run")) {

    ti_clang_compile(s_text_editor.GetText().c_str());
  }

  // ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0F, 0.0F));
  // ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0F);
  // ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize, 0.0F);
  s_text_editor.Render("##TextEditor", ImVec2(0.0F, 0.0F), ImGuiChildFlags_None, ImGuiWindowFlags_NoDecoration);
  // ImGui::PopStyleVar(3);

  ImGui::End();
}
void im_text_editor_refresh(void) {
  // TODO
}

void im_text_editor_reset(void) {
  s_text_editor.ClearText();
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
