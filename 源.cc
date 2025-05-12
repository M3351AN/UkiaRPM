#include <chrono>
#include <filesystem>
#include <iomanip>
#include <ShlObj.h>
#include <stdio.h>
#include "UkiaStuff.h"
#include "Utils/XorStr.h"

using namespace std;

namespace offsets {
	constexpr std::uintptr_t ctAddress = 0x0034EC10;
	std::vector<uintptr_t> ptrOffsets = { 0x7f8, 0x28, 0xd0, 0x18, 0xf8, 0x18, 0x460, };
}

int Mian()
{
#ifdef NDEBUG
	Ukia::AntiDebugger(XorStr("Initialize fail"));
#endif

	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);	//Gets a standard output device handle  

	srand((unsigned)time(NULL));
	Ukia::RandomTitle();

	printf(XorStr("Build-%s-%s\n"), __DATE__, __TIME__);
	string strHWID = Ukia::GenHwid();
	printf(XorStr("%s\n"), strHWID.substr(strHWID.length() - 16).c_str());
	
	DWORD processId = Ukia::GetProc(L"Tutorial-x86_64.exe");

	uintptr_t baseAddress = Ukia::GetModuleBaseAddr(processId, L"Tutorial-x86_64.exe");

	HANDLE processHandle = OpenProcess(PROCESS_VM_OPERATION | PROCESS_VM_READ | PROCESS_VM_WRITE, FALSE, processId);

	uintptr_t ptrAddress = baseAddress + offsets::ctAddress;
	uintptr_t valAddr = Ukia::DeRefPtr<uintptr_t>(processHandle, ptrAddress, offsets::ptrOffsets);
	//Sleep(3000);
	while (true) {
		long long iHP = -1;
		long long iVal;
		while (iHP) {
			system("cls");
			printf(XorStr("剟勻天切及白央件犯奈扑亦件手�垓蕭鵊馱峇縣嶀鋓\n"));
            iVal = Ukia::ReadAddr<long long>(processHandle, valAddr);
			std::cout << XorStr("You have HP: ") << (iVal >= 100 ? XorStr("\033[32m") : XorStr("\033[31m")) << iVal << XorStr("\033[0m") << XorStr(" now.\n");
			iHP = 0;
		}
		printf(XorStr("How much HP u wanna gain?\n"));
		std::cin >> iHP;
		iVal = Ukia::ReadAddr<long long>(processHandle, valAddr);//User may cost / gain HP before enter.
		Ukia::WriteAddr<long long>(processHandle, valAddr, iHP + iVal);
		Sleep(250);
	}

	return 0;
}

int main(){ return Mian(); }