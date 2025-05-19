#pragma once
#include "../Overlay.h"

#define AUTO_CONFIG_VARS    \
  X(RCS, bool)              \
  X(RCSScale.x, float)      \
  X(RCSScale.y, float)      \
  X(ESP, bool)              \
  X(ESPDormantTime, float)  \
  X(ESPInfo, bool)          \
  X(ESPBox, bool)           \
  X(ESPName, bool)          \
  X(Radar, bool)            \
  X(RadarCrossLine, bool)   \
  X(RadarPointSize, float)  \
  X(RadarProportion, float) \
  X(RadarRange, float)      \
  X(Sonar, bool)            \
  X(PitchIndicator, bool)   \
  X(InfoString, bool)       \
  X(TeamCheck, bool)        \
  X(BypassCapture, bool)    \
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

bool Radar = true;
bool RadarCrossLine = true;
float RadarPointSize = 1.f;
float RadarProportion = 3300.f;
float RadarRange = 150.f;

bool Sonar = false;
bool PitchIndicator = true;
bool InfoString = true;

bool TeamCheck = true;
bool BypassCapture = false;
int Style = 2;
}  // namespace config