#include "ConfigSaver.h"

namespace Menu {
inline void DrawMenu() {
  switch (config::Style) {
    case 0:
      ImGui::StyleColorsClassic();
      break;
    case 1:
      ImGui::StyleColorsDark();
      break;
    case 2:
      ImGui::StyleColorsLight();
      break;
    default:
      ImGui::StyleColorsClassic();
  }
  float textoffset;
  int randomresult = (rand() % 15) + 1;
  if (randomresult % 2 == 0)
    textoffset = randomresult * -0.1f;
  else
    textoffset = randomresult * 0.1f;
  ImGuiIO& io = ImGui::GetIO();
  ImGuiStyle& style = ImGui::GetStyle();
  if (!config::ShowMenu) return;
  const ImVec2 vecScreenSize = io.DisplaySize;

  ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, ImVec2(720, 365));

  ImGui::SetNextWindowPos(
      ImVec2(io.DisplaySize.x / 2.f, io.DisplaySize.y / 2.f), ImGuiCond_Once,
      ImVec2(0.5f, 0.5f));
  ImGui::SetNextWindowSize(ImVec2(720, 365), ImGuiCond_Always);
  ImGui::Begin(XorStr("UkiaRPM for Counter-Strike Source"), NULL,
               ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
                   ImGuiWindowFlags_NoScrollbar |
                   ImGuiWindowFlags_NoScrollWithMouse |
                   ImGuiWindowFlags_NoCollapse);

  const ImVec2 vecMenuPos = ImGui::GetWindowPos();
  const ImVec2 vecMenuSize = ImGui::GetWindowSize();
  ImDrawList* pDrawList = ImGui::GetWindowDrawList();

  ImGui::BeginTabBar(XorStr("##Tabs"));
#ifdef _DEBUG
  if (ImGui::BeginTabItem(XorStr("semirage"))) {
    ImGui::Text(XorStr("coming soon?"));
    ImGui::EndTabItem();
  }
#endif
  if (ImGui::BeginTabItem(XorStr("legit"))) {
    ImVec2 child_size =
        ImVec2((ImGui::GetColumnWidth() - (style.ItemSpacing.x * 2)) / 3,
               ImGui::GetWindowHeight() - 10.f -
                   (ImGui::GetCursorPosY() + style.ItemInnerSpacing.y * 2));
    auto childBegin = ImGui::GetCursorPosY();
    ImGui::BeginChild(
        XorStr("Aim assist##legit"),
        ImVec2(child_size.x,
               (child_size.y - (style.ItemInnerSpacing.y)) * .6f));
    {
    }
    ImGui::EndChild();

    ImGui::BeginChild(
        XorStr("RCS"),
        ImVec2(child_size.x,
               (child_size.y - (style.ItemInnerSpacing.y)) * .4f));
    {
      ImGui::Checkbox(XorStr("RCS"), &config::RCS);
      ImGui::SliderFloat(XorStr("RCS scale x"), &config::RCSScale.x, -2.5f,
                         2.5f, XorStr("%.1f"));
      ImGui::SliderFloat(XorStr("RCS scale y"), &config::RCSScale.y, -2.5f,
                         2.5f, XorStr("%.1f"));
    }
    ImGui::EndChild();

    ImGui::SameLine();
    ImGui::SetCursorPosY(childBegin);
    ImGui::BeginChild(XorStr("Triggerbot"), child_size);
    {
    }
    ImGui::EndChild();
    ImGui::SameLine();
    ImGui::SetCursorPosY(childBegin);
    ImGui::BeginChild(XorStr("Others##aimbot"), child_size);
    {
    }
    ImGui::EndChild();

    ImGui::EndTabItem();
  }

  if (ImGui::BeginTabItem(XorStr("visuals"))) {
    ImVec2 child_size =
        ImVec2((ImGui::GetColumnWidth() - (style.ItemSpacing.x * 2)) / 3,
               ImGui::GetWindowHeight() - 10.f -
                   (ImGui::GetCursorPosY() + style.ItemInnerSpacing.y * 2));
    static float flOverlayChildSize = 0.f;

    ImGui::BeginChild(XorStr("ESP"), child_size);
    {
      ImGui::PushStyleVar(ImGuiStyleVar_FramePadding,
                          ImVec2(style.FramePadding.x, 0));

      ImGui::Checkbox(XorStr("ESP"), &config::ESP);
      ImGui::SliderFloat(XorStr("dormant time##ESP"), &config::ESPDormantTime, 0.f,
                         10.f, XorStr("%.1f s"));
      ImGui::Checkbox(XorStr("player info"), &config::ESPInfo);
      ImGui::Checkbox(XorStr("box"), &config::ESPBox);
      ImGui::Checkbox(XorStr("player name"), &config::ESPName);
      ImGui::PopStyleVar();

      flOverlayChildSize = ImGui::GetCursorPosY() + style.ItemSpacing.y;
    }
    ImGui::EndChild();

    ImGui::SameLine();

    ImGui::BeginChild(XorStr("World"), child_size);
    {
      ImGui::PushStyleVar(ImGuiStyleVar_FramePadding,
                          ImVec2(style.FramePadding.x, 0));

      ImGui::PopStyleVar();
    }
    ImGui::EndChild();

    ImGui::SameLine();

    ImGui::BeginChild(XorStr("Others"), child_size);
    {
      ImGui::PushStyleVar(ImGuiStyleVar_FramePadding,
                          ImVec2(style.FramePadding.x, 0));
      ImGui::Checkbox(XorStr("radar"), &config::Radar);
      ImGui::SliderFloat(XorStr("dormant time##Radar"), &config::RadarDormantTime,
                         0.f, 10.f, XorStr("%.1f s"));
      ImGui::Checkbox(XorStr("crossline"), &config::RadarCrossLine);
      ImGui::SliderFloat(XorStr("point size"), &config::RadarPointSize, 0.5f,
                         5.f, XorStr("%.1f p"));
      ImGui::SliderFloat(XorStr("proportion"), &config::RadarProportion, 500.f,
                         5000.f, XorStr("%.1f u"));
      ImGui::SliderFloat(XorStr("range"), &config::RadarRange, 50.f,
                         300.f, XorStr("%.1f u"));
      ImGui::PopStyleVar();
    }
    ImGui::EndChild();

    ImGui::EndTabItem();
  }

  if (ImGui::BeginTabItem(XorStr("miscs"))) {
    ImVec2 child_size =
        ImVec2((ImGui::GetColumnWidth() - (style.ItemSpacing.x * 2)) / 3,
               ImGui::GetWindowHeight() - 10.f -
                   (ImGui::GetCursorPosY() + style.ItemInnerSpacing.y * 2));

    ImGui::BeginChild(XorStr("Globals##miscs"), child_size);
    {
      ImGui::PushStyleVar(ImGuiStyleVar_FramePadding,
                          ImVec2(style.FramePadding.x, 0));
      ImGui::Checkbox(XorStr("pitch indicator"), &config::PitchIndicator);
      ImGui::Checkbox(XorStr("sonar"), &config::Sonar);
      ImGui::PopStyleVar();
    }
    ImGui::EndChild();

    ImGui::SameLine();

    ImGui::BeginChild(XorStr("Misc##MiscChild"), child_size);
    {
      ImGui::PushStyleVar(ImGuiStyleVar_FramePadding,
                          ImVec2(style.FramePadding.x, 0));
      ImGui::Checkbox(XorStr("enemy info"), &config::InfoString);
      ImGui::PopStyleVar();
    }
    ImGui::EndChild();

    ImGui::SameLine();

    ImGui::BeginChild(XorStr("Movement"), child_size);
    {
      ImGui::PushStyleVar(ImGuiStyleVar_FramePadding,
                          ImVec2(style.FramePadding.x, 0));

      ImGui::PopStyleVar();
    }
    ImGui::EndChild();

    ImGui::EndTabItem();
  }

  if (ImGui::BeginTabItem(XorStr("settings"))) {
    ImVec2 child_size =
        ImVec2((ImGui::GetColumnWidth() - (style.ItemSpacing.x * 2)) / 3,
               ImGui::GetWindowHeight() - 10.f -
                   (ImGui::GetCursorPosY() + style.ItemInnerSpacing.y * 2));

    ImGui::BeginChild(XorStr("Info"), child_size);
    {
      ImGui::Text(XorStr("last build: %s"), __DATE__);
      ImGui::Text(XorStr("build for version: %s"),
                  std::to_string(CS_VERSION).c_str());
      ImGui::Text(XorStr("current version: %s"),
                  std::to_string(global::gameVersion).c_str());
#ifdef _MSC_VER
#ifndef __clang__
      ImGui::Text(XorStr("complier: MSVC %s"),
                  std::to_string(_MSC_VER).c_str());
#endif
#endif
#ifdef __clang__
      ImGui::Text(XorStr("complier: Clang %s"),
                  std::to_string(__clang_major__).c_str());
#endif
      ImGui::Text(XorStr("copy licenced to: %s"), getenv(XorStr("USERNAME")));
      ImGui::Text(XorStr("client base: %s"),
                  std::format("0x{:X}", Memory::clientAddress).c_str());
      ImGui::Text(XorStr("engine base: %s"),
                  std::format("0x{:X}", Memory::engineAddress).c_str());
    }
    ImGui::EndChild();

    ImGui::SameLine();

    static char configNameBuffer[128] = "NewConfig";
    static char configAuthorBuffer[128] = "";

    MyConfigSaver::UpdateConfigFiles();
    ImGui::BeginChild(XorStr("List"), child_size, NULL,
                      ImGuiWindowFlags_AlwaysVerticalScrollbar);
    {
      ImGui::PushStyleVar(ImGuiStyleVar_FramePadding,
                          ImVec2(ImGui::GetStyle().FramePadding.x, 0));

      const float itemWidth = 200.0f;
      const float availableWidth = ImGui::GetContentRegionAvail().x;
      const int columns = max(1, static_cast<int>(availableWidth / itemWidth));

      ImGui::Columns(columns, NULL, false);

      for (const auto& [filename, modiTimeStr, author] : configFiles) {
        ImGui::PushID(filename.c_str());
        ImGui::BeginChild((XorStr("##") + filename).c_str(),
                          ImVec2(itemWidth - 10, 85), true,
                          ImGuiWindowFlags_NoScrollWithMouse);
        {
          ImGui::SetCursorPosY(3.f);
          std::string configname = filename;
          size_t pos = configname.find_last_of('.');
          if (pos != std::string::npos) {
            configname.erase(pos);
          }
          ImGui::TextUnformatted(configname.c_str());
          ImGui::TextUnformatted(modiTimeStr.c_str());
          ImGui::Text(XorStr("Author: %s"),
                      author.empty() ? " " : author.c_str());

          if (selectedConfigFile == filename) {
            if (ImGui::Button(XorStr("Save"))) {
              MyConfigSaver::SaveConfig(filename, configAuthorBuffer);
            }
            ImGui::SameLine();
            if (ImGui::Button(XorStr("Reload"))) {
              MyConfigSaver::LoadConfig(filename);
            }
          } else {
            if (ImGui::Button(XorStr("Load"))) {
              MyConfigSaver::LoadConfig(filename);
              selectedConfigFile = filename;
            }
          }
          ImGui::SameLine();
          if (ImGui::Button(XorStr("Delete"))) {
            deletePendingFile = filename;
            ImGui::OpenPopup(XorStr("##deleteConfirm"));
          }
          if (ImGui::BeginPopup(XorStr("##deleteConfirm"))) {
            ImGui::Text(XorStr("Are you sure to delete %s?"),
                        deletePendingFile.c_str());
            if (ImGui::Button(XorStr("Yes"))) {
              std::string fullPath = config::path + "\\" + deletePendingFile;
              std::remove(fullPath.c_str());
              deletePendingFile.clear();
              ImGui::CloseCurrentPopup();
            }
            ImGui::SameLine();
            if (ImGui::Button(XorStr("No"))) {
              deletePendingFile.clear();
              ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
          }
        }
        ImGui::EndChild();
        bool hovered = ImGui::IsItemHovered();
        ImVec2 p_min = ImGui::GetItemRectMin();
        ImVec2 p_max = ImGui::GetItemRectMax();
        ImU32 border_color =
            (selectedConfigFile == filename)
                ? ImGui::GetColorU32(ImGuiCol_FrameBgActive)
                : (hovered ? ImGui::GetColorU32(ImGuiCol_FrameBgHovered)
                           : ImGui::GetColorU32(ImGuiCol_FrameBg));
        ImDrawList* draw_list = ImGui::GetWindowDrawList();
        draw_list->AddRect(p_min, p_max, border_color, 0.0f, 0, 1.0f);
        ImGui::PopID();

        ImGui::NextColumn();
      }
      ImGui::Columns(1);

      ImGui::PopStyleVar();
    }
    ImGui::EndChild();

    ImGui::SameLine();

    ImGui::BeginChild(XorStr("Settings"), child_size);
    {
      ImGui::Checkbox(XorStr("team check"), &config::TeamCheck);
      ImGui::Checkbox(XorStr("bypass capture"), &config::BypassCapture);
      ImGui::Combo(XorStr("style"), &config::Style,
                   XorStr("Classic\0Dark\0Light\0"));

      const float CursorX = 10.f;
      const float ComponentWidth = ImGui::GetColumnWidth() -
                                   ImGui::GetStyle().ItemSpacing.x -
                                   CursorX * 2;

      ImGui::SetNextItemWidth(ComponentWidth);
      ImGui::Text(XorStr("new config name"));
      ImGui::InputText(XorStr("##newConfigName"), configNameBuffer,
                       sizeof(configNameBuffer));

      ImGui::SetNextItemWidth(ComponentWidth);
      ImGui::Text(XorStr("author name"));
      ImGui::InputText(XorStr("##authorName"), configAuthorBuffer,
                       sizeof(configAuthorBuffer));
      std::string configFileName =
          std::string(configNameBuffer) + XorStr(".yaml");
      bool exists = std::any_of(configFiles.begin(), configFiles.end(),
                                [&](const auto& item) {
                                  return std::get<0>(item) == configFileName;
                                });

      if (!exists && !configFileName.empty()) {
        if (ImGui::Button(XorStr("Create"))) {
          MyConfigSaver::SaveConfig(configFileName, configAuthorBuffer);
          selectedConfigFile = configFileName;
        }
      } else {
        ImGui::BeginDisabled();
        {
          ImGui::Button(XorStr("Create"));
        }
        ImGui::EndDisabled();
      }
      ImGui::SameLine();
      if (ImGui::Button(XorStr("Open folder"))) {
        ShellExecuteA(NULL, XorStr("open"), config::path.c_str(), NULL, NULL,
                      SW_SHOWNORMAL);
      }

      if (ImGui::Button(XorStr("Unhook"))) {
        global::isRunning = false;
      }

      ImGui::Text(XorStr("Menukey [DEL]"));
    }
    ImGui::EndChild();

    ImGui::EndTabItem();
  }

  ImGui::EndTabBar();
  ImGui::SetCursorPos(ImVec2{style.ItemSpacing.x, 350});
  ImGui::Text(
      XorStr("UkiaRPM for Counter-Strike Source by \u6e1f\u96f2 %s"),
      UkiaData::strHWID.substr(UkiaData::strHWID.length() - 16).c_str());

  ImGui::End();
  ImVec2 mousePos = ImGui::GetMousePos();
  float interpolationFactorX = 0.035f;
  float interpolationFactorY = 0.015f;
  ImVec2 center = ImVec2(vecMenuPos.x - 300, vecMenuPos.y + 25);
  float radius = 40;
  ImVec2 factor = ImVec2(interpolationFactorX * (mousePos.x - vecMenuPos.x),
                         interpolationFactorY * (mousePos.y - vecMenuPos.y));
  ImVec2 interpolatedPos{center.x - factor.x, center.y - factor.y};

  // 计算 interpolatedPos 到圆心的距离
  float dx = interpolatedPos.x - center.x;
  float dy = interpolatedPos.y - center.y;
  float distance = sqrt(dx * dx + dy * dy);

  // 如果距离超过半径，则调整 interpolatedPos
  if (distance > radius) {
    float scale = radius / distance;
    factor.x = -dx * scale;
    factor.y = -dy * scale;
    interpolatedPos = ImVec2{center.x - factor.x, center.y - factor.y};
  }

  const ImVec2 vecOverlayPadding = ImVec2(90.f, 30.f);
  ImGui::PopStyleVar();
  ImGui::SetNextWindowPos(interpolatedPos);
  ImGui::Begin(XorStr("moe"), nullptr,
               ImGuiWindowFlags_NoMouseInputs |
                   ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove |
                   ImGuiWindowFlags_NoFocusOnAppearing |
                   ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar |
                   ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoCollapse |
                   ImGuiWindowFlags_NoScrollbar |
                   ImGuiWindowFlags_::ImGuiWindowFlags_NoBringToFrontOnFocus |
                   ImGuiWindowFlags_::ImGuiWindowFlags_NoBackground |
                   ImGuiWindowFlags_::ImGuiWindowFlags_NoNavFocus |
                   ImGuiWindowFlags_::ImGuiWindowFlags_NoNav);
  {
    ImGui::SetCursorPos(ImVec2{50, 25});
    ImGui::Image((ImTextureID)global::Shigure, ImVec2{258, 349}, ImVec2{0, 0},
                 ImVec2{1, 1});

    const ImVec2 vecWindPos = ImGui::GetWindowPos();
    const ImVec2 vecWindSize = ImGui::GetWindowSize();

    ImDrawList* pDrawList = ImGui::GetWindowDrawList();
    ImGui::SetCursorPos(ImVec2{20, 40});
    ImVec4 vecBox = {vecWindPos.x + vecOverlayPadding.x + 50.f,
                     vecWindPos.y + 15,
                     vecWindPos.x + vecWindSize.x - vecOverlayPadding.x + 50.f,
                     vecWindPos.y + vecWindSize.y - vecOverlayPadding.y};
    vecBox.x -= factor.x * 0.15f;
    vecBox.z -= factor.x * 0.15f;
    vecBox.y -= factor.y * 0.15f;
    vecBox.w -= factor.y * 0.15f;

    ImGui::SetCursorPos(ImVec2{15 + textoffset, 250 - textoffset} * 1.f +
                        factor * 0.3351f);
    ImGui::TextColored(ImColor(70, 50, 240, 200), XorStr("NOT FOR SELLING!!"));
    ImGui::SetCursorPos(ImVec2{15 + textoffset, 295 - textoffset} * 1.f +
                        factor * 0.1337f);
    ImGui::TextColored(ImColor(70, 50, 240, 200),
                       XorStr("UkiaRPM for Counter-Strike Source"));
    ImGui::SetCursorPos(ImVec2{15 + textoffset, 335 - textoffset} * 1.f +
                        factor * 0.2024f);
    ImGui::TextColored(ImColor(70, 50, 240, 200), XorStr("Build: %s %s"),
                       __DATE__, __TIME__);
    ImGui::SetCursorPos(ImVec2{15 - textoffset, 250 + textoffset} * 1.f +
                        factor * 0.3351f);
    ImGui::TextColored(ImColor(235, 5, 85, 200), XorStr("NOT FOR SELLING!!"));
    ImGui::SetCursorPos(ImVec2{15 - textoffset, 295 + textoffset} * 1.f +
                        factor * 0.1337f);
    ImGui::TextColored(ImColor(235, 5, 85, 200),
                       XorStr("UkiaRPM for Counter-Strike Source"));
    ImGui::SetCursorPos(ImVec2{15 - textoffset, 335 + textoffset} * 1.f +
                        factor * 0.2024f);
    ImGui::TextColored(ImColor(235, 5, 85, 200), XorStr("Build: %s %s"),
                       __DATE__, __TIME__);
    ImGui::SetCursorPos(ImVec2{15, 250} * 1.f + factor * 0.3351f);
    ImGui::TextColored(ImColor(245, 245, 245, 245),
                       XorStr("NOT FOR SELLING!!"));
    ImGui::SetCursorPos(ImVec2{15, 295} * 1.f + factor * 0.1337f);
    ImGui::TextColored(ImColor(245, 245, 245, 245),
                       XorStr("UkiaRPM for Counter-Strike Source"));
    ImGui::SetCursorPos(ImVec2{15, 335} * 1.f + factor * 0.2024f);
    ImGui::TextColored(ImColor(245, 245, 245, 245), XorStr("Build: %s %s"),
                       __DATE__, __TIME__);
  }
  ImGui::End();
}
}  // namespace Menu