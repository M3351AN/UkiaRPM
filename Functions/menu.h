#include "ConfigSaver.h"

namespace Menu {
void DrawMenu() {
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
  ImGui::Begin(XorStr("TokiKiri"), NULL,
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
    {}
    ImGui::EndChild();

    ImGui::BeginChild(
        XorStr("Silent"),
        ImVec2(child_size.x,
               (child_size.y - (style.ItemInnerSpacing.y)) * .4f));
    {}
    ImGui::EndChild();

    ImGui::SameLine();
    ImGui::SetCursorPosY(childBegin);
    ImGui::BeginChild(XorStr("Triggerbot"), child_size);
    {}
    ImGui::EndChild();
    ImGui::SameLine();
    ImGui::SetCursorPosY(childBegin);
    ImGui::BeginChild(XorStr("Others##aimbot"), child_size);
    {}
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
      ImGui::SliderFloat(XorStr("dormant time"), &config::ESPDormantTime, 0.f,
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
      // ImGui::Checkbox(XorStr("hitmark"), &C_GET(bool, Vars.bHitMark));
      ImGui::PushStyleVar(ImGuiStyleVar_FramePadding,
                          ImVec2(style.FramePadding.x, 0));

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
      ImGui::Text(XorStr("copy licenced to: %s"), getenv("USERNAME"));
      ImGui::Text(XorStr("client base: %s"),
                  std::format("0x{:X}", Memory::clientAddress).c_str());
      ImGui::Text(XorStr("engine base: %s"),
                  std::format("0x{:X}", Memory::engineAddress).c_str());
    }
    ImGui::EndChild();

    ImGui::SameLine();

    ImGui::BeginChild(XorStr("Settings"), child_size);
    {
      static int nSelectedColor = 0;
      static char configNameBuffer[128] = "NewConfig";
      static char configAuthorBuffer[128] = "";
      std::string configAuthorName;
      static int selectedConfig = -1;

      ImGui::PushStyleVar(ImGuiStyleVar_FramePadding,
                          ImVec2(ImGui::GetStyle().FramePadding.x, 0));

      const std::string configDir = config::path;
      static std::vector<std::string> configFiles;
      std::vector<const char*> configFilesCStr;

      configFiles.clear();
      for (const auto& entry : std::filesystem::directory_iterator(configDir)) {
        if (entry.is_regular_file() &&
            entry.path().extension() == XorStr(".yaml")) {
          configFiles.push_back(entry.path().filename().string());
        }
      }
      for (const auto& file : configFiles) {
        configFilesCStr.push_back(file.c_str());
      }

      const float CursorX = 10.f;
      const float CurrentCursorX = ImGui::GetCursorPosX();
      const float ComponentWidth = ImGui::GetColumnWidth() -
                                   ImGui::GetStyle().ItemSpacing.x -
                                   CursorX * 2;

      ImGui::SetCursorPosX(CurrentCursorX + CursorX);
      ImGui::SetNextItemWidth(ComponentWidth);
      ImGui::ListBox(XorStr("##ConfigFiles"), &selectedConfig,
                     configFilesCStr.data(), configFilesCStr.size(), 5);

      ImGui::SetCursorPosX(CurrentCursorX + CursorX);
      if (ImGui::Button(XorStr("Load")) && selectedConfig >= 0 &&
          selectedConfig < configFiles.size()) {
        std::string selectedConfigFile = configFiles[selectedConfig];
        MyConfigSaver::LoadConfig(selectedConfigFile);
      }
      ImGui::SameLine();
      if (ImGui::Button(XorStr("Save")) && selectedConfig >= 0 &&
          selectedConfig < configFiles.size()) {
        std::string selectedConfigFile = configFiles[selectedConfig];
        MyConfigSaver::SaveConfig(selectedConfigFile, configAuthorName);
      }

      ImGui::SetCursorPosX(CurrentCursorX + CursorX);
      if (ImGui::Button(XorStr("Delete")) && selectedConfig >= 0 &&
          selectedConfig < configFiles.size())
        ImGui::OpenPopup(XorStr("##reallyDelete"));
      if (ImGui::BeginPopup(XorStr("##reallyDelete"))) {
        ImGui::TextUnformatted(XorStr("Are you sure?"));
        if (ImGui::Button(XorStr("No"))) ImGui::CloseCurrentPopup();
        ImGui::SameLine();
        if (ImGui::Button(XorStr("Yes"))) {
          std::string fullPath = configDir + "\\" + configFiles[selectedConfig];
          if (std::remove(fullPath.c_str()) == 0) {
            configFiles.erase(configFiles.begin() + selectedConfig);
            selectedConfig = -1;
          }
          ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
      }

      ImGui::SetCursorPosX(CurrentCursorX + CursorX);
      ImGui::TextDisabled(XorStr("create"));

      ImGui::SetCursorPosX(CurrentCursorX + CursorX);
      ImGui::Text(XorStr("setting name"));
      ImGui::SetCursorPosX(CurrentCursorX + CursorX);
      ImGui::SetNextItemWidth(ComponentWidth + 8);
      ImGui::InputText(XorStr("###ConfigNameInput"), configNameBuffer,
                       sizeof(configNameBuffer));

      ImGui::SetCursorPosX(CurrentCursorX + CursorX);
      ImGui::Text(XorStr("author"));
      ImGui::SetCursorPosX(CurrentCursorX + CursorX);
      ImGui::SetNextItemWidth(ComponentWidth + 8);
      ImGui::InputText(XorStr("###AuthorNameInput"), configAuthorBuffer,
                       sizeof(configAuthorBuffer));

      ImGui::NewLine();
      ImGui::SetCursorPosX(CurrentCursorX + CursorX);
      if (ImGui::Button(XorStr("Create"))) {
        std::string configFileName =
            std::string(configNameBuffer) + XorStr(".yaml");
        configAuthorName = std::string(configAuthorBuffer);

        MyConfigSaver::SaveConfig(configFileName, configAuthorName);
      }
      ImGui::SameLine();
      if (ImGui::Button(XorStr("Open folder"))) {
        ShellExecuteA(NULL, XorStr("open"), configDir.c_str(), NULL, NULL,
                      SW_SHOWNORMAL);
      }

      ImGui::PopStyleVar();
    }
    ImGui::EndChild();

    ImGui::SameLine();

    ImGui::BeginChild(XorStr("Miscs"), child_size);
    {
      ImGui::Checkbox(XorStr("team check"), &config::TeamCheck);
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
      XorStr("UkiaRPM for Counter-Strike Source %s"),
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
    ImGui::Image((void*)global::Shigure, ImVec2{258, 349});

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

    ImGui::SetCursorPos(ImVec2{15 + textoffset, 250 - textoffset} + factor);
    ImGui::SetCursorPos(ImVec2{15, 250} + factor * 0.3351f);
    ImGui::TextColored(ImColor(245, 245, 245, 245), "NOT FOR SELLING!!");
    ImGui::SetCursorPos(ImVec2{15, 295} + factor * 0.1337f);
    ImGui::TextColored(ImColor(245, 245, 245, 245), "UkiaRPM");
    ImGui::SetCursorPos(ImVec2{15, 335} + factor * 0.2024f);
    ImGui::TextColored(ImColor(245, 245, 245, 245), "Build: %s %s", __DATE__,
                       __TIME__);
    ImGui::SetCursorPos(ImVec2{15 - textoffset, 250 + textoffset} + factor);
  }
  ImGui::End();
}
}  // namespace Menu