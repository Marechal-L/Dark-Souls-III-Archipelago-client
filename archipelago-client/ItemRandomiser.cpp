#include "Core.h"
#include "GameHook.h"
#include <functional>

extern CCore* Core;
extern CItemRandomiser* ItemRandomiser;

VOID fItemRandomiser(UINT_PTR qWorldChrMan, UINT_PTR pItemBuffer, UINT_PTR pItemData, DWORD64 qReturnAddress) {

	if (*(int*)(pItemData) >= 0) ItemRandomiser->RandomiseItem(qWorldChrMan, pItemBuffer, pItemData, qReturnAddress);

	return;
}


VOID CItemRandomiser::RandomiseItem(UINT_PTR qWorldChrMan, UINT_PTR pItemBuffer, UINT_PTR pItemData, DWORD64 qReturnAddress) {

	DWORD dItemAmount = 0;
	DWORD dItemID = 0;
	DWORD dItemQuantity = 0;
	DWORD dItemDurability = 0;

	dItemAmount = *(int*)pItemBuffer;
	pItemBuffer += 4;

	if (dItemAmount > 6) {
		Core->Panic("Too many items!", "...\\Source\\ItemRandomiser\\ItemRandomiser.cpp", FE_AmountTooHigh, 1);
		int3
	};

	while (dItemAmount) {

		dItemID = *(int*)(pItemBuffer);
		dItemQuantity = *(int*)(pItemBuffer + 0x04);
		dItemDurability = *(int*)(pItemBuffer + 0x08);

		dItemID = 0x40000172;

		*(int*)(pItemBuffer) = dItemID;
		*(int*)(pItemBuffer + 0x04) = dItemQuantity;
		*(int*)(pItemBuffer + 0x08) = -1;

		dItemAmount--;
		pItemBuffer += 0x0C;
	};

	return;

}