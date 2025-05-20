#pragma once

#include <Windows.h>

#include <string>
#include <vector>

#define RANDOM_PADDING rand()
#ifndef _is_invalid
#define _is_invalid(v) \
  if (v == NULL) return false
#define _is_invalid(v, n) \
  if (v == NULL) return n
#endif
enum StatusCode {
  SUCCEED,
  FAILE_PROCESSID,
  FAILE_HPROCESS,
  FAILE_MODULE,
};
typedef struct _UNICODE_STRING {
  USHORT Length;
  USHORT MaximumLength;
  PWCH Buffer;
} UNICODE_STRING, *PUNICODE_STRING;

typedef struct _OBJECT_ATTRIBUTES {
  ULONG Length;
  HANDLE RootDirectory;
  PUNICODE_STRING ObjectName;
  ULONG Attributes;
  PVOID SecurityDescriptor;
  PVOID SecurityQualityOfService;
} OBJECT_ATTRIBUTES, *POBJECT_ATTRIBUTES;

typedef struct _CLIENT_ID {
  PVOID UniqueProcess;
  PVOID UniqueThread;
} CLIENT_ID, *PCLIENT_ID;

typedef struct _SYSTEM_HANDLE_TABLE_ENTRY_INFO {
  ULONG ProcessId;
  BYTE ObjectTypeNumber;
  BYTE Flags;
  USHORT Handle;
  PVOID Object;
  ACCESS_MASK GrantedAccess;
} SYSTEM_HANDLE, *PSYSTEM_HANDLE;

typedef struct _SYSTEM_HANDLE_INFORMATION {
  ULONG HandleCount;
  SYSTEM_HANDLE Handles[1];
} SYSTEM_HANDLE_INFORMATION, *PSYSTEM_HANDLE_INFORMATION;

typedef NTSTATUS(NTAPI* _NtDuplicateObject)(HANDLE SourceProcessHandle,
                                            HANDLE SourceHandle,
                                            HANDLE TargetProcessHandle,
                                            PHANDLE TargetHandle,
                                            ACCESS_MASK DesiredAccess,
                                            ULONG Attributes, ULONG Options);

typedef NTSTATUS(NTAPI* _RtlAdjustPrivilege)(ULONG Privilege, BOOLEAN Enable,
                                             BOOLEAN CurrentThread,
                                             PBOOLEAN Enabled);

typedef NTSYSAPI NTSTATUS(NTAPI* _NtOpenProcess)(
    PHANDLE ProcessHandle, ACCESS_MASK DesiredAccess,
    POBJECT_ATTRIBUTES ObjectAttributes, PCLIENT_ID ClientId);

typedef NTSTATUS(NTAPI* _NtQuerySystemInformation)(
    ULONG
        SystemInformationClass,  // your supposed to supply the whole class but
                                 // microsoft kept the enum mostly empty so I
                                 // just passed 16 instead for handle info.
                                 // Thats why you get a warning in your code btw
    PVOID SystemInformation, ULONG SystemInformationLength,
    PULONG ReturnLength);

typedef NTSTATUS(WINAPI* pNtReadVirtualMemory)(HANDLE ProcessHandle,
                                               PVOID BaseAddress, PVOID Buffer,
                                               ULONG NumberOfBytesToRead,
                                               PULONG NumberOfBytesRead);

typedef NTSTATUS(WINAPI* pNtWriteVirtualMemory)(HANDLE ProcessHandle,
                                                PVOID BaseAddress, PVOID Buffer,
                                                ULONG NumberOfBytesToWrite,
                                                PULONG NumberOfBytesWritten);

inline SYSTEM_HANDLE_INFORMATION* hInfo;  // holds the handle information
namespace UkiaData {
inline std::string strHWID = "";
}
constexpr uint32_t CompileTimeSeed();
namespace Ukia {
void HideConsole();
void ShowConsole();
void AntiDebugger(std::string log = "") noexcept;
std::wstring utf8ToUtf16(const std::string& utf8Str);
std::string getRandomPoem();
LPCWSTR getRandomPoemW();
std::string GetSelfPath();
bool HasHashReadyParameter(int argc, char* argv[]);
void PreUpdateHash(const std::string& exePath);
void PostUpdateHash(const std::string& exePath);
void RandomTitle() noexcept;
bool IsFullscreen(HWND hwnd);
std::string GenerateMacAddress() noexcept;
std::string GenerateDiskSerial() noexcept;
std::string GenerateHwId();
HANDLE UkiaOpenProcess(DWORD dwDesiredAccess, BOOL bInheritHandle,
                       DWORD dwProcessId);
BOOL UkiaReadProcessMemory(HANDLE hProcess, LPCVOID lpBaseAddress,
                           LPVOID lpBuffer, SIZE_T nSize,
                           SIZE_T* lpNumberOfBytesRead);
BOOL UkiaWriteProcessMemory(HANDLE hProcess, LPVOID lpBaseAddress,
                            LPCVOID lpBuffer, SIZE_T nSize,
                            SIZE_T* lpNumberOfBytesWritten);

class ProcessManager {
 private:
  bool attached_ = false;

 public:
  HANDLE hProcess = 0;
  DWORD ProcessID = 0;
  DWORD64 ModuleAddress = 0;

 public:
  inline ~ProcessManager() {
    if (hProcess) CloseHandle(hProcess);
  }

  SYSTEM_HANDLE_INFORMATION* t_SYSTEM_HANDLE_INFORMATION;
  HANDLE Source_Process = NULL;
  HANDLE target_handle = NULL;

  StatusCode Attach(std::string ProcessName);
  void Detach();
  HWND GetWindowHandleFromProcessId(DWORD ProcessId);
  bool IsActive();
  template <typename ReadType>
  inline bool ReadMemory(DWORD64 Address, ReadType& Value, int Size) {
    _is_invalid(hProcess, false);
    _is_invalid(ProcessID, false);

    if (UkiaReadProcessMemory(hProcess, reinterpret_cast<LPCVOID>(Address),
                              &Value, Size, 0))
      return true;
    return false;
  }

  template <typename ReadType>
  inline bool ReadMemory(DWORD64 Address, ReadType& Value) {
    _is_invalid(hProcess, false);
    _is_invalid(ProcessID, false);

    if (UkiaReadProcessMemory(hProcess, reinterpret_cast<LPCVOID>(Address),
                              &Value, sizeof(ReadType), 0))
      return true;
    return false;
  }

  template <typename ReadType>
  inline bool WriteMemory(DWORD64 Address, ReadType& Value, int Size) {
    _is_invalid(hProcess, false);
    _is_invalid(ProcessID, false);

    if (UkiaWriteProcessMemory(hProcess, reinterpret_cast<LPCVOID>(Address),
                               &Value, Size, 0))
      return true;
    return false;
  }

  template <typename ReadType>
  inline bool WriteMemory(DWORD64 Address, ReadType& Value) {
    _is_invalid(hProcess, false);
    _is_invalid(ProcessID, false);

    if (UkiaWriteProcessMemory(hProcess, reinterpret_cast<LPVOID>(Address),
                               &Value, sizeof(ReadType), 0))
      return true;
    return false;
  }
  std::string ReadString(DWORD64 address, size_t maxLength = 256);
  DWORD GetProcessID(std::string ProcessName);
  DWORD64 TraceAddress(DWORD64 BaseAddress, std::vector<DWORD> Offsets);
  HMODULE GetProcessModuleHandle(std::string ModuleName);
};
inline ProcessManager ProcessMgr;

BOOL WINAPI ConsoleCtrlHandler(DWORD dwCtrlType);
int UkiaInit(int argc, char* argv[]);
int UkiaExit(DWORD code = 0);
}  // namespace Ukia