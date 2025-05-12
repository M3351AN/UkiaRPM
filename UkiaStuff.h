#pragma once
#include <iostream>
#include <string>
#include <windows.h>
#include <sstream>
#include <iomanip>
#include <ShlObj.h>
#include <iphlpapi.h>
#include <Windows.h>
#include <vector>
#include <TlHelp32.h>
#include <psapi.h>
#define WIN32_LEAN_AND_MEAN
#pragma comment(lib, "iphlpapi.lib")
namespace Ukia
{
	void AntiDebugger(std::string Log = "") noexcept
	{
		if (IsDebuggerPresent())
		{
			if (Log != "")printf((Log + "\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n").c_str());
			ShowWindow(GetConsoleWindow(), false);
			exit(0);
		}
	}
	void RandomTitle()
	{
		constexpr int length = 25;
		const auto characters = TEXT("0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz`-=~!@#$%^&*()_+,./;'[]|{}:?");
		TCHAR title[length + 1]{};

		for (int j = 0; j != length; j++)
		{
			title[j] += characters[rand() % 95];
		}

		SetConsoleTitle(title);
	}
    inline std::string GenMac() {
        IP_ADAPTER_INFO AdapterInfo[16];
        DWORD dwBufLen = sizeof(AdapterInfo);
        DWORD dwStatus = GetAdaptersInfo(AdapterInfo, &dwBufLen);

        if (dwStatus != ERROR_SUCCESS) {
            return "Error";
        }

        PIP_ADAPTER_INFO pAdapterInfo = AdapterInfo;
        std::stringstream macAddress;

        do {
            macAddress << std::hex << std::uppercase
                << std::setw(2) << std::setfill('0') << static_cast<int>(pAdapterInfo->Address[0])
                << std::setw(2) << std::setfill('0') << static_cast<int>(pAdapterInfo->Address[1])
                << std::setw(2) << std::setfill('0') << static_cast<int>(pAdapterInfo->Address[2])
                << std::setw(2) << std::setfill('0') << static_cast<int>(pAdapterInfo->Address[3])
                << std::setw(2) << std::setfill('0') << static_cast<int>(pAdapterInfo->Address[4])
                << std::setw(2) << std::setfill('0') << static_cast<int>(pAdapterInfo->Address[5]);

            pAdapterInfo = pAdapterInfo->Next;
        } while (pAdapterInfo);

        return macAddress.str();
    }

    inline std::string GenDiskSerial() {
        DWORD serialNum;
        GetVolumeInformationA(
            "C:\\",
            NULL,
            0,
            &serialNum,
            NULL,
            NULL,
            NULL,
            0
        );

        std::stringstream ss;
        ss << serialNum;
        return ss.str();
    }

    static std::string GenHwid() {
        std::string strMac = GenMac();
        std::string strDiskSerial = GenDiskSerial();
        return strMac + strDiskSerial;
    }

    DWORD GetProc(const wchar_t* Target) {
        HANDLE snapshotHandle = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

        if (snapshotHandle == INVALID_HANDLE_VALUE) {
            return NULL;
        }

        PROCESSENTRY32W processEntry = { };
        processEntry.dwSize = sizeof(PROCESSENTRY32W);

        if (Process32FirstW(snapshotHandle, &processEntry)) {

            do {

                if (_wcsicmp(processEntry.szExeFile, Target) == 0) {
                    CloseHandle(snapshotHandle);
                    return processEntry.th32ProcessID;
                }

            } while (Process32NextW(snapshotHandle, &processEntry));

        }

        CloseHandle(snapshotHandle);
        return NULL;

    }

    MODULEENTRY32W GetModuleBase(DWORD processId, const wchar_t* ModuleTarget) {

        HANDLE snapshotHandle = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, processId);

        MODULEENTRY32W moduleEntry = { };

        if (snapshotHandle == INVALID_HANDLE_VALUE) {
            MODULEENTRY32W module = { 0UL };
            return module;
        }


        moduleEntry.dwSize = sizeof(MODULEENTRY32W);

        if (Module32FirstW(snapshotHandle, &moduleEntry)) {

            do {

                if (_wcsicmp(moduleEntry.szModule, ModuleTarget) == 0) {
                    CloseHandle(snapshotHandle);
                    return moduleEntry;
                }

            } while (Module32NextW(snapshotHandle, &moduleEntry));

        }

        CloseHandle(snapshotHandle);
        MODULEENTRY32W module = { 0UL };
        return module;

    }

    uintptr_t GetModuleBaseAddr(DWORD processId, const wchar_t* ModuleTarget) {

        HANDLE snapshotHandle = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, processId);

        MODULEENTRY32W moduleEntry = { };

        if (snapshotHandle == INVALID_HANDLE_VALUE) {
            return NULL;
        }

        moduleEntry.dwSize = sizeof(MODULEENTRY32W);

        if (Module32FirstW(snapshotHandle, &moduleEntry)) {

            do {

                if (_wcsicmp(moduleEntry.szModule, ModuleTarget) == 0) {
                    CloseHandle(snapshotHandle);
                    return reinterpret_cast<uintptr_t>(moduleEntry.modBaseAddr);
                }

            } while (Module32NextW(snapshotHandle, &moduleEntry));

        }

        CloseHandle(snapshotHandle);
        return NULL;

    }

    template <typename T>
    T ReadAddr(HANDLE processHandle, uintptr_t ptrAddress) {
        T value = { };
        ReadProcessMemory(processHandle, (LPVOID)ptrAddress, &value, sizeof(T), NULL);
        return value;
    }

    template <typename T>
    bool WriteAddr(HANDLE processHandle, uintptr_t ptrAddress, T value) {

        return WriteProcessMemory(processHandle, (LPVOID)ptrAddress, &value, sizeof(T), NULL);

    }

    template<typename T>
    T DeRefPtr(HANDLE processHandle, uintptr_t baseAddr, const std::vector<uintptr_t>& offset) {
        uintptr_t addr = baseAddr;
        for (size_t i = 0; i < offset.size(); i++) {

            addr = ReadAddr<uintptr_t>(processHandle, addr);
            addr += offset[offset.size() - i - 1];//The ptr offsets in CT tablez are in reverse order.
        }

        return addr;
    }
}