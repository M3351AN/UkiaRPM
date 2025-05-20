#pragma once
#include "../Overlay.h"

#define AUTO_CONFIG_VARS     \
  X(RCS, bool)               \
  X(RCSScale.x, float)       \
  X(RCSScale.y, float)       \
  X(ESP, bool)               \
  X(ESPDormantTime, float)   \
  X(ESPInfo, bool)           \
  X(ESPBox, bool)            \
  X(ESPName, bool)           \
  X(Radar, bool)             \
  X(RadarDormantTime, float) \
  X(RadarCrossLine, bool)    \
  X(RadarPointSize, float)   \
  X(RadarProportion, float)  \
  X(RadarRange, float)       \
  X(Sonar, bool)             \
  X(PitchIndicator, bool)    \
  X(InfoString, bool)        \
  X(TeamCheck, bool)         \
  X(BypassCapture, bool)     \
  X(Style, int)
#ifndef UKIARPM_CONFIGS
#define UKIARPM_CONFIGS
namespace config {
inline std::string path = "";
inline bool ShowMenu = false;

inline bool RCS = false;
inline Vector2 RCSScale = {-2.f, 1.5f};

inline bool ESP = true;
inline float ESPDormantTime = 1.f;
inline bool ESPInfo = false;
inline bool ESPBox = true;
inline bool ESPName = true;

inline bool Radar = true;
inline float RadarDormantTime = 1.f;
inline bool RadarCrossLine = true;
inline float RadarPointSize = 1.f;
inline float RadarProportion = 3300.f;
inline float RadarRange = 150.f;

inline bool Sonar = false;
inline bool PitchIndicator = true;
inline bool InfoString = true;

inline bool TeamCheck = true;
inline bool BypassCapture = false;
inline int Style = 2;
}  // namespace config
#endif