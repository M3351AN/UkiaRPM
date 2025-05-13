#pragma once

#include <Windows.h>
#include <atlconv.h>
#include <iomanip>
#include <iostream>
#include <iphlpapi.h>
#include <memory>
#include <psapi.h>
#include <sstream>
#include <string>
#include <TlHelp32.h>
#include <vector>

#define WIN32_LEAN_AND_MEAN
#pragma comment(lib, "iphlpapi.lib")

#define _is_invalid(v) if(v==NULL) return false
#define _is_invalid(v,n) if(v==NULL) return n

#define InitializeObjectAttributes( p, n, a, r, s ) { \
    (p)->Length = sizeof( OBJECT_ATTRIBUTES );          \
    (p)->RootDirectory = r;                             \
    (p)->Attributes = a;                                \
    (p)->ObjectName = n;                                \
    (p)->SecurityDescriptor = s;                        \
    (p)->SecurityQualityOfService = NULL;               \
    }

#define SeDebugPriv 20
#define NT_SUCCESS(Status) (((NTSTATUS)(Status)) >= 0)
#define STATUS_INFO_LENGTH_MISMATCH ((NTSTATUS)0xC0000004)
#define NtCurrentProcess ( (HANDLE)(LONG_PTR) -1 ) 
#define ProcessHandleType 0x7
#define SystemHandleInformation 16 

typedef struct _UNICODE_STRING {
	USHORT Length;
	USHORT MaximumLength;
	PWCH   Buffer;
} UNICODE_STRING, * PUNICODE_STRING;


typedef struct _OBJECT_ATTRIBUTES {
	ULONG           Length;
	HANDLE          RootDirectory;
	PUNICODE_STRING ObjectName;
	ULONG           Attributes;
	PVOID           SecurityDescriptor;
	PVOID           SecurityQualityOfService;
}  OBJECT_ATTRIBUTES, * POBJECT_ATTRIBUTES;

typedef struct _CLIENT_ID
{
	PVOID UniqueProcess;
	PVOID UniqueThread;
} CLIENT_ID, * PCLIENT_ID;

typedef struct _SYSTEM_HANDLE_TABLE_ENTRY_INFO
{
	ULONG ProcessId;
	BYTE ObjectTypeNumber;
	BYTE Flags;
	USHORT Handle;
	PVOID Object;
	ACCESS_MASK GrantedAccess;
} SYSTEM_HANDLE, * PSYSTEM_HANDLE;

typedef struct _SYSTEM_HANDLE_INFORMATION
{
	ULONG HandleCount;
	SYSTEM_HANDLE Handles[1];
} SYSTEM_HANDLE_INFORMATION, * PSYSTEM_HANDLE_INFORMATION;

typedef NTSTATUS(NTAPI* _NtDuplicateObject)(
	HANDLE SourceProcessHandle,
	HANDLE SourceHandle,
	HANDLE TargetProcessHandle,
	PHANDLE TargetHandle,
	ACCESS_MASK DesiredAccess,
	ULONG Attributes,
	ULONG Options
	);

typedef NTSTATUS(NTAPI* _RtlAdjustPrivilege)(
	ULONG Privilege,
	BOOLEAN Enable,
	BOOLEAN CurrentThread,
	PBOOLEAN Enabled
	);

typedef NTSYSAPI NTSTATUS(NTAPI* _NtOpenProcess)(
	PHANDLE            ProcessHandle,
	ACCESS_MASK        DesiredAccess,
	POBJECT_ATTRIBUTES ObjectAttributes,
	PCLIENT_ID         ClientId
	);

typedef NTSTATUS(NTAPI* _NtQuerySystemInformation)(
	ULONG SystemInformationClass, //your supposed to supply the whole class but microsoft kept the enum mostly empty so I just passed 16 instead for handle info. Thats why you get a warning in your code btw
	PVOID SystemInformation,
	ULONG SystemInformationLength,
	PULONG ReturnLength
	);

typedef NTSTATUS(WINAPI* pNtReadVirtualMemory)(
	HANDLE ProcessHandle,
	PVOID BaseAddress,
	PVOID Buffer,
	ULONG NumberOfBytesToRead,
	PULONG NumberOfBytesRead
	);

typedef NTSTATUS(WINAPI* pNtWriteVirtualMemory)(
	HANDLE ProcessHandle,
	PVOID BaseAddress,
	PVOID Buffer,
	ULONG NumberOfBytesToWrite,
	PULONG NumberOfBytesWritten
	);

inline SYSTEM_HANDLE_INFORMATION* hInfo; //holds the handle information

/// <summary>
/// ½ø³Ì×´Ì¬Âë
/// </summary>
enum StatusCode
{
	SUCCEED,
	FAILE_PROCESSID,
	FAILE_HPROCESS,
	FAILE_MODULE,
};


namespace Ukia
{
	void AntiDebugger(std::string log = "") noexcept
	{
		if (IsDebuggerPresent())
		{
			if (log != "")printf((log + "\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n").c_str());
			ShowWindow(GetConsoleWindow(), false);
			exit(0);
		}
	}
	void RandomTitle() noexcept
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
	std::string GenerateMacAddress() noexcept
	{
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

	std::string GenerateDiskSerial() noexcept
	{
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

	static std::string GenerateHwId() {
		std::string strMac = GenerateMacAddress();
		std::string strDiskSerial = GenerateDiskSerial();
		return strMac + strDiskSerial;
	}

	inline HANDLE procHandle = NULL;
	inline HANDLE hProcess = NULL;
	inline HANDLE HijackedHandle = NULL;

	// simple function i made that will just initialize our Object_Attributes structure as NtOpenProcess will fail otherwise
	inline OBJECT_ATTRIBUTES InitObjectAttributes(PUNICODE_STRING name, ULONG attributes, HANDLE hRoot, PSECURITY_DESCRIPTOR security)
	{
		OBJECT_ATTRIBUTES object;

		object.Length = sizeof(OBJECT_ATTRIBUTES);
		object.ObjectName = name;
		object.Attributes = attributes;
		object.RootDirectory = hRoot;
		object.SecurityDescriptor = security;

		return object;
	}

	inline bool IsHandleValid(HANDLE handle) // i made this to simply check if a handle is valid rather than repeating the if statments
	{
		if (handle && handle != INVALID_HANDLE_VALUE)
			return true;
		else
			return false;

	}

	inline HANDLE HijackExistingHandle(DWORD dwTargetProcessId)
	{
		HMODULE Ntdll = GetModuleHandleA("ntdll"); // get the base address of ntdll.dll

		//get the address of RtlAdjustPrivilege in ntdll.dll so we can grant our process the highest permission possible
		_RtlAdjustPrivilege RtlAdjustPrivilege = (_RtlAdjustPrivilege)GetProcAddress(Ntdll, "RtlAdjustPrivilege");

		boolean OldPriv; //store the old privileges

		// Give our program SeDeugPrivileges whcih allows us to get a handle to every process, even the highest privileged SYSTEM level processes.
		RtlAdjustPrivilege(SeDebugPriv, TRUE, FALSE, &OldPriv);

		//get the address of NtQuerySystemInformation in ntdll.dll so we can find all the open handles on our system
		_NtQuerySystemInformation NtQuerySystemInformation = (_NtQuerySystemInformation)GetProcAddress(Ntdll, "NtQuerySystemInformation");

		//get the address of NtDuplicateObject in ntdll.dll so we can duplicate an existing handle into our cheat, basically performing the hijacking
		_NtDuplicateObject NtDuplicateObject = (_NtDuplicateObject)GetProcAddress(Ntdll, "NtDuplicateObject");

		//get the address of NtOpenProcess in ntdll.dll so wecan create a Duplicate handle
		_NtOpenProcess NtOpenProcess = (_NtOpenProcess)GetProcAddress(Ntdll, "NtOpenProcess");


		//initialize the Object Attributes structure, you can just set each member to NULL rather than create a function like i did
		OBJECT_ATTRIBUTES Obj_Attribute = InitObjectAttributes(NULL, NULL, NULL, NULL);

		//clientID is a PDWORD or DWORD* of the process id to create a handle to
		CLIENT_ID clientID = { 0 };


		//the size variable is the amount of bytes allocated to store all the open handles
		DWORD size = sizeof(SYSTEM_HANDLE_INFORMATION);

		//we allocate the memory to store all the handles on the heap rather than the stack becuase of the large amount of data
		hInfo = (SYSTEM_HANDLE_INFORMATION*) new std::byte[size];

		//zero the memory handle info
		ZeroMemory(hInfo, size);

		//we use this for checking if the Native functions succeed
		NTSTATUS NtRet = NULL;

		do
		{
			// delete the previously allocated memory on the heap because it wasn't large enough to store all the handles
			delete[] hInfo;

			//increase the amount of memory allocated by 50%
			size *= 1.5;
			try
			{
				//set and allocate the larger size on the heap
				hInfo = (PSYSTEM_HANDLE_INFORMATION) new std::byte[size];
			}
			catch (std::bad_alloc) //catch a bad heap allocation.
			{
				procHandle ? CloseHandle(procHandle) : 0;
			}
			Sleep(1); //sleep for the cpu

			//we continue this loop until all the handles have been stored
		} while ((NtRet = NtQuerySystemInformation(SystemHandleInformation, hInfo, size, NULL)) == STATUS_INFO_LENGTH_MISMATCH);

		//check if we got all the open handles on our system
		if (!NT_SUCCESS(NtRet))
			procHandle ? CloseHandle(procHandle) : 0;


		//loop through each handle on our system, and filter out handles that are invalid or cant be hijacked
		for (unsigned int i = 0; i < hInfo->HandleCount; ++i)
		{
			//a variable to store the number of handles OUR cheat has open.
			static DWORD NumOfOpenHandles;

			//get the amount of outgoing handles OUR cheat has open
			GetProcessHandleCount(GetCurrentProcess(), &NumOfOpenHandles);

			//you can do a higher number if this is triggering false positives. Its just to make sure we dont [removed] up and create thousands of handles
			if (NumOfOpenHandles > 256)
				procHandle ? CloseHandle(procHandle) : 0;

			//check if the current handle is valid, otherwise increment i and check the next handle
			if (!IsHandleValid((HANDLE)hInfo->Handles[i].Handle))
				continue;

			//check the handle type is 0x7 meaning a process handle so we dont hijack a file handle for example
			if (hInfo->Handles[i].ObjectTypeNumber != ProcessHandleType)
				continue;

			//set clientID to a pointer to the process with the handle to out target
			clientID.UniqueProcess = (DWORD*)hInfo->Handles[i].ProcessId;

			//if procHandle is open, close it
			procHandle ? CloseHandle(procHandle) : 0;

			//create a a handle with duplicate only permissions to the process with a handle to our target. NOT OUR TARGET.
			NtRet = NtOpenProcess(&procHandle, PROCESS_DUP_HANDLE, &Obj_Attribute, &clientID);
			if (!IsHandleValid(procHandle) || !NT_SUCCESS(NtRet)) //check is the funcions succeeded and check the handle is valid
				continue;

			//we duplicate the handle another process has to our target into our cheat with whatever permissions we want. I did all access.
			NtRet = NtDuplicateObject(procHandle, (HANDLE)hInfo->Handles[i].Handle, NtCurrentProcess, &HijackedHandle, PROCESS_ALL_ACCESS, 0, 0);
			if (!IsHandleValid(HijackedHandle) || !NT_SUCCESS(NtRet))//check is the funcions succeeded and check the handle is valid
				continue;

			//get the process id of the handle we duplicated and check its to our target
			if (GetProcessId(HijackedHandle) != dwTargetProcessId) {
				CloseHandle(HijackedHandle);
				continue;
			}

			hProcess = HijackedHandle;

			break;
		}

		procHandle ? CloseHandle(procHandle) : 0;

		return hProcess;
	}

	HANDLE UkiaOpenProcess(DWORD dwDesiredAccess, BOOL bInheritHandle, DWORD dwProcessId)
	{
		//At last is same with "OpenProcess", but we call "NtOpenProcess" (Native API) directly.
		HANDLE hProcess = 0;
		_NtOpenProcess NtOpenProcess = (_NtOpenProcess)GetProcAddress(GetModuleHandleW(L"ntdll.dll"), "NtOpenProcess");
		CLIENT_ID clientId = { (HANDLE)dwProcessId, NULL };
		OBJECT_ATTRIBUTES objAttr;
		InitializeObjectAttributes(&objAttr, NULL, 0, NULL, NULL);
		NtOpenProcess(&hProcess, PROCESS_VM_OPERATION | PROCESS_VM_READ | PROCESS_VM_WRITE, &objAttr, &clientId);
		return hProcess;
	}

	BOOL UkiaReadProcessMemory(HANDLE hProcess, LPCVOID lpBaseAddress, LPVOID  lpBuffer, SIZE_T  nSize, SIZE_T* lpNumberOfBytesRead)
	{
		static pNtReadVirtualMemory NtReadVirtualMemory = []() {
			return reinterpret_cast<pNtReadVirtualMemory>(GetProcAddress(GetModuleHandleW(L"ntdll.dll"), "NtReadVirtualMemory"));
			}();

		if (!NtReadVirtualMemory) {
			SetLastError(ERROR_PROC_NOT_FOUND);
			return FALSE;
		}

		ULONG ulSize = static_cast<ULONG>(nSize);
		ULONG bytesRead = 0;

		NTSTATUS status = NtReadVirtualMemory(
			hProcess,
			const_cast<PVOID>(lpBaseAddress),
			lpBuffer,
			ulSize,
			lpNumberOfBytesRead ? &bytesRead : nullptr
		);

		if (lpNumberOfBytesRead) {
			*lpNumberOfBytesRead = static_cast<SIZE_T>(bytesRead);
		}

		if (status == SUCCEED) {
			return TRUE;
		}

		if (status == 0x8000000D) {
			SetLastError(ERROR_PARTIAL_COPY);
			return FALSE;
		}

		using RtlNtStatusToDosErrorFn = ULONG(WINAPI*)(NTSTATUS);
		static auto RtlNtStatusToDosError = reinterpret_cast<RtlNtStatusToDosErrorFn>(GetProcAddress(GetModuleHandleW(L"ntdll.dll"), "RtlNtStatusToDosError"));

		if (RtlNtStatusToDosError) {
			SetLastError(RtlNtStatusToDosError(status));
		}
		else {
			SetLastError(ERROR_UNIDENTIFIED_ERROR);
		}

		return FALSE;
	}

	BOOL UkiaWriteProcessMemory(HANDLE  hProcess, LPVOID  lpBaseAddress, LPCVOID lpBuffer, SIZE_T  nSize, SIZE_T* lpNumberOfBytesWritten)
	{

		static pNtWriteVirtualMemory NtWriteVirtualMemory = []() {
			return reinterpret_cast<pNtWriteVirtualMemory>(GetProcAddress(GetModuleHandleW(L"ntdll.dll"), "NtWriteVirtualMemory"));
			}();

		if (!NtWriteVirtualMemory) {
			SetLastError(ERROR_PROC_NOT_FOUND);
			return FALSE;
		}

		ULONG ulSize = static_cast<ULONG>(nSize);
		ULONG bytesWritten = 0;

		NTSTATUS status = NtWriteVirtualMemory(
			hProcess,
			lpBaseAddress,
			const_cast<PVOID>(lpBuffer),
			ulSize,
			lpNumberOfBytesWritten ? &bytesWritten : nullptr
		);

		if (lpNumberOfBytesWritten) {
			*lpNumberOfBytesWritten = static_cast<SIZE_T>(bytesWritten);
		}

		if (status == SUCCEED) {
			return TRUE;
		}

		switch (status) {
		case 0xC0000005:  // STATUS_ACCESS_VIOLATION
			SetLastError(ERROR_NOACCESS);
			break;
		case 0xC0000022:  // STATUS_ACCESS_DENIED
			SetLastError(ERROR_ACCESS_DENIED);
			break;
		case 0x8000000D:  // STATUS_PARTIAL_COPY
			SetLastError(ERROR_WRITE_FAULT);
			break;
		default: {
			using RtlNtStatusToDosErrorFn = ULONG(WINAPI*)(NTSTATUS);
			static auto RtlNtStatusToDosError = reinterpret_cast<RtlNtStatusToDosErrorFn>(GetProcAddress(GetModuleHandleW(L"ntdll.dll"), "RtlNtStatusToDosError"));

			if (RtlNtStatusToDosError) {
				SetLastError(RtlNtStatusToDosError(status));
			}
			else {
				SetLastError(ERROR_UNIDENTIFIED_ERROR);
			}
		}
		}

		return FALSE;
	}

	class ProcessManager
	{
	private:
		bool attached_ = false;

	public:

		HANDLE hProcess = 0;
		DWORD ProcessID = 0;
		DWORD64 ModuleAddress = 0;

	public:
		~ProcessManager()
		{
			if (hProcess)
				CloseHandle(hProcess);
		}
		SYSTEM_HANDLE_INFORMATION* t_SYSTEM_HANDLE_INFORMATION;
		HANDLE Source_Process = NULL;
		HANDLE target_handle = NULL;

		StatusCode Attach(std::string ProcessName)
		{
			ProcessID = this->GetProcessID(ProcessName);
			_is_invalid(ProcessID, FAILE_PROCESSID);
			ModuleAddress = reinterpret_cast<DWORD64>(this->GetProcessModuleHandle(ProcessName));
			_is_invalid(ModuleAddress, FAILE_MODULE);

			//hProcess = OpenProcess(PROCESS_VM_OPERATION | PROCESS_VM_READ | PROCESS_VM_WRITE, FALSE, ProcessID);

			hProcess = UkiaOpenProcess(PROCESS_VM_OPERATION | PROCESS_VM_READ | PROCESS_VM_WRITE, FALSE, ProcessID);

			//hProcess = HijackExistingHandle(ProcessID);failed, idk why.

			if (GetProcessId(target_handle) == ProcessID) {
				hProcess = target_handle;
				attached_ = true;
				delete[] t_SYSTEM_HANDLE_INFORMATION;
			}
			else
			{
				CloseHandle(target_handle);
				CloseHandle(Source_Process);
				return FAILE_HPROCESS;
			}
			return SUCCEED;
		}

		void Detach()
		{
			if (hProcess)
				CloseHandle(hProcess);
			hProcess = 0;
			ProcessID = 0;
			ModuleAddress = 0;
			attached_ = false;
		}

		bool IsActive()
		{
			if (!attached_)
				return false;
			DWORD ExitCode{};
			GetExitCodeProcess(hProcess, &ExitCode);
			return ExitCode == STILL_ACTIVE;
		}

		template <typename ReadType>
		bool ReadMemory(DWORD64 Address, ReadType& Value, int Size)
		{
			_is_invalid(hProcess, false);
			_is_invalid(ProcessID, false);

			if (UkiaReadProcessMemory(hProcess, reinterpret_cast<LPCVOID>(Address), &Value, Size, 0))
				return true;
			return false;
		}

		template <typename ReadType>
		bool ReadMemory(DWORD64 Address, ReadType& Value)
		{
			_is_invalid(hProcess, false);
			_is_invalid(ProcessID, false);

			if (UkiaReadProcessMemory(hProcess, reinterpret_cast<LPCVOID>(Address), &Value, sizeof(ReadType), 0))
				return true;
			return false;
		}

		template <typename ReadType>
		bool WriteMemory(DWORD64 Address, ReadType& Value, int Size)
		{

			_is_invalid(hProcess, false);
			_is_invalid(ProcessID, false);

			if (UkiaWriteProcessMemory(hProcess, reinterpret_cast<LPCVOID>(Address), &Value, Size, 0))
				return true;
			return false;
		}

		template <typename ReadType>
		bool WriteMemory(DWORD64 Address, ReadType& Value)
		{

			_is_invalid(hProcess, false);
			_is_invalid(ProcessID, false);

			if (UkiaWriteProcessMemory(hProcess, reinterpret_cast<LPVOID>(Address), &Value, sizeof(ReadType), 0))
				return true;
			return false;
		}

		//tewshi0 idea
		std::string ReadString(DWORD64 address, size_t maxLength = 256)
		{
			std::vector<char> buffer(maxLength, 0);

			if (!ReadMemory<char>(address, buffer[0], maxLength)) {
				return "";
			}

			buffer[maxLength - 1] = '\0';

			size_t actualLength = 0;
			while (actualLength < maxLength && buffer[actualLength] != '\0') {
				++actualLength;
			}

			return std::string(buffer.data(), actualLength);
		}
	public:

		DWORD GetProcessID(std::string ProcessName)
		{
			PROCESSENTRY32 ProcessInfoPE;
			ProcessInfoPE.dwSize = sizeof(PROCESSENTRY32);
			HANDLE hSnapshot = CreateToolhelp32Snapshot(15, 0);
			Process32First(hSnapshot, &ProcessInfoPE);
			USES_CONVERSION;
			do {
				if (strcmp(W2A(ProcessInfoPE.szExeFile), ProcessName.c_str()) == 0)
				{
					CloseHandle(hSnapshot);
					return ProcessInfoPE.th32ProcessID;
				}
			} while (Process32Next(hSnapshot, &ProcessInfoPE));
			CloseHandle(hSnapshot);
			return 0;
		}
		DWORD64 TraceAddress(DWORD64 BaseAddress, std::vector<DWORD> Offsets)
		{
			_is_invalid(hProcess, 0);
			_is_invalid(ProcessID, 0);
			DWORD64 Address = 0;

			if (Offsets.size() == 0)
				return BaseAddress;

			if (!ReadMemory<DWORD64>(BaseAddress, Address))
				return 0;

			for (int i = 0; i < Offsets.size() - 1; i++)
			{
				if (!ReadMemory<DWORD64>(Address + Offsets[i], Address))
					return 0;
			}
			return Address == 0 ? 0 : Address + Offsets[Offsets.size() - 1];
		}

		HMODULE GetProcessModuleHandle(std::string ModuleName)
		{
			MODULEENTRY32 ModuleInfoPE;
			ModuleInfoPE.dwSize = sizeof(MODULEENTRY32);
			HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, this->ProcessID);
			Module32First(hSnapshot, &ModuleInfoPE);
			USES_CONVERSION;
			do {
				if (strcmp(W2A(ModuleInfoPE.szModule), ModuleName.c_str()) == 0)
				{
					CloseHandle(hSnapshot);
					return ModuleInfoPE.hModule;
				}
			} while (Module32Next(hSnapshot, &ModuleInfoPE));
			CloseHandle(hSnapshot);
			return 0;
		}
	};

	inline ProcessManager ProcessMgr;
}