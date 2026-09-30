#include "StdAfx.h"
#include "AbstractResources.h"
#include "PythonNetworkStream.h"
#include <boost/algorithm/string.hpp>
#include <Core/RaceConstans.hpp>
#include "PythonBindings.h"

AbstractResources::AbstractResources() = default;
AbstractResources::~AbstractResources() = default;

/* Loading Layers */
bool AbstractResources::LoadDefaultGameData()
{
	RegisterConstansData();
	RegisterAffects();
	RegisterEffects();
	RegisterRefinedEffects();
	RegisterFlyEffects();
	RegisterEmojis();
	CPythonBackground::Instance().__CreateProperty();
	return true;
}

bool AbstractResources::LoadRaceData()
{
	LoadActorMotions();
	LoadNonPlayerData();

	//Set Skills to Races
	CPythonNetworkStream& net = CPythonNetworkStream::Instance();
	RegisterSkills(net.GetMainActorRace(), net.GetMainActorSkillGroup(), net.GetMainActorEmpire());
	LoadRaceHeights();
	return true;
}

/* Register Constans Data */
const bool AbstractResources::RegisterConstansData()
{
	CInstanceBase::SetDustGap(250);
	CInstanceBase::SetHorseDustGap(500);
	CPythonPlayer::Instance().RegisterEffect(CPythonPlayer::EFFECT_PICK, "d:/ymir work/effect/etc/click/click.mse", true);

	/* Register Dungeon Names */
	for (const auto& DungeonNames : dungeonArray)
		CPythonBackground::Instance().RegisterDungeonMapName(DungeonNames);

	/* Load Interface Use Sounds */
	for (const auto& [useSoundIndex, useSoundFile] : useSoundFiles)
		CPythonItem::Instance().SetUseSoundFileName(useSoundIndex, useSoundFile);

	/* Load Game Drop Sounds */
	for (const auto& [dropSoundIndex, dropSoundFile] : dropSoundFiles)
		CPythonItem::Instance().SetDropSoundFileName(dropSoundIndex, dropSoundFile);

	/* Loading Intro Race Data */
	static std::vector<SRaceRegister> vecRaceData =
	{
		{ MAIN_RACE_WARRIOR_W,		"Globals/warrior_w.msm",			"d:/ymir work/pc2/warrior/intro/" },
		{ MAIN_RACE_WARRIOR_M,		"Globals/warrior_m.msm",			"d:/ymir work/pc/warrior/intro/" },
		{ MAIN_RACE_ASSASSIN_W,		"Globals/assassin_w.msm",			"d:/ymir work/pc/assassin/intro/" },
		{ MAIN_RACE_ASSASSIN_M,		"Globals/assassin_m.msm",			"d:/ymir work/pc2/assassin/intro/" },
		{ MAIN_RACE_SURA_W,			"Globals/sura_w.msm",				"d:/ymir work/pc2/sura/intro/" },
		{ MAIN_RACE_SURA_M,			"Globals/sura_m.msm",				"d:/ymir work/pc/sura/intro/" },
		{ MAIN_RACE_SHAMAN_W,		"Globals/shaman_w.msm",				"d:/ymir work/pc/shaman/intro/" },
		{ MAIN_RACE_SHAMAN_M,		"Globals/shaman_m.msm",				"d:/ymir work/pc2/shaman/intro/" },
	};

	std::vector<SMotionRegister> vecMotionData =
	{
		{ CRaceMotionData::NAME_INTRO_WAIT,			"wait.msa", 0 },
		{ CRaceMotionData::NAME_INTRO_SELECTED,		"selected.msa", 0 },
		{ CRaceMotionData::NAME_INTRO_NOT_SELECTED, "not_selected.msa", 0 },
	};

	CRaceManager& pkManager = CRaceManager::Instance();
	char szFileName[FILE_MAX_NUM];
	for (auto& race : vecRaceData)
	{
		pkManager.CreateRace(race.idRace);
		pkManager.SelectRace(race.idRace);

		CRaceData* pRaceData = pkManager.GetSelectedRaceDataPointer();
		if (!pRaceData)
			return false;

		if (!pRaceData->LoadRaceData(race.msm_file))
		{
			return false;
		}

		pRaceData->RegisterMotionMode(CRaceMotionData::MODE_GENERAL);
		for (const auto& motion : vecMotionData)
		{
			snprintf(szFileName, sizeof(szFileName), "%s%s", race.motion_path, motion.fileName);
			pRaceData->RegisterMotionData(CRaceMotionData::MODE_GENERAL, motion.idMotion, szFileName, motion.Percentage);
		}
	}

	return true;
}

/* Register Title Names and Colors */
void AbstractResources::RegisterTitleNamesAndColors()
{
	// TODO, exec: pybind import block
	/* Title Names */
	//auto localeInfo = py::module::import("localeInfo");
	//const static std::unordered_map<uint16_t, std::string> mapTitleName =
	//{
	//	{0, localeInfo.attr("PVP_LEVEL0").cast<std::string>()},
	//	{1, localeInfo.attr("PVP_LEVEL1").cast<std::string>()},
	//	{2, localeInfo.attr("PVP_LEVEL2").cast<std::string>()},
	//	{3, localeInfo.attr("PVP_LEVEL3").cast<std::string>()},
	//	{4, localeInfo.attr("PVP_LEVEL4").cast<std::string>()},
	//	{5, localeInfo.attr("PVP_LEVEL5").cast<std::string>()},
	//	{6, localeInfo.attr("PVP_LEVEL6").cast<std::string>()},
	//	{7, localeInfo.attr("PVP_LEVEL7").cast<std::string>()},
	//	{8, localeInfo.attr("PVP_LEVEL8").cast<std::string>()},
	//};
	//
	//for (const auto& [titleIndex, titleName] : mapTitleName)
	//	CInstanceBase::RegisterTitleName(titleIndex, titleName.c_str());
	
	/* Title Colors */
	for (const auto& [index, rgbTuple] : titleColorMap)
	{
		const auto& [r, g, b] = rgbTuple;
		CInstanceBase::RegisterTitleColor(index, r, g, b);
	}
	
	/* Name Colors */
	for (const auto& [index, rgbTuple] : nameColorMap)
	{
		const auto& [r, g, b] = rgbTuple;
		CInstanceBase::RegisterNameColor(index, r, g, b);
	}

	return;
}

/* Register Emojis and Chat Strings */
const bool AbstractResources::RegisterEmojis()
{
	CInstanceBase EmoticonRegister = CInstanceBase();
	EmoticonRegister.RegisterEffect(CInstanceBase::EFFECT_EMOTICON + 0, "", "d:/ymir work/effect/etc/emoticon/sweat.mse", true);
	CPythonNetworkStream::Instance().RegisterEmoticonString("(sweat)");

	EmoticonRegister.RegisterEffect(CInstanceBase::EFFECT_EMOTICON + 1, "", "d:/ymir work/effect/etc/emoticon/money.mse", true);
	CPythonNetworkStream::Instance().RegisterEmoticonString("(money)");

	EmoticonRegister.RegisterEffect(CInstanceBase::EFFECT_EMOTICON + 2, "", "d:/ymir work/effect/etc/emoticon/happy.mse", true);
	CPythonNetworkStream::Instance().RegisterEmoticonString("(happy)");

	EmoticonRegister.RegisterEffect(CInstanceBase::EFFECT_EMOTICON + 3, "", "d:/ymir work/effect/etc/emoticon/love_s.mse", true);
	CPythonNetworkStream::Instance().RegisterEmoticonString("(love_s)");

	EmoticonRegister.RegisterEffect(CInstanceBase::EFFECT_EMOTICON + 4, "", "d:/ymir work/effect/etc/emoticon/love_l.mse", true);
	CPythonNetworkStream::Instance().RegisterEmoticonString("(love_l)");

	EmoticonRegister.RegisterEffect(CInstanceBase::EFFECT_EMOTICON + 5, "", "d:/ymir work/effect/etc/emoticon/angry.mse", true);
	CPythonNetworkStream::Instance().RegisterEmoticonString("(angry)");

	EmoticonRegister.RegisterEffect(CInstanceBase::EFFECT_EMOTICON + 6, "", "d:/ymir work/effect/etc/emoticon/aha.mse", true);
	CPythonNetworkStream::Instance().RegisterEmoticonString("(aha)");

	EmoticonRegister.RegisterEffect(CInstanceBase::EFFECT_EMOTICON + 7, "", "d:/ymir work/effect/etc/emoticon/gloom.mse", true);
	CPythonNetworkStream::Instance().RegisterEmoticonString("(gloom)");

	EmoticonRegister.RegisterEffect(CInstanceBase::EFFECT_EMOTICON + 8, "", "d:/ymir work/effect/etc/emoticon/sorry.mse", true);
	CPythonNetworkStream::Instance().RegisterEmoticonString("(sorry)");

	EmoticonRegister.RegisterEffect(CInstanceBase::EFFECT_EMOTICON + 9, "", "d:/ymir work/effect/etc/emoticon/!_mix_back.mse", true);
	CPythonNetworkStream::Instance().RegisterEmoticonString("(!)");

	EmoticonRegister.RegisterEffect(CInstanceBase::EFFECT_EMOTICON + 10, "", "d:/ymir work/effect/etc/emoticon/question.mse", true);
	CPythonNetworkStream::Instance().RegisterEmoticonString("(?)");

	EmoticonRegister.RegisterEffect(CInstanceBase::EFFECT_EMOTICON + 11, "", "d:/ymir work/effect/etc/emoticon/fish.mse", true);
	CPythonNetworkStream::Instance().RegisterEmoticonString("(fish)");
	return true;
}

/* Register Game Affects */
const bool AbstractResources::RegisterAffects()
{
	CInstanceBase AffectRegistry = CInstanceBase();
	AffectRegistry.RegisterEffect(CInstanceBase::EFFECT_AFFECT + 0, "Bip01", "Globals/effect/gm.mse", true);
	AffectRegistry.RegisterEffect(CInstanceBase::EFFECT_AFFECT + 3, "Bip01", "d:/ymir work/effect/hit/blow_poison/poison_loop.mse", true);
	AffectRegistry.RegisterEffect(CInstanceBase::EFFECT_AFFECT + 4, "", "d:/ymir work/effect/affect/slow.mse", true);
	AffectRegistry.RegisterEffect(CInstanceBase::EFFECT_AFFECT + 5, "Bip01 Head", "d:/ymir work/effect/etc/stun/stun_loop.mse", true);
	AffectRegistry.RegisterEffect(CInstanceBase::EFFECT_AFFECT + 6, "", "d:/ymir work/effect/etc/ready/ready.mse", true);
	AffectRegistry.RegisterEffect(CInstanceBase::EFFECT_AFFECT + 16, "", "d:/ymir work/pc/warrior/effect/gyeokgongjang_loop.mse", true);
	AffectRegistry.RegisterEffect(CInstanceBase::EFFECT_AFFECT + 17, "", "d:/ymir work/pc/assassin/effect/gyeonggong_loop.mse", true);
	AffectRegistry.RegisterEffect(CInstanceBase::EFFECT_AFFECT + 19, "Bip01 R Finger2", "d:/ymir work/pc/sura/effect/gwigeom_loop.mse", true);
	AffectRegistry.RegisterEffect(CInstanceBase::EFFECT_AFFECT + 20, "", "d:/ymir work/pc/sura/effect/fear_loop.mse", true);
	AffectRegistry.RegisterEffect(CInstanceBase::EFFECT_AFFECT + 21, "", "d:/ymir work/pc/sura/effect/jumagap_loop.mse", true);
	AffectRegistry.RegisterEffect(CInstanceBase::EFFECT_AFFECT + 22, "", "d:/ymir work/pc/shaman/effect/3hosin_loop.mse", true);
	AffectRegistry.RegisterEffect(CInstanceBase::EFFECT_AFFECT + 23, "", "d:/ymir work/pc/shaman/effect/boho_loop.mse", true);
	AffectRegistry.RegisterEffect(CInstanceBase::EFFECT_AFFECT + 24, "", "d:/ymir work/pc/shaman/effect/10kwaesok_loop.mse", true);
	AffectRegistry.RegisterEffect(CInstanceBase::EFFECT_AFFECT + 25, "", "d:/ymir work/pc/sura/effect/heuksin_loop.mse", true);
	AffectRegistry.RegisterEffect(CInstanceBase::EFFECT_AFFECT + 26, "", "d:/ymir work/pc/sura/effect/muyeong_loop.mse", true);
	AffectRegistry.RegisterEffect(CInstanceBase::EFFECT_AFFECT + 28, "Bip01", "d:/ymir work/effect/hit/blow_flame/flame_loop.mse", true);
	AffectRegistry.RegisterEffect(CInstanceBase::EFFECT_AFFECT + 29, "Bip01 R Hand", "d:/ymir work/pc/shaman/effect/6gicheon_hand.mse", true);
	AffectRegistry.RegisterEffect(CInstanceBase::EFFECT_AFFECT + 30, "Bip01 L Hand", "d:/ymir work/pc/shaman/effect/jeungryeok_hand.mse", true);
	AffectRegistry.RegisterEffect(CInstanceBase::EFFECT_AFFECT + 32, "Bip01 Head", "d:/ymir work/pc/sura/effect/pabeop_loop.mse", true);
	AffectRegistry.RegisterEffect(CInstanceBase::EFFECT_AFFECT + 33, "", "d:/ymir work/pc/warrior/effect/gyeokgongjang_loop.mse", true);
	AffectRegistry.RegisterEffect(CInstanceBase::EFFECT_AFFECT + 35, "", "d:/ymir work/effect/etc/guild_war_flag/flag_red.mse", true);
	AffectRegistry.RegisterEffect(CInstanceBase::EFFECT_AFFECT + 36, "", "d:/ymir work/effect/etc/guild_war_flag/flag_blue.mse", true);
	AffectRegistry.RegisterEffect(CInstanceBase::EFFECT_AFFECT + 37, "", "d:/ymir work/effect/etc/guild_war_flag/flag_yellow.mse", true);
	AffectRegistry.RegisterEffect(CInstanceBase::EFFECT_AFFECT + 45, "", "d:/ymir work/pc/sura/effect/muyeong_loop.mse", true);
	return true;
}

/* Register Game Effects */
const bool AbstractResources::RegisterEffects()
{
	CInstanceBase EffectRegistry = CInstanceBase();
	EffectRegistry.RegisterEffect(CInstanceBase::EFFECT_DUST, "", "d:/ymir work/effect/etc/dust/dust.mse", true);
	EffectRegistry.RegisterEffect(CInstanceBase::EFFECT_HORSE_DUST, "", "d:/ymir work/effect/etc/dust/running_dust.mse", true);
	EffectRegistry.RegisterEffect(CInstanceBase::EFFECT_HIT, "", "d:/ymir work/effect/hit/blow_1/blow_1_low.mse", true);
	EffectRegistry.RegisterEffect(CInstanceBase::EFFECT_HPUP_RED, "", "d:/ymir work/effect/etc/recuperation/drugup_red.mse", true);
	EffectRegistry.RegisterEffect(CInstanceBase::EFFECT_SPUP_BLUE, "", "d:/ymir work/effect/etc/recuperation/drugup_blue.mse", true);
	EffectRegistry.RegisterEffect(CInstanceBase::EFFECT_SPEEDUP_GREEN, "", "d:/ymir work/effect/etc/recuperation/drugup_green.mse", true);
	EffectRegistry.RegisterEffect(CInstanceBase::EFFECT_DXUP_PURPLE, "", "d:/ymir work/effect/etc/recuperation/drugup_purple.mse", true);
	EffectRegistry.RegisterEffect(CInstanceBase::EFFECT_AUTO_HPUP, "", "d:/ymir work/effect/etc/recuperation/autodrugup_red.mse", true);
	EffectRegistry.RegisterEffect(CInstanceBase::EFFECT_AUTO_SPUP, "", "d:/ymir work/effect/etc/recuperation/autodrugup_blue.mse", true);
	EffectRegistry.RegisterEffect(CInstanceBase::EFFECT_RAMADAN_RING_EQUIP, "", "d:/ymir work/effect/etc/buff/buff_item1.mse", true);
	EffectRegistry.RegisterEffect(CInstanceBase::EFFECT_HALLOWEEN_CANDY_EQUIP, "", "d:/ymir work/effect/etc/buff/buff_item2.mse", true);
	EffectRegistry.RegisterEffect(CInstanceBase::EFFECT_HAPPINESS_RING_EQUIP, "", "d:/ymir work/effect/etc/buff/buff_item3.mse", true);
	EffectRegistry.RegisterEffect(CInstanceBase::EFFECT_LOVE_PENDANT_EQUIP, "", "d:/ymir work/effect/etc/buff/buff_item4.mse", true);
	//EffectRegistry.RegisterEffect(CInstanceBase::EFFECT_ACCE_SUCCEDED, "", "d:/ymir work/effect/etc/buff/buff_item6.mse", true);
	//EffectRegistry.RegisterEffect(CInstanceBase::EFFECT_ACCE_EQUIP, "", "d:/ymir work/effect/etc/buff/buff_item7.mse", true);
	EffectRegistry.RegisterEffect(CInstanceBase::EFFECT_PENETRATE, "Bip01", "d:/ymir work/effect/hit/gwantong.mse", true);
	EffectRegistry.RegisterEffect(CInstanceBase::EFFECT_FIRECRACKER, "", "d:/ymir work/effect/etc/firecracker/newyear_firecracker.mse", true);
	EffectRegistry.RegisterEffect(CInstanceBase::EFFECT_SPIN_TOP, "", "d:/ymir work/effect/etc/firecracker/paing_i.mse", true);
	EffectRegistry.RegisterEffect(CInstanceBase::EFFECT_SELECT, "", "d:/ymir work/effect/etc/click/click_select.mse", true);
	EffectRegistry.RegisterEffect(CInstanceBase::EFFECT_TARGET, "", "d:/ymir work/effect/etc/click/click_glow_select.mse", true);
	EffectRegistry.RegisterEffect(CInstanceBase::EFFECT_STUN, "Bip01 Head", "d:/ymir work/effect/etc/stun/stun.mse", true);
	EffectRegistry.RegisterEffect(CInstanceBase::EFFECT_CRITICAL, "Bip01 R Hand", "d:/ymir work/effect/hit/critical.mse", true);
	EffectRegistry.RegisterEffect(CInstanceBase::EFFECT_DAMAGE_TARGET, "", "d:/ymir work/effect/affect/damagevalue/target.mse", true);
	EffectRegistry.RegisterEffect(CInstanceBase::EFFECT_DAMAGE_NOT_TARGET, "", "d:/ymir work/effect/affect/damagevalue/nontarget.mse", true);
	EffectRegistry.RegisterEffect(CInstanceBase::EFFECT_DAMAGE_SELFDAMAGE, "", "d:/ymir work/effect/affect/damagevalue/damage.mse", true);
	EffectRegistry.RegisterEffect(CInstanceBase::EFFECT_DAMAGE_SELFDAMAGE2, "", "d:/ymir work/effect/affect/damagevalue/damage_1.mse", true);
	EffectRegistry.RegisterEffect(CInstanceBase::EFFECT_DAMAGE_POISON, "", "d:/ymir work/effect/affect/damagevalue/poison.mse", true);
	EffectRegistry.RegisterEffect(CInstanceBase::EFFECT_DAMAGE_MISS, "", "d:/ymir work/effect/affect/damagevalue/miss.mse", true);
	EffectRegistry.RegisterEffect(CInstanceBase::EFFECT_DAMAGE_TARGETMISS, "", "d:/ymir work/effect/affect/damagevalue/target_miss.mse", true);
	EffectRegistry.RegisterEffect(CInstanceBase::EFFECT_DAMAGE_CRITICAL, "", "d:/ymir work/effect/affect/damagevalue/critical.mse", true);
	EffectRegistry.RegisterEffect(CInstanceBase::EFFECT_PERCENT_DAMAGE1, "", "d:/ymir work/effect/hit/percent_damage1.mse", true);
	EffectRegistry.RegisterEffect(CInstanceBase::EFFECT_PERCENT_DAMAGE2, "", "d:/ymir work/effect/hit/percent_damage2.mse", true);
	EffectRegistry.RegisterEffect(CInstanceBase::EFFECT_PERCENT_DAMAGE3, "", "d:/ymir work/effect/hit/percent_damage3.mse", true);
	EffectRegistry.RegisterEffect(CInstanceBase::EFFECT_SPAWN_APPEAR, "Bip01", "d:/ymir work/effect/etc/appear_die/monster_appear.mse", true);
	EffectRegistry.RegisterEffect(CInstanceBase::EFFECT_SPAWN_DISAPPEAR, "Bip01", "d:/ymir work/effect/etc/appear_die/monster_die.mse", true);
	EffectRegistry.RegisterEffect(CInstanceBase::EFFECT_FLAME_ATTACK, "equip_right_hand", "d:/ymir work/effect/hit/blow_flame/flame_3_weapon.mse", true);
	EffectRegistry.RegisterEffect(CInstanceBase::EFFECT_FLAME_HIT, "", "d:/ymir work/effect/hit/blow_flame/flame_3_blow.mse", true);
	EffectRegistry.RegisterEffect(CInstanceBase::EFFECT_FLAME_ATTACH, "", "d:/ymir work/effect/hit/blow_flame/flame_3_body.mse", true);
	EffectRegistry.RegisterEffect(CInstanceBase::EFFECT_ELECTRIC_ATTACK, "equip_right", "d:/ymir work/effect/hit/blow_electric/light_1_weapon.mse", true);
	EffectRegistry.RegisterEffect(CInstanceBase::EFFECT_ELECTRIC_HIT, "", "d:/ymir work/effect/hit/blow_electric/light_1_blow.mse", true);
	EffectRegistry.RegisterEffect(CInstanceBase::EFFECT_ELECTRIC_ATTACH, "", "d:/ymir work/effect/hit/blow_electric/light_1_body.mse", true);
	EffectRegistry.RegisterEffect(CInstanceBase::EFFECT_LEVELUP, "", "d:/ymir work/effect/etc/levelup_1/level_up.mse", true);
	EffectRegistry.RegisterEffect(CInstanceBase::EFFECT_SKILLUP, "", "d:/ymir work/effect/etc/skillup/skillup_1.mse", true);
	EffectRegistry.RegisterEffect(CInstanceBase::EFFECT_EMPIRE + 1, "Bip01", "d:/ymir work/effect/etc/empire/empire_A.mse", true);
	EffectRegistry.RegisterEffect(CInstanceBase::EFFECT_EMPIRE + 2, "Bip01", "d:/ymir work/effect/etc/empire/empire_B.mse", true);
	EffectRegistry.RegisterEffect(CInstanceBase::EFFECT_EMPIRE + 3, "Bip01", "d:/ymir work/effect/etc/empire/empire_C.mse", true);
	EffectRegistry.RegisterEffect(CInstanceBase::EFFECT_WEAPON + 1, "equip_right_hand", "d:/ymir work/pc/warrior/effect/geom_sword_loop.mse", true);
	EffectRegistry.RegisterEffect(CInstanceBase::EFFECT_WEAPON + 2, "equip_right_hand", "d:/ymir work/pc/warrior/effect/geom_spear_loop.mse", true);
	return true;
}

/* Register Refined Game Effects */
const bool AbstractResources::RegisterRefinedEffects()
{
	CInstanceBase RefinedRegistry = CInstanceBase();
	RefinedRegistry.RegisterEffect(CInstanceBase::EFFECT_REFINED + 1, "PART_WEAPON", "D:/ymir work/pc/common/effect/sword/sword_7.mse", true);
	RefinedRegistry.RegisterEffect(CInstanceBase::EFFECT_REFINED + 2, "PART_WEAPON", "D:/ymir work/pc/common/effect/sword/sword_8.mse", true);
	RefinedRegistry.RegisterEffect(CInstanceBase::EFFECT_REFINED + 3, "PART_WEAPON", "D:/ymir work/pc/common/effect/sword/sword_9.mse", true);
	RefinedRegistry.RegisterEffect(CInstanceBase::EFFECT_REFINED + 4, "PART_WEAPON_LEFT", "D:/ymir work/pc/common/effect/sword/sword_7_b.mse", true);
	RefinedRegistry.RegisterEffect(CInstanceBase::EFFECT_REFINED + 5, "PART_WEAPON_LEFT", "D:/ymir work/pc/common/effect/sword/sword_8_b.mse", true);
	RefinedRegistry.RegisterEffect(CInstanceBase::EFFECT_REFINED + 6, "PART_WEAPON_LEFT", "D:/ymir work/pc/common/effect/sword/sword_9_b.mse", true);
	RefinedRegistry.RegisterEffect(CInstanceBase::EFFECT_REFINED + 7, "PART_WEAPON", "D:/ymir work/pc/common/effect/sword/sword_7_f.mse", true);
	RefinedRegistry.RegisterEffect(CInstanceBase::EFFECT_REFINED + 8, "PART_WEAPON", "D:/ymir work/pc/common/effect/sword/sword_8_f.mse", true);
	RefinedRegistry.RegisterEffect(CInstanceBase::EFFECT_REFINED + 9, "PART_WEAPON", "D:/ymir work/pc/common/effect/sword/sword_9_f.mse", true);
	RefinedRegistry.RegisterEffect(CInstanceBase::EFFECT_REFINED + 10, "PART_WEAPON", "D:/ymir work/pc/common/effect/sword/sword_7_s.mse", true);
	RefinedRegistry.RegisterEffect(CInstanceBase::EFFECT_REFINED + 11, "PART_WEAPON", "D:/ymir work/pc/common/effect/sword/sword_8_s.mse", true);
	RefinedRegistry.RegisterEffect(CInstanceBase::EFFECT_REFINED + 12, "PART_WEAPON", "D:/ymir work/pc/common/effect/sword/sword_9_s.mse", true);
	RefinedRegistry.RegisterEffect(CInstanceBase::EFFECT_REFINED + 13, "PART_WEAPON_LEFT", "D:/ymir work/pc/common/effect/sword/sword_7_s.mse", true);
	RefinedRegistry.RegisterEffect(CInstanceBase::EFFECT_REFINED + 14, "PART_WEAPON_LEFT", "D:/ymir work/pc/common/effect/sword/sword_8_s.mse", true);
	RefinedRegistry.RegisterEffect(CInstanceBase::EFFECT_REFINED + 15, "PART_WEAPON_LEFT", "D:/ymir work/pc/common/effect/sword/sword_9_s.mse", true);
	RefinedRegistry.RegisterEffect(CInstanceBase::EFFECT_REFINED + 16, "Bip01", "D:/ymir work/pc/common/effect/armor/armor_7.mse", true);
	RefinedRegistry.RegisterEffect(CInstanceBase::EFFECT_REFINED + 17, "Bip01", "D:/ymir work/pc/common/effect/armor/armor_8.mse", true);
	RefinedRegistry.RegisterEffect(CInstanceBase::EFFECT_REFINED + 18, "Bip01", "D:/ymir work/pc/common/effect/armor/armor_9.mse", true);
	//RefinedRegistry.RegisterEffect(CInstanceBase::EFFECT_REFINED + 19,			"Bip01",			"d:/ymir work/pc/common/effect/armor/acc_01.mse", true);
	return true;
}

/* Register Fly Effects */
const bool AbstractResources::RegisterFlyEffects()
{
	CFlyingManager::Instance().RegisterIndexedFlyData(FLY_EXP, INDEX_FLY_TYPE_NORMAL, "d:/ymir work/effect/etc/gathering/ga_piece_yellow_small2.msf");
	CFlyingManager::Instance().RegisterIndexedFlyData(FLY_HP_MEDIUM, INDEX_FLY_TYPE_NORMAL, "d:/ymir work/effect/etc/gathering/ga_piece_red_small.msf");
	CFlyingManager::Instance().RegisterIndexedFlyData(FLY_HP_BIG, INDEX_FLY_TYPE_NORMAL, "d:/ymir work/effect/etc/gathering/ga_piece_red_big.msf");
	CFlyingManager::Instance().RegisterIndexedFlyData(FLY_SP_SMALL, INDEX_FLY_TYPE_NORMAL, "d:/ymir work/effect/etc/gathering/ga_piece_blue_warrior_small.msf");
	CFlyingManager::Instance().RegisterIndexedFlyData(FLY_SP_MEDIUM, INDEX_FLY_TYPE_NORMAL, "d:/ymir work/effect/etc/gathering/ga_piece_blue_small.msf");
	CFlyingManager::Instance().RegisterIndexedFlyData(FLY_SP_BIG, INDEX_FLY_TYPE_NORMAL, "d:/ymir work/effect/etc/gathering/ga_piece_blue_big.msf");
	CFlyingManager::Instance().RegisterIndexedFlyData(FLY_FIREWORK1, INDEX_FLY_TYPE_FIRE_CRACKER, "d:/ymir work/effect/etc/firecracker/firecracker_1.msf");
	CFlyingManager::Instance().RegisterIndexedFlyData(FLY_FIREWORK2, INDEX_FLY_TYPE_FIRE_CRACKER, "d:/ymir work/effect/etc/firecracker/firecracker_2.msf");
	CFlyingManager::Instance().RegisterIndexedFlyData(FLY_FIREWORK3, INDEX_FLY_TYPE_FIRE_CRACKER, "d:/ymir work/effect/etc/firecracker/firecracker_3.msf");
	CFlyingManager::Instance().RegisterIndexedFlyData(FLY_FIREWORK4, INDEX_FLY_TYPE_FIRE_CRACKER, "d:/ymir work/effect/etc/firecracker/firecracker_4.msf");
	CFlyingManager::Instance().RegisterIndexedFlyData(FLY_FIREWORK5, INDEX_FLY_TYPE_FIRE_CRACKER, "d:/ymir work/effect/etc/firecracker/firecracker_5.msf");
	CFlyingManager::Instance().RegisterIndexedFlyData(FLY_FIREWORK6, INDEX_FLY_TYPE_FIRE_CRACKER, "d:/ymir work/effect/etc/firecracker/firecracker_6.msf");
	CFlyingManager::Instance().RegisterIndexedFlyData(FLY_FIREWORK_XMAS, INDEX_FLY_TYPE_FIRE_CRACKER, "d:/ymir work/effect/etc/firecracker/firecracker_xmas.msf");
	CFlyingManager::Instance().RegisterIndexedFlyData(FLY_CHAIN_LIGHTNING, INDEX_FLY_TYPE_NORMAL, "d:/ymir work/pc/shaman/effect/pokroe.msf");
	CFlyingManager::Instance().RegisterIndexedFlyData(FLY_HP_SMALL, INDEX_FLY_TYPE_NORMAL, "d:/ymir work/effect/etc/gathering/ga_piece_red_smallest.msf");
	CFlyingManager::Instance().RegisterIndexedFlyData(FLY_SKILL_MUYEONG, INDEX_FLY_TYPE_AUTO_FIRE, "d:/ymir work/pc/sura/effect/muyeong_fly.msf");
	return true;
}

/* Register Actor Skill Data */
const bool AbstractResources::RegisterSkills(uint32_t race, uint8_t skill_group, uint8_t empire)
{
	const uint32_t playerRace = race;
	const uint32_t playerSkillGroup = skill_group;
	const uint8_t playerJob = RaceToJob(playerRace);
	const uint32_t playerEmpire = empire;

	// Player Skills
	if (generalSkillIndexes.find(playerJob) != generalSkillIndexes.end() && generalSkillIndexes.at(playerJob).find(skill_group) != generalSkillIndexes.at(playerJob).end())
	{
		std::vector<uint8_t> playerSkills = generalSkillIndexes.at(playerJob).at(playerSkillGroup);

		for (size_t slot = 0; slot < playerSkills.size(); ++slot)
		{
			CPythonPlayer::Instance().SetSkill(slot + 1, playerSkills[slot]);
		}
	}

	// Support Skills
	const auto supportSkills = supportSkillIndexes;
	for (size_t slot = 0; slot < supportSkills.size(); ++slot)
	{
		CPythonPlayer::Instance().SetSkill(slot + 100 + 1, supportSkills[slot]);
	}

	//Language Skills
	if (playerEmpire)
	{
		for (size_t i = 0; i < 3; ++i)
		{
			if ((i + 1) != playerEmpire)
			{
				CPythonPlayer::Instance().SetSkill(109 + i, c_iSkillIndex_Language1 + i);
			}
		}
	}

	auto& passives2 = passives_.at("PASSIVE");
	for (size_t i =0; i < passives2.size(); i++)
		CPythonPlayer::Instance().SetSkill(180 + i, passives2[i]);

	// Passive Guild Skills
	auto& passiveSkills = guildSkillsIndexes.at("PASSIVE");
	for (size_t j = 0; j < passiveSkills.size(); ++j)
		CPythonPlayer::Instance().SetSkill(200 + j, passiveSkills[j]);

	// Active Guild Skills
	auto& activeSkills = guildSkillsIndexes.at("ACTIVE");
	for (size_t k = 0; k < activeSkills.size(); ++k)
		CPythonPlayer::Instance().SetSkill(210 + k, activeSkills[k]);

	return true;
}

/* Load Guild Building List */
bool AbstractResources::RegisterGuildBuildingList()
{
	CVirtualFile guild_buildinglist;

	if (!CallVFS().OpenFile(GUILD_BUILDING_LIST_FILENAME, guild_buildinglist))
	{
		TraceError("Register GuildBuilding List: %s - File not found..", GUILD_BUILDING_LIST_FILENAME);
		return false;
	}

	CMemoryTextFileLoader textFileLoader;
	textFileLoader.Bind(guild_buildinglist.GetSize(), guild_buildinglist.GetData());

	PyObject* pyGuildData = PyList_New(0);

	CTokenVector TokenVector;
	for (size_t i = 0; i < textFileLoader.GetLineCount(); ++i)
	{
		if (!textFileLoader.SplitLineByTab(i, &TokenVector))
			continue;

		std::string& strVnum = TokenVector[GUIILD_BUILDING_TOKEN_VNUM];

		if (strVnum.find_first_not_of("0123456789") != std::string::npos)
			continue;

		if (TokenVector.size() < GUIILD_BUILDING_LIMIT_TOKEN_COUNT)
		{
			TraceError("RegisterGuildBuildingList(%s) - StrangeLine in %s TokenVector size too long: %s", GUILD_BUILDING_LIST_FILENAME, i, TokenVector.size());
			continue;
		}

		const uint32_t& id = atoi(strVnum.c_str());
		const std::string_view type = TokenVector[GUIILD_BUILDING_TOKEN_TYPE];
		const char* name = TokenVector[GUIILD_BUILDING_TOKEN_NAME].c_str();
		const char* localName = TokenVector[GUIILD_BUILDING_TOKEN_LOCAL_NAME].c_str();
		const uint32_t& xRotLimit = atoi(TokenVector[GUIILD_BUILDING_TOKEN_X_ROT_LIMIT].c_str());
		const uint32_t& yRotLimit = atoi(TokenVector[GUIILD_BUILDING_TOKEN_Y_ROT_LIMIT].c_str());
		const uint32_t& zRotLimit = atoi(TokenVector[GUIILD_BUILDING_TOKEN_Z_ROT_LIMIT].c_str());
		const uint32_t& price = atoi(TokenVector[GUIILD_BUILDING_TOKEN_PRICE].c_str());
		const std::string_view material = TokenVector[GUIILD_BUILDING_TOKEN_MATERIAL];
		const uint32_t& enableType = atoi(TokenVector[GUIILD_BUILDING_TOKEN_ENABLE_FLAG].c_str());

		if (enableType == 0)
			continue;

		// Register material
		const std::string& folderName = m_buildingTypeToFolder[type];

		CRaceManager::Instance().RegisterRaceSrcName(name, folderName.c_str());
		CRaceManager::Instance().RegisterRaceName(id, name);

		// Create building material List
		std::vector<std::string> materialVector;
		boost::split(materialVector, material, boost::is_any_of("/"));

		PyObject* pyMaterialList = PyList_New(3);

		for (size_t j = 0; j < GUILD_MATERIAL_NUM; ++j)
		{
			PyList_SetItem(pyMaterialList, j, Py_BuildValue("s", "0"));
		}

		for (const std::string& itemPair : materialVector)
		{
			std::vector<std::string> itemVector;
			boost::split(itemVector, itemPair, boost::is_any_of(","));

			if (itemVector.size() != 2)
				continue;

			const uint32_t& vnum = atoi(itemVector[0].c_str());
			const std::string& count = itemVector[1];

			const uint8_t index = getGuildMaterialIndex(vnum);

			if (index == -1)
			{
				TraceError("Strange guild material: %s", vnum);
				continue;
			}

			PyList_SetItem(pyMaterialList, index, Py_BuildValue("s", count.c_str()));
		}

		// Create guild data dict and append to list
		PyObject* pyGuildDataItem = PyDict_New();
		PyDict_SetItem(pyGuildDataItem, Py_BuildValue("s", "VNUM"), Py_BuildValue("i", id));
		PyDict_SetItem(pyGuildDataItem, Py_BuildValue("s", "TYPE"), Py_BuildValue("s", type.data()));
		PyDict_SetItem(pyGuildDataItem, Py_BuildValue("s", "NAME"), Py_BuildValue("s", name));
		PyDict_SetItem(pyGuildDataItem, Py_BuildValue("s", "LOCAL_NAME"), Py_BuildValue("s", localName));
		PyDict_SetItem(pyGuildDataItem, Py_BuildValue("s", "X_ROT_LIMIT"), Py_BuildValue("i", xRotLimit));
		PyDict_SetItem(pyGuildDataItem, Py_BuildValue("s", "Y_ROT_LIMIT"), Py_BuildValue("i", yRotLimit));
		PyDict_SetItem(pyGuildDataItem, Py_BuildValue("s", "Z_ROT_LIMIT"), Py_BuildValue("i", zRotLimit));
		PyDict_SetItem(pyGuildDataItem, Py_BuildValue("s", "PRICE"), Py_BuildValue("i", price));
		PyDict_SetItem(pyGuildDataItem, Py_BuildValue("s", "MATERIAL"), Py_BuildValue("O", pyMaterialList));
		PyDict_SetItem(pyGuildDataItem, Py_BuildValue("s", "SHOW"), Py_BuildValue("i", enableType != 2));
		PyList_Append(pyGuildData, pyGuildDataItem);
	}

	CPythonNetworkStream::Instance().SendDataToPython(pyGuildData, "BINARY_SetGuildBuildingList");
	return true;
}

/* Load RaceHeights */
const bool AbstractResources::LoadRaceHeights()
{
	CVirtualFile race_height;
	if (!CallVFS().OpenFile(RACE_HEIGHT_FILENAME, race_height))
	{
		TraceError("Register Race Heights: %s - File not found..", RACE_HEIGHT_FILENAME);
		return false;
	}

	CMemoryTextFileLoader fileLoader;
	fileLoader.Bind(race_height.GetSize(), race_height.GetData());

	CTokenVector kTokenVector;
	for (uint32_t i = 0; i < fileLoader.GetLineCount(); ++i)
	{
		if (!fileLoader.SplitLineByTab(i, &kTokenVector))
			continue;

		for (auto& token : kTokenVector)
			token.erase(std::remove_if(token.begin(), token.end(), isspace), token.end());

		while (kTokenVector.size() < 3)
			kTokenVector.push_back("");

		const uint32_t dwRaceVnum = std::stoul(kTokenVector[0]);
		const float height_data = std::stof(kTokenVector.at(1));

		if (dwRaceVnum)
		{
			//CRaceManager::Instance().SetRaceHeight(dwRaceVnum, height_data);
		}
		else
		{
			TraceError("Register Race Heights: %s Strage instruction at ID: %s, Line: #%s", RACE_HEIGHT_FILENAME, dwRaceVnum, i);
		}
	}
	return true;
}

/* Load NonPlayer List */
const bool AbstractResources::LoadNonPlayerData()
{
	using namespace boost::algorithm;

	CVirtualFile npc_list;
	if (!CallVFS().OpenFile(NPC_LIST_FILENAME, npc_list))
	{
		TraceError("Load Non Player Data: %s - File not found..", NPC_LIST_FILENAME);
		return false;
	}

	CMemoryTextFileLoader fileLoader;
	fileLoader.Bind(npc_list.GetSize(), npc_list.GetData());
	CTokenVector kTokenVector;

	for (uint32_t i = 0; i < fileLoader.GetLineCount(); ++i)
	{
		if (!fileLoader.SplitLineByTab(i, &kTokenVector))
			continue;

		const uint32_t vnum = std::stoul(kTokenVector[0]);
		std::string& name = kTokenVector[1];
		trim(name);

		if (vnum)
		{
			CRaceManager::Instance().RegisterRaceName(vnum, name.c_str());
		}
		else
		{
			std::string& src = kTokenVector[2];
			trim(src);
			CRaceManager::Instance().RegisterRaceSrcName(name.c_str(), src.c_str());
		}
	}
	return true;
}

/* Start of Loading Actors */
const bool AbstractResources::LoadActorMotions()
{
	if (!WarriorMotionLoader(MAIN_RACE_WARRIOR_M, "d:/ymir work/pc/warrior/"))
	{
		TraceError("Failed to Load Warrior Race's Motions : MAIN_RACE_WARRIOR_M");
		return false;
	}

	if (!WarriorMotionLoader(MAIN_RACE_WARRIOR_W, "d:/ymir work/pc2/warrior/"))
	{
		TraceError("Failed to Load Warrior Race's Motions : MAIN_RACE_WARRIOR_W");
		return false;
	}

	if (!AssassinMotionLoader(MAIN_RACE_ASSASSIN_W, "d:/ymir work/pc/assassin/"))
	{
		TraceError("Failed to Load Assassin Race's Motions : MAIN_RACE_ASSASSIN_W");
		return false;
	}

	if (!AssassinMotionLoader(MAIN_RACE_ASSASSIN_M, "d:/ymir work/pc2/assassin/"))
	{
		TraceError("Failed to Load Assassin Race's Motions : MAIN_RACE_ASSASSIN_M");
		return false;
	}

	if (!SuraMotionLoader(MAIN_RACE_SURA_M, "d:/ymir work/pc/sura/"))
	{
		TraceError("Failed to Load Sura Race's Motions : MAIN_RACE_SURA_M");
		return false;
	}

	if (!SuraMotionLoader(MAIN_RACE_SURA_W, "d:/ymir work/pc2/sura/"))
	{
		TraceError("Failed to Load Sura Race's Motions : MAIN_RACE_SURA_W");
		return false;
	}

	if (!ShamanMotionLoader(MAIN_RACE_SHAMAN_W, "d:/ymir work/pc/shaman/"))
	{
		TraceError("Failed to Load Shaman Race's Motions : MAIN_RACE_SHAMAN_W");
		return false;
	}

	if (!ShamanMotionLoader(MAIN_RACE_SHAMAN_M, "d:/ymir work/pc2/shaman/"))
	{
		TraceError("Failed to Load Shaman Race's Motions : MAIN_RACE_SHAMAN_M");
		return false;
	}

	return true;
}

/* Register Emotions And Icons */
const bool AbstractResources::RegisterEmotions(const char* racePath)
{
	/* Load Emotion Motions */
	CRaceManager& pkManager = CRaceManager::Instance();

	CRaceData* pRaceData = pkManager.GetSelectedRaceDataPointer();
	if (!pRaceData)
	{
		TraceError("Failed to Find Race Data Pointer: %s", racePath);
		return false;
	}

	std::vector<SMotionRegister> vecEmotion =
	{
		{ CRaceMotionData::NAME_CLAP,			"clap.msa", 0 },
		{ CRaceMotionData::NAME_CHEERS_1,		"cheers_1.msa", 0 },
		{ CRaceMotionData::NAME_CHEERS_2,		"cheers_2.msa", 0 },
		{ CRaceMotionData::NAME_DANCE_1,		"dance_1.msa", 0 },
		{ CRaceMotionData::NAME_DANCE_2,		"dance_2.msa", 0 },
		{ CRaceMotionData::NAME_DANCE_3,		"dance_3.msa", 0 },
		{ CRaceMotionData::NAME_DANCE_4,		"dance_4.msa", 0 },
		{ CRaceMotionData::NAME_DANCE_5,		"dance_5.msa", 0 },
		{ CRaceMotionData::NAME_DANCE_6,		"dance_6.msa", 0 },
		{ CRaceMotionData::NAME_CONGRATULATION, "congratulation.msa", 0 },
		{ CRaceMotionData::NAME_FORGIVE,		"forgive.msa", 0 },
		{ CRaceMotionData::NAME_ANGRY,			"angry.msa", 0 },
		{ CRaceMotionData::NAME_ATTRACTIVE,		"attractive.msa", 0 },
		{ CRaceMotionData::NAME_SAD,			"sad.msa", 0 },
		{ CRaceMotionData::NAME_SHY,			"shy.msa", 0 },
		{ CRaceMotionData::NAME_CHEERUP,		"cheerup.msa", 0 },
		{ CRaceMotionData::NAME_BANTER,			"banter.msa", 0 },
		{ CRaceMotionData::NAME_JOY,			"joy.msa", 0 },
		{ CRaceMotionData::NAME_FRENCH_KISS_WITH_WARRIOR, "french_kiss_with_warrior.msa", 0 },
		{ CRaceMotionData::NAME_FRENCH_KISS_WITH_ASSASSIN, "french_kiss_with_assassin.msa", 0 },
		{ CRaceMotionData::NAME_FRENCH_KISS_WITH_SURA, "french_kiss_with_sura.msa", 0 },
		{ CRaceMotionData::NAME_FRENCH_KISS_WITH_SHAMAN, "french_kiss_with_shaman.msa", 0 },
		{ CRaceMotionData::NAME_KISS_WITH_WARRIOR, "kiss_with_warrior.msa", 0 },
		{ CRaceMotionData::NAME_KISS_WITH_ASSASSIN, "kiss_with_assassin.msa", 0 },
		{ CRaceMotionData::NAME_KISS_WITH_SURA, "kiss_with_sura.msa", 0 },
		{ CRaceMotionData::NAME_KISS_WITH_SHAMAN, "kiss_with_shaman.msa", 0 },
		{ CRaceMotionData::NAME_SLAP_HIT_WITH_WARRIOR, "slap_hit.msa", 0 },
		{ CRaceMotionData::NAME_SLAP_HIT_WITH_ASSASSIN, "slap_hit.msa", 0 },
		{ CRaceMotionData::NAME_SLAP_HIT_WITH_SURA, "slap_hit.msa", 0 },
		{ CRaceMotionData::NAME_SLAP_HIT_WITH_SHAMAN, "slap_hit.msa", 0 },
		{ CRaceMotionData::NAME_SLAP_HURT_WITH_WARRIOR, "slap_hurt.msa", 0 },
		{ CRaceMotionData::NAME_SLAP_HURT_WITH_ASSASSIN, "slap_hurt.msa", 0 },
		{ CRaceMotionData::NAME_SLAP_HURT_WITH_SURA, "slap_hurt.msa", 0 },
		{ CRaceMotionData::NAME_SLAP_HURT_WITH_SHAMAN, "slap_hurt.msa", 0 }
	};

	char szFileName[FILE_MAX_NUM];
	pRaceData->RegisterMotionMode(CRaceMotionData::MODE_GENERAL);
	for (const auto& it : vecEmotion)
	{
		snprintf(szFileName, sizeof(szFileName), "%s%s%s", racePath, "action/", it.fileName);
		pRaceData->RegisterMotionData(CRaceMotionData::MODE_GENERAL, it.idMotion, szFileName, it.Percentage);
	}

	pRaceData->RegisterMotionMode(CRaceMotionData::MODE_WEDDING_DRESS);
	for (const auto& it : vecEmotion)
	{
		snprintf(szFileName, sizeof(szFileName), "%s%s%s", racePath, "action/", it.fileName);
		pRaceData->RegisterMotionData(CRaceMotionData::MODE_WEDDING_DRESS, it.idMotion, szFileName, it.Percentage);
	}

	/* Load Wedding Dress */
	std::vector<SMotionRegister> vecWeddingDress =
	{
		{ CRaceMotionData::NAME_WAIT, "wait.msa", 0 },
		{ CRaceMotionData::NAME_WALK, "walk.msa", 0 },
		{ CRaceMotionData::NAME_RUN, "walk.msa", 0 },
	};

	pRaceData->RegisterMotionMode(CRaceMotionData::MODE_WEDDING_DRESS);
	for (const auto& it : vecWeddingDress)
	{
		snprintf(szFileName, sizeof(szFileName), "%s%s%s", racePath, "wedding/", it.fileName);
		pRaceData->RegisterMotionData(CRaceMotionData::MODE_WEDDING_DRESS, it.idMotion, szFileName, it.Percentage);
	}

	return true;
}

/* Register General Global Race Motions */
const bool AbstractResources::GeneralMotionLoading(const char* racePath)
{
	CRaceManager& pkManager = CRaceManager::Instance();

	CRaceData* pRaceData = pkManager.GetSelectedRaceDataPointer();
	if (!pRaceData)
	{
		TraceError("Failed to Find Race Data Pointer: %s", racePath);
		return false;
	}

	std::vector<SMotionRegister> vecBasicMotions =
	{
		{ CRaceMotionData::NAME_WAIT, "wait.msa", 0 },
		{ CRaceMotionData::NAME_WALK, "walk.msa", 0 },
		{ CRaceMotionData::NAME_RUN, "run.msa", 0 },
		{ CRaceMotionData::NAME_DAMAGE, "damage.msa", 50 },
		{ CRaceMotionData::NAME_DAMAGE, "damage_1.msa", 50 },
		{ CRaceMotionData::NAME_DAMAGE_BACK, "damage_2.msa", 50 },
		{ CRaceMotionData::NAME_DAMAGE_BACK, "damage_3.msa", 50 },
		{ CRaceMotionData::NAME_DAMAGE_FLYING, "damage_flying.msa", 0 },
		{ CRaceMotionData::NAME_STAND_UP, "falling_stand.msa", 0 },
		{ CRaceMotionData::NAME_DAMAGE_FLYING_BACK, "back_damage_flying.msa", 0 },
		{ CRaceMotionData::NAME_STAND_UP_BACK, "back_falling_stand.msa", 0 },
		{ CRaceMotionData::NAME_DEAD, "dead.msa", 0 },
		{ CRaceMotionData::NAME_DIG, "dig.msa", 0 },
	};

	char szFileName[FILE_MAX_NUM];
	pRaceData->RegisterMotionMode(CRaceMotionData::MODE_GENERAL);
	for (const auto& it : vecBasicMotions)
	{
		snprintf(szFileName, sizeof(szFileName), "%s%s%s", racePath, "general/", it.fileName);
		pRaceData->RegisterMotionData(CRaceMotionData::MODE_GENERAL, it.idMotion, szFileName, it.Percentage);
	}

	return true;
}

/* Register Race Motions: Warrior */
const bool AbstractResources::WarriorMotionLoader(uint32_t dwRace, const char* racePath)
{
	CRaceManager& pkManager = CRaceManager::Instance();
	pkManager.SelectRace(dwRace);

	if (!GeneralMotionLoading(racePath))
	{
		TraceError("Failed to Load Warrior General Motions: %s", racePath);
		return false;
	}

	CRaceData* pRaceData = pkManager.GetSelectedRaceDataPointer();
	if (!pRaceData)
	{
		TraceError("Failed to Find Race Data Pointer: %s", racePath);
		return false;
	}

	if (!pRaceData->SetMotionRandomWeight(CRaceMotionData::MODE_GENERAL, CRaceMotionData::NAME_WAIT, 0, 70))
		return false;

	std::vector<SMotionRegister> vecGeneralMotions =
	{
		{ CRaceMotionData::NAME_WAIT, "wait_1.msa", 30 },
		{ CRaceMotionData::NAME_COMBO_ATTACK_1, "attack.msa", 50 },
		{ CRaceMotionData::NAME_COMBO_ATTACK_1, "attack_1.msa", 50 },
	};

	char szFileName[FILE_MAX_NUM];
	pRaceData->RegisterMotionMode(CRaceMotionData::MODE_GENERAL);
	for (const auto& it : vecGeneralMotions)
	{
		snprintf(szFileName, sizeof(szFileName), "%s%s%s", racePath, "general/", it.fileName);
		pRaceData->RegisterMotionData(CRaceMotionData::MODE_GENERAL, it.idMotion, szFileName, it.Percentage);
	}

	std::vector<SMotionRegister> vecSkillMotions =
	{
		{ CRaceMotionData::NAME_SKILL + 107, "guild_yongsinuipi.msa", 0 },
		{ CRaceMotionData::NAME_SKILL + 108, "guild_yongsinuichukbok.msa", 0 },
		{ CRaceMotionData::NAME_SKILL + 109, "guild_seonghwigap.msa", 0 },
		{ CRaceMotionData::NAME_SKILL + 110, "guild_gasokhwa.msa", 0 },
		{ CRaceMotionData::NAME_SKILL + 111, "guild_yongsinuibunno.msa", 0 },
		{ CRaceMotionData::NAME_SKILL + 112, "guild_jumunsul.msa", 0 },
	};

	std::vector<std::pair<uint8_t, const char*>> vecSkillNames =
	{
		{ 1, "samyeon" },
		{ 2, "palbang" },
		{ 3, "jeongwi" },
		{ 4, "geomgyeong" },
		{ 5, "tanhwan" },
		{ 6, "gihyeol" },
		{ 16, "gigongcham" },
		{ 17, "gyeoksan" },
		{ 18, "daejin" },
		{ 19, "cheongeun" },
		{ 20, "geompung" },
		{ 21, "noegeom" },
	};

	char szSkillAdd[4] = "";
	for (uint16_t i = 0; i < CPythonSkill::SKILL_EFFECT_COUNT; i++)
	{
		for (auto& it : vecSkillNames)
		{
			SMotionRegister motion{};

			motion.idMotion = CRaceMotionData::NAME_SKILL + (i * CPythonSkill::SKILL_GRADEGAP) + it.first;
			motion.Percentage = 0;

			if (i > 0)
				snprintf(szSkillAdd, sizeof(szSkillAdd), "_%d", i + 1);
			else
				snprintf(szSkillAdd, sizeof(szSkillAdd), "");

			snprintf(motion.fileName, sizeof(motion.fileName), "%s%s.msa", it.second, szSkillAdd);
			vecSkillMotions.push_back(motion);
		}
	}

	pRaceData->RegisterMotionMode(CRaceMotionData::MODE_GENERAL);
	for (const auto& it : vecSkillMotions)
	{
		snprintf(szFileName, sizeof(szFileName), "%s%s%s", racePath, "skill/", it.fileName);
		pRaceData->RegisterMotionData(CRaceMotionData::MODE_GENERAL, it.idMotion, szFileName, it.Percentage);
	}

	pRaceData->ReserveComboAttack(CRaceMotionData::MODE_GENERAL, COMBO_TYPE_1, 1);
	pRaceData->RegisterComboAttack(CRaceMotionData::MODE_GENERAL, COMBO_TYPE_1, COMBO_INDEX_1, CRaceMotionData::NAME_COMBO_ATTACK_1);

	if (!RegisterEmotions(racePath))
	{
		TraceError("Failed to Load Warrior Emotions: %s", racePath);
		return false;
	}

	std::vector<SMotionRegister> vecSwordMotions =
	{
		{ CRaceMotionData::NAME_WAIT, "wait.msa", 50 },
		{ CRaceMotionData::NAME_WAIT, "wait_1.msa", 50 },
		{ CRaceMotionData::NAME_WALK, "walk.msa", 0 },
		{ CRaceMotionData::NAME_RUN, "run.msa", 0 },
		{ CRaceMotionData::NAME_DAMAGE, "damage.msa", 50 },
		{ CRaceMotionData::NAME_DAMAGE, "damage_1.msa", 50 },
		{ CRaceMotionData::NAME_DAMAGE_BACK, "damage_2.msa", 50 },
		{ CRaceMotionData::NAME_DAMAGE_BACK, "damage_3.msa", 50 },
		{ CRaceMotionData::NAME_COMBO_ATTACK_1, "combo_01.msa", 0 },
		{ CRaceMotionData::NAME_COMBO_ATTACK_2, "combo_02.msa", 0 },
		{ CRaceMotionData::NAME_COMBO_ATTACK_3, "combo_03.msa", 0 },
		{ CRaceMotionData::NAME_COMBO_ATTACK_4, "combo_04.msa", 0 },
		{ CRaceMotionData::NAME_COMBO_ATTACK_5, "combo_05.msa" , 0},
		{ CRaceMotionData::NAME_COMBO_ATTACK_6, "combo_06.msa", 0 },
		{ CRaceMotionData::NAME_COMBO_ATTACK_7, "combo_07.msa", 0 },
	};

	pRaceData->RegisterMotionMode(CRaceMotionData::MODE_ONEHAND_SWORD);
	for (const auto& it : vecSwordMotions)
	{
		snprintf(szFileName, sizeof(szFileName), "%s%s%s", racePath, "onehand_sword/", it.fileName);
		pRaceData->RegisterMotionData(CRaceMotionData::MODE_ONEHAND_SWORD, it.idMotion, szFileName, it.Percentage);
	}

	pRaceData->ReserveComboAttack(CRaceMotionData::MODE_ONEHAND_SWORD, COMBO_TYPE_1, 4);
	pRaceData->RegisterComboAttack(CRaceMotionData::MODE_ONEHAND_SWORD, COMBO_TYPE_1, COMBO_INDEX_1, CRaceMotionData::NAME_COMBO_ATTACK_1);
	pRaceData->RegisterComboAttack(CRaceMotionData::MODE_ONEHAND_SWORD, COMBO_TYPE_1, COMBO_INDEX_2, CRaceMotionData::NAME_COMBO_ATTACK_2);
	pRaceData->RegisterComboAttack(CRaceMotionData::MODE_ONEHAND_SWORD, COMBO_TYPE_1, COMBO_INDEX_3, CRaceMotionData::NAME_COMBO_ATTACK_3);
	pRaceData->RegisterComboAttack(CRaceMotionData::MODE_ONEHAND_SWORD, COMBO_TYPE_1, COMBO_INDEX_4, CRaceMotionData::NAME_COMBO_ATTACK_4);

	pRaceData->ReserveComboAttack(CRaceMotionData::MODE_ONEHAND_SWORD, COMBO_TYPE_2, 5);
	pRaceData->RegisterComboAttack(CRaceMotionData::MODE_ONEHAND_SWORD, COMBO_TYPE_2, COMBO_INDEX_1, CRaceMotionData::NAME_COMBO_ATTACK_1);
	pRaceData->RegisterComboAttack(CRaceMotionData::MODE_ONEHAND_SWORD, COMBO_TYPE_2, COMBO_INDEX_2, CRaceMotionData::NAME_COMBO_ATTACK_2);
	pRaceData->RegisterComboAttack(CRaceMotionData::MODE_ONEHAND_SWORD, COMBO_TYPE_2, COMBO_INDEX_3, CRaceMotionData::NAME_COMBO_ATTACK_3);
	pRaceData->RegisterComboAttack(CRaceMotionData::MODE_ONEHAND_SWORD, COMBO_TYPE_2, COMBO_INDEX_4, CRaceMotionData::NAME_COMBO_ATTACK_5);
	pRaceData->RegisterComboAttack(CRaceMotionData::MODE_ONEHAND_SWORD, COMBO_TYPE_2, COMBO_INDEX_5, CRaceMotionData::NAME_COMBO_ATTACK_7);

	pRaceData->ReserveComboAttack(CRaceMotionData::MODE_ONEHAND_SWORD, COMBO_TYPE_3, 6);
	pRaceData->RegisterComboAttack(CRaceMotionData::MODE_ONEHAND_SWORD, COMBO_TYPE_3, COMBO_INDEX_1, CRaceMotionData::NAME_COMBO_ATTACK_1);
	pRaceData->RegisterComboAttack(CRaceMotionData::MODE_ONEHAND_SWORD, COMBO_TYPE_3, COMBO_INDEX_2, CRaceMotionData::NAME_COMBO_ATTACK_2);
	pRaceData->RegisterComboAttack(CRaceMotionData::MODE_ONEHAND_SWORD, COMBO_TYPE_3, COMBO_INDEX_3, CRaceMotionData::NAME_COMBO_ATTACK_3);
	pRaceData->RegisterComboAttack(CRaceMotionData::MODE_ONEHAND_SWORD, COMBO_TYPE_3, COMBO_INDEX_4, CRaceMotionData::NAME_COMBO_ATTACK_5);
	pRaceData->RegisterComboAttack(CRaceMotionData::MODE_ONEHAND_SWORD, COMBO_TYPE_3, COMBO_INDEX_5, CRaceMotionData::NAME_COMBO_ATTACK_6);
	pRaceData->RegisterComboAttack(CRaceMotionData::MODE_ONEHAND_SWORD, COMBO_TYPE_3, COMBO_INDEX_6, CRaceMotionData::NAME_COMBO_ATTACK_4);

	std::vector<SMotionRegister> vecTwoHandedSwordMotions =
	{
		{ CRaceMotionData::NAME_WAIT, "wait.msa", 70 },
		{ CRaceMotionData::NAME_WAIT, "wait_1.msa", 30 },
		{ CRaceMotionData::NAME_WALK, "walk.msa", 0 },
		{ CRaceMotionData::NAME_RUN, "run.msa", 0 },
		{ CRaceMotionData::NAME_DAMAGE, "damage.msa", 50 },
		{ CRaceMotionData::NAME_DAMAGE, "damage_1.msa", 50 },
		{ CRaceMotionData::NAME_DAMAGE_BACK, "damage_2.msa", 50 },
		{ CRaceMotionData::NAME_DAMAGE_BACK, "damage_3.msa", 50 },
		{ CRaceMotionData::NAME_COMBO_ATTACK_1, "combo_01.msa", 0 },
		{ CRaceMotionData::NAME_COMBO_ATTACK_2, "combo_02.msa", 0 },
		{ CRaceMotionData::NAME_COMBO_ATTACK_3, "combo_03.msa", 0 },
		{ CRaceMotionData::NAME_COMBO_ATTACK_4, "combo_04.msa", 0 },
		{ CRaceMotionData::NAME_COMBO_ATTACK_5, "combo_05.msa", 0 },
		{ CRaceMotionData::NAME_COMBO_ATTACK_6, "combo_06.msa", 0 },
		{ CRaceMotionData::NAME_COMBO_ATTACK_7, "combo_07.msa", 0 },
	};

	pRaceData->RegisterMotionMode(CRaceMotionData::MODE_TWOHAND_SWORD);
	for (const auto& it : vecSwordMotions)
	{
		snprintf(szFileName, sizeof(szFileName), "%s%s%s", racePath, "twohand_sword/", it.fileName);
		pRaceData->RegisterMotionData(CRaceMotionData::MODE_TWOHAND_SWORD, it.idMotion, szFileName, it.Percentage);
	}

	pRaceData->ReserveComboAttack(CRaceMotionData::MODE_TWOHAND_SWORD, COMBO_TYPE_1, 4);
	pRaceData->RegisterComboAttack(CRaceMotionData::MODE_TWOHAND_SWORD, COMBO_TYPE_1, COMBO_INDEX_1, CRaceMotionData::NAME_COMBO_ATTACK_1);
	pRaceData->RegisterComboAttack(CRaceMotionData::MODE_TWOHAND_SWORD, COMBO_TYPE_1, COMBO_INDEX_2, CRaceMotionData::NAME_COMBO_ATTACK_2);
	pRaceData->RegisterComboAttack(CRaceMotionData::MODE_TWOHAND_SWORD, COMBO_TYPE_1, COMBO_INDEX_3, CRaceMotionData::NAME_COMBO_ATTACK_3);
	pRaceData->RegisterComboAttack(CRaceMotionData::MODE_TWOHAND_SWORD, COMBO_TYPE_1, COMBO_INDEX_4, CRaceMotionData::NAME_COMBO_ATTACK_4);

	pRaceData->ReserveComboAttack(CRaceMotionData::MODE_TWOHAND_SWORD, COMBO_TYPE_2, 5);
	pRaceData->RegisterComboAttack(CRaceMotionData::MODE_TWOHAND_SWORD, COMBO_TYPE_2, COMBO_INDEX_1, CRaceMotionData::NAME_COMBO_ATTACK_1);
	pRaceData->RegisterComboAttack(CRaceMotionData::MODE_TWOHAND_SWORD, COMBO_TYPE_2, COMBO_INDEX_2, CRaceMotionData::NAME_COMBO_ATTACK_2);
	pRaceData->RegisterComboAttack(CRaceMotionData::MODE_TWOHAND_SWORD, COMBO_TYPE_2, COMBO_INDEX_3, CRaceMotionData::NAME_COMBO_ATTACK_3);
	pRaceData->RegisterComboAttack(CRaceMotionData::MODE_TWOHAND_SWORD, COMBO_TYPE_2, COMBO_INDEX_4, CRaceMotionData::NAME_COMBO_ATTACK_5);
	pRaceData->RegisterComboAttack(CRaceMotionData::MODE_TWOHAND_SWORD, COMBO_TYPE_2, COMBO_INDEX_5, CRaceMotionData::NAME_COMBO_ATTACK_7);

	pRaceData->ReserveComboAttack(CRaceMotionData::MODE_TWOHAND_SWORD, COMBO_TYPE_3, 6);
	pRaceData->RegisterComboAttack(CRaceMotionData::MODE_TWOHAND_SWORD, COMBO_TYPE_3, COMBO_INDEX_1, CRaceMotionData::NAME_COMBO_ATTACK_1);
	pRaceData->RegisterComboAttack(CRaceMotionData::MODE_TWOHAND_SWORD, COMBO_TYPE_3, COMBO_INDEX_2, CRaceMotionData::NAME_COMBO_ATTACK_2);
	pRaceData->RegisterComboAttack(CRaceMotionData::MODE_TWOHAND_SWORD, COMBO_TYPE_3, COMBO_INDEX_3, CRaceMotionData::NAME_COMBO_ATTACK_3);
	pRaceData->RegisterComboAttack(CRaceMotionData::MODE_TWOHAND_SWORD, COMBO_TYPE_3, COMBO_INDEX_4, CRaceMotionData::NAME_COMBO_ATTACK_5);
	pRaceData->RegisterComboAttack(CRaceMotionData::MODE_TWOHAND_SWORD, COMBO_TYPE_3, COMBO_INDEX_5, CRaceMotionData::NAME_COMBO_ATTACK_6);
	pRaceData->RegisterComboAttack(CRaceMotionData::MODE_TWOHAND_SWORD, COMBO_TYPE_3, COMBO_INDEX_6, CRaceMotionData::NAME_COMBO_ATTACK_4);

	std::vector<SMotionRegister> vecFishingMotions =
	{
		{ CRaceMotionData::NAME_WAIT, "wait.msa", 0 },
		{ CRaceMotionData::NAME_WALK, "walk.msa", 0 },
		{ CRaceMotionData::NAME_RUN, "run.msa", 0 },
		{ CRaceMotionData::NAME_FISHING_THROW, "throw.msa", 0 },
		{ CRaceMotionData::NAME_FISHING_WAIT, "fishing_wait.msa", 0 },
		{ CRaceMotionData::NAME_FISHING_STOP, "fishing_cancel.msa", 0 },
		{ CRaceMotionData::NAME_FISHING_REACT, "fishing_react.msa", 0 },
		{ CRaceMotionData::NAME_FISHING_CATCH, "fishing_catch.msa", 0 },
		{ CRaceMotionData::NAME_FISHING_FAIL, "fishing_fail.msa", 0 },
	};

	pRaceData->RegisterMotionMode(CRaceMotionData::MODE_FISHING);
	for (const auto& it : vecFishingMotions)
	{
		snprintf(szFileName, sizeof(szFileName), "%s%s%s", racePath, "fishing/", it.fileName);
		pRaceData->RegisterMotionData(CRaceMotionData::MODE_FISHING, it.idMotion, szFileName, it.Percentage);
	}

	std::vector<SMotionRegister> vecHorseMotions =
	{
		{ CRaceMotionData::NAME_WAIT, "wait.msa", 90 },
		{ CRaceMotionData::NAME_WAIT, "wait_1.msa", 9 },
		{ CRaceMotionData::NAME_WAIT, "wait_2.msa", 1 },
		{ CRaceMotionData::NAME_WALK, "walk.msa", 0 },
		{ CRaceMotionData::NAME_RUN, "run.msa", 0 },
		{ CRaceMotionData::NAME_DAMAGE, "damage.msa", 0 },
		{ CRaceMotionData::NAME_DAMAGE_BACK, "damage.msa", 0 },
		{ CRaceMotionData::NAME_DEAD, "dead.msa", 0 },
		{ CRaceMotionData::NAME_SKILL + 122, "skill_charge.msa", 0 },
	};

	pRaceData->RegisterMotionMode(CRaceMotionData::MODE_HORSE);
	for (const auto& it : vecHorseMotions)
	{
		snprintf(szFileName, sizeof(szFileName), "%s%s%s", racePath, "horse/", it.fileName);
		pRaceData->RegisterMotionData(CRaceMotionData::MODE_HORSE, it.idMotion, szFileName, it.Percentage);
	}

	std::vector<SMotionRegister> vecHorseMotionsSword =
	{
		{ CRaceMotionData::NAME_COMBO_ATTACK_1, "combo_01.msa", 0 },
		{ CRaceMotionData::NAME_COMBO_ATTACK_2, "combo_02.msa", 0 },
		{ CRaceMotionData::NAME_COMBO_ATTACK_3, "combo_03.msa", 0 },
		{ CRaceMotionData::NAME_SKILL + 121, "skill_wildattack.msa", 0 },
		{ CRaceMotionData::NAME_SKILL + 123, "skill_splash.msa", 0 },
	};

	pRaceData->RegisterMotionMode(CRaceMotionData::MODE_HORSE_ONEHAND_SWORD);
	for (const auto& it : vecHorseMotionsSword)
	{
		snprintf(szFileName, sizeof(szFileName), "%s%s%s", racePath, "horse_onehand_sword/", it.fileName);
		pRaceData->RegisterMotionData(CRaceMotionData::MODE_HORSE_ONEHAND_SWORD, it.idMotion, szFileName, it.Percentage);
	}

	pRaceData->ReserveComboAttack(CRaceMotionData::MODE_HORSE_ONEHAND_SWORD, COMBO_TYPE_1, 3);
	pRaceData->RegisterComboAttack(CRaceMotionData::MODE_HORSE_ONEHAND_SWORD, COMBO_TYPE_1, COMBO_INDEX_1, CRaceMotionData::NAME_COMBO_ATTACK_1);
	pRaceData->RegisterComboAttack(CRaceMotionData::MODE_HORSE_ONEHAND_SWORD, COMBO_TYPE_1, COMBO_INDEX_2, CRaceMotionData::NAME_COMBO_ATTACK_2);
	pRaceData->RegisterComboAttack(CRaceMotionData::MODE_HORSE_ONEHAND_SWORD, COMBO_TYPE_1, COMBO_INDEX_3, CRaceMotionData::NAME_COMBO_ATTACK_3);

	std::vector<SMotionRegister> vecHorseMotionsTwoHandSword =
	{
		{ CRaceMotionData::NAME_COMBO_ATTACK_1, "combo_01.msa", 0 },
		{ CRaceMotionData::NAME_COMBO_ATTACK_2, "combo_02.msa", 0 },
		{ CRaceMotionData::NAME_COMBO_ATTACK_3, "combo_03.msa", 0 },
		{ CRaceMotionData::NAME_SKILL + 121, "skill_wildattack.msa", 0 },
		{ CRaceMotionData::NAME_SKILL + 123, "skill_splash.msa", 0 },
	};

	pRaceData->RegisterMotionMode(CRaceMotionData::MODE_HORSE_TWOHAND_SWORD);
	for (const auto& it : vecHorseMotionsTwoHandSword)
	{
		snprintf(szFileName, sizeof(szFileName), "%s%s%s", racePath, "horse_twohand_sword/", it.fileName);
		pRaceData->RegisterMotionData(CRaceMotionData::MODE_HORSE_TWOHAND_SWORD, it.idMotion, szFileName, it.Percentage);
	}

	pRaceData->ReserveComboAttack(CRaceMotionData::MODE_HORSE_TWOHAND_SWORD, COMBO_TYPE_1, 3);
	pRaceData->RegisterComboAttack(CRaceMotionData::MODE_HORSE_TWOHAND_SWORD, COMBO_TYPE_1, COMBO_INDEX_1, CRaceMotionData::NAME_COMBO_ATTACK_1);
	pRaceData->RegisterComboAttack(CRaceMotionData::MODE_HORSE_TWOHAND_SWORD, COMBO_TYPE_1, COMBO_INDEX_2, CRaceMotionData::NAME_COMBO_ATTACK_2);
	pRaceData->RegisterComboAttack(CRaceMotionData::MODE_HORSE_TWOHAND_SWORD, COMBO_TYPE_1, COMBO_INDEX_3, CRaceMotionData::NAME_COMBO_ATTACK_3);

	pRaceData->RegisterAttachingBoneName(CRaceData::PART_WEAPON, "equip_right_hand");
	//pRaceData->RegisterAttachingBoneName(CRaceData::PART_ACCE, "Bip01 Spine2"); // For Shoulder Sash

	return true;
}

/* Register Race Motions: Assassin */
const bool AbstractResources::AssassinMotionLoader(uint32_t dwRace, const char* racePath)
{
	CRaceManager& pkManager = CRaceManager::Instance();
	pkManager.SelectRace(dwRace);

	if (!GeneralMotionLoading(racePath))
	{
		TraceError("Failed to Load Assassin General Motions: %s", racePath);
		return false;
	}

	CRaceData* pRaceData = pkManager.GetSelectedRaceDataPointer();
	if (!pRaceData)
	{
		TraceError("Failed to Find Race Data Pointer: %s", racePath);
		return false;
	}

	if (!pRaceData->SetMotionRandomWeight(CRaceMotionData::MODE_GENERAL, CRaceMotionData::NAME_WAIT, 0, 70))
		return false;

	std::vector<SMotionRegister> vecGeneralMotions =
	{
		{ CRaceMotionData::NAME_WAIT, "wait_1.msa", 30 },
		{ CRaceMotionData::NAME_COMBO_ATTACK_1, "attack.msa", 50 },
		{ CRaceMotionData::NAME_COMBO_ATTACK_1, "attack_1.msa", 50 },
	};

	char szFileName[FILE_MAX_NUM];
	pRaceData->RegisterMotionMode(CRaceMotionData::MODE_GENERAL);
	for (const auto& it : vecGeneralMotions)
	{
		snprintf(szFileName, sizeof(szFileName), "%s%s%s", racePath, "general/", it.fileName);
		pRaceData->RegisterMotionData(CRaceMotionData::MODE_GENERAL, it.idMotion, szFileName, it.Percentage);
	}

	std::vector<SMotionRegister> vecSkillMotions =
	{
		{ CRaceMotionData::NAME_SKILL + 107, "guild_yongsinuipi.msa", 0 },
		{ CRaceMotionData::NAME_SKILL + 108, "guild_yongsinuichukbok.msa", 0 },
		{ CRaceMotionData::NAME_SKILL + 109, "guild_seonghwigap.msa", 0 },
		{ CRaceMotionData::NAME_SKILL + 110, "guild_gasokhwa.msa", 0 },
		{ CRaceMotionData::NAME_SKILL + 111, "guild_yongsinuibunno.msa", 0 },
		{ CRaceMotionData::NAME_SKILL + 112, "guild_jumunsul.msa", 0 },
	};

	std::vector<std::pair<uint8_t, const char*>> vecSkillNames =
	{
		{ 1, "amseup" },
		{ 2, "gungsin" },
		{ 3, "charyun" },
		{ 4, "eunhyeong" },
		{ 5, "sangong" },
		{ 6, "seomjeon" },
		{ 16, "yeonsa" },
		{ 17, "gwangyeok" },
		{ 18, "hwajo" },
		{ 19, "gyeonggong" },
		{ 20, "dokgigung" },
		{ 21, "seomgwang" },
	};

	char szSkillAdd[4] = "";
	for (uint16_t i = 0; i < CPythonSkill::SKILL_EFFECT_COUNT; i++)
	{
		for (auto& it : vecSkillNames)
		{
			SMotionRegister motion{};

			motion.idMotion = CRaceMotionData::NAME_SKILL + (i * CPythonSkill::SKILL_GRADEGAP) + it.first;
			motion.Percentage = 0;

			if (i > 0)
				snprintf(szSkillAdd, sizeof(szSkillAdd), "_%d", i + 1);
			else
				snprintf(szSkillAdd, sizeof(szSkillAdd), "");

			snprintf(motion.fileName, sizeof(motion.fileName), "%s%s.msa", it.second, szSkillAdd);
			vecSkillMotions.push_back(motion);
		}
	}

	pRaceData->RegisterMotionMode(CRaceMotionData::MODE_GENERAL);
	for (const auto& it : vecSkillMotions)
	{
		snprintf(szFileName, sizeof(szFileName), "%s%s%s", racePath, "skill/", it.fileName);
		pRaceData->RegisterMotionData(CRaceMotionData::MODE_GENERAL, it.idMotion, szFileName, it.Percentage);
	}

	pRaceData->ReserveComboAttack(CRaceMotionData::MODE_GENERAL, COMBO_TYPE_1, 1);
	pRaceData->RegisterComboAttack(CRaceMotionData::MODE_GENERAL, COMBO_TYPE_1, COMBO_INDEX_1, CRaceMotionData::NAME_COMBO_ATTACK_1);

	if (!RegisterEmotions(racePath))
	{
		TraceError("Failed to Load Assassin Emotions: %s", racePath);
		return false;
	}

	std::vector<SMotionRegister> vecSwordMotions =
	{
		{ CRaceMotionData::NAME_WAIT, "wait.msa", 70 },
		{ CRaceMotionData::NAME_WAIT, "wait_1.msa", 30 },
		{ CRaceMotionData::NAME_WALK, "walk.msa" , 0 },
		{ CRaceMotionData::NAME_RUN, "run.msa" , 0 },
		{ CRaceMotionData::NAME_DAMAGE, "damage.msa", 50 },
		{ CRaceMotionData::NAME_DAMAGE, "damage_1.msa", 50 },
		{ CRaceMotionData::NAME_DAMAGE_BACK, "damage_2.msa", 50 },
		{ CRaceMotionData::NAME_DAMAGE_BACK, "damage_3.msa", 50 },
		{ CRaceMotionData::NAME_COMBO_ATTACK_1, "combo_01.msa" , 0 },
		{ CRaceMotionData::NAME_COMBO_ATTACK_2, "combo_02.msa" , 0 },
		{ CRaceMotionData::NAME_COMBO_ATTACK_3, "combo_03.msa" , 0 },
		{ CRaceMotionData::NAME_COMBO_ATTACK_4, "combo_04.msa" , 0 },
		{ CRaceMotionData::NAME_COMBO_ATTACK_5, "combo_05.msa" , 0 },
		{ CRaceMotionData::NAME_COMBO_ATTACK_6, "combo_06.msa" , 0 },
		{ CRaceMotionData::NAME_COMBO_ATTACK_7, "combo_07.msa" , 0 },
	};

	pRaceData->RegisterMotionMode(CRaceMotionData::MODE_ONEHAND_SWORD);
	for (const auto& it : vecSwordMotions)
	{
		snprintf(szFileName, sizeof(szFileName), "%s%s%s", racePath, "onehand_sword/", it.fileName);
		pRaceData->RegisterMotionData(CRaceMotionData::MODE_ONEHAND_SWORD, it.idMotion, szFileName, it.Percentage);
	}

	pRaceData->ReserveComboAttack(CRaceMotionData::MODE_ONEHAND_SWORD, COMBO_TYPE_1, 4);
	pRaceData->RegisterComboAttack(CRaceMotionData::MODE_ONEHAND_SWORD, COMBO_TYPE_1, COMBO_INDEX_1, CRaceMotionData::NAME_COMBO_ATTACK_1);
	pRaceData->RegisterComboAttack(CRaceMotionData::MODE_ONEHAND_SWORD, COMBO_TYPE_1, COMBO_INDEX_2, CRaceMotionData::NAME_COMBO_ATTACK_2);
	pRaceData->RegisterComboAttack(CRaceMotionData::MODE_ONEHAND_SWORD, COMBO_TYPE_1, COMBO_INDEX_3, CRaceMotionData::NAME_COMBO_ATTACK_3);
	pRaceData->RegisterComboAttack(CRaceMotionData::MODE_ONEHAND_SWORD, COMBO_TYPE_1, COMBO_INDEX_4, CRaceMotionData::NAME_COMBO_ATTACK_4);

	pRaceData->ReserveComboAttack(CRaceMotionData::MODE_ONEHAND_SWORD, COMBO_TYPE_2, 5);
	pRaceData->RegisterComboAttack(CRaceMotionData::MODE_ONEHAND_SWORD, COMBO_TYPE_2, COMBO_INDEX_1, CRaceMotionData::NAME_COMBO_ATTACK_1);
	pRaceData->RegisterComboAttack(CRaceMotionData::MODE_ONEHAND_SWORD, COMBO_TYPE_2, COMBO_INDEX_2, CRaceMotionData::NAME_COMBO_ATTACK_2);
	pRaceData->RegisterComboAttack(CRaceMotionData::MODE_ONEHAND_SWORD, COMBO_TYPE_2, COMBO_INDEX_3, CRaceMotionData::NAME_COMBO_ATTACK_3);
	pRaceData->RegisterComboAttack(CRaceMotionData::MODE_ONEHAND_SWORD, COMBO_TYPE_2, COMBO_INDEX_4, CRaceMotionData::NAME_COMBO_ATTACK_5);
	pRaceData->RegisterComboAttack(CRaceMotionData::MODE_ONEHAND_SWORD, COMBO_TYPE_2, COMBO_INDEX_5, CRaceMotionData::NAME_COMBO_ATTACK_7);

	pRaceData->ReserveComboAttack(CRaceMotionData::MODE_ONEHAND_SWORD, COMBO_TYPE_3, 6);
	pRaceData->RegisterComboAttack(CRaceMotionData::MODE_ONEHAND_SWORD, COMBO_TYPE_3, COMBO_INDEX_1, CRaceMotionData::NAME_COMBO_ATTACK_1);
	pRaceData->RegisterComboAttack(CRaceMotionData::MODE_ONEHAND_SWORD, COMBO_TYPE_3, COMBO_INDEX_2, CRaceMotionData::NAME_COMBO_ATTACK_2);
	pRaceData->RegisterComboAttack(CRaceMotionData::MODE_ONEHAND_SWORD, COMBO_TYPE_3, COMBO_INDEX_3, CRaceMotionData::NAME_COMBO_ATTACK_3);
	pRaceData->RegisterComboAttack(CRaceMotionData::MODE_ONEHAND_SWORD, COMBO_TYPE_3, COMBO_INDEX_4, CRaceMotionData::NAME_COMBO_ATTACK_5);
	pRaceData->RegisterComboAttack(CRaceMotionData::MODE_ONEHAND_SWORD, COMBO_TYPE_3, COMBO_INDEX_5, CRaceMotionData::NAME_COMBO_ATTACK_6);
	pRaceData->RegisterComboAttack(CRaceMotionData::MODE_ONEHAND_SWORD, COMBO_TYPE_3, COMBO_INDEX_6, CRaceMotionData::NAME_COMBO_ATTACK_4);

	std::vector<SMotionRegister> vecDaggerMotions =
	{
		{ CRaceMotionData::NAME_WAIT, "wait.msa", 70 },
		{ CRaceMotionData::NAME_WAIT, "wait_1.msa", 30 },
		{ CRaceMotionData::NAME_WALK, "walk.msa", 0 },
		{ CRaceMotionData::NAME_RUN, "run.msa", 0 },
		{ CRaceMotionData::NAME_DAMAGE, "damage.msa", 50 },
		{ CRaceMotionData::NAME_DAMAGE, "damage_1.msa", 50 },
		{ CRaceMotionData::NAME_DAMAGE_BACK, "damage_2.msa", 50 },
		{ CRaceMotionData::NAME_DAMAGE_BACK, "damage_3.msa", 50 },
		{ CRaceMotionData::NAME_COMBO_ATTACK_1, "combo_01.msa", 0 },
		{ CRaceMotionData::NAME_COMBO_ATTACK_2, "combo_02.msa", 0 },
		{ CRaceMotionData::NAME_COMBO_ATTACK_3, "combo_03.msa", 0 },
		{ CRaceMotionData::NAME_COMBO_ATTACK_4, "combo_04.msa", 0 },
		{ CRaceMotionData::NAME_COMBO_ATTACK_5, "combo_05.msa", 0 },
		{ CRaceMotionData::NAME_COMBO_ATTACK_6, "combo_06.msa", 0 },
		{ CRaceMotionData::NAME_COMBO_ATTACK_7, "combo_07.msa", 0 },
		{ CRaceMotionData::NAME_COMBO_ATTACK_8, "combo_08.msa", 0 },
	};

	pRaceData->RegisterMotionMode(CRaceMotionData::MODE_DUALHAND_SWORD);
	for (const auto& it : vecDaggerMotions)
	{
		snprintf(szFileName, sizeof(szFileName), "%s%s%s", racePath, "dualhand_sword/", it.fileName);
		pRaceData->RegisterMotionData(CRaceMotionData::MODE_DUALHAND_SWORD, it.idMotion, szFileName, it.Percentage);
	}

	pRaceData->ReserveComboAttack(CRaceMotionData::MODE_DUALHAND_SWORD, COMBO_TYPE_1, 4);
	pRaceData->RegisterComboAttack(CRaceMotionData::MODE_DUALHAND_SWORD, COMBO_TYPE_1, COMBO_INDEX_1, CRaceMotionData::NAME_COMBO_ATTACK_1);
	pRaceData->RegisterComboAttack(CRaceMotionData::MODE_DUALHAND_SWORD, COMBO_TYPE_1, COMBO_INDEX_2, CRaceMotionData::NAME_COMBO_ATTACK_2);
	pRaceData->RegisterComboAttack(CRaceMotionData::MODE_DUALHAND_SWORD, COMBO_TYPE_1, COMBO_INDEX_3, CRaceMotionData::NAME_COMBO_ATTACK_3);
	pRaceData->RegisterComboAttack(CRaceMotionData::MODE_DUALHAND_SWORD, COMBO_TYPE_1, COMBO_INDEX_4, CRaceMotionData::NAME_COMBO_ATTACK_4);

	pRaceData->ReserveComboAttack(CRaceMotionData::MODE_DUALHAND_SWORD, COMBO_TYPE_2, 5);
	pRaceData->RegisterComboAttack(CRaceMotionData::MODE_DUALHAND_SWORD, COMBO_TYPE_2, COMBO_INDEX_1, CRaceMotionData::NAME_COMBO_ATTACK_1);
	pRaceData->RegisterComboAttack(CRaceMotionData::MODE_DUALHAND_SWORD, COMBO_TYPE_2, COMBO_INDEX_2, CRaceMotionData::NAME_COMBO_ATTACK_2);
	pRaceData->RegisterComboAttack(CRaceMotionData::MODE_DUALHAND_SWORD, COMBO_TYPE_2, COMBO_INDEX_3, CRaceMotionData::NAME_COMBO_ATTACK_3);
	pRaceData->RegisterComboAttack(CRaceMotionData::MODE_DUALHAND_SWORD, COMBO_TYPE_2, COMBO_INDEX_4, CRaceMotionData::NAME_COMBO_ATTACK_5);
	pRaceData->RegisterComboAttack(CRaceMotionData::MODE_DUALHAND_SWORD, COMBO_TYPE_2, COMBO_INDEX_5, CRaceMotionData::NAME_COMBO_ATTACK_7);

	pRaceData->ReserveComboAttack(CRaceMotionData::MODE_DUALHAND_SWORD, COMBO_TYPE_3, 6);
	pRaceData->RegisterComboAttack(CRaceMotionData::MODE_DUALHAND_SWORD, COMBO_TYPE_3, COMBO_INDEX_1, CRaceMotionData::NAME_COMBO_ATTACK_1);
	pRaceData->RegisterComboAttack(CRaceMotionData::MODE_DUALHAND_SWORD, COMBO_TYPE_3, COMBO_INDEX_2, CRaceMotionData::NAME_COMBO_ATTACK_2);
	pRaceData->RegisterComboAttack(CRaceMotionData::MODE_DUALHAND_SWORD, COMBO_TYPE_3, COMBO_INDEX_3, CRaceMotionData::NAME_COMBO_ATTACK_3);
	pRaceData->RegisterComboAttack(CRaceMotionData::MODE_DUALHAND_SWORD, COMBO_TYPE_3, COMBO_INDEX_4, CRaceMotionData::NAME_COMBO_ATTACK_5);
	pRaceData->RegisterComboAttack(CRaceMotionData::MODE_DUALHAND_SWORD, COMBO_TYPE_3, COMBO_INDEX_5, CRaceMotionData::NAME_COMBO_ATTACK_6);
	pRaceData->RegisterComboAttack(CRaceMotionData::MODE_DUALHAND_SWORD, COMBO_TYPE_3, COMBO_INDEX_6, CRaceMotionData::NAME_COMBO_ATTACK_8);

	std::vector<SMotionRegister> vecBowMotions =
	{
		{ CRaceMotionData::NAME_WAIT, "wait.msa", 70 },
		{ CRaceMotionData::NAME_WAIT, "wait_1.msa", 30},
		{ CRaceMotionData::NAME_WALK, "walk.msa", 0 },
		{ CRaceMotionData::NAME_RUN, "run.msa", 0 },
		{ CRaceMotionData::NAME_DAMAGE, "damage.msa", 50},
		{ CRaceMotionData::NAME_DAMAGE, "damage_1.msa", 50},
		{ CRaceMotionData::NAME_DAMAGE_BACK, "damage_2.msa", 50},
		{ CRaceMotionData::NAME_DAMAGE_BACK, "damage_3.msa", 50},
		{ CRaceMotionData::NAME_COMBO_ATTACK_1, "attack.msa", 0 },
	};

	pRaceData->RegisterMotionMode(CRaceMotionData::MODE_BOW);
	for (const auto& it : vecBowMotions)
	{
		snprintf(szFileName, sizeof(szFileName), "%s%s%s", racePath, "bow/", it.fileName);
		pRaceData->RegisterMotionData(CRaceMotionData::MODE_BOW, it.idMotion, szFileName, it.Percentage);
	}

	pRaceData->ReserveComboAttack(CRaceMotionData::MODE_BOW, COMBO_TYPE_1, 1);
	pRaceData->RegisterComboAttack(CRaceMotionData::MODE_BOW, COMBO_TYPE_1, COMBO_INDEX_1, CRaceMotionData::NAME_COMBO_ATTACK_1);

	std::vector<SMotionRegister> vecFishingMotions =
	{
		{ CRaceMotionData::NAME_WAIT, "wait.msa", 0 },
		{ CRaceMotionData::NAME_WALK, "walk.msa", 0 },
		{ CRaceMotionData::NAME_RUN, "run.msa", 0 },
		{ CRaceMotionData::NAME_FISHING_THROW, "throw.msa", 0 },
		{ CRaceMotionData::NAME_FISHING_WAIT, "fishing_wait.msa", 0 },
		{ CRaceMotionData::NAME_FISHING_STOP, "fishing_cancel.msa", 0 },
		{ CRaceMotionData::NAME_FISHING_REACT, "fishing_react.msa", 0 },
		{ CRaceMotionData::NAME_FISHING_CATCH, "fishing_catch.msa", 0 },
		{ CRaceMotionData::NAME_FISHING_FAIL, "fishing_fail.msa", 0 },
	};

	pRaceData->RegisterMotionMode(CRaceMotionData::MODE_FISHING);
	for (const auto& it : vecFishingMotions)
	{
		snprintf(szFileName, sizeof(szFileName), "%s%s%s", racePath, "fishing/", it.fileName);
		pRaceData->RegisterMotionData(CRaceMotionData::MODE_FISHING, it.idMotion, szFileName, it.Percentage);
	}

	std::vector<SMotionRegister> vecHorseMotions =
	{
		{ CRaceMotionData::NAME_WAIT, "wait.msa", 90 },
		{ CRaceMotionData::NAME_WAIT, "wait_1.msa", 9 },
		{ CRaceMotionData::NAME_WAIT, "wait_2.msa", 1 },
		{ CRaceMotionData::NAME_WALK, "walk.msa", 0 },
		{ CRaceMotionData::NAME_RUN, "run.msa", 0 },
		{ CRaceMotionData::NAME_DAMAGE, "damage.msa", 0 },
		{ CRaceMotionData::NAME_DAMAGE_BACK, "damage.msa", 0 },
		{ CRaceMotionData::NAME_DEAD, "dead.msa", 0 },
		{ CRaceMotionData::NAME_SKILL + 122, "skill_charge.msa", 0 },
	};

	pRaceData->RegisterMotionMode(CRaceMotionData::MODE_HORSE);
	for (const auto& it : vecHorseMotions)
	{
		snprintf(szFileName, sizeof(szFileName), "%s%s%s", racePath, "horse/", it.fileName);
		pRaceData->RegisterMotionData(CRaceMotionData::MODE_HORSE, it.idMotion, szFileName, it.Percentage);
	}

	std::vector<SMotionRegister> vecHorseMotionsSword =
	{
		{ CRaceMotionData::NAME_COMBO_ATTACK_1, "combo_01.msa", 0 },
		{ CRaceMotionData::NAME_COMBO_ATTACK_2, "combo_02.msa", 0 },
		{ CRaceMotionData::NAME_COMBO_ATTACK_3, "combo_03.msa", 0 },
		{ CRaceMotionData::NAME_SKILL + 121, "skill_wildattack.msa", 0 },
		{ CRaceMotionData::NAME_SKILL + 123, "skill_splash.msa", 0 },
	};

	pRaceData->RegisterMotionMode(CRaceMotionData::MODE_HORSE_ONEHAND_SWORD);
	for (const auto& it : vecHorseMotionsSword)
	{
		snprintf(szFileName, sizeof(szFileName), "%s%s%s", racePath, "horse_onehand_sword/", it.fileName);
		pRaceData->RegisterMotionData(CRaceMotionData::MODE_HORSE_ONEHAND_SWORD, it.idMotion, szFileName, it.Percentage);
	}

	pRaceData->ReserveComboAttack(CRaceMotionData::MODE_HORSE_ONEHAND_SWORD, COMBO_TYPE_1, 3);
	pRaceData->RegisterComboAttack(CRaceMotionData::MODE_HORSE_ONEHAND_SWORD, COMBO_TYPE_1, COMBO_INDEX_1, CRaceMotionData::NAME_COMBO_ATTACK_1);
	pRaceData->RegisterComboAttack(CRaceMotionData::MODE_HORSE_ONEHAND_SWORD, COMBO_TYPE_1, COMBO_INDEX_2, CRaceMotionData::NAME_COMBO_ATTACK_2);
	pRaceData->RegisterComboAttack(CRaceMotionData::MODE_HORSE_ONEHAND_SWORD, COMBO_TYPE_1, COMBO_INDEX_3, CRaceMotionData::NAME_COMBO_ATTACK_3);

	std::vector<SMotionRegister> vecHorseMotionsDagger =
	{
		{ CRaceMotionData::NAME_COMBO_ATTACK_1, "combo_01.msa", 0 },
		{ CRaceMotionData::NAME_COMBO_ATTACK_2, "combo_02.msa", 0 },
		{ CRaceMotionData::NAME_COMBO_ATTACK_3, "combo_03.msa", 0 },
		{ CRaceMotionData::NAME_SKILL + 121, "skill_wildattack.msa", 0 },
		{ CRaceMotionData::NAME_SKILL + 123, "skill_splash.msa", 0 },
	};

	pRaceData->RegisterMotionMode(CRaceMotionData::MODE_HORSE_DUALHAND_SWORD);
	for (const auto& it : vecHorseMotionsDagger)
	{
		snprintf(szFileName, sizeof(szFileName), "%s%s%s", racePath, "horse_dualhand_sword/", it.fileName);
		pRaceData->RegisterMotionData(CRaceMotionData::MODE_HORSE_DUALHAND_SWORD, it.idMotion, szFileName, it.Percentage);
	}

	pRaceData->ReserveComboAttack(CRaceMotionData::MODE_HORSE_DUALHAND_SWORD, COMBO_TYPE_1, 3);
	pRaceData->RegisterComboAttack(CRaceMotionData::MODE_HORSE_DUALHAND_SWORD, COMBO_TYPE_1, COMBO_INDEX_1, CRaceMotionData::NAME_COMBO_ATTACK_1);
	pRaceData->RegisterComboAttack(CRaceMotionData::MODE_HORSE_DUALHAND_SWORD, COMBO_TYPE_1, COMBO_INDEX_2, CRaceMotionData::NAME_COMBO_ATTACK_2);
	pRaceData->RegisterComboAttack(CRaceMotionData::MODE_HORSE_DUALHAND_SWORD, COMBO_TYPE_1, COMBO_INDEX_3, CRaceMotionData::NAME_COMBO_ATTACK_3);

	std::vector<SMotionRegister> vecHorseMotionsBow =
	{
		{ CRaceMotionData::NAME_WAIT, "wait.msa", 90 },
		{ CRaceMotionData::NAME_WAIT, "wait_1.msa", 9 },
		{ CRaceMotionData::NAME_WAIT, "wait_2.msa", 1 },
		{ CRaceMotionData::NAME_RUN, "run.msa", 0 },
		{ CRaceMotionData::NAME_DAMAGE, "damage.msa", 0 },
		{ CRaceMotionData::NAME_DEAD, "dead.msa", 0 },
		{ CRaceMotionData::NAME_COMBO_ATTACK_1, "attack.msa", 0 },
		{ CRaceMotionData::NAME_SKILL + 121, "skill_wildattack.msa", 0 },
		{ CRaceMotionData::NAME_SKILL + 123, "skill_splash.msa", 0 },
	};

	pRaceData->RegisterMotionMode(CRaceMotionData::MODE_HORSE_BOW);
	for (const auto& it : vecHorseMotionsBow)
	{
		snprintf(szFileName, sizeof(szFileName), "%s%s%s", racePath, "horse_bow/", it.fileName);
		pRaceData->RegisterMotionData(CRaceMotionData::MODE_HORSE_BOW, it.idMotion, szFileName, it.Percentage);
	}

	pRaceData->ReserveComboAttack(CRaceMotionData::MODE_HORSE_BOW, COMBO_TYPE_1, 3);
	pRaceData->RegisterComboAttack(CRaceMotionData::MODE_HORSE_BOW, COMBO_TYPE_1, COMBO_INDEX_1, CRaceMotionData::NAME_COMBO_ATTACK_1);

	pRaceData->RegisterAttachingBoneName(CRaceData::PART_WEAPON, "equip_right");
	pRaceData->RegisterAttachingBoneName(CRaceData::PART_WEAPON_LEFT, "equip_left");
	//pRaceData->RegisterAttachingBoneName(CRaceData::PART_ACCE, "Bip01 Spine2"); // For Shoulder Sash

	return true;
}

/* Register Race Motions: Sura */
const bool AbstractResources::SuraMotionLoader(uint32_t dwRace, const char* racePath)
{
	CRaceManager& pkManager = CRaceManager::Instance();
	pkManager.SelectRace(dwRace);

	if (!GeneralMotionLoading(racePath))
	{
		TraceError("Failed to Load Sura General Motions: %s", racePath);
		return false;
	}

	CRaceData* pRaceData = pkManager.GetSelectedRaceDataPointer();
	if (!pRaceData)
	{
		TraceError("Failed to Find Race Data Pointer: %s", racePath);
		return false;
	}

	std::vector<SMotionRegister> vecGeneralMotions =
	{
		{ CRaceMotionData::NAME_COMBO_ATTACK_1, "attack.msa", 50 },
		{ CRaceMotionData::NAME_COMBO_ATTACK_1, "attack_1.msa", 50 },
	};

	char szFileName[FILE_MAX_NUM];
	pRaceData->RegisterMotionMode(CRaceMotionData::MODE_GENERAL);
	for (const auto& it : vecGeneralMotions)
	{
		snprintf(szFileName, sizeof(szFileName), "%s%s%s", racePath, "general/", it.fileName);
		pRaceData->RegisterMotionData(CRaceMotionData::MODE_GENERAL, it.idMotion, szFileName, it.Percentage);
	}

	std::vector<SMotionRegister> vecSkillMotions =
	{
		{ CRaceMotionData::NAME_SKILL + 107, "guild_yongsinuipi.msa", 0 },
		{ CRaceMotionData::NAME_SKILL + 108, "guild_yongsinuichukbok.msa", 0 },
		{ CRaceMotionData::NAME_SKILL + 109, "guild_seonghwigap.msa", 0 },
		{ CRaceMotionData::NAME_SKILL + 110, "guild_gasokhwa.msa", 0 },
		{ CRaceMotionData::NAME_SKILL + 111, "guild_yongsinuibunno.msa", 0 },
		{ CRaceMotionData::NAME_SKILL + 112, "guild_jumunsul.msa", 0 },
	};

	std::vector<std::pair<uint8_t, const char*>> vecSkillNames =
	{
		{ 1, "swaeryeong" },
		{ 2, "yonggwon" },
		{ 3, "gwigeom" },
		{ 4, "gongpo" },
		{ 5, "jumagap" },
		{ 6, "pabeop" },
		{ 16, "maryeong" },
		{ 17, "hwayeom" },
		{ 18, "muyeong" },
		{ 19, "heuksin" },
		{ 20, "tusok" },
		{ 21, "mahwan" },
	};

	char szSkillAdd[4] = "";
	for (uint16_t i = 0; i < CPythonSkill::SKILL_EFFECT_COUNT; i++)
	{
		for (auto& it : vecSkillNames)
		{
			SMotionRegister motion{};

			motion.idMotion = CRaceMotionData::NAME_SKILL + (i * CPythonSkill::SKILL_GRADEGAP) + it.first;
			motion.Percentage = 0;

			if (i > 0)
				snprintf(szSkillAdd, sizeof(szSkillAdd), "_%d", i + 1);
			else
				snprintf(szSkillAdd, sizeof(szSkillAdd), "");

			snprintf(motion.fileName, sizeof(motion.fileName), "%s%s.msa", it.second, szSkillAdd);
			vecSkillMotions.push_back(motion);
		}
	}

	pRaceData->RegisterMotionMode(CRaceMotionData::MODE_GENERAL);
	for (const auto& it : vecSkillMotions)
	{
		snprintf(szFileName, sizeof(szFileName), "%s%s%s", racePath, "skill/", it.fileName);
		pRaceData->RegisterMotionData(CRaceMotionData::MODE_GENERAL, it.idMotion, szFileName, it.Percentage);
	}

	pRaceData->ReserveComboAttack(CRaceMotionData::MODE_GENERAL, COMBO_TYPE_1, 1);
	pRaceData->RegisterComboAttack(CRaceMotionData::MODE_GENERAL, COMBO_TYPE_1, COMBO_INDEX_1, CRaceMotionData::NAME_COMBO_ATTACK_1);

	if (!RegisterEmotions(racePath))
	{
		TraceError("Failed to Load Sura Emotions: %s", racePath);
		return false;
	}

	std::vector<SMotionRegister> vecSwordMotions =
	{
		{ CRaceMotionData::NAME_WAIT, "wait.msa", 0 },
		{ CRaceMotionData::NAME_WALK, "walk.msa" , 0 },
		{ CRaceMotionData::NAME_RUN, "run.msa" , 0 },
		{ CRaceMotionData::NAME_DAMAGE, "damage.msa", 50 },
		{ CRaceMotionData::NAME_DAMAGE, "damage_1.msa", 50 },
		{ CRaceMotionData::NAME_DAMAGE_BACK, "damage_2.msa", 50 },
		{ CRaceMotionData::NAME_DAMAGE_BACK, "damage_3.msa", 50 },
		{ CRaceMotionData::NAME_COMBO_ATTACK_1, "combo_01.msa" , 0 },
		{ CRaceMotionData::NAME_COMBO_ATTACK_2, "combo_02.msa" , 0 },
		{ CRaceMotionData::NAME_COMBO_ATTACK_3, "combo_03.msa" , 0 },
		{ CRaceMotionData::NAME_COMBO_ATTACK_4, "combo_04.msa" , 0 },
		{ CRaceMotionData::NAME_COMBO_ATTACK_5, "combo_05.msa" , 0 },
		{ CRaceMotionData::NAME_COMBO_ATTACK_6, "combo_06.msa" , 0 },
		{ CRaceMotionData::NAME_COMBO_ATTACK_7, "combo_07.msa" , 0 },
	};

	pRaceData->RegisterMotionMode(CRaceMotionData::MODE_ONEHAND_SWORD);
	for (const auto& it : vecSwordMotions)
	{
		snprintf(szFileName, sizeof(szFileName), "%s%s%s", racePath, "onehand_sword/", it.fileName);
		pRaceData->RegisterMotionData(CRaceMotionData::MODE_ONEHAND_SWORD, it.idMotion, szFileName, it.Percentage);
	}

	pRaceData->ReserveComboAttack(CRaceMotionData::MODE_ONEHAND_SWORD, COMBO_TYPE_1, 4);
	pRaceData->RegisterComboAttack(CRaceMotionData::MODE_ONEHAND_SWORD, COMBO_TYPE_1, COMBO_INDEX_1, CRaceMotionData::NAME_COMBO_ATTACK_1);
	pRaceData->RegisterComboAttack(CRaceMotionData::MODE_ONEHAND_SWORD, COMBO_TYPE_1, COMBO_INDEX_2, CRaceMotionData::NAME_COMBO_ATTACK_2);
	pRaceData->RegisterComboAttack(CRaceMotionData::MODE_ONEHAND_SWORD, COMBO_TYPE_1, COMBO_INDEX_3, CRaceMotionData::NAME_COMBO_ATTACK_3);
	pRaceData->RegisterComboAttack(CRaceMotionData::MODE_ONEHAND_SWORD, COMBO_TYPE_1, COMBO_INDEX_4, CRaceMotionData::NAME_COMBO_ATTACK_4);

	pRaceData->ReserveComboAttack(CRaceMotionData::MODE_ONEHAND_SWORD, COMBO_TYPE_2, 5);
	pRaceData->RegisterComboAttack(CRaceMotionData::MODE_ONEHAND_SWORD, COMBO_TYPE_2, COMBO_INDEX_1, CRaceMotionData::NAME_COMBO_ATTACK_1);
	pRaceData->RegisterComboAttack(CRaceMotionData::MODE_ONEHAND_SWORD, COMBO_TYPE_2, COMBO_INDEX_2, CRaceMotionData::NAME_COMBO_ATTACK_2);
	pRaceData->RegisterComboAttack(CRaceMotionData::MODE_ONEHAND_SWORD, COMBO_TYPE_2, COMBO_INDEX_3, CRaceMotionData::NAME_COMBO_ATTACK_3);
	pRaceData->RegisterComboAttack(CRaceMotionData::MODE_ONEHAND_SWORD, COMBO_TYPE_2, COMBO_INDEX_4, CRaceMotionData::NAME_COMBO_ATTACK_5);
	pRaceData->RegisterComboAttack(CRaceMotionData::MODE_ONEHAND_SWORD, COMBO_TYPE_2, COMBO_INDEX_5, CRaceMotionData::NAME_COMBO_ATTACK_7);

	pRaceData->ReserveComboAttack(CRaceMotionData::MODE_ONEHAND_SWORD, COMBO_TYPE_3, 6);
	pRaceData->RegisterComboAttack(CRaceMotionData::MODE_ONEHAND_SWORD, COMBO_TYPE_3, COMBO_INDEX_1, CRaceMotionData::NAME_COMBO_ATTACK_1);
	pRaceData->RegisterComboAttack(CRaceMotionData::MODE_ONEHAND_SWORD, COMBO_TYPE_3, COMBO_INDEX_2, CRaceMotionData::NAME_COMBO_ATTACK_2);
	pRaceData->RegisterComboAttack(CRaceMotionData::MODE_ONEHAND_SWORD, COMBO_TYPE_3, COMBO_INDEX_3, CRaceMotionData::NAME_COMBO_ATTACK_3);
	pRaceData->RegisterComboAttack(CRaceMotionData::MODE_ONEHAND_SWORD, COMBO_TYPE_3, COMBO_INDEX_4, CRaceMotionData::NAME_COMBO_ATTACK_5);
	pRaceData->RegisterComboAttack(CRaceMotionData::MODE_ONEHAND_SWORD, COMBO_TYPE_3, COMBO_INDEX_5, CRaceMotionData::NAME_COMBO_ATTACK_6);
	pRaceData->RegisterComboAttack(CRaceMotionData::MODE_ONEHAND_SWORD, COMBO_TYPE_3, COMBO_INDEX_6, CRaceMotionData::NAME_COMBO_ATTACK_4);

	std::vector<SMotionRegister> vecFishingMotions =
	{
		{ CRaceMotionData::NAME_WAIT, "wait.msa", 0 },
		{ CRaceMotionData::NAME_WALK, "walk.msa", 0 },
		{ CRaceMotionData::NAME_RUN, "run.msa", 0 },
		{ CRaceMotionData::NAME_FISHING_THROW, "throw.msa", 0 },
		{ CRaceMotionData::NAME_FISHING_WAIT, "fishing_wait.msa", 0 },
		{ CRaceMotionData::NAME_FISHING_STOP, "fishing_cancel.msa", 0 },
		{ CRaceMotionData::NAME_FISHING_REACT, "fishing_react.msa", 0 },
		{ CRaceMotionData::NAME_FISHING_CATCH, "fishing_catch.msa", 0 },
		{ CRaceMotionData::NAME_FISHING_FAIL, "fishing_fail.msa", 0 },
	};

	pRaceData->RegisterMotionMode(CRaceMotionData::MODE_FISHING);
	for (const auto& it : vecFishingMotions)
	{
		snprintf(szFileName, sizeof(szFileName), "%s%s%s", racePath, "fishing/", it.fileName);
		pRaceData->RegisterMotionData(CRaceMotionData::MODE_FISHING, it.idMotion, szFileName, it.Percentage);
	}

	std::vector<SMotionRegister> vecHorseMotions =
	{
		{ CRaceMotionData::NAME_WAIT, "wait.msa", 90 },
		{ CRaceMotionData::NAME_WAIT, "wait_1.msa", 9 },
		{ CRaceMotionData::NAME_WAIT, "wait_2.msa", 1 },
		{ CRaceMotionData::NAME_WALK, "walk.msa", 0 },
		{ CRaceMotionData::NAME_RUN, "run.msa", 0 },
		{ CRaceMotionData::NAME_DAMAGE, "damage.msa", 0 },
		{ CRaceMotionData::NAME_DAMAGE_BACK, "damage.msa", 0 },
		{ CRaceMotionData::NAME_DEAD, "dead.msa", 0 },
		{ CRaceMotionData::NAME_SKILL + 122, "skill_charge.msa", 0 },
		{ CRaceMotionData::NAME_SKILL + 123, "skill_splash.msa", 0 },
	};

	pRaceData->RegisterMotionMode(CRaceMotionData::MODE_HORSE);
	for (const auto& it : vecHorseMotions)
	{
		snprintf(szFileName, sizeof(szFileName), "%s%s%s", racePath, "horse/", it.fileName);
		pRaceData->RegisterMotionData(CRaceMotionData::MODE_HORSE, it.idMotion, szFileName, it.Percentage);
	}

	std::vector<SMotionRegister> vecHorseMotionsSword =
	{
		{ CRaceMotionData::NAME_COMBO_ATTACK_1, "combo_01.msa", 0 },
		{ CRaceMotionData::NAME_COMBO_ATTACK_2, "combo_02.msa", 0 },
		{ CRaceMotionData::NAME_COMBO_ATTACK_3, "combo_03.msa", 0 },
		{ CRaceMotionData::NAME_SKILL + 121, "skill_wildattack.msa", 0 },
	};

	pRaceData->RegisterMotionMode(CRaceMotionData::MODE_HORSE_ONEHAND_SWORD);
	for (const auto& it : vecHorseMotionsSword)
	{
		snprintf(szFileName, sizeof(szFileName), "%s%s%s", racePath, "horse_onehand_sword/", it.fileName);
		pRaceData->RegisterMotionData(CRaceMotionData::MODE_HORSE_ONEHAND_SWORD, it.idMotion, szFileName, it.Percentage);
	}

	pRaceData->ReserveComboAttack(CRaceMotionData::MODE_HORSE_ONEHAND_SWORD, COMBO_TYPE_1, 3);
	pRaceData->RegisterComboAttack(CRaceMotionData::MODE_HORSE_ONEHAND_SWORD, COMBO_TYPE_1, COMBO_INDEX_1, CRaceMotionData::NAME_COMBO_ATTACK_1);
	pRaceData->RegisterComboAttack(CRaceMotionData::MODE_HORSE_ONEHAND_SWORD, COMBO_TYPE_1, COMBO_INDEX_2, CRaceMotionData::NAME_COMBO_ATTACK_2);
	pRaceData->RegisterComboAttack(CRaceMotionData::MODE_HORSE_ONEHAND_SWORD, COMBO_TYPE_1, COMBO_INDEX_3, CRaceMotionData::NAME_COMBO_ATTACK_3);

	pRaceData->RegisterAttachingBoneName(CRaceData::PART_WEAPON, "equip_right");
	//pRaceData->RegisterAttachingBoneName(CRaceData::PART_ACCE, "Bip01 Spine2"); // For Shoulder Sash

	return true;
}

const bool AbstractResources::ShamanMotionLoader(uint32_t dwRace, const char* racePath)
{
	CRaceManager& pkManager = CRaceManager::Instance();
	pkManager.SelectRace(dwRace);

	if (!GeneralMotionLoading(racePath))
		return false;

	CRaceData* pRaceData = pkManager.GetSelectedRaceDataPointer();
	if (!pRaceData)
		return false;

	std::vector<SMotionRegister> vecGeneralMotions =
	{
		{ CRaceMotionData::NAME_COMBO_ATTACK_1, "attack.msa", 50 },
		{ CRaceMotionData::NAME_COMBO_ATTACK_1, "attack_1.msa", 50 },
	};

	char szFileName[FILE_MAX_NUM];
	pRaceData->RegisterMotionMode(CRaceMotionData::MODE_GENERAL);
	for (const auto& it : vecGeneralMotions)
	{
		snprintf(szFileName, sizeof(szFileName), "%s%s%s", racePath, "general/", it.fileName);
		pRaceData->RegisterMotionData(CRaceMotionData::MODE_GENERAL, it.idMotion, szFileName, it.Percentage);
	}

	std::vector<SMotionRegister> vecSkillMotions =
	{
		{ CRaceMotionData::NAME_SKILL + 1, "bipabu.msa" },
		{ CRaceMotionData::NAME_SKILL + 2, "yongpa.msa" },
		{ CRaceMotionData::NAME_SKILL + 3, "paeryong.msa" },
		{ CRaceMotionData::NAME_SKILL + 4, "hosin_target.msa" },
		{ CRaceMotionData::NAME_SKILL + 5, "boho_target.msa" },
		{ CRaceMotionData::NAME_SKILL + 6, "gicheon_target.msa" },
		{ CRaceMotionData::NAME_SKILL + 16, "noejeon.msa" },
		{ CRaceMotionData::NAME_SKILL + 17, "byeorak.msa" },
		{ CRaceMotionData::NAME_SKILL + 18, "pokroe.msa" },
		{ CRaceMotionData::NAME_SKILL + 19, "jeongeop_target.msa" },
		{ CRaceMotionData::NAME_SKILL + 20, "kwaesok_target.msa" },
		{ CRaceMotionData::NAME_SKILL + 21, "jeungryeok_target.msa" },
		{ CRaceMotionData::NAME_SKILL + 107, "guild_yongsinuipi.msa", 0 },
		{ CRaceMotionData::NAME_SKILL + 108, "guild_yongsinuichukbok.msa", 0 },
		{ CRaceMotionData::NAME_SKILL + 109, "guild_seonghwigap.msa", 0 },
		{ CRaceMotionData::NAME_SKILL + 110, "guild_gasokhwa.msa", 0 },
		{ CRaceMotionData::NAME_SKILL + 111, "guild_yongsinuibunno.msa", 0 },
		{ CRaceMotionData::NAME_SKILL + 112, "guild_jumunsul.msa", 0 },
	};

	std::vector<std::pair<uint8_t, const char*>> vecSkillNames =
	{
		{ 1, "bipabu" },
		{ 2, "yongpa" },
		{ 3, "paeryong" },
		{ 4, "hosin" },
		{ 5, "boho" },
		{ 6, "gicheon" },
		{ 16, "noejeon" },
		{ 17, "byeorak" },
		{ 18, "pokroe" },
		{ 19, "jeongeop" },
		{ 20, "kwaesok" },
		{ 21, "jeungryeok" },
	};

	char szSkillAdd[4] = "";
	for (uint16_t i = 1; i < CPythonSkill::SKILL_EFFECT_COUNT; i++)
	{
		for (auto& it : vecSkillNames)
		{
			SMotionRegister motion{};

			motion.idMotion = CRaceMotionData::NAME_SKILL + (i * CPythonSkill::SKILL_GRADEGAP) + it.first;
			motion.Percentage = 0;

			snprintf(szSkillAdd, sizeof(szSkillAdd), "_%d", i + 1);
			snprintf(motion.fileName, sizeof(motion.fileName), "%s%s.msa", it.second, szSkillAdd);
			vecSkillMotions.push_back(motion);
		}
	}

	pRaceData->RegisterMotionMode(CRaceMotionData::MODE_GENERAL);
	for (const auto& it : vecSkillMotions)
	{
		snprintf(szFileName, sizeof(szFileName), "%s%s%s", racePath, "skill/", it.fileName);
		pRaceData->RegisterMotionData(CRaceMotionData::MODE_GENERAL, it.idMotion, szFileName, it.Percentage);
	}

	pRaceData->ReserveComboAttack(CRaceMotionData::MODE_GENERAL, COMBO_TYPE_1, 1);
	pRaceData->RegisterComboAttack(CRaceMotionData::MODE_GENERAL, COMBO_TYPE_1, COMBO_INDEX_1, CRaceMotionData::NAME_COMBO_ATTACK_1);

	if (!RegisterEmotions(racePath))
	{
		TraceError("Failed to Load Shaman Emotions: %s", racePath);
		return false;
	}

	std::vector<SMotionRegister> vecFanMotions =
	{
		{ CRaceMotionData::NAME_WAIT, "wait.msa", 0 },
		{ CRaceMotionData::NAME_WALK, "walk.msa", 0 },
		{ CRaceMotionData::NAME_RUN, "run.msa", 0 },
		{ CRaceMotionData::NAME_DAMAGE, "damage.msa", 50,},
		{ CRaceMotionData::NAME_DAMAGE, "damage_1.msa", 50, },
		{ CRaceMotionData::NAME_DAMAGE_BACK, "damage_2.msa", 50,},
		{ CRaceMotionData::NAME_DAMAGE_BACK, "damage_3.msa", 50, },
		{ CRaceMotionData::NAME_COMBO_ATTACK_1, "combo_01.msa", 0 },
		{ CRaceMotionData::NAME_COMBO_ATTACK_2, "combo_02.msa", 0 },
		{ CRaceMotionData::NAME_COMBO_ATTACK_3, "combo_03.msa", 0 },
		{ CRaceMotionData::NAME_COMBO_ATTACK_4, "combo_04.msa", 0 },
		{ CRaceMotionData::NAME_COMBO_ATTACK_5, "combo_05.msa", 0 },
		{ CRaceMotionData::NAME_COMBO_ATTACK_6, "combo_06.msa", 0 },
		{ CRaceMotionData::NAME_COMBO_ATTACK_7, "combo_07.msa", 0 },
	};

	pRaceData->RegisterMotionMode(CRaceMotionData::MODE_FAN);
	for (const auto& it : vecFanMotions)
	{
		snprintf(szFileName, sizeof(szFileName), "%s%s%s", racePath, "fan/", it.fileName);
		pRaceData->RegisterMotionData(CRaceMotionData::MODE_FAN, it.idMotion, szFileName, it.Percentage);
	}

	pRaceData->ReserveComboAttack(CRaceMotionData::MODE_FAN, COMBO_TYPE_1, 4);
	pRaceData->RegisterComboAttack(CRaceMotionData::MODE_FAN, COMBO_TYPE_1, COMBO_INDEX_1, CRaceMotionData::NAME_COMBO_ATTACK_1);
	pRaceData->RegisterComboAttack(CRaceMotionData::MODE_FAN, COMBO_TYPE_1, COMBO_INDEX_2, CRaceMotionData::NAME_COMBO_ATTACK_2);
	pRaceData->RegisterComboAttack(CRaceMotionData::MODE_FAN, COMBO_TYPE_1, COMBO_INDEX_3, CRaceMotionData::NAME_COMBO_ATTACK_3);
	pRaceData->RegisterComboAttack(CRaceMotionData::MODE_FAN, COMBO_TYPE_1, COMBO_INDEX_4, CRaceMotionData::NAME_COMBO_ATTACK_4);

	pRaceData->ReserveComboAttack(CRaceMotionData::MODE_FAN, COMBO_TYPE_2, 5);
	pRaceData->RegisterComboAttack(CRaceMotionData::MODE_FAN, COMBO_TYPE_2, COMBO_INDEX_1, CRaceMotionData::NAME_COMBO_ATTACK_1);
	pRaceData->RegisterComboAttack(CRaceMotionData::MODE_FAN, COMBO_TYPE_2, COMBO_INDEX_2, CRaceMotionData::NAME_COMBO_ATTACK_2);
	pRaceData->RegisterComboAttack(CRaceMotionData::MODE_FAN, COMBO_TYPE_2, COMBO_INDEX_3, CRaceMotionData::NAME_COMBO_ATTACK_3);
	pRaceData->RegisterComboAttack(CRaceMotionData::MODE_FAN, COMBO_TYPE_2, COMBO_INDEX_4, CRaceMotionData::NAME_COMBO_ATTACK_5);
	pRaceData->RegisterComboAttack(CRaceMotionData::MODE_FAN, COMBO_TYPE_2, COMBO_INDEX_5, CRaceMotionData::NAME_COMBO_ATTACK_7);

	pRaceData->ReserveComboAttack(CRaceMotionData::MODE_FAN, COMBO_TYPE_3, 6);
	pRaceData->RegisterComboAttack(CRaceMotionData::MODE_FAN, COMBO_TYPE_3, COMBO_INDEX_1, CRaceMotionData::NAME_COMBO_ATTACK_1);
	pRaceData->RegisterComboAttack(CRaceMotionData::MODE_FAN, COMBO_TYPE_3, COMBO_INDEX_2, CRaceMotionData::NAME_COMBO_ATTACK_2);
	pRaceData->RegisterComboAttack(CRaceMotionData::MODE_FAN, COMBO_TYPE_3, COMBO_INDEX_3, CRaceMotionData::NAME_COMBO_ATTACK_3);
	pRaceData->RegisterComboAttack(CRaceMotionData::MODE_FAN, COMBO_TYPE_3, COMBO_INDEX_4, CRaceMotionData::NAME_COMBO_ATTACK_5);
	pRaceData->RegisterComboAttack(CRaceMotionData::MODE_FAN, COMBO_TYPE_3, COMBO_INDEX_5, CRaceMotionData::NAME_COMBO_ATTACK_6);
	pRaceData->RegisterComboAttack(CRaceMotionData::MODE_FAN, COMBO_TYPE_3, COMBO_INDEX_6, CRaceMotionData::NAME_COMBO_ATTACK_4);

	std::vector<SMotionRegister> vecBellMotions =
	{
		{ CRaceMotionData::NAME_WAIT, "wait.msa", 0 },
		{ CRaceMotionData::NAME_WALK, "walk.msa", 0 },
		{ CRaceMotionData::NAME_RUN, "run.msa", 0 },
		{ CRaceMotionData::NAME_DAMAGE, "damage.msa", 50,},
		{ CRaceMotionData::NAME_DAMAGE, "damage_1.msa", 50, },
		{ CRaceMotionData::NAME_DAMAGE_BACK, "damage_2.msa", 50,},
		{ CRaceMotionData::NAME_DAMAGE_BACK, "damage_3.msa", 50, },
		{ CRaceMotionData::NAME_COMBO_ATTACK_1, "combo_01.msa", 0 },
		{ CRaceMotionData::NAME_COMBO_ATTACK_2, "combo_02.msa", 0 },
		{ CRaceMotionData::NAME_COMBO_ATTACK_3, "combo_03.msa", 0 },
		{ CRaceMotionData::NAME_COMBO_ATTACK_4, "combo_04.msa", 0 },
		{ CRaceMotionData::NAME_COMBO_ATTACK_5, "combo_05.msa", 0 },
		{ CRaceMotionData::NAME_COMBO_ATTACK_6, "combo_06.msa", 0 },
		{ CRaceMotionData::NAME_COMBO_ATTACK_7, "combo_07.msa", 0 },
	};

	pRaceData->RegisterMotionMode(CRaceMotionData::MODE_BELL);
	for (const auto& it : vecBellMotions)
	{
		snprintf(szFileName, sizeof(szFileName), "%s%s%s", racePath, "bell/", it.fileName);
		pRaceData->RegisterMotionData(CRaceMotionData::MODE_BELL, it.idMotion, szFileName, it.Percentage);
	}

	pRaceData->ReserveComboAttack(CRaceMotionData::MODE_BELL, COMBO_TYPE_1, 4);
	pRaceData->RegisterComboAttack(CRaceMotionData::MODE_BELL, COMBO_TYPE_1, COMBO_INDEX_1, CRaceMotionData::NAME_COMBO_ATTACK_1);
	pRaceData->RegisterComboAttack(CRaceMotionData::MODE_BELL, COMBO_TYPE_1, COMBO_INDEX_2, CRaceMotionData::NAME_COMBO_ATTACK_2);
	pRaceData->RegisterComboAttack(CRaceMotionData::MODE_BELL, COMBO_TYPE_1, COMBO_INDEX_3, CRaceMotionData::NAME_COMBO_ATTACK_3);
	pRaceData->RegisterComboAttack(CRaceMotionData::MODE_BELL, COMBO_TYPE_1, COMBO_INDEX_4, CRaceMotionData::NAME_COMBO_ATTACK_4);

	pRaceData->ReserveComboAttack(CRaceMotionData::MODE_BELL, COMBO_TYPE_2, 5);
	pRaceData->RegisterComboAttack(CRaceMotionData::MODE_BELL, COMBO_TYPE_2, COMBO_INDEX_1, CRaceMotionData::NAME_COMBO_ATTACK_1);
	pRaceData->RegisterComboAttack(CRaceMotionData::MODE_BELL, COMBO_TYPE_2, COMBO_INDEX_2, CRaceMotionData::NAME_COMBO_ATTACK_2);
	pRaceData->RegisterComboAttack(CRaceMotionData::MODE_BELL, COMBO_TYPE_2, COMBO_INDEX_3, CRaceMotionData::NAME_COMBO_ATTACK_3);
	pRaceData->RegisterComboAttack(CRaceMotionData::MODE_BELL, COMBO_TYPE_2, COMBO_INDEX_4, CRaceMotionData::NAME_COMBO_ATTACK_5);
	pRaceData->RegisterComboAttack(CRaceMotionData::MODE_BELL, COMBO_TYPE_2, COMBO_INDEX_5, CRaceMotionData::NAME_COMBO_ATTACK_7);

	pRaceData->ReserveComboAttack(CRaceMotionData::MODE_BELL, COMBO_TYPE_3, 6);
	pRaceData->RegisterComboAttack(CRaceMotionData::MODE_BELL, COMBO_TYPE_3, COMBO_INDEX_1, CRaceMotionData::NAME_COMBO_ATTACK_1);
	pRaceData->RegisterComboAttack(CRaceMotionData::MODE_BELL, COMBO_TYPE_3, COMBO_INDEX_2, CRaceMotionData::NAME_COMBO_ATTACK_2);
	pRaceData->RegisterComboAttack(CRaceMotionData::MODE_BELL, COMBO_TYPE_3, COMBO_INDEX_3, CRaceMotionData::NAME_COMBO_ATTACK_3);
	pRaceData->RegisterComboAttack(CRaceMotionData::MODE_BELL, COMBO_TYPE_3, COMBO_INDEX_4, CRaceMotionData::NAME_COMBO_ATTACK_5);
	pRaceData->RegisterComboAttack(CRaceMotionData::MODE_BELL, COMBO_TYPE_3, COMBO_INDEX_5, CRaceMotionData::NAME_COMBO_ATTACK_6);
	pRaceData->RegisterComboAttack(CRaceMotionData::MODE_BELL, COMBO_TYPE_3, COMBO_INDEX_6, CRaceMotionData::NAME_COMBO_ATTACK_4);

	std::vector<SMotionRegister> vecFishingMotions =
	{
		{ CRaceMotionData::NAME_WAIT, "wait.msa", 0 },
		{ CRaceMotionData::NAME_WALK, "walk.msa", 0 },
		{ CRaceMotionData::NAME_RUN, "run.msa", 0 },
		{ CRaceMotionData::NAME_FISHING_THROW, "throw.msa", 0 },
		{ CRaceMotionData::NAME_FISHING_WAIT, "fishing_wait.msa", 0 },
		{ CRaceMotionData::NAME_FISHING_STOP, "fishing_cancel.msa", 0 },
		{ CRaceMotionData::NAME_FISHING_REACT, "fishing_react.msa", 0 },
		{ CRaceMotionData::NAME_FISHING_CATCH, "fishing_catch.msa", 0 },
		{ CRaceMotionData::NAME_FISHING_FAIL, "fishing_fail.msa", 0 },
	};

	pRaceData->RegisterMotionMode(CRaceMotionData::MODE_FISHING);
	for (const auto& it : vecFishingMotions)
	{
		snprintf(szFileName, sizeof(szFileName), "%s%s%s", racePath, "fishing/", it.fileName);
		pRaceData->RegisterMotionData(CRaceMotionData::MODE_FISHING, it.idMotion, szFileName, it.Percentage);
	}

	std::vector<SMotionRegister> vecHorseMotions =
	{
		{ CRaceMotionData::NAME_WAIT, "wait.msa", 90 },
		{ CRaceMotionData::NAME_WAIT, "wait_1.msa", 9 },
		{ CRaceMotionData::NAME_WAIT, "wait_2.msa", 1 },
		{ CRaceMotionData::NAME_WALK, "walk.msa", 0 },
		{ CRaceMotionData::NAME_RUN, "run.msa", 0 },
		{ CRaceMotionData::NAME_DAMAGE, "damage.msa", 0 },
		{ CRaceMotionData::NAME_DAMAGE_BACK, "damage.msa", 0 },
		{ CRaceMotionData::NAME_DEAD, "dead.msa", 0 },
		{ CRaceMotionData::NAME_SKILL + 122, "skill_charge.msa", 0 },
		{ CRaceMotionData::NAME_SKILL + 123, "skill_splash.msa", 0 },
	};

	pRaceData->RegisterMotionMode(CRaceMotionData::MODE_HORSE);
	for (const auto& it : vecHorseMotions)
	{
		snprintf(szFileName, sizeof(szFileName), "%s%s%s", racePath, "horse/", it.fileName);
		pRaceData->RegisterMotionData(CRaceMotionData::MODE_HORSE, it.idMotion, szFileName, it.Percentage);
	}

	std::vector<SMotionRegister> vecHorseMotionsFan =
	{
		{ CRaceMotionData::NAME_COMBO_ATTACK_1, "combo_01.msa", 0 },
		{ CRaceMotionData::NAME_COMBO_ATTACK_2, "combo_02.msa", 0 },
		{ CRaceMotionData::NAME_COMBO_ATTACK_3, "combo_03.msa", 0 },
		{ CRaceMotionData::NAME_SKILL + 121, "skill_wildattack.msa", 0 },
	};

	pRaceData->RegisterMotionMode(CRaceMotionData::MODE_HORSE_FAN);
	for (const auto& it : vecHorseMotionsFan)
	{
		snprintf(szFileName, sizeof(szFileName), "%s%s%s", racePath, "horse_fan/", it.fileName);
		pRaceData->RegisterMotionData(CRaceMotionData::MODE_HORSE_FAN, it.idMotion, szFileName, it.Percentage);
	}

	pRaceData->ReserveComboAttack(CRaceMotionData::MODE_HORSE_FAN, COMBO_TYPE_1, 3);
	pRaceData->RegisterComboAttack(CRaceMotionData::MODE_HORSE_FAN, COMBO_TYPE_1, COMBO_INDEX_1, CRaceMotionData::NAME_COMBO_ATTACK_1);
	pRaceData->RegisterComboAttack(CRaceMotionData::MODE_HORSE_FAN, COMBO_TYPE_1, COMBO_INDEX_2, CRaceMotionData::NAME_COMBO_ATTACK_2);
	pRaceData->RegisterComboAttack(CRaceMotionData::MODE_HORSE_FAN, COMBO_TYPE_1, COMBO_INDEX_3, CRaceMotionData::NAME_COMBO_ATTACK_3);

	std::vector<SMotionRegister> vecHorseMotionsBell =
	{
		{ CRaceMotionData::NAME_COMBO_ATTACK_1, "combo_01.msa", 0 },
		{ CRaceMotionData::NAME_COMBO_ATTACK_2, "combo_02.msa", 0 },
		{ CRaceMotionData::NAME_COMBO_ATTACK_3, "combo_03.msa", 0 },
		{ CRaceMotionData::NAME_SKILL + 121, "skill_wildattack.msa", 0 },
	};

	pRaceData->RegisterMotionMode(CRaceMotionData::MODE_HORSE_BELL);
	for (const auto& it : vecHorseMotionsBell)
	{
		snprintf(szFileName, sizeof(szFileName), "%s%s%s", racePath, "horse_bell/", it.fileName);
		pRaceData->RegisterMotionData(CRaceMotionData::MODE_HORSE_BELL, it.idMotion, szFileName, it.Percentage);
	}

	pRaceData->ReserveComboAttack(CRaceMotionData::MODE_HORSE_BELL, COMBO_TYPE_1, 3);
	pRaceData->RegisterComboAttack(CRaceMotionData::MODE_HORSE_BELL, COMBO_TYPE_1, COMBO_INDEX_1, CRaceMotionData::NAME_COMBO_ATTACK_1);
	pRaceData->RegisterComboAttack(CRaceMotionData::MODE_HORSE_BELL, COMBO_TYPE_1, COMBO_INDEX_2, CRaceMotionData::NAME_COMBO_ATTACK_2);
	pRaceData->RegisterComboAttack(CRaceMotionData::MODE_HORSE_BELL, COMBO_TYPE_1, COMBO_INDEX_3, CRaceMotionData::NAME_COMBO_ATTACK_3);

	pRaceData->RegisterAttachingBoneName(CRaceData::PART_WEAPON, "equip_right");
	pRaceData->RegisterAttachingBoneName(CRaceData::PART_WEAPON_LEFT, "equip_left");
	//pRaceData->RegisterAttachingBoneName(CRaceData::PART_ACCE, "Bip01 Spine2"); // For Shoulder Sash

	return true;
}

/*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////*/
/*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////*/
PyObject* resRegisterSkills(PyObject * poSelf, PyObject * poArgs)
{
	int32_t race;
	if (!PyTuple_GetInteger(poArgs, 0, &race))
		return Py_BuildException();

	int32_t skill_group;
	if (!PyTuple_GetInteger(poArgs, 1, &skill_group))
		return Py_BuildException();

	int32_t empire;
	if (!PyTuple_GetInteger(poArgs, 2, &empire))
		return Py_BuildException();

	AbstractResources::Instance().RegisterSkills(race, skill_group, empire);
	return Py_BuildNone();
}

PyObject* resGetCurrentLoadingState(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("i", AbstractResources::Instance().GetLoadingState());
}

PyObject* resSetPhaseToLoading(PyObject* poSelf, PyObject* poArgs)
{
	uint8_t bCharacterSlot;
	if (!PyTuple_GetByte(poArgs, 0, &bCharacterSlot))
		return Py_BadArgument();

	uint8_t bRaceIndex = 0, bSkillGroupIndex = 0, bEmpireIndex = 0;
	if (!PyTuple_GetByte(poArgs, 1, &bRaceIndex) || !PyTuple_GetByte(poArgs, 2, &bSkillGroupIndex) || !PyTuple_GetByte(poArgs, 3, &bEmpireIndex))
		return Py_BadArgument();

	int32_t iPlayerX, iPlayerY;
	if (!PyTuple_GetInteger(poArgs, 4, &iPlayerX) || !PyTuple_GetInteger(poArgs, 5, &iPlayerY))
		return Py_BadArgument();

	auto& rkNetStream = CPythonNetworkStream::Instance();
	rkNetStream.Warp(iPlayerX, iPlayerY);
	rkNetStream.SendSelectCharacterPacket(bCharacterSlot);
	rkNetStream.SendClientVersionPacket();
	return Py_BuildNone();
}

PyObject* resSetPhaseToWarping(PyObject* poSelf, PyObject* poArgs)
{
	int32_t iPlayerX, iPlayerY;
	if (!PyTuple_GetInteger(poArgs, 0, &iPlayerX) || !PyTuple_GetInteger(poArgs, 1, &iPlayerY))
		return Py_BadArgument();

	auto& rkNetStream = CPythonNetworkStream::Instance();
	rkNetStream.Warp(iPlayerX, iPlayerY);
	//rkNetStream.SendClientVersionPacket();
	return Py_BuildNone();
}

void initResource()
{
	static PyMethodDef s_methods[] =
	{
		/* Abstract Resource -> Client Loading */
		{ "RegisterSkills",						resRegisterSkills,						METH_VARARGS },
		{ "GetCurrentLoadingState",               resGetCurrentLoadingState,                METH_VARARGS },
		{ "SetPhaseToLoading",                    resSetPhaseToLoading,                    METH_VARARGS },
		{ "SetPhaseToWarping",                    resSetPhaseToWarping,                    METH_VARARGS },
		{  nullptr,								nullptr,								0			 },
	};

	PyObject* poModule = Py_InitModule("Resource", s_methods);
}