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
	Core->Logger(std::string("Archipelago client v") + VERSION);
	Core->Logger("A new version may or may not be available, please check this link for updates : https://github.com/Marechal-L/Dark-Souls-III-Archipelago-client/releases", false);
	Core->Logger("Type '/connect {SERVER_IP}:{SERVER_PORT} {SLOT_NAME} [password:{PASSWORD}]' to connect to the room", false);
	Core->Logger("Type '/help for more information", false);
	Core->Logger("-----------------------------------------------------", false);

	if (!GameHook->preInitialize()) {
		Core->Panic("Check if the game version is 1.15 and not 1.15.1, you must use the provided DarkSoulsIII.exe", "Cannot hook the game", FE_InitFailed, 1);
		return false;
	}

	if (!GameHook->initialize()) {
		Core->Panic("Failed to initialise GameHook", "...\\Randomiser\\Core\\Core.cpp", FE_InitFailed, 1);
		return false;
	}

	if (CheckOldApFile()) {
		Core->Logger("The AP.json file is not supported in this version, make sure to finish your previous seed on version 1.2 or use this version on the new Archipelago server");
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

			//Apply player settings
			BOOL initResult = GameHook->applySettings();
			if (!initResult) {
				Core->Panic("Failed to apply settings", "...\\Randomiser\\Core\\Core.cpp", FE_ApplySettings, 1);
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
		Core->Logger("Removing " + pLastReceivedIndex + std::string(" items according to the last_received_index"), true, false);
		for (int i = 0; i < pLastReceivedIndex; i++) {
			ItemRandomiser->receivedItemsQueue.pop_back();
		}
	}
}


VOID CCore::Panic(const char* pMessage, const char* pSort, DWORD dError, DWORD dIsFatalError) {

	char pOutput[MAX_PATH];
	char pTitle[MAX_PATH];

	sprintf_s(pOutput, "\n%s (%i)\n", pMessage, dError);

	Core->Logger(pOutput);
	
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
			printf("/debug on|off : Prints additional debug info \n");
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

		if (line.find("/debug ") == 0) {
			std::string param = line.substr(7);
			BOOL res = (param.find("on") == 0);
			if (res) {
				Core->Logger("Debug logs activated", true, true);
				Core->debugLogs = true;
			}
			else {
				Core->Logger("Debug logs deactivated", true, true);
				Core->debugLogs = false;
			}

			
		} 
		else if (line.find("/connect ") == 0) {
			std::string param = line.substr(9);
			int spaceIndex = param.find(" ");
			if (spaceIndex == std::string::npos) {
				Core->Logger("Missing parameter : Make sure to type '/connect {SERVER_IP}:{SERVER_PORT} {SLOT_NAME} [password:{PASSWORD}]'");
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
					Core->Panic("Failed to initialise Archipelago", "", AP_InitFailed, 1);
					int3
				}
			}
		}
		else if (line.find("!") == 0) {
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
			Logger("No save found, starting a new game", true, false);
			return;
		}
	}

	//Read the game file
	Logger("Reading " + outputFolder + "/" + filename, true, false);
	json k;

	try {
		gameFile >> k;
		k.at("last_received_index").get_to(pLastReceivedIndex);
		std::map<DWORD, int>::iterator it;
		for (it = ItemRandomiser->progressiveLocations.begin(); it != ItemRandomiser->progressiveLocations.end(); it++) {
			char buf[10];
			sprintf(buf, "0x%x", it->first);
			if(k.at("progressive_locations").contains(buf)) {
				k.at("progressive_locations").at(buf).get_to(ItemRandomiser->progressiveLocations[it->first]);
			}
		}
	} catch (const std::exception&) {
		gameFile.close();
		Core->Panic(("Failed reading " + outputFolder + "/" + filename).c_str(), "", AP_InitFailed, 1);
		int3
	}

	gameFile.close();
};

VOID CCore::SaveConfigFiles() {

	if (!saveConfigFiles)
		return;

	saveConfigFiles = false;
	
	std::string outputFolder = "archipelago";
	std::string filename = Core->pSeed + "_" + Core->pSlotName + ".json";

	Logger("Writing to " + outputFolder + "/" + filename, true, false);

	json j;
	j["last_received_index"] = pLastReceivedIndex;
	
	std::map<DWORD, int>::iterator it;
	for (it = ItemRandomiser->progressiveLocations.begin(); it != ItemRandomiser->progressiveLocations.end(); it++) {
		char buf[20];
		sprintf(buf, "0x%x", it->first);
		j["progressive_locations"][buf] = it->second;
	}

	try {
		if (CreateDirectory(outputFolder.c_str(), NULL) || ERROR_ALREADY_EXISTS == GetLastError()) {
			std::ofstream outfile(outputFolder + "\\" + filename);
			outfile << std::setw(4) << j << std::endl;
			outfile.close();
		}
		else {
			std::ofstream outfile(filename);
			outfile << std::setw(4) << j << std::endl;
			outfile.close();
		}
	}
	catch (const std::exception&) {
		Logger("Failed writing " + outputFolder + "/" + filename, true, true);
	}
}


inline std::string getCurrentDateTime(std::string s) {
	time_t now = time(0);
	struct tm  tstruct;
	char buf[80];
	tstruct = *localtime(&now);
	if (s == "now")
		strftime(buf, sizeof(buf), "%Y-%m-%d %X", &tstruct);
	else if (s == "date")
		strftime(buf, sizeof(buf), "%Y-%m-%d", &tstruct);
	return std::string(buf);
};

VOID CCore::Logger(std::string logMessage, BOOL inFile, BOOL inConsole) {

	if(inConsole)
		std::cout << logMessage << std::endl;

	if (inFile) {
		try {
			std::string outputFolder = "archipelago";
			std::string filename = "log_" + getCurrentDateTime("date") + ".txt";
			std::ofstream logFile(outputFolder + "\\" + filename, std::ios_base::out | std::ios_base::app);

			std::string now = getCurrentDateTime("now");
			logFile << now << '\t' << logMessage << '\n';
			logFile.close();
		} catch (const std::exception&) {
			//Logging is optional and should not crash the mod
		}
	}
}


