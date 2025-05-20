#pragma once

#include <mutex>

#include "../../CSSx64.h"

namespace Sonar {
struct SoundParams {
  float frequency = 1000.0f;  // 默认频率
  float interval = 1000.0f;   // 默认间隔(ms)
  bool active = false;        // 是否激活
};

std::atomic<bool> running{true};
std::mutex soundMutex;
SoundParams currentParams;

inline void SoundThread() {
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

namespace Misc {

inline void FoundEnemy(EntityList& entityList) {
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

inline void SonarRun(EntityList& entityList) {
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

inline void PitchIndicator(EntityList& entityList) {
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
}  // namespace Misc