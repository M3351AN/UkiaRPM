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