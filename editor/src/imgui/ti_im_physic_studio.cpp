#include <imgui/ti_im_physic_studio.h>

#include <imgui.h>

static ti_physic_demo_item_t s_item = {
  .shape = TI_PHYSIC_DEMO_SHAPE_BOX,
  .position = {0.0F, 5.0F, 0.0F},
  .rotation = {0.0F, 0.0F, 0.0F, 1.0F},
  .size = {1.0F, 1.0F, 1.0F},
  .radius = 0.5F,
  .height = 1.0F,
};

void im_physic_studio_draw(void) {
  ImGui::PushStyleColor(ImGuiCol_WindowBg, TI_DARK_GREY);

  if (ImGui::Begin("Physics Studio")) {

    bool active = g_ph_physic_demo.active != 0;

    ImGui::TextUnformatted("Demo:");
    ImGui::SameLine();

    if (ImGui::Checkbox("##Physics Demo", &active)) {
      if (active) {
        ti_physic_demo_create();
      } else {
        ti_physic_demo_destroy();
      }
    }

    ImGui::BeginDisabled(g_ph_physic_demo.active == 0);

    if (ImGui::Button(g_ph_world.running ? "Pause" : "Play")) {
      g_ph_world.running = g_ph_world.running == 0;
      g_ph_world.accumulator = 0.0F;
    }

    ImGui::SameLine();
    ImGui::BeginDisabled(g_ph_world.running != 0);

    if (ImGui::Button("Step")) {
      ph_world_step();
    }

    ImGui::EndDisabled();
    ImGui::SameLine();

    if (ImGui::Button("Reset")) {
      ti_physic_demo_reset();
    }

    ImGui::EndDisabled();

    JPH_Vec3 gravity;
    JPH_PhysicsSystem_GetGravity(g_ph_world.system, &gravity);

    ImGui::TextUnformatted("Gravity (X, Y, Z):");
    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);

    if (ImGui::DragFloat3("##Physics Gravity", &gravity.x, 0.1F)) {
      JPH_PhysicsSystem_SetGravity(g_ph_world.system, &gravity);

      uint32_t item_index = 0;

      while (item_index < g_ph_physic_demo.item_count) {
        JPH_BodyInterface_ActivateBody(g_ph_world.body_interface, g_ph_physic_demo.items[item_index].body);
        item_index++;
      }
    }

    ImGui::Text("Bodies: %u", JPH_PhysicsSystem_GetNumBodies(g_ph_world.system));
    ImGui::Text("Active: %u", JPH_PhysicsSystem_GetNumActiveBodies(g_ph_world.system, JPH_BodyType_Rigid));
    ImGui::Text("Step: %.4f s", TI_PHYSIC_TIME_STEP);

    ImGui::SeparatorText("Room:");
    ImGui::BeginDisabled(g_ph_physic_demo.active == 0);

    JPH_Vec3 room_size = {10.0F, 10.0F, 10.0F};
    JPH_Quat room_rotation = {0.0F, 0.0F, 0.0F, 1.0F};
    JPH_Vec3 room_angles = {0.0F, 0.0F, 0.0F};

    if (g_ph_physic_demo.active != 0) {
      room_size = g_ph_physic_demo.room_size;
      room_rotation = g_ph_physic_demo.room_rotation;
    }

    ImGui::TextUnformatted("Size (Width, Height, Depth):");
    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
    bool room_changed = ImGui::DragFloat3("##Physics Room Size", &room_size.x, 0.1F, 0.1F, 100.0F, "%.2f", ImGuiSliderFlags_AlwaysClamp);

    JPH_Quat_GetEulerAngles(&room_rotation, &room_angles);
    room_angles.x = rad_to_deg(room_angles.x);
    room_angles.y = rad_to_deg(room_angles.y);
    room_angles.z = rad_to_deg(room_angles.z);

    ImGui::TextUnformatted("Rotation (X, Y, Z; degrees):");
    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);

    if (ImGui::DragFloat3("##Physics Room Rotation", &room_angles.x, 1.0F)) {
      room_angles.x = deg_to_rad(room_angles.x);
      room_angles.y = deg_to_rad(room_angles.y);
      room_angles.z = deg_to_rad(room_angles.z);
      JPH_Quat_FromEulerAngles(&room_angles, &room_rotation);
      room_changed = true;
    }

    if (room_changed) {
      ti_physic_demo_room_edit(&room_size, &room_rotation);
      room_size = g_ph_physic_demo.room_size;
    }

    ImGui::EndDisabled();
    ImGui::TextWrapped("Room resizes move items inside. Size and rotation must fit.");

    float item_size_max = fmaxf(0.2F, fmaxf(room_size.x, fmaxf(room_size.y, room_size.z)) - 0.2F);

    ImGui::SeparatorText("Add Item:");

    char const *shape_names[] = {"Cube", "Sphere", "Capsule", "Cylinder"};

    ImGui::TextUnformatted("Shape:");
    ImGui::SameLine();
    ImGui::Combo("##Physics Shape", (int32_t *)&s_item.shape, shape_names, 4);

    ImGui::TextUnformatted("Position (X, Y, Z):");
    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
    ImGui::DragFloat3("##Physics Spawn Position", &s_item.position.x, 0.1F);

    JPH_Vec3 spawn_angles;
    JPH_Quat_GetEulerAngles(&s_item.rotation, &spawn_angles);
    spawn_angles.x = rad_to_deg(spawn_angles.x);
    spawn_angles.y = rad_to_deg(spawn_angles.y);
    spawn_angles.z = rad_to_deg(spawn_angles.z);

    ImGui::TextUnformatted("Rotation (X, Y, Z; degrees):");
    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);

    if (ImGui::DragFloat3("##Physics Spawn Rotation", &spawn_angles.x, 1.0F)) {
      spawn_angles.x = deg_to_rad(spawn_angles.x);
      spawn_angles.y = deg_to_rad(spawn_angles.y);
      spawn_angles.z = deg_to_rad(spawn_angles.z);
      JPH_Quat_FromEulerAngles(&spawn_angles, &s_item.rotation);
    }

    switch (s_item.shape) {
      case TI_PHYSIC_DEMO_SHAPE_BOX: {
        ImGui::TextUnformatted("Size (Width, Height, Depth):");
        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
        ImGui::DragFloat3("##Physics Cube Size", &s_item.size.x, 0.05F, 0.1F, item_size_max, "%.2f", ImGuiSliderFlags_AlwaysClamp);
        break;
      }
      case TI_PHYSIC_DEMO_SHAPE_SPHERE: {
        ImGui::TextUnformatted("Radius:");
        ImGui::SameLine();
        ImGui::DragFloat("##Physics Sphere Radius", &s_item.radius, 0.05F, 0.05F, item_size_max * 0.5F, "%.2f", ImGuiSliderFlags_AlwaysClamp);
        break;
      }
      case TI_PHYSIC_DEMO_SHAPE_CAPSULE:
      case TI_PHYSIC_DEMO_SHAPE_CYLINDER: {
        float radius_max = item_size_max * 0.5F;

        if (s_item.shape == TI_PHYSIC_DEMO_SHAPE_CAPSULE) {
          s_item.height = fminf(s_item.height, item_size_max - 0.1F);
          radius_max = fmaxf(0.05F, (item_size_max - s_item.height) * 0.5F);
          s_item.radius = fminf(s_item.radius, radius_max);
        }

        ImGui::TextUnformatted("Radius:");
        ImGui::SameLine();
        ImGui::DragFloat("##Physics Radius", &s_item.radius, 0.05F, 0.05F, radius_max, "%.2f", ImGuiSliderFlags_AlwaysClamp);

        float height_max = s_item.shape == TI_PHYSIC_DEMO_SHAPE_CAPSULE ? fmaxf(0.1F, item_size_max - 2.0F * s_item.radius) : item_size_max;

        ImGui::TextUnformatted(s_item.shape == TI_PHYSIC_DEMO_SHAPE_CAPSULE ? "Cylinder Height (excluding caps):" : "Height:");
        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
        ImGui::DragFloat("##Physics Height", &s_item.height, 0.05F, 0.1F, height_max, "%.2f", ImGuiSliderFlags_AlwaysClamp);
        break;
      }
    }

    ImGui::BeginDisabled(g_ph_physic_demo.active == 0 || g_ph_physic_demo.item_count >= TI_PHYSIC_DEMO_ITEM_LIMIT);

    if (ImGui::Button("Add")) {
      uint32_t item_count = g_ph_physic_demo.item_count;
      ti_physic_demo_add(&s_item);

      if (g_ph_physic_demo.item_count > item_count) {
        s_item.position = g_ph_physic_demo.items[g_ph_physic_demo.item_count - 1].position;
      }
    }

    ImGui::EndDisabled();
    ImGui::SeparatorText("Items:");
    ImGui::Text("Count: %u / %u", g_ph_physic_demo.item_count, TI_PHYSIC_DEMO_ITEM_LIMIT);
    ImGui::TextWrapped("Item edits are used by Reset.");

    uint32_t item_index = 0;

    while (item_index < g_ph_physic_demo.item_count) {
      ti_physic_demo_item_t *item = &g_ph_physic_demo.items[item_index];
      bool removed = false;

      ImGui::PushID(item->body);

      if (ImGui::TreeNodeEx("##Physics Item", ImGuiTreeNodeFlags_SpanFullWidth, "%s %u:", shape_names[item->shape], item_index + 1)) {
        ti_physic_demo_item_t settings = *item;
        JPH_BodyInterface_GetPositionAndRotation(g_ph_world.body_interface, item->body, &settings.position, &settings.rotation);

        ImGui::TextUnformatted("Position (X, Y, Z):");
        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);

        bool changed = ImGui::DragFloat3("##Physics Item Position", &settings.position.x, 0.1F);

        JPH_Vec3 angles;
        JPH_Quat_GetEulerAngles(&settings.rotation, &angles);
        angles.x = rad_to_deg(angles.x);
        angles.y = rad_to_deg(angles.y);
        angles.z = rad_to_deg(angles.z);

        ImGui::TextUnformatted("Rotation (X, Y, Z; degrees):");
        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);

        if (ImGui::DragFloat3("##Physics Item Rotation", &angles.x, 1.0F)) {
          angles.x = deg_to_rad(angles.x);
          angles.y = deg_to_rad(angles.y);
          angles.z = deg_to_rad(angles.z);
          JPH_Quat_FromEulerAngles(&angles, &settings.rotation);
          changed = true;
        }

        switch (item->shape) {
          case TI_PHYSIC_DEMO_SHAPE_BOX: {
            ImGui::TextUnformatted("Size (Width, Height, Depth):");
            ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
            changed |= ImGui::DragFloat3("##Physics Item Cube Size", &settings.size.x, 0.05F, 0.1F, item_size_max, "%.2f", ImGuiSliderFlags_AlwaysClamp);
            break;
          }
          case TI_PHYSIC_DEMO_SHAPE_SPHERE: {
            ImGui::TextUnformatted("Radius:");
            ImGui::SameLine();
            changed |= ImGui::DragFloat("##Physics Item Sphere Radius", &settings.radius, 0.05F, 0.05F, item_size_max * 0.5F, "%.2f", ImGuiSliderFlags_AlwaysClamp);
            break;
          }
          case TI_PHYSIC_DEMO_SHAPE_CAPSULE:
          case TI_PHYSIC_DEMO_SHAPE_CYLINDER: {
            float radius_max = settings.shape == TI_PHYSIC_DEMO_SHAPE_CAPSULE ? fmaxf(0.05F, (item_size_max - settings.height) * 0.5F) : item_size_max * 0.5F;

            ImGui::TextUnformatted("Radius:");
            ImGui::SameLine();
            changed |= ImGui::DragFloat("##Physics Item Radius", &settings.radius, 0.05F, 0.05F, radius_max, "%.2f", ImGuiSliderFlags_AlwaysClamp);

            float height_max = settings.shape == TI_PHYSIC_DEMO_SHAPE_CAPSULE ? fmaxf(0.1F, item_size_max - 2.0F * settings.radius) : item_size_max;

            ImGui::TextUnformatted(settings.shape == TI_PHYSIC_DEMO_SHAPE_CAPSULE ? "Cylinder Height (excluding caps):" : "Height:");
            ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
            changed |= ImGui::DragFloat("##Physics Item Height", &settings.height, 0.05F, 0.1F, height_max, "%.2f", ImGuiSliderFlags_AlwaysClamp);
            break;
          }
        }

        if (changed) {
          ti_physic_demo_edit(item, &settings);
        }

        if (ImGui::Button("Remove")) {
          ti_physic_demo_remove(item_index);
          removed = true;
        }

        ImGui::TreePop();
      }

      ImGui::PopID();

      if (removed == false) {
        item_index++;
      }
    }
  }

  ImGui::End();
  ImGui::PopStyleColor();
}
