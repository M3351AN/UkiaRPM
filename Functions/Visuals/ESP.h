#pragma once

#include "../../CSSx64.h"

namespace ESP {

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
}  // namespace ESP