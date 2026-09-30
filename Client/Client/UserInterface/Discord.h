#include "StdAfx.h"
#include "PythonCharacterManager.h"
#include "PythonBackground.h"
#include "PythonPlayer.h"
#include "PythonGuild.h"
#include <fmt/format.h>

namespace Discord
{
	constexpr auto DiscordClientID = "1359446918119428127";

	using DCDATA = std::pair<std::string, std::string>;

	inline void ReplaceStringInPlace(std::string& subject, const std::string& search,
		const std::string& replace) {
		size_t pos = 0;
		while ((pos = subject.find(search, pos)) != std::string::npos) {
			subject.replace(pos, search.length(), replace);
			pos += replace.length();
		}
	}
	inline void capitalizeWord(std::string& str)
	{
		bool canCapitalize = true;
		for (auto& c : str)
		{
			if (isalpha(c))
			{
				if (canCapitalize)
				{
					c = std::toupper(c);
					canCapitalize = false;
				}
			}
			else
				canCapitalize = true;
		}
	}

	/*NAME*/
	inline DCDATA GetNameData()
	{
		/*Map Name*/
		auto WarpName = std::string(CPythonBackground::Instance().GetWarpMapName());

		//atlasinfo.txt
		static const std::map<std::string, std::string> DCmapname{
			{ "metin2_map_a1", "Yongan" },
			{ "metin2_map_c1", "Pyungmoo" },
			{ "metin2_map_c3", "Bakra" },
			{ "metin2_map_a3", "Yayang" },
			{ "metin2_map_anglar_dungeon_01", "Spider Dungeon I" },
			{ "alune_map24_exp_light", "Sacred Valley" },
			{ "zaris_fish", "Fishing Coast" },
			{ "map_mining", "Hidden Mine" },
			{ "ateop_fm_4", "Ancient Desert" },
			{ "ateop_fm_3", "Frosty Landscape" },
			{ "ateop_map_orc", "Orc Valley" },
			{ "metin2_map_las", "Green Forest" },
			{ "metin2_zakatki", "Mysterious Lands" },
			{ "ateop_fm_2", "Enchanted Forest" },
			{ "metin2_map_whitedragoncave_boss", "Ice Cave" },
			{ "plechito_ancient", "Ancient Jungle" },
			{ "plechito_wukong_dungeon", "Wukong Mountains" },
			{ "plechito_chamber_of_wisdom", "Hall of the Spider Queen" },
			{ "plechito_scorpion_dungeon", "Scorpion Ruins" },
			{ "ateop_map_flame", "Fiery Wasteland" },
			{ "mehok_map_worldboss_2", "Worldboss Map" },
			{ "zaris_map_swamp", "Swamp (200+)" },
			{ "alune_ac3_party_2", "Emperor's Gorge" },
			{ "alune_map24_exp_dark", "Cursed Valley" },
			{ "4d_orki", "Dustfang Camp" },
			{ "dt_glador", "Devil Tower" },
			{ "6d_sanktuarium", "Sanctum of Azrael" },
			{ "5d_ezoty", "Hwang Temple" },
			{ "2d_beran", "Emerald Lair" },
			{ "9d_trytony", "Gorgon Depths" },
			{ "10d_spiderqueen", "Silken Depths" },
			{ "d_pieklo", "Hellforge Depths" },
			{ "8d_zlote", "Crimson Blossom Fortress" },
			{ "zaris_easter_map_2025", "Easter Valley" },
		};

		if (!DCmapname.count(WarpName))
		{
			ReplaceStringInPlace(WarpName, "season1/", "");
			ReplaceStringInPlace(WarpName, "season2/", "");
			ReplaceStringInPlace(WarpName, "metin2_map_", "");
			ReplaceStringInPlace(WarpName, "metin2_", "");
			ReplaceStringInPlace(WarpName, "plechito_", "");
			ReplaceStringInPlace(WarpName, "_", " ");
			capitalizeWord(WarpName);
		}
		auto MapName = "Mapa: " + (DCmapname.count(WarpName) ? DCmapname.at(WarpName) : WarpName);

		/*CH Name*/
		std::string GuildName;
		CPythonGuild::Instance().GetGuildName(CPythonPlayer::Instance().GetGuildID(), &GuildName);
		auto CHName = fmt::format("Nick: {} (Lv. {}) {}", CPythonPlayer::Instance().GetName(), CPythonPlayer::Instance().GetStatus(POINT_LEVEL), GuildName);

		return { MapName, CHName };
	}

	/*RACE*/
	inline DCDATA GetRaceData()
	{
		auto pInstance = CPythonCharacterManager::Instance().GetMainInstancePtr();
		if (!pInstance)
			return { "","" };

		auto RACENUM = pInstance->GetRace();

		/*Image*/
		auto RaceImage = "race_" + std::to_string(RACENUM);

		/*Name*/
		auto RaceName = "Valecnik";
		switch (RACENUM)
		{
		case NRaceData::JOB_ASSASSIN:
		case NRaceData::JOB_ASSASSIN + 4:
			RaceName = "Ninja";
			break;
		case NRaceData::JOB_SURA:
		case NRaceData::JOB_SURA + 4:
			RaceName = "Sura";
			break;
		case NRaceData::JOB_SHAMAN:
		case NRaceData::JOB_SHAMAN + 4:
			RaceName = "Saman";
			break;
#if defined(ENABLE_WOLFMAN_CHARACTER)
		case NRaceData::JOB_WOLFMAN + 4:
			RaceName = "Lykan";
#endif
		}
		return { RaceImage , RaceName };
	}

	/*EMPIRE*/
	inline DCDATA GetEmpireData()
	{
		auto pInstance = CPythonCharacterManager::Instance().GetMainInstancePtr();
		if (!pInstance)
			return { "","" };

		auto EmpireID = pInstance->GetEmpireID();

		/*Image*/
		auto EmpireImage = "empire_" + std::to_string(EmpireID);

		/*Name*/
		auto EmpireName = "Shinsoo";
		switch (EmpireID)
		{
		case 2:
			EmpireName = "Chunjo";
			break;
		case 3:
			EmpireName = "Jinno";
		}
		return { EmpireImage, EmpireName };
	}
}
//martysama0134's ceqyqttoaf71vasf9t71218
