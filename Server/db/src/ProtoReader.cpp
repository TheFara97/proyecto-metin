#include "stdafx.h"

#include <math.h>
#include "ProtoReader.h"

#include "CsvReader.h"
#include "../../common/CommonDefines.h"
#include <sstream>

using namespace std;

#define ENABLE_NUMERIC_FIELD
#ifdef ENABLE_NUMERIC_FIELD
bool _IsNumericString(const std::string& trimInputString)
{
	if (trimInputString.empty())
		return false;
	bool isNumber = true;
	for (auto& c : trimInputString)
	{
		if (!std::isdigit(c) && c != '-' && c != '+')
		{
			isNumber = false;
			break;
		}
	}
	return isNumber;
}
#endif

inline string trim_left(const string& str)
{
    string::size_type n = str.find_first_not_of(" \t\v\n\r");
    return n == string::npos ? str : str.substr(n, str.length());
}

inline string trim_right(const string& str)
{
    string::size_type n = str.find_last_not_of(" \t\v\n\r");
    return n == string::npos ? str : str.substr(0, n + 1);
}

string trim(const string& str){return trim_left(trim_right(str));}

static string* StringSplit(string strOrigin, string strTok)
{
    size_t cutAt;
    int index = 0;
    string* strResult = new string[30];

    while ((cutAt = strOrigin.find_first_of(strTok)) != strOrigin.npos)
    {
       if (cutAt > 0)
       {
            strResult[index++] = strOrigin.substr(0, cutAt);
       }
       strOrigin = strOrigin.substr(cutAt+1);
    }

    if(strOrigin.length() > 0)
    {
        strResult[index++] = strOrigin.substr(0, cutAt);
    }

	for( int i=0;i<index;i++)
	{
		strResult[i] = trim(strResult[i]);
	}

    return strResult;
}

int get_Item_Type_Value(string inputString)
{
#ifdef ENABLE_NUMERIC_FIELD
	if (_IsNumericString(inputString))
		return std::stoi(inputString);
#endif
	string arType[] = {"ITEM_NONE", "ITEM_WEAPON",
		"ITEM_ARMOR", "ITEM_USE",
		"ITEM_AUTOUSE", "ITEM_MATERIAL",
		"ITEM_SPECIAL", "ITEM_TOOL",
		"ITEM_LOTTERY", "ITEM_ELK",

		"ITEM_METIN", "ITEM_CONTAINER",
		"ITEM_FISH", "ITEM_ROD",
		"ITEM_RESOURCE", "ITEM_CAMPFIRE",
		"ITEM_UNIQUE", "ITEM_SKILLBOOK",
		"ITEM_QUEST", "ITEM_POLYMORPH",

		"ITEM_TREASURE_BOX", "ITEM_TREASURE_KEY",
		"ITEM_SKILLFORGET", "ITEM_GIFTBOX",
		"ITEM_PICK", "ITEM_HAIR",
		"ITEM_TOTEM", "ITEM_BLEND",
		"ITEM_COSTUME", "ITEM_DS",

		"ITEM_SPECIAL_DS",	"ITEM_EXTRACT",
		"ITEM_SECONDARY_COIN",

		"ITEM_RING",
		"ITEM_BELT",
		"ITEM_PET", 
#ifdef ENABLE_NEW_PET_SYSTEM
		"ITEM_NEW_PET", // 36
		"ITEM_NEW_PET_EQ", // 37
#endif
#ifdef TITLE_SYSTEM_BYLUZER
		"ITEM_TITLE",
#endif
#ifdef ENABLE_CHEQUE_SYSTEM
		"ITEM_CHEQUE",
#endif
//#ifdef ENABLE_SYSTEM_RUNE
		"ITEM_RUNE",								//36
		"ITEM_RUNE_RED",							//37
		"ITEM_RUNE_BLUE",							//38
		"ITEM_RUNE_GREEN",							//39
		"ITEM_RUNE_YELLOW",							//40
		"ITEM_RUNE_BLACK",							//45
//#endif
#ifdef ENABLE_ARTEFAKT_SYSTEM
		"ITEM_ARTEFAKT",			//46
#endif
		"ITEM_MOUNT_EQUIPMENT",
		"ITEM_MEDIUM", // 36,37
		"ITEM_GACHA", // 38
		"ITEM_TOGGLE", // 50
		"ITEM_MOUNT",
		"ITEM_SKILL_COSTUME",
		"ITEM_GUILD_GUARD",
//#ifdef __VIP_SYSTEM__
		"ITEM_VIP",
//#endif

	};

	int retInt = -1;
	//cout << "Type : " << typeStr << " -> ";
	for (size_t j=0;j<_countof(arType);j++) {
		string tempString = arType[j];
		if	(inputString.find(tempString)!=string::npos && tempString.find(inputString)!=string::npos) {
			//cout << j << " ";
			retInt =  j;
			break;
		}
	}
	//cout << endl;

	return retInt;
}

int get_Item_SubType_Value(unsigned int type_value, string inputString)
{
#ifdef ENABLE_NUMERIC_FIELD
	if (_IsNumericString(inputString))
		return std::stoi(inputString);
#endif
	static string arSub1[] = { "WEAPON_SWORD", "WEAPON_DAGGER", "WEAPON_BOW", "WEAPON_TWO_HANDED",
				"WEAPON_BELL", "WEAPON_FAN", "WEAPON_ARROW", "WEAPON_MOUNT_SPEAR", "WEAPON_CLAW", "WEAPON_QUIVER", "WEAPON_BOUQUET"};
	static string arSub2[] = { "ARMOR_BODY", "ARMOR_HEAD", "ARMOR_SHIELD", "ARMOR_WRIST", "ARMOR_FOOTS",
				"ARMOR_NECK", "ARMOR_EAR", 
#ifdef ENABLE_GLOVE_SYSTEM
				"ARMOR_GLOVE",
#endif
#ifdef __PENDANT__
				"ARMOR_PENDANT_FIRE",
				"ARMOR_PENDANT_ICE",
				"ARMOR_PENDANT_THUNDER",
				"ARMOR_PENDANT_WIND",
				"ARMOR_PENDANT_EARTH",
				"ARMOR_PENDANT_DARK",
				"ARMOR_PENDANT_SOUL",
#endif
				"ARMOR_NUM_TYPES"};

	static string arSub3[] = { "USE_POTION", "USE_TALISMAN", "USE_TUNING", "USE_MOVE", "USE_TREASURE_BOX", "USE_MONEYBAG", "USE_BAIT",
				"USE_ABILITY_UP", "USE_AFFECT", "USE_CREATE_STONE", "USE_SPECIAL", "USE_POTION_NODELAY", "USE_CLEAR",
				"USE_INVISIBILITY", "USE_DETACHMENT", "USE_BUCKET", "USE_POTION_CONTINUE", "USE_CLEAN_SOCKET",
				"USE_CHANGE_ATTRIBUTE", "USE_ADD_ATTRIBUTE", "USE_ADD_ACCESSORY_SOCKET", "USE_PUT_INTO_ACCESSORY_SOCKET",
				"USE_ADD_ATTRIBUTE2", "USE_RECIPE", "USE_CHANGE_ATTRIBUTE2", "USE_BIND", "USE_UNBIND", "USE_TIME_CHARGE_PER", "USE_TIME_CHARGE_FIX", "USE_PUT_INTO_BELT_SOCKET", "USE_PUT_INTO_RING_SOCKET",
				"USE_CHANGE_COSTUME_ATTR", "USE_RESET_COSTUME_ATTR"
#ifdef ENABLE_SPECIAL_BONUS
				,"USE_ADD_ATTRIBUTE_SPECIAL_1",
#endif
				"USE_COLLECTION_SCROLL", "USE_ADD_IS_BONUS", "USE_ADD_NEW_BONUS", "USE_ADD_EXTRA_BONUS", "USE_POLY_SKIN", "USE_ALCHEMY_CHANGER", "USE_SASH_CLEANER", "USE_FISHING_ROD_ADD_ATTRIBUTE", "USE_COMPANION_EXP_VOUCHER"};
	static string arSub4[] = { "AUTOUSE_POTION", "AUTOUSE_ABILITY_UP", "AUTOUSE_BOMB", "AUTOUSE_GOLD", "AUTOUSE_MONEYBAG", "AUTOUSE_TREASURE_BOX"};
	static string arSub5[] = { "MATERIAL_LEATHER", "MATERIAL_BLOOD", "MATERIAL_ROOT", "MATERIAL_NEEDLE", "MATERIAL_JEWEL",
		"MATERIAL_DS_REFINE_NORMAL", "MATERIAL_DS_REFINE_BLESSED", "MATERIAL_DS_REFINE_HOLLY", "MATERIAL_DS_CHANGE_ATTR"};
	static string arSub6[] = { "SPECIAL_MAP", "SPECIAL_KEY", "SPECIAL_DOC", "SPECIAL_SPIRIT"};
	static string arSub7[] = { "TOOL_FISHING_ROD" };
	static string arSub8[] = { "LOTTERY_TICKET", "LOTTERY_INSTANT" };
	static string arSub10[] = { "METIN_NORMAL", "METIN_GOLD" };
	static string arSub12[] = { "FISH_ALIVE", "FISH_DEAD"};
	static string arSub14[] = { "RESOURCE_FISHBONE", "RESOURCE_WATERSTONEPIECE", "RESOURCE_WATERSTONE", "RESOURCE_BLOOD_PEARL",
						"RESOURCE_BLUE_PEARL", "RESOURCE_WHITE_PEARL", "RESOURCE_BUCKET", "RESOURCE_CRYSTAL", "RESOURCE_GEM",
						"RESOURCE_STONE", "RESOURCE_METIN", "RESOURCE_ORE" };
	static string arSub16[] = { "UNIQUE_NONE", "UNIQUE_BOOK", "UNIQUE_SPECIAL_RIDE", "UNIQUE_3", "UNIQUE_4", "UNIQUE_5",
					"UNIQUE_6", "UNIQUE_7", "UNIQUE_8", "UNIQUE_9", "USE_SPECIAL"};
	static string arSub28[] = { "COSTUME_BODY", "COSTUME_HAIR", "COSTUME_MOUNT", "COSTUME_ACCE", "COSTUME_STOLE", "COSTUME_WEAPON", "COSTUME_EMBLEMAT", "COSTUME_AURA", 
#ifdef ENABLE_ASLAN_BUFF_NPC_SYSTEM
	"BUFF_COSTUME_WEAPON", "BUFF_COSTUME_BODY", "BUFF_COSTUME_HAIR",
#endif
	};
	static string arSub29[] = { "DS_SLOT1", "DS_SLOT2", "DS_SLOT3", "DS_SLOT4", "DS_SLOT5", "DS_SLOT6" };
	static string arSub31[] = { "EXTRACT_DRAGON_SOUL", "EXTRACT_DRAGON_HEART" };
	static string arSub35[] = { "PET_EGG", "PET_UPBRINGING", "PET_BAG", "PET_FEEDSTUFF", "PET_SKILL",
					"PET_SKILL_DEL_BOOK", "PET_NAME_CHANGE", "PET_EXPFOOD", "PET_SKILL_ALL_DEL_BOOK", "PET_EXPFOOD_PER",
					"PET_ATTR_DETERMINE", "PET_ATTR_CHANGE", "PET_PAY", "PET_PRIMIUM_FEEDSTUFF" };
#ifdef ENABLE_NEW_PET_SYSTEM
	static string arSubPet[] = { "PET_EQ0", "PET_EQ1", "PET_EQ2", "PET_EQ3", "PET_EQ4" };
#endif
#ifdef ENABLE_ARTEFAKT_SYSTEM
	static string arSub46[] = { "ARTEFAKT_1", "ARTEFAKT_2", "ARTEFAKT_3", "ARTEFAKT_4", "ARTEFAKT_5", "ARTEFAKT_6","ARTEFAKT_7", "ARTEFAKT_8", "ARTEFAKT_9", "ARTEFAKT_10", "ARTEFAKT_11", "ARTEFAKT_12", "ARTEFAKT_13", "ARTEFAKT_14" };
	#endif
	static string arSub51[] = { "MOUNT_SHOE", "MOUNT_SADDLE", "MOUNT_ARMOR" };
	static string arSub52[] = { "SKILL_COSTUME_1", "SKILL_COSTUME_2", "SKILL_COSTUME_3", "SKILL_COSTUME_4", "SKILL_COSTUME_5", "SKILL_COSTUME_6" };
	static string* arSubType[] = {0,	//0
		arSub1,		//1
		arSub2,	//2
		arSub3,	//3
		arSub4,	//4
		arSub5,	//5
		arSub6,	//6
		arSub7,	//7
		arSub8,	//8
		0,			//9
		arSub10,	//10
		0,			//11
		arSub12,	//12
		0,			//13
		arSub14,	//14
		0,			//15
		arSub16,	//16
		0,			//17
		0,			//18
		0,			//19
		0,			//20
		0,			//21
		0,			//22
		0,			//23
		0,			//24
		0,			//25
		0,			//26
		0,			//27
		arSub28,	//28
		arSub29,		//29
		arSub29,	//30
		arSub31,	//31
		0,			//32
		0,
		0,
		arSub35,	//35
#ifdef ENABLE_NEW_PET_SYSTEM
		0,
		arSubPet,
#endif
#ifdef TITLE_SYSTEM_BYLUZER
		0,
#endif
#ifdef ENABLE_CHEQUE_SYSTEM
		0,			// 35
#endif
//#ifdef ENABLE_SYSTEM_RUNE
		0,			//35
		0,			//36
		0,			//37
		0,			//38
		0,			//39
		0,			//40
//#endif
#ifdef ENABLE_ARTEFAKT_SYSTEM
		arSub46,	//35
#endif
		0,			//36
		0,			//37
		0,			//38
		0,
		arSub51,
		arSub52,
		0,

	};
	static int arNumberOfSubtype[_countof(arSubType)] = {
		0,
		_countof(arSub1),
		_countof(arSub2),
		_countof(arSub3),
		_countof(arSub4),
		_countof(arSub5),
		_countof(arSub6),
		_countof(arSub7),
		_countof(arSub8),
		0,
		_countof(arSub10),
		0,
		_countof(arSub12),
		0,
		_countof(arSub14),
		0,
		_countof(arSub16),
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		_countof(arSub28),
		_countof(arSub29),
		_countof(arSub29),
		_countof(arSub31),
		0, // 32
		0,
		0,
		_countof(arSub35), // 35
#ifdef ENABLE_NEW_PET_SYSTEM
		0,
		_countof(arSubPet),
#endif
#ifdef TITLE_SYSTEM_BYLUZER
		0,
#endif
#ifdef ENABLE_CHEQUE_SYSTEM
		0,	// 35
#endif
//#ifdef ENABLE_SYSTEM_RUNE
		0,	//35
		0,	//36
		0,	//37
		0,	//38
		0,	//39
		0,	//40
//#endif
#ifdef ENABLE_ARTEFAKT_SYSTEM
		sizeof(arSub46)/sizeof(arSub46[0]), // 35
		#endif
		0, // 36
		0, // 37
		0, // 38
		0,
		sizeof(arSub51)/sizeof(arSub51[0]),
		sizeof(arSub52)/sizeof(arSub52[0]),

	};

	assert(_countof(arSubType) > type_value && "Subtype rule: Out of range!!");

	if (_countof(arSubType) <= type_value)
	{
		sys_err("SubType : Out of range!! (type_value: %d, count of registered subtype: %d", type_value, _countof(arSubType));
		return -1;
	}

	if (arSubType[type_value]==0) {
		return 0;
	}

	int retInt = -1;
	//cout << "SubType : " << subTypeStr << " -> ";
	string tempInputString = trim(inputString);
	for (int j=0;j<arNumberOfSubtype[type_value];j++) {
		string tempString = arSubType[type_value][j];
		if	(tempInputString.compare(tempString)==0)
		{
			//cout << j << " ";
			retInt =  j;
			break;
		}
	}
	//cout << endl;

	return retInt;
}

int get_Item_AntiFlag_Value(string inputString)
{
#ifdef ENABLE_NUMERIC_FIELD
	if (_IsNumericString(inputString))
		return std::stoi(inputString);
#endif
	string arAntiFlag[] = {"ANTI_FEMALE", "ANTI_MALE", "ANTI_MUSA", "ANTI_ASSASSIN", "ANTI_SURA", "ANTI_MUDANG",
							"ANTI_GET", "ANTI_DROP", "ANTI_SELL", "ANTI_EMPIRE_A", "ANTI_EMPIRE_B", "ANTI_EMPIRE_C",
							"ANTI_SAVE", "ANTI_GIVE", "ANTI_PKDROP", "ANTI_STACK", "ANTI_MYSHOP", "ANTI_SAFEBOX", "ANTI_WOLFMAN",
							"ANTI_PET20", "ANTI_PET21"};

	int retValue = 0;
	string* arInputString = StringSplit(inputString, "|");
	for(size_t i=0;i<_countof(arAntiFlag);i++) {
		string tempString = arAntiFlag[i];
		for (size_t j=0; j<30 ; j++)
		{
			string tempString2 = arInputString[j];
			if (tempString2.compare(tempString)==0) {
				retValue = retValue + pow((float)2,(float)i);
			}

			if(tempString2.compare("") == 0)
				break;
		}
	}
	delete []arInputString;
	//cout << "AntiFlag : " << antiFlagStr << " -> " << retValue << endl;

	return retValue;
}

int get_Item_Flag_Value(string inputString)
{
#ifdef ENABLE_NUMERIC_FIELD
	if (_IsNumericString(inputString))
		return std::stoi(inputString);
#endif
	string arFlag[] = {"ITEM_TUNABLE", "ITEM_SAVE", "ITEM_STACKABLE", "COUNT_PER_1GOLD", "ITEM_SLOW_QUERY", "ITEM_UNIQUE",
			"ITEM_MAKECOUNT", "ITEM_IRREMOVABLE", "CONFIRM_WHEN_USE", "QUEST_USE", "QUEST_USE_MULTIPLE",
			"QUEST_GIVE", "ITEM_QUEST", "LOG", "STACKABLE", "SLOW_QUERY", "REFINEABLE", "IRREMOVABLE", "ITEM_APPLICABLE"};

	int retValue = 0;
	string* arInputString = StringSplit(inputString, "|");
	for(size_t i=0;i<_countof(arFlag);i++) {
		string tempString = arFlag[i];
		for (size_t j=0; j<30 ; j++)
		{
			string tempString2 = arInputString[j];
			if (tempString2.compare(tempString)==0) {
				retValue = retValue + pow((float)2,(float)i);
			}

			if(tempString2.compare("") == 0)
				break;
		}
	}
	delete []arInputString;
	//cout << "Flag : " << flagStr << " -> " << retValue << endl;

	return retValue;
}

int get_Item_WearFlag_Value(string inputString)
{
#ifdef ENABLE_NUMERIC_FIELD
	if (_IsNumericString(inputString))
		return std::stoi(inputString);
#endif
	string arWearrFlag[] = {"WEAR_BODY", "WEAR_HEAD", "WEAR_FOOTS", "WEAR_WRIST", "WEAR_WEAPON", "WEAR_NECK", "WEAR_EAR", "WEAR_SHIELD", "WEAR_UNIQUE",
					"WEAR_ARROW", "WEAR_HAIR", "WEAR_ABILITY", "WEAR_COSTUME_BODY"
#ifdef ENABLE_ACCE_COSTUME_SYSTEM
					, "WEAR_COSTUME_ACCE"
#endif
					, "WEAR_MOUNT_EQUIPMENT_SHOE"
					, "WEAR_MOUNT_EQUIPMENT_SADDLE"
					, "WEAR_MOUNT_EQUIPMENT_ARMOR"
#ifdef __PENDANT__
					, "WEAR_PENDANT_FIRE"
					, "WEAR_PENDANT_ICE"
					, "WEAR_PENDANT_THUNDER"
					, "WEAR_PENDANT_WIND"
					, "WEAR_PENDANT_DARK"
					, "WEAR_PENDANT_EARTH"
					, "WEAR_PENDANT_SOUL_FIRST"
					, "WEAR_PENDANT_SOUL_SECOND"
#endif
					, "WEAR_SKILL_COSTUME_1"
					, "WEAR_SKILL_COSTUME_2"
					, "WEAR_SKILL_COSTUME_3"
					, "WEAR_SKILL_COSTUME_4"
					, "WEAR_SKILL_COSTUME_5"
					, "WEAR_SKILL_COSTUME_6"
	};

	int retValue = 0;
	string* arInputString = StringSplit(inputString, "|");
	for(size_t i=0;i<_countof(arWearrFlag);i++) {
		string tempString = arWearrFlag[i];
		for (size_t j=0; j<30 ; j++)
		{
			string tempString2 = arInputString[j];
			if (tempString2.compare(tempString)==0) {
				retValue = retValue + pow((float)2,(float)i);
			}

			if(tempString2.compare("") == 0)
				break;
		}
	}
	delete []arInputString;
	//cout << "WearFlag : " << wearFlagStr << " -> " << retValue << endl;

	return retValue;
}

int get_Item_Immune_Value(string inputString)
{
#ifdef ENABLE_NUMERIC_FIELD
	if (_IsNumericString(inputString))
		return std::stoi(inputString);
#endif
	string arImmune[] = {"PARA","CURSE","STUN","SLEEP","SLOW","POISON","TERROR"};

	int retValue = 0;
	string* arInputString = StringSplit(inputString, "|");
	for(size_t i=0;i<_countof(arImmune);i++) {
		string tempString = arImmune[i];
		for (size_t j=0; j<30 ; j++)
		{
			string tempString2 = arInputString[j];
			if (tempString2.compare(tempString)==0) {
				retValue = retValue + pow((float)2,(float)i);
			}

			if(tempString2.compare("") == 0)
				break;
		}
	}
	delete []arInputString;
	//cout << "Immune : " << immuneStr << " -> " << retValue << endl;

	return retValue;
}

int get_Item_LimitType_Value(string inputString)
{
#ifdef ENABLE_NUMERIC_FIELD
	if (_IsNumericString(inputString))
		return std::stoi(inputString);
#endif
	string arLimitType[] = {"LIMIT_NONE", "LEVEL", "STR", "DEX", "INT", "CON", "PC_BANG", "REAL_TIME", "REAL_TIME_FIRST_USE", "TIMER_BASED_ON_WEAR", "LIMIT_SECONDARY_LEVEL", "LIMIT_MOUNT_LEVEL"};

	int retInt = -1;
	//cout << "LimitType : " << limitTypeStr << " -> ";
	for (unsigned int j=0;j<_countof(arLimitType);j++) {
		string tempString = arLimitType[j];
		string tempInputString = trim(inputString);
		if	(tempInputString.compare(tempString)==0)
		{
			//cout << j << " ";
			retInt =  j;
			break;
		}
	}
	//cout << endl;

	return retInt;
}

int get_Item_ApplyType_Value(string inputString)
{
#ifdef ENABLE_NUMERIC_FIELD
	if (_IsNumericString(inputString))
		return std::stoi(inputString);
#endif
	string arApplyType[] = {"APPLY_NONE", "APPLY_MAX_HP", "APPLY_MAX_SP", "APPLY_CON", "APPLY_INT", "APPLY_STR", "APPLY_DEX", "APPLY_ATT_SPEED",
			"APPLY_MOV_SPEED", "APPLY_CAST_SPEED", "APPLY_HP_REGEN", "APPLY_SP_REGEN", "APPLY_POISON_PCT", "APPLY_STUN_PCT",
			"APPLY_SLOW_PCT", "APPLY_CRITICAL_PCT", "APPLY_PENETRATE_PCT", "APPLY_ATTBONUS_HUMAN", "APPLY_ATTBONUS_ANIMAL",
			"APPLY_ATTBONUS_ORC", "APPLY_ATTBONUS_MILGYO", "APPLY_ATTBONUS_UNDEAD", "APPLY_ATTBONUS_DEVIL", "APPLY_STEAL_HP",
			"APPLY_STEAL_SP", "APPLY_MANA_BURN_PCT", "APPLY_DAMAGE_SP_RECOVER", "APPLY_BLOCK", "APPLY_DODGE", "APPLY_RESIST_SWORD",
			"APPLY_RESIST_TWOHAND", "APPLY_RESIST_DAGGER", "APPLY_RESIST_BELL", "APPLY_RESIST_FAN", "APPLY_RESIST_BOW", "APPLY_RESIST_FIRE",
			"APPLY_RESIST_ELEC", "APPLY_RESIST_MAGIC", "APPLY_RESIST_WIND", "APPLY_REFLECT_MELEE", "APPLY_REFLECT_CURSE", "APPLY_POISON_REDUCE",
			"APPLY_KILL_SP_RECOVER", "APPLY_EXP_DOUBLE_BONUS", "APPLY_GOLD_DOUBLE_BONUS", "APPLY_ITEM_DROP_BONUS", "APPLY_POTION_BONUS",
			"APPLY_KILL_HP_RECOVER", "APPLY_IMMUNE_STUN", "APPLY_IMMUNE_SLOW", "APPLY_IMMUNE_FALL", "APPLY_SKILL", "APPLY_BOW_DISTANCE",
			"APPLY_ATT_GRADE_BONUS", "APPLY_DEF_GRADE_BONUS", "APPLY_MAGIC_ATT_GRADE", "APPLY_MAGIC_DEF_GRADE", "APPLY_CURSE_PCT",
			"APPLY_MAX_STAMINA", "APPLY_ATTBONUS_WARRIOR", "APPLY_ATTBONUS_ASSASSIN", "APPLY_ATTBONUS_SURA", "APPLY_ATTBONUS_SHAMAN",
			"APPLY_ATTBONUS_MONSTER", "APPLY_MALL_ATTBONUS", "APPLY_MALL_DEFBONUS", "APPLY_MALL_EXPBONUS", "APPLY_MALL_ITEMBONUS",
			"APPLY_MALL_GOLDBONUS", "APPLY_MAX_HP_PCT", "APPLY_MAX_SP_PCT", "APPLY_SKILL_DAMAGE_BONUS", "APPLY_NORMAL_HIT_DAMAGE_BONUS",
			"APPLY_SKILL_DEFEND_BONUS", "APPLY_NORMAL_HIT_DEFEND_BONUS", "APPLY_PC_BANG_EXP_BONUS", "APPLY_PC_BANG_DROP_BONUS",
			"APPLY_EXTRACT_HP_PCT", "APPLY_RESIST_WARRIOR", "APPLY_RESIST_ASSASSIN", "APPLY_RESIST_SURA", "APPLY_RESIST_SHAMAN",
			"APPLY_ENERGY",	"APPLY_DEF_GRADE", "APPLY_COSTUME_ATTR_BONUS", "APPLY_MAGIC_ATTBONUS_PER", "APPLY_MELEE_MAGIC_ATTBONUS_PER",
			"APPLY_RESIST_ICE", "APPLY_RESIST_EARTH", "APPLY_RESIST_DARK", "APPLY_ANTI_CRITICAL_PCT", "APPLY_ANTI_PENETRATE_PCT",
			"APPLY_BLEEDING_REDUCE", "APPLY_BLEEDING_PCT", "APPLY_ATTBONUS_WOLFMAN", "APPLY_RESIST_WOLFMAN", "APPLY_RESIST_CLAW",
			"APPLY_ACCEDRAIN_RATE", "APPLY_RESIST_MAGIC_REDUCTION", // 97,98
			"APPLY_ENCHANT_ELECT", "APPLY_ENCHANT_FIRE", "APPLY_ENCHANT_ICE", "APPLY_ENCHANT_WIND", "APPLY_ENCHANT_EARTH", "APPLY_ENCHANT_DARK", // 99-104
			"APPLY_RESIST_HUMAN", "APPLY_ATTBONUS_BOSS", "APPLY_ATTBONUS_WLADCA", "APPLY_ATTBONUS_STONE", "APPLY_RESIST_BOSS" , "APPLY_RESIST_WLADCA",
			"APPLY_RESIST_MONSTER", "APPLY_STAT_BONUS", "APPLY_ATTBONUS_DUNGEON", "APPLY_ATTBONUS_LEGENDA", "APPLY_ATTBONUS_KLASY", "APPLY_RESIST_KLASY",
			"APPLY_DMG_BONUS", "APPLY_FINAL_DMG_BONUS",
			"APPLY_MINING_SUCCESS_CHANCE",
			"APPLY_MINING_MORE_ORES_CHANCE",
			"APPLY_MINING_LESS_MINE_TIME",
			"APPLY_MINING_MORE_POINTS",
			"APPLY_FISHING_INSTANT_CATCH_CHANCE",
			"APPLY_FISHING_BIGGER_FISH_CHANCE",
			"APPLY_FISHING_NEXT_SECTION_CHANCE",
			"APPLY_FISHING_MORE_YANG_MULTIPLIER",
			"APPLY_FISHING_MORE_POINTS_CHANCE",
			"APPLY_FISHING_SUCCESS_CHANCE",
			"APPLY_ATTBONUS_PVM_SKILL",
			"APPLY_ATTBONUS_ELEMENT",
			"APPLY_DEFBONUS_ELEMENT",
			"APPLY_DOUBLE_LOOT_SUMMER",
			"APPLY_DOUBLE_LOOT_DROP",
			"APPLY_ATTBONUS_PVM_STR",
	};

	int retInt = -1;
	//cout << "ApplyType : " << applyTypeStr << " -> ";
	for (size_t j=0;j<_countof(arApplyType);j++) {
		string tempString = arApplyType[j];
		string tempInputString = trim(inputString);
		if	(tempInputString.compare(tempString)==0)
		{
			//cout << j << " ";
			retInt =  j;
			break;
		}
	}
	//cout << endl;

	return retInt;
}

int get_Mob_Rank_Value(string inputString)
{
#ifdef ENABLE_NUMERIC_FIELD
	if (_IsNumericString(inputString))
		return std::stoi(inputString);
#endif
	string arRank[] = {"PAWN", "S_PAWN", "KNIGHT", "S_KNIGHT", "BOSS", "KING", "WLADCA", "LEGENDA"};

	int retInt = -1;
	//cout << "Rank : " << rankStr << " -> ";
	for (size_t j=0;j<_countof(arRank);j++) {
		string tempString = arRank[j];
		string tempInputString = trim(inputString);
		if	(tempInputString.compare(tempString)==0)
		{
			//cout << j << " ";
			retInt =  j;
			break;
		}
	}
	//cout << endl;

	return retInt;
}

int get_Mob_Type_Value(string inputString)
{
#ifdef ENABLE_NUMERIC_FIELD
	if (_IsNumericString(inputString))
		return std::stoi(inputString);
#endif
	string arType[] = { "MONSTER", "NPC", "STONE", "WARP", "DOOR", "BUILDING", "PC", "POLYMORPH_PC", "HORSE", "GOTO"};

	int retInt = -1;
	//cout << "Type : " << typeStr << " -> ";
	for (size_t j=0;j<_countof(arType);j++) {
		string tempString = arType[j];
		string tempInputString = trim(inputString);
		if	(tempInputString.compare(tempString)==0)
		{
			//cout << j << " ";
			retInt =  j;
			break;
		}
	}
	//cout << endl;

	return retInt;
}

int get_Mob_BattleType_Value(string inputString)
{
#ifdef ENABLE_NUMERIC_FIELD
	if (_IsNumericString(inputString))
		return std::stoi(inputString);
#endif
	string arBattleType[] = { "MELEE", "RANGE", "MAGIC", "SPECIAL", "POWER", "TANKER", "SUPER_POWER", "SUPER_TANKER"};

	int retInt = -1;
	//cout << "Battle Type : " << battleTypeStr << " -> ";
	for (size_t j=0;j<_countof(arBattleType);j++) {
		string tempString = arBattleType[j];
		string tempInputString = trim(inputString);
		if	(tempInputString.compare(tempString)==0)
		{
			//cout << j << " ";
			retInt =  j;
			break;
		}
	}
	//cout << endl;

	return retInt;
}

int get_Mob_Size_Value(string inputString)
{
#ifdef ENABLE_NUMERIC_FIELD
	if (_IsNumericString(inputString))
		return std::stoi(inputString);
#endif
	string arSize[] = { "SMALL", "MEDIUM", "BIG"}; //@fixme201 SAMLL to SMALL

	int retInt = 0;
	//cout << "Size : " << sizeStr << " -> ";
	for (size_t j=0;j<_countof(arSize);j++) {
		string tempString = arSize[j];
		string tempInputString = trim(inputString);
		if	(tempInputString.compare(tempString)==0)
		{
			//cout << j << " ";
			retInt =  j + 1;
			break;
		}
	}
	//cout << endl;

	return retInt;
}

int get_Mob_AIFlag_Value(string inputString)
{
#ifdef ENABLE_NUMERIC_FIELD
	if (_IsNumericString(inputString))
		return std::stoi(inputString);
#endif
	string arAIFlag[] = {"AGGR","NOMOVE","COWARD","NOATTSHINSU","NOATTCHUNJO","NOATTJINNO","ATTMOB","BERSERK","STONESKIN","GODSPEED","DEATHBLOW","REVIVE",
	};

	int retValue = 0;
	string* arInputString = StringSplit(inputString, ",");
	for (size_t i =0;i<_countof(arAIFlag);i++) {
		string tempString = arAIFlag[i];
		for (size_t j=0; j<30 ; j++)
		{
			string tempString2 = arInputString[j];
			if (tempString2.compare(tempString)==0) {
				retValue = retValue + pow((float)2,(float)i);
			}

			if(tempString2.compare("") == 0)
				break;
		}
	}
	delete []arInputString;
	//cout << "AIFlag : " << aiFlagStr << " -> " << retValue << endl;

	return retValue;
}
int get_Mob_RaceFlag_Value(string inputString)
{
#ifdef ENABLE_NUMERIC_FIELD
	if (_IsNumericString(inputString))
		return std::stoi(inputString);
#endif
	string arRaceFlag[] = {"ANIMAL","UNDEAD","DEVIL","HUMAN","ORC","MILGYO","INSECT","FIRE","ICE","DESERT","TREE",
		"ATT_ELEC","ATT_FIRE","ATT_ICE","ATT_WIND","ATT_EARTH","ATT_DARK", "BOSS", "WLADCA", "STONE", "DUNGEON"};

	int retValue = 0;
	string* arInputString = StringSplit(inputString, ",");
	for(size_t i =0;i<_countof(arRaceFlag);i++) {
		string tempString = arRaceFlag[i];
		for (size_t j=0; j<30 ; j++)
		{
			string tempString2 = arInputString[j];
			if (tempString2.compare(tempString)==0) {
				retValue = retValue + pow((float)2,(float)i);
			}

			if(tempString2.compare("") == 0)
				break;
		}
	}
	delete []arInputString;
	//cout << "Race Flag : " << raceFlagStr << " -> " << retValue << endl;

	return retValue;
}
int get_Mob_ImmuneFlag_Value(string inputString)
{
#ifdef ENABLE_NUMERIC_FIELD
	if (_IsNumericString(inputString))
		return std::stoi(inputString);
#endif
	string arImmuneFlag[] = {"STUN","SLOW","FALL","CURSE","POISON","TERROR", "REFLECT"};

	int retValue = 0;
	string* arInputString = StringSplit(inputString, ",");
	for(size_t i =0;i<_countof(arImmuneFlag);i++) {
		string tempString = arImmuneFlag[i];
		for (size_t j=0; j<30 ; j++)
		{
			string tempString2 = arInputString[j];
			if (tempString2.compare(tempString)==0) {
				retValue = retValue + pow((float)2,(float)i);
			}

			if(tempString2.compare("") == 0)
				break;
		}
	}
	delete []arInputString;
	//cout << "Immune Flag : " << immuneFlagStr << " -> " << retValue << endl;

	return retValue;
}

#ifndef __DUMP_PROTO__

bool Set_Proto_Mob_Table(TMobTable *mobTable, cCsvTable &csvTable,std::map<int,const char*> &nameMap)
{
	int col = 0;
	str_to_number(mobTable->dwVnum, csvTable.AsStringByIndex(col++));
	strlcpy(mobTable->szName, csvTable.AsStringByIndex(col++), sizeof(mobTable->szName));

	auto it = nameMap.find(mobTable->dwVnum);
	if (it != nameMap.end()) {
		const char * localeName = it->second;
		strlcpy(mobTable->szLocaleName, localeName, sizeof(mobTable->szLocaleName));
	} else {
		strlcpy(mobTable->szLocaleName, mobTable->szName, sizeof(mobTable->szLocaleName));
	}

	//RANK
	int rankValue = get_Mob_Rank_Value(csvTable.AsStringByIndex(col++));
	mobTable->bRank = rankValue;
	//TYPE
	int typeValue = get_Mob_Type_Value(csvTable.AsStringByIndex(col++));
	mobTable->bType = typeValue;
	//BATTLE_TYPE
	int battleTypeValue = get_Mob_BattleType_Value(csvTable.AsStringByIndex(col++));
	mobTable->bBattleType = battleTypeValue;

	str_to_number(mobTable->bLevel, csvTable.AsStringByIndex(col++));
	//SIZE
	int sizeValue = get_Mob_Size_Value(csvTable.AsStringByIndex(col++));
	mobTable->bSize = sizeValue;
	//AI_FLAG
	int aiFlagValue = get_Mob_AIFlag_Value(csvTable.AsStringByIndex(col++));
	mobTable->dwAIFlag = aiFlagValue;
	//mount_capacity;
	col++;
	//RACE_FLAG
	int raceFlagValue = get_Mob_RaceFlag_Value(csvTable.AsStringByIndex(col++));
	mobTable->dwRaceFlag = raceFlagValue;
	//IMMUNE_FLAG
	int immuneFlagValue = get_Mob_ImmuneFlag_Value(csvTable.AsStringByIndex(col++));
	mobTable->dwImmuneFlag = immuneFlagValue;

	str_to_number(mobTable->bEmpire, csvTable.AsStringByIndex(col++));  //col = 11

	strlcpy(mobTable->szFolder, csvTable.AsStringByIndex(col++), sizeof(mobTable->szFolder));

	str_to_number(mobTable->bOnClickType, csvTable.AsStringByIndex(col++));

	str_to_number(mobTable->bStr, csvTable.AsStringByIndex(col++));
	str_to_number(mobTable->bDex, csvTable.AsStringByIndex(col++));
	str_to_number(mobTable->bCon, csvTable.AsStringByIndex(col++));
	str_to_number(mobTable->bInt, csvTable.AsStringByIndex(col++));
	str_to_number(mobTable->dwDamageRange[0], csvTable.AsStringByIndex(col++));
	str_to_number(mobTable->dwDamageRange[1], csvTable.AsStringByIndex(col++));
	{
		const char* hp_str = csvTable.AsStringByIndex(col++);
		if (hp_str && *hp_str)
		{
			int64_t temp_hp = std::strtoll(hp_str, nullptr, 10);
			mobTable->dwMaxHP = temp_hp;
		}
		else
		{
			mobTable->dwMaxHP = 0;
		}
	}
	str_to_number(mobTable->bRegenCycle, csvTable.AsStringByIndex(col++));
	str_to_number(mobTable->bRegenPercent,	csvTable.AsStringByIndex(col++));
	str_to_number(mobTable->dwGoldMin, csvTable.AsStringByIndex(col++));
	str_to_number(mobTable->dwGoldMax, csvTable.AsStringByIndex(col++));
	str_to_number(mobTable->dwExp,	csvTable.AsStringByIndex(col++));
	str_to_number(mobTable->wDef, csvTable.AsStringByIndex(col++));
	str_to_number(mobTable->sAttackSpeed, csvTable.AsStringByIndex(col++));
	str_to_number(mobTable->sMovingSpeed, csvTable.AsStringByIndex(col++));
	str_to_number(mobTable->bAggresiveHPPct, csvTable.AsStringByIndex(col++));
	str_to_number(mobTable->wAggressiveSight, csvTable.AsStringByIndex(col++));
	str_to_number(mobTable->wAttackRange, csvTable.AsStringByIndex(col++));

	str_to_number(mobTable->dwDropItemVnum, csvTable.AsStringByIndex(col++));	//32
	str_to_number(mobTable->dwResurrectionVnum, csvTable.AsStringByIndex(col++));
	for (int i = 0; i < MOB_ENCHANTS_MAX_NUM; ++i)
		str_to_number(mobTable->cEnchants[i], csvTable.AsStringByIndex(col++));

	for (int i = 0; i < MOB_RESISTS_MAX_NUM; ++i)
		str_to_number(mobTable->cResists[i], csvTable.AsStringByIndex(col++));

	str_to_number(mobTable->fDamMultiply, csvTable.AsStringByIndex(col++));
	str_to_number(mobTable->dwSummonVnum, csvTable.AsStringByIndex(col++));
	str_to_number(mobTable->dwDrainSP, csvTable.AsStringByIndex(col++));

	//Mob_Color
	++col;

	str_to_number(mobTable->dwPolymorphItemVnum, csvTable.AsStringByIndex(col++));

	str_to_number(mobTable->Skills[0].bLevel, csvTable.AsStringByIndex(col++));
	str_to_number(mobTable->Skills[0].dwVnum, csvTable.AsStringByIndex(col++));
	str_to_number(mobTable->Skills[1].bLevel, csvTable.AsStringByIndex(col++));
	str_to_number(mobTable->Skills[1].dwVnum, csvTable.AsStringByIndex(col++));
	str_to_number(mobTable->Skills[2].bLevel, csvTable.AsStringByIndex(col++));
	str_to_number(mobTable->Skills[2].dwVnum, csvTable.AsStringByIndex(col++));
	str_to_number(mobTable->Skills[3].bLevel, csvTable.AsStringByIndex(col++));
	str_to_number(mobTable->Skills[3].dwVnum, csvTable.AsStringByIndex(col++));
	str_to_number(mobTable->Skills[4].bLevel, csvTable.AsStringByIndex(col++));
	str_to_number(mobTable->Skills[4].dwVnum, csvTable.AsStringByIndex(col++));

	str_to_number(mobTable->bBerserkPoint, csvTable.AsStringByIndex(col++));
	str_to_number(mobTable->bStoneSkinPoint, csvTable.AsStringByIndex(col++));
	str_to_number(mobTable->bGodSpeedPoint, csvTable.AsStringByIndex(col++));
	str_to_number(mobTable->bDeathBlowPoint, csvTable.AsStringByIndex(col++));
	str_to_number(mobTable->bRevivePoint, csvTable.AsStringByIndex(col++));
#ifdef ENABLE_PUNKTY_OSIAGNIEC
	str_to_number(mobTable->sPktOsiag, csvTable.AsStringByIndex(col++));
#endif

	sys_log(0, "MOB #%-5d %-24s level: %-3u rank: %u empire: %d", mobTable->dwVnum, mobTable->szLocaleName, mobTable->bLevel, mobTable->bRank, mobTable->bEmpire);

	return true;
}

bool Set_Proto_Item_Table(TItemTable *itemTable, cCsvTable &csvTable,std::map<int,const char*> &nameMap)
{
	int col = 0;

	int dataArray[34];
	for (size_t i=0; i<_countof(dataArray);i++) {
		int validCheck = 0;
		if (i==2) {
			dataArray[i] = get_Item_Type_Value(csvTable.AsStringByIndex(col));
			validCheck = dataArray[i];
		} else if (i==3) {
			dataArray[i] = get_Item_SubType_Value((unsigned int)dataArray[i-1], csvTable.AsStringByIndex(col));
			validCheck = dataArray[i];
		} else if (i==5) {
			dataArray[i] = get_Item_AntiFlag_Value(csvTable.AsStringByIndex(col));
			validCheck = dataArray[i];
		} else if (i==6) {
			dataArray[i] = get_Item_Flag_Value(csvTable.AsStringByIndex(col));
			validCheck = dataArray[i];
		} else if (i==7) {
			dataArray[i] = get_Item_WearFlag_Value(csvTable.AsStringByIndex(col));
			validCheck = dataArray[i];
		} else if (i==8) {
			dataArray[i] = get_Item_Immune_Value(csvTable.AsStringByIndex(col));
			validCheck = dataArray[i];
		} else if (i==14) {
			dataArray[i] = get_Item_LimitType_Value(csvTable.AsStringByIndex(col));
			validCheck = dataArray[i];
		} else if (i==16) {
			dataArray[i] = get_Item_LimitType_Value(csvTable.AsStringByIndex(col));
			validCheck = dataArray[i];
		} else if (i==18) {
			dataArray[i] = get_Item_ApplyType_Value(csvTable.AsStringByIndex(col));
			validCheck = dataArray[i];
		} else if (i==20) {
			dataArray[i] = get_Item_ApplyType_Value(csvTable.AsStringByIndex(col));
			validCheck = dataArray[i];
		} else if (i==22) {
			dataArray[i] = get_Item_ApplyType_Value(csvTable.AsStringByIndex(col));
			validCheck = dataArray[i];
		} else {
			str_to_number(dataArray[i], csvTable.AsStringByIndex(col));
		}

		if (validCheck == -1)
		{
			std::ostringstream dataStream;

			for (size_t j = 0; j < i; ++j)
				dataStream << dataArray[j] << ",";

			//fprintf(stderr, "ItemProto Reading Failed : Invalid value.\n");
			sys_err("ItemProto Reading Failed : Invalid value. (index: %d, col: %d, value: %s)", i, col, csvTable.AsStringByIndex(col));
			sys_err("\t%d ~ %d Values: %s", 0, i, dataStream.str().c_str());

			exit(0);
		}

		col = col + 1;
	}

	{
		std::string s(csvTable.AsStringByIndex(0));
		size_t pos = s.find("~");
		if (std::string::npos == pos)
		{
			itemTable->dwVnum = dataArray[0];
			itemTable->dwVnumRange = 0;
		}
		else
		{
			std::string s_start_vnum (s.substr(0, pos));
			std::string s_end_vnum (s.substr(pos +1 ));

			int start_vnum = atoi(s_start_vnum.c_str());
			int end_vnum = atoi(s_end_vnum.c_str());
			if (0 == start_vnum || (0 != end_vnum && end_vnum < start_vnum))
			{
				sys_err ("INVALID VNUM %s", s.c_str());
				return false;
			}
			itemTable->dwVnum = start_vnum;
			itemTable->dwVnumRange = end_vnum - start_vnum;
		}
	}

	strlcpy(itemTable->szName, csvTable.AsStringByIndex(1), sizeof(itemTable->szName));
	auto it = nameMap.find(itemTable->dwVnum);
	if (it != nameMap.end()) {
		const char * localeName = it->second;
		strlcpy(itemTable->szLocaleName, localeName, sizeof(itemTable->szLocaleName));
	} else {
		strlcpy(itemTable->szLocaleName, itemTable->szName, sizeof(itemTable->szLocaleName));
	}
	itemTable->bType = dataArray[2];
	itemTable->bSubType = dataArray[3];
	itemTable->bSize = MINMAX(1, dataArray[4], 3); // @fixme179
	itemTable->dwAntiFlags = dataArray[5];
	itemTable->dwFlags = dataArray[6];
	itemTable->dwWearFlags = dataArray[7];
	itemTable->dwImmuneFlag = dataArray[8];
	itemTable->dwGold = dataArray[9];
	itemTable->dwShopBuyPrice = dataArray[10];
	itemTable->dwRefinedVnum = dataArray[11];
	itemTable->wRefineSet = dataArray[12];
	itemTable->bAlterToMagicItemPct = dataArray[13];
	itemTable->cLimitRealTimeFirstUseIndex = -1;
	itemTable->cLimitTimerBasedOnWearIndex = -1;

	int i;

	for (i = 0; i < ITEM_LIMIT_MAX_NUM; ++i)
	{
		itemTable->aLimits[i].bType = dataArray[14+i*2];
		itemTable->aLimits[i].lValue = dataArray[15+i*2];

		if (LIMIT_REAL_TIME_START_FIRST_USE == itemTable->aLimits[i].bType)
			itemTable->cLimitRealTimeFirstUseIndex = (char)i;

		if (LIMIT_TIMER_BASED_ON_WEAR == itemTable->aLimits[i].bType)
			itemTable->cLimitTimerBasedOnWearIndex = (char)i;

	}

	for (i = 0; i < ITEM_APPLY_MAX_NUM; ++i)
	{
		itemTable->aApplies[i].bType = dataArray[18+i*2];
		itemTable->aApplies[i].lValue = dataArray[19+i*2];
	}

	for (i = 0; i < ITEM_VALUES_MAX_NUM; ++i)
		itemTable->alValues[i] = dataArray[24+i];

	//column for 'Specular'
	itemTable->bGainSocketPct = dataArray[31];
	itemTable->sAddonType = dataArray[32];
#ifdef ENABLE_SPLIT_INVENTORY_SYSTEM
	itemTable->SpecialInvType = dataArray[33];
	itemTable->CanUseInSpecialInv = dataArray[34];
#endif

	//test
	str_to_number(itemTable->bWeight, "0");

#ifdef ENABLE_CHECK_SELL_PRICE
	if (itemTable->dwShopBuyPrice > itemTable->dwGold)
	{
		itemTable->dwShopBuyPrice = itemTable->dwGold;
		sys_err("ITEM: #%-5lu %-24s SELL_OVERFLOW ShopBuyPrice: %u > Gold %u",
			itemTable->dwVnum,
			itemTable->szLocaleName,
			itemTable->dwShopBuyPrice,
			itemTable->dwGold
		);
	}
#endif

	return true;
}

#endif
//martysama0134's ceqyqttoaf71vasf9t71218
