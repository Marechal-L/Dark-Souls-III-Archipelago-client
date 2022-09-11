#include "Core.h"
#include "GameHook.h"

CCore* Core;
CGameHook* GameHook;
CItemRandomiser* ItemRandomiser;
CAutoEquip* AutoEquip;
SCore* CoreStruct;
CArchipelago* ArchipelagoInterface;

using nlohmann::json;

VOID CCore::Start() {

	Core = new CCore();
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
		Sleep(2000);
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
	SetConsoleTitleA("Dark Souls III - Archipelago Console");
	freopen_s(&fp, "CONOUT$", "w", stdout);
	freopen_s(&fp, "CONIN$", "r", stdin);
	printf_s("Archipelago client v%s \n", VERSION);
	printf_s("A new version may or may not be available, please check this link for updates : %s \n\n\n", "https://github.com/Marechal-L/Dark-Souls-III-Archipelago-client/releases");
	
	ReadConfigFiles();


	if (!GameHook->preInitialize()) {
		Core->Panic("Check if the game version is 1.15 and not 1.15.1, you must use the provided DarkSoulsIII.exe", "Cannot hook the game", FE_InitFailed, 1);
	}
	

	//Start command prompt
	CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)Core->InputCommand, NULL, NULL, NULL);

	return true;
}

bool isInit = false;

VOID CCore::Run() {

	ArchipelagoInterface->update();

	GameHook->updateRuntimeValues();
	if(GameHook->healthPointRead != 0 && GameHook->playTimeRead !=0) {

		if (!isInit) {
			//Inject custom shell codes
			BOOL initResult = GameHook->initialize();
			if (!initResult) {
				Core->Panic("Failed to initialise GameHook", "...\\Randomiser\\Core\\Core.cpp", FE_InitFailed, 1);
				int3
			}
			isInit = true;
		}


		GameHook->manageDeathLink();

		if (!ItemRandomiser->receivedItemsQueue.empty()) {
			GameHook->giveItems();
			pLastReceivedIndex++;
			saveConfigFiles = true;
		}

		if (GameHook->isSoulOfCinderDefeated() && sendGoalStatus) {
			sendGoalStatus = false;
			ArchipelagoInterface->gameFinished();
		}
	}

	SaveConfigFiles();

	return;
};



VOID CCore::Panic(const char* pMessage, const char* pSort, DWORD dError, DWORD dIsFatalError) {

	char pOutput[MAX_PATH];
	char pTitle[MAX_PATH];

	sprintf_s(pOutput, "\n%s -> %s (%i)\n", pSort, pMessage, dError);

	printf("%s", pOutput);
	
	if (dIsFatalError) {
		sprintf_s(pTitle, "[Archipelago client - Fatal Error]");
	}
	else {
		sprintf_s(pTitle, "[Archipelago client - Error]");
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
			printf("!help : Prints the help message related to Archipelago.\n");
			printf("/connect {SERVER_IP}:{SERVER_PORT} : Connect to the specified server.\n");
			printf("/connect : Connect to the localhost:38281 server.\n");
		}

#ifdef DEBUG
		if (line.find("/itemGib ") == 0) {
			std::string param = line.substr(9);
			std::cout << "/itemGib executed with " << param << "\n";
			ItemRandomiser->receivedItemsQueue.push_front(std::stoi(param));
		}

		if (line.find("/save") == 0) {
			std::cout << "/save\n";
			Core->saveConfigFiles = true;
		}
			
#endif

		if (line.find("/connect ") == 0) {
			std::string param = line.substr(9);
			int spaceIndex = param.find(" ");
			if (spaceIndex == std::string::npos) {
				if (!ArchipelagoInterface->Initialise(param)) {
					Core->Panic("Failed to initialise Archipelago", "...\\Randomiser\\Core\\Core.cpp", AP_InitFailed, 1);
					int3
				}
			} else {
				std::string address = param.substr(0, spaceIndex);
				std::string slotName = param.substr(spaceIndex + 1);
				std::cout << address << " - " << slotName << "\n";
				Core->pSlotName = slotName;
				if (!ArchipelagoInterface->Initialise(address)) {
					Core->Panic("Failed to initialise Archipelago", "...\\Randomiser\\Core\\Core.cpp", AP_InitFailed, 1);
					int3
				}
			}
		}

		if (line == "/connect") {
			if (!ArchipelagoInterface->Initialise("localhost:38281")) {
				Core->Panic("Failed to initialise Archipelago", "...\\Randomiser\\Core\\Core.cpp", AP_InitFailed, 1);
				int3
			}
		}

		if (line.find("!") == 0) {
			ArchipelagoInterface->say(line);
		}
	}
};

VOID CCore::ReadConfigFiles() {
	
	printf("Reading AP.json ... \n");

	// read the archipelago json file
	std::ifstream i("AP.json");
	if( i.fail() ) {
		Core->Panic("Double check the filename and the file extension", "Can not open the `AP.json` file", AP_MissingFile, 1);
	}

	json j;
	i >> j;

	//Mandatory values
	if (!j.contains("locationsId") || !j.contains("locationsAddress") || !j.contains("locationsTarget") || !j.contains("itemsId") 
		|| !j.contains("itemsAddress") || !j.contains("base_id") || !j.contains("seed") || !j.contains("slot")) {
		Core->Panic("Please check the following values : [locationsId], [locationsAddress], [locationsTarget], [itemsId], [itemsAddress], [base_id], [seed] and [slot]", "One of the mandatory values is missing in the `AP.json` file", AP_MissingValue, 1);
	}

	j.at("locationsId").get_to(ItemRandomiser->pLocationsId);
	j.at("locationsAddress").get_to(ItemRandomiser->pLocationsAddress);
	j.at("locationsTarget").get_to(ItemRandomiser->pLocationsTarget);
	j.at("itemsId").get_to(ItemRandomiser->pItemsId);
	j.at("itemsAddress").get_to(ItemRandomiser->pItemsAddress);
	j.at("base_id").get_to(ItemRandomiser->pBaseId);
	j.at("seed").get_to(Core->pSeed);
	j.at("slot").get_to(Core->pSlotName);

	if (j.contains("options")) {
		(j.at("options").contains("auto_equip")) ? (j.at("options").at("auto_equip").get_to(GameHook->dIsAutoEquip)) : GameHook->dIsAutoEquip = false;
		(j.at("options").contains("lock_equip")) ? (j.at("options").at("lock_equip").get_to(GameHook->dLockEquipSlots)) : GameHook->dLockEquipSlots = false;
		(j.at("options").contains("no_weapon_requirements")) ? (j.at("options").at("no_weapon_requirements").get_to(GameHook->dIsNoWeaponRequirements)) : GameHook->dIsNoWeaponRequirements = false;
		(j.at("options").contains("death_link")) ? (j.at("options").at("death_link").get_to(GameHook->dIsDeathLink)) : GameHook->dIsDeathLink = false;
		(j.at("options").contains("no_spell_requirements")) ? (j.at("options").at("no_spell_requirements").get_to(GameHook->dIsNoSpellsRequirements)) : GameHook->dIsNoSpellsRequirements = false;
		(j.at("options").contains("no_equip_load")) ? (j.at("options").at("no_equip_load").get_to(GameHook->dIsNoEquipLoadRequirements)) : GameHook->dIsNoEquipLoadRequirements = false;
	}

	std::string outputFolder = "archipelago";
	std::string filename = Core->pSeed + "_" + Core->pSlotName + ".json";

	//Check in archipelago folder
	std::ifstream gameFile(outputFolder+"\\"+filename);
	if (!gameFile.good()) {
		//Check outside the folder
		std::ifstream gameFile(filename);
		if (!gameFile.good()) {
			
			//Missing session file, that's probably a new game
			return;
		}
	}

	//Read the game file
	json k;
	gameFile >> k;
	k.at("received_locations").get_to(pReceivedItems);
	k.at("last_received_index").get_to(pLastReceivedIndex);
	gameFile.close();
};

VOID CCore::SaveConfigFiles() {

	if (!saveConfigFiles)
		return;

	saveConfigFiles = false;

	
	std::string outputFolder = "archipelago";
	std::string filename = Core->pSeed + "_" + Core->pSlotName + ".json";

	json j;
	j["received_locations"] = pReceivedItems;
	j["last_received_index"] = pLastReceivedIndex;

	if (CreateDirectory(outputFolder.c_str(), NULL) || ERROR_ALREADY_EXISTS == GetLastError()) {
		std::ofstream outfile(outputFolder + "\\" +filename);
		outfile << std::setw(4) << j << std::endl;
	} else {
		std::ofstream outfile(filename);
		outfile << std::setw(4) << j << std::endl;
	}
}


