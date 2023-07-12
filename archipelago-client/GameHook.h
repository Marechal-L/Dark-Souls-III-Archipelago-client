#pragma once
#include "Core.h"
#include "./subprojects/minhook/include/MinHook.h"

#define ItemType_Weapon 0
#define ItemType_Protector 1
#define ItemType_Accessory 2
#define ItemType_Goods 4

struct SEquipBuffer;

typedef VOID fEquipItem(DWORD dSlot, SEquipBuffer* E);

class CGameHook {
public:
	virtual BOOL preInitialize();
	virtual BOOL initialize();
	virtual BOOL applySettings();
	virtual BOOL updateRuntimeValues();
	virtual VOID giveItems();
	virtual VOID itemGib(DWORD itemId);
	virtual BOOL isSoulOfCinderDefeated();
	virtual VOID manageDeathLink();
	virtual BYTE* findPattern(BYTE* pBaseAddress, BYTE* pbMask, const char* pszMask, size_t nLength);
	int healthPoint = -1, lastHealthPoint = -1, playTime = -1;
	char soulOfCinderDefeated;
	SIZE_T healthPointRead, playTimeRead, soulOfCinderDefeatedFlagRead;

	DWORD dIsAutoEquip;
	DWORD dLockEquipSlots;
	DWORD dIsNoWeaponRequirements;
	DWORD dIsNoSpellsRequirements;
	DWORD dIsNoEquipLoadRequirements;
	DWORD dIsDeathLink;
	DWORD dEnableDLC;
	UINT_PTR qLocalPlayer = 0x144740178;
	UINT_PTR qWorldChrMan = 0x144768E78;
	UINT_PTR qSprjLuaEvent = 0x14473A9C8;
	HANDLE hHeap;

	BOOL deathLinkData = false;

private:
	static BOOL replaceShellCodeAddress(BYTE* shellcode, int shellCodeOffset, LPVOID codeCave, int codeCaveOffset, int length);
	static LPVOID InjectShellCode(LPVOID address, BYTE* shellCode, size_t len);
	static void ConvertToLittleEndianByteArray(uintptr_t address, char* output);
	static uintptr_t FindExecutableAddress(uintptr_t ptrOffset, std::vector<unsigned int> offsets);
	static uintptr_t GetModuleBaseAddress();
	static uintptr_t FindDMAAddy(HANDLE hProc, uintptr_t ptr, std::vector<unsigned int> offsets);
	static uintptr_t FindDMAAddyStandalone(uintptr_t ptr, std::vector<unsigned int> offsets);
	static BOOL Hook(DWORD64 qAddress, DWORD64 qDetour, DWORD64* pReturn, DWORD dByteLen);
	static VOID LockEquipSlots();
	static VOID RemoveSpellsRequirements();
	static VOID RemoveEquipLoad();
	static VOID killThePlayer();
	BOOL checkIsDlcOwned();

	
	
	uintptr_t BaseB = -1;
	uintptr_t GameFlagData = -1;
	uintptr_t Param = -1;
	uintptr_t EquipLoad = -1;

	uintptr_t BaseA = -1;
	const char* baseAPattern = reinterpret_cast<const char*>("\x48\x8B\x05\x00\x00\x00\x00\x48\x85\xc0\x00\x00\x48\x8b\x40\x00\xc3");
	const char* baseAMask = "xxx????xxx??xxx?x";

	uintptr_t CSDlc = -1;
	const char* csDlcPattern = reinterpret_cast<const char*>("\x48\x8B\x0d\x00\x00\x00\x00\x48\x85\xc9\x0f\x84\x00\x00\x00\x00\x0f\xba\xe0\x00\x72\x65\x0f\xba\xe8");
	const char* csDlcMask = "xxx????xxxxx????xxx?xxxxx";



	BYTE ItemGibDataShellcode[17] =
	{
		0x01, 0x00,
		0x00, 0x00,
		0xF4,
		0x01, 0x00,
		0x40, 0xFF,
		0xFF,
		0xFF,
		0xFF, 0x00,
		0x00, 0x00,
		0x00, 0x0A
	};

	BYTE ItemGibShellcode[93] =
	{
		0x48, 0x83, 0xEC, 0x48,
		0x44, 0x8D, 0x44, 0x24, 0x20,
		0x48, 0x8D, 0x54, 0x24, 0x30,
		0xA1, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
		0x8B, 0x1C, 0x25, 0xFF, 0xFF, 0xFF, 0xFF,
		0x8B, 0x34, 0x25, 0xFF, 0xFF, 0xFF, 0xFF,
		0xC7, 0x02, 0x01, 0x00, 0x00, 0x00,							
		0x89, 0x72, 0x0C,
		0x41, 0x89, 0x58, 0x14,
		0x41, 0x89, 0x40, 0x18,
		0x48, 0xA1, 0x78, 0x8E, 0x76, 0x44, 0x01, 0x00, 0x00, 0x00,
		0x48, 0x8B, 0xA8, 0x80, 0x00, 0x00, 0x00,
		0x48, 0x8B, 0x1D, 0xB2, 0x22, 0x77, 0x04,
		0x48, 0x8B, 0xCB,
		0xE8, 0x1A, 0xBA, 0x7D, 0x00,
		0x48, 0x83, 0xC4, 0x48,
		0xC3
	};

};


class CItemRandomiser {
public:
	virtual VOID RandomiseItem(UINT_PTR qWorldChrMan, UINT_PTR pItemBuffer, UINT_PTR pItemData, DWORD64 qReturnAddress);
	
	std::vector<DWORD> pLocationsId = { };
	std::vector<DWORD> pLocationsAddress = { };
	std::vector<DWORD> pLocationsTarget = { };
	std::vector<DWORD> pItemsId = { };
	std::vector<DWORD> pItemsAddress = { };
	int pBaseId = 0;
	std::deque<DWORD> receivedItemsQueue = { };
	std::list<int64_t> checkedLocationsList = { };
	std::map<DWORD, int> progressiveLocations{
		{ 0x400003E8, -1 }, //Titanite Shard
		{ 0x400003E9, -1 }, //Large Titanite Shard
		{ 0x400003EA, -1 }, //Titanite Chunk
		{ 0x400003FC, -1 }, //Titanite Scale
		{ 0x400003EB, -1 }, //Titanite Slab
		{ 0x4000085D, -1 }, //Estus Shard
		{ 0x4000085F, -1 }, //Undead Bone Shard
		{ 0x40000124, -1 }, //Firebomb
		{ 0x40000136, -1 }, //Throwing Knife
		{ 0x40000190, -1 }, //Faded Soul
		{ 0x40000191, -1 }, //Soul of a Deserted Corpse
		{ 0x40000192, -1 }, //Large Soul of a Deserted Corpse
		{ 0x40000193, -1 }, //Soul of an Unknown Traveler
		{ 0x40000194, -1 }, //Large Soul of an Unknown Traveler
		{ 0x40000195, -1 }, //Soul of a Nameless Soldier
		{ 0x40000196, -1 }, //Large Soul of a Nameless Soldier
		{ 0x20004EF2, -1 }, //Ring of Sacrifice
		{ 0x4000015E, -1 }, //Homeward Bone
		{ 0x400001F4, -1 }, //Ember
		{ 0x40000104, -1 }, //Green Blossom
		{ 0x4000014A, -1 }, //Charcoal Pine Resin
		{ 0x4000014B, -1 }, //Gold Pine Resin
		{ 0x4000014E, -1 }, //Human Pine Resin
		{ 0x4000014F, -1 }, //Carthus Rouge
		{ 0x40000150, -1 }, //Pale Pine Resin
		{ 0x40000154, -1 }, //Charcoal Pine Bundle
		{ 0x40000157, -1 }, //Rotten Pine Resin
		{ 0x40000175, -1 }, //Pale Tongue
		{ 0x40000126, -1 }, //Alluring Skull
		{ 0x40000128, -1 }, //Undead Hunter Charm
		{ 0x40000130, -1 }, //Duel Charm
		{ 0x400001C7, -1 }, //Rusted Coin
		{ 0x400001C9, -1 }, //Rusted Gold Coin
		{ 0x40000406, -1 }, //Twinkling Titanite
		{ 0x40000197, -1 }, //Soul of a Weary Warrior
		{ 0x40000198, -1 }, //Large Soul of a Weary Warrior
		{ 0x40000199, -1 }, //Soul of a Crestfallen Knight
		{ 0x4000019A, -1 }, //Large Soul of a Crestfallen Knight
		{ 0x400000F0, -1 }, //Divine Blessing
		{ 0x400000F1, -1 }, //Hidden Blessing
		{ 0x40000106, -1 }, //Budding Green Blossom
		{ 0x4000010E, -1 }, //Bloodred Moss Clump
		{ 0x4000010F, -1 }, //Purple Moss Clump
		{ 0x40000110, -1 }, //Blooming Purple Moss Clump
		{ 0x40000112, -1 }, //Purging Stone
		{ 0x40000114, -1 }, //Rime-blue Moss Clump
		{ 0x40000118, -1 }, //Repair Powder
		{ 0x40000122, -1 }, //Kukri
		{ 0x4000012C, -1 }, //Lightning Urn
		{ 0x4000017C, -1 }, //Rubbish
		{ 0x400001CA, -1 }, //Blue Bug Pellet
		{ 0x400001CB, -1 }, //Red Bug Pellet
		{ 0x400001CC, -1 }, //Yellow Bug Pellet
		{ 0x400001CD, -1 }, //Black Bug Pellet
		{ 0x4000044C, -1 }, //Heavy Gem
		{ 0x40000456, -1 }, //Sharp Gem
		{ 0x40000460, -1 }, //Refined Gem
		{ 0x4000046A, -1 }, //Crystal Gem
		{ 0x40000474, -1 }, //Simple Gem
		{ 0x4000047E, -1 }, //Fire Gem
		{ 0x40000488, -1 }, //Chaos Gem
		{ 0x40000492, -1 }, //Lightning Gem
		{ 0x4000049C, -1 }, //Deep Gem
		{ 0x400004A6, -1 }, //Dark Gem
		{ 0x400004B0, -1 }, //Poison Gem
		{ 0x400004BA, -1 }, //Blood Gem
		{ 0x400004C4, -1 }, //Raw Gem
		{ 0x400004CE, -1 }, //Blessed Gem
		{ 0x400004D8, -1 }, //Hollow Gem
		{ 0x400004E2, -1 }, //Shriving Stone
	};
	bool enablePathOfTheDragon;

private:
	int isARandomizedLocation(DWORD dItemID);
	BOOL isReceivedFromServer(DWORD dItemID);
	BOOL isProgressiveLocation(DWORD dItemID);
};

class CAutoEquip {
public:
	virtual VOID AutoEquipItem(UINT_PTR pItemBuffer, DWORD64 qReturnAddress);
	virtual BOOL SortItem(DWORD dItemID, SEquipBuffer* E);
	virtual BOOL FindEquipType(DWORD dItem, DWORD* pArray);
	virtual DWORD GetInventorySlotID(DWORD dItemID);
	virtual VOID LockUnlockEquipSlots(int iIsUnlock);
	fEquipItem* EquipItem; //0x140AFBBB0
};

struct SEquipBuffer {
	DWORD dUn1;
	DWORD dUn2;
	DWORD dEquipSlot;
	char unkBytes[0x2C];
	DWORD dInventorySlot;
	char paddingBytes[0x60];
};

extern "C" DWORD64 qItemEquipComms;

extern "C" DWORD64 rItemRandomiser;
extern "C" VOID tItemRandomiser();
extern "C" VOID fItemRandomiser(UINT_PTR qWorldChrMan, UINT_PTR pItemBuffer, UINT_PTR pItemData, DWORD64 qReturnAddress);

extern "C" DWORD64 rAutoEquip;
extern "C" VOID tAutoEquip();
extern "C" VOID fAutoEquip(UINT_PTR pItemBuffer, DWORD64 pItemData, DWORD64 qReturnAddress);

extern "C" DWORD64 rNoWeaponRequirements;
extern "C" VOID tNoWeaponRequirements();
extern "C" VOID fNoWeaponRequirements(DWORD * pRequirementPtr);
