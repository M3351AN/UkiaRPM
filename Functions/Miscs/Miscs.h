#pragma once
#include "../../CSSx64.h"

namespace Sonar {
void SoundThread() noexcept;
}  // namespace Sonar

namespace Misc {
void FoundEnemy(EntityList& entityList) noexcept;
void SonarRun(EntityList& entityList) noexcept;
void PitchIndicator(EntityList& entityList) noexcept;
void FastStop(EntityList& entityList) noexcept;
}  // namespace Misc