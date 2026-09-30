#include "Stdafx.h"
#ifdef ENABLE_PERFORMANCE_IMPROVEMENTS_NEW

#include "PythonPlayerSettingsModule.h"
#include "PythonNetworkStream.h"
#include "PythonBackground.h"
#include "InstanceBase.h"

#include "../EterLib/ResourceManager.h"
#include "../EterPack/EterPackManager.h"
#include "../GameLib/RaceManager.h"

#include "boost/algorithm/string.hpp"

CRaceMotionData& pkMotionManager = CRaceMotionData();
static const DWORD GUILD_SKILL_DRAGONBLOOD = pkMotionManager.NAME_SKILL + 101;
static const DWORD GUILD_SKILL_DRAGONBLESS = pkMotionManager.NAME_SKILL + 102;
static const DWORD GUILD_SKILL_BLESSARMOR = pkMotionManager.NAME_SKILL + 103;
static const DWORD GUILD_SKILL_SPPEDUP = pkMotionManager.NAME_SKILL + 104;
static const DWORD GUILD_SKILL_DRAGONWRATH = pkMotionManager.NAME_SKILL + 105;
static const DWORD GUILD_SKILL_MAGICUP = pkMotionManager.NAME_SKILL + 106;

static const DWORD HORSE_SKILL_WILDATTACK = pkMotionManager.NAME_SKILL + 121;
static const DWORD HORSE_SKILL_CHARGE = pkMotionManager.NAME_SKILL + 122;
static const DWORD HORSE_SKILL_SPLASH = pkMotionManager.NAME_SKILL + 123;

CPythonPlayerSettingsModule::CPythonPlayerSettingsModule()
{
	isLoaded = false;
}

void CPythonPlayerSettingsModule::Load()
{
	if (isLoaded)
		return;

	__RegisterDungeonMapNames();
	__RegisterTitleNames();
	__RegisterTitleAchievementNames();
	__RegisterNameColors();
	__RegisterTitleColors();
	__RegisterEmotionIcons();
	__LoadGameSound();
	__LoadGameEffects();
	__LoadGameInit();
	__LoadEmoticons();
	__LoadGameNPC();
	LoadInitData();

	CPythonPlayerSettingsModule::LoadingRaces();

	CPythonBackground::Instance().__CreateProperty();
	
	isLoaded = true;
	CPythonNetworkStream::Instance().SetClientLoaded();
}

const bool CPythonPlayerSettingsModule::RegisterEmotionAnis(char stFolder[FILE_MAX_NUM])
{
	CRaceManager& pkManager = CRaceManager::Instance();

	CRaceData* pRaceData = pkManager.GetSelectedRaceDataPointer();
	if (!pRaceData)
		return false;

	std::vector<SMotion> m_vecEmotion =
	{
		{ pkMotionManager.NAME_CLAP, "clap.msa", 0 },
		{ pkMotionManager.NAME_CHEERS_1, "cheers_1.msa", 0 },
		{ pkMotionManager.NAME_CHEERS_2, "cheers_2.msa", 0 },
		{ pkMotionManager.NAME_DANCE_1, "dance_1.msa", 0 },
		{ pkMotionManager.NAME_DANCE_2, "dance_2.msa", 0 },
		{ pkMotionManager.NAME_DANCE_3, "dance_3.msa", 0 },
		{ pkMotionManager.NAME_DANCE_4, "dance_4.msa", 0 },
		{ pkMotionManager.NAME_DANCE_5, "dance_5.msa", 0 },
		{ pkMotionManager.NAME_DANCE_6, "dance_6.msa", 0 },
		{ pkMotionManager.NAME_CONGRATULATION, "congratulation.msa", 0 },
		{ pkMotionManager.NAME_FORGIVE, "forgive.msa", 0 },
		{ pkMotionManager.NAME_ANGRY, "angry.msa", 0 },
		{ pkMotionManager.NAME_ATTRACTIVE, "attractive.msa", 0 },
		{ pkMotionManager.NAME_SAD, "sad.msa", 0 },
		{ pkMotionManager.NAME_SHY, "shy.msa", 0 },
		{ pkMotionManager.NAME_CHEERUP, "cheerup.msa", 0 },
		{ pkMotionManager.NAME_BANTER, "banter.msa", 0 },
		{ pkMotionManager.NAME_JOY, "joy.msa", 0 },
		{ pkMotionManager.NAME_FRENCH_KISS_WITH_WARRIOR, "french_kiss_with_warrior.msa", 0 },
		{ pkMotionManager.NAME_FRENCH_KISS_WITH_ASSASSIN, "french_kiss_with_assassin.msa", 0 },
		{ pkMotionManager.NAME_FRENCH_KISS_WITH_SURA, "french_kiss_with_sura.msa", 0 },
		{ pkMotionManager.NAME_FRENCH_KISS_WITH_SHAMAN, "french_kiss_with_shaman.msa", 0 },
		{ pkMotionManager.NAME_KISS_WITH_WARRIOR, "kiss_with_warrior.msa", 0 },
		{ pkMotionManager.NAME_KISS_WITH_ASSASSIN, "kiss_with_assassin.msa", 0 },
		{ pkMotionManager.NAME_KISS_WITH_SURA, "kiss_with_sura.msa", 0 },
		{ pkMotionManager.NAME_KISS_WITH_SHAMAN, "kiss_with_shaman.msa", 0 },
		{ pkMotionManager.NAME_SLAP_HIT_WITH_WARRIOR, "slap_hit.msa", 0 },
		{ pkMotionManager.NAME_SLAP_HIT_WITH_ASSASSIN, "slap_hit.msa", 0 },
		{ pkMotionManager.NAME_SLAP_HIT_WITH_SURA, "slap_hit.msa", 0 },
		{ pkMotionManager.NAME_SLAP_HIT_WITH_SHAMAN, "slap_hit.msa", 0 },
		{ pkMotionManager.NAME_SLAP_HURT_WITH_WARRIOR, "slap_hurt.msa", 0 },
		{ pkMotionManager.NAME_SLAP_HURT_WITH_ASSASSIN, "slap_hurt.msa", 0 },
		{ pkMotionManager.NAME_SLAP_HURT_WITH_SURA, "slap_hurt.msa", 0 },
		{ pkMotionManager.NAME_SLAP_HURT_WITH_SHAMAN, "slap_hurt.msa", 0 },
	};

	char szFileName[FILE_MAX_NUM];
	pRaceData->RegisterMotionMode(pkMotionManager.MODE_GENERAL);
	for (const auto& it : m_vecEmotion)
	{
		snprintf(szFileName, sizeof(szFileName), "%s%s%s", stFolder, "action/", it.stName);
		auto pkMootionThing = pRaceData->RegisterMotionData(pkMotionManager.MODE_GENERAL, it.wMotionIndex, szFileName, it.byPercentage);

		if (pkMootionThing)
		{
			CResourceManager::Instance().LoadStaticCache(pkMootionThing->GetFileName());
		}
	}

	pRaceData->RegisterMotionMode(pkMotionManager.MODE_WEDDING_DRESS);
	for (const auto& it : m_vecEmotion)
	{
		snprintf(szFileName, sizeof(szFileName), "%s%s%s", stFolder, "action/", it.stName);
		auto pkMootionThing = pRaceData->RegisterMotionData(pkMotionManager.MODE_WEDDING_DRESS, it.wMotionIndex, szFileName, it.byPercentage);

		if (pkMootionThing)
		{
			CResourceManager::Instance().LoadStaticCache(pkMootionThing->GetFileName());
		}
	}

	std::vector<SMotion> m_vecEmotionDress =
	{
		{ pkMotionManager.NAME_WAIT, "wait.msa", 0 },
		{ pkMotionManager.NAME_WALK, "walk.msa", 0 },
		{ pkMotionManager.NAME_RUN, "walk.msa", 0 },
	};

	pRaceData->RegisterMotionMode(pkMotionManager.MODE_WEDDING_DRESS);
	for (const auto& it : m_vecEmotionDress)
	{
		snprintf(szFileName, sizeof(szFileName), "%s%s%s", stFolder, "wedding/", it.stName);
		auto pkMootionThing = pRaceData->RegisterMotionData(pkMotionManager.MODE_WEDDING_DRESS, it.wMotionIndex, szFileName, it.byPercentage);

		if (pkMootionThing)
		{
			CResourceManager::Instance().LoadStaticCache(pkMootionThing->GetFileName());
		}
	}

	return true;
}

const bool CPythonPlayerSettingsModule::LoadInitData()
{
	static std::vector<TRaceData> m_vecRaceData =
	{
		{ RACE_WARRIOR_M, "warrior_m.msm", "d:/ymir work/pc/warrior/intro/" },
		{ RACE_WARRIOR_W, "warrior_w.msm", "d:/ymir work/pc2/warrior/intro/" },

		{ RACE_ASSASSIN_W, "assassin_w.msm", "d:/ymir work/pc/assassin/intro/" },
		{ RACE_ASSASSIN_M, "assassin_m.msm", "d:/ymir work/pc2/assassin/intro/" },

		{ RACE_SURA_M, "sura_m.msm", "d:/ymir work/pc/sura/intro/" },
		{ RACE_SURA_W, "sura_w.msm", "d:/ymir work/pc2/sura/intro/" },

		{ RACE_SHAMAN_W, "shaman_w.msm", "d:/ymir work/pc/shaman/intro/" },
		{ RACE_SHAMAN_M, "shaman_m.msm", "d:/ymir work/pc2/shaman/intro/" },
	};

	std::vector<SMotion> m_vecMotion =
	{
		{ pkMotionManager.NAME_INTRO_WAIT, "wait.msa", 0 },
		{ pkMotionManager.NAME_INTRO_SELECTED, "selected.msa", 0 },
		{ pkMotionManager.NAME_INTRO_NOT_SELECTED, "not_selected.msa", 0 },
	};

	CRaceManager& pkManager = CRaceManager::Instance();
	char szFileName[FILE_MAX_NUM];
	for (auto& it : m_vecRaceData)
	{
		pkManager.CreateRace(it.dwRace);
		pkManager.SelectRace(it.dwRace);

		CRaceData* pRaceData = pkManager.GetSelectedRaceDataPointer();
		if (!pRaceData)
			return false;

		if (!pRaceData->LoadRaceData(it.stMSM))
		{
			return false;
		}

		pRaceData->RegisterMotionMode(pkMotionManager.MODE_GENERAL);
		for (const auto& it2 : m_vecMotion)
		{
			snprintf(szFileName, sizeof(szFileName), "%s%s", it.stIntroMotion, it2.stName);
			auto pkMootionThing = pRaceData->RegisterMotionData(pkMotionManager.MODE_GENERAL, it2.wMotionIndex, szFileName, it2.byPercentage);

			if (pkMootionThing)
			{
				CResourceManager::Instance().LoadStaticCache(pkMootionThing->GetFileName());
			}
		}
	}

	return true;
}

void CPythonPlayerSettingsModule::LoadingRaces()
{
	LoadGameWarrior(RACE_WARRIOR_M, "d:/ymir work/pc/warrior/");
	LoadGameWarrior(RACE_WARRIOR_W, "d:/ymir work/pc2/warrior/");
	LoadGameAssassin(RACE_ASSASSIN_W, "d:/ymir work/pc/assassin/");
	LoadGameAssassin(RACE_ASSASSIN_M, "d:/ymir work/pc2/assassin/");
	LoadGameSura(RACE_SURA_M, "d:/ymir work/pc/sura/");
	LoadGameSura(RACE_SURA_W, "d:/ymir work/pc2/sura/");
	LoadGameShaman(RACE_SHAMAN_W, "d:/ymir work/pc/shaman/");
	LoadGameShaman(RACE_SHAMAN_M, "d:/ymir work/pc2/shaman/");
	LoadNewSkills();
}

void CPythonPlayerSettingsModule::__RegisterEmotionIcons()
{
	for (const auto& [index, imagePath] : emotionIcons)
	{
		CGraphicImage* pImage = (CGraphicImage*)CResourceManager::Instance().GetResourcePointer(imagePath);
		m_kMap_iEmotionIndex_pkIconImage.insert(std::make_pair(index, pImage));
	}
}
#include <unordered_set>
void CPythonPlayerSettingsModule::__RegisterTitleNames()
{
	BYTE bIndex = 0;

	const std::unordered_set<std::string> titles = { 
		"PVP_LEVEL0", 
		"PVP_LEVEL1",
		"PVP_LEVEL2",
		"PVP_LEVEL3", 
		"PVP_LEVEL4", 
		"PVP_LEVEL5", 
		"PVP_LEVEL6", 
		"PVP_LEVEL7", 
		"PVP_LEVEL8", 
		"PVP_LEVEL9",
		"PVP_LEVEL10", 
		"PVP_LEVEL11",
		"PVP_LEVEL12",
		"PVP_LEVEL13"
	};

	for (const std::string& stTitle : titles)
	{
		CInstanceBase::RegisterTitleName(bIndex++, GetLocaleInfoString(stTitle));
	}
}
#include <unordered_set>
void CPythonPlayerSettingsModule::__RegisterTitleAchievementNames()
{
	BYTE bIndex = 0;

	// MUST use std::vector to preserve order! std::unordered_set will scramble the order!
	const std::vector<std::string> titles = {
		"",                     // Index 0 = No title
		"TITLE_ACHIEVEMENT_1",
		"TITLE_ACHIEVEMENT_2",
		"TITLE_ACHIEVEMENT_3",
		"TITLE_ACHIEVEMENT_4",
		"TITLE_ACHIEVEMENT_5",
		"TITLE_ACHIEVEMENT_6",
		"TITLE_ACHIEVEMENT_7",
		"TITLE_ACHIEVEMENT_8",
		"TITLE_ACHIEVEMENT_9",
		"TITLE_ACHIEVEMENT_10",
		"TITLE_ACHIEVEMENT_11",
		"TITLE_ACHIEVEMENT_12",
		"TITLE_ACHIEVEMENT_13",
		"TITLE_ACHIEVEMENT_14",
		"TITLE_ACHIEVEMENT_15",
		"TITLE_ACHIEVEMENT_16",
		"TITLE_ACHIEVEMENT_17",
		"TITLE_ACHIEVEMENT_18",
		"TITLE_ACHIEVEMENT_19",
		"TITLE_ACHIEVEMENT_20",
		"TITLE_ACHIEVEMENT_21",
		"TITLE_ACHIEVEMENT_22",
		"TITLE_ACHIEVEMENT_23",
		"TITLE_ACHIEVEMENT_24"
	};

	for (const std::string& stTitle : titles)
	{
		CInstanceBase::RegisterTitleAchievementName(bIndex, GetLocaleInfoString(stTitle));
		bIndex++;
	}
}

void CPythonPlayerSettingsModule::__RegisterNameColors()
{
	for (const auto &[index, rgbTuple] : characterNameColors)
	{
		const auto &[r, g, b] = rgbTuple;
		CInstanceBase::RegisterNameColor(index, r, g, b);
	}
}

void CPythonPlayerSettingsModule::__RegisterTitleColors()
{
	for (const auto &[index, rgbTuple] : characterTitleColors)
	{
		const auto &[r, g, b] = rgbTuple;
		CInstanceBase::RegisterTitleColor(index, r, g, b);
	}
}

void CPythonPlayerSettingsModule::__LoadGameSound()
{
	for (const auto& [useSoundIndex, fileName] : vecUseSoundFileName)
		CPythonItem::Instance().SetUseSoundFileName(useSoundIndex, fileName);

	for (const auto &[dropSoundIndex, fileName] : vecDropSoundFileName)
		CPythonItem::Instance().SetDropSoundFileName(dropSoundIndex, fileName);
}

void CPythonPlayerSettingsModule::__LoadGameNPC()
{
	CMappedFile kMappedFile;
	const void* pvData;

	if (!CEterPackManager::Instance().Get(kMappedFile, g_stNPCList_FileName.c_str(), &pvData))
	{
		Tracef("CPythonLoading::LoadNPCList(): Failed to load file %s", g_stNPCList_FileName.c_str());
		return;
	}

	CMemoryTextFileLoader kMemTextFileLoader;
	kMemTextFileLoader.Bind(kMappedFile.Size(), pvData);

	CTokenVector kTokenVector;
	for (DWORD dwLine = 0; dwLine < kMemTextFileLoader.GetLineCount(); ++dwLine)
	{
		if (!kMemTextFileLoader.SplitLineByTab(dwLine, &kTokenVector))
			continue;

		if (kTokenVector.size() < NPC_TOKEN_TYPE_MODEL_NAME)
		{
			Tracef("CPythonLoading::LoadNPCList(): Strange Token Count [Line:%d / TokenCount:%d]\n", dwLine, kTokenVector.size());
			continue;
		}

		DWORD dwVnum = std::stol(kTokenVector[NPC_TOKEN_TYPE_VNUM].c_str());

		std::string& stModelName = kTokenVector[NPC_TOKEN_TYPE_MODEL_NAME];
		stModelName.erase(remove(stModelName.begin(), stModelName.end(), ' '), stModelName.end());

		if (dwVnum == 0 && kTokenVector.size() > NPC_TOKEN_TYPE_MODEL_NAME)
		{
			std::string& stModelSrcName = kTokenVector[NPC_LIST_TOKEN_MODEL_SRC_NAME];
			stModelSrcName.erase(remove(stModelSrcName.begin(), stModelSrcName.end(), ' '), stModelSrcName.end());

			CRaceManager::Instance().RegisterRaceSrcName(stModelName.c_str(), stModelSrcName.c_str());
		}
		else
		{
			CRaceManager::Instance().RegisterRaceName(dwVnum, stModelName.c_str());
		}
	}
}

void CPythonPlayerSettingsModule::__RegisterDungeonMapNames()
{
	CPythonBackground::Instance().RegisterDungeonMapName(std::string("metin2_map_spiderdungeon").c_str());
	CPythonBackground::Instance().RegisterDungeonMapName(std::string("metin2_map_deviltower1").c_str());
}

void CPythonPlayerSettingsModule::__LoadEmoticons()
{
	CInstanceBase::RegisterEffect(CInstanceBase::EFFECT_EMOTICON + 0, std::string("").c_str(), std::string("d:/ymir work/effect/etc/emoticon/sweat.mse").c_str(), true);
	CPythonNetworkStream::Instance().RegisterEmoticonString(std::string("(sweat)").c_str());
	
	CInstanceBase::RegisterEffect(CInstanceBase::EFFECT_EMOTICON + 1, std::string("").c_str(), std::string("d:/ymir work/effect/etc/emoticon/money.mse").c_str(), true);
	CPythonNetworkStream::Instance().RegisterEmoticonString(std::string("(money)").c_str());

	CInstanceBase::RegisterEffect(CInstanceBase::EFFECT_EMOTICON + 2, std::string("").c_str(), std::string("d:/ymir work/effect/etc/emoticon/happy.mse").c_str(), true);
	CPythonNetworkStream::Instance().RegisterEmoticonString(std::string("(happy)").c_str());

	CInstanceBase::RegisterEffect(CInstanceBase::EFFECT_EMOTICON + 3, std::string("").c_str(), std::string("d:/ymir work/effect/etc/emoticon/love_s.mse").c_str(), true);
	CPythonNetworkStream::Instance().RegisterEmoticonString(std::string("(love_s)").c_str());

	CInstanceBase::RegisterEffect(CInstanceBase::EFFECT_EMOTICON + 4, std::string("").c_str(), std::string("d:/ymir work/effect/etc/emoticon/love_l.mse").c_str(), true);
	CPythonNetworkStream::Instance().RegisterEmoticonString(std::string("(love_l)").c_str());

	CInstanceBase::RegisterEffect(CInstanceBase::EFFECT_EMOTICON + 5, std::string("").c_str(),std::string( "d:/ymir work/effect/etc/emoticon/angry.mse").c_str(), true);
	CPythonNetworkStream::Instance().RegisterEmoticonString(std::string("(angry)").c_str());

	CInstanceBase::RegisterEffect(CInstanceBase::EFFECT_EMOTICON + 6, std::string("").c_str(), std::string("d:/ymir work/effect/etc/emoticon/aha.mse").c_str(), true);
	CPythonNetworkStream::Instance().RegisterEmoticonString(std::string("(aha)").c_str());

	CInstanceBase::RegisterEffect(CInstanceBase::EFFECT_EMOTICON + 7, std::string("").c_str(), std::string("d:/ymir work/effect/etc/emoticon/gloom.mse").c_str(), true);
	CPythonNetworkStream::Instance().RegisterEmoticonString(std::string("(gloom)").c_str());

	CInstanceBase::RegisterEffect(CInstanceBase::EFFECT_EMOTICON + 8, std::string("").c_str(), std::string("d:/ymir work/effect/etc/emoticon/sorry.mse").c_str(), true);
	CPythonNetworkStream::Instance().RegisterEmoticonString(std::string("(sorry)").c_str());

	CInstanceBase::RegisterEffect(CInstanceBase::EFFECT_EMOTICON + 10, std::string("").c_str(), std::string("d:/ymir work/effect/etc/emoticon/question.mse").c_str(), true);
	CPythonNetworkStream::Instance().RegisterEmoticonString(std::string("(?)").c_str());

	CInstanceBase::RegisterEffect(CInstanceBase::EFFECT_EMOTICON + 11, std::string("").c_str(), std::string("d:/ymir work/effect/etc/emoticon/fish.mse").c_str(), true);
	CPythonNetworkStream::Instance().RegisterEmoticonString(std::string("(fish)").c_str());
}

void CPythonPlayerSettingsModule::__LoadGameEffects()
{
	/*for (const auto& [effectName, effectType, effectFolder, effectCache] : vecEffectList)
		CInstanceBase::RegisterEffect(effectName, effectType, effectFolder, effectCache);*/
	for (const SEffectData& it : vecEffectList)
		CInstanceBase::RegisterEffect(it.iEftType, it.stEftAttachBone.c_str(), it.stEftFileName.c_str(), it.bCache);

	CFlyingManager::Instance().RegisterIndexedFlyData(CFlyingManager::FLY_EXP, CFlyingManager::INDEX_FLY_TYPE_NORMAL, std::string("d:/ymir work/effect/etc/gathering/ga_piece_yellow_small2.msf").c_str());
	CFlyingManager::Instance().RegisterIndexedFlyData(CFlyingManager::FLY_HP_MEDIUM, CFlyingManager::INDEX_FLY_TYPE_NORMAL, std::string("d:/ymir work/effect/etc/gathering/ga_piece_red_small.msf").c_str());
	CFlyingManager::Instance().RegisterIndexedFlyData(CFlyingManager::FLY_HP_BIG, CFlyingManager::INDEX_FLY_TYPE_NORMAL, std::string("d:/ymir work/effect/etc/gathering/ga_piece_red_big.msf").c_str());
	CFlyingManager::Instance().RegisterIndexedFlyData(CFlyingManager::FLY_SP_SMALL, CFlyingManager::INDEX_FLY_TYPE_NORMAL, std::string("d:/ymir work/effect/etc/gathering/ga_piece_blue_warrior_small.msf").c_str());
	CFlyingManager::Instance().RegisterIndexedFlyData(CFlyingManager::FLY_SP_MEDIUM, CFlyingManager::INDEX_FLY_TYPE_NORMAL, std::string("d:/ymir work/effect/etc/gathering/ga_piece_blue_small.msf").c_str());
	CFlyingManager::Instance().RegisterIndexedFlyData(CFlyingManager::FLY_SP_BIG, CFlyingManager::INDEX_FLY_TYPE_NORMAL, std::string("d:/ymir work/effect/etc/gathering/ga_piece_blue_big.msf").c_str());
	CFlyingManager::Instance().RegisterIndexedFlyData(CFlyingManager::FLY_FIREWORK1, CFlyingManager::INDEX_FLY_TYPE_FIRE_CRACKER, std::string("d:/ymir work/effect/etc/firecracker/firecracker_1.msf").c_str());
	CFlyingManager::Instance().RegisterIndexedFlyData(CFlyingManager::FLY_FIREWORK2, CFlyingManager::INDEX_FLY_TYPE_FIRE_CRACKER, std::string("d:/ymir work/effect/etc/firecracker/firecracker_2.msf").c_str());
	CFlyingManager::Instance().RegisterIndexedFlyData(CFlyingManager::FLY_FIREWORK3, CFlyingManager::INDEX_FLY_TYPE_FIRE_CRACKER, std::string("d:/ymir work/effect/etc/firecracker/firecracker_3.msf").c_str());
	CFlyingManager::Instance().RegisterIndexedFlyData(CFlyingManager::FLY_FIREWORK4, CFlyingManager::INDEX_FLY_TYPE_FIRE_CRACKER, std::string("d:/ymir work/effect/etc/firecracker/firecracker_4.msf").c_str());
	CFlyingManager::Instance().RegisterIndexedFlyData(CFlyingManager::FLY_FIREWORK5, CFlyingManager::INDEX_FLY_TYPE_FIRE_CRACKER, std::string("d:/ymir work/effect/etc/firecracker/firecracker_5.msf").c_str());
	CFlyingManager::Instance().RegisterIndexedFlyData(CFlyingManager::FLY_FIREWORK6, CFlyingManager::INDEX_FLY_TYPE_FIRE_CRACKER, std::string("d:/ymir work/effect/etc/firecracker/firecracker_6.msf").c_str());
	CFlyingManager::Instance().RegisterIndexedFlyData(CFlyingManager::FLY_FIREWORK_XMAS, CFlyingManager::INDEX_FLY_TYPE_FIRE_CRACKER, std::string("d:/ymir work/effect/etc/firecracker/firecracker_xmas.msf").c_str());
	CFlyingManager::Instance().RegisterIndexedFlyData(CFlyingManager::FLY_CHAIN_LIGHTNING, CFlyingManager::INDEX_FLY_TYPE_NORMAL, std::string("d:/ymir work/pc/shaman/effect/pokroe.msf").c_str());
	CFlyingManager::Instance().RegisterIndexedFlyData(CFlyingManager::FLY_HP_SMALL, CFlyingManager::INDEX_FLY_TYPE_NORMAL, std::string("d:/ymir work/effect/etc/gathering/ga_piece_red_smallest.msf").c_str());
	CFlyingManager::Instance().RegisterIndexedFlyData(CFlyingManager::FLY_SKILL_MUYEONG, CFlyingManager::INDEX_FLY_TYPE_AUTO_FIRE, std::string("d:/ymir work/pc/sura/effect/muyeong_fly.msf").c_str());
	CFlyingManager::Instance().RegisterIndexedFlyData(CFlyingManager::FLY_QUIVER_ATTACK_NORMAL, CFlyingManager::INDEX_FLY_TYPE_NORMAL, std::string("d:/ymir work/pc/assassin/effect/arrow_02.msf").c_str());
}

void CPythonPlayerSettingsModule::__LoadGameInit()
{
	CInstanceBase::SetDustGap(250);
	CInstanceBase::SetHorseDustGap(500);
	CPythonPlayer::Instance().RegisterEffect(CPythonPlayer::EFFECT_PICK, std::string("d:/ymir work/effect/etc/click/click.mse").c_str(), false);
}

void CPythonPlayerSettingsModule::RegisterSkills()
{
	CPythonNetworkStream& net = CPythonNetworkStream::Instance();
	RegisterSkills(net.GetMainActorRace(), net.GetMainActorSkillGroup(), net.GetMainActorEmpire());
}

const bool CPythonPlayerSettingsModule::LoadGeneralMotion(char stFolder[FILE_MAX_NUM])
{
	CRaceManager& pkManager = CRaceManager::Instance();

	CRaceData* pRaceData = pkManager.GetSelectedRaceDataPointer();
	if (!pRaceData)
		return false;

	std::vector<SMotion> m_vecBasicMotion =
	{
		{ pkMotionManager.NAME_WAIT, "wait.msa", 0 },
		{ pkMotionManager.NAME_WALK, "walk.msa", 0 },
		{ pkMotionManager.NAME_RUN, "run.msa", 0 },
		{ pkMotionManager.NAME_DAMAGE, "damage.msa", 50 },
		{ pkMotionManager.NAME_DAMAGE, "damage_1.msa", 50 },
		{ pkMotionManager.NAME_DAMAGE_BACK, "damage_2.msa", 50 },
		{ pkMotionManager.NAME_DAMAGE_BACK, "damage_3.msa", 50 },
		{ pkMotionManager.NAME_DAMAGE_FLYING, "damage_flying.msa", 0 },
		{ pkMotionManager.NAME_STAND_UP, "falling_stand.msa", 0 },
		{ pkMotionManager.NAME_DAMAGE_FLYING_BACK, "back_damage_flying.msa", 0 },
		{ pkMotionManager.NAME_STAND_UP_BACK, "back_falling_stand.msa", 0 },
		{ pkMotionManager.NAME_DEAD, "dead.msa", 0 },
		{ pkMotionManager.NAME_DIG, "dig.msa", 0 },
	};

	char szFileName[FILE_MAX_NUM];
	pRaceData->RegisterMotionMode(pkMotionManager.MODE_GENERAL);
	for (const auto& it : m_vecBasicMotion)
	{
		snprintf(szFileName, sizeof(szFileName), "%s%s%s", stFolder, "general/", it.stName);
		auto pkMootionThing = pRaceData->RegisterMotionData(pkMotionManager.MODE_GENERAL, it.wMotionIndex, szFileName, it.byPercentage);

		if (pkMootionThing)
		{
			CResourceManager::Instance().LoadStaticCache(pkMootionThing->GetFileName());
		}
	}

	return true;
}

//recoded
const bool CPythonPlayerSettingsModule::LoadGameWarrior(DWORD dwRace, char stFolder[FILE_MAX_NUM])
{
	CRaceManager& pkManager = CRaceManager::Instance();
	pkManager.SelectRace(dwRace);

	if (!LoadGeneralMotion(stFolder))
		return false;

	CRaceData* pRaceData = pkManager.GetSelectedRaceDataPointer();
	if (!pRaceData)
		return false;

	if (!pRaceData->SetMotionRandomWeight(pkMotionManager.MODE_GENERAL, pkMotionManager.NAME_WAIT, 0, 70))
		return false;

	std::vector<SMotion> m_vecMotionGeneral =
	{
		{ pkMotionManager.NAME_WAIT, "wait_1.msa", 30 },
		{ pkMotionManager.NAME_COMBO_ATTACK_1, "attack.msa", 50 },
		{ pkMotionManager.NAME_COMBO_ATTACK_1, "attack_1.msa", 50 },
	};

	char szFileName[FILE_MAX_NUM];
	pRaceData->RegisterMotionMode(pkMotionManager.MODE_GENERAL);
	for (const auto& it : m_vecMotionGeneral)
	{
		snprintf(szFileName, sizeof(szFileName), "%s%s%s", stFolder, "general/", it.stName);
		auto pkMootionThing = pRaceData->RegisterMotionData(pkMotionManager.MODE_GENERAL, it.wMotionIndex, szFileName, it.byPercentage);

		if (pkMootionThing)
		{
			CResourceManager::Instance().LoadStaticCache(pkMootionThing->GetFileName());
		}
	}

	std::vector<SMotion> m_vecMotionSkill =
	{
		{ GUILD_SKILL_DRAGONBLOOD, "guild_yongsinuipi.msa", 0 },
		{ GUILD_SKILL_DRAGONBLESS, "guild_yongsinuichukbok.msa", 0 },
		{ GUILD_SKILL_BLESSARMOR, "guild_seonghwigap.msa", 0 },
		{ GUILD_SKILL_SPPEDUP, "guild_gasokhwa.msa", 0 },
		{ GUILD_SKILL_DRAGONWRATH, "guild_yongsinuibunno.msa", 0 },
		{ GUILD_SKILL_MAGICUP, "guild_jumunsul.msa", 0 },
	};

	std::vector<std::pair<BYTE, const char*>> m_vecSkillName = {
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

	char szSkillName[FILE_MAX_NUM];
#ifdef ENABLE_SKILLS_LEVEL_OVER_P
	char szSkillAdd[5] = "";
#else
	char szSkillAdd[4] = "";
#endif
	for (WORD i = 0; i < CPythonSkill::SKILL_EFFECT_COUNT; i++)
	{
		for (auto& it : m_vecSkillName)
		{
			SMotion motion;

			motion.wMotionIndex = pkMotionManager.NAME_SKILL + (i * CPythonSkill::SKILL_GRADEGAP) + it.first;
			motion.byPercentage = 0;

			if (i > 0)
				snprintf(szSkillAdd, sizeof(szSkillAdd), "_%d", i + 1);
			else
				snprintf(szSkillAdd, sizeof(szSkillAdd), "");

			snprintf(motion.stName, sizeof(motion.stName), "%s%s.msa", it.second, szSkillAdd);
			m_vecMotionSkill.push_back(motion);
		}
	}

	pRaceData->RegisterMotionMode(pkMotionManager.MODE_GENERAL);
	for (const auto& it : m_vecMotionSkill)
	{
		snprintf(szFileName, sizeof(szFileName), "%s%s%s", stFolder, "skill/", it.stName);
		auto pkMootionThing = pRaceData->RegisterMotionData(pkMotionManager.MODE_GENERAL, it.wMotionIndex, szFileName, it.byPercentage);

		if (pkMootionThing)
		{
			CResourceManager::Instance().LoadStaticCache(pkMootionThing->GetFileName());
		}
	}

	pRaceData->ReserveComboAttack(pkMotionManager.MODE_GENERAL, COMBO_TYPE_1, 1);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_GENERAL, COMBO_TYPE_1, COMBO_INDEX_1, pkMotionManager.NAME_COMBO_ATTACK_1);

	RegisterEmotionAnis(stFolder);

	std::vector<SMotion> m_vecMotionSword =
	{
		{ pkMotionManager.NAME_WAIT, "wait.msa", 50 },
		{ pkMotionManager.NAME_WAIT, "wait_1.msa", 50 },
		{ pkMotionManager.NAME_WALK, "walk.msa", 0 },
		{ pkMotionManager.NAME_RUN, "run.msa", 0 },
		{ pkMotionManager.NAME_DAMAGE, "damage.msa", 50 },
		{ pkMotionManager.NAME_DAMAGE, "damage_1.msa", 50 },
		{ pkMotionManager.NAME_DAMAGE_BACK, "damage_2.msa", 50 },
		{ pkMotionManager.NAME_DAMAGE_BACK, "damage_3.msa", 50 },
		{ pkMotionManager.NAME_COMBO_ATTACK_1, "combo_01.msa", 0 },
		{ pkMotionManager.NAME_COMBO_ATTACK_2, "combo_02.msa", 0 },
		{ pkMotionManager.NAME_COMBO_ATTACK_3, "combo_03.msa", 0 },
		{ pkMotionManager.NAME_COMBO_ATTACK_4, "combo_04.msa", 0 },
		{ pkMotionManager.NAME_COMBO_ATTACK_5, "combo_05.msa" , 0},
		{ pkMotionManager.NAME_COMBO_ATTACK_6, "combo_06.msa", 0 },
		{ pkMotionManager.NAME_COMBO_ATTACK_7, "combo_07.msa", 0 },
	};

	pRaceData->RegisterMotionMode(pkMotionManager.MODE_ONEHAND_SWORD);
	for (const auto& it : m_vecMotionSword)
	{
		snprintf(szFileName, sizeof(szFileName), "%s%s%s", stFolder, "onehand_sword/", it.stName);
		auto pkMootionThing = pRaceData->RegisterMotionData(pkMotionManager.MODE_ONEHAND_SWORD, it.wMotionIndex, szFileName, it.byPercentage);

		if (pkMootionThing)
		{
			CResourceManager::Instance().LoadStaticCache(pkMootionThing->GetFileName());
		}
	}

	pRaceData->ReserveComboAttack(pkMotionManager.MODE_ONEHAND_SWORD, COMBO_TYPE_1, 4);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_ONEHAND_SWORD, COMBO_TYPE_1, COMBO_INDEX_1, pkMotionManager.NAME_COMBO_ATTACK_1);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_ONEHAND_SWORD, COMBO_TYPE_1, COMBO_INDEX_2, pkMotionManager.NAME_COMBO_ATTACK_2);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_ONEHAND_SWORD, COMBO_TYPE_1, COMBO_INDEX_3, pkMotionManager.NAME_COMBO_ATTACK_3);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_ONEHAND_SWORD, COMBO_TYPE_1, COMBO_INDEX_4, pkMotionManager.NAME_COMBO_ATTACK_4);

	pRaceData->ReserveComboAttack(pkMotionManager.MODE_ONEHAND_SWORD, COMBO_TYPE_2, 5);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_ONEHAND_SWORD, COMBO_TYPE_2, COMBO_INDEX_1, pkMotionManager.NAME_COMBO_ATTACK_1);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_ONEHAND_SWORD, COMBO_TYPE_2, COMBO_INDEX_2, pkMotionManager.NAME_COMBO_ATTACK_2);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_ONEHAND_SWORD, COMBO_TYPE_2, COMBO_INDEX_3, pkMotionManager.NAME_COMBO_ATTACK_3);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_ONEHAND_SWORD, COMBO_TYPE_2, COMBO_INDEX_4, pkMotionManager.NAME_COMBO_ATTACK_5);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_ONEHAND_SWORD, COMBO_TYPE_2, COMBO_INDEX_5, pkMotionManager.NAME_COMBO_ATTACK_7);

	pRaceData->ReserveComboAttack(pkMotionManager.MODE_ONEHAND_SWORD, COMBO_TYPE_3, 6);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_ONEHAND_SWORD, COMBO_TYPE_3, COMBO_INDEX_1, pkMotionManager.NAME_COMBO_ATTACK_1);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_ONEHAND_SWORD, COMBO_TYPE_3, COMBO_INDEX_2, pkMotionManager.NAME_COMBO_ATTACK_2);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_ONEHAND_SWORD, COMBO_TYPE_3, COMBO_INDEX_3, pkMotionManager.NAME_COMBO_ATTACK_3);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_ONEHAND_SWORD, COMBO_TYPE_3, COMBO_INDEX_4, pkMotionManager.NAME_COMBO_ATTACK_5);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_ONEHAND_SWORD, COMBO_TYPE_3, COMBO_INDEX_5, pkMotionManager.NAME_COMBO_ATTACK_6);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_ONEHAND_SWORD, COMBO_TYPE_3, COMBO_INDEX_6, pkMotionManager.NAME_COMBO_ATTACK_4);

	std::vector<SMotion> m_vecMotionTwoHandSword =
	{
		{ pkMotionManager.NAME_WAIT, "wait.msa", 70 },
		{ pkMotionManager.NAME_WAIT, "wait_1.msa", 30 },
		{ pkMotionManager.NAME_WALK, "walk.msa", 0 },
		{ pkMotionManager.NAME_RUN, "run.msa", 0 },
		{ pkMotionManager.NAME_DAMAGE, "damage.msa", 50 },
		{ pkMotionManager.NAME_DAMAGE, "damage_1.msa", 50 },
		{ pkMotionManager.NAME_DAMAGE_BACK, "damage_2.msa", 50 },
		{ pkMotionManager.NAME_DAMAGE_BACK, "damage_3.msa", 50 },
		{ pkMotionManager.NAME_COMBO_ATTACK_1, "combo_01.msa", 0 },
		{ pkMotionManager.NAME_COMBO_ATTACK_2, "combo_02.msa", 0 },
		{ pkMotionManager.NAME_COMBO_ATTACK_3, "combo_03.msa", 0 },
		{ pkMotionManager.NAME_COMBO_ATTACK_4, "combo_04.msa", 0 },
		{ pkMotionManager.NAME_COMBO_ATTACK_5, "combo_05.msa", 0 },
		{ pkMotionManager.NAME_COMBO_ATTACK_6, "combo_06.msa", 0 },
		{ pkMotionManager.NAME_COMBO_ATTACK_7, "combo_07.msa", 0 },
	};

	pRaceData->RegisterMotionMode(pkMotionManager.MODE_TWOHAND_SWORD);
	for (const auto& it : m_vecMotionSword)
	{
		snprintf(szFileName, sizeof(szFileName), "%s%s%s", stFolder, "twohand_sword/", it.stName);
		auto pkMootionThing = pRaceData->RegisterMotionData(pkMotionManager.MODE_TWOHAND_SWORD, it.wMotionIndex, szFileName, it.byPercentage);

		if (pkMootionThing)
		{
			CResourceManager::Instance().LoadStaticCache(pkMootionThing->GetFileName());
		}
	}

	pRaceData->ReserveComboAttack(pkMotionManager.MODE_TWOHAND_SWORD, COMBO_TYPE_1, 4);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_TWOHAND_SWORD, COMBO_TYPE_1, COMBO_INDEX_1, pkMotionManager.NAME_COMBO_ATTACK_1);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_TWOHAND_SWORD, COMBO_TYPE_1, COMBO_INDEX_2, pkMotionManager.NAME_COMBO_ATTACK_2);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_TWOHAND_SWORD, COMBO_TYPE_1, COMBO_INDEX_3, pkMotionManager.NAME_COMBO_ATTACK_3);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_TWOHAND_SWORD, COMBO_TYPE_1, COMBO_INDEX_4, pkMotionManager.NAME_COMBO_ATTACK_4);

	pRaceData->ReserveComboAttack(pkMotionManager.MODE_TWOHAND_SWORD, COMBO_TYPE_2, 5);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_TWOHAND_SWORD, COMBO_TYPE_2, COMBO_INDEX_1, pkMotionManager.NAME_COMBO_ATTACK_1);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_TWOHAND_SWORD, COMBO_TYPE_2, COMBO_INDEX_2, pkMotionManager.NAME_COMBO_ATTACK_2);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_TWOHAND_SWORD, COMBO_TYPE_2, COMBO_INDEX_3, pkMotionManager.NAME_COMBO_ATTACK_3);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_TWOHAND_SWORD, COMBO_TYPE_2, COMBO_INDEX_4, pkMotionManager.NAME_COMBO_ATTACK_5);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_TWOHAND_SWORD, COMBO_TYPE_2, COMBO_INDEX_5, pkMotionManager.NAME_COMBO_ATTACK_7);

	pRaceData->ReserveComboAttack(pkMotionManager.MODE_TWOHAND_SWORD, COMBO_TYPE_3, 6);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_TWOHAND_SWORD, COMBO_TYPE_3, COMBO_INDEX_1, pkMotionManager.NAME_COMBO_ATTACK_1);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_TWOHAND_SWORD, COMBO_TYPE_3, COMBO_INDEX_2, pkMotionManager.NAME_COMBO_ATTACK_2);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_TWOHAND_SWORD, COMBO_TYPE_3, COMBO_INDEX_3, pkMotionManager.NAME_COMBO_ATTACK_3);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_TWOHAND_SWORD, COMBO_TYPE_3, COMBO_INDEX_4, pkMotionManager.NAME_COMBO_ATTACK_5);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_TWOHAND_SWORD, COMBO_TYPE_3, COMBO_INDEX_5, pkMotionManager.NAME_COMBO_ATTACK_6);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_TWOHAND_SWORD, COMBO_TYPE_3, COMBO_INDEX_6, pkMotionManager.NAME_COMBO_ATTACK_4);

	std::vector<SMotion> m_vecMotionFishing =
	{
		{ pkMotionManager.NAME_WAIT, "wait.msa", 0 },
		{ pkMotionManager.NAME_WALK, "walk.msa", 0 },
		{ pkMotionManager.NAME_RUN, "run.msa", 0 },
		{ pkMotionManager.NAME_FISHING_THROW, "throw.msa", 0 },
		{ pkMotionManager.NAME_FISHING_WAIT, "fishing_wait.msa", 0 },
		{ pkMotionManager.NAME_FISHING_STOP, "fishing_cancel.msa", 0 },
		{ pkMotionManager.NAME_FISHING_REACT, "fishing_react.msa", 0 },
		{ pkMotionManager.NAME_FISHING_CATCH, "fishing_catch.msa", 0 },
		{ pkMotionManager.NAME_FISHING_FAIL, "fishing_fail.msa", 0 },
	};

	pRaceData->RegisterMotionMode(pkMotionManager.MODE_FISHING);
	for (const auto& it : m_vecMotionFishing)
	{
		snprintf(szFileName, sizeof(szFileName), "%s%s%s", stFolder, "fishing/", it.stName);
		auto pkMootionThing = pRaceData->RegisterMotionData(pkMotionManager.MODE_FISHING, it.wMotionIndex, szFileName, it.byPercentage);

		if (pkMootionThing)
		{
			CResourceManager::Instance().LoadStaticCache(pkMootionThing->GetFileName());
		}
	}

	std::vector<SMotion> m_vecMotionHorse =
	{
		{ pkMotionManager.NAME_WAIT, "wait.msa", 90 },
		{ pkMotionManager.NAME_WAIT, "wait_1.msa", 9 },
		{ pkMotionManager.NAME_WAIT, "wait_2.msa", 1 },
		{ pkMotionManager.NAME_WALK, "walk.msa", 0 },
		{ pkMotionManager.NAME_RUN, "run.msa", 0 },
		{ pkMotionManager.NAME_DAMAGE, "damage.msa", 0 },
		{ pkMotionManager.NAME_DAMAGE_BACK, "damage.msa", 0 },
		{ pkMotionManager.NAME_DEAD, "dead.msa", 0 },
		{ HORSE_SKILL_CHARGE, "skill_charge.msa", 0 },
	};

	pRaceData->RegisterMotionMode(pkMotionManager.MODE_HORSE);
	for (const auto& it : m_vecMotionHorse)
	{
		snprintf(szFileName, sizeof(szFileName), "%s%s%s", stFolder, "horse/", it.stName);
		auto pkMootionThing = pRaceData->RegisterMotionData(pkMotionManager.MODE_HORSE, it.wMotionIndex, szFileName, it.byPercentage);

		if (pkMootionThing)
		{
			CResourceManager::Instance().LoadStaticCache(pkMootionThing->GetFileName());
		}
	}

	std::vector<SMotion> m_vecMotionHorseSword =
	{
		{ pkMotionManager.NAME_COMBO_ATTACK_1, "combo_01.msa", 0 },
		{ pkMotionManager.NAME_COMBO_ATTACK_2, "combo_02.msa", 0 },
		{ pkMotionManager.NAME_COMBO_ATTACK_3, "combo_03.msa", 0 },
		{ HORSE_SKILL_WILDATTACK, "skill_wildattack.msa", 0 },
		{ HORSE_SKILL_SPLASH, "skill_splash.msa", 0 },
	};

	pRaceData->RegisterMotionMode(pkMotionManager.MODE_HORSE_ONEHAND_SWORD);
	for (const auto& it : m_vecMotionHorseSword)
	{
		snprintf(szFileName, sizeof(szFileName), "%s%s%s", stFolder, "horse_onehand_sword/", it.stName);
		auto pkMootionThing = pRaceData->RegisterMotionData(pkMotionManager.MODE_HORSE_ONEHAND_SWORD, it.wMotionIndex, szFileName, it.byPercentage);

		if (pkMootionThing)
		{
			CResourceManager::Instance().LoadStaticCache(pkMootionThing->GetFileName());
		}
	}

	pRaceData->ReserveComboAttack(pkMotionManager.MODE_HORSE_ONEHAND_SWORD, COMBO_TYPE_1, 3);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_HORSE_ONEHAND_SWORD, COMBO_TYPE_1, COMBO_INDEX_1, pkMotionManager.NAME_COMBO_ATTACK_1);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_HORSE_ONEHAND_SWORD, COMBO_TYPE_1, COMBO_INDEX_2, pkMotionManager.NAME_COMBO_ATTACK_2);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_HORSE_ONEHAND_SWORD, COMBO_TYPE_1, COMBO_INDEX_3, pkMotionManager.NAME_COMBO_ATTACK_3);

	std::vector<SMotion> m_vecMotionHorseTwoHandSword =
	{
		{ pkMotionManager.NAME_COMBO_ATTACK_1, "combo_01.msa", 0 },
		{ pkMotionManager.NAME_COMBO_ATTACK_2, "combo_02.msa", 0 },
		{ pkMotionManager.NAME_COMBO_ATTACK_3, "combo_03.msa", 0 },
		{ HORSE_SKILL_WILDATTACK, "skill_wildattack.msa", 0 },
		{ HORSE_SKILL_SPLASH, "skill_splash.msa", 0 },
	};

	pRaceData->RegisterMotionMode(pkMotionManager.MODE_HORSE_TWOHAND_SWORD);
	for (const auto& it : m_vecMotionHorseTwoHandSword)
	{
		snprintf(szFileName, sizeof(szFileName), "%s%s%s", stFolder, "horse_twohand_sword/", it.stName);
		auto pkMootionThing = pRaceData->RegisterMotionData(pkMotionManager.MODE_HORSE_TWOHAND_SWORD, it.wMotionIndex, szFileName, it.byPercentage);

		if (pkMootionThing)
		{
			CResourceManager::Instance().LoadStaticCache(pkMootionThing->GetFileName());
		}
	}

	pRaceData->ReserveComboAttack(pkMotionManager.MODE_HORSE_TWOHAND_SWORD, COMBO_TYPE_1, 3);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_HORSE_TWOHAND_SWORD, COMBO_TYPE_1, COMBO_INDEX_1, pkMotionManager.NAME_COMBO_ATTACK_1);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_HORSE_TWOHAND_SWORD, COMBO_TYPE_1, COMBO_INDEX_2, pkMotionManager.NAME_COMBO_ATTACK_2);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_HORSE_TWOHAND_SWORD, COMBO_TYPE_1, COMBO_INDEX_3, pkMotionManager.NAME_COMBO_ATTACK_3);

	pRaceData->RegisterAttachingBoneName(CRaceData::PART_WEAPON, "equip_right_hand");

#ifdef ENABLE_SHOULDER_SASH_SYSTEM
	pRaceData->RegisterAttachingBoneName(CRaceData::PART_SASH, "Bip01 Spine2");
#endif

	return true;
}


const bool CPythonPlayerSettingsModule::LoadGameAssassin(DWORD dwRace, char stFolder[FILE_MAX_NUM])
{
	CRaceManager& pkManager = CRaceManager::Instance();
	pkManager.SelectRace(dwRace);

	if (!LoadGeneralMotion(stFolder))
		return false;

	CRaceData* pRaceData = pkManager.GetSelectedRaceDataPointer();
	if (!pRaceData)
		return false;

	if (!pRaceData->SetMotionRandomWeight(pkMotionManager.MODE_GENERAL, pkMotionManager.NAME_WAIT, 0, 70))
		return false;

	std::vector<SMotion> m_vecMotionGeneral =
	{
		{ pkMotionManager.NAME_WAIT, "wait_1.msa", 30 },
		{ pkMotionManager.NAME_COMBO_ATTACK_1, "attack.msa", 50 },
		{ pkMotionManager.NAME_COMBO_ATTACK_1, "attack_1.msa", 50 },
	};

	char szFileName[FILE_MAX_NUM];
	pRaceData->RegisterMotionMode(pkMotionManager.MODE_GENERAL);
	for (const auto& it : m_vecMotionGeneral)
	{
		snprintf(szFileName, sizeof(szFileName), "%s%s%s", stFolder, "general/", it.stName);
		auto pkMootionThing = pRaceData->RegisterMotionData(pkMotionManager.MODE_GENERAL, it.wMotionIndex, szFileName, it.byPercentage);

		if (pkMootionThing)
		{
			CResourceManager::Instance().LoadStaticCache(pkMootionThing->GetFileName());
		}
	}

	std::vector<SMotion> m_vecMotionSkill =
	{
		{ GUILD_SKILL_DRAGONBLOOD, "guild_yongsinuipi.msa", 0 },
		{ GUILD_SKILL_DRAGONBLESS, "guild_yongsinuichukbok.msa", 0 },
		{ GUILD_SKILL_BLESSARMOR, "guild_seonghwigap.msa", 0 },
		{ GUILD_SKILL_SPPEDUP, "guild_gasokhwa.msa", 0 },
		{ GUILD_SKILL_DRAGONWRATH, "guild_yongsinuibunno.msa", 0 },
		{ GUILD_SKILL_MAGICUP, "guild_jumunsul.msa", 0 },
	};

	std::vector<std::pair<BYTE, const char*>> m_vecSkillName = {
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

	char szSkillName[FILE_MAX_NUM];
#ifdef ENABLE_SKILLS_LEVEL_OVER_P
	char szSkillAdd[5] = "";
#else
	char szSkillAdd[4] = "";
#endif
	for (WORD i = 0; i < CPythonSkill::SKILL_EFFECT_COUNT; i++)
	{
		for (auto& it : m_vecSkillName)
		{
			SMotion motion;

			motion.wMotionIndex = pkMotionManager.NAME_SKILL + (i * CPythonSkill::SKILL_GRADEGAP) + it.first;
			motion.byPercentage = 0;

			if (i > 0)
				snprintf(szSkillAdd, sizeof(szSkillAdd), "_%d", i + 1);
			else
				snprintf(szSkillAdd, sizeof(szSkillAdd), "");

			snprintf(motion.stName, sizeof(motion.stName), "%s%s.msa", it.second, szSkillAdd);
			m_vecMotionSkill.push_back(motion);
		}
	}

	pRaceData->RegisterMotionMode(pkMotionManager.MODE_GENERAL);
	for (const auto& it : m_vecMotionSkill)
	{
		snprintf(szFileName, sizeof(szFileName), "%s%s%s", stFolder, "skill/", it.stName);
		auto pkMootionThing = pRaceData->RegisterMotionData(pkMotionManager.MODE_GENERAL, it.wMotionIndex, szFileName, it.byPercentage);

		if (pkMootionThing)
		{
			CResourceManager::Instance().LoadStaticCache(pkMootionThing->GetFileName());
		}
	}

	pRaceData->ReserveComboAttack(pkMotionManager.MODE_GENERAL, COMBO_TYPE_1, 1);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_GENERAL, COMBO_TYPE_1, COMBO_INDEX_1, pkMotionManager.NAME_COMBO_ATTACK_1);

	RegisterEmotionAnis(stFolder);

	std::vector<SMotion> m_vecMotionSword =
	{
		{ pkMotionManager.NAME_WAIT, "wait.msa", 70 },
		{ pkMotionManager.NAME_WAIT, "wait_1.msa", 30 },
		{ pkMotionManager.NAME_WALK, "walk.msa" , 0 },
		{ pkMotionManager.NAME_RUN, "run.msa" , 0 },
		{ pkMotionManager.NAME_DAMAGE, "damage.msa", 50 },
		{ pkMotionManager.NAME_DAMAGE, "damage_1.msa", 50 },
		{ pkMotionManager.NAME_DAMAGE_BACK, "damage_2.msa", 50 },
		{ pkMotionManager.NAME_DAMAGE_BACK, "damage_3.msa", 50 },
		{ pkMotionManager.NAME_COMBO_ATTACK_1, "combo_01.msa" , 0 },
		{ pkMotionManager.NAME_COMBO_ATTACK_2, "combo_02.msa" , 0 },
		{ pkMotionManager.NAME_COMBO_ATTACK_3, "combo_03.msa" , 0 },
		{ pkMotionManager.NAME_COMBO_ATTACK_4, "combo_04.msa" , 0 },
		{ pkMotionManager.NAME_COMBO_ATTACK_5, "combo_05.msa" , 0 },
		{ pkMotionManager.NAME_COMBO_ATTACK_6, "combo_06.msa" , 0 },
		{ pkMotionManager.NAME_COMBO_ATTACK_7, "combo_07.msa" , 0 },
	};

	pRaceData->RegisterMotionMode(pkMotionManager.MODE_ONEHAND_SWORD);
	for (const auto& it : m_vecMotionSword)
	{
		snprintf(szFileName, sizeof(szFileName), "%s%s%s", stFolder, "onehand_sword/", it.stName);
		auto pkMootionThing = pRaceData->RegisterMotionData(pkMotionManager.MODE_ONEHAND_SWORD, it.wMotionIndex, szFileName, it.byPercentage);

		if (pkMootionThing)
		{
			CResourceManager::Instance().LoadStaticCache(pkMootionThing->GetFileName());
		}
	}

	pRaceData->ReserveComboAttack(pkMotionManager.MODE_ONEHAND_SWORD, COMBO_TYPE_1, 4);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_ONEHAND_SWORD, COMBO_TYPE_1, COMBO_INDEX_1, pkMotionManager.NAME_COMBO_ATTACK_1);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_ONEHAND_SWORD, COMBO_TYPE_1, COMBO_INDEX_2, pkMotionManager.NAME_COMBO_ATTACK_2);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_ONEHAND_SWORD, COMBO_TYPE_1, COMBO_INDEX_3, pkMotionManager.NAME_COMBO_ATTACK_3);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_ONEHAND_SWORD, COMBO_TYPE_1, COMBO_INDEX_4, pkMotionManager.NAME_COMBO_ATTACK_4);

	pRaceData->ReserveComboAttack(pkMotionManager.MODE_ONEHAND_SWORD, COMBO_TYPE_2, 5);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_ONEHAND_SWORD, COMBO_TYPE_2, COMBO_INDEX_1, pkMotionManager.NAME_COMBO_ATTACK_1);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_ONEHAND_SWORD, COMBO_TYPE_2, COMBO_INDEX_2, pkMotionManager.NAME_COMBO_ATTACK_2);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_ONEHAND_SWORD, COMBO_TYPE_2, COMBO_INDEX_3, pkMotionManager.NAME_COMBO_ATTACK_3);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_ONEHAND_SWORD, COMBO_TYPE_2, COMBO_INDEX_4, pkMotionManager.NAME_COMBO_ATTACK_5);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_ONEHAND_SWORD, COMBO_TYPE_2, COMBO_INDEX_5, pkMotionManager.NAME_COMBO_ATTACK_7);

	pRaceData->ReserveComboAttack(pkMotionManager.MODE_ONEHAND_SWORD, COMBO_TYPE_3, 6);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_ONEHAND_SWORD, COMBO_TYPE_3, COMBO_INDEX_1, pkMotionManager.NAME_COMBO_ATTACK_1);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_ONEHAND_SWORD, COMBO_TYPE_3, COMBO_INDEX_2, pkMotionManager.NAME_COMBO_ATTACK_2);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_ONEHAND_SWORD, COMBO_TYPE_3, COMBO_INDEX_3, pkMotionManager.NAME_COMBO_ATTACK_3);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_ONEHAND_SWORD, COMBO_TYPE_3, COMBO_INDEX_4, pkMotionManager.NAME_COMBO_ATTACK_5);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_ONEHAND_SWORD, COMBO_TYPE_3, COMBO_INDEX_5, pkMotionManager.NAME_COMBO_ATTACK_6);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_ONEHAND_SWORD, COMBO_TYPE_3, COMBO_INDEX_6, pkMotionManager.NAME_COMBO_ATTACK_4);

	std::vector<SMotion> m_vecMotionDagger =
	{
		{ pkMotionManager.NAME_WAIT, "wait.msa", 70 },
		{ pkMotionManager.NAME_WAIT, "wait_1.msa", 30 },
		{ pkMotionManager.NAME_WALK, "walk.msa", 0 },
		{ pkMotionManager.NAME_RUN, "run.msa", 0 },
		{ pkMotionManager.NAME_DAMAGE, "damage.msa", 50 },
		{ pkMotionManager.NAME_DAMAGE, "damage_1.msa", 50 },
		{ pkMotionManager.NAME_DAMAGE_BACK, "damage_2.msa", 50 },
		{ pkMotionManager.NAME_DAMAGE_BACK, "damage_3.msa", 50 },
		{ pkMotionManager.NAME_COMBO_ATTACK_1, "combo_01.msa", 0 },
		{ pkMotionManager.NAME_COMBO_ATTACK_2, "combo_02.msa", 0 },
		{ pkMotionManager.NAME_COMBO_ATTACK_3, "combo_03.msa", 0 },
		{ pkMotionManager.NAME_COMBO_ATTACK_4, "combo_04.msa", 0 },
		{ pkMotionManager.NAME_COMBO_ATTACK_5, "combo_05.msa", 0 },
		{ pkMotionManager.NAME_COMBO_ATTACK_6, "combo_06.msa", 0 },
		{ pkMotionManager.NAME_COMBO_ATTACK_7, "combo_07.msa", 0 },
		{ pkMotionManager.NAME_COMBO_ATTACK_8, "combo_08.msa", 0 },
	};

	pRaceData->RegisterMotionMode(pkMotionManager.MODE_DUALHAND_SWORD);
	for (const auto& it : m_vecMotionDagger)
	{
		snprintf(szFileName, sizeof(szFileName), "%s%s%s", stFolder, "dualhand_sword/", it.stName);
		auto pkMootionThing = pRaceData->RegisterMotionData(pkMotionManager.MODE_DUALHAND_SWORD, it.wMotionIndex, szFileName, it.byPercentage);

		if (pkMootionThing)
		{
			CResourceManager::Instance().LoadStaticCache(pkMootionThing->GetFileName());
		}
	}

	pRaceData->ReserveComboAttack(pkMotionManager.MODE_DUALHAND_SWORD, COMBO_TYPE_1, 4);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_DUALHAND_SWORD, COMBO_TYPE_1, COMBO_INDEX_1, pkMotionManager.NAME_COMBO_ATTACK_1);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_DUALHAND_SWORD, COMBO_TYPE_1, COMBO_INDEX_2, pkMotionManager.NAME_COMBO_ATTACK_2);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_DUALHAND_SWORD, COMBO_TYPE_1, COMBO_INDEX_3, pkMotionManager.NAME_COMBO_ATTACK_3);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_DUALHAND_SWORD, COMBO_TYPE_1, COMBO_INDEX_4, pkMotionManager.NAME_COMBO_ATTACK_4);

	pRaceData->ReserveComboAttack(pkMotionManager.MODE_DUALHAND_SWORD, COMBO_TYPE_2, 5);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_DUALHAND_SWORD, COMBO_TYPE_2, COMBO_INDEX_1, pkMotionManager.NAME_COMBO_ATTACK_1);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_DUALHAND_SWORD, COMBO_TYPE_2, COMBO_INDEX_2, pkMotionManager.NAME_COMBO_ATTACK_2);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_DUALHAND_SWORD, COMBO_TYPE_2, COMBO_INDEX_3, pkMotionManager.NAME_COMBO_ATTACK_3);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_DUALHAND_SWORD, COMBO_TYPE_2, COMBO_INDEX_4, pkMotionManager.NAME_COMBO_ATTACK_5);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_DUALHAND_SWORD, COMBO_TYPE_2, COMBO_INDEX_5, pkMotionManager.NAME_COMBO_ATTACK_7);

	pRaceData->ReserveComboAttack(pkMotionManager.MODE_DUALHAND_SWORD, COMBO_TYPE_3, 6);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_DUALHAND_SWORD, COMBO_TYPE_3, COMBO_INDEX_1, pkMotionManager.NAME_COMBO_ATTACK_1);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_DUALHAND_SWORD, COMBO_TYPE_3, COMBO_INDEX_2, pkMotionManager.NAME_COMBO_ATTACK_2);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_DUALHAND_SWORD, COMBO_TYPE_3, COMBO_INDEX_3, pkMotionManager.NAME_COMBO_ATTACK_3);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_DUALHAND_SWORD, COMBO_TYPE_3, COMBO_INDEX_4, pkMotionManager.NAME_COMBO_ATTACK_5);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_DUALHAND_SWORD, COMBO_TYPE_3, COMBO_INDEX_5, pkMotionManager.NAME_COMBO_ATTACK_6);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_DUALHAND_SWORD, COMBO_TYPE_3, COMBO_INDEX_6, pkMotionManager.NAME_COMBO_ATTACK_8);

	std::vector<SMotion> m_vecMotionBow =
	{
		{ pkMotionManager.NAME_WAIT, "wait.msa", 70 },
		{ pkMotionManager.NAME_WAIT, "wait_1.msa", 30},
		{ pkMotionManager.NAME_WALK, "walk.msa", 0 },
		{ pkMotionManager.NAME_RUN, "run.msa", 0 },
		{ pkMotionManager.NAME_DAMAGE, "damage.msa", 50},
		{ pkMotionManager.NAME_DAMAGE, "damage_1.msa", 50},
		{ pkMotionManager.NAME_DAMAGE_BACK, "damage_2.msa", 50},
		{ pkMotionManager.NAME_DAMAGE_BACK, "damage_3.msa", 50},
		{ pkMotionManager.NAME_COMBO_ATTACK_1, "attack.msa", 0 },
	};

	pRaceData->RegisterMotionMode(pkMotionManager.MODE_BOW);
	for (const auto& it : m_vecMotionBow)
	{
		snprintf(szFileName, sizeof(szFileName), "%s%s%s", stFolder, "bow/", it.stName);
		auto pkMootionThing = pRaceData->RegisterMotionData(pkMotionManager.MODE_BOW, it.wMotionIndex, szFileName, it.byPercentage);

		if (pkMootionThing)
		{
			CResourceManager::Instance().LoadStaticCache(pkMootionThing->GetFileName());
		}
	}

	pRaceData->ReserveComboAttack(pkMotionManager.MODE_BOW, COMBO_TYPE_1, 1);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_BOW, COMBO_TYPE_1, COMBO_INDEX_1, pkMotionManager.NAME_COMBO_ATTACK_1);

	std::vector<SMotion> m_vecMotionFishing =
	{
		{ pkMotionManager.NAME_WAIT, "wait.msa", 0 },
		{ pkMotionManager.NAME_WALK, "walk.msa", 0 },
		{ pkMotionManager.NAME_RUN, "run.msa", 0 },
		{ pkMotionManager.NAME_FISHING_THROW, "throw.msa", 0 },
		{ pkMotionManager.NAME_FISHING_WAIT, "fishing_wait.msa", 0 },
		{ pkMotionManager.NAME_FISHING_STOP, "fishing_cancel.msa", 0 },
		{ pkMotionManager.NAME_FISHING_REACT, "fishing_react.msa", 0 },
		{ pkMotionManager.NAME_FISHING_CATCH, "fishing_catch.msa", 0 },
		{ pkMotionManager.NAME_FISHING_FAIL, "fishing_fail.msa", 0 },
	};

	pRaceData->RegisterMotionMode(pkMotionManager.MODE_FISHING);
	for (const auto& it : m_vecMotionFishing)
	{
		snprintf(szFileName, sizeof(szFileName), "%s%s%s", stFolder, "fishing/", it.stName);
		auto pkMootionThing = pRaceData->RegisterMotionData(pkMotionManager.MODE_FISHING, it.wMotionIndex, szFileName, it.byPercentage);

		if (pkMootionThing)
		{
			CResourceManager::Instance().LoadStaticCache(pkMootionThing->GetFileName());
		}
	}

	std::vector<SMotion> m_vecMotionHorse =
	{
		{ pkMotionManager.NAME_WAIT, "wait.msa", 90 },
		{ pkMotionManager.NAME_WAIT, "wait_1.msa", 9 },
		{ pkMotionManager.NAME_WAIT, "wait_2.msa", 1 },
		{ pkMotionManager.NAME_WALK, "walk.msa", 0 },
		{ pkMotionManager.NAME_RUN, "run.msa", 0 },
		{ pkMotionManager.NAME_DAMAGE, "damage.msa", 0 },
		{ pkMotionManager.NAME_DAMAGE_BACK, "damage.msa", 0 },
		{ pkMotionManager.NAME_DEAD, "dead.msa", 0 },
		{ HORSE_SKILL_CHARGE, "skill_charge.msa", 0 },
	};

	pRaceData->RegisterMotionMode(pkMotionManager.MODE_HORSE);
	for (const auto& it : m_vecMotionHorse)
	{
		snprintf(szFileName, sizeof(szFileName), "%s%s%s", stFolder, "horse/", it.stName);
		auto pkMootionThing = pRaceData->RegisterMotionData(pkMotionManager.MODE_HORSE, it.wMotionIndex, szFileName, it.byPercentage);

		if (pkMootionThing)
		{
			CResourceManager::Instance().LoadStaticCache(pkMootionThing->GetFileName());
		}
	}

	std::vector<SMotion> m_vecMotionHorseSword =
	{
		{ pkMotionManager.NAME_COMBO_ATTACK_1, "combo_01.msa", 0 },
		{ pkMotionManager.NAME_COMBO_ATTACK_2, "combo_02.msa", 0 },
		{ pkMotionManager.NAME_COMBO_ATTACK_3, "combo_03.msa", 0 },
		{ HORSE_SKILL_WILDATTACK, "skill_wildattack.msa", 0 },
		{ HORSE_SKILL_SPLASH, "skill_splash.msa", 0 },
	};

	pRaceData->RegisterMotionMode(pkMotionManager.MODE_HORSE_ONEHAND_SWORD);
	for (const auto& it : m_vecMotionHorseSword)
	{
		snprintf(szFileName, sizeof(szFileName), "%s%s%s", stFolder, "horse_onehand_sword/", it.stName);
		auto pkMootionThing = pRaceData->RegisterMotionData(pkMotionManager.MODE_HORSE_ONEHAND_SWORD, it.wMotionIndex, szFileName, it.byPercentage);

		if (pkMootionThing)
		{
			CResourceManager::Instance().LoadStaticCache(pkMootionThing->GetFileName());
		}
	}

	pRaceData->ReserveComboAttack(pkMotionManager.MODE_HORSE_ONEHAND_SWORD, COMBO_TYPE_1, 3);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_HORSE_ONEHAND_SWORD, COMBO_TYPE_1, COMBO_INDEX_1, pkMotionManager.NAME_COMBO_ATTACK_1);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_HORSE_ONEHAND_SWORD, COMBO_TYPE_1, COMBO_INDEX_2, pkMotionManager.NAME_COMBO_ATTACK_2);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_HORSE_ONEHAND_SWORD, COMBO_TYPE_1, COMBO_INDEX_3, pkMotionManager.NAME_COMBO_ATTACK_3);

	std::vector<SMotion> m_vecMotionHorseDagger =
	{
		{ pkMotionManager.NAME_COMBO_ATTACK_1, "combo_01.msa", 0 },
		{ pkMotionManager.NAME_COMBO_ATTACK_2, "combo_02.msa", 0 },
		{ pkMotionManager.NAME_COMBO_ATTACK_3, "combo_03.msa", 0 },
		{ HORSE_SKILL_WILDATTACK, "skill_wildattack.msa", 0 },
		{ HORSE_SKILL_SPLASH, "skill_splash.msa", 0 },
	};

	pRaceData->RegisterMotionMode(pkMotionManager.MODE_HORSE_DUALHAND_SWORD);
	for (const auto& it : m_vecMotionHorseDagger)
	{
		snprintf(szFileName, sizeof(szFileName), "%s%s%s", stFolder, "horse_dualhand_sword/", it.stName);
		auto pkMootionThing = pRaceData->RegisterMotionData(pkMotionManager.MODE_HORSE_DUALHAND_SWORD, it.wMotionIndex, szFileName, it.byPercentage);

		if (pkMootionThing)
		{
			CResourceManager::Instance().LoadStaticCache(pkMootionThing->GetFileName());
		}
	}

	pRaceData->ReserveComboAttack(pkMotionManager.MODE_HORSE_DUALHAND_SWORD, COMBO_TYPE_1, 3);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_HORSE_DUALHAND_SWORD, COMBO_TYPE_1, COMBO_INDEX_1, pkMotionManager.NAME_COMBO_ATTACK_1);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_HORSE_DUALHAND_SWORD, COMBO_TYPE_1, COMBO_INDEX_2, pkMotionManager.NAME_COMBO_ATTACK_2);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_HORSE_DUALHAND_SWORD, COMBO_TYPE_1, COMBO_INDEX_3, pkMotionManager.NAME_COMBO_ATTACK_3);

	std::vector<SMotion> m_vecMotionHorseBow =
	{
		{ pkMotionManager.NAME_WAIT, "wait.msa", 90 },
		{ pkMotionManager.NAME_WAIT, "wait_1.msa", 9 },
		{ pkMotionManager.NAME_WAIT, "wait_2.msa", 1 },
		{ pkMotionManager.NAME_RUN, "run.msa", 0 },
		{ pkMotionManager.NAME_DAMAGE, "damage.msa", 0 },
		{ pkMotionManager.NAME_DEAD, "dead.msa", 0 },
		{ pkMotionManager.NAME_COMBO_ATTACK_1, "attack.msa", 0 },
		{ HORSE_SKILL_WILDATTACK, "skill_wildattack.msa", 0 },
		{ HORSE_SKILL_SPLASH, "skill_splash.msa", 0 },
	};

	pRaceData->RegisterMotionMode(pkMotionManager.MODE_HORSE_BOW);
	for (const auto& it : m_vecMotionHorseBow)
	{
		snprintf(szFileName, sizeof(szFileName), "%s%s%s", stFolder, "horse_bow/", it.stName);
		auto pkMootionThing = pRaceData->RegisterMotionData(pkMotionManager.MODE_HORSE_BOW, it.wMotionIndex, szFileName, it.byPercentage);

		if (pkMootionThing)
		{
			CResourceManager::Instance().LoadStaticCache(pkMootionThing->GetFileName());
		}
	}

	pRaceData->ReserveComboAttack(pkMotionManager.MODE_HORSE_BOW, COMBO_TYPE_1, 3);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_HORSE_BOW, COMBO_TYPE_1, COMBO_INDEX_1, pkMotionManager.NAME_COMBO_ATTACK_1);

	pRaceData->RegisterAttachingBoneName(CRaceData::PART_WEAPON, "equip_right");
	pRaceData->RegisterAttachingBoneName(CRaceData::PART_WEAPON_LEFT, "equip_left");

#ifdef ENABLE_SHOULDER_SASH_SYSTEM
	pRaceData->RegisterAttachingBoneName(CRaceData::PART_SASH, "Bip01 Spine2");
#endif
	return true;
}

const bool CPythonPlayerSettingsModule::LoadGameSura(DWORD dwRace, char stFolder[FILE_MAX_NUM])
{
	CRaceManager& pkManager = CRaceManager::Instance();
	pkManager.SelectRace(dwRace);

	if (!LoadGeneralMotion(stFolder))
		return false;

	CRaceData* pRaceData = pkManager.GetSelectedRaceDataPointer();
	if (!pRaceData)
		return false;

	std::vector<SMotion> m_vecMotionGeneral =
	{
		{ pkMotionManager.NAME_COMBO_ATTACK_1, "attack.msa", 50 },
		{ pkMotionManager.NAME_COMBO_ATTACK_1, "attack_1.msa", 50 },
	};

	char szFileName[FILE_MAX_NUM];
	pRaceData->RegisterMotionMode(pkMotionManager.MODE_GENERAL);
	for (const auto& it : m_vecMotionGeneral)
	{
		snprintf(szFileName, sizeof(szFileName), "%s%s%s", stFolder, "general/", it.stName);
		auto pkMootionThing = pRaceData->RegisterMotionData(pkMotionManager.MODE_GENERAL, it.wMotionIndex, szFileName, it.byPercentage);

		if (pkMootionThing)
		{
			CResourceManager::Instance().LoadStaticCache(pkMootionThing->GetFileName());
		}
	}

	std::vector<SMotion> m_vecMotionSkill =
	{
		{ GUILD_SKILL_DRAGONBLOOD, "guild_yongsinuipi.msa", 0 },
		{ GUILD_SKILL_DRAGONBLESS, "guild_yongsinuichukbok.msa", 0 },
		{ GUILD_SKILL_BLESSARMOR, "guild_seonghwigap.msa", 0 },
		{ GUILD_SKILL_SPPEDUP, "guild_gasokhwa.msa", 0 },
		{ GUILD_SKILL_DRAGONWRATH, "guild_yongsinuibunno.msa", 0 },
		{ GUILD_SKILL_MAGICUP, "guild_jumunsul.msa", 0 },
	};

	std::vector<std::pair<BYTE, const char*>> m_vecSkillName = {
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

	char szSkillName[FILE_MAX_NUM];
#ifdef ENABLE_SKILLS_LEVEL_OVER_P
	char szSkillAdd[5] = "";
#else
	char szSkillAdd[4] = "";
#endif
	for (WORD i = 0; i < CPythonSkill::SKILL_EFFECT_COUNT; i++)
	{
		for (auto& it : m_vecSkillName)
		{
			SMotion motion;

			motion.wMotionIndex = pkMotionManager.NAME_SKILL + (i * CPythonSkill::SKILL_GRADEGAP) + it.first;
			motion.byPercentage = 0;

			if (i > 0)
				snprintf(szSkillAdd, sizeof(szSkillAdd), "_%d", i + 1);
			else
				snprintf(szSkillAdd, sizeof(szSkillAdd), "");

			snprintf(motion.stName, sizeof(motion.stName), "%s%s.msa", it.second, szSkillAdd);
			m_vecMotionSkill.push_back(motion);
		}
	}

	pRaceData->RegisterMotionMode(pkMotionManager.MODE_GENERAL);
	for (const auto& it : m_vecMotionSkill)
	{
		snprintf(szFileName, sizeof(szFileName), "%s%s%s", stFolder, "skill/", it.stName);
		auto pkMootionThing = pRaceData->RegisterMotionData(pkMotionManager.MODE_GENERAL, it.wMotionIndex, szFileName, it.byPercentage);

		if (pkMootionThing)
		{
			CResourceManager::Instance().LoadStaticCache(pkMootionThing->GetFileName());
		}
	}

	pRaceData->ReserveComboAttack(pkMotionManager.MODE_GENERAL, COMBO_TYPE_1, 1);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_GENERAL, COMBO_TYPE_1, COMBO_INDEX_1, pkMotionManager.NAME_COMBO_ATTACK_1);

	RegisterEmotionAnis(stFolder);

	std::vector<SMotion> m_vecMotionSword =
	{
		{ pkMotionManager.NAME_WAIT, "wait.msa", 0 },
		{ pkMotionManager.NAME_WALK, "walk.msa" , 0 },
		{ pkMotionManager.NAME_RUN, "run.msa" , 0 },
		{ pkMotionManager.NAME_DAMAGE, "damage.msa", 50 },
		{ pkMotionManager.NAME_DAMAGE, "damage_1.msa", 50 },
		{ pkMotionManager.NAME_DAMAGE_BACK, "damage_2.msa", 50 },
		{ pkMotionManager.NAME_DAMAGE_BACK, "damage_3.msa", 50 },
		{ pkMotionManager.NAME_COMBO_ATTACK_1, "combo_01.msa" , 0 },
		{ pkMotionManager.NAME_COMBO_ATTACK_2, "combo_02.msa" , 0 },
		{ pkMotionManager.NAME_COMBO_ATTACK_3, "combo_03.msa" , 0 },
		{ pkMotionManager.NAME_COMBO_ATTACK_4, "combo_04.msa" , 0 },
		{ pkMotionManager.NAME_COMBO_ATTACK_5, "combo_05.msa" , 0 },
		{ pkMotionManager.NAME_COMBO_ATTACK_6, "combo_06.msa" , 0 },
		{ pkMotionManager.NAME_COMBO_ATTACK_7, "combo_07.msa" , 0 },
	};

	pRaceData->RegisterMotionMode(pkMotionManager.MODE_ONEHAND_SWORD);
	for (const auto& it : m_vecMotionSword)
	{
		snprintf(szFileName, sizeof(szFileName), "%s%s%s", stFolder, "onehand_sword/", it.stName);
		auto pkMootionThing = pRaceData->RegisterMotionData(pkMotionManager.MODE_ONEHAND_SWORD, it.wMotionIndex, szFileName, it.byPercentage);

		if (pkMootionThing)
		{
			CResourceManager::Instance().LoadStaticCache(pkMootionThing->GetFileName());
		}
	}

	pRaceData->ReserveComboAttack(pkMotionManager.MODE_ONEHAND_SWORD, COMBO_TYPE_1, 4);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_ONEHAND_SWORD, COMBO_TYPE_1, COMBO_INDEX_1, pkMotionManager.NAME_COMBO_ATTACK_1);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_ONEHAND_SWORD, COMBO_TYPE_1, COMBO_INDEX_2, pkMotionManager.NAME_COMBO_ATTACK_2);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_ONEHAND_SWORD, COMBO_TYPE_1, COMBO_INDEX_3, pkMotionManager.NAME_COMBO_ATTACK_3);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_ONEHAND_SWORD, COMBO_TYPE_1, COMBO_INDEX_4, pkMotionManager.NAME_COMBO_ATTACK_4);

	pRaceData->ReserveComboAttack(pkMotionManager.MODE_ONEHAND_SWORD, COMBO_TYPE_2, 5);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_ONEHAND_SWORD, COMBO_TYPE_2, COMBO_INDEX_1, pkMotionManager.NAME_COMBO_ATTACK_1);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_ONEHAND_SWORD, COMBO_TYPE_2, COMBO_INDEX_2, pkMotionManager.NAME_COMBO_ATTACK_2);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_ONEHAND_SWORD, COMBO_TYPE_2, COMBO_INDEX_3, pkMotionManager.NAME_COMBO_ATTACK_3);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_ONEHAND_SWORD, COMBO_TYPE_2, COMBO_INDEX_4, pkMotionManager.NAME_COMBO_ATTACK_5);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_ONEHAND_SWORD, COMBO_TYPE_2, COMBO_INDEX_5, pkMotionManager.NAME_COMBO_ATTACK_7);

	pRaceData->ReserveComboAttack(pkMotionManager.MODE_ONEHAND_SWORD, COMBO_TYPE_3, 6);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_ONEHAND_SWORD, COMBO_TYPE_3, COMBO_INDEX_1, pkMotionManager.NAME_COMBO_ATTACK_1);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_ONEHAND_SWORD, COMBO_TYPE_3, COMBO_INDEX_2, pkMotionManager.NAME_COMBO_ATTACK_2);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_ONEHAND_SWORD, COMBO_TYPE_3, COMBO_INDEX_3, pkMotionManager.NAME_COMBO_ATTACK_3);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_ONEHAND_SWORD, COMBO_TYPE_3, COMBO_INDEX_4, pkMotionManager.NAME_COMBO_ATTACK_5);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_ONEHAND_SWORD, COMBO_TYPE_3, COMBO_INDEX_5, pkMotionManager.NAME_COMBO_ATTACK_6);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_ONEHAND_SWORD, COMBO_TYPE_3, COMBO_INDEX_6, pkMotionManager.NAME_COMBO_ATTACK_4);

	std::vector<SMotion> m_vecMotionFishing =
	{
		{ pkMotionManager.NAME_WAIT, "wait.msa", 0 },
		{ pkMotionManager.NAME_WALK, "walk.msa", 0 },
		{ pkMotionManager.NAME_RUN, "run.msa", 0 },
		{ pkMotionManager.NAME_FISHING_THROW, "throw.msa", 0 },
		{ pkMotionManager.NAME_FISHING_WAIT, "fishing_wait.msa", 0 },
		{ pkMotionManager.NAME_FISHING_STOP, "fishing_cancel.msa", 0 },
		{ pkMotionManager.NAME_FISHING_REACT, "fishing_react.msa", 0 },
		{ pkMotionManager.NAME_FISHING_CATCH, "fishing_catch.msa", 0 },
		{ pkMotionManager.NAME_FISHING_FAIL, "fishing_fail.msa", 0 },
	};

	pRaceData->RegisterMotionMode(pkMotionManager.MODE_FISHING);
	for (const auto& it : m_vecMotionFishing)
	{
		snprintf(szFileName, sizeof(szFileName), "%s%s%s", stFolder, "fishing/", it.stName);
		auto pkMootionThing = pRaceData->RegisterMotionData(pkMotionManager.MODE_FISHING, it.wMotionIndex, szFileName, it.byPercentage);

		if (pkMootionThing)
		{
			CResourceManager::Instance().LoadStaticCache(pkMootionThing->GetFileName());
		}
	}

	std::vector<SMotion> m_vecMotionHorse =
	{
		{ pkMotionManager.NAME_WAIT, "wait.msa", 90 },
		{ pkMotionManager.NAME_WAIT, "wait_1.msa", 9 },
		{ pkMotionManager.NAME_WAIT, "wait_2.msa", 1 },
		{ pkMotionManager.NAME_WALK, "walk.msa", 0 },
		{ pkMotionManager.NAME_RUN, "run.msa", 0 },
		{ pkMotionManager.NAME_DAMAGE, "damage.msa", 0 },
		{ pkMotionManager.NAME_DAMAGE_BACK, "damage.msa", 0 },
		{ pkMotionManager.NAME_DEAD, "dead.msa", 0 },
		{ HORSE_SKILL_CHARGE, "skill_charge.msa", 0 },
		{ HORSE_SKILL_CHARGE, "skill_splash.msa", 0 },
	};

	pRaceData->RegisterMotionMode(pkMotionManager.MODE_HORSE);
	for (const auto& it : m_vecMotionHorse)
	{
		snprintf(szFileName, sizeof(szFileName), "%s%s%s", stFolder, "horse/", it.stName);
		auto pkMootionThing = pRaceData->RegisterMotionData(pkMotionManager.MODE_HORSE, it.wMotionIndex, szFileName, it.byPercentage);

		if (pkMootionThing)
		{
			CResourceManager::Instance().LoadStaticCache(pkMootionThing->GetFileName());
		}
	}

	std::vector<SMotion> m_vecMotionHorseSword =
	{
		{ pkMotionManager.NAME_COMBO_ATTACK_1, "combo_01.msa", 0 },
		{ pkMotionManager.NAME_COMBO_ATTACK_2, "combo_02.msa", 0 },
		{ pkMotionManager.NAME_COMBO_ATTACK_3, "combo_03.msa", 0 },
		{ HORSE_SKILL_WILDATTACK, "skill_wildattack.msa", 0 },
	};

	pRaceData->RegisterMotionMode(pkMotionManager.MODE_HORSE_ONEHAND_SWORD);
	for (const auto& it : m_vecMotionHorseSword)
	{
		snprintf(szFileName, sizeof(szFileName), "%s%s%s", stFolder, "horse_onehand_sword/", it.stName);
		auto pkMootionThing = pRaceData->RegisterMotionData(pkMotionManager.MODE_HORSE_ONEHAND_SWORD, it.wMotionIndex, szFileName, it.byPercentage);

		if (pkMootionThing)
		{
			CResourceManager::Instance().LoadStaticCache(pkMootionThing->GetFileName());
		}
	}

	pRaceData->ReserveComboAttack(pkMotionManager.MODE_HORSE_ONEHAND_SWORD, COMBO_TYPE_1, 3);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_HORSE_ONEHAND_SWORD, COMBO_TYPE_1, COMBO_INDEX_1, pkMotionManager.NAME_COMBO_ATTACK_1);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_HORSE_ONEHAND_SWORD, COMBO_TYPE_1, COMBO_INDEX_2, pkMotionManager.NAME_COMBO_ATTACK_2);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_HORSE_ONEHAND_SWORD, COMBO_TYPE_1, COMBO_INDEX_3, pkMotionManager.NAME_COMBO_ATTACK_3);

	pRaceData->RegisterAttachingBoneName(CRaceData::PART_WEAPON, "equip_right");

#ifdef ENABLE_SHOULDER_SASH_SYSTEM
	pRaceData->RegisterAttachingBoneName(CRaceData::PART_SASH, "Bip01 Spine2");
#endif

	return true;
}

const bool CPythonPlayerSettingsModule::LoadGameShaman(DWORD dwRace, char stFolder[FILE_MAX_NUM])
{
	CRaceManager& pkManager = CRaceManager::Instance();
	pkManager.SelectRace(dwRace);

	if (!LoadGeneralMotion(stFolder))
		return false;

	CRaceData* pRaceData = pkManager.GetSelectedRaceDataPointer();
	if (!pRaceData)
		return false;

	std::vector<SMotion> m_vecMotionGeneral =
	{
		{ pkMotionManager.NAME_COMBO_ATTACK_1, "attack.msa", 50 },
		{ pkMotionManager.NAME_COMBO_ATTACK_1, "attack_1.msa", 50 },
	};

	char szFileName[FILE_MAX_NUM];
	pRaceData->RegisterMotionMode(pkMotionManager.MODE_GENERAL);
	for (const auto& it : m_vecMotionGeneral)
	{
		snprintf(szFileName, sizeof(szFileName), "%s%s%s", stFolder, "general/", it.stName);
		auto pkMootionThing = pRaceData->RegisterMotionData(pkMotionManager.MODE_GENERAL, it.wMotionIndex, szFileName, it.byPercentage);

		if (pkMootionThing)
		{
			CResourceManager::Instance().LoadStaticCache(pkMootionThing->GetFileName());
		}
	}

	std::vector<SMotion> m_vecMotionSkill =
	{
		{ pkMotionManager.NAME_SKILL + 1, "bipabu.msa" },
		{ pkMotionManager.NAME_SKILL + 2, "yongpa.msa" },
		{ pkMotionManager.NAME_SKILL + 3, "paeryong.msa" },
		{ pkMotionManager.NAME_SKILL + 4, "hosin_target.msa" },
		{ pkMotionManager.NAME_SKILL + 5, "boho_target.msa" },
		{ pkMotionManager.NAME_SKILL + 6, "gicheon_target.msa" },
		{ pkMotionManager.NAME_SKILL + 16, "noejeon.msa" },
		{ pkMotionManager.NAME_SKILL + 17, "byeorak.msa" },
		{ pkMotionManager.NAME_SKILL + 18, "pokroe.msa" },
		{ pkMotionManager.NAME_SKILL + 19, "jeongeop_target.msa" },
		{ pkMotionManager.NAME_SKILL + 20, "kwaesok_target.msa" },
		{ pkMotionManager.NAME_SKILL + 21, "jeungryeok_target.msa" },
		{ GUILD_SKILL_DRAGONBLOOD, "guild_yongsinuipi.msa", 0 },
		{ GUILD_SKILL_DRAGONBLESS, "guild_yongsinuichukbok.msa", 0 },
		{ GUILD_SKILL_BLESSARMOR, "guild_seonghwigap.msa", 0 },
		{ GUILD_SKILL_SPPEDUP, "guild_gasokhwa.msa", 0 },
		{ GUILD_SKILL_DRAGONWRATH, "guild_yongsinuibunno.msa", 0 },
		{ GUILD_SKILL_MAGICUP, "guild_jumunsul.msa", 0 },
	};

	std::vector<std::pair<BYTE, const char*>> m_vecSkillName = {
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

	char szSkillName[FILE_MAX_NUM];
#ifdef ENABLE_SKILLS_LEVEL_OVER_P
	char szSkillAdd[5] = "";
#else
	char szSkillAdd[4] = "";
#endif
	for (WORD i = 1; i < CPythonSkill::SKILL_EFFECT_COUNT; i++)
	{
		for (auto& it : m_vecSkillName)
		{
			SMotion motion;

			motion.wMotionIndex = pkMotionManager.NAME_SKILL + (i * CPythonSkill::SKILL_GRADEGAP) + it.first;
			motion.byPercentage = 0;

			snprintf(szSkillAdd, sizeof(szSkillAdd), "_%d", i + 1);
			snprintf(motion.stName, sizeof(motion.stName), "%s%s.msa", it.second, szSkillAdd);
			m_vecMotionSkill.push_back(motion);
		}
	}

	pRaceData->RegisterMotionMode(pkMotionManager.MODE_GENERAL);
	for (const auto& it : m_vecMotionSkill)
	{
		snprintf(szFileName, sizeof(szFileName), "%s%s%s", stFolder, "skill/", it.stName);
		auto pkMootionThing = pRaceData->RegisterMotionData(pkMotionManager.MODE_GENERAL, it.wMotionIndex, szFileName, it.byPercentage);

		if (pkMootionThing)
		{
			CResourceManager::Instance().LoadStaticCache(pkMootionThing->GetFileName());
		}
	}

	pRaceData->ReserveComboAttack(pkMotionManager.MODE_GENERAL, COMBO_TYPE_1, 1);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_GENERAL, COMBO_TYPE_1, COMBO_INDEX_1, pkMotionManager.NAME_COMBO_ATTACK_1);

	RegisterEmotionAnis(stFolder);

	std::vector<SMotion> m_vecMotionFan =
	{
		{ pkMotionManager.NAME_WAIT, "wait.msa", 0 },
		{ pkMotionManager.NAME_WALK, "walk.msa", 0 },
		{ pkMotionManager.NAME_RUN, "run.msa", 0 },
		{ pkMotionManager.NAME_DAMAGE, "damage.msa", 50,},
		{ pkMotionManager.NAME_DAMAGE, "damage_1.msa", 50, },
		{ pkMotionManager.NAME_DAMAGE_BACK, "damage_2.msa", 50,},
		{ pkMotionManager.NAME_DAMAGE_BACK, "damage_3.msa", 50, },
		{ pkMotionManager.NAME_COMBO_ATTACK_1, "combo_01.msa", 0 },
		{ pkMotionManager.NAME_COMBO_ATTACK_2, "combo_02.msa", 0 },
		{ pkMotionManager.NAME_COMBO_ATTACK_3, "combo_03.msa", 0 },
		{ pkMotionManager.NAME_COMBO_ATTACK_4, "combo_04.msa", 0 },
		{ pkMotionManager.NAME_COMBO_ATTACK_5, "combo_05.msa", 0 },
		{ pkMotionManager.NAME_COMBO_ATTACK_6, "combo_06.msa", 0 },
		{ pkMotionManager.NAME_COMBO_ATTACK_7, "combo_07.msa", 0 },
	};

	pRaceData->RegisterMotionMode(pkMotionManager.MODE_FAN);
	for (const auto& it : m_vecMotionFan)
	{
		snprintf(szFileName, sizeof(szFileName), "%s%s%s", stFolder, "fan/", it.stName);
		auto pkMootionThing = pRaceData->RegisterMotionData(pkMotionManager.MODE_FAN, it.wMotionIndex, szFileName, it.byPercentage);

		if (pkMootionThing)
		{
			CResourceManager::Instance().LoadStaticCache(pkMootionThing->GetFileName());
		}
	}

	pRaceData->ReserveComboAttack(pkMotionManager.MODE_FAN, COMBO_TYPE_1, 4);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_FAN, COMBO_TYPE_1, COMBO_INDEX_1, pkMotionManager.NAME_COMBO_ATTACK_1);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_FAN, COMBO_TYPE_1, COMBO_INDEX_2, pkMotionManager.NAME_COMBO_ATTACK_2);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_FAN, COMBO_TYPE_1, COMBO_INDEX_3, pkMotionManager.NAME_COMBO_ATTACK_3);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_FAN, COMBO_TYPE_1, COMBO_INDEX_4, pkMotionManager.NAME_COMBO_ATTACK_4);

	pRaceData->ReserveComboAttack(pkMotionManager.MODE_FAN, COMBO_TYPE_2, 5);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_FAN, COMBO_TYPE_2, COMBO_INDEX_1, pkMotionManager.NAME_COMBO_ATTACK_1);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_FAN, COMBO_TYPE_2, COMBO_INDEX_2, pkMotionManager.NAME_COMBO_ATTACK_2);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_FAN, COMBO_TYPE_2, COMBO_INDEX_3, pkMotionManager.NAME_COMBO_ATTACK_3);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_FAN, COMBO_TYPE_2, COMBO_INDEX_4, pkMotionManager.NAME_COMBO_ATTACK_5);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_FAN, COMBO_TYPE_2, COMBO_INDEX_5, pkMotionManager.NAME_COMBO_ATTACK_7);

	pRaceData->ReserveComboAttack(pkMotionManager.MODE_FAN, COMBO_TYPE_3, 6);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_FAN, COMBO_TYPE_3, COMBO_INDEX_1, pkMotionManager.NAME_COMBO_ATTACK_1);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_FAN, COMBO_TYPE_3, COMBO_INDEX_2, pkMotionManager.NAME_COMBO_ATTACK_2);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_FAN, COMBO_TYPE_3, COMBO_INDEX_3, pkMotionManager.NAME_COMBO_ATTACK_3);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_FAN, COMBO_TYPE_3, COMBO_INDEX_4, pkMotionManager.NAME_COMBO_ATTACK_5);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_FAN, COMBO_TYPE_3, COMBO_INDEX_5, pkMotionManager.NAME_COMBO_ATTACK_6);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_FAN, COMBO_TYPE_3, COMBO_INDEX_6, pkMotionManager.NAME_COMBO_ATTACK_4);

	std::vector<SMotion> m_vecMotionBell =
	{
		{ pkMotionManager.NAME_WAIT, "wait.msa", 0 },
		{ pkMotionManager.NAME_WALK, "walk.msa", 0 },
		{ pkMotionManager.NAME_RUN, "run.msa", 0 },
		{ pkMotionManager.NAME_DAMAGE, "damage.msa", 50,},
		{ pkMotionManager.NAME_DAMAGE, "damage_1.msa", 50, },
		{ pkMotionManager.NAME_DAMAGE_BACK, "damage_2.msa", 50,},
		{ pkMotionManager.NAME_DAMAGE_BACK, "damage_3.msa", 50, },
		{ pkMotionManager.NAME_COMBO_ATTACK_1, "combo_01.msa", 0 },
		{ pkMotionManager.NAME_COMBO_ATTACK_2, "combo_02.msa", 0 },
		{ pkMotionManager.NAME_COMBO_ATTACK_3, "combo_03.msa", 0 },
		{ pkMotionManager.NAME_COMBO_ATTACK_4, "combo_04.msa", 0 },
		{ pkMotionManager.NAME_COMBO_ATTACK_5, "combo_05.msa", 0 },
		{ pkMotionManager.NAME_COMBO_ATTACK_6, "combo_06.msa", 0 },
		{ pkMotionManager.NAME_COMBO_ATTACK_7, "combo_07.msa", 0 },
	};

	pRaceData->RegisterMotionMode(pkMotionManager.MODE_BELL);
	for (const auto& it : m_vecMotionBell)
	{
		snprintf(szFileName, sizeof(szFileName), "%s%s%s", stFolder, "bell/", it.stName);
		auto pkMootionThing = pRaceData->RegisterMotionData(pkMotionManager.MODE_BELL, it.wMotionIndex, szFileName, it.byPercentage);

		if (pkMootionThing)
		{
			CResourceManager::Instance().LoadStaticCache(pkMootionThing->GetFileName());
		}
	}

	pRaceData->ReserveComboAttack(pkMotionManager.MODE_BELL, COMBO_TYPE_1, 4);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_BELL, COMBO_TYPE_1, COMBO_INDEX_1, pkMotionManager.NAME_COMBO_ATTACK_1);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_BELL, COMBO_TYPE_1, COMBO_INDEX_2, pkMotionManager.NAME_COMBO_ATTACK_2);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_BELL, COMBO_TYPE_1, COMBO_INDEX_3, pkMotionManager.NAME_COMBO_ATTACK_3);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_BELL, COMBO_TYPE_1, COMBO_INDEX_4, pkMotionManager.NAME_COMBO_ATTACK_4);

	pRaceData->ReserveComboAttack(pkMotionManager.MODE_BELL, COMBO_TYPE_2, 5);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_BELL, COMBO_TYPE_2, COMBO_INDEX_1, pkMotionManager.NAME_COMBO_ATTACK_1);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_BELL, COMBO_TYPE_2, COMBO_INDEX_2, pkMotionManager.NAME_COMBO_ATTACK_2);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_BELL, COMBO_TYPE_2, COMBO_INDEX_3, pkMotionManager.NAME_COMBO_ATTACK_3);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_BELL, COMBO_TYPE_2, COMBO_INDEX_4, pkMotionManager.NAME_COMBO_ATTACK_5);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_BELL, COMBO_TYPE_2, COMBO_INDEX_5, pkMotionManager.NAME_COMBO_ATTACK_7);

	pRaceData->ReserveComboAttack(pkMotionManager.MODE_BELL, COMBO_TYPE_3, 6);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_BELL, COMBO_TYPE_3, COMBO_INDEX_1, pkMotionManager.NAME_COMBO_ATTACK_1);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_BELL, COMBO_TYPE_3, COMBO_INDEX_2, pkMotionManager.NAME_COMBO_ATTACK_2);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_BELL, COMBO_TYPE_3, COMBO_INDEX_3, pkMotionManager.NAME_COMBO_ATTACK_3);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_BELL, COMBO_TYPE_3, COMBO_INDEX_4, pkMotionManager.NAME_COMBO_ATTACK_5);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_BELL, COMBO_TYPE_3, COMBO_INDEX_5, pkMotionManager.NAME_COMBO_ATTACK_6);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_BELL, COMBO_TYPE_3, COMBO_INDEX_6, pkMotionManager.NAME_COMBO_ATTACK_4);

	std::vector<SMotion> m_vecMotionFishing =
	{
		{ pkMotionManager.NAME_WAIT, "wait.msa", 0 },
		{ pkMotionManager.NAME_WALK, "walk.msa", 0 },
		{ pkMotionManager.NAME_RUN, "run.msa", 0 },
		{ pkMotionManager.NAME_FISHING_THROW, "throw.msa", 0 },
		{ pkMotionManager.NAME_FISHING_WAIT, "fishing_wait.msa", 0 },
		{ pkMotionManager.NAME_FISHING_STOP, "fishing_cancel.msa", 0 },
		{ pkMotionManager.NAME_FISHING_REACT, "fishing_react.msa", 0 },
		{ pkMotionManager.NAME_FISHING_CATCH, "fishing_catch.msa", 0 },
		{ pkMotionManager.NAME_FISHING_FAIL, "fishing_fail.msa", 0 },
	};

	pRaceData->RegisterMotionMode(pkMotionManager.MODE_FISHING);
	for (const auto& it : m_vecMotionFishing)
	{
		snprintf(szFileName, sizeof(szFileName), "%s%s%s", stFolder, "fishing/", it.stName);
		auto pkMootionThing = pRaceData->RegisterMotionData(pkMotionManager.MODE_FISHING, it.wMotionIndex, szFileName, it.byPercentage);

		if (pkMootionThing)
		{
			CResourceManager::Instance().LoadStaticCache(pkMootionThing->GetFileName());
		}
	}

	std::vector<SMotion> m_vecMotionHorse =
	{
		{ pkMotionManager.NAME_WAIT, "wait.msa", 90 },
		{ pkMotionManager.NAME_WAIT, "wait_1.msa", 9 },
		{ pkMotionManager.NAME_WAIT, "wait_2.msa", 1 },
		{ pkMotionManager.NAME_WALK, "walk.msa", 0 },
		{ pkMotionManager.NAME_RUN, "run.msa", 0 },
		{ pkMotionManager.NAME_DAMAGE, "damage.msa", 0 },
		{ pkMotionManager.NAME_DAMAGE_BACK, "damage.msa", 0 },
		{ pkMotionManager.NAME_DEAD, "dead.msa", 0 },
		{ HORSE_SKILL_CHARGE, "skill_charge.msa", 0 },
		{ HORSE_SKILL_CHARGE, "skill_splash.msa", 0 },
	};

	pRaceData->RegisterMotionMode(pkMotionManager.MODE_HORSE);
	for (const auto& it : m_vecMotionHorse)
	{
		snprintf(szFileName, sizeof(szFileName), "%s%s%s", stFolder, "horse/", it.stName);
		auto pkMootionThing = pRaceData->RegisterMotionData(pkMotionManager.MODE_HORSE, it.wMotionIndex, szFileName, it.byPercentage);

		if (pkMootionThing)
		{
			CResourceManager::Instance().LoadStaticCache(pkMootionThing->GetFileName());
		}
	}

	std::vector<SMotion> m_vecMotionHorseFan =
	{
		{ pkMotionManager.NAME_COMBO_ATTACK_1, "combo_01.msa", 0 },
		{ pkMotionManager.NAME_COMBO_ATTACK_2, "combo_02.msa", 0 },
		{ pkMotionManager.NAME_COMBO_ATTACK_3, "combo_03.msa", 0 },
		{ HORSE_SKILL_WILDATTACK, "skill_wildattack.msa", 0 },
	};

	pRaceData->RegisterMotionMode(pkMotionManager.MODE_HORSE_FAN);
	for (const auto& it : m_vecMotionHorseFan)
	{
		snprintf(szFileName, sizeof(szFileName), "%s%s%s", stFolder, "horse_fan/", it.stName);
		auto pkMootionThing = pRaceData->RegisterMotionData(pkMotionManager.MODE_HORSE_FAN, it.wMotionIndex, szFileName, it.byPercentage);

		if (pkMootionThing)
		{
			CResourceManager::Instance().LoadStaticCache(pkMootionThing->GetFileName());
		}
	}

	pRaceData->ReserveComboAttack(pkMotionManager.MODE_HORSE_FAN, COMBO_TYPE_1, 3);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_HORSE_FAN, COMBO_TYPE_1, COMBO_INDEX_1, pkMotionManager.NAME_COMBO_ATTACK_1);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_HORSE_FAN, COMBO_TYPE_1, COMBO_INDEX_2, pkMotionManager.NAME_COMBO_ATTACK_2);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_HORSE_FAN, COMBO_TYPE_1, COMBO_INDEX_3, pkMotionManager.NAME_COMBO_ATTACK_3);

	std::vector<SMotion> m_vecMotionHorseBell =
	{
		{ pkMotionManager.NAME_COMBO_ATTACK_1, "combo_01.msa", 0 },
		{ pkMotionManager.NAME_COMBO_ATTACK_2, "combo_02.msa", 0 },
		{ pkMotionManager.NAME_COMBO_ATTACK_3, "combo_03.msa", 0 },
		{ HORSE_SKILL_WILDATTACK, "skill_wildattack.msa", 0 },
	};

	pRaceData->RegisterMotionMode(pkMotionManager.MODE_HORSE_BELL);
	for (const auto& it : m_vecMotionHorseBell)
	{
		snprintf(szFileName, sizeof(szFileName), "%s%s%s", stFolder, "horse_bell/", it.stName);
		auto pkMootionThing = pRaceData->RegisterMotionData(pkMotionManager.MODE_HORSE_BELL, it.wMotionIndex, szFileName, it.byPercentage);

		if (pkMootionThing)
		{
			CResourceManager::Instance().LoadStaticCache(pkMootionThing->GetFileName());
		}
	}

	pRaceData->ReserveComboAttack(pkMotionManager.MODE_HORSE_BELL, COMBO_TYPE_1, 3);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_HORSE_BELL, COMBO_TYPE_1, COMBO_INDEX_1, pkMotionManager.NAME_COMBO_ATTACK_1);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_HORSE_BELL, COMBO_TYPE_1, COMBO_INDEX_2, pkMotionManager.NAME_COMBO_ATTACK_2);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_HORSE_BELL, COMBO_TYPE_1, COMBO_INDEX_3, pkMotionManager.NAME_COMBO_ATTACK_3);

	pRaceData->RegisterAttachingBoneName(CRaceData::PART_WEAPON, "equip_right");
	pRaceData->RegisterAttachingBoneName(CRaceData::PART_WEAPON_LEFT, "equip_left");

#ifdef ENABLE_SHOULDER_SASH_SYSTEM
	pRaceData->RegisterAttachingBoneName(CRaceData::PART_SASH, "Bip01 Spine2");
#endif

	return true;
}

#ifdef ENABLE_SKILL_COSTUME_SYSTEM

void CPythonPlayerSettingsModule::LoadNewSkills()
{
	// You need to define SKILL_COSTUME_EFFECT_COUNT somewhere in your defines
	for (int x = 1; x <= SKILL_COSTUME_EFFECT_COUNT; x++)
	{
		// Warrior
		LoadNewWarriorSkillsEx(RACE_WARRIOR_M, "d:/ymir work/pc/warrior/", x);
		LoadNewWarriorSkillsEx(RACE_WARRIOR_W, "d:/ymir work/pc2/warrior/", x);

		// Assassin
		LoadNewAssassinSkillsEx(RACE_ASSASSIN_W, "d:/ymir work/pc/assassin/", x);
		LoadNewAssassinSkillsEx(RACE_ASSASSIN_M, "d:/ymir work/pc2/assassin/", x);

		// Sura
		LoadNewSuraSkillsEx(RACE_SURA_M, "d:/ymir work/pc/sura/", x);
		LoadNewSuraSkillsEx(RACE_SURA_W, "d:/ymir work/pc2/sura/", x);

		// Shaman
		LoadNewShamanSkillsEx(RACE_SHAMAN_W, "d:/ymir work/pc/shaman/", x);
		LoadNewShamanSkillsEx(RACE_SHAMAN_M, "d:/ymir work/pc2/shaman/", x);

		// Loop Data
		RegisterLoopData(x);
	}
}

void CPythonPlayerSettingsModule::RegisterLoopData(int loadSkillCount)
{
	char szEndString[16];
	snprintf(szEndString, sizeof(szEndString), "_%d", loadSkillCount);

	char szFileName[FILE_MAX_NUM];

	// Register loop effects for each costume variant
	// You'll need to define these constants: SKILL_COSTUME_EFFECT_START, EFFECT_AFFECT, EFFECT_WEAPON

	// Warrior effects
	snprintf(szFileName, sizeof(szFileName), "d:/ymir work/pc/warrior/skill%s/gyeokgongjang_loop.mse", szEndString);
	CInstanceBase::RegisterEffect(CInstanceBase::SKILL_COSTUME_EFFECT_START + CInstanceBase::EFFECT_AFFECT + 16 + (100 * (loadSkillCount - 1)), "", szFileName, true);

	// Assassin effects
	snprintf(szFileName, sizeof(szFileName), "d:/ymir work/pc/assassin/skill%s/gyeonggong_loop.mse", szEndString);
	CInstanceBase::RegisterEffect(CInstanceBase::SKILL_COSTUME_EFFECT_START + CInstanceBase::EFFECT_AFFECT + 17 + (100 * (loadSkillCount - 1)), "", szFileName, true);

	// Sura effects
	snprintf(szFileName, sizeof(szFileName), "d:/ymir work/pc/sura/skill%s/gwigeom_loop.mse", szEndString);
	CInstanceBase::RegisterEffect(CInstanceBase::SKILL_COSTUME_EFFECT_START + CInstanceBase::EFFECT_AFFECT + 19 + (100 * (loadSkillCount - 1)), "Bip01 R Finger2", szFileName, true);

	snprintf(szFileName, sizeof(szFileName), "d:/ymir work/pc/sura/skill%s/fear_loop.mse", szEndString);
	CInstanceBase::RegisterEffect(CInstanceBase::SKILL_COSTUME_EFFECT_START + CInstanceBase::EFFECT_AFFECT + 20 + (100 * (loadSkillCount - 1)), "", szFileName, true);

	snprintf(szFileName, sizeof(szFileName), "d:/ymir work/pc/sura/skill%s/jumagap_loop.mse", szEndString);
	CInstanceBase::RegisterEffect(CInstanceBase::SKILL_COSTUME_EFFECT_START + CInstanceBase::EFFECT_AFFECT + 21 + (100 * (loadSkillCount - 1)), "", szFileName, true);

	snprintf(szFileName, sizeof(szFileName), "d:/ymir work/pc/sura/skill%s/heuksin_loop.mse", szEndString);
	CInstanceBase::RegisterEffect(CInstanceBase::SKILL_COSTUME_EFFECT_START + CInstanceBase::EFFECT_AFFECT + 25 + (100 * (loadSkillCount - 1)), "", szFileName, true);

	snprintf(szFileName, sizeof(szFileName), "d:/ymir work/pc/sura/skill%s/muyeong_loop.mse", szEndString);
	CInstanceBase::RegisterEffect(CInstanceBase::SKILL_COSTUME_EFFECT_START + CInstanceBase::EFFECT_AFFECT + 26 + (100 * (loadSkillCount - 1)), "", szFileName, true);

	snprintf(szFileName, sizeof(szFileName), "d:/ymir work/pc/sura/skill%s/pabeop_loop.mse", szEndString);
	CInstanceBase::RegisterEffect(CInstanceBase::SKILL_COSTUME_EFFECT_START + CInstanceBase::EFFECT_AFFECT + 32 + (100 * (loadSkillCount - 1)), "Bip01 Head", szFileName, true);

	// Shaman effects
	snprintf(szFileName, sizeof(szFileName), "d:/ymir work/pc/shaman/skill%s/3hosin_loop.mse", szEndString);
	CInstanceBase::RegisterEffect(CInstanceBase::SKILL_COSTUME_EFFECT_START + CInstanceBase::EFFECT_AFFECT + 22 + (100 * (loadSkillCount - 1)), "", szFileName, true);

	snprintf(szFileName, sizeof(szFileName), "d:/ymir work/pc/shaman/skill%s/boho_loop.mse", szEndString);
	CInstanceBase::RegisterEffect(CInstanceBase::SKILL_COSTUME_EFFECT_START + CInstanceBase::EFFECT_AFFECT + 23 + (100 * (loadSkillCount - 1)), "", szFileName, true);

	snprintf(szFileName, sizeof(szFileName), "d:/ymir work/pc/shaman/skill%s/10kwaesok_loop.mse", szEndString);
	CInstanceBase::RegisterEffect(CInstanceBase::SKILL_COSTUME_EFFECT_START + CInstanceBase::EFFECT_AFFECT + 24 + (100 * (loadSkillCount - 1)), "", szFileName, true);

	snprintf(szFileName, sizeof(szFileName), "d:/ymir work/pc/shaman/skill%s/6gicheon_hand.mse", szEndString);
	CInstanceBase::RegisterEffect(CInstanceBase::SKILL_COSTUME_EFFECT_START + CInstanceBase::EFFECT_AFFECT + 29 + (100 * (loadSkillCount - 1)), "Bip01 R Hand", szFileName, true);

	snprintf(szFileName, sizeof(szFileName), "d:/ymir work/pc/shaman/skill%s/jeungryeok_hand.mse", szEndString);
	CInstanceBase::RegisterEffect(CInstanceBase::SKILL_COSTUME_EFFECT_START + CInstanceBase::EFFECT_AFFECT + 30 + (100 * (loadSkillCount - 1)), "Bip01 L Hand", szFileName, true);

	// Additional effects
	snprintf(szFileName, sizeof(szFileName), "d:/ymir work/skill%s/hit/blow_flame/flame_loop.mse", szEndString);
	CInstanceBase::RegisterEffect(CInstanceBase::SKILL_COSTUME_EFFECT_START + CInstanceBase::EFFECT_AFFECT + 28 + (100 * (loadSkillCount - 1)), "Bip01", szFileName, true);

	snprintf(szFileName, sizeof(szFileName), "d:/ymir work/pc/warrior/skill%s/gyeokgongjang_loop.mse", szEndString);
	CInstanceBase::RegisterEffect(CInstanceBase::SKILL_COSTUME_EFFECT_START + CInstanceBase::EFFECT_AFFECT + 33 + (100 * (loadSkillCount - 1)), "", szFileName, true);

	// Weapon effects
	snprintf(szFileName, sizeof(szFileName), "d:/ymir work/pc/warrior/skill%s/geom_sword_loop.mse", szEndString);
	CInstanceBase::RegisterEffect(CInstanceBase::SKILL_COSTUME_EFFECT_START + CInstanceBase::EFFECT_WEAPON + 1 + (100 * (loadSkillCount - 1)), "equip_right_hand", szFileName, true);

	snprintf(szFileName, sizeof(szFileName), "d:/ymir work/pc/warrior/skill%s/geom_spear_loop.mse", szEndString);
	CInstanceBase::RegisterEffect(CInstanceBase::SKILL_COSTUME_EFFECT_START + CInstanceBase::EFFECT_WEAPON + 2 + (100 * (loadSkillCount - 1)), "equip_right_hand", szFileName, true);
}

void CPythonPlayerSettingsModule::LoadNewWarriorSkillsEx(DWORD dwRace, const std::string& path, int loadSkillCount)
{
	CRaceManager& pkManager = CRaceManager::Instance();
	pkManager.SelectRace(dwRace);

	CRaceData* pRaceData = pkManager.GetSelectedRaceDataPointer();
	if (!pRaceData)
		return;

	char szEndString[16];
	snprintf(szEndString, sizeof(szEndString), "_%d/", loadSkillCount);

	char szFileName[FILE_MAX_NUM];
	std::string skillPath = path + "skill" + szEndString;

	// Skill motions for warrior costume variants
	std::vector<std::pair<DWORD, const char*>> warriorSkills = {
		{ pkMotionManager.NAME_SKILL + (3 * CPythonSkill::SKILL_GRADEGAP) + 1 + CInstanceBase::SKILL_COSTUME_EFFECT_END + (100 * (loadSkillCount - 1)), "samyeon_4.msa" },
		{ pkMotionManager.NAME_SKILL + (3 * CPythonSkill::SKILL_GRADEGAP) + 2 + CInstanceBase::SKILL_COSTUME_EFFECT_END + (100 * (loadSkillCount - 1)), "palbang_4.msa" },
		{ pkMotionManager.NAME_SKILL + (3 * CPythonSkill::SKILL_GRADEGAP) + 3 + CInstanceBase::SKILL_COSTUME_EFFECT_END + (100 * (loadSkillCount - 1)), "jeongwi_4.msa" },
		{ pkMotionManager.NAME_SKILL + (3 * CPythonSkill::SKILL_GRADEGAP) + 4 + CInstanceBase::SKILL_COSTUME_EFFECT_END + (100 * (loadSkillCount - 1)), "geomgyeong_4.msa" },
		{ pkMotionManager.NAME_SKILL + (3 * CPythonSkill::SKILL_GRADEGAP) + 5 + CInstanceBase::SKILL_COSTUME_EFFECT_END + (100 * (loadSkillCount - 1)), "tanhwan_4.msa" },
		{ pkMotionManager.NAME_SKILL + (3 * CPythonSkill::SKILL_GRADEGAP) + 16 + CInstanceBase::SKILL_COSTUME_EFFECT_END + (100 * (loadSkillCount - 1)), "gigongcham_4.msa" },
		{ pkMotionManager.NAME_SKILL + (3 * CPythonSkill::SKILL_GRADEGAP) + 17 + CInstanceBase::SKILL_COSTUME_EFFECT_END + (100 * (loadSkillCount - 1)), "gyeoksan_4.msa" },
		{ pkMotionManager.NAME_SKILL + (3 * CPythonSkill::SKILL_GRADEGAP) + 18 + CInstanceBase::SKILL_COSTUME_EFFECT_END + (100 * (loadSkillCount - 1)), "daejin_4.msa" },
		{ pkMotionManager.NAME_SKILL + (3 * CPythonSkill::SKILL_GRADEGAP) + 19 + CInstanceBase::SKILL_COSTUME_EFFECT_END + (100 * (loadSkillCount - 1)), "cheongeun_4.msa" },
		{ pkMotionManager.NAME_SKILL + (3 * CPythonSkill::SKILL_GRADEGAP) + 20 + CInstanceBase::SKILL_COSTUME_EFFECT_END + (100 * (loadSkillCount - 1)), "geompung_4.msa" }
	};

	pRaceData->RegisterMotionMode(pkMotionManager.MODE_GENERAL);
	for (const auto& skill : warriorSkills)
	{
		snprintf(szFileName, sizeof(szFileName), "%s%s", skillPath.c_str(), skill.second);
		auto pkMotionThing = pRaceData->RegisterMotionData(pkMotionManager.MODE_GENERAL, skill.first, szFileName, 0);

		if (pkMotionThing)
		{
			CResourceManager::Instance().LoadStaticCache(pkMotionThing->GetFileName());
		}
	}
}

void CPythonPlayerSettingsModule::LoadNewAssassinSkillsEx(DWORD dwRace, const std::string& path, int loadSkillCount)
{
	CRaceManager& pkManager = CRaceManager::Instance();
	pkManager.SelectRace(dwRace);

	CRaceData* pRaceData = pkManager.GetSelectedRaceDataPointer();
	if (!pRaceData)
		return;

	char szEndString[16];
	snprintf(szEndString, sizeof(szEndString), "_%d/", loadSkillCount);

	char szFileName[FILE_MAX_NUM];
	std::string skillPath = path + "skill" + szEndString;

	// Skill motions for assassin costume variants
	std::vector<std::pair<DWORD, const char*>> assassinSkills = {
		{ pkMotionManager.NAME_SKILL + (3 * CPythonSkill::SKILL_GRADEGAP) + 1 + CInstanceBase::SKILL_COSTUME_EFFECT_END + (100 * (loadSkillCount - 1)), "amseup_4.msa" },
		{ pkMotionManager.NAME_SKILL + (3 * CPythonSkill::SKILL_GRADEGAP) + 2 + CInstanceBase::SKILL_COSTUME_EFFECT_END + (100 * (loadSkillCount - 1)), "gungsin_4.msa" },
		{ pkMotionManager.NAME_SKILL + (3 * CPythonSkill::SKILL_GRADEGAP) + 3 + CInstanceBase::SKILL_COSTUME_EFFECT_END + (100 * (loadSkillCount - 1)), "charyun_4.msa" },
		{ pkMotionManager.NAME_SKILL + (3 * CPythonSkill::SKILL_GRADEGAP) + 4 + CInstanceBase::SKILL_COSTUME_EFFECT_END + (100 * (loadSkillCount - 1)), "eunhyeong_4.msa" },
		{ pkMotionManager.NAME_SKILL + (3 * CPythonSkill::SKILL_GRADEGAP) + 5 + CInstanceBase::SKILL_COSTUME_EFFECT_END + (100 * (loadSkillCount - 1)), "sangong_4.msa" },
		{ pkMotionManager.NAME_SKILL + (3 * CPythonSkill::SKILL_GRADEGAP) + 16 + CInstanceBase::SKILL_COSTUME_EFFECT_END + (100 * (loadSkillCount - 1)), "yeonsa_4.msa" },
		{ pkMotionManager.NAME_SKILL + (3 * CPythonSkill::SKILL_GRADEGAP) + 17 + CInstanceBase::SKILL_COSTUME_EFFECT_END + (100 * (loadSkillCount - 1)), "gwangyeok_4.msa" },
		{ pkMotionManager.NAME_SKILL + (3 * CPythonSkill::SKILL_GRADEGAP) + 18 + CInstanceBase::SKILL_COSTUME_EFFECT_END + (100 * (loadSkillCount - 1)), "hwajo_4.msa" },
		{ pkMotionManager.NAME_SKILL + (3 * CPythonSkill::SKILL_GRADEGAP) + 19 + CInstanceBase::SKILL_COSTUME_EFFECT_END + (100 * (loadSkillCount - 1)), "gyeonggong_4.msa" },
		{ pkMotionManager.NAME_SKILL + (3 * CPythonSkill::SKILL_GRADEGAP) + 20 + CInstanceBase::SKILL_COSTUME_EFFECT_END + (100 * (loadSkillCount - 1)), "dokgigung_4.msa" }
	};

	pRaceData->RegisterMotionMode(pkMotionManager.MODE_GENERAL);
	for (const auto& skill : assassinSkills)
	{
		snprintf(szFileName, sizeof(szFileName), "%s%s", skillPath.c_str(), skill.second);
		auto pkMotionThing = pRaceData->RegisterMotionData(pkMotionManager.MODE_GENERAL, skill.first, szFileName, 0);

		if (pkMotionThing)
		{
			CResourceManager::Instance().LoadStaticCache(pkMotionThing->GetFileName());
		}
	}
}

void CPythonPlayerSettingsModule::LoadNewSuraSkillsEx(DWORD dwRace, const std::string& path, int loadSkillCount)
{
	CRaceManager& pkManager = CRaceManager::Instance();
	pkManager.SelectRace(dwRace);

	CRaceData* pRaceData = pkManager.GetSelectedRaceDataPointer();
	if (!pRaceData)
		return;

	char szEndString[16];
	snprintf(szEndString, sizeof(szEndString), "_%d/", loadSkillCount);

	char szFileName[FILE_MAX_NUM];
	std::string skillPath = path + "skill" + szEndString;

	// Skill motions for sura costume variants
	std::vector<std::pair<DWORD, const char*>> suraSkills = {
		{ pkMotionManager.NAME_SKILL + (3 * CPythonSkill::SKILL_GRADEGAP) + 1 + CInstanceBase::SKILL_COSTUME_EFFECT_END + (100 * (loadSkillCount - 1)), "swaeryeong_4.msa" },
		{ pkMotionManager.NAME_SKILL + (3 * CPythonSkill::SKILL_GRADEGAP) + 2 + CInstanceBase::SKILL_COSTUME_EFFECT_END + (100 * (loadSkillCount - 1)), "yonggwon_4.msa" },
		{ pkMotionManager.NAME_SKILL + (3 * CPythonSkill::SKILL_GRADEGAP) + 3 + CInstanceBase::SKILL_COSTUME_EFFECT_END + (100 * (loadSkillCount - 1)), "gwigeom_4.msa" },
		{ pkMotionManager.NAME_SKILL + (3 * CPythonSkill::SKILL_GRADEGAP) + 4 + CInstanceBase::SKILL_COSTUME_EFFECT_END + (100 * (loadSkillCount - 1)), "gongpo_4.msa" },
		{ pkMotionManager.NAME_SKILL + (3 * CPythonSkill::SKILL_GRADEGAP) + 5 + CInstanceBase::SKILL_COSTUME_EFFECT_END + (100 * (loadSkillCount - 1)), "jumagap_4.msa" },
		{ pkMotionManager.NAME_SKILL + (3 * CPythonSkill::SKILL_GRADEGAP) + 6 + CInstanceBase::SKILL_COSTUME_EFFECT_END + (100 * (loadSkillCount - 1)), "pabeop_4.msa" },
		{ pkMotionManager.NAME_SKILL + (3 * CPythonSkill::SKILL_GRADEGAP) + 16 + CInstanceBase::SKILL_COSTUME_EFFECT_END + (100 * (loadSkillCount - 1)), "maryeong_4.msa" },
		{ pkMotionManager.NAME_SKILL + (3 * CPythonSkill::SKILL_GRADEGAP) + 17 + CInstanceBase::SKILL_COSTUME_EFFECT_END + (100 * (loadSkillCount - 1)), "hwayeom_4.msa" },
		{ pkMotionManager.NAME_SKILL + (3 * CPythonSkill::SKILL_GRADEGAP) + 18 + CInstanceBase::SKILL_COSTUME_EFFECT_END + (100 * (loadSkillCount - 1)), "muyeong_4.msa" },
		{ pkMotionManager.NAME_SKILL + (3 * CPythonSkill::SKILL_GRADEGAP) + 19 + CInstanceBase::SKILL_COSTUME_EFFECT_END + (100 * (loadSkillCount - 1)), "heuksin_4.msa" },
		{ pkMotionManager.NAME_SKILL + (3 * CPythonSkill::SKILL_GRADEGAP) + 20 + CInstanceBase::SKILL_COSTUME_EFFECT_END + (100 * (loadSkillCount - 1)), "tusok_4.msa" },
		{ pkMotionManager.NAME_SKILL + (3 * CPythonSkill::SKILL_GRADEGAP) + 21 + CInstanceBase::SKILL_COSTUME_EFFECT_END + (100 * (loadSkillCount - 1)), "mahwan_4.msa" }
	};

	pRaceData->RegisterMotionMode(pkMotionManager.MODE_GENERAL);
	for (const auto& skill : suraSkills)
	{
		snprintf(szFileName, sizeof(szFileName), "%s%s", skillPath.c_str(), skill.second);
		auto pkMotionThing = pRaceData->RegisterMotionData(pkMotionManager.MODE_GENERAL, skill.first, szFileName, 0);

		if (pkMotionThing)
		{
			CResourceManager::Instance().LoadStaticCache(pkMotionThing->GetFileName());
		}
	}
}

void CPythonPlayerSettingsModule::LoadNewShamanSkillsEx(DWORD dwRace, const std::string& path, int loadSkillCount)
{
	CRaceManager& pkManager = CRaceManager::Instance();
	pkManager.SelectRace(dwRace);

	CRaceData* pRaceData = pkManager.GetSelectedRaceDataPointer();
	if (!pRaceData)
		return;

	char szEndString[16];
	snprintf(szEndString, sizeof(szEndString), "_%d/", loadSkillCount);

	char szFileName[FILE_MAX_NUM];
	std::string skillPath = path + "skill" + szEndString;

	// Skill motions for shaman costume variants
	std::vector<std::pair<DWORD, const char*>> shamanSkills = {
		{ pkMotionManager.NAME_SKILL + (3 * CPythonSkill::SKILL_GRADEGAP) + 1 + CInstanceBase::SKILL_COSTUME_EFFECT_END + (100 * (loadSkillCount - 1)), "bipabu_4.msa" },
		{ pkMotionManager.NAME_SKILL + (3 * CPythonSkill::SKILL_GRADEGAP) + 2 + CInstanceBase::SKILL_COSTUME_EFFECT_END + (100 * (loadSkillCount - 1)), "yongpa_4.msa" },
		{ pkMotionManager.NAME_SKILL + (3 * CPythonSkill::SKILL_GRADEGAP) + 3 + CInstanceBase::SKILL_COSTUME_EFFECT_END + (100 * (loadSkillCount - 1)), "paeryong_4.msa" },
		{ pkMotionManager.NAME_SKILL + (3 * CPythonSkill::SKILL_GRADEGAP) + 4 + CInstanceBase::SKILL_COSTUME_EFFECT_END + (100 * (loadSkillCount - 1)), "hosin_4.msa" },
		{ pkMotionManager.NAME_SKILL + (3 * CPythonSkill::SKILL_GRADEGAP) + 5 + CInstanceBase::SKILL_COSTUME_EFFECT_END + (100 * (loadSkillCount - 1)), "boho_4.msa" },
		{ pkMotionManager.NAME_SKILL + (3 * CPythonSkill::SKILL_GRADEGAP) + 6 + CInstanceBase::SKILL_COSTUME_EFFECT_END + (100 * (loadSkillCount - 1)), "gicheon_4.msa" },
		{ pkMotionManager.NAME_SKILL + (3 * CPythonSkill::SKILL_GRADEGAP) + 16 + CInstanceBase::SKILL_COSTUME_EFFECT_END + (100 * (loadSkillCount - 1)), "noejeon_4.msa" },
		{ pkMotionManager.NAME_SKILL + (3 * CPythonSkill::SKILL_GRADEGAP) + 17 + CInstanceBase::SKILL_COSTUME_EFFECT_END + (100 * (loadSkillCount - 1)), "byeorak_4.msa" },
		{ pkMotionManager.NAME_SKILL + (3 * CPythonSkill::SKILL_GRADEGAP) + 18 + CInstanceBase::SKILL_COSTUME_EFFECT_END + (100 * (loadSkillCount - 1)), "pokroe_4.msa" },
		{ pkMotionManager.NAME_SKILL + (3 * CPythonSkill::SKILL_GRADEGAP) + 19 + CInstanceBase::SKILL_COSTUME_EFFECT_END + (100 * (loadSkillCount - 1)), "jeongeop_4.msa" },
		{ pkMotionManager.NAME_SKILL + (3 * CPythonSkill::SKILL_GRADEGAP) + 20 + CInstanceBase::SKILL_COSTUME_EFFECT_END + (100 * (loadSkillCount - 1)), "kwaesok_4.msa" },
		{ pkMotionManager.NAME_SKILL + (3 * CPythonSkill::SKILL_GRADEGAP) + 21 + CInstanceBase::SKILL_COSTUME_EFFECT_END + (100 * (loadSkillCount - 1)), "jeungryeok_4.msa" }
	};

	pRaceData->RegisterMotionMode(pkMotionManager.MODE_GENERAL);
	for (const auto& skill : shamanSkills)
	{
		snprintf(szFileName, sizeof(szFileName), "%s%s", skillPath.c_str(), skill.second);
		auto pkMotionThing = pRaceData->RegisterMotionData(pkMotionManager.MODE_GENERAL, skill.first, szFileName, 0);

		if (pkMotionThing)
		{
			CResourceManager::Instance().LoadStaticCache(pkMotionThing->GetFileName());
		}
	}
}

#endif // ENABLE_SKILL_COSTUME_SYSTEM

void CPythonPlayerSettingsModule::RegisterSkills(uint32_t race, uint8_t skill_group, uint8_t empire)
{
	const uint32_t playerRace = race;
	const uint32_t playerSkillGroup = skill_group;
	const uint8_t playerJob = RaceToJob(playerRace);
	//const uint32_t playerEmpire = empire;

	// Player Skills
	if (skillIndexes.find(playerJob) != skillIndexes.end() &&
		skillIndexes.at(playerJob).find(skill_group) != skillIndexes.at(playerJob).end())
	{
		std::vector<uint8_t> playerSkills = skillIndexes.at(playerJob).at(playerSkillGroup);

		for (size_t slot = 0; slot < playerSkills.size(); ++slot)
			CPythonPlayer::Instance().SetSkill(slot + 1, playerSkills[slot]);
	}

	// Support Skills
	const auto supportSkills = supportSkillIndexes;
	for (size_t slot = 0; slot < supportSkills.size(); ++slot)
		CPythonPlayer::Instance().SetSkill(slot + 100 + 1, supportSkills[slot]);

	 //Language Skills
	/*if (playerEmpire)
	{
		for (size_t i = 0; i < 3; ++i)
		{
			if ((i+1) != playerEmpire)
				CPythonPlayer::Instance().SetSkill(109+i, c_iSkillIndex_Language1+i);
		}
	}*/

	// Passive Guild Skills
	auto passiveSkills = guildSkills.at("PASSIVE");
	for (size_t j = 0; j < passiveSkills.size(); ++j)
		CPythonPlayer::Instance().SetSkill(200 + j, passiveSkills[j]);

	// Active Guild Skills
	auto activeSkills = guildSkills.at("ACTIVE");
	for (size_t k = 0; k < activeSkills.size(); ++k)
		CPythonPlayer::Instance().SetSkill(210 + k, activeSkills[k]);
}

// void CPythonPlayerSettingsModule::__LoadRaceHeight()
// {
	// for (const auto & height : raceHeight)
		// CRaceManager::Instance().SetRaceHeight(height.first, height.second);
// }
#endif