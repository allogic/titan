#include <imgui/ti_im_output.h>

#include <imgui.h>

static void draw_background(void);

static im_output_message_t s_messages[TI_OUTPUT_MESSAGE_CAPACITY] = {0};

static uint64_t s_message_count = 0;

void im_output_draw(void) {
  if (ImGui::Begin("Output", 0, ImGuiWindowFlags_NoDecoration)) {

    draw_background();

    if (ImGui::Button("Clear")) {
      im_output_clear();
    }

    if (ImGui::BeginChild("Messages")) {

      ImGui::PushTextWrapPos(0.0F);

      uint64_t message_index = 0;
      uint64_t message_count = s_message_count;

      while (message_index < message_count) {

        ImGui::Text("%s", s_messages[message_index].text);

        message_index++;
      }

      ImGui::PopTextWrapPos();
    }

    ImGui::EndChild();
  }

  ImGui::End();
}
void im_output_push(char const *format, ...) {
  va_list args;
  va_start(args, format);
  int32_t length = vsnprintf(0, 0, format, args);
  va_end(args);

  uint64_t size = length + 1;
  im_output_message_t message = {0};
  message.text = (char *)TI_ALLOC(size, 0, 0);

  va_start(args, format);
  vsnprintf(message.text, size, format, args);
  va_end(args);

  if (s_message_count == TI_OUTPUT_MESSAGE_CAPACITY) {

    s_message_count--;

    TI_FREE(s_messages[s_message_count].text);
  }

  memmove(s_messages + 1, s_messages, sizeof(im_output_message_t) * s_message_count);

  s_messages[0] = message;
  s_message_count++;
}
void im_output_clear(void) {
  while (s_message_count > 0) {

    s_message_count--;

    TI_FREE(s_messages[s_message_count].text);

    s_messages[s_message_count].text = 0;
  }
}
void im_output_reset(void) {
  while (s_message_count > 0) {

    s_message_count--;

    TI_FREE(s_messages[s_message_count].text);

    s_messages[s_message_count].text = 0;
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
