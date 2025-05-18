#include <ShlObj.h>
#include <mmsystem.h>
#include <stdio.h>
#include <windows.h>

#include <array>
#include <atomic>
#include <chrono>
#include <cmath>
#include <execution>
#include <filesystem>
#include <iomanip>
#include <iostream>
#include <limits>
#include <mutex>
#include <thread>
#include <unordered_map>
#include <vector>

#include "Functions/Func.h"
#include "Utils/uiaccess.h"
#pragma comment(lib, "winmm.lib")

enum ZBID {
  ZBID_DEFAULT = 0,
  ZBID_DESKTOP = 1,
  ZBID_UIACCESS = 2,
  ZBID_IMMERSIVE_IHM = 3,
  ZBID_IMMERSIVE_NOTIFICATION = 4,
  ZBID_IMMERSIVE_APPCHROME = 5,
  ZBID_IMMERSIVE_MOGO = 6,
  ZBID_IMMERSIVE_EDGY = 7,
  ZBID_IMMERSIVE_INACTIVEMOBODY = 8,
  ZBID_IMMERSIVE_INACTIVEDOCK = 9,
  ZBID_IMMERSIVE_ACTIVEMOBODY = 10,
  ZBID_IMMERSIVE_ACTIVEDOCK = 11,
  ZBID_IMMERSIVE_BACKGROUND = 12,
  ZBID_IMMERSIVE_SEARCH = 13,
  ZBID_GENUINE_WINDOWS = 14,
  ZBID_IMMERSIVE_RESTRICTED = 15,
  ZBID_SYSTEM_TOOLS = 16,
  // Win10
  ZBID_LOCK = 17,
  ZBID_ABOVELOCK_UX = 18,
};

typedef HWND(WINAPI* CreateWindowInBand)(
    _In_ DWORD dwExStyle, _In_opt_ ATOM atom, _In_opt_ LPCWSTR lpWindowName,
    _In_ DWORD dwStyle, _In_ int X, _In_ int Y, _In_ int nWidth,
    _In_ int nHeight, _In_opt_ HWND hWndParent, _In_opt_ HMENU hMenu,
    _In_opt_ HINSTANCE hInstance, _In_opt_ LPVOID lpParam, DWORD band);

CreateWindowInBand pCreateWindowInBand =
    reinterpret_cast<CreateWindowInBand>(GetProcAddress(
        LoadLibraryA(XorStr("user32.dll")), XorStr("CreateWindowInBand")));

void JustGetWindowRect() {
  if (Ukia::IsFullscreen(global::hwnd_)) {
    global::screenSize.x = GetSystemMetrics(SM_CXSCREEN);
    global::screenSize.y = GetSystemMetrics(SM_CYSCREEN);
    global::screenPos.x = 0;
    global::screenPos.y = 0;
  } else {
    RECT clientRect;
    if (GetClientRect(global::hwnd_, &clientRect)) {
      int clientWidth = clientRect.right - clientRect.left;
      int clientHeight = clientRect.bottom - clientRect.top;

      global::screenSize.x = clientWidth;
      global::screenSize.y = clientHeight;
    } else {
      MessageBoxA(nullptr, XorStr("Failed to get window rect."),
                  XorStr("UkiaRPM"), MB_OK);
      Ukia::UkiaExit();
    }
  }
}

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd,
                                                             UINT msg,
                                                             WPARAM wParam,
                                                             LPARAM lParam);
LRESULT CALLBACK WinProc(HWND hWnd, UINT Message, WPARAM wParam,
                         LPARAM lParam) {
  if (ImGui_ImplWin32_WndProcHandler(hWnd, Message, wParam, lParam))
    return true;

  switch (Message) {
    case WM_DESTROY:
      if (DirectX9Interface::pDevice != NULL) {
        DirectX9Interface::pDevice->EndScene();
        DirectX9Interface::pDevice->Release();
      }
      if (DirectX9Interface::Direct3D9 != NULL) {
        DirectX9Interface::Direct3D9->Release();
      }
      PostQuitMessage(0);
      Ukia::UkiaExit(4);
      break;
    case WM_MOVE:
    case WM_SIZE:
      if (DirectX9Interface::pDevice != NULL && wParam != SIZE_MINIMIZED) {
        if (LOWORD(lParam) > 0 && HIWORD(lParam) > 0) {
          ImGui_ImplDX9_InvalidateDeviceObjects();
          DirectX9Interface::pParams.BackBufferWidth = LOWORD(lParam);
          DirectX9Interface::pParams.BackBufferHeight = HIWORD(lParam);
        }
      }
      break;
    default:
      return DefWindowProc(hWnd, Message, wParam, lParam);
      break;
  }
  return 0;
}

std::mutex g_d3dMutex;
struct WindowStateTracker {
  RECT oldRect = {0};
  bool lastMenuState = false;
  bool wasGameFocused = true;
  bool lastFullscreen = false;

  template <typename T>
  void UpdateWindowState(T&& checker) {
    lastFullscreen = checker();
  }
};

bool HandleFocusState(bool& wasFocused) {
  const HWND foreground = GetForegroundWindow();
  const bool focused =
      (foreground == global::hwnd_) || (foreground == OverlayWindow::Hwnd);

  if (focused != wasFocused) {
    ShowWindow(OverlayWindow::Hwnd, focused ? SW_SHOW : SW_HIDE);
    if (focused) {
      SetWindowPos(OverlayWindow::Hwnd, HWND_TOPMOST, 0, 0, 0, 0,
                   SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
    }
    wasFocused = focused;
  }
  return focused;
}

void SyncMenuState(bool& lastState) {
  if (config::ShowMenu == lastState) return;

  DWORD newExStyle = WS_EX_TOPMOST | WS_EX_LAYERED;
  newExStyle |= config::ShowMenu ? 0 : WS_EX_TRANSPARENT;

  SetWindowLongPtr(OverlayWindow::Hwnd, GWL_EXSTYLE, newExStyle);

  SetWindowPos(OverlayWindow::Hwnd, HWND_TOPMOST, 0, 0, 0, 0,
               SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);

  if (!config::ShowMenu) {
    SetForegroundWindow(global::hwnd_);
    SetActiveWindow(global::hwnd_);
    SetFocus(global::hwnd_);
  }

  lastState = config::ShowMenu;
}

void ProcessMessageQueue() {
  MSG msg;
  while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }
}

void SyncOverlayPosition(WindowStateTracker& stateTracker) {
  std::lock_guard<std::mutex> lock(g_d3dMutex);
  RECT clientRect;
  GetClientRect(global::hwnd_, &clientRect);
  POINT clientPos{0};
  ClientToScreen(global::hwnd_, &clientPos);

  bool positionChanged = (clientPos.x != stateTracker.oldRect.left) ||
                         (clientPos.y != stateTracker.oldRect.top);
  bool sizeChanged =
      (clientRect.right - clientRect.left != stateTracker.oldRect.right) ||
      (clientRect.bottom - clientRect.top != stateTracker.oldRect.bottom);

  if (positionChanged || sizeChanged) {
    SetWindowPos(OverlayWindow::Hwnd, HWND_TOPMOST, clientPos.x, clientPos.y,
                 clientRect.right, clientRect.bottom,
                 SWP_NOZORDER | SWP_NOACTIVATE);

    global::screenSize.x = clientRect.right;
    global::screenSize.y = clientRect.bottom;
    stateTracker.oldRect = {0, 0, clientRect.right, clientRect.bottom};

    global::screenSize.x = clientRect.right;
    global::screenSize.y = clientRect.bottom;

    DirectX9Interface::pParams.BackBufferWidth = global::screenSize.x;
    DirectX9Interface::pParams.BackBufferHeight = global::screenSize.y;
  }
}

void UpdateInputState() {
  ImGuiIO& io = ImGui::GetIO();

  POINT cursorPos{0};
  GetCursorPos(&cursorPos);
  ScreenToClient(global::hwnd_, &cursorPos);
  io.MousePos =
      ImVec2(static_cast<float>(cursorPos.x), static_cast<float>(cursorPos.y));
  io.MouseDown[0] = (GetAsyncKeyState(VK_LBUTTON) & 0x8000) != 0;

  static std::unordered_map<UINT, bool> keyStates;

  const UINT vkDelete = VK_DELETE;
  bool currentState = (GetAsyncKeyState(vkDelete) & 0x8000) != 0;

  if (currentState && !keyStates[vkDelete]) {
    config::ShowMenu = !config::ShowMenu;
  }
  keyStates[vkDelete] = currentState;

  io.KeyCtrl = (GetAsyncKeyState(VK_CONTROL) & 0x8000) != 0;
  io.KeyShift = (GetAsyncKeyState(VK_SHIFT) & 0x8000) != 0;
  io.KeyAlt = (GetAsyncKeyState(VK_MENU) & 0x8000) != 0;
  io.KeysDown[vkDelete] = currentState;
}

void HandlePresentResult(HRESULT result) {
  if (result == D3DERR_DEVICELOST &&
      DirectX9Interface::pDevice->TestCooperativeLevel() ==
          D3DERR_DEVICENOTRESET) {
    ImGui_ImplDX9_InvalidateDeviceObjects();
    DirectX9Interface::pDevice->Reset(&DirectX9Interface::pParams);
    ImGui_ImplDX9_CreateDeviceObjects();
  }
}

void RenderFrame() {
  std::lock_guard<std::mutex> lock(g_d3dMutex);
  ImGui_ImplDX9_NewFrame();
  ImGui_ImplWin32_NewFrame();
  ImGui::NewFrame();

  RenderFunctions(entity_list);  // 主渲染逻辑

  ImGui::EndFrame();

  if (SUCCEEDED(DirectX9Interface::pDevice->BeginScene())) {
    DirectX9Interface::pDevice->Clear(0, NULL, D3DCLEAR_TARGET,
                                      D3DCOLOR_ARGB(0, 0, 0, 0), 1.0f, 0);
    ImGui::Render();
    ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
    DirectX9Interface::pDevice->EndScene();
  }

  HandlePresentResult(
      DirectX9Interface::pDevice->Present(NULL, NULL, NULL, NULL));
}

void CleanupRenderResources() {
  ImGui_ImplDX9_Shutdown();
  ImGui_ImplWin32_Shutdown();
  ImGui::DestroyContext();

  if (DirectX9Interface::pDevice) {
    DirectX9Interface::pDevice->Release();
    DirectX9Interface::pDevice = nullptr;
  }

  if (OverlayWindow::Hwnd) {
    DestroyWindow(OverlayWindow::Hwnd);
    UnregisterClassA(OverlayWindow::WindowClass.lpszClassName,
                     OverlayWindow::WindowClass.hInstance);
  }
}

void MainLoop() {
  static WindowStateTracker stateTracker;
  MSG& msg = DirectX9Interface::Message;
  ZeroMemory(&msg, sizeof(MSG));

  while (msg.message != WM_QUIT) {
    if (!global::isRunning) break;

    if (config::BypassCapture)
      SetWindowDisplayAffinity(OverlayWindow::Hwnd, WDA_EXCLUDEFROMCAPTURE);
    else
      SetWindowDisplayAffinity(OverlayWindow::Hwnd, WDA_NONE);

    stateTracker.UpdateWindowState([&]() {
      const bool isFullscreen = Ukia::IsFullscreen(global::hwnd_);
      if (isFullscreen != stateTracker.lastFullscreen) {
        JustGetWindowRect();
        return true;
      }
      return false;
    });

    const bool gameIsFocused = HandleFocusState(stateTracker.wasGameFocused);
    if (!gameIsFocused) {
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
      continue;
    }

    SyncMenuState(stateTracker.lastMenuState);

    ProcessMessageQueue();

    SyncOverlayPosition(stateTracker);

    UpdateInputState();

    RenderFrame();
  }

  CleanupRenderResources();
}

bool DirectXInit() {
  if (FAILED(
          Direct3DCreate9Ex(D3D_SDK_VERSION, &DirectX9Interface::Direct3D9))) {
    return false;
  }

  D3DPRESENT_PARAMETERS Params = {0};
  Params.Windowed = TRUE;
  Params.SwapEffect = D3DSWAPEFFECT_DISCARD;
  Params.hDeviceWindow = OverlayWindow::Hwnd;
  Params.MultiSampleQuality = D3DMULTISAMPLE_NONE;
  Params.BackBufferFormat = D3DFMT_A8R8G8B8;
  Params.BackBufferWidth = global::screenSize.x;
  Params.BackBufferHeight = global::screenSize.y;
  Params.PresentationInterval = D3DPRESENT_INTERVAL_ONE;
  Params.EnableAutoDepthStencil = TRUE;
  Params.AutoDepthStencilFormat = D3DFMT_D16;
  Params.PresentationInterval = D3DPRESENT_INTERVAL_ONE;
  Params.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;

  if (FAILED(DirectX9Interface::Direct3D9->CreateDeviceEx(
          D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, OverlayWindow::Hwnd,
          D3DCREATE_HARDWARE_VERTEXPROCESSING, &Params, 0,
          &DirectX9Interface::pDevice))) {
    DirectX9Interface::Direct3D9->Release();
    return false;
  }

  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO();
  (void)io;
  ImGui::GetIO().WantCaptureMouse || ImGui::GetIO().WantTextInput ||
      ImGui::GetIO().WantCaptureKeyboard;
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
  io.Fonts->AddFontDefault();
  ImGui_ImplWin32_EnableDpiAwareness();
  ImGui_ImplWin32_Init(OverlayWindow::Hwnd);
  ImGui_ImplDX9_Init(DirectX9Interface::pDevice);
  LoadTextureFromMemory(DirectX9Interface::pDevice, ShigureImg,
                        sizeof(ShigureImg), &global::Shigure);
  DirectX9Interface::Direct3D9->Release();
  return true;
}

void SetupWindow() {
  OverlayWindow::WindowClass = {sizeof(WNDCLASSEX),
                                0,
                                WinProc,
                                0,
                                0,
                                nullptr,
                                LoadIcon(nullptr, IDI_APPLICATION),
                                LoadCursor(nullptr, IDC_ARROW),
                                nullptr,
                                nullptr,
                                OverlayWindow::Name,
                                LoadIcon(nullptr, IDI_APPLICATION)};

  RegisterClassExA(&OverlayWindow::WindowClass);
  if (global::hwnd_) {
    static RECT TempRect = {NULL};
    static POINT TempPoint;
    GetClientRect(global::hwnd_, &TempRect);
    ClientToScreen(global::hwnd_, &TempPoint);
    TempRect.left = TempPoint.x;
    TempRect.top = TempPoint.y;
    global::screenSize.x = TempRect.right;
    global::screenSize.y = TempRect.bottom;
  }

  JustGetWindowRect();  // again.

  if (global::uiAccessStatus != ERROR_SUCCESS)
    OverlayWindow::Hwnd = CreateWindowExA(
        WS_EX_TOPMOST, OverlayWindow::Name, OverlayWindow::Name,
        WS_POPUP | WS_VISIBLE, global::screenPos.x, global::screenPos.y,
        global::screenSize.x, global::screenSize.y, NULL, NULL, 0, NULL);
  else {
    WNDCLASSEXW wc = {
        sizeof(wc), CS_CLASSDC, WinProc, 0L,   0L,       GetModuleHandle(NULL),
        NULL,       NULL,       NULL,    NULL, L"Ukia?", NULL};
    auto res = RegisterClassExW(&wc);
    OverlayWindow::Hwnd = pCreateWindowInBand(
        WS_EX_TOPMOST, res, L"Ukia!", WS_POPUP | WS_VISIBLE,
        global::screenPos.x, global::screenPos.y, global::screenSize.x,
        global::screenSize.y, NULL, NULL, wc.hInstance, NULL, ZBID_UIACCESS);
  }

  DwmExtendFrameIntoClientArea(OverlayWindow::Hwnd, &DirectX9Interface::Margin);
  SetWindowLong(OverlayWindow::Hwnd, GWL_EXSTYLE,
                WS_EX_LAYERED | WS_EX_TRANSPARENT);
  ShowWindow(OverlayWindow::Hwnd, SW_SHOW);
  UpdateWindow(OverlayWindow::Hwnd);
}

void LogInfo() {
  printf(XorStr("UkiaRPM for Counter-Strike Source\n"));
  printf(
      XorStr("\u4eca\u65e5\u306f\u3084\u308b\u6c17\u306f\u306a\u3044\u3063"
             "\u3066\n\u30e9\u30c3\u30ad\u30fc\u30a2\u30a4\u30c6\u30e0\u3092"
             "\u6301\u3063\u3066\n"));
  printf(XorStr("Menukey [DEL]\n"));
  printf(XorStr("ProcessId: %d\nClientBase: %p\nEngineBase: %p\n"),
         global::processId, reinterpret_cast<void*>(Memory::clientAddress),
         reinterpret_cast<void*>(Memory::engineAddress));
}

bool InitializeGameProcess() {
  Ukia::ProcessMgr.Attach(XorStr("cstrike_win64.exe"));
  const DWORD processId = Ukia::ProcessMgr.ProcessID;
  if (!processId) return false;

  global::processId = processId;
  global::hwnd_ = Ukia::ProcessMgr.GetWindowHandleFromProcessId(processId);

  if (!Memory::UpdateAddress()) {
    MessageBoxA(nullptr, XorStr("Memory update failed"),
                XorStr("UkiaRPM Error"), MB_ICONERROR);
    return false;
  }

  LogInfo();
  return true;
}

class ScopedThreadManager {
 public:
  bool CreateThreads() {
    try {
      m_threads.emplace_back([&] { CheckAliveThread(); });
      m_threads.emplace_back([&] { EntityUpdateThread(); });
      m_threads.emplace_back([&] { MemoryProcessThread(); });
      m_threads.emplace_back([&] { Sonar::SoundThread(); });
      return true;
    } catch (const std::exception& e) {
      MessageBoxA(nullptr, e.what(), XorStr("Thread Creation Error"),
                  MB_ICONERROR);
      return false;
    }
  }

  ~ScopedThreadManager() {
    global::isRunning = false;
    for (auto& thread : m_threads) {
      if (thread.joinable()) thread.join();
    }
  }

 private:
  std::vector<std::thread> m_threads;

  void CheckAliveThread() {
    while (global::isRunning) {
      constexpr wchar_t EXPECTED_TITLE[] =
          L"Counter-Strike Source - Direct3D 9 - 64 Bit";
      wchar_t actualTitle[256] = {0};
      GetWindowTextW(global::hwnd_, actualTitle, _countof(actualTitle));
      global::isRunning = (wcscmp(actualTitle, EXPECTED_TITLE) == 0);
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
  }

  void EntityUpdateThread() {
    while (global::isRunning) {
      entity_list.UpdateAll();
      std::this_thread::sleep_for(std::chrono::milliseconds(15));
    }
  }

  void MemoryProcessThread() {
    while (global::isRunning) {
      MemoryFunctions(entity_list);
      std::this_thread::sleep_for(std::chrono::milliseconds(15));
    }
  }
};

bool WaitForGameFocus() {
  constexpr auto focusCheckInterval = std::chrono::milliseconds(15);
  auto startTime = std::chrono::steady_clock::now();

  while (global::isRunning) {
    DWORD foregroundPID = 0;
    GetWindowThreadProcessId(GetForegroundWindow(), &foregroundPID);

    if (foregroundPID == global::processId) {
      JustGetWindowRect();
      return true;
    }

    if (std::chrono::steady_clock::now() - startTime >
        std::chrono::seconds(30)) {
      MessageBoxA(nullptr, XorStr("Focus wait timeout"),
                  XorStr("UkiaRPM Error"), MB_ICONERROR);
      return false;
    }
    std::this_thread::sleep_for(focusCheckInterval);
  }
  return false;
}

bool InitializeRendering() {
  OverlayWindow::Name = XorStr("Ukia.");
  SetupWindow();

  if (!DirectXInit()) {
    MessageBoxA(nullptr, XorStr("DirectX initialization failed"),
                XorStr("UkiaRPM Error"), MB_ICONERROR);
    return false;
  }

  return true;
}

void RunMainLoop() {
  try {
    while (global::isRunning) {
      MainLoop();
    }
  } catch (const std::exception& e) {
    MessageBoxA(nullptr, e.what(), XorStr("Rendering Error"), MB_ICONERROR);
  }
}

void CleanupResources() {
  if (OverlayWindow::Hwnd) {
    DestroyWindow(OverlayWindow::Hwnd);
    UnregisterClassA(OverlayWindow::WindowClass.lpszClassName,
                     OverlayWindow::WindowClass.hInstance);
    Ukia::UkiaExit();
  }
}

int Mian() {
  global::uiAccessStatus = PrepareForUIAccess();

  if (!InitializeGameProcess()) {
    MessageBoxA(nullptr, XorStr("Failed to initialize game process"),
                XorStr("UkiaRPM Error"), MB_ICONERROR);
    return -1;
  }

  char documentsPath[MAX_PATH];
  if (SHGetFolderPathA(NULL, CSIDL_PERSONAL, NULL, 0, documentsPath) != S_OK) {
    MessageBoxA(nullptr, XorStr("Failed to get the Documents folder path."),
                XorStr("UkiaRPM Error"), MB_ICONERROR);
    Ukia::UkiaExit();
  }
  config::path = documentsPath;
  config::path += XorStr("\\UkiaRPM-CSS");
  if (std::filesystem::exists(config::path)) {
    printf(XorStr("Config folder connected: %s\n"), config::path.c_str());
  } else {
    if (std::filesystem::create_directories(config::path)) {
      printf(XorStr("Config folder created: %s\n"), config::path.c_str());
    } else {
      MessageBoxA(nullptr, XorStr("Failed to create the config directory."),
                  XorStr("UkiaRPM Error"), MB_ICONERROR);
      Ukia::UkiaExit();
    }
  }

  global::isRunning = true;

  if (!WaitForGameFocus()) {
    MessageBoxA(nullptr, XorStr("Wait game window focus time out"),
                XorStr("UkiaRPM Error"), MB_ICONERROR);
    return -1;
  }

  ScopedThreadManager threadManager;
  if (!threadManager.CreateThreads()) {
    MessageBoxA(nullptr, XorStr("Failed to create worker threads"),
                XorStr("UkiaRPM Error"), MB_ICONERROR);
    return -1;
  }

  if (!InitializeRendering()) {
    MessageBoxA(nullptr, XorStr("Failed to initialize rendering system"),
                XorStr("UkiaRPM Error"), MB_ICONERROR);
    return -1;
  }

  RunMainLoop();

  CleanupResources();
  return 0;
}

int main(int argc, char* argv[]) {
  Ukia::UkiaInit(argc, argv);
  return Mian();
}