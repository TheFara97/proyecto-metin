#include "stdafx.h"
#include "utils.h"
#include "char.h"
#include "affect.h"
#include "packet.h"
#include "constants.h"
#include "desc.h"
#include "desc_manager.h"
#include "char_manager.h"
#ifdef ENABLE_NEWSTUFF
#include "../../common/PulseManager.h"
#endif
#include "entity.h"

extern int passes_per_sec;

#ifdef TITLE_SYSTEM_BYLUZER
const int titles_bonuses[TITLE_MAX_NUM][2] = {
                                                {63, 10},
                                                {63, 15},
                                                {63, 25},
                                                {106, 8},
                                                {106, 12},
                                                {106, 20},
                                                {108, 8},
                                                {108, 12},
                                                {108, 20},
                                                {113, 8},
                                                {113, 12},
                                                {113, 20},
};
void CHARACTER::TitleSendGeneralInfo()
{
	if (!GetDesc())
		return;
    BYTE now_active_id = 99;
    TPacketGCTitleActive pTitle;
    pTitle.bHeader = HEADER_GC_TITLE_INFO;
    for (int i = 0; i < TITLE_MAX_NUM; i++)
    {
        if (m_pTitle[i].active == 2)
        {
            now_active_id = i;
            break;
        }
    }
    thecore_memcpy(&pTitle.titles, m_pTitle, sizeof(TPlayerTitle) * TITLE_MAX_NUM);
    pTitle.actually_active = now_active_id;
    
    GetDesc()->Packet(&pTitle, sizeof(TPacketGCTitleActive));

    if (FindAffect(AFFECT_TITLES))
        RemoveAffect(AFFECT_TITLES);
    if (now_active_id != 99)
        ActiveTitle(now_active_id);
}
void CHARACTER::ActiveTitle(BYTE id)
{
	if (!GetDesc())
		return;

    if (titles_bonuses[id][0] > 0) {
        AddAffect(AFFECT_TITLES, aApplyInfo[titles_bonuses[id][0]].bPointType, titles_bonuses[id][1], AFF_NONE, 5*365*24*60, 0, false);
    }

    TPacketGCTitleActiveNow pTitle;
    pTitle.bHeader = HEADER_GC_TITLE_ACTIVE;
    pTitle.id = id;
    
    GetDesc()->Packet(&pTitle, sizeof(TPacketGCTitleActiveNow));

    UpdatePacket();
}

void CHARACTER::DetachTitle(BYTE id)
{
    if (id>=255)
        return;
    if (id<0)
        return;

    if (m_pTitle[id].active == 2) {
        for (int i = 0; i < TITLE_MAX_NUM; i++)
        {
            if (m_pTitle[i].active == 2)
            {
                m_pTitle[i].active = 1;
            }
        }

        TitleSendGeneralInfo();
    }
    ComputePoints();
    UpdatePacket();
}

void CHARACTER::ActiveTitleByClient(BYTE id)
{
#ifdef ENABLE_NEWSTUFF
    if (!PulseManager::Instance().IncreaseClock(GetPlayerID(), ePulse::ChangeTitle, std::chrono::milliseconds(5000)))
    {
        ChatPacket(CHAT_TYPE_INFO, "[LS;2200;%f]", PULSEMANAGER_CLOCK_TO_SEC2(GetPlayerID(), ePulse::ChangeTitle));
        return;
    }
#endif

    if (m_pTitle[id].active == 0)
    {
        ChatPacket(CHAT_TYPE_INFO, "[LS;2280]");
        return;
    }
    if (m_pTitle[id].active == 2)
    {
        ChatPacket(CHAT_TYPE_INFO, "[LS;2281]");
        return;
    }

	SetCooldownTitle();

    for (int i = 0; i < TITLE_MAX_NUM; i++)
    {
        if (m_pTitle[i].active == 2)
        {
            m_pTitle[i].active = 1;
            break;
        }
    }

    m_pTitle[id].active = 2;
    TitleSendGeneralInfo();
}

bool CHARACTER::IsTitleActive(int id)
{
    if (m_pTitle[id].active == 2)
        return true;
    else
        return false;
}
#endif