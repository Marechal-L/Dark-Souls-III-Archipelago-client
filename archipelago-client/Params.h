// Much of this comes from the TGA table
// https://github.com/The-Grand-Archives/Dark-Souls-III-CT-TGA/blob/main/resources/include/tga

#pragma once
#include "fd4_singleton.h"

typedef struct _DLWString {
	union {
		wchar_t in_place[8];
		wchar_t* ptr;
	} str;
	uint64_t length;
	uint64_t capacity;
} DLWString;

inline wchar_t* dlw_c_str(DLWString* s) {
	return (s->capacity > 7) ? s->str.ptr : s->str.in_place;
}

typedef struct _ParamRowInfo {
	uint64_t row_id; // ID of param row
	uint64_t param_offset; // Offset of pointer to param data relative to parent table
	uint64_t param_end_offset; // Seems to point to end of ParamTable struct
} ParamRowInfo;

typedef struct _ParamTable {
	uint8_t pad00[0x00A];
	uint16_t num_rows; // Number of rows in param table

	uint8_t pad01[0x004];
	uint64_t param_type_offset; // Offset of param type string from the beginning of this struct

	uint8_t pad02[0x028];
	ParamRowInfo rows[0]; // Array of row information structs
} ParamTable;

typedef struct _ParamHeader {
	uint8_t pad00[0x60];
	uint64_t param_table_size_bytes;
	ParamTable* param_table;
} ParamHeader;

typedef struct _ParamResCap {
	void** vftable_ptr;

	uint8_t pad00[0x08];
	DLWString param_name;

	uint8_t pad01[0x38];
	ParamHeader* param_header;
} ParamResCap;

typedef char s8;
typedef unsigned char u8;
typedef short s16;
typedef unsigned short u16;
typedef int s32;
typedef unsigned int u32;
typedef long long s64;
typedef unsigned long long u64;
typedef char fixstr;
typedef short fixstrW;
typedef float f32;
typedef double f64;
typedef unsigned char dummy8;

typedef struct _EquipParamGoodsRow {
	s32 refId_default;
	s32 sfxVariationId;
	f32 weight;
	s32 basicPrice;
	s32 sellValue;
	s32 behaviorId;
	s32 replaceItemId;
	s32 sortId;
	s32 appearanceReplaceItemId;
	s32 yesNoDialogMessageId;
	s32 magicId;
	s16 iconId;
	s16 modelId;
	s16 shopLv;
	s16 compTrophySedId;
	s16 trophySeqId;
	s16 maxNum;
	u8 consumeHeroPoint;
	u8 overDexterity;
	u8 goodsType;
	u8 refCategory;
	u8 spEffectCategory;
	u8 pad3;
	s8 goodsUseAnim;
	u8 opmeMenuType;
	u8 useLimitCategory;
	u8 replaceCategory;
	u8 vowType0 : 1;
	u8 vowType1 : 1;
	u8 vowType2 : 1;
	u8 vowType3 : 1;
	u8 vowType4 : 1;
	u8 vowType5 : 1;
	u8 vowType6 : 1;
	u8 vowType7 : 1;
	u8 vowType8 : 1;
	u8 vowType9 : 1;
	u8 vowType10 : 1;
	u8 vowType11 : 1;
	u8 vowType12 : 1;
	u8 vowType13 : 1;
	u8 vowType14 : 1;
	u8 vowType15 : 1;
	u8 enable_live : 1;
	u8 enable_gray : 1;
	u8 enable_white : 1;
	u8 enable_black : 1;
	u8 enable_multi : 1;
	u8 disable_offline : 1;
	u8 isEquip : 1;
	u8 isConsume : 1;
	u8 isAutoEquip : 1;
	u8 isEstablishment : 1;
	u8 isOnlyOne : 1;
	u8 isDiscard : 1;
	u8 isDeposit : 1;
	u8 isDisableHand : 1;
	u8 isRemoveItem_forGameClear : 1;
	u8 isSuppleItem : 1;
	u8 isFullSuppleItem : 1;
	u8 isEnhance : 1;
	u8 isFixItem : 1;
	u8 disableMutliDropShare : 1;
	u8 disableUseAtColiseum : 1;
	u8 disableUseAtOutofColiseum : 1;
	u8 isEnableFastUseItem : 1;
	u8 isApplySpecialEffect : 1;
	u8 syncNumVaryId : 1;
	u8 isAshenEstusFlask : 1;
	u8 unknown1 : 1;
	u8 unknown2 : 1;
	u8 unknown3 : 1;
	u8 unknown4 : 1;
	u8 unknown5 : 1;
	u8 unknown6 : 1;
	s32 refId_1;
	s32 refVirtualWepId;
	s32 vagrantItemLotId;
	s32 vagrantBonusEneDropItemLotId;
	s32 vagrantItemEneDropItemLotId;
	s32 castSfxId;
	s32 fireSfxId;
	s32 effectSfxId;
	u8 enable_ActiveEmber : 1;
	u8 isBonfireWarpItem : 1;
	u8 enable_Ladder : 1;
	u8 isUseMultiPlayPreparation : 1;
	u8 canMultiUse : 1;
	u8 isShieldEnchant : 1;
	u8 isWarpProhibited : 1;
	u8 isUseMultiPenaltyOnly : 1;
	u8 suppleType;
	u8 autoReplenishType;
	u8 isDrop;
	s16 maxRepNum;
	u8 invadeType;
	dummy8 pad1[1];
	s32 shopId;
	s16 consumeMP;
	s16 useLimitCategory2;
	dummy8 pad2[8];
} EquipParamGoodsRow;

struct CSRegulationManager : public FD4Singleton<CSRegulationManager, "CSRegulationManager"> {
	void** vftable_ptr;
	uint8_t pad00[0x08];
	ParamResCap** param_list_begin;
	ParamResCap** param_list_end;
};

// Returns the address of the parameter data in the table with the given ID, or NULL if there is no
// such row.
inline LPVOID* FindRow(ParamTable* table, uint64_t rowId) {
	uint16_t rows = table->num_rows;
	for (uint16_t i = 0; i < rows; i++) {
		ParamRowInfo* row = &table->rows[i];
		if (row->row_id == rowId) {
			return (LPVOID*)((char*)table + row->param_offset);
		}
	}
	return NULL;
}

// Returns the param data for the goods item with the given ID (which should he the base ID without
// the category flag). Prints an error and returns NULL if this can't be loaded.
inline EquipParamGoodsRow* GetGoodsParam(DWORD id) {
	CSRegulationManager* manager = CSRegulationManager::instance();
	ParamTable* equipParamGoods = manager->param_list_begin[24]->param_header->param_table;
	EquipParamGoodsRow* row = (EquipParamGoodsRow*)FindRow(equipParamGoods, id);
	if (id == NULL) {
		printf("ERROR: Failed to locate item ID %d, randomizer may not be set up correctly\n", id);
	}
	else {
		return row;
	}
}
