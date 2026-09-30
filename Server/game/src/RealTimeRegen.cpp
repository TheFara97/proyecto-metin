#include "stdafx.h"

#ifdef ENABLE_REAL_TIME_REGEN
#include "RealTimeRegen.hpp"
#include "sectree_manager.h"
#include "locale_service.h"
#include "char_manager.h"
#include "desc_manager.h"
#include "sectree.h"
#include "utils.h"
#include "desc.h"

#include <sstream>
#include <fstream>
#include <boost/date_time/posix_time/posix_time.hpp>

#ifdef ENABLE_RESP_SYSTEM
#include "resp_manager.h"
#include <chrono>
#include <numeric>
#endif

static const BYTE BASE_HOUR = 60;
static const BYTE BASE_DAY = 24;

CRealTimeRegen::CRealTimeRegen()
{
	m_dwLastTick = get_global_time();
	m_dwLastMobsRefresh = 0;
	m_bIsInitialized = false;
}

void CRealTimeRegen::Initialize()
{
	// Load regen file
	LoadRegen();

	// Initializing first-next respawn time
	auto time = boost::posix_time::second_clock::local_time();
	int32_t lHour = time.time_of_day().hours();
	int32_t lMinute = time.time_of_day().minutes();

	for (const auto & it : mapRespawnTimes)
	{
		const int32_t lMapIndex = std::get<REGEN_MAP_INDEX>(it.second);
		
		// If there is no such map index assigned to the core, skip it
		SECTREE_MAP	* pSectree = SECTREE_MANAGER::instance().GetMap(lMapIndex);
		if (!pSectree)
			continue;
			
		uint32_t wNum = it.first;
		BYTE bType = std::get<REGEN_TYPE>(it.second);
		auto vecTimes = std::get<REGEN_TIMES>(it.second);
		
		if (bType == REAL_TIME_REGEN_STANDARD)
		{
			int32_t intervalMinutes = vecTimes.at(0);
			int32_t lAddHour, lAddMin;

			if (intervalMinutes < BASE_HOUR) {
				// For intervals less than 1 hour (like 1, 2, 5, 30 minutes)
				// Calculate next spawn based on current minute + interval
				lAddMin = lMinute + intervalMinutes;
				lAddHour = lHour;

				// Handle minute overflow
				if (lAddMin >= BASE_HOUR) {
					lAddMin -= BASE_HOUR;
					lAddHour++;
					if (lAddHour >= BASE_DAY) {
						lAddHour = 0;
					}
				}
			} else {
				// For intervals of 1 hour or more, use the original logic
				lAddHour = lHour + (intervalMinutes / BASE_HOUR);
				lAddMin = intervalMinutes % BASE_HOUR;

				// Find first-next minute
				while (lAddMin <= lMinute && lAddMin != 0)
				{
					lAddMin += intervalMinutes % BASE_HOUR;

					if (lAddMin >= BASE_HOUR)
					{
						lAddMin -= BASE_HOUR;
						lAddHour++;
						break;
					}
				}

				if (lAddHour >= BASE_DAY)
					lAddHour -= BASE_DAY;
			}

			mapNextRespawn.insert(std::make_pair(wNum, std::make_tuple(lAddHour, lAddMin)));

#ifdef ENABLE_RESP_SYSTEM
			const DWORD dwVnum = std::get<REGEN_VNUM>(it.second);
			const int32_t lX = std::get<REGEN_X>(it.second);
			const int32_t lY = std::get<REGEN_Y>(it.second);

			int32_t x = 100 * lX + pSectree->m_setting.iBaseX;
			int32_t y = 100 * lY + pSectree->m_setting.iBaseY;

			// Convert minutes to seconds for RegisterMob
			CRespManager::instance().RegisterMob(lMapIndex, wNum, dwVnum, x, y, intervalMinutes * 60, 0);
#endif
		}
		else if (bType == REAL_TIME_REGEN_PRECISE)
		{
			int32_t lRespHour = vecTimes.at(0);

			for (const auto & lFurtherHour : vecTimes)
			{
				if (lFurtherHour > lHour)
				{
					lRespHour = lFurtherHour;
					break;
				}
			}

			mapNextRespawn.insert(std::make_pair(wNum, std::make_tuple(lRespHour, 0)));

#ifdef ENABLE_RESP_SYSTEM
			const DWORD dwVnum = std::get<REGEN_VNUM>(it.second);
			const int32_t lX = std::get<REGEN_X>(it.second);
			const int32_t lY = std::get<REGEN_Y>(it.second);

			int32_t x = 100 * lX + pSectree->m_setting.iBaseX;
			int32_t y = 100 * lY + pSectree->m_setting.iBaseY;

			int totalSeconds = std::accumulate(vecTimes.begin(), vecTimes.end(), 0, [](int sum, int hour) {
				return sum + (hour * 3600);
			});

			int meanSeconds = std::round(static_cast<double>(totalSeconds) / vecTimes.size());

			CRespManager::instance().RegisterMob(lMapIndex, wNum, dwVnum, x, y, meanSeconds, get_global_time() + SecondsToSpawn(lRespHour, 0));
#endif
		}
		else if (bType == REAL_TIME_REGEN_DEATH)
		{
			// Individual per-mob death timer: TIMES[0] is respawn delay in minutes
			int32_t delayMinutes = vecTimes.at(0);
			mapDeathTimes[wNum] = 0; // 0 = alive (not waiting to respawn)

#ifdef ENABLE_RESP_SYSTEM
			const DWORD dwVnum = std::get<REGEN_VNUM>(it.second);
			const int32_t lX = std::get<REGEN_X>(it.second);
			const int32_t lY = std::get<REGEN_Y>(it.second);

			int32_t x = 100 * lX + pSectree->m_setting.iBaseX;
			int32_t y = 100 * lY + pSectree->m_setting.iBaseY;

			CRespManager::instance().RegisterMob(lMapIndex, wNum, dwVnum, x, y, delayMinutes * 60, 0);
#endif
		}

		// Spawn initial monsters for standard and death regen types
		if (bType == REAL_TIME_REGEN_STANDARD || bType == REAL_TIME_REGEN_DEATH)
		{
			const DWORD dwVnum = std::get<REGEN_VNUM>(it.second);
			const int32_t lX = std::get<REGEN_X>(it.second);
			const int32_t lY = std::get<REGEN_Y>(it.second);
			const int32_t lRangeX = std::get<REGEN_RANGE_X>(it.second);
			const int32_t lRangeY = std::get<REGEN_RANGE_Y>(it.second);

			SpawnMonster(wNum, dwVnum, lMapIndex, lX, lY, lRangeX, lRangeY);
		}
	}
	
	m_bIsInitialized = true;
}

void CRealTimeRegen::Process()
{
	if (!m_bIsInitialized)
		return;
	
	if (get_global_time() <= m_dwLastTick)
		return;
	
	auto time = boost::posix_time::second_clock::local_time();
	int32_t lHour = time.time_of_day().hours();
	int32_t lMinute = time.time_of_day().minutes();
	int32_t lSeconds = time.time_of_day().seconds();

	// STANDARD / PRECISE: clock-based scheduled respawns (bosses etc.)
	for (auto & it : mapNextRespawn)
	{
		TRegen regen = mapRespawnTimes[it.first];

		BYTE bType = std::get<REGEN_TYPE>(regen);

		if (lHour == std::get<0>(it.second) && lMinute >= std::get<1>(it.second) && lSeconds == 0)
		{
			auto vecTimes = std::get<REGEN_TIMES>(regen);

			int32_t lAddHour = 0, lAddMinute = 0;

			if (std::get<REGEN_TYPE>(regen) == REAL_TIME_REGEN_PRECISE)
			{
				lAddHour = vecTimes.at(0);

				for (const auto & lFurtherHour : vecTimes)
				{
					if (lFurtherHour > lHour)
					{
						lAddHour = lFurtherHour;
						break;
					}
				}
			}
			else
			{
				lAddHour = std::get<1>(it.second)+(vecTimes.at(0) % BASE_HOUR) >= BASE_HOUR ? lHour + (vecTimes.at(0) / BASE_HOUR) + 1 : lHour + (vecTimes.at(0) / BASE_HOUR);
				lAddMinute = std::get<1>(it.second)+(vecTimes.at(0) % BASE_HOUR) >= BASE_HOUR ? std::get<1>(it.second) + (vecTimes.at(0) % BASE_HOUR) - BASE_HOUR : std::get<1>(it.second) + (vecTimes.at(0) % BASE_HOUR);
			}

			if (lAddHour >= BASE_DAY)
				lAddHour -= BASE_DAY;

			it.second = std::make_tuple(lAddHour, lAddMinute);

			const DWORD dwVnum = std::get<REGEN_VNUM>(regen);
			const int32_t lMapIndex = std::get<REGEN_MAP_INDEX>(regen);
			const int32_t lX = std::get<REGEN_X>(regen);
			const int32_t lY = std::get<REGEN_Y>(regen);
			const int32_t lRangeX = std::get<REGEN_RANGE_X>(regen);
			const int32_t lRangeY = std::get<REGEN_RANGE_Y>(regen);

			SpawnMonster(it.first, dwVnum, lMapIndex, lX, lY, lRangeX, lRangeY);
		}
	}

	// DEATH type: individual per-mob timers, staggered naturally across time
	DWORD dwNow = get_global_time();
	for (auto & it : mapDeathTimes)
	{
		if (it.second == 0 || dwNow < static_cast<DWORD>(it.second))
			continue;

		it.second = 0; // reset — alive again

		const auto& regenIt = mapRespawnTimes.find(it.first);
		if (regenIt == mapRespawnTimes.end())
			continue;

		const TRegen& regen = regenIt->second;
		SpawnMonster(it.first,
			std::get<REGEN_VNUM>(regen),
			std::get<REGEN_MAP_INDEX>(regen),
			std::get<REGEN_X>(regen),
			std::get<REGEN_Y>(regen),
			std::get<REGEN_RANGE_X>(regen),
			std::get<REGEN_RANGE_Y>(regen));
	}
	
	// Refreshing all mobs if the option is enabled
	if (bRefreshAllMobs && bRefreshAllMobsHour == lHour && lMinute == 0 && time.time_of_day().seconds() == 0 && m_dwLastMobsRefresh != get_global_time())
	{
		CHARACTER_MANAGER::Instance().RefreshAllMonsters();
		m_dwLastMobsRefresh = get_global_time();

		SendRegenNotice("REAL_TIME_REGEN_NOTICE_RESPAWN_ALL_MOBS");
	}
	
	m_dwLastTick = get_global_time();
}

bool CRealTimeRegen::LoadRegen()
{
	std::string f_name(std::string(LocaleService_GetBasePath()) + "/" + file_name);
	
	std::ifstream config(f_name.c_str());
	if (config.is_open() && config.good())
	{
		int line_num = 0;
		for (std::string line; std::getline(config, line, '\n');)
		{
			size_t pos = line.find('\t');
			while (pos != std::string::npos)
			{
				line[pos] = ' ';
				pos = line.find('\t');
			}

			line_num++;

			if (line.find('#') != std::string::npos)
				continue;

			int32_t type, vnum, map_index, x, y, range_x, range_y;
			std::vector<int32_t> times;
			
			std::istringstream tokens(line);

			if (!(tokens >> vnum) || !(tokens >> type) || !(tokens >> map_index) || !(tokens >> x) || !(tokens >> y) || !(tokens >> range_x) || !(tokens >> range_y))
			{
				sys_err("Error during loading RealTimeRegen, line number %d. Skipping.", line_num);
				continue;
			}
			
			int32_t time;
			while (tokens >> time)
			{
				if (type == REAL_TIME_REGEN_PRECISE && time > BASE_DAY)
				{
					sys_err("Time is greater than 24. Line: %d. Skipping.", line_num);
					continue;
				}
				
				if (type == REAL_TIME_REGEN_PRECISE && time == BASE_DAY)
					time = 0;
				
				times.push_back(time);
			}
			
			std::sort(times.begin(), times.end());
			
			// Generate deterministic ID: vnum * 100000 + (x + y)
			uint32_t generatedId = vnum * 100000 + (x + y);
			
			// Insert regen into the map with generated ID instead of line_num
			mapRespawnTimes.insert(std::make_pair(generatedId, std::make_tuple(type, vnum, map_index, x, y, range_x, range_y, times)));
		}
	}
	else
		sys_err("Error. Cannot load RealTimeRegen file."); 
	
	return true;
}

void CRealTimeRegen::SpawnMonster(const uint32_t & wNum, const DWORD & dwVnum, const int32_t & lMapIndex, const int32_t & lX, const int32_t & lY, const int32_t & lRangeX, const int32_t & lRangeY)
{
	// Do not spawn mob if it is still alive
	if (CHARACTER_MANAGER::Instance().FindRealTimeRegenMonster(wNum))
		return;

	const TMapRegion * region = SECTREE_MANAGER::instance().GetMapRegion(lMapIndex);
	if (!region)
		return;

	// Clean up any stale reference before spawning
	CHARACTER_MANAGER::Instance().EraseRealTimeRegenCharacter(wNum);

	// Spawn the monster
	const int32_t lMinSpawnX = MAX(region->sx, region->sx+(lX-lRangeX)*100);
	const int32_t lMinSpawnY = MAX(region->sy, region->sy+(lY-lRangeY)*100);
	
	const int32_t lMaxSpawnX = MIN(region->ex, region->sx+(lX+lRangeX)*100);
	const int32_t lMaxSpawnY = MIN(region->ey, region->sy+(lY+lRangeY)*100);

	LPCHARACTER pMonster = CHARACTER_MANAGER::Instance().SpawnMobRange(dwVnum, lMapIndex, lMinSpawnX, lMinSpawnY, lMaxSpawnX, lMaxSpawnY);
	if (!pMonster)
	{
		sys_err("Monster %d has not been spawned at %d %d, map index: %d.", dwVnum, lX, lY, lMapIndex);
		return;
	}

	pMonster->SetRealTimeRegenNum(wNum);
#ifdef ENABLE_RESP_SYSTEM
	CRespManager::instance().RegisterMob(pMonster);
#endif
}

void CRealTimeRegen::SendRegenNotice(const std::string & szNotice)
{
	const DESC_MANAGER::DESC_SET & c_client_set = DESC_MANAGER::instance().GetClientSet();

	std::for_each(c_client_set.begin(), c_client_set.end(), 
			[&szNotice](LPDESC d){ if (d->GetCharacter()) d->GetCharacter()->ChatPacket(CHAT_TYPE_BIG_NOTICE, szNotice.c_str()); });
}

#ifdef ENABLE_RESP_SYSTEM
int CRealTimeRegen::GetNextSpawnTime(const uint32_t& wNum)
{
	const auto& it = mapRespawnTimes.find(wNum);
	if (it == mapRespawnTimes.end())
	{
		sys_err("Failed to find resp data for num %d", wNum);
		return 0;
	}

	const TRegen regen = it->second;

	// DEATH type: return the fixed delay in seconds (atlas shows time-since-death countdown)
	if (std::get<REGEN_TYPE>(regen) == REAL_TIME_REGEN_DEATH)
	{
		const auto& vecTimes = std::get<REGEN_TIMES>(regen);
		return vecTimes.empty() ? 0 : vecTimes.at(0) * 60;
	}

	auto time = GetTime();
	int32_t lHour = time.tm_hour;
	int32_t lMinute = time.tm_min;

	const TRegenTime regen_time = mapNextRespawn[wNum];

	int32_t lAddHour = std::get<0>(regen_time);
	int32_t lAddMinute = std::get<1>(regen_time);

	return SecondsToSpawn(lAddHour, lAddMinute);
}


int CRealTimeRegen::SecondsToSpawn(const int32_t hour, const int32_t minute)
{
    auto currentTime = std::chrono::system_clock::now();
    std::time_t currentTimeT = std::chrono::system_clock::to_time_t(currentTime);
    std::tm* localTime = std::localtime(&currentTimeT);

    // Create target time for today
    std::tm targetTime = *localTime;
    targetTime.tm_hour = hour;
    targetTime.tm_min = minute;
    targetTime.tm_sec = 0;

    std::time_t targetTimeT = std::mktime(&targetTime);
    std::time_t currentTimeT_raw = std::chrono::system_clock::to_time_t(currentTime);
    
    int32_t secondsDiff = static_cast<int32_t>(targetTimeT - currentTimeT_raw);
    
    // If target time is in the past, check if it's a short interval (< 1 hour)
    if (secondsDiff < 0) {
        // For minute-based intervals, if we're past the target minute in current hour,
        // the next occurrence is in the next hour
        if (secondsDiff > -3600) { // Within the last hour
            secondsDiff += 3600; // Add 1 hour
        } else {
            secondsDiff += 86400; // Add 24 hours for next day
        }
    }
    
    //sys_err("teraz %ld, wtedy %ld (aktualna godzina: %d, docelowa: %d, diff: %d)", 
    //        currentTimeT_raw, targetTimeT, localTime->tm_hour, hour, secondsDiff);

    return secondsDiff;
}

const std::tm CRealTimeRegen::GetTime()
{
	const auto global_time = get_global_time();
	return *localtime(&global_time);
}
#endif

void CRealTimeRegen::OnMobDeath(const uint32_t wNum)
{
	const auto& it = mapRespawnTimes.find(wNum);
	if (it == mapRespawnTimes.end())
		return;

	if (std::get<REGEN_TYPE>(it->second) != REAL_TIME_REGEN_DEATH)
		return;

	const auto& vecTimes = std::get<REGEN_TIMES>(it->second);
	if (vecTimes.empty())
		return;

	int32_t delaySeconds = vecTimes.at(0) * 60;
	mapDeathTimes[wNum] = static_cast<int64_t>(get_global_time()) + delaySeconds;
}

void CRealTimeRegen::Destroy()
{
	m_bIsInitialized = false;

	mapRespawnTimes.clear();
	mapNextRespawn.clear();
	mapDeathTimes.clear();
}
#endif
