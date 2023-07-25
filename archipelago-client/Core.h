#pragma once
#define ASIO_STANDALONE
#define _WEBSOCKETPP_CPP11_INTERNAL_
#define _CRT_SECURE_NO_WARNINGS
//#define WSWRAP_NO_SSL

#include "subprojects/apclientpp/apclient.hpp"
#include <windows.h>
#include <Windows.h>
#include <iostream>
#include <vector>
#include <map>
#include <deque>
#include <string>
#include <fstream>
#include <bitset>
#include <tlhelp32.h>
#include <stdio.h>
#include <functional>
#include "./subprojects/json/include/nlohmann/json.hpp"
#include "ArchipelagoInterface.h"

#define int3 __debugbreak();

#define FE_InitFailed 0
#define FE_AmountTooHigh 1
#define FE_NullPtr 2
#define FE_NullArray 3
#define FE_BadFunc 4
#define FE_MemError 5
#define HE_InvalidItemType 6
#define HE_InvalidInventoryEquipID 7
#define HE_Undefined 8
#define HE_NoPlayerChar 9
#define AP_InitFailed 10
#define AP_MissingFile 11
#define AP_MissingValue 12
#define FE_MissingDLC 13
#define FE_ApplySettings 14

#define VERSION "2.2.0"


struct SCore;

class CCore {
public:
	static VOID Start();
	static VOID InputCommand();
	virtual VOID Run();
	virtual BOOL Initialise();
	virtual VOID Panic(const char* pMessage, const char* pSort, DWORD dError, DWORD dIsFatalError);
	virtual VOID ReadConfigFiles();
	virtual VOID SaveConfigFiles();
	virtual VOID CleanReceivedItemsList();
	virtual BOOL CheckOldApFile();
	virtual VOID Logger(std::string logMessage, BOOL inFile = true, BOOL inConsole = true);

	std::string pSlotName;
	std::string pPassword;
	std::string pSeed;
	BOOL saveConfigFiles = false;
	BOOL sendGoalStatus = true;
	std::list<std::string> pReceivedItems = { };
	int pLastReceivedIndex = 0;
	BOOL debugLogs = false;

	static const int RUN_SLEEP = 2000;
};