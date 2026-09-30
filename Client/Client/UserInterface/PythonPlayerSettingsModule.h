#pragma once
#include <thread>
#include <array>

#include "../gamelib/MapManager.h"
#include "PythonItem.h"
#include "InstanceBase.h"
#include "PythonPlayer.h"
#include "locale_inc.h"


#ifdef ENABLE_PERFORMANCE_IMPROVEMENTS_NEW
static bool loaded = false;
static const std::string g_stNPCList_FileName = "npclist.txt";
static constexpr bool g_bCacheMotionData = false;

static const char* GetLocaleInfoString(const std::string& c_rstToken)
{
	PyObject* ppyModule = PyImport_AddModule("localeInfo");
	PyObject* ppyObject = PyObject_GetAttrString(ppyModule, c_rstToken.c_str());

	const char* szText = PyString_AsString(ppyObject);
	if ((szText == nullptr) || (szText[0] == '\0'))
		return _strdup("NoName");

	return szText;
}

// Effect
using EffectData = struct SEffectData
{
	UINT iEftType;
	std::string stEftAttachBone;
	std::string stEftFileName;
	bool bCache = false;
};
using EffectDataVector = std::vector<SEffectData>;

typedef struct SSound
{
	DWORD dwType;
	char stName[70];
} TSound;

class CPythonPlayerSettingsModule : public CMapManager, public CSingleton<CPythonPlayerSettingsModule>
{
public:
	CPythonPlayerSettingsModule();
	virtual ~CPythonPlayerSettingsModule() = default;
	void Load();

	enum Config {
		FILE_MAX_NUM = 70,
		RACE_WARRIOR_M = 0,
		RACE_ASSASSIN_W = 1,
		RACE_SURA_M = 2,
		RACE_SHAMAN_W = 3,
		RACE_WARRIOR_W = 4,
		RACE_ASSASSIN_M = 5,
		RACE_SURA_W = 6,
		RACE_SHAMAN_M = 7,
	};

public:
	void RegisterSkills();
	void RegisterSkills(uint32_t race, uint8_t skill_group, uint8_t empire);
	void __RegisterDungeonMapNames();
	void __RegisterTitleNames();
	void __RegisterTitleAchievementNames();
	void __RegisterNameColors();
	void __RegisterTitleColors();
	void __RegisterEmotionIcons();
	void __LoadGameInit();
	void __LoadGameSound();
	void __LoadGameEffects();
	void __LoadGameNPC();
	void __LoadEmoticons();
	void LoadingRaces();
	const static bool RegisterEmotionAnis(char stFolder[FILE_MAX_NUM]);
	const static bool LoadInitData();
	// void __LoadRaceHeight();

	//recoded
	const static bool LoadGeneralMotion(char stFolder[FILE_MAX_NUM]);
	const static bool LoadGameWarrior(DWORD dwRace, char stFolder[FILE_MAX_NUM]);
	const static bool LoadGameAssassin(DWORD dwRace, char stFolder[FILE_MAX_NUM]);
	const static bool LoadGameSura(DWORD dwRace, char stFolder[FILE_MAX_NUM]);
	const static bool LoadGameShaman(DWORD dwRace, char stFolder[FILE_MAX_NUM]);
#ifdef ENABLE_SKILL_COSTUME_SYSTEM
private:
	void LoadNewSkills();
	void RegisterLoopData(int loadSkillCount);
	void LoadNewWarriorSkillsEx(DWORD dwRace, const std::string& path, int loadSkillCount);
	void LoadNewAssassinSkillsEx(DWORD dwRace, const std::string& path, int loadSkillCount);
	void LoadNewSuraSkillsEx(DWORD dwRace, const std::string& path, int loadSkillCount);
	void LoadNewShamanSkillsEx(DWORD dwRace, const std::string& path, int loadSkillCount);
#endif
protected:
	std::thread m_loadThread;
	bool isLoaded;
	
private:
	enum EGuildBuildingListToken
	{
		GUIILD_BUILDING_TOKEN_VNUM,
		GUIILD_BUILDING_TOKEN_TYPE,
		GUIILD_BUILDING_TOKEN_NAME,
		GUIILD_BUILDING_TOKEN_LOCAL_NAME,
		GUIILD_BUILDING_NO_USE_TOKEN_SIZE_1,
		GUIILD_BUILDING_NO_USE_TOKEN_SIZE_2,
		GUIILD_BUILDING_NO_USE_TOKEN_SIZE_3,
		GUIILD_BUILDING_NO_USE_TOKEN_SIZE_4,
		GUIILD_BUILDING_TOKEN_X_ROT_LIMIT,
		GUIILD_BUILDING_TOKEN_Y_ROT_LIMIT,
		GUIILD_BUILDING_TOKEN_Z_ROT_LIMIT,
		GUIILD_BUILDING_TOKEN_PRICE,
		GUIILD_BUILDING_TOKEN_MATERIAL,
		GUIILD_BUILDING_TOKEN_NPC,
		GUIILD_BUILDING_TOKEN_GROUP,
		GUIILD_BUILDING_TOKEN_DEPEND_GROUP,
		GUIILD_BUILDING_TOKEN_ENABLE_FLAG,
		GUIILD_BUILDING_LIMIT_TOKEN_COUNT,
	} EGuildBuildingListToken;

	enum ENPCListToken : BYTE
	{
		NPC_TOKEN_TYPE_VNUM,
		NPC_TOKEN_TYPE_MODEL_NAME,
		NPC_LIST_TOKEN_MODEL_SRC_NAME,
		NPC_LIST_TOKEN_MAX,
	};

	std::map<std::string_view, std::string> m_buildingTypeToFolder{
		{"HEADQUARTER", "headquarter"},
		{"FACILITY", "facility"},
		{"OBJECT", "object"},
		{"WALL", "fence"},
	};

	typedef struct SMotion
	{
		int wMotionIndex;
		char stName[FILE_MAX_NUM];
		BYTE byPercentage;
	} TMotion;

	typedef struct SRaceData
	{
		DWORD dwRace;
		char stMSM[FILE_MAX_NUM];
		char stIntroMotion[FILE_MAX_NUM];
	} TRaceData;
	
};
	
static const std::map<BYTE, const char*> emotionIcons =
{
	{ CPythonPlayer::EMOTION_CLAP, "d:/ymir work/ui/game/windows/emotion_clap.sub" },
	{ CPythonPlayer::EMOTION_CHEERS_1, "d:/ymir work/ui/game/windows/emotion_cheers_1.sub" },
	{ CPythonPlayer::EMOTION_CHEERS_2, "d:/ymir work/ui/game/windows/emotion_cheers_2.sub" },
	{ CPythonPlayer::EMOTION_DANCE_1, "icon/action/dance1.tga" },
	{ CPythonPlayer::EMOTION_DANCE_2, "icon/action/dance2.tga" },
	{ CPythonPlayer::EMOTION_CONGRATULATION, "icon/action/congratulation.tga" },
	{ CPythonPlayer::EMOTION_FORGIVE, "icon/action/forgive.tga" },
	{ CPythonPlayer::EMOTION_ANGRY, "icon/action/angry.tga" },
	{ CPythonPlayer::EMOTION_ATTRACTIVE, "icon/action/attractive.tga" },
	{ CPythonPlayer::EMOTION_SAD, "icon/action/sad.tga" },
	{ CPythonPlayer::EMOTION_SHY, "icon/action/shy.tga" },
	{ CPythonPlayer::EMOTION_CHEERUP, "icon/action/cheerup.tga" },
	{ CPythonPlayer::EMOTION_BANTER, "icon/action/banter.tga" },
	{ CPythonPlayer::EMOTION_JOY, "icon/action/joy.tga" },
	{ CPythonPlayer::EMOTION_DANCE_1, "icon/action/dance1.tga" },
	{ CPythonPlayer::EMOTION_DANCE_2, "icon/action/dance2.tga" },
	{ CPythonPlayer::EMOTION_DANCE_3, "icon/action/dance3.tga" },
	{ CPythonPlayer::EMOTION_DANCE_4, "icon/action/dance4.tga" },
	{ CPythonPlayer::EMOTION_DANCE_5, "icon/action/dance5.tga" },
	{ CPythonPlayer::EMOTION_DANCE_6, "icon/action/dance6.tga" },
	{ CPythonPlayer::EMOTION_KISS, "d:/ymir work/ui/game/windows/emotion_kiss.sub" },
	{ CPythonPlayer::EMOTION_FRENCH_KISS, "d:/ymir work/ui/game/windows/emotion_french_kiss.sub" },
	{ CPythonPlayer::EMOTION_SLAP, "d:/ymir work/ui/game/windows/emotion_slap.sub" },
};

	
typedef std::array<uint8_t, 3> RGB;
typedef std::map<uint8_t, RGB> TCharacterColorMap;

static const TCharacterColorMap characterNameColors
{
	{ CInstanceBase::NAMECOLOR_NORMAL_PC, { 255, 215, 76 }},
	{ CInstanceBase::NAMECOLOR_NORMAL_NPC, { 122, 231, 93 }},
	{ CInstanceBase::NAMECOLOR_NORMAL_MOB, { 235, 22, 9 }},
	//{ CInstanceBase::NAMECOLOR_BOSS, { 255, 215, 0 }},
	{ CInstanceBase::NAMECOLOR_PVP, { 238, 54, 223 }},
	{ CInstanceBase::NAMECOLOR_PK, { 180, 100, 0 }},
	{ CInstanceBase::NAMECOLOR_PARTY, { 128, 192, 255 }},
	{ CInstanceBase::NAMECOLOR_WARP, { 136, 218, 241 }},
	{ CInstanceBase::NAMECOLOR_WAYPOINT, { 255, 255, 255 }},
	{ CInstanceBase::NAMECOLOR_EMPIRE_MOB, { 235, 22, 9 }},
	{ CInstanceBase::NAMECOLOR_EMPIRE_NPC, { 122, 231, 93 }},
	{ CInstanceBase::NAMECOLOR_EMPIRE_PC + 1, { 157, 0, 0 }},
	{ CInstanceBase::NAMECOLOR_EMPIRE_PC + 2, { 222, 160, 47 }},
	{ CInstanceBase::NAMECOLOR_EMPIRE_PC + 3, { 23, 30, 138 }},

	//{ CInstanceBase::NAMECOLOR_OFFLINE_SHOP, { 163, 199, 80 }},

	#ifdef ENABLE_COMPANION_SYSTEM
	{ CInstanceBase::NAMECOLOR_COMPANION, { 153, 255, 255 }},
	#endif
};

static const TCharacterColorMap characterTitleColors
{
	{ 0, { 143, 36, 36 }},
	{ 1, { 255, 255, 150 }},
	{ 2, { 57, 250, 67 }},
	{ 3, { 250, 57, 57 }},
	{ 4, { 245, 66, 108 }},
	{ 5, { 203, 66, 245 }},
	{ 6, { 255, 140, 0 }},
	{ 7, { 245, 170, 66 }},
	{ 8, { 205, 255, 255 }},
};

typedef std::pair<BYTE, std::string> TSoundFileName;
static const std::array<TSoundFileName, CPythonItem::USESOUND_NUM - 1> vecUseSoundFileName =
{ {
	{CPythonItem::USESOUND_DEFAULT, "sound/ui/drop.wav"},
	{CPythonItem::USESOUND_ACCESSORY, "sound/ui/equip_ring_amulet.wav"},
	{CPythonItem::USESOUND_ARMOR, "sound/ui/equip_metal_armor.wav"},
	{CPythonItem::USESOUND_BOW, "sound/ui/equip_bow.wav"},
	{CPythonItem::USESOUND_WEAPON, "sound/ui/equip_metal_weapon.wav"},
	{CPythonItem::USESOUND_POTION, "sound/ui/eat_potion.wav"},
	{CPythonItem::USESOUND_PORTAL, "sound/ui/potal_scroll.wav"},
} };

static const std::array<TSoundFileName, CPythonItem::DROPSOUND_NUM> vecDropSoundFileName =
{ {
		{CPythonItem::DROPSOUND_DEFAULT, "sound/ui/drop.wav"},
		{CPythonItem::DROPSOUND_ACCESSORY, "sound/ui/equip_ring_amulet.wav"},
		{CPythonItem::DROPSOUND_ARMOR, "sound/ui/equip_metal_armor.wav"},
		{CPythonItem::DROPSOUND_BOW, "sound/ui/equip_bow.wav"},
		{CPythonItem::DROPSOUND_WEAPON, "sound/ui/equip_metal_weapon.wav"},
} };

static const EffectDataVector vecEffectList =
{
	{ CInstanceBase::EFFECT_SPAWN_APPEAR, "Bip01", "d:/ymir work/effect/etc/appear_die/monster_appear.mse", true},
	{ CInstanceBase::EFFECT_SPAWN_DISAPPEAR, "Bip01", "d:/ymir work/effect/etc/appear_die/monster_die.mse", true},
	{ CInstanceBase::EFFECT_FLAME_ATTACK, "equip_right_hand", "d:/ymir work/effect/hit/blow_flame/flame_3_weapon.mse", true},
	{ CInstanceBase::EFFECT_FLAME_HIT, "", "d:/ymir work/effect/hit/blow_flame/flame_3_blow.mse", true},
	{ CInstanceBase::EFFECT_FLAME_ATTACH, "", "d:/ymir work/effect/hit/blow_flame/flame_3_body.mse", true},
	{ CInstanceBase::EFFECT_ELECTRIC_ATTACK, "equip_right", "d:/ymir work/effect/hit/blow_electric/light_1_weapon.mse", true},
	{ CInstanceBase::EFFECT_ELECTRIC_HIT, "", "d:/ymir work/effect/hit/blow_electric/light_1_blow.mse", true},
	{ CInstanceBase::EFFECT_ELECTRIC_ATTACH, "", "d:/ymir work/effect/hit/blow_electric/light_1_body.mse", true},
	{ CInstanceBase::EFFECT_LEVELUP, "", "d:/ymir work/effect/etc/levelup_1/level_up.mse", true},
	{ CInstanceBase::EFFECT_SKILLUP, "", "d:/ymir work/effect/etc/skillup/skillup_1.mse", true},
	{ CInstanceBase::SE_PET_LV_UP, "", "d:/ymir work/effect/etc/levelup_1/level_up.mse", true},
	{ CInstanceBase::EFFECT_RUNA_WHITE_EQUIP, "", "d:/ymir work/effect/rune/buff_white.mse", true},
	{ CInstanceBase::EFFECT_RUNA_RED_EQUIP, "", "d:/ymir work/effect/rune/buff_red.mse", true},
	{ CInstanceBase::EFFECT_RUNA_BLUE_EQUIP, "", "d:/ymir work/effect/rune/buff_blue.mse", true},
	{ CInstanceBase::EFFECT_RUNA_YELLOW_EQUIP, "", "d:/ymir work/effect/rune/buff_yellow.mse", true},
	{ CInstanceBase::EFFECT_RUNA_BLACK_EQUIP, "", "d:/ymir work/effect/rune/buff_black.mse", true},
	{ CInstanceBase::EFFECT_RUNA_GREEN_EQUIP, "", "d:/ymir work/effect/rune/buff_green.mse", true},
	{ CInstanceBase::EFFECT_EMPIRE + 1, "Bip01", "d:/ymir work/effect/etc/empire/empire_A.mse", true},
	{ CInstanceBase::EFFECT_EMPIRE + 2, "Bip01", "d:/ymir work/effect/etc/empire/empire_B.mse", true},
	{ CInstanceBase::EFFECT_EMPIRE + 3, "Bip01", "d:/ymir work/effect/etc/empire/empire_C.mse", true},
	{ CInstanceBase::EFFECT_WEAPON + 1, "equip_right_hand", "d:/ymir work/pc/warrior/effect/geom_sword_loop.mse", true},
	{ CInstanceBase::EFFECT_WEAPON + 2, "equip_right_hand", "d:/ymir work/pc/warrior/effect/geom_spear_loop.mse", true},
	{ CInstanceBase::EFFECT_AFFECT + 0, "Bip01", "d:/ymir work/effect/gm.mse", true},
	{ CInstanceBase::EFFECT_AFFECT + 3, "Bip01", "d:/ymir work/effect/hit/blow_poison/poison_loop.mse", true},
	{ CInstanceBase::EFFECT_AFFECT + 4, "", "d:/ymir work/effect/affect/slow.mse", true},
	{ CInstanceBase::EFFECT_AFFECT + 5, "Bip01 Head", "d:/ymir work/effect/etc/stun/stun_loop.mse", true},
	{ CInstanceBase::EFFECT_AFFECT + 6, "", "d:/ymir work/effect/etc/ready/ready.mse", true},
	{ CInstanceBase::EFFECT_AFFECT + 16, "", "d:/ymir work/pc/warrior/effect/gyeokgongjang_loop.mse", true},
	{ CInstanceBase::EFFECT_AFFECT + 17, "", "d:/ymir work/pc/assassin/effect/gyeonggong_loop.mse", true},
	{ CInstanceBase::EFFECT_AFFECT + 19, "Bip01 R Finger2", "d:/ymir work/pc/sura/effect/gwigeom_loop.mse", true},
	{ CInstanceBase::EFFECT_AFFECT + 20, "", "d:/ymir work/pc/sura/effect/fear_loop.mse", true},
	{ CInstanceBase::EFFECT_AFFECT + 21, "", "d:/ymir work/pc/sura/effect/jumagap_loop.mse", true},
	{ CInstanceBase::EFFECT_AFFECT + 22, "", "d:/ymir work/pc/shaman/effect/3hosin_loop.mse", true},
	{ CInstanceBase::EFFECT_AFFECT + 23, "", "d:/ymir work/pc/shaman/effect/boho_loop.mse", true},
	{ CInstanceBase::EFFECT_AFFECT + 24, "", "d:/ymir work/pc/shaman/effect/10kwaesok_loop.mse", true},
	{ CInstanceBase::EFFECT_AFFECT + 25, "", "d:/ymir work/pc/sura/effect/heuksin_loop.mse", true},
	{ CInstanceBase::EFFECT_AFFECT + 26, "", "d:/ymir work/pc/sura/effect/muyeong_loop.mse", true},
	{ CInstanceBase::EFFECT_AFFECT + 28, "Bip01", "d:/ymir work/effect/hit/blow_flame/flame_loop.mse", true},
	{ CInstanceBase::EFFECT_AFFECT + 29, "Bip01 R Hand", "d:/ymir work/pc/shaman/effect/6gicheon_hand.mse", true},
	{ CInstanceBase::EFFECT_AFFECT + 30, "Bip01 L Hand", "d:/ymir work/pc/shaman/effect/jeungryeok_hand.mse", true},
	{ CInstanceBase::EFFECT_AFFECT + 32, "Bip01 Head", "d:/ymir work/pc/sura/effect/pabeop_loop.mse", true},
	{ CInstanceBase::EFFECT_AFFECT + 33, "", "d:/ymir work/pc/warrior/effect/gyeokgongjang_loop.mse", true},
	{ CInstanceBase::EFFECT_AFFECT + 35, "", "d:/ymir work/effect/etc/guild_war_flag/flag_red.mse", true},
	{ CInstanceBase::EFFECT_AFFECT + 36, "", "d:/ymir work/effect/etc/guild_war_flag/flag_blue.mse", true},
	{ CInstanceBase::EFFECT_AFFECT + 37, "", "d:/ymir work/effect/etc/guild_war_flag/flag_yellow.mse", true},
#ifdef ENABLE_ASLAN_BUFF_NPC_SYSTEM
	{ CInstanceBase::EFFECT_AFFECT + 42, "", "d:/ymir work/pc/shaman/effect/3hosin_loop.mse", true},
	{ CInstanceBase::EFFECT_AFFECT + 43, "Bip01 R Hand", "d:/ymir work/pc/shaman/effect/6gicheon_hand.mse", true},
	{ CInstanceBase::EFFECT_AFFECT + 44, "Bip01 L Hand", "d:/ymir work/pc/shaman/effect/jeungryeok_hand.mse", true},
#endif

	//{ CInstanceBase::EFFECT_AFFECT + 45, "Bip01", "d:/ymir work/effect/etc/beer/beer.mse", true},
	//{ CInstanceBase::EFFECT_AFFECT + 46, "Bip01", "d:/ymir work/effect/etc/summer2022/summer2022.mse", true},

	{ CInstanceBase::EFFECT_REFINED + 1, "PART_WEAPON", "D:/ymir work/pc/common/effect/sword/sword_7.mse", true},
	{ CInstanceBase::EFFECT_REFINED + 2, "PART_WEAPON", "D:/ymir work/pc/common/effect/sword/sword_8.mse", true},
	{ CInstanceBase::EFFECT_REFINED + 3, "PART_WEAPON", "D:/ymir work/pc/common/effect/sword/sword_9.mse", true},
	{ CInstanceBase::EFFECT_REFINED + 4, "PART_WEAPON_LEFT", "D:/ymir work/pc/common/effect/sword/sword_7_b.mse", true},
	{ CInstanceBase::EFFECT_REFINED + 5, "PART_WEAPON_LEFT", "D:/ymir work/pc/common/effect/sword/sword_8_b.mse", true},
	{ CInstanceBase::EFFECT_REFINED + 6, "PART_WEAPON_LEFT", "D:/ymir work/pc/common/effect/sword/sword_9_b.mse", true},
	{ CInstanceBase::EFFECT_REFINED + 7, "PART_WEAPON", "D:/ymir work/pc/common/effect/sword/sword_7_f.mse", true},
	{ CInstanceBase::EFFECT_REFINED + 8, "PART_WEAPON", "D:/ymir work/pc/common/effect/sword/sword_8_f.mse", true},
	{ CInstanceBase::EFFECT_REFINED + 9, "PART_WEAPON", "D:/ymir work/pc/common/effect/sword/sword_9_f.mse", true},
	{ CInstanceBase::EFFECT_REFINED + 10, "PART_WEAPON", "D:/ymir work/pc/common/effect/sword/sword_7_s.mse", true},
	{ CInstanceBase::EFFECT_REFINED + 11, "PART_WEAPON", "D:/ymir work/pc/common/effect/sword/sword_8_s.mse", true},
	{ CInstanceBase::EFFECT_REFINED + 12, "PART_WEAPON", "D:/ymir work/pc/common/effect/sword/sword_9_s.mse", true},
	{ CInstanceBase::EFFECT_REFINED + 13, "PART_WEAPON_LEFT", "D:/ymir work/pc/common/effect/sword/sword_7_s.mse", true},
	{ CInstanceBase::EFFECT_REFINED + 14, "PART_WEAPON_LEFT", "D:/ymir work/pc/common/effect/sword/sword_8_s.mse", true},
	{ CInstanceBase::EFFECT_REFINED + 15, "PART_WEAPON_LEFT", "D:/ymir work/pc/common/effect/sword/sword_9_s.mse", true},
	{ CInstanceBase::EFFECT_REFINED + 16, "Bip01", "D:/ymir work/pc/common/effect/armor/armor_7.mse", true},
	{ CInstanceBase::EFFECT_REFINED + 17, "Bip01", "D:/ymir work/pc/common/effect/armor/armor_8.mse", true},
	{ CInstanceBase::EFFECT_REFINED + 18, "Bip01", "D:/ymir work/pc/common/effect/armor/armor_9.mse", true},
	{ CInstanceBase::EFFECT_REFINED + 19, "Bip01", "D:/ymir work/pc/common/effect/armor/armor-4-2-1.mse", true},
	{ CInstanceBase::EFFECT_REFINED + 20, "Bip01", "D:/ymir work/pc/common/effect/armor/armor-4-2-2.mse", true},
	{ CInstanceBase::EFFECT_REFINED + 21, "Bip01", "D:/ymir work/pc/common/effect/armor/armor-5-1.mse", true}, // EFFECT_BODYARMOR_FIRE
	{ CInstanceBase::EFFECT_REFINED + 22, "Bip01", "D:/ymir work/pc/common/effect/armor/armor-5_jin.mse", true}, // EFFECT_BODYARMOR_FIRE2
	{ CInstanceBase::EFFECT_REFINED + 23, "Bip01", "D:/ymir work/pc/common/effect/armor/armor-5_jin.mse", true}, // EFFECT_ACCE
	//{ CInstanceBase::EFFECT_REFINED + 50, "Bip01", "D:/ymir work/item/wing/plechito/dark_cult_wing/dark_wing.mse", true}, // EFFECT_STOLA_GREEN
	{ CInstanceBase::EFFECT_DUST, "", "d:/ymir work/effect/etc/dust/dust.mse", true},
	{ CInstanceBase::EFFECT_HORSE_DUST, "", "d:/ymir work/effect/etc/dust/running_dust.mse", true},
	{ CInstanceBase::EFFECT_HIT, "", "d:/ymir work/effect/hit/blow_1/blow_1_low.mse", true},
	{ CInstanceBase::EFFECT_HPUP_RED, "", "d:/ymir work/effect/etc/recuperation/drugup_red.mse", true},
	{ CInstanceBase::EFFECT_SPUP_BLUE, "", "d:/ymir work/effect/etc/recuperation/drugup_blue.mse", true},
	{ CInstanceBase::EFFECT_SPEEDUP_GREEN, "", "d:/ymir work/effect/etc/recuperation/drugup_green.mse", true},
	{ CInstanceBase::EFFECT_DXUP_PURPLE, "", "d:/ymir work/effect/etc/recuperation/drugup_purple.mse", true},
	{ CInstanceBase::EFFECT_AUTO_HPUP, "", "d:/ymir work/effect/etc/recuperation/autodrugup_red.mse", true},
	{ CInstanceBase::EFFECT_AUTO_SPUP, "", "d:/ymir work/effect/etc/recuperation/autodrugup_blue.mse", true},
	{ CInstanceBase::EFFECT_RAMADAN_RING_EQUIP, "", "d:/ymir work/effect/etc/buff/buff_item1.mse", true},
	{ CInstanceBase::EFFECT_HALLOWEEN_CANDY_EQUIP, "", "d:/ymir work/effect/etc/buff/buff_item2.mse", true},
	{ CInstanceBase::EFFECT_HAPPINESS_RING_EQUIP, "", "d:/ymir work/effect/etc/buff/buff_item3.mse", true},
	{ CInstanceBase::EFFECT_LOVE_PENDANT_EQUIP, "", "d:/ymir work/effect/etc/buff/buff_item4.mse", true},
	{ CInstanceBase::EFFECT_FIRECRACKER, "", "d:/ymir work/effect/etc/firecracker/newyear_firecracker.mse", true},
	{ CInstanceBase::EFFECT_SPIN_TOP, "", "d:/ymir work/effect/etc/firecracker/paing_i.mse", true},
	{ CInstanceBase::EFFECT_SELECT, "", "d:/ymir work/effect/etc/click/click_select.mse", true},
	{ CInstanceBase::EFFECT_TARGET, "", "d:/ymir work/effect/etc/click/click_glow_select.mse", true},
	{ CInstanceBase::EFFECT_STUN, "Bip01 Head", "d:/ymir work/effect/etc/stun/stun.mse", true},
	{ CInstanceBase::EFFECT_CRITICAL, "Bip01 R Hand", "d:/ymir work/effect/hit/critical.mse", true},
	{ CInstanceBase::EFFECT_DAMAGE_TARGET, "", "d:/ymir work/effect/affect/damagevalue/target.mse", true},
	{ CInstanceBase::EFFECT_DAMAGE_NOT_TARGET, "", "d:/ymir work/effect/affect/damagevalue/nontarget.mse", true},
	{ CInstanceBase::EFFECT_DAMAGE_SELFDAMAGE, "", "d:/ymir work/effect/affect/damagevalue/damage.mse", true},
	{ CInstanceBase::EFFECT_DAMAGE_SELFDAMAGE2, "", "d:/ymir work/effect/affect/damagevalue/damage_1.mse", true},
	{ CInstanceBase::EFFECT_DAMAGE_POISON, "", "d:/ymir work/effect/affect/damagevalue/poison.mse", true},
	{ CInstanceBase::EFFECT_DAMAGE_MISS, "", "d:/ymir work/effect/affect/damagevalue/miss.mse", true},
	{ CInstanceBase::EFFECT_DAMAGE_TARGETMISS, "", "d:/ymir work/effect/affect/damagevalue/target_miss.mse", true},
	{ CInstanceBase::EFFECT_LEVELUP_ON_14_FOR_GERMANY, "", "season1/effect/paymessage_warning.mse", true},
	{ CInstanceBase::EFFECT_LEVELUP_UNDER_15_FOR_GERMANY, "", "season1/effect/paymessage_decide.mse", true},
	{ CInstanceBase::EFFECT_PERCENT_DAMAGE1, "", "d:/ymir work/effect/hit/percent_damage1.mse", true},
	{ CInstanceBase::EFFECT_PERCENT_DAMAGE2, "", "d:/ymir work/effect/hit/percent_damage2.mse", true},
	{ CInstanceBase::EFFECT_PERCENT_DAMAGE3, "", "d:/ymir work/effect/hit/percent_damage3.mse", true},
	{ CInstanceBase::EFFECT_QUEUE,   "", "d:/ymir work/effect/ridack_work/ridack_metineffect/ridack_metineffect.mse", true },
	{ CInstanceBase::EFFECT_QUEUE_2, "", "d:/ymir work/effect/ridack_work/ridack_metineffect/ridack_metineffect2.mse", true },
	{ CInstanceBase::EFFECT_QUEUE_3, "", "d:/ymir work/effect/ridack_work/ridack_metineffect/ridack_metineffect3.mse", true },
	{ CInstanceBase::EFFECT_QUEUE_4, "", "d:/ymir work/effect/ridack_work/ridack_metineffect/ridack_metineffect4.mse", true },
	{ CInstanceBase::EFFECT_BOSS, "", "d:/ymir work/effect/boss_effect.mse", true },
	{ CInstanceBase::EFFECT_FISH1, "", "d:/ymir work/effect/special/fish/1.mse", true },
	{ CInstanceBase::EFFECT_FISH2, "", "d:/ymir work/effect/special/fish/2.mse", true },
	{ CInstanceBase::EFFECT_FISH3, "", "d:/ymir work/effect/special/fish/3.mse", true },
	{ CInstanceBase::EFFECT_FISH4, "", "d:/ymir work/effect/special/fish/4.mse", true },
};

/*typedef std::pair<DWORD, std::string> TSoundFileName;
static const std::array<TSoundFileName, CPythonItem::USESOUND_NUM - 1> vecUseSoundFileName =
{ {
	{CPythonItem::USESOUND_DEFAULT, "sound/ui/drop.wav"},
	{CPythonItem::USESOUND_ACCESSORY, "sound/ui/equip_ring_amulet.wav"},
	{CPythonItem::USESOUND_ARMOR, "sound/ui/equip_metal_armor.wav"},
	{CPythonItem::USESOUND_BOW, "sound/ui/equip_bow.wav"},
	{CPythonItem::USESOUND_WEAPON, "sound/ui/equip_metal_weapon.wav"},
	{CPythonItem::USESOUND_POTION, "sound/ui/eat_potion.wav"},
	{CPythonItem::USESOUND_PORTAL, "sound/ui/potal_scroll.wav"},
} };

static const std::array<TSoundFileName, CPythonItem::DROPSOUND_NUM> vecDropSoundFileName =
{ {
		{CPythonItem::DROPSOUND_DEFAULT, "sound/ui/drop.wav"},
		{CPythonItem::DROPSOUND_ACCESSORY, "sound/ui/equip_ring_amulet.wav"},
		{CPythonItem::DROPSOUND_ARMOR, "sound/ui/equip_metal_armor.wav"},
		{CPythonItem::DROPSOUND_BOW, "sound/ui/equip_bow.wav"},
		{CPythonItem::DROPSOUND_WEAPON, "sound/ui/equip_metal_weapon.wav"},
} };*/


enum ComboType
{
	COMBO_TYPE_1,
	COMBO_TYPE_2,
	COMBO_TYPE_3,
};

enum ComboIndex
{
	COMBO_INDEX_1,
	COMBO_INDEX_2,
	COMBO_INDEX_3,
	COMBO_INDEX_4,
	COMBO_INDEX_5,
	COMBO_INDEX_6,
};
	
// static const std::map<uint8_t, std::map<uint8_t, std::vector<uint8_t>>> skillIndexes{
	// { NRaceData::JOB_WARRIOR, {
		// {1, {1, 2, 3, 4, 5, 0, 0, 0, 137, 0, 0, 0, 0, 0}},
		// {2, {16, 17, 18, 19, 20, 0, 0, 0, 137, 0, 0, 0, 0, 0}},
	// }},
	// { NRaceData::JOB_ASSASSIN, {
		// {1, {31, 32, 33, 34, 35, 0, 0, 0, 137, 0, 0, 0, 0, 0, 0}},
		// {2, {46, 47, 48, 49, 50, 0, 0, 0, 137, 0, 0, 0, 0, 0, 0}},
	// }},
	// { NRaceData::JOB_SURA, {
		// {1, {61, 62, 63, 64, 65, 66, 0, 0, 137, 0, 0, 0, 0, 0}},
		// {2, {76, 77, 78, 79, 80, 81, 0, 0, 137, 0, 0, 0, 0, 0}},
	// }},
	// { NRaceData::JOB_SHAMAN, {
		// {1, {91, 92, 93, 94, 95, 96, 0, 0, 137, 0, 0, 0, 0, 0}},
		// {2, {106, 107, 108, 109, 110, 111, 0, 0, 137, 0, 0, 0, 0, 0}},
	// }},
// #ifdef ENABLE_WOLFMAN_CHARACTER
	// { NRaceData::JOB_WOLFMAN, {
		// {1, {170, 171, 172, 173, 174, 175, 0, 0, 137, 0, 0, 0, 0, 0}},
		// {2, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}},
	// }},
// #endif
// };

static const std::map<uint8_t, std::map<uint8_t, std::vector<uint8_t>>> skillIndexes{
	{ NRaceData::JOB_WARRIOR, {
		{1, {1, 2, 3, 4, 5, 0, 0, 0, 137, 0, 138, 0, 139, 0}},
		{2, {16, 17, 18, 19, 20, 0, 0, 0, 137, 0, 138, 0, 139, 0}},
	}},
	{ NRaceData::JOB_ASSASSIN, {
		{1, {31, 32, 33, 34, 35, 0, 0, 0, 137, 0, 138, 0, 139, 0, 140}},
		{2, {46, 47, 48, 49, 50, 0, 0, 0, 137, 0, 138, 0, 139, 0, 140}},
	}},
	{ NRaceData::JOB_SURA, {
		{1, {61, 62, 63, 64, 65, 66, 0, 0, 137, 0, 138, 0, 139, 0}},
		{2, {76, 77, 78, 79, 80, 81, 0, 0, 137, 0, 138, 0, 139, 0}},
	}},
	{ NRaceData::JOB_SHAMAN, {
		{1, {91, 92, 93, 94, 95, 96, 0, 0, 137, 0, 138, 0, 139, 0}},
		{2, {106, 107, 108, 109, 110, 111, 0, 0, 137, 0, 138, 0, 139, 0}},
	}},
#ifdef ENABLE_WOLFMAN_CHARACTER
	{ NRaceData::JOB_WOLFMAN, {
		{1, {170, 171, 172, 173, 174, 175, 0, 0, 137, 0, 0, 0, 0, 0}},
		{2, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}},
	}},
#endif
};


constexpr std::array<BYTE, 18> supportSkillIndexes =
{
	{122, 123, 121, 124, 131, 247, 130, 180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190},
};


static const std::map<std::string, std::vector<BYTE>> guildSkills =
{
	{ "PASSIVE", { 151 }},
	{ "ACTIVE", { 152, 153, 154, 155, 156, 157 }},
};

// static const std::map<DWORD, float> raceHeight =
// {
	// {20246, 160.00f},
	// {20247, 160.00f},
	// {2493, 500.00f},
	// {3999, 500.00f},
	// {20030, 220.00f},
	// {20101, 220.00f},
	// {20102, 220.00f},
	// {20103, 220.00f},
	// {20104, 220.00f},
	// {20105, 220.00f},
	// {20106, 220.00f},
	// {20107, 220.00f},
	// {20108, 220.00f},
	// {20109, 220.00f},
	// {0, 180.00f},
	// {1, 180.00f},
	// {2, 190.00f},
	// {3, 180.00f},
	// {4, 180.00f},
	// {5, 180.00f},
	// {6, 180.00f},
	// {7, 180.00f},
// #ifdef ENABLE_WOLFMAN_CHARACTER
	// {8, 230.00f},
// #endif
//};
#endif

