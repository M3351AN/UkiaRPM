#pragma once
#include <sstream>

#include "CSSx64.h"

struct RGB {
  int r;
  int g;
  int b;

  // Conversion function from RGB to COLORREF
  operator COLORREF() const { return RGB(r, g, b); }
};

namespace config {
bool ShowMenu = false;
bool ESP = true;
bool Sonar = false;
bool PitchIndicator = true;
bool InfoString = true;
}  // namespace config

namespace Sonar {
struct SoundParams {
  float frequency = 1000.0f;  // 默认频率
  float interval = 1000.0f;   // 默认间隔(ms)
  bool active = false;        // 是否激活
};

std::atomic<bool> running{true};
std::mutex soundMutex;
SoundParams currentParams;

void SoundThread() {
  auto lastBeep = std::chrono::steady_clock::now();

  while (running) {
    std::this_thread::sleep_for(std::chrono::milliseconds(1));

    SoundParams localParams;
    {
      std::lock_guard<std::mutex> lock(soundMutex);
      localParams = currentParams;
    }

    if (localParams.active) {
      auto now = std::chrono::steady_clock::now();
      auto elapsed =
          std::chrono::duration_cast<std::chrono::milliseconds>(now - lastBeep)
              .count();

      if (elapsed >= localParams.interval) {
        Beep(static_cast<DWORD>(localParams.frequency), 35);
        lastBeep = now;
      }
    }
  }
}
}  // namespace Sonar

namespace Func {
void DrawMenu() {
  float textoffset;
  int randomresult = (rand() % 15) + 1;
  if (randomresult % 2 == 0)
    textoffset = randomresult * -0.1f;
  else
    textoffset = randomresult * 0.1f;
  ImGuiIO& io = ImGui::GetIO();
  ImGuiStyle& style = ImGui::GetStyle();
  if (!config::ShowMenu) return;
  const ImVec2 vecScreenSize = io.DisplaySize;

  ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, ImVec2(720, 365));

  ImGui::SetNextWindowPos(
      ImVec2(io.DisplaySize.x / 2.f, io.DisplaySize.y / 2.f), ImGuiCond_Once,
      ImVec2(0.5f, 0.5f));
  ImGui::SetNextWindowSize(ImVec2(720, 365), ImGuiCond_Always);
  ImGui::Begin(XorStr("TokiKiri"), NULL,
               ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
                   ImGuiWindowFlags_NoScrollbar |
                   ImGuiWindowFlags_NoScrollWithMouse |
                   ImGuiWindowFlags_NoCollapse);

  const ImVec2 vecMenuPos = ImGui::GetWindowPos();
  const ImVec2 vecMenuSize = ImGui::GetWindowSize();
  ImDrawList* pDrawList = ImGui::GetWindowDrawList();

  ImGui::BeginTabBar(XorStr("Tabs"));
#ifdef _DEBUG
  if (ImGui::BeginTabItem(XorStr("semirage"))) {
    ImGui::EndTabItem();
  }
#endif
  if (ImGui::BeginTabItem(XorStr("legit"))) {
    ImVec2 child_size =
        ImVec2((ImGui::GetColumnWidth() - (style.ItemSpacing.x * 2)) / 3,
               ImGui::GetWindowHeight() - 10.f -
                   (ImGui::GetCursorPosY() + style.ItemInnerSpacing.y * 2));
    auto childBegin = ImGui::GetCursorPosY();
    ImGui::BeginChild(
        XorStr("Aim assist##legit"),
        ImVec2(child_size.x,
               (child_size.y - (style.ItemInnerSpacing.y)) * .6f));
    {}
    ImGui::EndChild();

    ImGui::BeginChild(
        XorStr("Silent"),
        ImVec2(child_size.x,
               (child_size.y - (style.ItemInnerSpacing.y)) * .4f));
    {}
    ImGui::EndChild();

    ImGui::SameLine();
    ImGui::SetCursorPosY(childBegin);
    ImGui::BeginChild(XorStr("Triggerbot"), child_size);
    {}
    ImGui::EndChild();
    ImGui::SameLine();
    ImGui::SetCursorPosY(childBegin);
    ImGui::BeginChild(XorStr("Others##aimbot"), child_size);
    {}
    ImGui::EndChild();

    ImGui::EndTabItem();
  }

  if (ImGui::BeginTabItem(XorStr("visuals"))) {
    ImVec2 child_size =
        ImVec2((ImGui::GetColumnWidth() - (style.ItemSpacing.x * 2)) / 3,
               ImGui::GetWindowHeight() - 10.f -
                   (ImGui::GetCursorPosY() + style.ItemInnerSpacing.y * 2));
    static float flOverlayChildSize = 0.f;

    ImGui::BeginChild(XorStr("ESP"), child_size);
    {
      ImGui::PushStyleVar(ImGuiStyleVar_FramePadding,
                          ImVec2(style.FramePadding.x, 0));

      ImGui::Checkbox(XorStr("ESP"), &config::ESP);

      ImGui::PopStyleVar();

      flOverlayChildSize = ImGui::GetCursorPosY() + style.ItemSpacing.y;
    }
    ImGui::EndChild();

    ImGui::SameLine();

    ImGui::BeginChild(XorStr("World"), child_size);
    {
      ImGui::PushStyleVar(ImGuiStyleVar_FramePadding,
                          ImVec2(style.FramePadding.x, 0));

      ImGui::PopStyleVar();
    }
    ImGui::EndChild();

    ImGui::SameLine();

    ImGui::BeginChild(XorStr("Others"), child_size);
    {
      // ImGui::Checkbox(XorStr("hitmark"), &C_GET(bool, Vars.bHitMark));
      ImGui::PushStyleVar(ImGuiStyleVar_FramePadding,
                          ImVec2(style.FramePadding.x, 0));

      ImGui::PopStyleVar();
    }
    ImGui::EndChild();

    ImGui::EndTabItem();
  }

  if (ImGui::BeginTabItem(XorStr("miscs"))) {
    ImVec2 child_size =
        ImVec2((ImGui::GetColumnWidth() - (style.ItemSpacing.x * 2)) / 3,
               ImGui::GetWindowHeight() - 10.f -
                   (ImGui::GetCursorPosY() + style.ItemInnerSpacing.y * 2));

    ImGui::BeginChild(XorStr("Globals##miscs"), child_size);
    {
      ImGui::PushStyleVar(ImGuiStyleVar_FramePadding,
                          ImVec2(style.FramePadding.x, 0));
      ImGui::Checkbox(XorStr("PitchIndicator"), &config::PitchIndicator);
      ImGui::Checkbox(XorStr("Sonar"), &config::Sonar);
      ImGui::PopStyleVar();
    }
    ImGui::EndChild();

    ImGui::SameLine();

    ImGui::BeginChild(XorStr("Misc##MiscChild"), child_size);
    {
      ImGui::PushStyleVar(ImGuiStyleVar_FramePadding,
                          ImVec2(style.FramePadding.x, 0));
      ImGui::Checkbox(XorStr("Enemy info"), &config::InfoString);
      ImGui::PopStyleVar();
    }
    ImGui::EndChild();

    ImGui::SameLine();

    ImGui::BeginChild(XorStr("Movement"), child_size);
    {
      ImGui::PushStyleVar(ImGuiStyleVar_FramePadding,
                          ImVec2(style.FramePadding.x, 0));

      ImGui::PopStyleVar();
    }
    ImGui::EndChild();

    ImGui::EndTabItem();
  }

  if (ImGui::BeginTabItem(XorStr("settings"))) {
    ImVec2 child_size =
        ImVec2((ImGui::GetColumnWidth() - (style.ItemSpacing.x * 2)) / 3,
               ImGui::GetWindowHeight() - 10.f -
                   (ImGui::GetCursorPosY() + style.ItemInnerSpacing.y * 2));

    ImGui::BeginChild(XorStr("Info"), child_size);
    {
      ImGui::LabelText(__DATE__, XorStr("last build:"));
      ImGui::LabelText(std::to_string(CS_VERSION).c_str(),
                       XorStr("build for version:"));
      ImGui::LabelText(std::to_string(global::gameVersion).c_str(),
                       XorStr("current version:"));
#ifdef _MSC_VER
#ifndef __clang__
      ImGui::LabelText(std::to_string(_MSC_VER).c_str(),
                       XorStr("complier: MSVC"));
#endif
#endif
#ifdef __clang__
      ImGui::LabelText(std::to_string(__clang_major__).c_str(),
                       XorStr("complier: Clang"));
#endif
      ImGui::LabelText(getenv("USERNAME"), XorStr("copy licenced to:"));

      ImGui::LabelText(std::format("0x{:X}", Memory::clientAddress).c_str(),
                       "client base:");
      ImGui::LabelText(std::format("0x{:X}", Memory::engineAddress).c_str(),
                       "engine base:");
    }
    ImGui::EndChild();

    ImGui::SameLine();

    ImGui::BeginChild(XorStr("Menu"), child_size);
    {
      static int nSelectedColor = 0;

      ImGui::PushStyleVar(ImGuiStyleVar_FramePadding,
                          ImVec2(ImGui::GetStyle().FramePadding.x, 0));

      ImGui::PopStyleVar();
    }
    ImGui::EndChild();

    ImGui::SameLine();

    ImGui::BeginChild(XorStr("Configs"), child_size);
    {
      if (ImGui::Button(XorStr("Unhook"))) {
        global::isRunning = false;
      }

      ImGui::Text(XorStr("Menukey [DEL]"));
    }
    ImGui::EndChild();

    ImGui::EndTabItem();
  }

  ImGui::EndTabBar();
  ImGui::SetCursorPos(ImVec2{style.ItemSpacing.x, 350});
  ImGui::Text(
      XorStr("UkiaRPM for Counter-Strike Source %s"),
      UkiaData::strHWID.substr(UkiaData::strHWID.length() - 16).c_str());

  ImGui::End();
  ImVec2 mousePos = ImGui::GetMousePos();
  float interpolationFactorX = 0.035f;
  float interpolationFactorY = 0.015f;
  ImVec2 center = ImVec2(vecMenuPos.x - 300, vecMenuPos.y + 25);
  float radius = 40;
  ImVec2 factor = ImVec2(interpolationFactorX * (mousePos.x - vecMenuPos.x),
                         interpolationFactorY * (mousePos.y - vecMenuPos.y));
  ImVec2 interpolatedPos{center.x - factor.x, center.y - factor.y};

  // 计算 interpolatedPos 到圆心的距离
  float dx = interpolatedPos.x - center.x;
  float dy = interpolatedPos.y - center.y;
  float distance = sqrt(dx * dx + dy * dy);

  // 如果距离超过半径，则调整 interpolatedPos
  if (distance > radius) {
    float scale = radius / distance;
    factor.x = -dx * scale;
    factor.y = -dy * scale;
    interpolatedPos = ImVec2{center.x - factor.x, center.y - factor.y};
  }

  const ImVec2 vecOverlayPadding = ImVec2(90.f, 30.f);
  ImGui::PopStyleVar();
  ImGui::SetNextWindowPos(interpolatedPos);
  ImGui::Begin(XorStr("moe"), nullptr,
               ImGuiWindowFlags_NoMouseInputs |
                   ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove |
                   ImGuiWindowFlags_NoFocusOnAppearing |
                   ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar |
                   ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoCollapse |
                   ImGuiWindowFlags_NoScrollbar |
                   ImGuiWindowFlags_::ImGuiWindowFlags_NoBringToFrontOnFocus |
                   ImGuiWindowFlags_::ImGuiWindowFlags_NoBackground |
                   ImGuiWindowFlags_::ImGuiWindowFlags_NoNavFocus |
                   ImGuiWindowFlags_::ImGuiWindowFlags_NoNav);
  {
    ImGui::SetCursorPos(ImVec2{50, 25});
    ImGui::Image((void*)global::Shigure, ImVec2{258, 349});

    const ImVec2 vecWindPos = ImGui::GetWindowPos();
    const ImVec2 vecWindSize = ImGui::GetWindowSize();

    ImDrawList* pDrawList = ImGui::GetWindowDrawList();
    ImGui::SetCursorPos(ImVec2{20, 40});
    ImVec4 vecBox = {vecWindPos.x + vecOverlayPadding.x + 50.f,
                     vecWindPos.y + 15,
                     vecWindPos.x + vecWindSize.x - vecOverlayPadding.x + 50.f,
                     vecWindPos.y + vecWindSize.y - vecOverlayPadding.y};
    vecBox.x -= factor.x * 0.15f;
    vecBox.z -= factor.x * 0.15f;
    vecBox.y -= factor.y * 0.15f;
    vecBox.w -= factor.y * 0.15f;

    ImGui::SetCursorPos(ImVec2{15 + textoffset, 250 - textoffset} + factor);
    ImGui::SetCursorPos(ImVec2{15, 250} + factor * 0.3351f);
    ImGui::TextColored(ImColor(245, 245, 245, 245), "NOT FOR SELLING!!");
    ImGui::SetCursorPos(ImVec2{15, 295} + factor * 0.1337f);
    ImGui::TextColored(ImColor(245, 245, 245, 245), "UkiaRPM");
    ImGui::SetCursorPos(ImVec2{15, 335} + factor * 0.2024f);
    ImGui::TextColored(ImColor(245, 245, 245, 245), "Build: %s %s", __DATE__,
                       __TIME__);
    ImGui::SetCursorPos(ImVec2{15 - textoffset, 250 + textoffset} + factor);
  }
  ImGui::End();
}

void FoundEnemy(EntityList& entityList) {
  if (!config::InfoString) return;
  std::lock_guard<std::mutex> lock(entityList.buffer_mtx);

  std::ostringstream allinfo;

  Entity* best_enemy = nullptr;
  float min_center_dist_sq = FLT_MAX;
  float min_3d_dist = FLT_MAX;
  Vector2 best_screen_pos;
  bool best_on_screen = false;
  const Vector3& local_pos = entityList.local_player_data.position;
  int local_team = entityList.local_player_data.team;

  for (const auto& ent : entityList.GetCurrentEntities()) {
    if (!ent.IsValid()) continue;
    if (!ent.IsEnemy(local_team)) continue;
    if (ent.data.dormant) continue;

    Vector3 head_pos = {ent.data.position.x, ent.data.position.y,
                        ent.data.position.z + ent.data.head_height};
    float dist = ent.DistanceTo(local_pos);
    Vector2 screen_pos;
    bool on_screen = entityList.view_matrix.WorldToScreen(head_pos, screen_pos);
    float center_dist_sq = FLT_MAX;

    if (on_screen) {
      float dx = screen_pos.x - entityList.view_matrix.screen_center.x;
      float dy = screen_pos.y - entityList.view_matrix.screen_center.y;
      center_dist_sq = dx * dx + dy * dy;
    }

    if (center_dist_sq < min_center_dist_sq) {
      min_center_dist_sq = center_dist_sq;
      min_3d_dist = dist;
      best_enemy = const_cast<Entity*>(&ent);
      best_on_screen = on_screen;
      if (on_screen) best_screen_pos = screen_pos;
    } else if (center_dist_sq == min_center_dist_sq && dist < min_3d_dist) {
      min_3d_dist = dist;
      best_enemy = const_cast<Entity*>(&ent);
      best_on_screen = on_screen;
      if (on_screen) best_screen_pos = screen_pos;
    }
  }

  if (best_enemy) {
    allinfo << "Enemy:\n"
            << "Name: " << best_enemy->name << "\n"
            << "Index: " << best_enemy->index << "\n"
            << "Address: 0x" << std::hex << best_enemy->address << "\n"
            << "Health: " << best_enemy->data.health << " HP\n"
            << "Team: " << best_enemy->data.team << "\n"
            << "Pos: (" << best_enemy->data.position.x << ", "
            << best_enemy->data.position.y << ", "
            << best_enemy->data.position.z << ")\n";

    if (best_on_screen) {
      allinfo << "Screen pos: (" << best_screen_pos.x << ", "
              << best_screen_pos.y << ")\n";
    } else {
      allinfo << "Enemy OOF\n";
    }
  } else {
    allinfo << "No valid enemy\n";
  }

  allinfo << "Local:\n"
          << "Health: " << entityList.local_player_data.health << " HP\n"
          << "Team: " << entityList.local_player_data.team << "\n"
          << "Pos: (" << local_pos.x << ", " << local_pos.y << ", "
          << local_pos.z << ")\n"
          << "Flags: " << entityList.local_player_data.flags << "\n"
          << "FOV: " << entityList.local_player_data.fov << "\n\n";
  global::infos = allinfo.str();
  return;
}

void SonarRun(EntityList& entityList) {
  if (!config::Sonar) return;
  std::lock_guard<std::mutex> lock(entityList.buffer_mtx);

  Sonar::SoundParams newParams;
  newParams.active = false;

  Entity* best_enemy = nullptr;
  float min_center_dist_sq = FLT_MAX;
  float min_3d_dist = FLT_MAX;
  Vector2 best_screen_pos;
  bool best_on_screen = false;
  const Vector3& local_pos = entityList.local_player_data.position;
  int local_team = entityList.local_player_data.team;

  for (const auto& ent : entityList.GetCurrentEntities()) {
    if (!ent.IsValid()) continue;
    if (!ent.IsEnemy(local_team)) continue;
    if (ent.data.dormant) continue;
    Vector3 head_pos = {ent.data.position.x, ent.data.position.y,
                        ent.data.position.z + ent.data.head_height};
    float dist = ent.DistanceTo(local_pos);
    Vector2 screen_pos;
    bool on_screen = entityList.view_matrix.WorldToScreen(head_pos, screen_pos);
    float center_dist_sq = FLT_MAX;

    if (on_screen) {
      float dx = screen_pos.x - entityList.view_matrix.screen_center.x;
      float dy = screen_pos.y - entityList.view_matrix.screen_center.y;
      center_dist_sq = dx * dx + dy * dy;
    }

    if (center_dist_sq < min_center_dist_sq) {
      min_center_dist_sq = center_dist_sq;
      min_3d_dist = dist;
      best_enemy = const_cast<Entity*>(&ent);
      best_on_screen = on_screen;
      if (on_screen) {
        best_screen_pos = screen_pos;
      }
    } else if (center_dist_sq == min_center_dist_sq && dist < min_3d_dist) {
      min_3d_dist = dist;
      best_enemy = const_cast<Entity*>(&ent);
      best_on_screen = on_screen;
      if (on_screen) {
        best_screen_pos = screen_pos;
      }
    }
  }
  if (best_enemy) {
    if (best_on_screen) {
      float dx = best_screen_pos.x - entityList.view_matrix.screen_center.x;
      float dy = best_screen_pos.y - entityList.view_matrix.screen_center.y;
      float centerDist = sqrtf(dx * dx + dy * dy);
      float playerDist = min_3d_dist;
      newParams.frequency =
          500 + (3500 * (1 - std::clamp(playerDist / 1000.0f, 0.0f, 1.0f)));
      newParams.interval =
          30 + (950 * (centerDist / entityList.view_matrix.screen_center.x));
      newParams.active = true;
    }
  }
  {
    std::lock_guard<std::mutex> lock(Sonar::soundMutex);
    Sonar::currentParams = newParams;
  }
  return;
}

void ESPRun(EntityList& entityList) {
  if (!config::ESP) return;
  const Vector3& local_pos = entityList.local_player_data.position;
  int local_team = entityList.local_player_data.team;

  for (const auto& ent : entityList.GetCurrentEntities()) {
    if (!ent.IsValid()) continue;
    if (!ent.IsEnemy(local_team)) continue;
    float dist = ent.DistanceTo(local_pos);
    Vector2 screen_pos;
    bool on_screen =
        entityList.view_matrix.WorldToScreen(ent.data.position, screen_pos);
    if (on_screen) {
      Vector3 head_pos = {ent.data.position.x, ent.data.position.y,
                          ent.data.position.z + ent.data.head_height + 12.f};
      Vector2 head_screen_pos;
      entityList.view_matrix.WorldToScreen(head_pos, head_screen_pos);
      std::ostringstream playerInfo;
      playerInfo << "^ " << ent.name << "\n"  // maybe name?
                 << "Health: " << ent.data.health << "\n"
                 << "Dist:" << dist << "\n";
      if (ent.data.dormant) {
        DrawNewText(screen_pos.x, screen_pos.y, &Grey,
                    playerInfo.str().c_str());
        DrawNewText(head_screen_pos.x, head_screen_pos.y, &Grey, "v");
      }

      else {
        DrawNewText(screen_pos.x, screen_pos.y, &White,
                    playerInfo.str().c_str());
        DrawNewText(head_screen_pos.x, head_screen_pos.y, &White, "v");
      }
    }
  }
  return;
}

void PitchIndicator(EntityList& entityList) {
  if (!config::PitchIndicator) return;
  if (entityList.local_player_address == 0) return;

  int centerX = static_cast<int>(entityList.view_matrix.screen_center.x);
  int centerY = static_cast<int>(entityList.view_matrix.screen_center.y);

  float pitch = entityList.local_player_data.viewangles.x;

  float pitchRadians = pitch * (3.14159265f / 180.0f);

  float actualFOV = static_cast<float>(entityList.local_player_data.fov);

  float verticalOffsetRatio =
      (pitchRadians / (actualFOV * (3.14159265f / 180.0f)));

  float verticalOffset = verticalOffsetRatio * global::screenSize.y;

  int dynamicY = centerY - static_cast<int>(verticalOffset);

  RGBA lineColor = {0, 255, 0, 255};
  int lineLength = 15;
  int thickness = 1;
  DrawNewText(centerX - 4, centerY - 2, &lineColor, "^");
  DrawLine(centerX - lineLength, dynamicY, centerX + lineLength - 20, dynamicY,
           &lineColor, thickness);
  DrawLine(centerX - lineLength + 20, dynamicY, centerX + lineLength, dynamicY,
           &lineColor, thickness);
  DrawNewText(centerX + lineLength + 5, dynamicY, &lineColor,
              std::to_string(pitch).c_str());
}
}  // namespace Func
void RenderFunctions(EntityList& entityList) {
  Func::ESPRun(entityList);
  Func::PitchIndicator(entityList);
  Func::SonarRun(entityList);
  Func::DrawMenu();
  DrawNewText(10, 10, &White, XorStr("UkiaRPM for Counter-Strike Source"));
  DrawNewText(10, 100, &White, global::infos.c_str());
  DrawNewText(
      10, ImGui::GetIO().DisplaySize.y - 20, &White,
      UkiaData::strHWID.substr(UkiaData::strHWID.length() - 16).c_str());
}
void MemoryFunctions(EntityList& entityList) { Func::FoundEnemy(entityList); }
