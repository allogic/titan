#include <ti_pch.h>

#include <imgui.h>
#include <TextEditor.h>

static void draw_background(void);

static TextEditor s_text_editor;

void im_text_editor_draw(void) {
  if (ImGui::Begin("Text Editor", 0, ImGuiWindowFlags_NoDecoration)) {

    draw_background();

    s_text_editor.Render("##TextEditor");

    ImGui::End();
  }
}
void im_text_editor_refresh(void) {
  s_text_editor.SetLanguage(TextEditor::Language::C());
  s_text_editor.SetPalette(TextEditor::GetDarkPalette());
  s_text_editor.SetTabSize(2);

  s_text_editor.SetText("int main(void) {\n  return 0;\n}\n");
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
