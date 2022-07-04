#include "GameHook.h"

VOID fNoWeaponRequirements(DWORD* pRequirementPtr) {
	*pRequirementPtr = 0;
	return;
};