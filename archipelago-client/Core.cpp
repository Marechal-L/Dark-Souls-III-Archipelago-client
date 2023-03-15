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
		Sleep(RUN_SLEEP);
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
	printf_s("Type '/connect {SERVER_IP}:{SERVER_PORT} {SLOT_NAME} [password:{PASSWORD}]' to connect to the room\n\n");

	if (!GameHook->preInitialize()) {
		Core->Panic("Check if the game version is 1.15 and not 1.15.1, you must use the provided DarkSoulsIII.exe", "Cannot hook the game", FE_InitFailed, 1);
		return false;
	}

	if (CheckOldApFile()) {
		printf_s("The AP.json file is not supported in this version, make sure to finish your previous seed on version 1.2 or use this version on the new Archipelago server\n\n");
	}

	//Start command prompt
	CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)Core->InputCommand, NULL, NULL, NULL);

	return true;
}

BOOL CCore::CheckOldApFile() {

	// read the archipelago json file
	std::ifstream i("AP.json");
	return !i.fail();
}

bool isInit = false;
int initProtectionDelay = 3;

VOID CCore::Run() {

	ArchipelagoInterface->update();
	GameHook->updateRuntimeValues();

	if(GameHook->healthPointRead != 0 && GameHook->playTimeRead !=0) {

		if (!isInit && ArchipelagoInterface->isConnected() && initProtectionDelay <= 0) {
			ReadConfigFiles();
			CleanReceivedItemsList();

			//Inject custom shell codes
			BOOL initResult = GameHook->initialize();
			if (!initResult) {
				Core->Panic("Failed to initialise GameHook", "...\\Randomiser\\Core\\Core.cpp", FE_InitFailed, 1);
				int3
			}
			printf("Mod initialized successfully\n");
			isInit = true;
		}

		if (isInit) {
			GameHook->manageDeathLink();

			if (!ItemRandomiser->receivedItemsQueue.empty()) {
				GameHook->giveItems();
				pLastReceivedIndex++;
			}

			if (GameHook->isSoulOfCinderDefeated() && sendGoalStatus) {
				sendGoalStatus = false;
				ArchipelagoInterface->gameFinished();
			}
		} else {
			int secondsRemaining = (RUN_SLEEP / 1000) * initProtectionDelay;
			printf("The mod will be initialized in %d seconds\n", secondsRemaining);
			initProtectionDelay--;
		}
	}

	SaveConfigFiles();

	return;
};

/*
* Permits to remove all received item indexes lower than pLastReceivedIndex from the list.
* It has be to performed after the first connection because we now read the pLastReceivedIndex from the slot_data.
*/
VOID CCore::CleanReceivedItemsList() {
	if (!ItemRandomiser->receivedItemsQueue.empty()) {

		for (int i = 0; i < pLastReceivedIndex; i++) {
			ItemRandomiser->receivedItemsQueue.pop_back();
		}
	}
}



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
			printf("/connect {SERVER_IP}:{SERVER_PORT} {SLOT_NAME} [password:{PASSWORD}] : Connect to the specified server.\n");
		}

#ifdef DEBUG
		if (line.find("/itemGib ") == 0) {
			std::string param = line.substr(9);
			std::cout << "/itemGib executed with " << param << "\n";
			GameHook->itemGib(std::stoi(param));
		}

		if (line.find("/give ") == 0) {
			std::string param = line.substr(6);
			std::cout << "/give executed with " << param << "\n";
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
				printf("Missing parameter : Make sure to type '/connect {SERVER_IP}:{SERVER_PORT} {SLOT_NAME} [password:{PASSWORD}]' \n");
			} else {
				int passwordIndex = param.find("password:");
				std::string address = param.substr(0, spaceIndex);
				std::string slotName = param.substr(spaceIndex + 1, passwordIndex - spaceIndex - 2);
				std::string password = "";
				std::cout << address << " - " << slotName << "\n";
				Core->pSlotName = slotName;
				if (passwordIndex != std::string::npos)
				{
					password = param.substr(passwordIndex + 9);
				}
				Core->pPassword = password;
				if (!ArchipelagoInterface->Initialise(address)) {
					Core->Panic("Failed to initialise Archipelago", "...\\Randomiser\\Core\\Core.cpp", AP_InitFailed, 1);
					int3
				}
			}
		}

		if (line.find("!") == 0) {
			ArchipelagoInterface->say(line);
		}
	}
};

VOID CCore::ReadConfigFiles() {

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

#if DEBUG
	printf("Reading %s ... \n", filename.c_str());
#endif

	//Read the game file
	json k;
	gameFile >> k;
	k.at("last_received_index").get_to(pLastReceivedIndex);
	std::map<DWORD, int>::iterator it;
	for (it = ItemRandomiser->progressiveLocations.begin(); it != ItemRandomiser->progressiveLocations.end(); it++) {
		char buf[10];
		sprintf(buf, "0x%x", it->first);
		k.at("progressive_locations").at(buf).get_to(ItemRandomiser->progressiveLocations[it->first]);
	}

	gameFile.close();
};

VOID CCore::SaveConfigFiles() {

	if (!saveConfigFiles)
		return;

	saveConfigFiles = false;

	
	std::string outputFolder = "archipelago";
	std::string filename = Core->pSeed + "_" + Core->pSlotName + ".json";

	
#if DEBUG
	printf("Writing %s ... \n", filename.c_str());
#endif

	json j;
	j["last_received_index"] = pLastReceivedIndex;
	
	std::map<DWORD, int>::iterator it;
	for (it = ItemRandomiser->progressiveLocations.begin(); it != ItemRandomiser->progressiveLocations.end(); it++) {
		char buf[20];
		sprintf(buf, "0x%x", it->first);
		j["progressive_locations"][buf] = it->second;
	}


	if (CreateDirectory(outputFolder.c_str(), NULL) || ERROR_ALREADY_EXISTS == GetLastError()) {
		std::ofstream outfile(outputFolder + "\\" + filename);
		outfile << std::setw(4) << j << std::endl;
	}
	else {
		std::ofstream outfile(filename);
		outfile << std::setw(4) << j << std::endl;
	}
}


