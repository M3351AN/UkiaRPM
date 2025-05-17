#pragma once
#include <sstream>

#include "../CSSx64.h"

#include "Visuals/ESP.h"
#include "Miscs/Miscs.h"
#include "menu.h"

void RenderFunctions(EntityList& entityList) {
  ESP::ESPRun(entityList);
  Misc::PitchIndicator(entityList);
  Misc::SonarRun(entityList);
  Menu::DrawMenu();
  DrawNewText(10, 10, &White, XorStr("UkiaRPM for Counter-Strike Source"));
  DrawNewText(10, 100, &White, global::infos.c_str());
  DrawNewText(
      10, ImGui::GetIO().DisplaySize.y - 20, &White,
      UkiaData::strHWID.substr(UkiaData::strHWID.length() - 16).c_str());
}
void MemoryFunctions(EntityList& entityList) { Misc::FoundEnemy(entityList); }
