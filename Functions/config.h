#pragma once

#define AUTO_CONFIG_VARS   \
  X(RCS, bool)             \
  X(RCSScale.x, float)     \
  X(RCSScale.y, float)     \
  X(ESP, bool)             \
  X(ESPDormantTime, float) \
  X(ESPInfo, bool)         \
  X(ESPBox, bool)          \
  X(ESPName, bool)         \
  X(Sonar, bool)           \
  X(PitchIndicator, bool)  \
  X(InfoString, bool)      \
  X(TeamCheck, bool)       \
  X(BypassCapture, bool)   \
  X(Style, int)

namespace config {
inline std::string path = "";
inline bool ShowMenu = false;
bool RCS = false;
Vector2 RCSScale = {-2.f, 1.5f};
bool ESP = true;
float ESPDormantTime = 1.f;
bool ESPInfo = false;
bool ESPBox = true;
bool ESPName = true;
bool Sonar = false;
bool PitchIndicator = true;
bool InfoString = true;
bool TeamCheck = true;
bool BypassCapture = false;
int Style = 2;
}  // namespace config