#include <imgui/ti_im_sound_studio.h>

#include <imgui.h>

static ti_sound_t *s_sound = 0;
static ti_audio_source_t *s_source = 0;
static char s_source_path[TI_PATH_SIZE] = "static/sound/demo.wav";
static char s_asset_path[TI_PATH_SIZE] = "asset/sound/demo.pak";
static char s_loaded_path[TI_PATH_SIZE] = {0};
static char s_error_path[TI_PATH_SIZE] = {0};
static char const *s_error = 0;
static float s_position[3] = {6.0F, 0.0F, 0.0F};
static float s_listener_position[3] = {0.0F, 0.0F, 0.0F};
static float s_volume = 1.0F;
static bool s_loop = false;
static bool s_mono = true;

static int load_sound(char const *path) {
  ti_sound_t *sound = 0;
  ti_audio_source_t *source = 0;
  IPLVector3 position = {s_position[0], s_position[1], s_position[2]};

  s_error = 0;
  snprintf(s_error_path, TI_PATH_SIZE, "%s", path);

  if (audio_load(&sound, path) != 0) {
    s_error = "Could not load:";
    return 1;
  }

  if (audio_source_create(&source, sound) != 0) {
    s_error = "Could not create playback source for:";
    audio_unload(&sound);
    return 1;
  }

  audio_source_set(source, &position, s_volume, s_loop);

  audio_source_destroy(&s_source);
  audio_unload(&s_sound);

  s_sound = sound;
  s_source = source;
  snprintf(s_loaded_path, TI_PATH_SIZE, "%s", path);

  return 0;
}

void im_sound_studio_draw(void) {
  /*
  ImGui::SetNextWindowSize(ImVec2(540.0F, 460.0F), ImGuiCond_FirstUseEver);

  ImGui::PushStyleColor(ImGuiCol_WindowBg, TI_DARK_GREY);
  ImGui::PushStyleVar(ImGuiStyleVar_DisabledAlpha, 0.6F);

  if (ImGui::Begin("Sound Studio")) {

    ImGui::TextUnformatted("Source:");
    ImGui::SameLine();

    if (ImGui::InputText("##Sound Source", s_source_path, TI_PATH_SIZE)) {
      s_error = 0;
    }

    ImGui::TextUnformatted("Asset:");
    ImGui::SameLine();

    if (ImGui::InputText("##Sound Asset", s_asset_path, TI_PATH_SIZE)) {
      s_error = 0;
    }

    ImGui::TextUnformatted("Mono for 3D:");
    ImGui::SameLine();
    ImGui::Checkbox("##Sound Mono", &s_mono);

    if (ImGui::Button("Import")) {
      fs_asset_t asset = {
        .magic = TI_FS_ASSET_MAGIC,
        .type = FS_ASSET_TYPE_SOUND,
        .path = s_asset_path,
      };

      s_error = 0;
      fs_asset_create(&asset);

      if (fs_import_sound(&asset, s_source_path, s_mono) == 0) {
        fs_asset_store(&asset);
        im_filesystem_refresh();
      } else {
        s_error = "Could not import:";
        snprintf(s_error_path, TI_PATH_SIZE, "%s", s_source_path);
      }

      fs_asset_destroy(&asset);
    }

    ImGui::SameLine();
    ImGui::BeginDisabled(g_audio.device_available == 0);

    if (ImGui::Button("Load")) {
      load_sound(s_asset_path);
    }

    ImGui::SameLine();

    if (ImGui::Button("Fart")) {
      char asset_path[TI_PATH_SIZE] = {0};

      snprintf(asset_path, TI_PATH_SIZE, "asset/sound/fart_%02d.pak", rand() % 10 + 1);

      if (load_sound(asset_path) == 0) {
        IPLVector3 position = {s_position[0], s_position[1], s_position[2]};

        s_loop = false;
        audio_source_set(s_source, &position, s_volume, s_loop);
        audio_source_play(s_source);
      }
    }

    ImGui::EndDisabled();

    if (g_audio.device_available == 0) {
      ImGui::TextUnformatted("No audio device available.");
    }

    if (s_error != 0) {
      ImGui::TextWrapped("%s %s", s_error, s_error_path);

      if (s_source != 0) {
        ImGui::TextUnformatted("Loaded sound is unchanged.");
      }
    }

    if (s_sound != 0) {
      ImGui::TextWrapped("Loaded: %s", s_loaded_path);

      if (s_sound->channel_count == 2) {
        ImGui::TextWrapped("Stereo: Import as mono for 3D sound and room effects.");
      }
    }

    ImGui::BeginDisabled(s_source == 0);

    if (ImGui::Button("Play")) {
      audio_source_play(s_source);
    }

    ImGui::SameLine();

    if (ImGui::Button("Pause")) {
      audio_source_pause(s_source);
    }

    ImGui::SameLine();

    if (ImGui::Button("Stop")) {
      audio_source_stop(s_source);
    }

    ImGui::TextUnformatted("Loop:");
    ImGui::SameLine();

    bool changed = ImGui::Checkbox("##Sound Loop", &s_loop);

    ImGui::EndDisabled();

    ImGui::TextUnformatted("Volume:");
    ImGui::SameLine();
    changed |= ImGui::SliderFloat("##Sound Volume", &s_volume, 0.0F, 1.0F, "%.2f", ImGuiSliderFlags_AlwaysClamp);

    ImGui::TextUnformatted("Source Position (X, Y, Z):");
    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
    changed |= ImGui::DragFloat3("##Sound Position", s_position, 0.1F);

    if (changed && (s_source != 0)) {
      IPLVector3 position = {s_position[0], s_position[1], s_position[2]};
      audio_source_set(s_source, &position, s_volume, s_loop);
    }

    ImGui::TextUnformatted("Listener Position (X, Y, Z):");
    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);

    if (ImGui::DragFloat3("##Sound Listener Position", s_listener_position, 0.1F)) {
      IPLCoordinateSpace3 listener = {
        .right = {1.0F, 0.0F, 0.0F},
        .up = {0.0F, 1.0F, 0.0F},
        .ahead = {0.0F, 0.0F, -1.0F},
        .origin = {s_listener_position[0], s_listener_position[1], s_listener_position[2]},
      };

      audio_listener_set(&listener);
    }

    ImGui::SeparatorText("Room Demo:");
    ImGui::TextUnformatted("Enabled:");
    ImGui::SameLine();

    bool room_enabled = g_audio_demo_available != 0;

    if (ImGui::Checkbox("##Sound Room Enabled", &room_enabled)) {
      if (room_enabled) {
        ti_audio_demo_create();
      } else {
        ti_audio_demo_destroy();
      }
    }

    ImGui::TextUnformatted("Door Open:");
    ImGui::SameLine();
    ImGui::BeginDisabled(g_audio_demo_available == 0);

    bool door_open = g_audio_demo_door_open != 0;

    if (ImGui::Checkbox("##Sound Door Open", &door_open)) {
      ti_audio_demo_door(door_open);
    }

    ImGui::EndDisabled();

    ImGui::TextWrapped(g_audio_demo_available != 0 ? "Top view: +X right, +Z down." : "Layout preview (room demo off):");

    {
      ImVec2 canvas = ImGui::GetCursorScreenPos();
      ImVec2 size = ImVec2(ImGui::GetContentRegionAvail().x, 190.0F);
      float padding = 32.0F;
      float x_min = fminf(-3.0F, fminf(s_position[0], s_listener_position[0]));
      float x_max = fmaxf(9.0F, fmaxf(s_position[0], s_listener_position[0]));
      float z_min = fminf(-3.0F, fminf(s_position[2], s_listener_position[2]));
      float z_max = fmaxf(3.0F, fmaxf(s_position[2], s_listener_position[2]));
      float scale = fminf((size.x - 2.0F * padding) / (x_max - x_min), (size.y - 2.0F * padding) / (z_max - z_min));
      ImVec2 origin = ImVec2(canvas.x + size.x * 0.5F - (x_min + x_max) * scale * 0.5F,
                             canvas.y + size.y * 0.5F - (z_min + z_max) * scale * 0.5F);
      ImVec2 room_min = ImVec2(origin.x - 3.0F * scale, origin.y - 3.0F * scale);
      ImVec2 room_max = ImVec2(origin.x + 9.0F * scale, origin.y + 3.0F * scale);
      float wall_x = origin.x + 3.0F * scale;
      ImVec2 door_min = ImVec2(wall_x, origin.y - 0.75F * scale);
      ImVec2 door_max = ImVec2(wall_x, origin.y + 0.75F * scale);
      ImVec2 listener = ImVec2(origin.x + s_listener_position[0] * scale, origin.y + s_listener_position[2] * scale);
      ImVec2 source = ImVec2(origin.x + s_position[0] * scale, origin.y + s_position[2] * scale);
      ImU32 wall_color = IM_COL32(210, 215, 220, 255);
      ImU32 door_color = door_open ? IM_COL32(90, 210, 130, 255) : IM_COL32(240, 100, 90, 255);
      ImU32 listener_color = IM_COL32(100, 190, 255, 255);
      ImU32 source_color = IM_COL32(255, 195, 90, 255);
      ImDrawList *draw = ImGui::GetWindowDrawList();

      ImGui::Dummy(size);
      draw->PushClipRect(canvas, ImVec2(canvas.x + size.x, canvas.y + size.y), true);
      draw->AddRectFilled(canvas, ImVec2(canvas.x + size.x, canvas.y + size.y), IM_COL32(24, 25, 29, 255));
      draw->AddRectFilled(room_min, ImVec2(wall_x, room_max.y), IM_COL32(35, 49, 62, 255));
      draw->AddRectFilled(ImVec2(wall_x, room_min.y), room_max, IM_COL32(57, 49, 36, 255));
      draw->AddRect(room_min, room_max, wall_color, 0.0F, 2.0F);
      draw->AddLine(ImVec2(wall_x, room_min.y), door_min, wall_color, 3.0F);
      draw->AddLine(door_max, ImVec2(wall_x, room_max.y), wall_color, 3.0F);
      draw->AddCircleFilled(door_min, 3.0F, door_color);
      draw->AddCircleFilled(door_max, 3.0F, door_color);

      if (door_open == false) {
        draw->AddLine(door_min, door_max, door_color, 4.0F);
      }

      draw->AddText(ImVec2(origin.x - ImGui::CalcTextSize("Room 1").x * 0.5F, room_min.y + 7.0F), wall_color, "Room 1");
      draw->AddText(ImVec2(origin.x + 6.0F * scale - ImGui::CalcTextSize("Room 2").x * 0.5F, room_min.y + 7.0F), wall_color, "Room 2");
      draw->AddText(ImVec2(wall_x + 7.0F, door_max.y + 5.0F), door_color, "Door");
      draw->AddCircleFilled(source, 5.0F, source_color);
      draw->AddCircle(listener, 7.0F, listener_color, 0, 2.0F);
      draw->AddLine(listener, ImVec2(listener.x, listener.y - 15.0F), listener_color, 2.0F);
      draw->AddText(ImVec2(listener.x - 15.0F, listener.y + 8.0F), listener_color, "L");
      draw->AddText(ImVec2(source.x + 8.0F, source.y + 8.0F), source_color, "S");
      draw->PopClipRect();
    }

    ImGui::TextUnformatted("L: Listener   S: Source");
    ImGui::TextWrapped("Wall: X = 3. Door: X = 3, Z = 0.");
    ImGui::TextWrapped("Opening: Z -0.75 to +0.75. Y is height.");

    if (s_source == 0) {
      ImGui::TextUnformatted("State: No sound loaded");
    } else {

      switch (audio_source_state(s_source)) {
        case TI_AUDIO_STATE_STOPPED: {
          ImGui::TextUnformatted("State: Stopped");
          break;
        }
        case TI_AUDIO_STATE_PLAYING: {
          ImGui::TextUnformatted("State: Playing");
          break;
        }
        case TI_AUDIO_STATE_PAUSED: {
          ImGui::TextUnformatted("State: Paused");
          break;
        }
      }
    }
  }

  ImGui::End();
  ImGui::PopStyleVar();
  ImGui::PopStyleColor();
  */
}

void im_sound_studio_reset(void) {
  audio_source_destroy(&s_source);
  audio_unload(&s_sound);
  s_loaded_path[0] = '\0';
  s_error = 0;
}
