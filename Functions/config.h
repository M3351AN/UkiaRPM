#pragma once

#define AUTO_CONFIG_VARS   \
  X(ESP, bool)             \
  X(ESPDormantTime, float) \
  X(ESPInfo, bool)         \
  X(ESPBox, bool)          \
  X(ESPName, bool)         \
  X(Sonar, bool)           \
  X(PitchIndicator, bool)  \
  X(InfoString, bool)      \
  X(TeamCheck, bool)

namespace config {
inline std::string path = "";
inline bool ShowMenu = false;
bool ESP = true;
float ESPDormantTime = 1.f;
bool ESPInfo = false;
bool ESPBox = true;
bool ESPName = true;
bool Sonar = false;
bool PitchIndicator = true;
bool InfoString = true;
bool TeamCheck = true;
}  // namespace config