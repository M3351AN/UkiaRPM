#include "Func.h"

#include <sstream>

#include "../Overlay.h"
#include "Aimbot/RCS.h"
#include "Miscs/Miscs.h"
#include "Visuals/ESP.h"
#include "Visuals/Radar.h"
#include "menu.h"

void RenderFunctions(EntityList& entityList) {
  ESP::ESPRun(entityList);
  Radar::RadarRun(entityList);
  Misc::PitchIndicator(entityList);
  Menu::DrawMenu();
  DrawNewText(10, 10, &White, XorStr("UkiaRPM for Counter-Strike Source"));
  DrawNewText(10, 100, &White, global::infos.c_str());
  DrawNewText(
      10, ImGui::GetIO().DisplaySize.y - 20, &White,
      UkiaData::strHWID.substr(UkiaData::strHWID.length() - 16).c_str());
}
void ViewFunctions(EntityList& entityList) {
  Misc::FastStop(entityList);
  RCS::RCSRun(entityList);
  Misc::SonarRun(entityList);
}
void MemoryFunctions(EntityList& entityList) { Misc::FoundEnemy(entityList); }
void NonMemoryFunctions() { Sonar::SoundThread(); }