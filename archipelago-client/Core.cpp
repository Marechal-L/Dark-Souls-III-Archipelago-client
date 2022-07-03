#include "Core.h"

CCore* Core;
SCore* CoreStruct;

VOID CCore::Start() {

	Core = new CCore();
	CoreStruct = new SCore();

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

	FILE* fp;

	AllocConsole();
	SetConsoleTitleA("Dark Souls III - Item Randomiser Console");
	freopen_s(&fp, "CONOUT$", "w", stdout);
	freopen_s(&fp, "CONIN$", "r", stdin);
	printf_s("Starting DS3 ...\n");

	return true;
}


VOID CCore::Run() {

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