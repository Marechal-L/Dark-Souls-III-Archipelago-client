#include "Core.h"
#include "GameHook.h"

CCore* Core;
CGameHook* GameHook;
CItemRandomiser* ItemRandomiser;
CAutoEquip* AutoEquip;
SCore* CoreStruct;

using nlohmann::json;

VOID CCore::Start() {

	Core = new CCore();
	CoreStruct = new SCore();
	GameHook = new CGameHook();
	ItemRandomiser = new CItemRandomiser();
	AutoEquip = new CAutoEquip();
	AutoEquip->EquipItem = (fEquipItem*)0x140AFBBB0;

	if (!Core->Initialise()) {
		Core->Panic("Failed to initialise", "...\\Randomiser\\Core\\Core.cpp", FE_InitFailed, 1);
		int3
	};

	while (true) {
		Core->Run();
		Sleep(1000);
	};

	delete CoreStruct;
	delete Core;
	delete GameHook;
	delete ItemRandomiser;
	delete AutoEquip;

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

	ReadConfigFiles();

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

	
	if (dIsFatalError) {
		sprintf_s(pTitle, "[Item Randomiser - Fatal Error]");
	}
	else {
		sprintf_s(pTitle, "[Item Randomiser - Error]");
	};

	MessageBoxA(NULL, pOutput, pTitle, MB_ICONERROR);
	

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

VOID CCore::ReadConfigFiles() {
	
	printf("Reading AP.json\n");

	// read the archipelago json file
	std::ifstream i("AP.json");
	json j;
	i >> j;

	j.at("locationsId").get_to(ItemRandomiser->pLocationsId);
	j.at("locationsAddress").get_to(ItemRandomiser->pLocationsAddress);
	j.at("locationsTarget").get_to(ItemRandomiser->pLocationsTarget);
	j.at("itemsId").get_to(ItemRandomiser->pItemsId);
	j.at("itemsAddress").get_to(ItemRandomiser->pItemsAddress);
	j.at("base_id").get_to(ItemRandomiser->pBaseId);

	j.at("seed").get_to(Core->pSeed);
	j.at("slot").get_to(Core->pSlotName);

	j.at("options").at("auto_equip").get_to(CoreStruct->dIsAutoEquip);
	j.at("options").at("lock_equip").get_to(CoreStruct->dLockEquipSlots);
	j.at("options").at("no_weapon_requirements").get_to(CoreStruct->dIsNoWeaponRequirements);


	printf("Number of locations : %d\n", ItemRandomiser->pLocationsId.size());
	printf("auto-equip enabled : %d\n", CoreStruct->dIsAutoEquip);
	printf("lock-equip-slot enabled : %d\n", CoreStruct->dLockEquipSlots);
	printf("no-weapon-requirements enabled : %d\n", CoreStruct->dIsNoWeaponRequirements);


};


