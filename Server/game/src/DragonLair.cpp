#include "stdafx.h"
#include "config.h"

#include "DragonLair.h"

#include "entity.h"
#include "sectree_manager.h"
#include "char.h"
#include "guild.h"
#include "locale_service.h"
#include "regen.h"
#include "log.h"
#include "utils.h"

struct FWarpToDragronLairWithGuildMembers
{
	DWORD dwGuildID;
	int32_t mapIndex;
	int32_t x, y;

	FWarpToDragronLairWithGuildMembers( DWORD guildID, int32_t map, int32_t X, int32_t Y )
		: dwGuildID(guildID), mapIndex(map), x(X), y(Y)
	{
	}

	void operator()(LPENTITY ent)
	{
		if (NULL != ent && true == ent->IsType(ENTITY_CHARACTER))
		{
			LPCHARACTER pChar = static_cast<LPCHARACTER>(ent);

			if (true == pChar->IsPC())
			{
				if (NULL != pChar->GetGuild())
				{
					if (dwGuildID == pChar->GetGuild()->GetID())
					{
						pChar->WarpSet(x, y, mapIndex);
					}
				}
			}
		}
	}
};

struct FWarpToVillage
{
	void operator() (LPENTITY ent)
	{
		if (NULL != ent)
		{
			LPCHARACTER pChar = static_cast<LPCHARACTER>(ent);

			if (NULL != pChar)
			{
				if (true == pChar->IsPC())
				{
					pChar->GoHome();
				}
			}
		}
	}
};

EVENTINFO(tag_DragonLair_Collapse_EventInfo)
{
	int step;
	CDragonLair* pLair;
	int32_t InstanceMapIndex;

	tag_DragonLair_Collapse_EventInfo()
	: step( 0 )
	, pLair( 0 )
	, InstanceMapIndex( 0 )
	{
	}
};

EVENTFUNC( DragonLair_Collapse_Event )
{
	tag_DragonLair_Collapse_EventInfo* pInfo = dynamic_cast<tag_DragonLair_Collapse_EventInfo*>(event->info);

	if ( pInfo == NULL )
	{
		sys_err( "DragonLair_Collapse_Event> <Factor> Null pointer" );
		return 0;
	}

	if (0 == pInfo->step)
	{

		pInfo->step++;

		return PASSES_PER_SEC( 30 );
	}
	else if (1 == pInfo->step)
	{
		LPSECTREE_MAP pMap = SECTREE_MANAGER::instance().GetMap( pInfo->InstanceMapIndex );

		if (NULL != pMap)
		{
			FWarpToVillage f;
			pMap->for_each( f );
		}

		pInfo->step++;

		return PASSES_PER_SEC( 30 );
	}
	else
	{
		SECTREE_MANAGER::instance().DestroyPrivateMap( pInfo->InstanceMapIndex );
		M2_DELETE(pInfo->pLair);
	}

	return 0;
}

CDragonLair::CDragonLair(DWORD guildID, int32_t BaseMapID, int32_t PrivateMapID)
	: GuildID_(guildID), BaseMapIndex_(BaseMapID), PrivateMapIndex_(PrivateMapID)
{
	StartTime_ = get_global_time();
}

CDragonLair::~CDragonLair()
{
}

DWORD CDragonLair::GetEstimatedTime() const
{
	return get_global_time() - StartTime_;
}

void CDragonLair::OnDragonDead(LPCHARACTER pDragon)
{
	sys_log(0, "DragonLair: µµ¶ó°ïÀÌ Á×¾î½áÈ¿");

	LogManager::instance().DragonSlayLog(  GuildID_, pDragon->GetMobTable().dwVnum, StartTime_, get_global_time() );
}

CDragonLairManager::CDragonLairManager()
{
}

CDragonLairManager::~CDragonLairManager()
{
}

bool CDragonLairManager::Start(int32_t MapIndexFrom, int32_t BaseMapIndex, DWORD GuildID)
{
	int32_t instanceMapIndex = SECTREE_MANAGER::instance().CreatePrivateMap(BaseMapIndex);
	if (instanceMapIndex == 0) {
		sys_err("CDragonLairManager::Start() : no private map index available");
		return false;
	}

	LPSECTREE_MAP pMap = SECTREE_MANAGER::instance().GetMap(MapIndexFrom);

	if (NULL != pMap)
	{
		LPSECTREE_MAP pTargetMap = SECTREE_MANAGER::instance().GetMap(BaseMapIndex);

		if (NULL == pTargetMap)
		{
			return false;
		}

		const TMapRegion* pRegionInfo = SECTREE_MANAGER::instance().GetMapRegion( pTargetMap->m_setting.iIndex );

		if (NULL != pRegionInfo)
		{
			FWarpToDragronLairWithGuildMembers f(GuildID, instanceMapIndex, 844000, 1066900);

			pMap->for_each( f );

			LairMap_.emplace(GuildID, M2_NEW CDragonLair(GuildID, BaseMapIndex, instanceMapIndex));

			std::string strMapBasePath( LocaleService_GetMapPath() );

			strMapBasePath += "/" + pRegionInfo->strMapName + "/instance_regen.txt";

			sys_log(0, "%s", strMapBasePath.c_str());

			regen_do(strMapBasePath.c_str(), instanceMapIndex, pTargetMap->m_setting.iBaseX, pTargetMap->m_setting.iBaseY, NULL, true);

			return true;
		}
	}

	return false;
}

void CDragonLairManager::OnDragonDead(LPCHARACTER pDragon, DWORD KillerGuildID)
{
	if (NULL == pDragon)
		return;

	if (false == pDragon->IsMonster())
		return;

	std::unordered_map<DWORD, CDragonLair*>::iterator iter = LairMap_.find( KillerGuildID );

	if (LairMap_.end() == iter)
	{
		return;
	}

	LPSECTREE_MAP pMap = SECTREE_MANAGER::instance().GetMap( pDragon->GetMapIndex() );

	if (NULL == iter->second || NULL == pMap)
	{
		LairMap_.erase( iter );
		return;
	}

	iter->second->OnDragonDead( pDragon );

	tag_DragonLair_Collapse_EventInfo* info;
	info = AllocEventInfo<tag_DragonLair_Collapse_EventInfo>();

	info->step = 0;
	info->pLair = iter->second;
	info->InstanceMapIndex = pDragon->GetMapIndex();

	event_create(DragonLair_Collapse_Event, info, PASSES_PER_SEC(10));

	LairMap_.erase( iter );
}
//martysama0134's ceqyqttoaf71vasf9t71218
