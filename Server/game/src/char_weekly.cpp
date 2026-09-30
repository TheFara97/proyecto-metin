#include "stdafx.h"
#include "utils.h"
#include "char.h"
#include "affect.h"
#include "packet.h"
#include "constants.h"
#include "desc.h"
#include "desc_manager.h"
#include "char_manager.h"
#include "entity.h"
#include "questmanager.h"
#include "db.h"
#include <time.h>

extern int passes_per_sec;

enum EWeeklyPage
{
    WEEKLY_MONSTERS = 0,
    WEEKLY_BOSSES = 1,
    WEEKLY_METINS = 2,
    WEEKLY_DUNGS = 3,
    WEEKLY_DS = 4,
    WEEKLY_REFINE = 5,
    WEEKLY_SM = 6,
};

#ifdef WEEKLY_RANK_BYLUZER
void CHARACTER::SelectWeeklyRankPage(BYTE page)
{
    if (!GetDesc())
        return;
    if (page < 0 || page > 6)
        return;

    TPacketGCSelectWeeklyPage p;
    p.bHeader = HEADER_GC_WEEKLY_PAGE;
    p.page = page;

	int weekly_season;
	std::unique_ptr<SQLMsg> pkMsg2(DBManager::instance().DirectQuery("SELECT season from weekly_season where weekly=1"));
	auto uiNumRows2 = pkMsg2->Get()->uiNumRows;
	if (uiNumRows2 == 0)
		return;
	MYSQL_ROW row2 = mysql_fetch_row(pkMsg2->Get()->pSQLResult);
	str_to_number(weekly_season, row2[0]);

    p.season = weekly_season;

	int iPulse = thecore_pulse();

	if (iPulse - GetCooldownWeekly()  < PASSES_PER_SEC(3))
	{
        p.can_select = false;
        GetDesc()->Packet(&p, sizeof(TPacketGCSelectWeeklyPage));
		return;
	}

    p.can_select = true;
    GetDesc()->Packet(&p, sizeof(TPacketGCSelectWeeklyPage));

    SetWeeklyLoadCount(GetWeeklyLoadCount()+1);

	if (GetWeeklyLoadCount() >= 15)
	{
		SetWeeklyLoadCount(0);
		SetCooldownWeekly();
	}
    char szQuery[QUERY_MAX_LEN + 1];
    switch (page)
    {
        case WEEKLY_MONSTERS:
        {
            snprintf(szQuery, sizeof(szQuery), "w_monsters");
        }
        break;
        case WEEKLY_BOSSES:
        {
            snprintf(szQuery, sizeof(szQuery), "w_bosses");
        }
        break;
        case WEEKLY_METINS:
        {
            snprintf(szQuery, sizeof(szQuery), "w_metins");
        }
        break;
        case WEEKLY_DUNGS:
        {
            snprintf(szQuery, sizeof(szQuery), "w_dungs");
        }
        break;
        case WEEKLY_DS:
        {
            snprintf(szQuery, sizeof(szQuery), "w_ds");
        }
        break;
        case WEEKLY_REFINE:
        {
            snprintf(szQuery, sizeof(szQuery), "w_refine");
        }
        break;
        case WEEKLY_SM:
        {
            snprintf(szQuery, sizeof(szQuery), "w_sm");
        }
        break;
    }

    std::unique_ptr<SQLMsg> pkMsg(DBManager::instance().DirectQuery(
    "SELECT %s, player.name, player.job, player_index.empire "
    "FROM srv1_player.player "
    "LEFT JOIN srv1_player.player_index ON player_index.id = player.account_id "
    "WHERE weekly_season = %d "
    "ORDER BY %s DESC "
    "LIMIT 10", 
    szQuery, weekly_season, szQuery, szQuery));

    if (pkMsg->uiSQLErrno != 0)
        return;

    DWORD i = 0;
    auto uiNumRows = pkMsg->Get()->uiNumRows;
    MYSQL_ROW row;

    TPacketGCSendWeeklyRank p2;
    p2.bHeader = HEADER_GC_WEEKLY_RANK;

    while ((row = mysql_fetch_row(pkMsg->Get()->pSQLResult)))
    {
        p2.rankingData[i] = TPacketGCTableWeeklyRank();
        p2.rankingData[i].rank = i;
        str_to_number(p2.rankingData[i].points, row[0]);
        strncpy(p2.rankingData[i].name, row[1], sizeof(p2.rankingData[i].name));
        str_to_number(p2.rankingData[i].job, row[2]);
        str_to_number(p2.rankingData[i].empire, row[3]);
        i++;
    }

    if (uiNumRows < 10)
    {
        while (i < 10)
        {
            p2.rankingData[i] = TPacketGCTableWeeklyRank();
            p2.rankingData[i].rank = i;
            strncpy(p2.rankingData[i].name, "", sizeof(p2.rankingData[i].name));
            p2.rankingData[i].points = 0;
            p2.rankingData[i].job = 0;
            p2.rankingData[i].empire = 0;
            i++;
        }
    }

	GetDesc()->Packet(&p2, sizeof(p2));
}
#endif