#pragma once

#include "../../CSSx64.h"

namespace ESP {

static std::unordered_map<uintptr_t, float> g_dormantStartTimes;

void ESPRun(EntityList& entityList) {
  if (!config::ESP) return;

  const Vector3& local_pos = entityList.local_player_data.position;
  int local_team = entityList.local_player_data.team;

  auto now = std::chrono::steady_clock::now();
  float currentTime =
      std::chrono::duration<float>(now.time_since_epoch()).count();

  for (const auto& ent : entityList.GetCurrentEntities()) {
    if (!ent.IsValid()) continue;
    if (!ent.IsEnemy(local_team)) continue;

    if (ent.data.dormant) {
      if (g_dormantStartTimes.find(ent.address) == g_dormantStartTimes.end()) {
        g_dormantStartTimes[ent.address] = currentTime;
      }

      if (currentTime - g_dormantStartTimes[ent.address] >
          config::ESPDormantTime) {
        continue;
      }
    } else {
      g_dormantStartTimes.erase(ent.address);
    }

    float dist = ent.DistanceTo(local_pos);
    Vector2 screen_pos;
    bool on_screen =
        entityList.view_matrix.WorldToScreen(ent.data.position, screen_pos);
    if (on_screen) {
      Vector3 head_pos = {ent.data.position.x, ent.data.position.y,
                          ent.data.position.z + ent.data.head_height + 12.f};
      Vector2 head_screen_pos;
      entityList.view_matrix.WorldToScreen(head_pos, head_screen_pos);

      if (config::ESPInfo) {
        std::ostringstream playerInfo;
        playerInfo << "^\n"
                   << "Health: " << ent.data.health << "\n"
                   << "Dist:" << dist << "\n";
        if (ent.data.dormant)
          DrawStrokeText(screen_pos.x, screen_pos.y, &Grey,
                         playerInfo.str().c_str());
        else
          DrawStrokeText(screen_pos.x, screen_pos.y, &White,
                         playerInfo.str().c_str());
      }

      if (config::ESPBox) {
        if (ent.data.dormant)
          DrawEspBox2D(screen_pos, head_screen_pos, &Grey, 1);
        else
          DrawEspBox2D(screen_pos, head_screen_pos, &Green, 1);
      }
      if (config::ESPName)
        DrawNameTag(screen_pos, head_screen_pos, (char*)ent.name.c_str());
    }
  }
}

}  // namespace ESP