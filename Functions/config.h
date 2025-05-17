#pragma once

#define AUTO_CONFIG_VARS  \
  X(ESP, bool)            \
  X(Sonar, bool)          \
  X(PitchIndicator, bool) \
  X(InfoString, bool)

namespace config {
inline std::string path = "";
inline std::string author = "";
inline bool ShowMenu = false;
bool ESP = true;
bool Sonar = false;
bool PitchIndicator = true;
bool InfoString = true;
}  // namespace config