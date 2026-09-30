#include <boost/unordered_map.hpp>

#include "../../common/stl.h"

class CDragonLair
{
	public:
		CDragonLair (DWORD dwGuildID, int32_t BaseMapID, int32_t PrivateMapID);
		virtual ~CDragonLair ();

		DWORD GetEstimatedTime () const;

		void OnDragonDead (LPCHARACTER pDragon);

	private:
		DWORD StartTime_;
		DWORD GuildID_;
		int32_t BaseMapIndex_;
		int32_t PrivateMapIndex_;
};

class CDragonLairManager : public singleton<CDragonLairManager>
{
	public:
		CDragonLairManager ();
		virtual ~CDragonLairManager ();

		bool Start (int32_t MapIndexFrom, int32_t BaseMapIndex, DWORD GuildID);
		void OnDragonDead (LPCHARACTER pDragon, DWORD KillerGuildID);

		size_t GetLairCount () const { return LairMap_.size(); }

	private:
		std::unordered_map<DWORD, CDragonLair*> LairMap_;
};
//martysama0134's ceqyqttoaf71vasf9t71218
