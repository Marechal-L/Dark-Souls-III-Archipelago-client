#include "Core.h"
#include "GameHook.h"

CCore* Core;
CGameHook* GameHook;
CItemRandomiser* ItemRandomiser;
SCore* CoreStruct;

VOID CCore::Start() {

	Core = new CCore();
	CoreStruct = new SCore();
	GameHook = new CGameHook();
	ItemRandomiser = new CItemRandomiser();

	CoreStruct->hHeap = HeapCreate(8, 0x10000, 0);
	if (!CoreStruct->hHeap) {
		Core->Panic("Unable to allocate appropriate heap", "...\\Randomiser\\Core\\Core.cpp", FE_MemError, 1);
		int3
	};

	if (!Core->Initialise()) {
		Core->Panic("Failed to initialise", "...\\Randomiser\\Core\\Core.cpp", FE_InitFailed, 1);
		int3
	};

	while (true) {
		Core->Run();
		Sleep(1000);
	};

	if (!HeapFree(CoreStruct->hHeap, 8, CoreStruct->pItemArray)) {
		Core->Panic("Given memory block appears invalid, or freed already", "...\\Randomiser\\Core\\Core.cpp", FE_InitFailed, 1);
		int3
	};

	HeapDestroy(CoreStruct->hHeap);

	delete CoreStruct;
	delete Core;

	return;
};

BOOL CCore::Initialise() {

	//Setup the client console
	FILE* fp;
	AllocConsole();
	SetConsoleTitleA("Dark Souls III - Item Randomiser Console");
	freopen_s(&fp, "CONOUT$", "w", stdout);
	freopen_s(&fp, "CONIN$", "r", stdin);
	printf_s("Starting DS3 ...\n");
	
	//Start command prompt
	CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)Core->InputCommand, NULL, NULL, NULL);


	//Inject custom shell codes
	BOOL initResult = GameHook->initialize();

	return true;
}


VOID CCore::Run() {

	GameHook->updateRuntimeValues();
	if(GameHook->healthPointRead != 0) {
		printf("healthPoint : %d\n", GameHook->healthPoint);
	}

	return;
};



VOID CCore::Panic(const char* pMessage, const char* pSort, DWORD dError, DWORD dIsFatalError) {

	char pOutput[MAX_PATH];
	char pTitle[MAX_PATH];

	sprintf_s(pOutput, "%s -> %s (%i)", pSort, pMessage, dError);

	if (IsDebuggerPresent()) {
		OutputDebugStringA(pOutput);
	};

	if (CoreStruct->dIsDebug) {
		printf_s("CCore::Panic is outputting debug-mode error information\n");
		sprintf_s(pOutput, "%s\n", pOutput);
		printf_s(pOutput);
	}
	else {
		if (dIsFatalError) {
			sprintf_s(pTitle, "[Item Randomiser - Fatal Error]");
		}
		else {
			sprintf_s(pTitle, "[Item Randomiser - Error]");
		};

		MessageBoxA(NULL, pOutput, pTitle, MB_ICONERROR);
	};

	if (dIsFatalError) *(int*)0 = 0;

	return;
};


VOID CCore::InputCommand() {
	while (true) {
		std::string line;
		std::getline(std::cin, line);

		if (line == "/help") {
			printf("List of available commands : \n");
			printf("/help : Prints this help message.\n");
		}
	}
};


