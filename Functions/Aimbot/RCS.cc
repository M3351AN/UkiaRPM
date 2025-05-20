#include "RCS.h"
namespace RCS {

int RCSBullet = 1;

void RCSRun(EntityList& entityList) noexcept {
  if (false) return;
  static Vector3 OldPunch;
  if (entityList.local_player_data.m_iShotsFired > 1) {
    Vector3 delta =
        (OldPunch - (entityList.local_player_data.punchangle * 1.f));

    int MouseX = (int)(delta.y / (3.f * 0.022f) * config::RCSScale.x);
    int MouseY =
        (int)(delta.x / (3.f /*LocalPlayer.Client.Sensitivity*/ * 0.022f) *
              config::RCSScale.y);

    if ((GetAsyncKeyState(VK_LBUTTON) & 0x8000))
      mouse_event(MOUSEEVENTF_MOVE, MouseX, MouseY, 0, 0);

    OldPunch = entityList.local_player_data.punchangle * 1.f;
  } else {
    OldPunch = Vector3{0, 0, 0};
  }
}
}  // namespace RCS
