#pragma once

#include "Packet.h"

class CPythonGuild : public CSingleton<CPythonGuild>
{
	public:
		enum
		{
			GUILD_SKILL_MAX_NUM = 12,
			ENEMY_GUILD_SLOT_MAX_COUNT = 6,
		};

		typedef struct SGulidInfo
		{
			DWORD dwGuildID;
			char szGuildName[GUILD_NAME_MAX_LEN+1];
			DWORD dwMasterPID;
			DWORD dwGuildLevel;
			DWORD dwCurrentExperience;
			DWORD dwCurrentMemberCount;
			DWORD dwMaxMemberCount;
			DWORD dwGuildMoney;
			BOOL bHasLand;
			uint8_t bGuardActiveSetID;
			time_t m_nextCanChangeBonusActiveStatusTime;
			time_t m_guardBonusExpireTime;
			uint8_t	tickets[GUILD_TICKETS_TYPE_COUNT];
			time_t iTodayGivenExp;
			time_t iTodayGivenExpRefreshTime;
			int m_donateStone;
			int m_donateLog;
			int m_donatePlywood;
			int m_dailyStone;
			int m_dailyLog;
			int m_dailyPlywood;
			uint8_t m_bonusLevel[5];
		} TGuildInfo;

		typedef struct SGuildGradeData
		{
			SGuildGradeData(){}
			SGuildGradeData(BYTE byAuthorityFlag_, const char * c_szName_) : byAuthorityFlag(byAuthorityFlag_), strName(c_szName_) {}
			BYTE byAuthorityFlag;
			std::string strName;
		} TGuildGradeData;
		typedef std::map<BYTE, TGuildGradeData> TGradeDataMap;

		typedef struct SGuildMemberData
		{
			DWORD dwPID;

			std::string strName;
			BYTE byGrade;
			BYTE byJob;
			BYTE byLevel;
			BYTE byGeneralFlag;
			DWORD dwOffer;
		} TGuildMemberData;
		typedef std::vector<TGuildMemberData> TGuildMemberDataVector;

		typedef struct SGuildBoardCommentData
		{
			DWORD dwCommentID;
			std::string strName;
			std::string strComment;
		} TGuildBoardCommentData;
		typedef std::vector<TGuildBoardCommentData> TGuildBoardCommentDataVector;

		typedef struct SGuildSkillData
		{
			BYTE bySkillPoint;
			BYTE bySkillLevel[GUILD_SKILL_MAX_NUM];
			WORD wGuildPoint;
			WORD wMaxGuildPoint;
		} TGuildSkillData;

		typedef std::map<DWORD, std::string> TGuildNameMap;

	public:
		CPythonGuild();
		virtual ~CPythonGuild();

		void Destroy();

		void EnableGuild();
		void SetGuildMoney(DWORD dwMoney);
		void SetGuildEXP(BYTE byLevel, DWORD dwEXP);
		void SetGradeData(BYTE byGradeNumber, TGuildGradeData & rGuildGradeData);
		void SetGradeName(BYTE byGradeNumber, const char * c_szName);
		void SetGradeAuthority(BYTE byGradeNumber, BYTE byAuthority);
		void ClearComment();
		void RegisterComment(DWORD dwCommentID, const char * c_szName, const char * c_szComment);
		void SetNote(const char* c_szNote);
		std::string GetNote() { return guildNote; }
		void RegisterMember(TGuildMemberData & rGuildMemberData);
		void ChangeGuildMemberGrade(DWORD dwPID, BYTE byGrade);
		void ChangeGuildMemberGeneralFlag(DWORD dwPID, BYTE byFlag);
		void RemoveMember(DWORD dwPID);
		void RegisterGuildName(DWORD dwID, const char * c_szName);

		BOOL IsMainPlayer(DWORD dwPID);
		BOOL IsGuildEnable();
		TGuildInfo & GetGuildInfoRef();
		BOOL GetGradeDataPtr(DWORD dwGradeNumber, TGuildGradeData ** ppData);
		const TGuildBoardCommentDataVector & GetGuildBoardCommentVector();
		DWORD GetMemberCount();
		BOOL GetMemberDataPtr(DWORD dwIndex, TGuildMemberData ** ppData);
		BOOL GetMemberDataPtrByPID(DWORD dwPID, TGuildMemberData ** ppData);
		BOOL GetMemberDataPtrByName(const char * c_szName, TGuildMemberData ** ppData);
		DWORD GetGuildMemberLevelSummary();
		DWORD GetGuildMemberLevelAverage();
		DWORD GetGuildExperienceSummary();
		TGuildSkillData & GetGuildSkillDataRef();
		bool GetGuildName(DWORD dwID, std::string * pstrGuildName);
		DWORD GetGuildID();
		BOOL HasGuildLand();

		void StartGuildWar(DWORD dwEnemyGuildID);
		void EndGuildWar(DWORD dwEnemyGuildID);
		DWORD GetEnemyGuildID(DWORD dwIndex);
		BOOL IsDoingGuildWar();

		void SetDungeonHistoryInfo(TGuildDungeonHistoryInfo* data);
		std::pair<uint16_t, uint32_t> GetDungeonHistoryInfo(uint8_t dungeonIdx);

		void ClearDungeonBossDmgRank();
		void SetDungeonBossRankDmgData(uint8_t pos, TGuildDungeonBossDamageInfo* data);
		TGuildDungeonBossDamageInfo& GetDungeonBossRankDmgDataByPos(uint8_t pos);
		void SetDungeonBossRankMyDmg(uint64_t myDmg);
		uint64_t GetDungeonBossRankMyDmg();
		
#if defined(ENABLE_LOADING_PERFORMANCE)
		void SetGuildBuildingList(PyObject* ppyAppendingList) { m_ppyGuildBuildingList = ppyAppendingList; }
		const PyObject* GetGuildBuildingList() const { return m_ppyGuildBuildingList; }
#endif

		void SetGuardItemData(uint8_t setID, uint8_t pos, uint32_t vnum);
		uint32_t GetGuardItemVnum(uint8_t setID, uint8_t pos);
		bool HaveAnyGuardSetItem(uint8_t setID);

		void ClearLogs();
		void SetLogsData(TGuildLog* data);
		void SetLogsInfoData(uint32_t _lastLogID, uint32_t _nextLogsRefreshTime);
		uint32_t GetLastLogID() { return lastLogID; }
		uint32_t GetNextLogRefreshTime() { return nextLogsRefreshTime; }
		TGuildLog* GetLogData(uint32_t idx);
		size_t GetLogsCount() { return guildLogsVec.size(); }

	protected:
		void __CalculateLevelAverage();
		void __SortMember();
		BOOL __IsGradeData(BYTE byGradeNumber);

		void __Initialize();

	protected:
		TGuildInfo m_GuildInfo;
		TGradeDataMap m_GradeDataMap;
		TGuildMemberDataVector m_GuildMemberDataVector;
		TGuildBoardCommentDataVector m_GuildBoardCommentVector;
		std::string guildNote;
		TGuildSkillData m_GuildSkillData;
		TGuildNameMap m_GuildNameMap;
		DWORD m_adwEnemyGuildID[ENEMY_GUILD_SLOT_MAX_COUNT];

		DWORD m_dwMemberLevelSummary;
		DWORD m_dwMemberLevelAverage;
		DWORD m_dwMemberExperienceSummary;

		BOOL m_bGuildEnable;
#if defined(ENABLE_LOADING_PERFORMANCE)
		PyObject* m_ppyGuildBuildingList;
#endif
	private:
		std::map<uint8_t, std::pair<uint16_t, uint32_t>> m_mapDungeonHistoryInfo;
		TGuildDungeonBossDamageInfo dungeonBossDmgRank[10];
		uint64_t dungeonBossRankMyDmg;
		std::vector<TGuildGuardItemData> m_vecGuildItems[2];

		std::vector<TGuildLog> guildLogsVec;
		uint32_t lastLogID;
		uint32_t	nextLogsRefreshTime;
};
//martysama0134's ceqyqttoaf71vasf9t71218
