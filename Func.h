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
bool ShowMenu = true;
bool ESP = true;
bool Sonar = false;
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

void FoundEnemy(EntityList& entity_list) {
  if (!config::InfoString) return;
  std::lock_guard<std::mutex> lock(entity_list.buffer_mtx);

  std::ostringstream allinfo;

  Entity* best_enemy = nullptr;
  float min_center_dist_sq = FLT_MAX;
  float min_3d_dist = FLT_MAX;
  Vector2 best_screen_pos;
  bool best_on_screen = false;
  const Vector3& local_pos = entity_list.local_player_data.position;
  int local_team = entity_list.local_player_data.team;

  for (const auto& ent : entity_list.GetCurrentEntities()) {
    if (!ent.IsValid()) continue;
    if (!ent.IsEnemy(local_team)) continue;
    if (ent.data.dormant) continue;

    Vector3 head_pos = {ent.data.position.x, ent.data.position.y,
                        ent.data.position.z + ent.data.head_height};
    float dist = ent.DistanceTo(local_pos);
    Vector2 screen_pos;
    bool on_screen =
        entity_list.view_matrix.WorldToScreen(head_pos, screen_pos);
    float center_dist_sq = FLT_MAX;

    if (on_screen) {
      float dx = screen_pos.x - entity_list.view_matrix.screen_center.x;
      float dy = screen_pos.y - entity_list.view_matrix.screen_center.y;
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
            << "Index: " << best_enemy->index << "\n"
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
          << "Health: " << entity_list.local_player_data.health << " HP\n"
          << "Team: " << entity_list.local_player_data.team << "\n"
          << "Pos: (" << local_pos.x << ", " << local_pos.y << ", "
          << local_pos.z << ")\n"
          << "Flags: " << entity_list.local_player_data.flags << "\n"
          << "FOV: " << entity_list.local_player_data.fov << "\n\n";
  global::infos = allinfo.str();
  return;
}

void SonarRun(EntityList& entity_list) {
  if (!config::Sonar) return;
  std::lock_guard<std::mutex> lock(entity_list.buffer_mtx);

  Sonar::SoundParams newParams;
  newParams.active = false;

  Entity* best_enemy = nullptr;
  float min_center_dist_sq = FLT_MAX;
  float min_3d_dist = FLT_MAX;
  Vector2 best_screen_pos;
  bool best_on_screen = false;
  const Vector3& local_pos = entity_list.local_player_data.position;
  int local_team = entity_list.local_player_data.team;

  for (const auto& ent : entity_list.GetCurrentEntities()) {
    if (!ent.IsValid()) continue;
    if (!ent.IsEnemy(local_team)) continue;
    if (ent.data.dormant) continue;
    Vector3 head_pos = {ent.data.position.x, ent.data.position.y,
                        ent.data.position.z + ent.data.head_height};
    float dist = ent.DistanceTo(local_pos);
    Vector2 screen_pos;
    bool on_screen =
        entity_list.view_matrix.WorldToScreen(head_pos, screen_pos);
    float center_dist_sq = FLT_MAX;

    if (on_screen) {
      float dx = screen_pos.x - entity_list.view_matrix.screen_center.x;
      float dy = screen_pos.y - entity_list.view_matrix.screen_center.y;
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
      float dx = best_screen_pos.x - entity_list.view_matrix.screen_center.x;
      float dy = best_screen_pos.y - entity_list.view_matrix.screen_center.y;
      float centerDist = sqrtf(dx * dx + dy * dy);
      float playerDist = min_3d_dist;
      newParams.frequency =
          500 + (3500 * (1 - std::clamp(playerDist / 1000.0f, 0.0f, 1.0f)));
      newParams.interval =
          30 + (950 * (centerDist / entity_list.view_matrix.screen_center.x));
      newParams.active = true;
    }
  }
  {
    std::lock_guard<std::mutex> lock(Sonar::soundMutex);
    Sonar::currentParams = newParams;
  }
  return;
}

void ESPRun(EntityList& entity_list) {
  if (!config::ESP) return;
  const Vector3& local_pos = entity_list.local_player_data.position;
  int local_team = entity_list.local_player_data.team;

  for (const auto& ent : entity_list.GetCurrentEntities()) {
    if (!ent.IsValid()) continue;
    if (!ent.IsEnemy(local_team)) continue;
    float dist = ent.DistanceTo(local_pos);
    Vector2 screen_pos;
    bool on_screen =
        entity_list.view_matrix.WorldToScreen(ent.data.position, screen_pos);
    if (on_screen) {
      std::ostringstream playerInfo;
      playerInfo << "^ a dude\n"  // maybe name?
                 << "Health: " << ent.data.health << "\n"
                 << "Dist:" << dist << "\n";
      if (ent.data.dormant)
        DrawNewText(screen_pos.x, screen_pos.y, &Grey,
                    playerInfo.str().c_str());
      else
        DrawNewText(screen_pos.x, screen_pos.y, &White,
                    playerInfo.str().c_str());
    }
  }
  return;
}