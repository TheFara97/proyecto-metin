#ifndef __INC_METIN_II_GAME_SECTREE_MANAGER_H__
#define __INC_METIN_II_GAME_SECTREE_MANAGER_H__

#include "sectree.h"
#include <unordered_set>

typedef struct SMapRegion
{
	int			index;
	int			sx, sy, ex, ey;
	PIXEL_POSITION	posSpawn;

	bool		bEmpireSpawnDifferent;
	PIXEL_POSITION	posEmpire[3];

	std::string		strMapName;
} TMapRegion;

struct TAreaInfo
{
	int sx, sy, ex, ey, dir;
	TAreaInfo(int sx, int sy, int ex, int ey, int dir)
		: sx(sx), sy(sy), ex(ex), ey(ey), dir(dir)
		{}
};

struct npc_info
{
	BYTE bType;
	const char* name;
	int32_t x, y;
	npc_info(BYTE bType, const char* name, int32_t x, int32_t y)
		: bType(bType), name(name), x(x), y(y)
		{}
};

typedef std::map<std::string, TAreaInfo> TAreaMap;

typedef struct SSetting
{
	int			iIndex;
	int			iCellScale;
	int			iBaseX;
	int			iBaseY;
	int			iWidth;
	int			iHeight;

	PIXEL_POSITION	posSpawn;
} TMapSetting;

class SECTREE_MAP
{
	public:
		typedef std::map<DWORD, LPSECTREE> MapType;

		SECTREE_MAP();
		SECTREE_MAP(SECTREE_MAP & r);
		virtual ~SECTREE_MAP();

		bool Add(DWORD key, LPSECTREE sectree) {
			return map_.emplace(key, sectree).second;
		}

		LPSECTREE	Find(DWORD dwPackage);
		LPSECTREE	Find(DWORD x, DWORD y);
		void		Build();

		TMapSetting	m_setting;

		template< typename Func >
		void for_each( Func & rfunc )
		{
			// <Factor> Using snapshot copy to avoid side-effects
			FCollectEntity collector;
			std::map<DWORD, LPSECTREE>::iterator it = map_.begin();
			for ( ; it != map_.end(); ++it)
			{
				LPSECTREE sectree = it->second;
				sectree->for_each_entity(collector);
			}
			collector.ForEach(rfunc);
			/*
			std::map<DWORD,LPSECTREE>::iterator i = map_.begin();
			for (; i != map_.end(); ++i )
			{
				LPSECTREE pSec = i->second;
				pSec->for_each_entity( rfunc );
			}
			*/
		}

		void DumpAllToSysErr() {
			SECTREE_MAP::MapType::iterator i;
			for (i = map_.begin(); i != map_.end(); ++i)
			{
				sys_err("SECTREE %x(%u, %u)", i->first, i->first & 0xffff, i->first >> 16);
			}
		}

	private:
		MapType map_;
};

enum EAttrRegionMode
{
	ATTR_REGION_MODE_SET,
	ATTR_REGION_MODE_REMOVE,
	ATTR_REGION_MODE_CHECK,
};

class SECTREE_MANAGER : public singleton<SECTREE_MANAGER>
{
	public:
		SECTREE_MANAGER();
		virtual ~SECTREE_MANAGER();

		LPSECTREE_MAP GetMap(int32_t lMapIndex);
		LPSECTREE 	Get(DWORD dwIndex, DWORD package);
		LPSECTREE 	Get(DWORD dwIndex, DWORD x, DWORD y);

		template< typename Func >
		void for_each( int iMapIndex, Func & rfunc )
		{
			LPSECTREE_MAP pSecMap = SECTREE_MANAGER::instance().GetMap( iMapIndex );
			if ( pSecMap )
			{
				pSecMap->for_each( rfunc );
			}
		}

		int		LoadSettingFile(int32_t lIndex, const char * c_pszSettingFileName, TMapSetting & r_setting);
		bool		LoadMapRegion(const char * c_pszFileName, TMapSetting & r_Setting, const char * c_pszMapName);
		int		Build(const char * c_pszListFileName, const char* c_pszBasePath);
		LPSECTREE_MAP BuildSectreeFromSetting(TMapSetting & r_setting);
		bool		LoadAttribute(LPSECTREE_MAP pkMapSectree, const char * c_pszFileName, TMapSetting & r_setting);
		void		LoadDungeon(int iIndex, const char * c_pszFileName);
		bool		GetValidLocation(int32_t lMapIndex, int32_t x, int32_t y, int32_t & r_lValidMapIndex, PIXEL_POSITION & r_pos, BYTE empire = 0);
		bool		GetSpawnPosition(int32_t x, int32_t y, PIXEL_POSITION & r_pos);
		bool		GetSpawnPositionByMapIndex(int32_t lMapIndex, PIXEL_POSITION & r_pos);
		bool		GetRecallPositionByEmpire(int iMapIndex, BYTE bEmpire, PIXEL_POSITION & r_pos);

		const TMapRegion *	GetMapRegion(int32_t lMapIndex);
		int			GetMapIndex(int32_t x, int32_t y);
		const TMapRegion *	FindRegionByPartialName(const char* szMapName);

		bool		GetMapBasePosition(int32_t x, int32_t y, PIXEL_POSITION & r_pos);
		bool		GetMapBasePositionByMapIndex(int32_t lMapIndex, PIXEL_POSITION & r_pos);
		bool		GetMovablePosition(int32_t lMapIndex, int32_t x, int32_t y, PIXEL_POSITION & pos);
		bool		IsMovablePosition(int32_t lMapIndex, int32_t x, int32_t y);
		bool		GetCenterPositionOfMap(int32_t lMapIndex, PIXEL_POSITION & r_pos);
		bool        GetRandomLocation(int32_t lMapIndex, PIXEL_POSITION & r_pos, DWORD dwCurrentX = 0, DWORD dwCurrentY = 0, int iMaxDistance = 0);

		int32_t		CreatePrivateMap(int32_t lMapIndex);	// returns new private map index, returns 0 when fail
		void		DestroyPrivateMap(int32_t lMapIndex);

		TAreaMap&	GetDungeonArea(int32_t lMapIndex);
		void		SendNPCPosition(LPCHARACTER ch);
		void		InsertNPCPosition(int32_t lMapIndex, BYTE bType, const char* szName, int32_t x, int32_t y);

		BYTE		GetEmpireFromMapIndex(int32_t lMapIndex);

		bool		IsAttackablePosition(int32_t lMapIndex, int32_t x, int32_t y);

		void		PurgeMonstersInMap(int32_t lMapIndex);
		void		PurgeStonesInMap(int32_t lMapIndex);
		void		PurgeNPCsInMap(int32_t lMapIndex);
		size_t		GetMonsterCountInMap(int32_t lMapIndex);
		size_t		GetMonsterCountInMap(int32_t lMpaIndex, DWORD dwVnum);
#ifdef ENABLE_ENTITY_PRELOADING
		void GeneratePreloadedEntitiesMap(int32_t mapIndex, LPSECTREE_MAP lpMapSectree);
		void ExtendPreloadedEntitiesMap(int32_t mapIndex, uint32_t mob_race);
		void SendPreloadEntitiesPacket(LPCHARACTER ch);
#endif

		bool		ForAttrRegion(int32_t lMapIndex, int32_t lStartX, int32_t lStartY, int32_t lEndX, int32_t lEndY, int32_t lRotate, DWORD dwAttr, EAttrRegionMode mode);

		bool		SaveAttributeToImage(int lMapIndex, const char * c_pszFileName, LPSECTREE_MAP pMapSrc = NULL);

	private:

		bool		ForAttrRegionRightAngle( int32_t lMapIndex, int32_t lCX, int32_t lCY, int32_t lCW, int32_t lCH, int32_t lRotate, DWORD dwAttr, EAttrRegionMode mode );

		bool		ForAttrRegionFreeAngle( int32_t lMapIndex, int32_t lCX, int32_t lCY, int32_t lCW, int32_t lCH, int32_t lRotate, DWORD dwAttr, EAttrRegionMode mode );

		bool		ForAttrRegionCell( int32_t lMapIndex, int32_t lCX, int32_t lCY, DWORD dwAttr, EAttrRegionMode mode );

		static WORD			current_sectree_version;
		std::map<DWORD, LPSECTREE_MAP>	m_map_pkSectree;
		std::map<int, TAreaMap>	m_map_pkArea;
		std::vector<TMapRegion>		m_vec_mapRegion;
		std::map<DWORD, std::vector<npc_info> > m_mapNPCPosition;

		// <Factor> Circular private map indexing
		typedef std::unordered_map<int32_t, int> PrivateIndexMapType;
		PrivateIndexMapType next_private_index_map_;
#ifdef ENABLE_ENTITY_PRELOADING
		std::unordered_map<int32_t, std::unordered_set<uint32_t>> m_preloadedEntities;
#endif
};

#endif
//martysama0134's ceqyqttoaf71vasf9t71218
