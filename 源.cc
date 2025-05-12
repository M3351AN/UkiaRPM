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
	std::vector<DWORD> ptrOffsets = { 0x7f8 };//{ 0x460, 0x18, 0xf8, 0x18, 0xd0, 0x28, 0x7f8 };OOOOOOPPPPPS LOOP
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
	
	if (!Ukia::ProcessMgr.Attach(XorStr("Tutorial-x86_64.exe")))
		return -1;
	DWORD processId = Ukia::ProcessMgr.ProcessID;

	uintptr_t baseAddress = reinterpret_cast<uintptr_t>(Ukia::ProcessMgr.GetProcessModuleHandle(XorStr("Tutorial-x86_64.exe")));

	uintptr_t ptrAddress = baseAddress + offsets::ctAddress;
	uintptr_t valAddr = Ukia::ProcessMgr.TraceAddress(ptrAddress, offsets::ptrOffsets);
	//Sleep(3000);
	while (true) {
		long long iHP = -1;
		long long iVal;
		while (iHP) {
			system("cls");
			printf(XorStr("嘘っぱちのファンデーションも全部今夜のためよ~\n"));
			Ukia::ProcessMgr.ReadMemory(valAddr, iVal);
			std::cout << XorStr("You have HP: ") << (iVal >= 100 ? XorStr("\033[32m") : XorStr("\033[31m")) << iVal << XorStr("\033[0m") << XorStr(" now.\n");
			iHP = 0;
		}
		printf(XorStr("How much HP u wanna gain?\n"));
		std::cin >> iHP;
		Ukia::ProcessMgr.ReadMemory(valAddr, iVal);//User may cost / gain HP before enter.
		iHP += iVal;
		Ukia::ProcessMgr.WriteMemory(valAddr, iHP);
		Sleep(250);
	}

	return 0;
}

int main(){ return Mian(); }