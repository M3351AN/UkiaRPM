#include <string>
#include <fstream>
#include <filesystem>
#include <iostream>
#include <XorStr.h>
#include <unordered_map>
#include "ConfigSaver.h"

namespace MyConfigSaver {

static std::string GetAuthorFromFile(const std::string& filePath) {
  std::ifstream file(filePath);
  std::string line;
  for (int i = 0; i < 3 && std::getline(file, line); ++i) {
    if (i == 2 && line.find(XorStr("# Author: ")) == 0) {
      return line.substr(10);
    }
  }
  return "";
}

void SaveConfig(const std::string& filename, const std::string& author) {
  const std::string fullPath = config::path + XorStr("/") + filename;
  const bool isNewFile = !std::filesystem::exists(fullPath);

  std::string actualAuthor = author;

  if (isNewFile) {
    if (actualAuthor.empty()) {
      if (const char* username = std::getenv(XorStr("USERNAME"))) {
        actualAuthor = username;
      }
    }
  } else {
    actualAuthor = GetAuthorFromFile(fullPath);
  }

  YAML::Emitter emitter;
  emitter << YAML::Comment(
      XorStr("UkiaRPM-CSS Configuration File\nVersion: 1.0\nAuthor: ") +
      actualAuthor);
  emitter << YAML::BeginMap;

  emitter << YAML::Key << XorStr("config") << YAML::Value << YAML::BeginMap;
#define X(var, type) emitter << YAML::Key << #var << YAML::Value << config::var;
  AUTO_CONFIG_VARS
#undef X
  emitter << YAML::EndMap;

  emitter << YAML::EndMap;

  std::ofstream configFile(fullPath);
  if (!configFile.is_open()) {
    MessageBoxA(nullptr, (XorStr("Could not open file:") + fullPath).c_str(),
                XorStr("UkiaRPM"), MB_OK);
    return;
  }
  configFile << emitter.c_str();
  configFile.close();

  printf(XorStr("Configuration %s at %s (Author: %s)\n"),
         isNewFile ? XorStr("created") : XorStr("updated"), fullPath.c_str(),
         actualAuthor.c_str());
}

void LoadConfig(const std::string& filename) {
  YAML::Node root = YAML::LoadFile(config::path + XorStr("/") + filename);
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

}  // namespace MyConfigSaver
