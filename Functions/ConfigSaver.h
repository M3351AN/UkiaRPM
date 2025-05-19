#pragma once

#include "../Utils/yaml-cpp/yaml.h"
#include "../ImGui/imgui.h"
#include "config.h"

#include <string>

namespace MyConfigSaver {

void SaveConfig(const std::string& filename, std::string& author) {
  std::ofstream configFile(config::path + '/' + filename);
  if (!configFile.is_open()) {
    std::cerr << "[Info] Error: Could not open the configuration file."
              << std::endl;
    return;
  }
  if (author.empty()) author = getenv("USERNAME");
  YAML::Emitter emitter;
  emitter << YAML::Comment(
      "UkiaRPM-CSS Configuration File\nVersion: 1.0\nAuthor: " + author);
  emitter << YAML::BeginMap;

  emitter << YAML::Key << "config" << YAML::Value << YAML::BeginMap;
#define X(var, type) emitter << YAML::Key << #var << YAML::Value << config::var;
  AUTO_CONFIG_VARS
#undef X
  emitter << YAML::EndMap;

  emitter << YAML::EndMap;

  configFile << emitter.c_str();
  configFile.close();
  printf(XorStr("Configuration saved to %s\n"),
         (config::path + '/' + filename).c_str());
}

void LoadConfig(const std::string& filename) {
  YAML::Node root = YAML::LoadFile(config::path + '/' + filename);
  if (root["config"]) {
    YAML::Node configNode = root["config"];
#define X(var, type)                           \
  if (configNode[#var]) {                      \
    config::var = configNode[#var].as<type>(); \
  }
    AUTO_CONFIG_VARS
#undef X
  }
}

template <typename T>
static T ReadData(const YAML::Node& node, T defaultValue) {
  return node.IsDefined() ? node.as<T>() : defaultValue;
}
static int ReadOffset(const YAML::Node& node, int defaultValue) {
  return node.IsDefined() ? std::stoi(node.as<std::string>(), nullptr, 16)
                          : defaultValue;
}
static uint32_t ImColorToUInt32(const ImColor& color) {
  uint32_t r = static_cast<uint32_t>(color.Value.x * 255);
  uint32_t g = static_cast<uint32_t>(color.Value.y * 255) << 8;
  uint32_t b = static_cast<uint32_t>(color.Value.z * 255) << 16;
  uint32_t a = static_cast<uint32_t>(color.Value.w * 255) << 24;

  return r | g | b | a;
}

static ImColor UInt32ToImColor(uint32_t value) {
  ImColor TempColor;
  TempColor.Value.x = static_cast<float>(value & 0xFF) / 255.0f;
  TempColor.Value.y = static_cast<float>((value >> 8) & 0xFF) / 255.0f;
  TempColor.Value.z = static_cast<float>((value >> 16) & 0xFF) / 255.0f;
  TempColor.Value.w = static_cast<float>((value >> 24) & 0xFF) / 255.0f;
  return TempColor;
}

static std::vector<int> LoadVector(const YAML::Node& node,
                                   std::vector<int> defaultValue) {
  if (node.IsDefined() && node.IsSequence()) {
    std::vector<int> result;
    for (const YAML::Node& element : node) {
      result.push_back(element.as<int>());
    }
    return result;
  } else
    return defaultValue;
}
}  // namespace MyConfigSaver
