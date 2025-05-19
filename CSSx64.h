#pragma once
#include <Uxtheme.h>
#include <d3d9.h>
#include <dwmapi.h>

#include <atomic>
#include <chrono>
#include <iostream>
#include <mutex>

#include "Overlay.h"
#include "UkiaStuff.h"
#include "Utils/XorStr.h"
#include "Functions/ConfigSaver.h"

#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "dwmapi.lib")

#define CS_VERSION 9540945

namespace global {
std::atomic<bool> isRunning(true);

DWORD uiAccessStatus;
DWORD processId;

DWORD gameVersion;

inline HDC hdcBuffer = NULL;
inline HBITMAP hbmBuffer = NULL;

RECT gameBounds;
HWND hwnd_;

Vector2 screenSize;
Vector2 screenPos;

std::string infos;

inline IDirect3DTexture9* Shigure = nullptr;
}  // namespace global
class ViewMatrix {
 private:
  float matrix[4][4];

 public:
  std::chrono::steady_clock::time_point last_update;
  Vector2 screen_size;
  Vector2 screen_center;

  ViewMatrix() : last_update(std::chrono::steady_clock::now()) {
    screen_size = {0, 0};
  }

  float* operator[](int index) { return matrix[index]; }

  void SetScreenData(Vector2 screenSize) {
    screen_size = screenSize;
    screen_center = {screen_size.x / 2.f, screen_size.y / 2.f};
  }

  bool NeedUpdate() const {
    return std::chrono::steady_clock::now() - last_update >
           std::chrono::milliseconds(2);
  }

  void Update(uintptr_t engineAddress) {
    uintptr_t viewMatrixPtr;
    Ukia::ProcessMgr.ReadMemory(engineAddress + 0x00698F18, viewMatrixPtr);
    Ukia::ProcessMgr.ReadMemory(viewMatrixPtr + 0x2D4, matrix);
    last_update = std::chrono::steady_clock::now();
  }

  bool WorldToScreen(Vector3 position, Vector2& out_position) {
    out_position.x = matrix[0][0] * position.x + matrix[0][1] * position.y +
                     matrix[0][2] * position.z + matrix[0][3];
    out_position.y = matrix[1][0] * position.x + matrix[1][1] * position.y +
                     matrix[1][2] * position.z + matrix[1][3];

    float w = matrix[3][0] * position.x + matrix[3][1] * position.y +
              matrix[3][2] * position.z + matrix[3][3];

    if (w < 0.01f) return false;

    float inv_w = 1.f / w;
    out_position.x *= inv_w;
    out_position.y *= inv_w;

    float x = screen_size.x * .5f;
    float y = screen_size.y * .5f;

    x += 0.5f * out_position.x * screen_size.x + 0.5f;
    y -= 0.5f * out_position.y * screen_size.y + 0.5f;

    out_position.x = x;
    out_position.y = y;
    return true;
  }
};

#pragma pack(push, 1)
struct BaseEntityData {
  char _pad1[0xCF];
  uint8_t life_state;
  int health;
  char _pad2[0xD8 - 0xD4];
  int team;
  char _pad3[0x144 - 0xDC];
  float head_height;
  Vector3 velocity;
  char _pad4[0x1F4 - 0x154];
  int movetype;
  char _pad5[0x1FA - 0x1F8];
  bool dormant;
  char _pad6[0x314 - 0x1FB];
  Vector3 viewangles;
  Vector3 position;
};

struct EntityData : public BaseEntityData {
  char _pad7[0x15DC - 0x32C];  // 0x32C-0x15DB
  int m_iObserverMode;         // 0x15DC
  int m_hObserverTarget;       // 0x15E0

  char _pad8[0x1A10 - 0x15E4];   // 0x15E4-0x1A0F
  int m_bIsDefusing;             // 0x1A10
  char _pad9[0x1A34 - 0x1A14];   // 0x1A14-0x1A33
  int m_iShotsFired;             // 0x1A34
  char _pad10[0x1A54 - 0x1A38];  // 0x1A38-0x1A53
  float m_flFlashMaxAlpha;       // 0x1A54
  float m_flFlashDuration;       // 0x1A58

  char _pad11[0x1A9C - 0x1A5C];  // 0x1A5C-0x1A9B
  int m_bHasHelmet;              // 0x1A9C
  char _pad12[0x1AA4 - 0x1AA0];  // 0x1AA0-0x1AA3
  int m_ArmorValue;              // 0x1AA4
  char _pad13[0x1AB4 - 0x1AA8];  // 0x1AA8-0x1AB3
  int m_bHasDefuser;             // 0x1AB4
};

struct LocalData : public BaseEntityData {
  char _pad7[0x398 - 0x32C];
  Vector2 viewangles1;
  char _pad8[0x440 - 0x3A0];
  int flags;
  char _pad9[0x127C - 0x444];
  Vector3 punchangle;
  char _pad10[0x12D8 - 0x1288];
  Vector3 punchangle_vel;
  char _pad11[0x1570 - 0x12E4];
  int fovend;
  int fov;
  char _pad12[0x1584 - 0x1578];
  int crosshair_entity_id;
};
#pragma pack(pop)

namespace Memory {
uintptr_t clientAddress, engineAddress, nameListBase;

bool UpdateAddress() {
  clientAddress = reinterpret_cast<uintptr_t>(
      Ukia::ProcessMgr.GetProcessModuleHandle(XorStr("client.dll")));

  engineAddress = reinterpret_cast<uintptr_t>(
      Ukia::ProcessMgr.GetProcessModuleHandle(XorStr("engine.dll")));

  Ukia::ProcessMgr.ReadMemory(engineAddress + 0x00793EB0, global::gameVersion);
  return true;
}
};  // namespace Memory

class Entity {
 public:
  uintptr_t address;
  EntityData data;
  std::string name;
  int index;

  Entity() : address(0), index(-1) {}
  Entity(uintptr_t addr, int idx) : address(addr), index(idx) {}

  void RefreshFullData() {
    if (address != 0) {
      memset(&data, 0, sizeof(data));
      Ukia::ProcessMgr.ReadMemory(address, data);

      uintptr_t nameAddr;
      Ukia::ProcessMgr.ReadMemory(Memory::nameListBase + 0x798 + (index * 0x2),
                                  nameAddr);
      name = Ukia::ProcessMgr.ReadString(nameAddr);
    }
  }

  bool IsValid() const {
    return address != 0 && data.health > 0 && data.health < 250 &&
           data.life_state == 0;
  }

  bool IsEnemy(int local_team) const {
    if (index == 0) return false;  // it's local player
    if (!config::TeamCheck) return true;
    return (data.team != local_team);
  }

  float DistanceTo(const Vector3& other) const {
    return std::sqrt(std::pow(data.position.x - other.x, 2) +
                     std::pow(data.position.y - other.y, 2) +
                     std::pow(data.position.z - other.z, 2));
  }
};

class EntityList {
 public:
  static constexpr int MAX_ENTITIES = 64;

  LocalData local_player_data;
  uintptr_t local_player_address;
  ViewMatrix view_matrix;

  // Ë«»º³å½á¹¹
  struct EntityBuffer {
    std::array<Entity, MAX_ENTITIES> entities;
    std::array<uintptr_t, MAX_ENTITIES> address_cache;
    std::chrono::steady_clock::time_point update_time;
  };

  EntityBuffer front_buffer, back_buffer;
  mutable std::mutex buffer_mtx;

  EntityList() {
    front_buffer.entities.fill(Entity());
    back_buffer.entities.fill(Entity());
  }

  void UpdateAll() {
    std::lock_guard<std::mutex> lock(buffer_mtx);

    constexpr uintptr_t kEntityListOffset = 0x6098C8;
    constexpr uintptr_t kLocalPlayerOffset = 0x5F4B68;
    constexpr uintptr_t kNameListOffset = 0x609D68;
    Ukia::ProcessMgr.ReadMemory(Memory::clientAddress + kEntityListOffset,
                                back_buffer.address_cache,
                                MAX_ENTITIES * sizeof(uintptr_t));

    Ukia::ProcessMgr.ReadMemory(Memory::clientAddress + kLocalPlayerOffset,
                                local_player_address);
    Ukia::ProcessMgr.ReadMemory(local_player_address, local_player_data);
    Ukia::ProcessMgr.ReadMemory(Memory::clientAddress + kNameListOffset,
                                Memory::nameListBase);

    std::for_each(std::execution::par_unseq, back_buffer.entities.begin(),
                  back_buffer.entities.end(), [&](Entity& ent) {
                    const int i = &ent - &back_buffer.entities[0];
                    ent.address = back_buffer.address_cache[i];
                    ent.index = i;

                    ent.RefreshFullData();
                  });

    if (view_matrix.NeedUpdate()) {
      view_matrix.Update(Memory::engineAddress);
    }
    view_matrix.SetScreenData(global::screenSize);
    back_buffer.update_time = std::chrono::steady_clock::now();
    std::swap(front_buffer, back_buffer);
  }

  const auto& GetCurrentEntities() const { return front_buffer.entities; }
};

EntityList entity_list;

namespace DirectX9Interface {
IDirect3D9Ex* Direct3D9 = NULL;
IDirect3DDevice9Ex* pDevice = NULL;
D3DPRESENT_PARAMETERS pParams = {NULL};
MARGINS Margin = {-1};
MSG Message = {NULL};
}  // namespace DirectX9Interface

namespace OverlayWindow {
WNDCLASSEXA WindowClass;
HWND Hwnd;
LPCSTR Name;
}  // namespace OverlayWindow