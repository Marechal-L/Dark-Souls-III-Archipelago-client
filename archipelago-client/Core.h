#pragma once

#include <windows.h>
#include <Windows.h>
#include <iostream>
#include <vector>
#include <deque>
#include <string>
#include <fstream>
#include <bitset>
#include <tlhelp32.h>
#include <stdio.h>
#include <functional>

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

struct SCore;

class CCore {
public:
	static VOID Start();
	virtual VOID Run();
	virtual BOOL Initialise();
	//virtual VOID InputCommand();
	virtual VOID Panic(const char* pMessage, const char* pSort, DWORD dError, DWORD dIsFatalError);
};

struct SCore {
	DWORD dIsDebug;
	DWORD dIsAutoSave;
	DWORD dRandomsieHealItems;
	DWORD dRandomiseKeyItems;
	DWORD dIsAutoEquip;
	DWORD dLockEquipSlots;
	DWORD dIsNoWeaponRequirements;
	DWORD dIsMessageActive;
	DWORD dIsListChanged;
	UINT_PTR qLocalPlayer = 0x144740178;
	UINT_PTR qWorldChrMan = 0x144768E78;
	UINT_PTR qSprjLuaEvent = 0x14473A9C8;
	HANDLE hHeap;
	DWORD* pOffsetArray;
	DWORD* pItemArray;
};
