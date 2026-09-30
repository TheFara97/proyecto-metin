#include "stdafx.h"
#ifdef __TEAM_MEMBER_STATUS__
#include "char.h"
#include "char_manager.h"
#include "desc.h"
#include "p2p.h"
#include "config.h"
#include "db.h"
#include "TeamList.hpp"
#include <boost/algorithm/string/replace.hpp>
namespace
{
const std::string sLoadQuery{"SELECT mName, mLang FROM srv1_common.gmlist WHERE bOnList > 0"};
const std::string sMemberStatusCMD{"UpdateTeamMemberStatus %s %s %d"};
const std::string sMemberClearCMD{"ClearTeamMemberList"};
} 
CTeamListManager::CTeamListManager()
{
	Initialize();
}
void CTeamListManager::Initialize()
{
	m_team_members.clear();
	std::unique_ptr<SQLMsg> pMsg(DBManager::Instance().DirectQuery(sLoadQuery.c_str()));
	if (pMsg->uiSQLErrno != 0)
	{
		sys_err("Cannot fetch team list!");
		return;
	}
	if (pMsg->Get() && pMsg->Get()->pSQLResult)
	{
		MYSQL_ROW row{};
		while ((row = mysql_fetch_row(pMsg->Get()->pSQLResult)))
		{
			std::string sTrimmedLang{row[1]};
			boost::algorithm::replace_all(sTrimmedLang, " ", "");
			m_team_members[row[0]] = {sTrimmedLang, false};
			if (test_server)
				sys_log(0, "Loaded: %s, lang %s", row[0], sTrimmedLang.c_str());
		}
	}
	sys_log(0, "%d team members were loaded!", m_team_members.size());
}
void CTeamListManager::RegisterTeamMember(const LPCHARACTER pChar)
{
	if (auto fIt = m_team_members.find(pChar->GetName()); fIt != m_team_members.end())
	{
		fIt->second.second = true;
		TPacketGGTeamList p{};
		p.bHeader = HEADER_GG_TEAM_MEMBER;
		p.bSubHeader = SUBHEADER_GG_TEAM_MEMBER_ADD;
		strlcpy(p.szName, pChar->GetName(), sizeof(p.szName));
		P2P_MANAGER::instance().Send(&p, sizeof(TPacketGGTeamList));
		CHARACTER_MANAGER::instance().for_each_pc([&p](auto& rChar) {
			if (rChar->GetDesc() && rChar->GetDesc()->IsPhase(PHASE_GAME))
				CTeamListManager::instance().SendGMList(rChar, p.szName);
		});
	}
}
void CTeamListManager::RegisterTeamMember(const TPacketGGTeamList* pPacket)
{
	if (auto fIt = m_team_members.find(pPacket->szName); fIt != m_team_members.end())
	{
		fIt->second.second = true;
		CHARACTER_MANAGER::instance().for_each_pc([&pPacket](auto& rChar) {
			if (rChar->GetDesc() && rChar->GetDesc()->IsPhase(PHASE_GAME))
				CTeamListManager::instance().SendGMList(rChar, pPacket->szName);
		});
	}
}
void CTeamListManager::RemoveTeamMember(const std::string& sName)
{
	if (auto fIt = m_team_members.find(sName); fIt != m_team_members.end())
	{
		fIt->second.second = false;
		TPacketGGTeamList p{};
		p.bHeader = HEADER_GG_TEAM_MEMBER;
		p.bSubHeader = SUBHEADER_GG_TEAM_MEMBER_REMOVE;
		strlcpy(p.szName, sName.c_str(), sizeof(p.szName));
		P2P_MANAGER::instance().Send(&p, sizeof(TPacketGGTeamList));
		CHARACTER_MANAGER::instance().for_each_pc([&sName](auto& rChar) {
			if (rChar->GetDesc() && rChar->GetDesc()->IsPhase(PHASE_GAME))
				CTeamListManager::instance().SendGMList(rChar, sName);
		});
	}
}
void CTeamListManager::RemoveTeamMember(const TPacketGGTeamList* pPacket)
{
	if (auto fIt = m_team_members.find(pPacket->szName); fIt != m_team_members.end())
	{
		fIt->second.second = false;
		CHARACTER_MANAGER::instance().for_each_pc([&pPacket](auto& rChar) {
			if (rChar->GetDesc() && rChar->GetDesc()->IsPhase(PHASE_GAME))
				CTeamListManager::instance().SendGMList(rChar, pPacket->szName);
		});
	}
}
void CTeamListManager::ClearTeamMember()
{
	CHARACTER_MANAGER::instance().for_each_pc([](auto& rChar) {
		if (rChar->GetDesc() && rChar->GetDesc()->IsPhase(PHASE_GAME))
			CTeamListManager::instance().ClearGMList(rChar);
	});
	Initialize();
	CHARACTER_MANAGER::instance().for_each_pc([](auto& rChar) {
		if (rChar->GetDesc() && rChar->GetDesc()->IsPhase(PHASE_GAME))
			CTeamListManager::instance().SendGMList(rChar);
	});
}
void CTeamListManager::RequestReload()
{
	ClearTeamMember();
	TPacketGGTeamList p{};
	p.bHeader = HEADER_GG_TEAM_MEMBER;
	p.bSubHeader = SUBHEADER_GG_TEAM_MEMBER_RELOAD;
	P2P_MANAGER::instance().Send(&p, sizeof(TPacketGGTeamList));
}
void CTeamListManager::SendGMList(const LPCHARACTER pChar, const std::string& rKey)
{
	if (rKey.empty())
	{
		for (const auto& [rKey, rValue] : m_team_members)
			pChar->ChatPacket(CHAT_TYPE_COMMAND, sMemberStatusCMD.c_str(), rKey.c_str(),
							  rValue.first.c_str(), static_cast<int>(rValue.second));
	}
	else if (auto fIt = m_team_members.find(rKey); fIt != m_team_members.end())
		pChar->ChatPacket(CHAT_TYPE_COMMAND, sMemberStatusCMD.c_str(), rKey.c_str(),
						  fIt->second.first.c_str(), static_cast<int>(fIt->second.second));
	else
		sys_err("%s team member is not present in team member list!", rKey.c_str());
}
void CTeamListManager::ClearGMList(const LPCHARACTER pChar)
{
	pChar->ChatPacket(CHAT_TYPE_COMMAND, sMemberClearCMD.c_str());
}
#endif
