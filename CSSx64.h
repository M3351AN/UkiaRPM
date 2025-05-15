#pragma once
#include <Uxtheme.h>
#include <d3d9.h>
#include <dwmapi.h>

#include "Overlay.h"
#include "UkiaStuff.h"
#include "Utils/XorStr.h"

#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "dwmapi.lib")

namespace global {
DWORD processId;
inline HDC hdcBuffer = NULL;
inline HBITMAP hbmBuffer = NULL;

RECT gameBounds;
HWND hwnd_;

Vector2 screenSize;
Vector2 screenPos;

std::string infos;
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

struct EntityData : public BaseEntityData {};
struct LocalData : public BaseEntityData {
  char _pad7[0x398 - 0x32C];
  Vector2 viewangles1;
  char _pad8[0x440 - 0x3A0];
  int flags;
  char _pad9[0x1570 - 0x444];
  int fovend;
  int fov;
  char _pad10[0x1584 - 0x1578];
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
  return true;
}
};  // namespace Memory

class Entity {
 public:
  uintptr_t address;
  EntityData data;
  std::string name;
  int index;
  std::chrono::steady_clock::time_point last_full_update;

  Entity() : address(0), index(-1), last_full_update() {}
  Entity(uintptr_t addr, int idx)
      : address(addr), index(idx), last_full_update() {}

  void RefreshCriticalData() {
    // call once for 1 block in in 5 periods or call twice for 10 blocks in a
    // period i think call twice for 1 block in a period and then call once for
    // 10 blocks in 5 periods better.
    if (address != 0) {
      constexpr uintptr_t position_offset = offsetof(EntityData, position);
      Ukia::ProcessMgr.ReadMemory(address + position_offset, data.position);
    }
  }

  void RefreshFullData() {
    if (address != 0) {
      memset(&data, 0, sizeof(data));
      Ukia::ProcessMgr.ReadMemory(address, data);

      uintptr_t nameAddr;
      Ukia::ProcessMgr.ReadMemory(
          Memory::nameListBase + 0x798 + (index * 0x2),
          nameAddr);
      name = Ukia::ProcessMgr.ReadString(nameAddr);

      last_full_update = std::chrono::steady_clock::now();
    }
  }

  bool IsValid() const {
    return address != 0 && data.health > 0 && data.health < 250 &&
           data.life_state == 0;
  }

  bool IsEnemy(int local_team) const { return data.team != local_team; }

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

    std::for_each(
        std::execution::par_unseq, back_buffer.entities.begin(),
        back_buffer.entities.end(), [&](Entity& ent) {
          const int i = &ent - &back_buffer.entities[0];
          ent.address = back_buffer.address_cache[i];
          ent.index = i;

          if (ent.last_full_update.time_since_epoch().count() == 0 ||
              std::chrono::steady_clock::now() - ent.last_full_update >
                  std::chrono::milliseconds(50)) {
            ent.RefreshFullData();
          } else {
            ent.RefreshCriticalData();
          }
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