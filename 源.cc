#include <chrono>
#include <filesystem>
#include <iomanip>
#include <ShlObj.h>
#include <stdio.h>
#include "UkiaStuff.h"
#include "Utils/XorStr.h"

using namespace std;

namespace offsets {
	/*<?xml version="1.0" encoding="utf-8"?>
	<CheatTable CheatEngineTableVersion="46">
	  <CheatEntries>
		<CheatEntry>
		  <ID>0</ID>
		  <Description>"REL"</Description>
		  <VariableType>4 Bytes</VariableType>
		  <Address>"mono.dll"+00296BC8</Address>
		  <Offsets>
			<Offset>E8</Offset>
			<Offset>0</Offset>
			<Offset>330</Offset>
			<Offset>20</Offset>
		  </Offsets>
		</CheatEntry>
		<CheatEntry>
		  <ID>1</ID>
		  <Description>"VIEW"</Description>
		  <VariableType>4 Bytes</VariableType>
		  <Address>"mono.dll"+00296BC8</Address>
		  <Offsets>
			<Offset>F8</Offset>
			<Offset>0</Offset>
			<Offset>80</Offset>
			<Offset>38</Offset>
			<Offset>F8</Offset>
			<Offset>318</Offset>
			<Offset>1D0</Offset>
		  </Offsets>
		</CheatEntry>
	  </CheatEntries>
	  <UserdefinedSymbols/>
	  <Comments>Info about this table:
	</Comments>
	</CheatTable>
	*/
	constexpr std::uintptr_t ctAddress = 0x00296BC8;
	std::vector<uintptr_t> relOffsets = { 0xE8, 0x0, 0x330, 0x20 };
	std::vector<uintptr_t> viewOffsets = { 0xF8, 0x0, 0x80, 0x38, 0xF8, 0x318, 0x1D0 };
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
	
	DWORD processId = Ukia::GetProc(L"Cities.exe");

	uintptr_t baseAddress = Ukia::GetModuleBaseAddr(processId, L"mono.dll");

	HANDLE processHandle = OpenProcess(PROCESS_VM_OPERATION | PROCESS_VM_READ | PROCESS_VM_WRITE, FALSE, processId);

	uintptr_t ptrAddress = baseAddress + offsets::ctAddress;
	uintptr_t relAddr = Ukia::DeRefPtr<uintptr_t>(processHandle, ptrAddress, offsets::relOffsets);
	uintptr_t viewAddr = relAddr + 0x10;//Ukia::DeRefPtr<uintptr_t>(processHandle, ptrAddress, offsets::viewOffsets);
	//Sleep(3000);
	while (true) {
		long long iMoney = -1;
		long long viewVal;
		while (iMoney) {
			system("cls");
			printf(XorStr("剟勻天切及白央件犯奈扑亦件手�垓蕭鵊馱峇縣嶀鋓\n"));
            //relVal = Ukia::ReadAddr<long long>(processHandle, relAddr);
            viewVal = Ukia::ReadAddr<long long>(processHandle, viewAddr);
			//std::cout << "relVal:" << relVal << "\n";
			//std::cout << "viewVal:" << viewVal << "\n";
			std::cout << XorStr("You have money: ") << (viewVal > 0 ? XorStr("\033[32m") : XorStr("\033[31m")) << viewVal / 100 << XorStr("\033[0m") << XorStr(" now.\n");//Yes 100 times, it's correct.
			iMoney = 0;
		}
		printf(XorStr("How much u wanna gain?\n"));
		std::cin >> iMoney;
		iMoney = iMoney * 100;
		//relVal = Ukia::ReadAddr<long long>(processHandle, relAddr);
		viewVal = Ukia::ReadAddr<long long>(processHandle, viewAddr);//User may cost / gain money before enter.
		Ukia::WriteAddr<long long>(processHandle, relAddr, iMoney + viewVal);
		Ukia::WriteAddr<long long>(processHandle, viewAddr, iMoney + viewVal);
		Sleep(250);
	}

	return 0;
}

int main(){ return Mian(); }