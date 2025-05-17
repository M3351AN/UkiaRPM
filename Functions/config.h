#pragma once

struct RGB {
  int r;
  int g;
  int b;

  // Conversion function from RGB to COLORREF
  operator COLORREF() const { return RGB(r, g, b); }
};

namespace config {
bool ShowMenu = false;
bool ESP = true;
bool Sonar = false;
bool PitchIndicator = true;
bool InfoString = true;
}  // namespace config