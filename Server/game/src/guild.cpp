#include "stdafx.h"
#include "utils.h"
#include "config.h"
#include "char.h"
#include "packet.h"
#include "desc_client.h"
#include "buffer_manager.h"
#include "char_manager.h"
#include "db.h"
#include "guild.h"
#include "guild_manager.h"
#include "affect.h"
#include "p2p.h"
#include "questmanager.h"
#include "building.h"
#include "dungeon.h"
#include "item.h"
#include "item_manager.h"
#include "locale_service.h"
#include "log.h"
#include "questmanager.h"

#define ENABLE_GUILD_COMMENT_ANTIFLOOD
#ifdef ENABLE_GUILD_COMMENT_ANTIFLOOD
#include "../../common/PulseManager.h"
#endif

SGuildMember::SGuildMember(LPCHARACTER ch, BYTE grade, DWORD offer_exp)
: pid(ch->GetPlayerID()), grade(grade), is_general(0), job(ch->GetJob()), level(ch->GetLevel()), offer_exp(offer_exp), name(ch->GetName())
{}
SGuildMember::SGuildMember(DWORD pid, BYTE grade, BYTE is_general, BYTE job, BYTE level, DWORD offer_exp, char* name)
: pid(pid), grade(grade), is_general(is_general), job(job), level(level), offer_exp(offer_exp), name(name)
{}

namespace
{
	struct FGuildNameSender
	{
		FGuildNameSender(DWORD id, const char* guild_name) : id(id), name(guild_name)
		{
			p.header = HEADER_GC_GUILD;
			p.subheader = GUILD_SUBHEADER_GC_GUILD_NAME;
			p.size = sizeof(p) + sizeof(DWORD) + GUILD_NAME_MAX_LEN;
		}

		void operator() (LPCHARACTER ch)
		{
			LPDESC d = ch->GetDesc();

			if (d)
			{
				d->BufferedPacket(&p, sizeof(p));
				d->BufferedPacket(&id, sizeof(id));
				d->Packet(name, GUILD_NAME_MAX_LEN);
			}
		}

		DWORD id;
		const char * name;
		TPacketGCGuild p;
	};
}

CGuild::CGuild(TGuildCreateParameter & cp)
{
	Initialize();

	m_general_count = 0;

	m_iMemberCountBonus = 0;

	strlcpy(m_data.name, cp.name, sizeof(m_data.name));
	m_data.master_pid = cp.master->GetPlayerID();
	strlcpy(m_data.grade_array[0].grade_name, "[LS;927]", sizeof(m_data.grade_array[0].grade_name));
	m_data.grade_array[0].auth_flag = GUILD_AUTH_ADD_MEMBER | GUILD_AUTH_REMOVE_MEMBER | GUILD_AUTH_NOTICE | GUILD_AUTH_USE_SKILL | GUILD_AUTH_MANAGE_GUARD | GUILD_AUTH_MANAGE_DUNGEON | GUILD_AUTH_LOGS | GUILD_AUTH_EDIT_NOTICE;

	for (int i = 1; i < GUILD_GRADE_COUNT; ++i)
	{
		strlcpy(m_data.grade_array[i].grade_name, "[LS;928]", sizeof(m_data.grade_array[i].grade_name));
		m_data.grade_array[i].auth_flag = 0;
	}

	auto pmsg(DBManager::instance().DirectQuery(
				"INSERT INTO guild%s(name, master, sp, level, exp, skill_point, skill, guard_active_set, free_ticket, paid_ticket, paid_ticket_limit, ticket_refresh_time) "
				"VALUES('%s', %u, 1000, 1, 0, 0, '\\0\\0\\0\\0\\0\\0\\0\\0\\0\\0\\0\\0', 0, 3, 0, 0, %d)",
				get_table_postfix(), m_data.name, m_data.master_pid, m_data.ticketRefreshTime));

	// TODO if error occur?
	m_data.guild_id = pmsg->Get()->uiInsertID;

	for (int i = 0; i < GUILD_GRADE_COUNT; ++i)
	{
		DBManager::instance().Query("INSERT INTO guild_grade%s VALUES(%u, %d, '%s', %d)",
				get_table_postfix(),
				m_data.guild_id,
				i + 1,
				m_data.grade_array[i].grade_name,
				m_data.grade_array[i].auth_flag);
	}

	ComputeGuildPoints();
	m_data.power	= m_data.max_power;
	
	DBManager::instance().Query("INSERT INTO guild_notes%s VALUES(%u, 'Notice')", get_table_postfix(), m_data.guild_id);
	guildNote = "Notice";
	m_dwLastNoteChangeTime = 0;
	
	m_data.ladder_point	= 0;
	db_clientdesc->DBPacket(HEADER_GD_GUILD_CREATE, 0, &m_data.guild_id, sizeof(DWORD));

	TPacketGuildSkillUpdate guild_skill;
	guild_skill.guild_id = m_data.guild_id;
	guild_skill.amount = 0;
	guild_skill.skill_point = 0;
	memset(guild_skill.skill_levels, 0, GUILD_SKILL_COUNT);

	db_clientdesc->DBPacket(HEADER_GD_GUILD_SKILL_UPDATE, 0, &guild_skill, sizeof(guild_skill));

	// TODO GUILD_NAME
	CHARACTER_MANAGER::instance().for_each_pc(FGuildNameSender(GetID(), GetName()));
	RequestAddMember(cp.master, GUILD_LEADER_GRADE);
}

void CGuild::Initialize()
{
	memset(&m_data, 0, sizeof(m_data));
	m_data.level = 1;

	for (int i = 0; i < GUILD_SKILL_COUNT; ++i)
		abSkillUsable[i] = true;

	m_iMemberCountBonus = 0;
	guildNote = "";
	m_dwLastNoteChangeTime = 0;
	
	m_vecGuildItems[0].clear();
	m_vecGuildItems[1].clear();
	m_dwLastUseGuardItem = 0;

	m_mapGuildGuardItemsBonuses.clear();
	m_dwNextCanChangeBonusActiveStatusTime = 0;
	
	guildLogsVec.clear();
	lastLogID = 0;
	nextLogsRefreshTime = 0;
}

CGuild::~CGuild()
{
}

void CGuild::RequestAddMember(LPCHARACTER ch, int grade)
{
	if (ch->GetGuild())
		return;

	TPacketGDGuildAddMember gd;

	if (m_member.find(ch->GetPlayerID()) != m_member.end())
	{
		sys_err("Already a member in guild %s[%d]", ch->GetName(), ch->GetPlayerID());
		return;
	}

	gd.dwPID = ch->GetPlayerID();
	gd.dwGuild = GetID();
	gd.bGrade = grade;

	db_clientdesc->DBPacket(HEADER_GD_GUILD_ADD_MEMBER, 0, &gd, sizeof(TPacketGDGuildAddMember));

	GuildLog(18, ch->GetName(), ch->GetPlayerID());
}

void CGuild::AddMember(TPacketDGGuildMember * p)
{
	TGuildMemberContainer::iterator it;

	if ((it = m_member.find(p->dwPID)) == m_member.end())
		m_member.emplace(p->dwPID, TGuildMember(p->dwPID, p->bGrade, p->isGeneral, p->bJob, p->bLevel, p->dwOffer, p->szName));
	else
	{
		TGuildMember & r_gm = it->second;
		r_gm.pid = p->dwPID;
		r_gm.grade = p->bGrade;
		r_gm.job = p->bJob;
		r_gm.offer_exp = p->dwOffer;
		r_gm.is_general = p->isGeneral;
	}

	CGuildManager::instance().Link(p->dwPID, this);

	SendListOneToAll(p->dwPID);

	LPCHARACTER ch = CHARACTER_MANAGER::instance().FindByPID(p->dwPID);

	sys_log(0, "GUILD: AddMember PID %u, grade %u, job %u, level %u, offer %u, name %s ptr %p",
			p->dwPID, p->bGrade, p->bJob, p->bLevel, p->dwOffer, p->szName, get_pointer(ch));

	if (ch)
		LoginMember(ch);
	else
		P2PLoginMember(p->dwPID);
}

bool CGuild::RequestRemoveMember(DWORD pid)
{
	TGuildMemberContainer::iterator it;

	if ((it = m_member.find(pid)) == m_member.end())
		return false;

	if (it->second.grade == GUILD_LEADER_GRADE)
		return false;

	TPacketGuild gd_guild;

	gd_guild.dwGuild = GetID();
	gd_guild.dwInfo = pid;

	db_clientdesc->DBPacket(HEADER_GD_GUILD_REMOVE_MEMBER, 0, &gd_guild, sizeof(TPacketGuild));
	return true;
}

bool CGuild::RemoveMember(DWORD pid)
{
	sys_log(0, "Receive Guild P2P RemoveMember");
	TGuildMemberContainer::iterator it;

	if ((it = m_member.find(pid)) == m_member.end())
		return false;

	if (it->second.grade == GUILD_LEADER_GRADE)
		return false;

	if (it->second.is_general)
		m_general_count--;

	m_member.erase(it);
	SendOnlineRemoveOnePacket(pid);

	CGuildManager::instance().Unlink(pid);

	LPCHARACTER ch = CHARACTER_MANAGER::instance().FindByPID(pid);

	if (ch)
	{
		m_memberOnline.erase(ch);
		ch->SetGuild(NULL);
	}

	if (g_bGuildInviteLimit)
		DBManager::instance().Query("REPLACE INTO guild_invite_limit VALUES(%d, %d)", GetID(), get_global_time());

	return true;
}

void CGuild::P2PLoginMember(DWORD pid)
{
	if (m_member.find(pid) == m_member.end())
	{
		sys_err("GUILD [%d] is not a memeber of guild.", pid);
		return;
	}

	m_memberP2POnline.emplace(pid);

	// Login event occur + Send List
	TGuildMemberOnlineContainer::iterator it;

	for (it = m_memberOnline.begin(); it!=m_memberOnline.end();++it)
		SendLoginPacket(*it, pid);
}

void CGuild::LoginMember(LPCHARACTER ch)
{
	if (m_member.find(ch->GetPlayerID()) == m_member.end())
	{
		sys_err("GUILD %s[%d] is not a memeber of guild.", ch->GetName(), ch->GetPlayerID());
		return;
	}

	ch->SetGuild(this);

	quest::PC* pPC = quest::CQuestManager::instance().GetPC(ch->GetPlayerID());
	if (get_global_time() >= pPC->GetFlag("guild_manage.exp_limit_refresh"))
	{
		pPC->SetFlag("guild_manage.today_exp", 0);
		pPC->SetFlag("guild_manage.exp_limit_refresh", GetMidnightTime());
	}

	CheckRefreshTicketLimits();

	// Login event occur + Send List
	TGuildMemberOnlineContainer::iterator it;

	for (it = m_memberOnline.begin(); it!=m_memberOnline.end();++it)
		SendLoginPacket(*it, ch);

	m_memberOnline.emplace(ch);

	SendAllGradePacket(ch);
	SendGuildInfoPacket(ch);
	SendListPacket(ch);
	SendSkillInfoPacket(ch);
	SendEnemyGuild(ch);
}

void CGuild::P2PLogoutMember(DWORD pid)
{
	if (m_member.find(pid)==m_member.end())
	{
		sys_err("GUILD [%d] is not a memeber of guild.", pid);
		return;
	}

	m_memberP2POnline.erase(pid);

	// Logout event occur
	TGuildMemberOnlineContainer::iterator it;
	for (it = m_memberOnline.begin(); it!=m_memberOnline.end();++it)
	{
		SendLogoutPacket(*it, pid);
	}
}

void CGuild::LogoutMember(LPCHARACTER ch)
{
	if (m_member.find(ch->GetPlayerID())==m_member.end())
	{
		sys_err("GUILD %s[%d] is not a memeber of guild.", ch->GetName(), ch->GetPlayerID());
		return;
	}

	m_memberOnline.erase(ch);

	// Logout event occur
	TGuildMemberOnlineContainer::iterator it;
	for (it = m_memberOnline.begin(); it!=m_memberOnline.end();++it)
	{
		SendLogoutPacket(*it, ch);
	}
}

void CGuild::SendOnlineRemoveOnePacket(DWORD pid)
{
	TPacketGCGuild pack;
	pack.header = HEADER_GC_GUILD;
	pack.size = sizeof(pack)+4;
	pack.subheader = GUILD_SUBHEADER_GC_REMOVE;

	TEMP_BUFFER buf;
	buf.write(&pack,sizeof(pack));
	buf.write(&pid, sizeof(pid));

	TGuildMemberOnlineContainer::iterator it;

	for (it = m_memberOnline.begin(); it!=m_memberOnline.end();++it)
	{
		LPDESC d = (*it)->GetDesc();

		if (d)
			d->Packet(buf.read_peek(), buf.size());
	}
}

void CGuild::SendAllGradePacket(LPCHARACTER ch)
{
	LPDESC d = ch->GetDesc();
	if (!d)
		return;

	TPacketGCGuild pack;
	pack.header = HEADER_GC_GUILD;
	pack.size = sizeof(pack)+1+GUILD_GRADE_COUNT*(sizeof(TGuildGrade)+1);
	pack.subheader = GUILD_SUBHEADER_GC_GRADE;

	TEMP_BUFFER buf;

	buf.write(&pack, sizeof(pack));
	BYTE n = 15;
	buf.write(&n, 1);

	for (int i=0;i<GUILD_GRADE_COUNT;i++)
	{
		BYTE j = i+1;
		buf.write(&j, 1);
		buf.write(&m_data.grade_array[i], sizeof(TGuildGrade));
	}

	d->Packet(buf.read_peek(), buf.size());
}

void CGuild::SendListOneToAll(LPCHARACTER ch)
{
	SendListOneToAll(ch->GetPlayerID());
}

void CGuild::SendListOneToAll(DWORD pid)
{
	TPacketGCGuild pack;
	pack.header = HEADER_GC_GUILD;
	pack.size = sizeof(TPacketGCGuild);
	pack.subheader = GUILD_SUBHEADER_GC_LIST;

	pack.size += sizeof(TGuildMemberPacketData);

	char c[CHARACTER_NAME_MAX_LEN+1];
	memset(c, 0, sizeof(c));

	TGuildMemberContainer::iterator cit = m_member.find(pid);
	if (cit == m_member.end())
		return;

	for (TGuildMemberOnlineContainer::iterator it = m_memberOnline.begin(); it!= m_memberOnline.end(); ++it)
	{
		LPDESC d = (*it)->GetDesc();
		if (!d)
			continue;

		TEMP_BUFFER buf;

		buf.write(&pack, sizeof(pack));

		cit->second._dummy = 1;

		buf.write(&(cit->second), sizeof(DWORD) * 3 +1);
		buf.write(cit->second.name.c_str(), cit->second.name.length());
		buf.write(c, CHARACTER_NAME_MAX_LEN + 1 - cit->second.name.length());
		d->Packet(buf.read_peek(), buf.size());
	}
}

void CGuild::SendListPacket(LPCHARACTER ch)
{
	LPDESC d;
	if (!(d=ch->GetDesc()))
		return;

	TPacketGCGuild pack;
	pack.header = HEADER_GC_GUILD;
	pack.size = sizeof(TPacketGCGuild);
	pack.subheader = GUILD_SUBHEADER_GC_LIST;

	pack.size += sizeof(TGuildMemberPacketData) * m_member.size();

	TEMP_BUFFER buf;

	buf.write(&pack,sizeof(pack));

	char c[CHARACTER_NAME_MAX_LEN+1];

	for (TGuildMemberContainer::iterator it = m_member.begin(); it != m_member.end(); ++it)
	{
		it->second._dummy = 1;

		buf.write(&(it->second), sizeof(DWORD)*3+1);

		strlcpy(c, it->second.name.c_str(), MIN(sizeof(c), it->second.name.length() + 1));

		buf.write(c, CHARACTER_NAME_MAX_LEN+1 );

		if ( test_server )
			sys_log(0 ,"name %s job %d  ", it->second.name.c_str(), it->second.job );
	}

	d->Packet(buf.read_peek(), buf.size());

	for (TGuildMemberOnlineContainer::iterator it = m_memberOnline.begin(); it != m_memberOnline.end(); ++it)
	{
		SendLoginPacket(ch, *it);
	}

	for (TGuildMemberP2POnlineContainer::iterator it = m_memberP2POnline.begin(); it != m_memberP2POnline.end(); ++it)
	{
		SendLoginPacket(ch, *it);
	}
}

void CGuild::SendLoginPacket(LPCHARACTER ch, LPCHARACTER chLogin)
{
	SendLoginPacket(ch, chLogin->GetPlayerID());
}

void CGuild::SendLoginPacket(LPCHARACTER ch, DWORD pid)
{
	/*
	   Login Packet
	   header 4
	   pid 4
	 */
	if (!ch->GetDesc())
		return;

	TPacketGCGuild pack;
	pack.header = HEADER_GC_GUILD;
	pack.size = sizeof(pack)+4;
	pack.subheader = GUILD_SUBHEADER_GC_LOGIN;

	TEMP_BUFFER buf;

	buf.write(&pack, sizeof(pack));

	buf.write(&pid, 4);

	ch->GetDesc()->Packet(buf.read_peek(), buf.size());
}

void CGuild::SendLogoutPacket(LPCHARACTER ch, LPCHARACTER chLogout)
{
	SendLogoutPacket(ch, chLogout->GetPlayerID());
}

void CGuild::SendLogoutPacket(LPCHARACTER ch, DWORD pid)
{
	/*
	   Logout Packet
	   header 4
	   pid 4
	 */
	if (!ch->GetDesc())
		return;

	TPacketGCGuild pack;
	pack.header = HEADER_GC_GUILD;
	pack.size = sizeof(pack)+4;
	pack.subheader = GUILD_SUBHEADER_GC_LOGOUT;

	TEMP_BUFFER buf;

	buf.write(&pack, sizeof(pack));
	buf.write(&pid, 4);

	ch->GetDesc()->Packet(buf.read_peek(), buf.size());
}

void CGuild::LoadGuildMemberData(SQLMsg* pmsg)
{
	if (!CGuildManager::instance().FindGuild(m_data.guild_id))
		return;

	if (pmsg->Get()->uiNumRows == 0)
		return;

	m_general_count = 0;

	m_member.clear();

	for (uint i = 0; i < pmsg->Get()->uiNumRows; ++i)
	{
		MYSQL_ROW row = mysql_fetch_row(pmsg->Get()->pSQLResult);

		DWORD pid = strtoul(row[0], (char**) NULL, 10);
		BYTE grade = (BYTE) strtoul(row[1], (char**) NULL, 10);
		BYTE is_general = 0;

		if (row[2] && *row[2] == '1')
			is_general = 1;

		DWORD offer = strtoul(row[3], (char**) NULL, 10);
		BYTE level = (BYTE)strtoul(row[4], (char**) NULL, 10);
		BYTE job = (BYTE)strtoul(row[5], (char**) NULL, 10);
		char * name = row[6];

		if (is_general)
			m_general_count++;

		m_member.emplace(pid, TGuildMember(pid, grade, is_general, job, level, offer, name));
		CGuildManager::instance().Link(pid, this);
	}
}

void CGuild::LoadGuildGradeData(SQLMsg* pmsg)
{
	for (uint i = 0; i < pmsg->Get()->uiNumRows; ++i)
	{
		MYSQL_ROW row = mysql_fetch_row(pmsg->Get()->pSQLResult);
		BYTE grade = 0;
		str_to_number(grade, row[0]);
		char * name = row[1];
		DWORD auth = strtoul(row[2], NULL, 10);

		if (grade >= 1 && grade <= 15)
		{
			strlcpy(m_data.grade_array[grade-1].grade_name, name, sizeof(m_data.grade_array[grade-1].grade_name));
			m_data.grade_array[grade-1].auth_flag = auth;
		}
	}
}
void CGuild::LoadGuildData(SQLMsg* pmsg)
{
	if (pmsg->Get()->uiNumRows == 0)
	{
		sys_err("Query failed: getting guild data %s", pmsg->stQuery.c_str());
		return;
	}

	MYSQL_ROW row = mysql_fetch_row(pmsg->Get()->pSQLResult);
	m_data.master_pid = strtoul(row[0], (char **)NULL, 10);
	m_data.level = (BYTE)strtoul(row[1], (char **)NULL, 10);
	m_data.exp = strtoul(row[2], (char **)NULL, 10);
	strlcpy(m_data.name, row[3], sizeof(m_data.name));

	m_data.skill_point = (BYTE) strtoul(row[4], (char **) NULL, 10);
	if (row[5])
		thecore_memcpy(m_data.abySkill, row[5], sizeof(BYTE) * GUILD_SKILL_COUNT);
	else
		memset(m_data.abySkill, 0, sizeof(BYTE) * GUILD_SKILL_COUNT);

	m_data.power = MAX(0, strtoul(row[6], (char **) NULL, 10));

	str_to_number(m_data.ladder_point, row[7]);

	if (m_data.ladder_point < 0)
		m_data.ladder_point = 0;

	str_to_number(m_data.win, row[8]);
	str_to_number(m_data.draw, row[9]);
	str_to_number(m_data.loss, row[10]);
	str_to_number(m_data.gold, row[11]);
	m_data.guard_active_set_id = (uint8_t)strtoul(row[12], (char**)NULL, 10);
	str_to_number(m_data.tickets[GUILD_TICKETS_TYPE_FREE], row[13]);
	str_to_number(m_data.tickets[GUILD_TICKETS_TYPE_PAID], row[14]);
	str_to_number(m_data.tickets[GUILD_TICKETS_TYPE_PAID_LIMIT], row[15]);
	str_to_number(m_data.ticketRefreshTime, row[16]);
	str_to_number(m_data.dung_idx, row[17]);
	str_to_number(m_data.dung_map_idx, row[18]);
	str_to_number(m_data.dung_start_time, row[19]);
	str_to_number(m_data.guard_active_expire_time, row[20]);
	str_to_number(m_data.donate_stone,   row[21]);
	str_to_number(m_data.donate_log,     row[22]);
	str_to_number(m_data.donate_plywood, row[23]);

	// If guardian bonus has expired while the server was offline, auto-deactivate
	if (m_data.guard_active_expire_time != 0 && get_global_time() > m_data.guard_active_expire_time)
	{
		m_data.guard_active_set_id = 0;
		m_data.guard_active_expire_time = 0;
		DBManager::instance().Query("UPDATE guild%s SET guard_active_set=0, guard_active_expire_time=0 WHERE id=%u", get_table_postfix(), m_data.guild_id);
	}

	ComputeGuildPoints();
}

void CGuild::Load(DWORD guild_id)
{
	Initialize();

	m_data.guild_id = guild_id;

	DBManager::instance().FuncQuery(msl::bind1st(std::mem_fn(&CGuild::LoadGuildData), this),
			"SELECT master, level, exp, name, skill_point, skill, sp, ladder_point, win, draw, loss, gold, guard_active_set, free_ticket, paid_ticket, paid_ticket_limit, ticket_refresh_time, dung_idx, dung_map_idx, dung_start_time, guard_active_expire_time, donate_stone, donate_log, donate_plywood FROM guild%s WHERE id = %u", get_table_postfix(), m_data.guild_id);

	sys_log(0, "GUILD: loading guild id %12s %u", m_data.name, guild_id);

	DBManager::instance().FuncQuery(msl::bind1st(std::mem_fn(&CGuild::LoadGuildGradeData), this),
			"SELECT grade, name, auth+0 FROM guild_grade%s WHERE guild_id = %u", get_table_postfix(), m_data.guild_id);

	DBManager::instance().FuncQuery(msl::bind1st(std::mem_fn(&CGuild::LoadGuildMemberData), this),
			"SELECT pid, grade, is_general, offer, level, job, name FROM guild_member%s, player%s WHERE guild_id = %u and pid = id", get_table_postfix(), get_table_postfix(), guild_id);

	DBManager::instance().FuncQuery(msl::bind1st(std::mem_fn(&CGuild::LoadGuildDungeonHistoryData), this),
		"SELECT dungeon_idx,done_count,best_time FROM guild_dungeons%s WHERE guild_id=%u", get_table_postfix(), guild_id);
}

void CGuild::LoadGuardItems()
{
	DBManager::instance().FuncQuery(msl::bind1st(std::mem_fn(&CGuild::LoadGuildGuardItemsData), this),
		"SELECT set_id,pos,vnum FROM guild_guard_items%s WHERE guild_id=%u", get_table_postfix(), m_data.guild_id);
}

void CGuild::SaveLevel()
{
	DBManager::instance().Query("UPDATE guild%s SET level=%d, exp=%u, skill_point=%d WHERE id = %u", get_table_postfix(), m_data.level,m_data.exp, m_data.skill_point,m_data.guild_id);
}

void CGuild::SendDBSkillUpdate(int amount)
{
	TPacketGuildSkillUpdate guild_skill;
	guild_skill.guild_id = m_data.guild_id;
	guild_skill.amount = amount;
	guild_skill.skill_point = m_data.skill_point;
	thecore_memcpy(guild_skill.skill_levels, m_data.abySkill, sizeof(BYTE) * GUILD_SKILL_COUNT);

	db_clientdesc->DBPacket(HEADER_GD_GUILD_SKILL_UPDATE, 0, &guild_skill, sizeof(guild_skill));
}

void CGuild::SaveSkill()
{
	char text[GUILD_SKILL_COUNT * 2 + 1];

	DBManager::instance().EscapeString(text, sizeof(text), (const char *) m_data.abySkill, sizeof(m_data.abySkill));
	DBManager::instance().Query("UPDATE guild%s SET sp = %d, skill_point=%d, skill='%s' WHERE id = %u",
			get_table_postfix(), m_data.power, m_data.skill_point, text, m_data.guild_id);
}

TGuildMember* CGuild::GetMember(DWORD pid)
{
	TGuildMemberContainer::iterator it = m_member.find(pid);
	if (it==m_member.end())
		return NULL;

	return &it->second;
}

DWORD CGuild::GetMemberPID(const std::string& strName)
{
	for ( TGuildMemberContainer::iterator iter = m_member.begin();
			iter != m_member.end(); iter++ )
	{
		if ( iter->second.name == strName ) return iter->first;
	}

	return 0;
}

void CGuild::__P2PUpdateGrade(SQLMsg* pmsg)
{
	if (pmsg->Get()->uiNumRows)
	{
		MYSQL_ROW row = mysql_fetch_row(pmsg->Get()->pSQLResult);

		int grade = 0;
		const char* name = row[1];
		int auth = 0;

		str_to_number(grade, row[0]);
		str_to_number(auth, row[2]);

		if (grade <= 0)
			return;

		grade--;

		if (0 != strcmp(m_data.grade_array[grade].grade_name, name))
		{
			strlcpy(m_data.grade_array[grade].grade_name, name, sizeof(m_data.grade_array[grade].grade_name));

			TPacketGCGuild pack;

			pack.header = HEADER_GC_GUILD;
			pack.size = sizeof(pack);
			pack.subheader = GUILD_SUBHEADER_GC_GRADE_NAME;

			TOneGradeNamePacket pack2;

			pack.size += sizeof(pack2);
			pack2.grade = grade + 1;
			strlcpy(pack2.grade_name, name, sizeof(pack2.grade_name));

			TEMP_BUFFER buf;

			buf.write(&pack,sizeof(pack));
			buf.write(&pack2,sizeof(pack2));

			for (TGuildMemberOnlineContainer::iterator it = m_memberOnline.begin(); it!=m_memberOnline.end(); ++it)
			{
				LPDESC d = (*it)->GetDesc();

				if (d)
					d->Packet(buf.read_peek(), buf.size());
			}
		}

		if (m_data.grade_array[grade].auth_flag != auth)
		{
			m_data.grade_array[grade].auth_flag = auth;

			TPacketGCGuild pack;
			pack.header = HEADER_GC_GUILD;
			pack.size = sizeof(pack);
			pack.subheader = GUILD_SUBHEADER_GC_GRADE_AUTH;

			TOneGradeAuthPacket pack2;
			pack.size+=sizeof(pack2);
			pack2.grade = grade+1;
			pack2.auth = auth;

			TEMP_BUFFER buf;
			buf.write(&pack,sizeof(pack));
			buf.write(&pack2,sizeof(pack2));

			for (TGuildMemberOnlineContainer::iterator it = m_memberOnline.begin(); it!=m_memberOnline.end(); ++it)
			{
				LPDESC d = (*it)->GetDesc();
				if (d)
				{
					d->Packet(buf.read_peek(), buf.size());
				}
			}
		}
	}
}

void CGuild::P2PChangeGrade(BYTE grade)
{
	DBManager::instance().FuncQuery(msl::bind1st(std::mem_fn(&CGuild::__P2PUpdateGrade),this),
			"SELECT grade, name, auth+0 FROM guild_grade%s WHERE guild_id = %u and grade = %d", get_table_postfix(), m_data.guild_id, grade);
}

namespace
{
	struct FSendChangeGrade
	{
		BYTE grade;
		TPacketGuild p;

		FSendChangeGrade(DWORD guild_id, BYTE grade) : grade(grade)
		{
			p.dwGuild = guild_id;
			p.dwInfo = grade;
		}

		void operator()()
		{
			db_clientdesc->DBPacket(HEADER_GD_GUILD_CHANGE_GRADE, 0, &p, sizeof(p));
		}
	};
}

void CGuild::ChangeGradeName(BYTE grade, const char* grade_name)
{
	if (grade == 1)
		return;

	if (grade < 1 || grade > 15)
	{
		sys_err("Wrong guild grade value %d", grade);
		return;
	}

	if (strlen(grade_name) > GUILD_NAME_MAX_LEN)
		return;

	if (!*grade_name)
		return;

	char text[GUILD_NAME_MAX_LEN * 2 + 1];

	DBManager::instance().EscapeString(text, sizeof(text), grade_name, strlen(grade_name));
	DBManager::instance().FuncAfterQuery(FSendChangeGrade(GetID(), grade), "UPDATE guild_grade%s SET name = '%s' where guild_id = %u and grade = %d", get_table_postfix(), text, m_data.guild_id, grade);

	grade--;
	strlcpy(m_data.grade_array[grade].grade_name, grade_name, sizeof(m_data.grade_array[grade].grade_name));

	TPacketGCGuild pack;
	pack.header = HEADER_GC_GUILD;
	pack.size = sizeof(pack);
	pack.subheader = GUILD_SUBHEADER_GC_GRADE_NAME;

	TOneGradeNamePacket pack2;
	pack.size+=sizeof(pack2);
	pack2.grade = grade+1;
	strlcpy(pack2.grade_name,grade_name, sizeof(pack2.grade_name));

	TEMP_BUFFER buf;
	buf.write(&pack,sizeof(pack));
	buf.write(&pack2,sizeof(pack2));

	for (TGuildMemberOnlineContainer::iterator it = m_memberOnline.begin(); it!=m_memberOnline.end(); ++it)
	{
		LPDESC d = (*it)->GetDesc();

		if (d)
			d->Packet(buf.read_peek(), buf.size());
	}
}

void CGuild::ChangeGradeAuth(BYTE grade, BYTE auth)
{
	if (grade == 1)
		return;

	if (grade < 1 || grade > 15)
	{
		sys_err("Wrong guild grade value %d", grade);
		return;
	}

	DBManager::instance().FuncAfterQuery(FSendChangeGrade(GetID(),grade), "UPDATE guild_grade%s SET auth = %d where guild_id = %u and grade = %d", get_table_postfix(), auth, m_data.guild_id, grade);

	grade--;

	m_data.grade_array[grade].auth_flag=auth;

	TPacketGCGuild pack;
	pack.header = HEADER_GC_GUILD;
	pack.size = sizeof(pack);
	pack.subheader = GUILD_SUBHEADER_GC_GRADE_AUTH;

	TOneGradeAuthPacket pack2;
	pack.size += sizeof(pack2);
	pack2.grade = grade + 1;
	pack2.auth = auth;

	TEMP_BUFFER buf;
	buf.write(&pack, sizeof(pack));
	buf.write(&pack2, sizeof(pack2));

	for (TGuildMemberOnlineContainer::iterator it = m_memberOnline.begin(); it != m_memberOnline.end(); ++it)
	{
		LPDESC d = (*it)->GetDesc();

		if (d)
			d->Packet(buf.read_peek(), buf.size());
	}
}

void CGuild::SendGuildInfoPacket(LPCHARACTER ch)
{
	LPDESC d = ch->GetDesc();

	if (!d)
		return;

	TPacketGCGuild pack;
	pack.header = HEADER_GC_GUILD;
	pack.size = sizeof(TPacketGCGuild) + sizeof(TPacketGCGuildInfo);
	pack.subheader = GUILD_SUBHEADER_GC_INFO;

	TPacketGCGuildInfo pack_sub;

	memset(&pack_sub, 0, sizeof(TPacketGCGuildInfo));
	pack_sub.member_count = GetMemberCount();
	pack_sub.max_member_count = GetMaxMemberCount();
	pack_sub.guild_id = m_data.guild_id;
	pack_sub.master_pid = m_data.master_pid;
	pack_sub.exp	= m_data.exp;
	pack_sub.level	= m_data.level;
	strlcpy(pack_sub.name, m_data.name, sizeof(pack_sub.name));
	pack_sub.gold	= m_data.gold;
	pack_sub.has_land	= HasLand();
	pack_sub.tickets[GUILD_TICKETS_TYPE_FREE] = m_data.tickets[GUILD_TICKETS_TYPE_FREE];
	pack_sub.tickets[GUILD_TICKETS_TYPE_PAID] = m_data.tickets[GUILD_TICKETS_TYPE_PAID];
	pack_sub.tickets[GUILD_TICKETS_TYPE_PAID_LIMIT] = m_data.tickets[GUILD_TICKETS_TYPE_PAID_LIMIT];

	quest::PC* pPC = quest::CQuestManager::instance().GetPC(ch->GetPlayerID());
	pack_sub.todayGivenExp = pPC->GetFlag("guild_manage.today_exp");
	pack_sub.todayGivenExpRefreshTime = pPC->GetFlag("guild_manage.exp_limit_refresh");

	sys_log(0, "GMC guild_name %s", m_data.name);
	sys_log(0, "GMC master %d", m_data.master_pid);

	d->BufferedPacket(&pack, sizeof(TPacketGCGuild));
	d->Packet(&pack_sub, sizeof(TPacketGCGuildInfo));
}

bool CGuild::OfferExp(LPCHARACTER ch, int amount)
{
	if (ch->IsAntyExp()) {
		ch->ChatPacket(CHAT_TYPE_INFO, "[LS;2023]");
		return false;
	}

	TGuildMemberContainer::iterator cit = m_member.find(ch->GetPlayerID());

	if (cit == m_member.end())
		return false;

	if (m_data.exp+amount < m_data.exp)
		return false;

	if (amount < 0)
		return false;

	if (ch->GetExp() < (DWORD) amount)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "[LS;929]");
		return false;
	}

	if (ch->GetExp() - (DWORD) amount > ch->GetExp())
	{
		sys_err("Wrong guild offer amount %d by %s[%u]", amount, ch->GetName(), ch->GetPlayerID());
		return false;
	}

	ch->PointChange(POINT_EXP, -amount);

	TPacketGuildExpUpdate guild_exp;
	guild_exp.guild_id = GetID();
	guild_exp.amount = amount / 100;
	db_clientdesc->DBPacket(HEADER_GD_GUILD_EXP_UPDATE, 0, &guild_exp, sizeof(guild_exp));
	GuildPointChange(POINT_EXP, amount / 100, true);

	cit->second.offer_exp += amount / 100;
	cit->second._dummy = 0;

	TPacketGCGuild pack;
	pack.header = HEADER_GC_GUILD;

	for (TGuildMemberOnlineContainer::iterator it = m_memberOnline.begin(); it != m_memberOnline.end(); ++it)
	{
		LPDESC d = (*it)->GetDesc();
		if (d)
		{
			pack.subheader = GUILD_SUBHEADER_GC_LIST;
			pack.size = sizeof(pack) + 13;
			d->BufferedPacket(&pack, sizeof(pack));
			d->Packet(&(cit->second), sizeof(DWORD) * 3 + 1);
		}
	}

	SaveMember(ch->GetPlayerID());

	TPacketGuildChangeMemberData gd_guild;

	gd_guild.guild_id = GetID();
	gd_guild.pid = ch->GetPlayerID();
	gd_guild.offer = cit->second.offer_exp;
	gd_guild.level = ch->GetLevel();
	gd_guild.grade = cit->second.grade;

	db_clientdesc->DBPacket(HEADER_GD_GUILD_CHANGE_MEMBER_DATA, 0, &gd_guild, sizeof(gd_guild));

	quest::PC* pPC = quest::CQuestManager::instance().GetPC(ch->GetPlayerID());
	if (pPC)
	{
		pPC->SetFlag("guild_manage.today_exp", pPC->GetFlag("guild_manage.today_exp") + amount);
		RefreshMyTodayGivenExpData(ch);
	}
	
	return true;
}

bool CGuild::AddGuildExp(LPCHARACTER ch, int amount)
{	
	//if (ch->IsAntyExp()) {
	//	ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("ANTY_EXP_GUILD_INFO"));
	//	return false;
	//}
	
	TGuildMemberContainer::iterator cit = m_member.find(ch->GetPlayerID());
	if (cit == m_member.end())
		return false;

	if (m_data.exp+amount < m_data.exp)
		return false;

	if (amount < 0)
		return false;

	/*if (ch->GetExp() < (DWORD)amount)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<길드> 제공하고자 하는 경험치가 남은 경험치보다 많습니다."));
		return false;
	}
	if (ch->GetExp() - (DWORD) amount > ch->GetExp())
	{
		sys_err("Wrong guild offer amount %d by %s[%u]", amount, ch->GetName(), ch->GetPlayerID());
		return false;
	}
	ch->PointChange(POINT_EXP, -amount);*/

	TPacketGuildExpUpdate guild_exp;
	guild_exp.guild_id = GetID();
	guild_exp.amount = amount;
	db_clientdesc->DBPacket(HEADER_GD_GUILD_EXP_UPDATE, 0, &guild_exp, sizeof(guild_exp));
	GuildPointChange(GUILD_POINT_EXP, amount, true);

	cit->second.offer_exp += amount;
	cit->second._dummy = 0;

	TPacketGCGuild pack;
	pack.header = HEADER_GC_GUILD;
	pack.subheader = GUILD_SUBHEADER_GC_LIST;
	pack.size = sizeof(pack) + 13;

	for (TGuildMemberOnlineContainer::iterator it = m_memberOnline.begin(); it != m_memberOnline.end(); ++it)
	{
		LPDESC d = (*it)->GetDesc();
		if (d)
		{
			d->BufferedPacket(&pack, sizeof(pack));
			d->Packet(&(cit->second), sizeof(DWORD) * 3 + 1);
		}
	}

	SaveMember(ch->GetPlayerID());

	TPacketGuildChangeMemberData gd_guild;

	gd_guild.guild_id = GetID();
	gd_guild.pid = ch->GetPlayerID();
	gd_guild.offer = cit->second.offer_exp;
	gd_guild.level = ch->GetLevel();
	gd_guild.grade = cit->second.grade;

	db_clientdesc->DBPacket(HEADER_GD_GUILD_CHANGE_MEMBER_DATA, 0, &gd_guild, sizeof(gd_guild));

	quest::PC* pPC = quest::CQuestManager::instance().GetPC(ch->GetPlayerID());
	if (pPC)
	{
		pPC->SetFlag("guild_manage.today_exp", pPC->GetFlag("guild_manage.today_exp") + amount);
		RefreshMyTodayGivenExpData(ch);
	}

	return true;
}

void CGuild::Disband()
{
	sys_log(0, "GUILD: Disband %s:%u", GetName(), GetID());

	for (TGuildMemberOnlineContainer::iterator it = m_memberOnline.begin(); it != m_memberOnline.end(); ++it)
	{
		LPCHARACTER ch = *it;
		ch->SetGuild(NULL);
		SendOnlineRemoveOnePacket(ch->GetPlayerID());
		//ch->SetQuestFlag("guild_manage.new_disband_time", get_global_time()); // @fixme401
		ch->SetIsGuildGuardItemsLoaded(false);
		ch->SetIsGuildDungeonHistoryLoaded(false);
	}

	for (TGuildMemberContainer::iterator it = m_member.begin(); it != m_member.end(); ++it)
	{
		CGuildManager::instance().Unlink(it->first);
	}
}

void CGuild::RequestDisband(DWORD pid)
{
	if (m_data.master_pid != pid)
		return;

	TPacketGuild gd_guild;
	gd_guild.dwGuild = GetID();
	gd_guild.dwInfo = 0;
	db_clientdesc->DBPacket(HEADER_GD_GUILD_DISBAND, 0, &gd_guild, sizeof(TPacketGuild));

	// LAND_CLEAR
	building::CManager::instance().ClearLandByGuildID(GetID());
	// END_LAND_CLEAR
}

void CGuild::AddComment(LPCHARACTER ch, const std::string& str)
{
#ifdef ENABLE_GUILD_COMMENT_ANTIFLOOD
	if (!PulseManager::Instance().IncreaseClock(ch->GetPlayerID(), ePulse::GuildComment, std::chrono::milliseconds(1500))) {
		ch->ChatPacket(CHAT_TYPE_INFO, "[LS;2200;%f]", PULSEMANAGER_CLOCK_TO_SEC2(ch->GetPlayerID(), ePulse::GuildComment));
		return;
	}
#endif

	if (str.length() > GUILD_COMMENT_MAX_LEN)
		return;

	if (str.length() < 2)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "At least 2 characters are needed to write a comment");
		return;
	}

	char text[GUILD_COMMENT_MAX_LEN * 2 + 1];
	DBManager::instance().EscapeString(text, sizeof(text), str.c_str(), str.length());

	auto pid = ch->GetPlayerID();
	DBManager::instance().FuncAfterQuery([this, pid](){ this->RefreshCommentForce(pid); },
			"INSERT INTO guild_comment%s(guild_id, name, notice, content, time) VALUES(%u, '%s', %d, '%s', NOW())",
			get_table_postfix(), m_data.guild_id, ch->GetName(), (str[0] == '!') ? 1 : 0, text);
}

void CGuild::DeleteComment(LPCHARACTER ch, DWORD comment_id)
{
	std::unique_ptr<SQLMsg> pmsg;

	if (GetMember(ch->GetPlayerID())->grade == GUILD_LEADER_GRADE)
		pmsg = DBManager::instance().DirectQuery("DELETE FROM guild_comment%s WHERE id = %u AND guild_id = %u",get_table_postfix(), comment_id, m_data.guild_id);
	else
	{
			char szEscName[CHARACTER_NAME_MAX_LEN * 2 + 1];
			DBManager::instance().EscapeString(szEscName, sizeof(szEscName), ch->GetName(), strlen(ch->GetName()));
			pmsg = DBManager::instance().DirectQuery("DELETE FROM guild_comment%s WHERE id = %u AND guild_id = %u AND name = '%s'",get_table_postfix(), comment_id, m_data.guild_id, szEscName);
		}

	if (pmsg->Get()->uiAffectedRows == 0 || pmsg->Get()->uiAffectedRows == (uint32_t)-1)
		ch->ChatPacket(CHAT_TYPE_INFO, "[LS;930]");
	else
		RefreshCommentForce(ch->GetPlayerID());
}

void CGuild::RefreshComment(LPCHARACTER ch)
{
	RefreshCommentForce(ch->GetPlayerID());
}

void CGuild::RefreshCommentForce(DWORD player_id)
{
	LPCHARACTER ch = CHARACTER_MANAGER::instance().FindByPID(player_id);
	if (ch == NULL) {
		return;
	}

	auto pmsg(DBManager::instance().DirectQuery("SELECT id, name, content FROM guild_comment%s WHERE guild_id = %u ORDER BY notice DESC, id DESC LIMIT %d", get_table_postfix(), m_data.guild_id, GUILD_COMMENT_MAX_COUNT));

	TPacketGCGuild pack;
	pack.header = HEADER_GC_GUILD;
	pack.size = sizeof(pack)+1;
	pack.subheader = GUILD_SUBHEADER_GC_COMMENTS;

	BYTE count = pmsg->Get()->uiNumRows;

	LPDESC d = ch->GetDesc();

	if (!d)
		return;

	pack.size += (sizeof(DWORD)+CHARACTER_NAME_MAX_LEN+1+GUILD_COMMENT_MAX_LEN+1)*(WORD)count;
	d->BufferedPacket(&pack,sizeof(pack));
	d->BufferedPacket(&count, 1);
	char szName[CHARACTER_NAME_MAX_LEN + 1];
	char szContent[GUILD_COMMENT_MAX_LEN + 1];
	memset(szName, 0, sizeof(szName));
	memset(szContent, 0, sizeof(szContent));

	for (uint i = 0; i < pmsg->Get()->uiNumRows; i++)
	{
		MYSQL_ROW row = mysql_fetch_row(pmsg->Get()->pSQLResult);
		DWORD id = strtoul(row[0], NULL, 10);

		strlcpy(szName, row[1], sizeof(szName));
		strlcpy(szContent, row[2], sizeof(szContent));

		d->BufferedPacket(&id, sizeof(id));
		d->BufferedPacket(szName, sizeof(szName));

		if (i == pmsg->Get()->uiNumRows - 1)
			d->Packet(szContent, sizeof(szContent));
		else
			d->BufferedPacket(szContent, sizeof(szContent));
	}
}

void CGuild::SetNote(LPCHARACTER ch, const std::string& str)
{
	if (get_dword_time() < m_dwLastNoteChangeTime + 5000)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "[LS;10124]");
		return;
	}

	m_dwLastNoteChangeTime = get_dword_time();

	if (str.length() > GUILD_NOTE_MAX_LEN)
		return;

	if (str.length() < 4)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "[LS;10125]");
		return;
	}

	char text[GUILD_NOTE_MAX_LEN * 2 + 1];
	DBManager::instance().EscapeString(text, sizeof(text), str.c_str(), str.length());

	std::unique_ptr<SQLMsg> pmsg(DBManager::instance().DirectQuery("UPDATE guild_notes%s SET note='%s' WHERE guild_id=%u", get_table_postfix(), text, m_data.guild_id));

	db_clientdesc->DBPacket(HEADER_GD_GUILD_REFRESH_NOTE, 0, &m_data.guild_id, sizeof(DWORD));

	RefreshNote(NULL, true);

	GuildLog(25, ch->GetName(), ch->GetPlayerID());
}

void CGuild::RefreshNote(LPCHARACTER ch, bool force)
{
	if (guildNote.empty() || force)
	{
		std::unique_ptr<SQLMsg> pmsg(DBManager::instance().DirectQuery("SELECT note FROM guild_notes%s WHERE guild_id=%u", get_table_postfix(), m_data.guild_id));
		SQLResult* pRes = pmsg->Get();
		if (pRes && pRes->uiNumRows != 0 && pRes->pSQLResult)
		{
			char szNote[GUILD_NOTE_MAX_LEN + 1];
			memset(szNote, 0, sizeof(szNote));

			MYSQL_ROW row = mysql_fetch_row(pRes->pSQLResult);
			strlcpy(szNote, row[0], sizeof(szNote));

			guildNote = szNote;
		}
		else
			guildNote = "Notice";
	}

	TPacketGCGuild pack;
	pack.header = HEADER_GC_GUILD;
	pack.size = sizeof(pack) + guildNote.size();
	pack.subheader = GUILD_SUBHEADER_GC_NOTE;

	if (ch)
	{
		LPDESC d = ch->GetDesc();
		if (d)
		{
			d->BufferedPacket(&pack, sizeof(pack));
			d->Packet(guildNote.c_str(), guildNote.size());
		}
	}
	else
	{
		for (const auto& member : m_memberOnline)
		{
			LPDESC d = member->GetDesc();
			if (d)
			{
				d->BufferedPacket(&pack, sizeof(pack));
				d->Packet(guildNote.c_str(), guildNote.size());
			}
		}
	}
}

bool CGuild::ChangeMemberGeneral(DWORD pid, BYTE is_general)
{
	if (is_general && GetGeneralCount() >= GetMaxGeneralCount())
		return false;

	TGuildMemberContainer::iterator it = m_member.find(pid);
	if (it == m_member.end())
	{
		return true;
	}

	is_general = is_general?1:0;

	if (it->second.is_general == is_general)
		return true;

	if (is_general)
		++m_general_count;
	else
		--m_general_count;

	it->second.is_general = is_general;

	TGuildMemberOnlineContainer::iterator itOnline = m_memberOnline.begin();

	TPacketGCGuild pack;
	pack.header = HEADER_GC_GUILD;
	pack.size = sizeof(pack)+5;
	pack.subheader = GUILD_SUBHEADER_GC_CHANGE_MEMBER_GENERAL;

	while (itOnline != m_memberOnline.end())
	{
		LPDESC d = (*(itOnline++))->GetDesc();

		if (!d)
			continue;

		d->BufferedPacket(&pack, sizeof(pack));
		d->BufferedPacket(&pid, sizeof(pid));
		d->Packet(&is_general, sizeof(is_general));
	}

	SaveMember(pid);
	return true;
}

void CGuild::ChangeMemberGrade(DWORD pid, BYTE grade)
{
	if (grade == 1)
		return;

	TGuildMemberContainer::iterator it = m_member.find(pid);

	if (it == m_member.end())
		return;

	it->second.grade = grade;

	TGuildMemberOnlineContainer::iterator itOnline = m_memberOnline.begin();

	TPacketGCGuild pack;
	pack.header = HEADER_GC_GUILD;
	pack.size = sizeof(pack)+5;
	pack.subheader = GUILD_SUBHEADER_GC_CHANGE_MEMBER_GRADE;

	while (itOnline != m_memberOnline.end())
	{
		LPDESC d = (*(itOnline++))->GetDesc();

		if (!d)
			continue;

		d->BufferedPacket(&pack, sizeof(pack));
		d->BufferedPacket(&pid, sizeof(pid));
		d->Packet(&grade, sizeof(grade));
	}

	SaveMember(pid);

	TPacketGuildChangeMemberData gd_guild;

	gd_guild.guild_id = GetID();
	gd_guild.pid = pid;
	gd_guild.offer = it->second.offer_exp;
	gd_guild.level = it->second.level;
	gd_guild.grade = grade;

	db_clientdesc->DBPacket(HEADER_GD_GUILD_CHANGE_MEMBER_DATA, 0, &gd_guild, sizeof(gd_guild));
}

void CGuild::SkillLevelUp(uint8_t skillIdx)
{
	if (skillIdx >= GUILD_SKILL_COUNT)
		return;

	const uint8_t skillMaxLevel = CGuildManager::instance().GetGuildSkillMaxLevel(skillIdx);
	if (skillMaxLevel == 0)
		return;

	if (m_data.abySkill[skillIdx] >= skillMaxLevel)
		return;

	if (m_data.skill_point <= 0)
		return;
	m_data.skill_point--;

	m_data.abySkill[skillIdx]++;
	
	SaveSkill();
	SendDBSkillUpdate();

	std::for_each(m_memberOnline.begin(), m_memberOnline.end(), msl::bind1st(std::mem_fn(&CGuild::SendSkillInfoPacket), this));
	
	if (skillIdx == GUILD_SKILL_ATTBONUS_BOSS || skillIdx == GUILD_SKILL_ATTBONUS_STONE || skillIdx == GUILD_SKILL_DOUBLE_DROP_ITEMS)
	{
		for (const auto& member : m_memberOnline)
			member->ComputePoints();
	}

	sys_log(0, "Guild SkillUp: %s %d level %d", GetName(), skillIdx, m_data.abySkill[skillIdx]);
}

void CGuild::UpgradeBonusSkill(uint8_t skillIdx)
{
	if (skillIdx >= 5)
		return;

	if (m_data.abySkill[skillIdx] >= GUILD_BONUS_MAX_LEVEL)
		return;

	m_data.abySkill[skillIdx]++;

	SaveSkill();
	SendDBSkillUpdate();

	std::for_each(m_memberOnline.begin(), m_memberOnline.end(), msl::bind1st(std::mem_fn(&CGuild::SendSkillInfoPacket), this));

	if (skillIdx == GUILD_SKILL_ATTBONUS_BOSS || skillIdx == GUILD_SKILL_ATTBONUS_STONE || skillIdx == GUILD_SKILL_DOUBLE_DROP_ITEMS)
	{
		for (const auto& member : m_memberOnline)
			member->ComputePoints();
	}

	sys_log(0, "Guild BonusSkillUpgrade: %s idx=%u level=%u", GetName(), skillIdx, m_data.abySkill[skillIdx]);
}

void CGuild::UseSkill(DWORD dwVnum, LPCHARACTER ch, DWORD pid)
{
	return;
	//LPCHARACTER victim = NULL;
	//
	//if (!GetMember(ch->GetPlayerID()) || !HasGradeAuth(GetMember(ch->GetPlayerID())->grade, GUILD_AUTH_USE_SKILL))
	//	return;
	//
	//sys_log(0,"GUILD_USE_SKILL : cname(%s), skill(%d)", ch ? ch->GetName() : "", dwVnum);
	//
	//DWORD dwRealVnum = dwVnum - GUILD_SKILL_START;
	//
	//if (!ch->CanMove())
	//	return;
	//
	//if (dwRealVnum >= GUILD_SKILL_COUNT)
	//	return;
	//
	//CSkillProto* pkSk = CSkillManager::instance().Get(dwVnum);
	//
	//if (!pkSk)
	//{
	//	sys_err("There is no such guild skill by number %u", dwVnum);
	//	return;
	//}
	//
	//if (m_data.abySkill[dwRealVnum] == 0)
	//	return;
	//
	//if ((pkSk->dwFlag & SKILL_FLAG_SELFONLY))
	//{
	//	if (ch->FindAffect(pkSk->dwVnum))
	//		return;
	//
	//	victim = ch;
	//}
	//
	//if (ch->IsAffectFlag(AFF_REVIVE_INVISIBLE))
	//	ch->RemoveAffect(AFFECT_REVIVE_INVISIBLE);
	//
	//if (ch->IsAffectFlag(AFF_EUNHYUNG))
	//	ch->RemoveAffect(SKILL_EUNHYUNG);
	//
	//double k =1.0*m_data.abySkill[dwRealVnum]/pkSk->bMaxLevel;
	//pkSk->kSPCostPoly.SetVar("k", k);
	//int iNeededSP = (int) pkSk->kSPCostPoly.Eval();
	//
	//if (GetSP() < iNeededSP)
	//{
	//	ch->ChatPacket(CHAT_TYPE_INFO, "[LS;931;%d;%d]", GetSP(), iNeededSP);
	//	return;
	//}
	//
	//pkSk->kCooldownPoly.SetVar("k", k);
	//int iCooltime = (int) pkSk->kCooldownPoly.Eval();
	//
	//if (!abSkillUsable[dwRealVnum])
	//{
	//	ch->ChatPacket(CHAT_TYPE_INFO, "[LS;932]");
	//	return;
	//}
	//
	//{
	//	TPacketGuildUseSkill p;
	//	p.dwGuild = GetID();
	//	p.dwSkillVnum = pkSk->dwVnum;
	//	p.dwCooltime = iCooltime;
	//	db_clientdesc->DBPacket(HEADER_GD_GUILD_USE_SKILL, 0, &p, sizeof(p));
	//}
	//abSkillUsable[dwRealVnum] = false;
	//
	//switch (dwVnum)
	//{
	//	case GUILD_SKILL_TELEPORT:
	//
	//		SendDBSkillUpdate(-iNeededSP);
	//		if ((victim = (CHARACTER_MANAGER::instance().FindByPID(pid))))
	//			ch->WarpSet(victim->GetX(), victim->GetY());
	//		else
	//		{
	//			if (m_memberP2POnline.find(pid) != m_memberP2POnline.end())
	//			{
	//				CCI * pcci = P2P_MANAGER::instance().FindByPID(pid);
	//
	//				if (pcci->bChannel != g_bChannel)
	//				{
	//					ch->ChatPacket(CHAT_TYPE_INFO, "[LS;934;%d;%d]", pcci->bChannel, g_bChannel);
	//				}
	//				else
	//				{
	//					TPacketGGFindPosition p;
	//					p.header = HEADER_GG_FIND_POSITION;
	//					p.dwFromPID = ch->GetPlayerID();
	//					p.dwTargetPID = pid;
	//					pcci->pkDesc->Packet(&p, sizeof(TPacketGGFindPosition));
	//					if (test_server) ch->ChatPacket(CHAT_TYPE_PARTY, "sent find position packet for guild teleport");
	//				}
	//			}
	//			else
	//				ch->ChatPacket(CHAT_TYPE_INFO, "[LS;935]");
	//		}
	//		break;
	//
	//	default:
	//		{
	//			if (!UnderAnyWar())
	//			{
	//				ch->ChatPacket(CHAT_TYPE_INFO, "[LS;937]");
	//				return;
	//			}
	//
	//			SendDBSkillUpdate(-iNeededSP);
	//
	//			for (itertype(m_memberOnline) it = m_memberOnline.begin(); it != m_memberOnline.end(); ++it)
	//			{
	//				LPCHARACTER victim = *it;
	//				victim->RemoveAffect(dwVnum);
	//				ch->ComputeSkill(dwVnum, victim, m_data.abySkill[dwRealVnum]);
	//			}
	//		}
	//		break;
	//}
}

void CGuild::SendSkillInfoPacket(LPCHARACTER ch) const
{
	LPDESC d = ch->GetDesc();

	if (!d)
		return;

	TPacketGCGuild pack;

	pack.header		= HEADER_GC_GUILD;
	pack.size		= sizeof(pack) + 6 + GUILD_SKILL_COUNT;
	pack.subheader	= GUILD_SUBHEADER_GC_SKILL_INFO;

	d->BufferedPacket(&pack, sizeof(pack));
	d->BufferedPacket(&m_data.skill_point,	1);
	d->BufferedPacket(&m_data.abySkill,		GUILD_SKILL_COUNT);
	d->BufferedPacket(&m_data.power,		2);
	d->Packet(&m_data.max_power,	2);
}

void CGuild::ComputeGuildPoints()
{
	m_data.max_power = GUILD_BASE_POWER + (m_data.level-1) * GUILD_POWER_PER_LEVEL;

	m_data.power = MINMAX(0, m_data.power, m_data.max_power);
}

int CGuild::GetSkillLevel(DWORD vnum)
{
	DWORD dwRealVnum = vnum - GUILD_SKILL_START;

	if (dwRealVnum >= GUILD_SKILL_COUNT)
		return 0;

	return m_data.abySkill[dwRealVnum];
}

void CGuild::UpdateSkill(BYTE skill_point, BYTE* skill_levels)
{
	m_data.skill_point = skill_point;
	
	const uint8_t oldBossAttSkillPoint  = m_data.abySkill[GUILD_SKILL_ATTBONUS_BOSS];
	const uint8_t oldStoneAttSkillPoint = m_data.abySkill[GUILD_SKILL_ATTBONUS_STONE];
	const uint8_t oldDoubleDropPoint    = m_data.abySkill[GUILD_SKILL_DOUBLE_DROP_ITEMS];

	thecore_memcpy(m_data.abySkill, skill_levels, sizeof(BYTE) * GUILD_SKILL_COUNT);

	for_each(m_memberOnline.begin(), m_memberOnline.end(), msl::bind1st(std::mem_fn(&CGuild::SendSkillInfoPacket), *this));

	if (oldBossAttSkillPoint != m_data.abySkill[GUILD_SKILL_ATTBONUS_BOSS] || oldStoneAttSkillPoint != m_data.abySkill[GUILD_SKILL_ATTBONUS_STONE] || oldDoubleDropPoint != m_data.abySkill[GUILD_SKILL_DOUBLE_DROP_ITEMS])
	{
		for (const auto& member : m_memberOnline)
			member->ComputePoints();
	}
	
	ComputeGuildPoints();
}

static DWORD __guild_levelup_exp(int level)
{
	return guild_exp_table2[level];
}

void CGuild::GuildPointChange(BYTE type, int amount, bool save)
{
	switch (type)
	{
		case POINT_SP:
			m_data.power += amount;

			m_data.power = MINMAX(0, m_data.power, m_data.max_power);

			if (save)
			{
				SaveSkill();
			}

			std::for_each(m_memberOnline.begin(), m_memberOnline.end(), msl::bind1st(std::mem_fn(&CGuild::SendSkillInfoPacket), this));
			break;

		case GUILD_POINT_EXP:
			if (amount < 0 && m_data.exp < (DWORD) - amount)
			{
				m_data.exp = 0;
			}
			else
			{
				m_data.exp += amount;

				while (m_data.exp >= __guild_levelup_exp(m_data.level))
				{
					if (m_data.level < GUILD_MAX_LEVEL)
					{
						m_data.exp -= __guild_levelup_exp(m_data.level);
						++m_data.level;
						++m_data.skill_point;

						if (m_data.level > GUILD_MAX_LEVEL)
							m_data.level = GUILD_MAX_LEVEL;

						ComputeGuildPoints();
						GuildPointChange(POINT_SP, m_data.max_power-m_data.power);

						if (save)
							ChangeLadderPoint(GUILD_LADDER_POINT_PER_LEVEL);

						// NOTIFY_GUILD_EXP_CHANGE
						std::for_each(m_memberOnline.begin(), m_memberOnline.end(), msl::bind1st(std::mem_fn(&CGuild::SendGuildInfoPacket), this));
						// END_OF_NOTIFY_GUILD_EXP_CHANGE
					}

					if (m_data.level == GUILD_MAX_LEVEL)
					{
						m_data.exp = 0;
					}
				}
			}

			TPacketGCGuild pack;
			pack.header = HEADER_GC_GUILD;
			pack.size = sizeof(pack)+5;
			pack.subheader = GUILD_SUBHEADER_GC_CHANGE_EXP;

			TEMP_BUFFER buf;
			buf.write(&pack,sizeof(pack));
			buf.write(&m_data.level,1);
			buf.write(&m_data.exp,4);

			for (TGuildMemberOnlineContainer::iterator it = m_memberOnline.begin(); it != m_memberOnline.end(); ++it)
			{
				LPDESC d = (*it)->GetDesc();

				if (d)
					d->Packet(buf.read_peek(), buf.size());
			}

			if (save)
				SaveLevel();

			{
				TPacketGGGuild gg;
				TPacketGGGuildChangeExp gg2;
				gg.bHeader    = HEADER_GG_GUILD;
				gg.bSubHeader = GUILD_SUBHEADER_GG_CHANGE_EXP;
				gg.dwGuild    = GetID();
				gg2.byLevel   = m_data.level;
				gg2.dwExp     = m_data.exp;
				P2P_MANAGER::instance().Send(&gg,  sizeof(TPacketGGGuild));
				P2P_MANAGER::instance().Send(&gg2, sizeof(TPacketGGGuildChangeExp));
			}

			break;
	}
}

void CGuild::SkillRecharge()
{
}

void CGuild::SaveMember(DWORD pid)
{
	TGuildMemberContainer::iterator it = m_member.find(pid);

	if (it == m_member.end())
		return;

	DBManager::instance().Query(
			"UPDATE guild_member%s SET grade = %d, offer = %u, is_general = %d WHERE pid = %u and guild_id = %u",
			get_table_postfix(), it->second.grade, it->second.offer_exp, it->second.is_general, pid, m_data.guild_id);
}

void CGuild::LevelChange(DWORD pid, BYTE level)
{
	TGuildMemberContainer::iterator cit = m_member.find(pid);

	if (cit == m_member.end())
		return;

	cit->second.level = level;

	TPacketGuildChangeMemberData gd_guild;

	gd_guild.guild_id = GetID();
	gd_guild.pid = pid;
	gd_guild.offer = cit->second.offer_exp;
	gd_guild.grade = cit->second.grade;
	gd_guild.level = level;

	db_clientdesc->DBPacket(HEADER_GD_GUILD_CHANGE_MEMBER_DATA, 0, &gd_guild, sizeof(gd_guild));

	TPacketGCGuild pack;
	pack.header = HEADER_GC_GUILD;
	cit->second._dummy = 0;

	for (TGuildMemberOnlineContainer::iterator it = m_memberOnline.begin(); it != m_memberOnline.end(); ++it)
	{
		LPDESC d = (*it)->GetDesc();

		if (d)
		{
			pack.subheader = GUILD_SUBHEADER_GC_LIST;
			pack.size = sizeof(pack) + 13;
			d->BufferedPacket(&pack, sizeof(pack));
			d->Packet(&(cit->second), sizeof(DWORD) * 3 + 1);
		}
	}
}

void CGuild::ChangeMemberData(DWORD pid, DWORD offer, BYTE level, BYTE grade)
{
	TGuildMemberContainer::iterator cit = m_member.find(pid);

	if (cit == m_member.end())
		return;

	cit->second.offer_exp = offer;
	cit->second.level = level;
	cit->second.grade = grade;
	cit->second._dummy = 0;

	TPacketGCGuild pack;
	memset(&pack, 0, sizeof(pack));
	pack.header = HEADER_GC_GUILD;

	for (TGuildMemberOnlineContainer::iterator it = m_memberOnline.begin(); it != m_memberOnline.end(); ++it)
	{
		LPDESC d = (*it)->GetDesc();
		if (d)
		{
			pack.subheader = GUILD_SUBHEADER_GC_LIST;
			pack.size = sizeof(pack) + 13;
			d->BufferedPacket(&pack, sizeof(pack));
			d->Packet(&(cit->second), sizeof(DWORD) * 3 + 1);
		}
	}
}

namespace
{
	struct FGuildChat
	{
		const char* c_pszText;

		FGuildChat(const char* c_pszText)
			: c_pszText(c_pszText)
			{}

		void operator()(LPCHARACTER ch)
		{
			ch->ChatPacket(CHAT_TYPE_GUILD, "%s", c_pszText);
		}
	};
}

void CGuild::P2PChat(const char* c_pszText)
{
	std::for_each(m_memberOnline.begin(), m_memberOnline.end(), FGuildChat(c_pszText));
}

void CGuild::Chat(const char* c_pszText)
{
	std::for_each(m_memberOnline.begin(), m_memberOnline.end(), FGuildChat(c_pszText));

	TPacketGGGuild p1;
	TPacketGGGuildChat p2;

	p1.bHeader = HEADER_GG_GUILD;
	p1.bSubHeader = GUILD_SUBHEADER_GG_CHAT;
	p1.dwGuild = GetID();
	strlcpy(p2.szText, c_pszText, sizeof(p2.szText));

	P2P_MANAGER::instance().Send(&p1, sizeof(TPacketGGGuild));
	P2P_MANAGER::instance().Send(&p2, sizeof(TPacketGGGuildChat));
}

LPCHARACTER CGuild::GetMasterCharacter()
{
	return CHARACTER_MANAGER::instance().FindByPID(GetMasterPID());
}

void CGuild::Packet(const void* buf, int size)
{
	for (itertype(m_memberOnline) it = m_memberOnline.begin(); it!=m_memberOnline.end();++it)
	{
		LPDESC d = (*it)->GetDesc();

		if (d)
			d->Packet(buf, size);
	}
}

int CGuild::GetTotalLevel() const
{
	int total = 0;

	for (itertype(m_member) it = m_member.begin(); it != m_member.end(); ++it)
	{
		total += it->second.level;
	}

	return total;
}

bool CGuild::ChargeSP(LPCHARACTER ch, int iSP)
{
	int gold = iSP * 100;

	if (gold < iSP || ch->GetGold() < gold)
		return false;

	int iRemainSP = m_data.max_power - m_data.power;

	if (iSP > iRemainSP)
	{
		iSP = iRemainSP;
		gold = iSP * 100;
	}

	ch->PointChange(POINT_GOLD, -gold);
	DBManager::instance().SendMoneyLog(MONEY_LOG_GUILD, 1, -gold);

	SendDBSkillUpdate(iSP);

	return true;
}

void CGuild::SkillUsableChange(DWORD dwSkillVnum, bool bUsable)
{
	DWORD dwRealVnum = dwSkillVnum - GUILD_SKILL_START;

	if (dwRealVnum >= GUILD_SKILL_COUNT)
		return;

	abSkillUsable[dwRealVnum] = bUsable;

	// GUILD_SKILL_COOLTIME_BUG_FIX
	sys_log(0, "CGuild::SkillUsableChange(guild=%s, skill=%d, usable=%d)", GetName(), dwSkillVnum, bUsable);
	// END_OF_GUILD_SKILL_COOLTIME_BUG_FIX
}

// GUILD_MEMBER_COUNT_BONUS
void CGuild::SetMemberCountBonus(int iBonus)
{
	m_iMemberCountBonus = iBonus;
	sys_log(0, "GUILD_IS_FULL_BUG : Bonus set to %d(val:%d)", iBonus, m_iMemberCountBonus);
}

void CGuild::BroadcastMemberCountBonus()
{
	TPacketGGGuild p1;

	p1.bHeader = HEADER_GG_GUILD;
	p1.bSubHeader = GUILD_SUBHEADER_GG_SET_MEMBER_COUNT_BONUS;
	p1.dwGuild = GetID();

	P2P_MANAGER::instance().Send(&p1, sizeof(TPacketGGGuild));
	P2P_MANAGER::instance().Send(&m_iMemberCountBonus, sizeof(int));
}

void CGuild::BroadcastDonateMaterials()
{
	TPacketGGGuild p1;
	TPacketGGGuildDonateMaterials p2;

	p1.bHeader    = HEADER_GG_GUILD;
	p1.bSubHeader = GUILD_SUBHEADER_GG_DONATE_MATERIALS;
	p1.dwGuild    = GetID();

	p2.donate_stone   = m_data.donate_stone;
	p2.donate_log     = m_data.donate_log;
	p2.donate_plywood = m_data.donate_plywood;

	P2P_MANAGER::instance().Send(&p1, sizeof(TPacketGGGuild));
	P2P_MANAGER::instance().Send(&p2, sizeof(TPacketGGGuildDonateMaterials));
}

int CGuild::GetMaxMemberCount()
{
	// GUILD_IS_FULL_BUG_FIX
	if ( m_iMemberCountBonus < 0 || m_iMemberCountBonus > 18 )
		m_iMemberCountBonus = 0;
	// END_GUILD_IS_FULL_BUG_FIX

	if (g_bGuildInfiniteMembers)
		return INT_MAX;

	return GUILD_BASE_MAX_MEMBER_COUNT + 2 * (m_data.level-1) + m_iMemberCountBonus
		+ GetSkillValue(GUILD_SKILL_MAX_MEMBER_COUNT);
}
// END_OF_GUILD_MEMBER_COUNT_BONUS

void CGuild::AdvanceLevel(int iLevel)
{
	if (m_data.level == iLevel)
		return;

	m_data.level = MIN(GUILD_MAX_LEVEL, iLevel);
}

void CGuild::RequestDepositMoney(LPCHARACTER ch, int iGold)
{
	if (false==ch->CanDeposit())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "[LS;994]");
		return;
	}

	if (ch->GetGold() < iGold)
		return;

	ch->PointChange(POINT_GOLD, -iGold);

	TPacketGDGuildMoney p;
	p.dwGuild = GetID();
	p.iGold = iGold;
	db_clientdesc->DBPacket(HEADER_GD_GUILD_DEPOSIT_MONEY, 0, &p, sizeof(p));

	char buf[64+1];
	snprintf(buf, sizeof(buf), "%u %s", GetID(), GetName());
	LogManager::instance().CharLog(ch, iGold, "GUILD_DEPOSIT", buf);

	ch->UpdateDepositPulse();
	sys_log(0, "GUILD: DEPOSIT %s:%u player %s[%u] gold %d", GetName(), GetID(), ch->GetName(), ch->GetPlayerID(), iGold);
}

void CGuild::RequestWithdrawMoney(LPCHARACTER ch, int iGold)
{
	if (false==ch->CanDeposit())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "[LS;994]");
		return;
	}

	if (ch->GetPlayerID() != GetMasterPID())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "[LS;939]");
		return;
	}

	if (m_data.gold < iGold)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "[LS;687]");
		return;
	}

	TPacketGDGuildMoney p;
	p.dwGuild = GetID();
	p.iGold = iGold;
	db_clientdesc->DBPacket(HEADER_GD_GUILD_WITHDRAW_MONEY, 0, &p, sizeof(p));

	ch->UpdateDepositPulse();
}

void CGuild::ChangeMoney(int addMoney)
{
	if (static_cast<int64_t>(m_data.gold)+addMoney >= 2000000000)
	{
		m_data.gold = 2000000000;
	}
	else
	{
		m_data.gold += addMoney;
	}

	std::unique_ptr<SQLMsg> pmsg(DBManager::instance().DirectQuery("UPDATE guild%s SET gold=%d WHERE id=%u", get_table_postfix(), m_data.gold, m_data.guild_id));

	TPacketGDGuildMoney p;
	p.dwGuild = GetID();
	p.iGold = m_data.gold;
	db_clientdesc->DBPacket(HEADER_GD_GUILD_CHANGE_MONEY, 0, &p, sizeof(p));
}

void CGuild::RecvMoneyChange(int iGold)
{
	m_data.gold = iGold;

	TPacketGCGuild p;
	p.header = HEADER_GC_GUILD;
	p.size = sizeof(p) + sizeof(int);
	p.subheader = GUILD_SUBHEADER_GC_MONEY_CHANGE;

	for (itertype(m_memberOnline) it = m_memberOnline.begin(); it != m_memberOnline.end(); ++it)
	{
		LPCHARACTER ch = *it;
		LPDESC d = ch->GetDesc();
		d->BufferedPacket(&p, sizeof(p));
		d->Packet(&iGold, sizeof(int));
	}
}

void CGuild::RecvWithdrawMoneyGive(int iChangeGold)
{
	LPCHARACTER ch = GetMasterCharacter();

	if (ch)
	{
		ch->PointChange(POINT_GOLD, iChangeGold);
		sys_log(0, "GUILD: WITHDRAW %s:%u player %s[%u] gold %d", GetName(), GetID(), ch->GetName(), ch->GetPlayerID(), iChangeGold);
	}

	TPacketGDGuildMoneyWithdrawGiveReply p;
	p.dwGuild = GetID();
	p.iChangeGold = iChangeGold;
	p.bGiveSuccess = ch ? 1 : 0;
	db_clientdesc->DBPacket(HEADER_GD_GUILD_WITHDRAW_MONEY_GIVE_REPLY, 0, &p, sizeof(p));
}

bool CGuild::HasLand()
{
	return building::CManager::instance().FindLandByGuild(GetID()) != NULL;
}

// GUILD_JOIN_BUG_FIX
EVENTINFO(TInviteGuildEventInfo)
{
	DWORD	dwInviteePID;
	DWORD	dwGuildID;

	TInviteGuildEventInfo()
	: dwInviteePID( 0 )
	, dwGuildID( 0 )
	{
	}
};

EVENTFUNC( GuildInviteEvent )
{
	TInviteGuildEventInfo *pInfo = dynamic_cast<TInviteGuildEventInfo*>( event->info );

	if ( pInfo == NULL )
	{
		sys_err( "GuildInviteEvent> <Factor> Null pointer" );
		return 0;
	}

	CGuild* pGuild = CGuildManager::instance().FindGuild( pInfo->dwGuildID );

	if ( pGuild )
	{
		sys_log( 0, "GuildInviteEvent %s", pGuild->GetName() );
		pGuild->InviteDeny( pInfo->dwInviteePID );
	}

	return 0;
}

void CGuild::Invite( LPCHARACTER pchInviter, LPCHARACTER pchInvitee )
{
	if (quest::CQuestManager::instance().GetPCForce(pchInviter->GetPlayerID())->IsRunning() == true)
	{
	    return;
	}

	if (quest::CQuestManager::instance().GetPCForce(pchInvitee->GetPlayerID())->IsRunning() == true)
		return;

	if ( pchInvitee->IsBlockMode( BLOCK_GUILD_INVITE ) )
	{
		pchInviter->ChatPacket( CHAT_TYPE_INFO, "[LS;940]" );
		return;
	}
	else if ( !HasGradeAuth( GetMember( pchInviter->GetPlayerID() )->grade, GUILD_AUTH_ADD_MEMBER ) )
	{
		pchInviter->ChatPacket( CHAT_TYPE_INFO, "[LS;941]" );
		return;
	}
	else if ( pchInvitee->GetEmpire() != pchInviter->GetEmpire() )
	{
		pchInviter->ChatPacket( CHAT_TYPE_INFO, "[LS;942]" );
		return;
	}

	GuildJoinErrCode errcode = VerifyGuildJoinableCondition( pchInvitee );
	switch ( errcode )
	{
		case GERR_NONE: break;
		case GERR_WITHDRAWPENALTY:
						pchInviter->ChatPacket( CHAT_TYPE_INFO,
								"[LS;943;%d]",
								quest::CQuestManager::instance().GetEventFlag( "guild_withdraw_delay" ) );
						return;
		case GERR_COMMISSIONPENALTY:
						pchInviter->ChatPacket( CHAT_TYPE_INFO,
								"[LS;944;%d]",
								quest::CQuestManager::instance().GetEventFlag( "guild_disband_delay") );
						return;
		case GERR_ALREADYJOIN:	pchInviter->ChatPacket(CHAT_TYPE_INFO, "[LS;945]"); return;
		case GERR_GUILDISFULL:	pchInviter->ChatPacket(CHAT_TYPE_INFO, "[LS;946]"); return;
		case GERR_GUILD_IS_IN_WAR : pchInviter->ChatPacket( CHAT_TYPE_INFO, "<���> ���� ��尡 ���� �� �Դϴ�." ); return;
		case GERR_INVITE_LIMIT : pchInviter->ChatPacket( CHAT_TYPE_INFO, "<���> ���� �ű� ���� ���� ���� �Դϴ�." ); return;

		default: sys_err( "ignore guild join error(%d)", errcode ); return;
	}

	if ( m_GuildInviteEventMap.end() != m_GuildInviteEventMap.find( pchInvitee->GetPlayerID() ) )
		return;

	TInviteGuildEventInfo* pInfo = AllocEventInfo<TInviteGuildEventInfo>();
	pInfo->dwInviteePID = pchInvitee->GetPlayerID();
	pInfo->dwGuildID = GetID();

	m_GuildInviteEventMap.emplace(pchInvitee->GetPlayerID(), event_create(GuildInviteEvent, pInfo, PASSES_PER_SEC(10)));

	DWORD gid = GetID();

	TPacketGCGuild p;
	p.header	= HEADER_GC_GUILD;
	p.size	= sizeof(p) + sizeof(DWORD) + GUILD_NAME_MAX_LEN + 1;
	p.subheader	= GUILD_SUBHEADER_GC_GUILD_INVITE;

	TEMP_BUFFER buf;
	buf.write( &p, sizeof(p) );
	buf.write( &gid, sizeof(DWORD) );
	buf.write( GetName(), GUILD_NAME_MAX_LEN + 1 );

	pchInvitee->GetDesc()->Packet( buf.read_peek(), buf.size() );
}

void CGuild::InviteAccept( LPCHARACTER pchInvitee )
{
	EventMap::iterator itFind = m_GuildInviteEventMap.find( pchInvitee->GetPlayerID() );
	if ( itFind == m_GuildInviteEventMap.end() )
	{
		sys_log( 0, "GuildInviteAccept from not invited character(invite guild: %s, invitee: %s)", GetName(), pchInvitee->GetName() );
		return;
	}

	event_cancel( &itFind->second );
	m_GuildInviteEventMap.erase( itFind );

	GuildJoinErrCode errcode = VerifyGuildJoinableCondition( pchInvitee );
	switch ( errcode )
	{
		case GERR_NONE: break;
		case GERR_WITHDRAWPENALTY:
						pchInvitee->ChatPacket( CHAT_TYPE_INFO,
								"[LS;943;%d]",
								quest::CQuestManager::instance().GetEventFlag( "guild_withdraw_delay" ) );
						return;
		case GERR_COMMISSIONPENALTY:
						pchInvitee->ChatPacket( CHAT_TYPE_INFO,
								"[LS;944;%d]",
								quest::CQuestManager::instance().GetEventFlag( "guild_disband_delay") );
						return;
		case GERR_ALREADYJOIN:	pchInvitee->ChatPacket(CHAT_TYPE_INFO, "[LS;945]"); return;
		case GERR_GUILDISFULL:	pchInvitee->ChatPacket(CHAT_TYPE_INFO, "[LS;946]"); return;
		case GERR_GUILD_IS_IN_WAR : pchInvitee->ChatPacket( CHAT_TYPE_INFO, "<���> ���� ��尡 ���� �� �Դϴ�." ); return;
		case GERR_INVITE_LIMIT : pchInvitee->ChatPacket( CHAT_TYPE_INFO, "<���> ���� �ű� ���� ���� ���� �Դϴ�." ); return;

		default: sys_err( "ignore guild join error(%d)", errcode ); return;
	}

	RequestAddMember( pchInvitee, 15 );
}

void CGuild::InviteDeny( DWORD dwPID )
{
	EventMap::iterator itFind = m_GuildInviteEventMap.find( dwPID );
	if ( itFind == m_GuildInviteEventMap.end() )
	{
		sys_log( 0, "GuildInviteDeny from not invited character(invite guild: %s, invitee PID: %d)", GetName(), dwPID );
		return;
	}

	event_cancel( &itFind->second );
	m_GuildInviteEventMap.erase( itFind );
}

CGuild::GuildJoinErrCode CGuild::VerifyGuildJoinableCondition( const LPCHARACTER pchInvitee )
{
	if ( get_global_time() - pchInvitee->GetQuestFlag( "guild_manage.new_withdraw_time" )
			< CGuildManager::instance().GetWithdrawDelay() )
		return GERR_WITHDRAWPENALTY;
	else if ( get_global_time() - pchInvitee->GetQuestFlag( "guild_manage.new_disband_time" )
			< CGuildManager::instance().GetDisbandDelay() )
		return GERR_COMMISSIONPENALTY;
	else if ( pchInvitee->GetGuild() )
		return GERR_ALREADYJOIN;
	else if ( GetMemberCount() >= GetMaxMemberCount() )
	{
		sys_log(1, "GuildName = %s, GetMemberCount() = %d, GetMaxMemberCount() = %d (32 + MAX(level(%d)-10, 0) * 2 + bonus(%d)",
				GetName(), GetMemberCount(), GetMaxMemberCount(), m_data.level, m_iMemberCountBonus);
		return GERR_GUILDISFULL;
	}
	else if ( UnderAnyWar() != 0 )
	{
		return GERR_GUILD_IS_IN_WAR;
	}
	else if (g_bGuildInviteLimit)
	{
		auto pMsg( DBManager::instance().DirectQuery("SELECT value FROM guild_invite_limit WHERE id=%d", GetID()) );
		if ( pMsg->Get()->uiNumRows > 0 )
		{
			MYSQL_ROW row = mysql_fetch_row(pMsg->Get()->pSQLResult);
			time_t limit_time=0;
			str_to_number( limit_time, row[0] );

			if ( test_server == true )
			{
				limit_time += quest::CQuestManager::instance().GetEventFlag("guild_invite_limit") * 60;
			}
			else
			{
				limit_time += quest::CQuestManager::instance().GetEventFlag("guild_invite_limit") * 24 * 60 * 60;
			}

			if ( get_global_time() < limit_time ) return GERR_INVITE_LIMIT;
		}
	}

	return GERR_NONE;
}
// END_OF_GUILD_JOIN_BUG_FIX

bool CGuild::ChangeMasterTo(DWORD dwPID)
{
	TGuildMember* member = GetMember(dwPID);
	if (!member)
		return false;

	TPacketChangeGuildMaster p;
	p.dwGuildID = GetID();
	p.idFrom = GetMasterPID();
	p.idTo = dwPID;

	db_clientdesc->DBPacket(HEADER_GD_REQ_CHANGE_GUILD_MASTER, 0, &p, sizeof(p));

	GuildLog(24, member->name.c_str(), dwPID);

	return true;
}

void CGuild::SendGuildDataUpdateToAllMember(SQLMsg* pmsg)
{
	TGuildMemberOnlineContainer::iterator iter = m_memberOnline.begin();

	for (; iter != m_memberOnline.end(); iter++ )
	{
		SendGuildInfoPacket(*iter);
		SendAllGradePacket(*iter);
	}
}

#ifdef ENABLE_D_NJGUILD
void CGuild::SetDungeon_for_Only_guild(LPDUNGEON pDungeon)
{
	m_pkDungeon_for_Only_guild = pDungeon;
}

LPDUNGEON CGuild::GetDungeon_for_Only_guild()
{
	return m_pkDungeon_for_Only_guild;
}

#endif

#ifdef ENABLE_NEWSTUFF
void CGuild::SetSkillLevel(DWORD dwVnum, BYTE level, BYTE point)
{
	DWORD dwRealVnum = dwVnum - GUILD_SKILL_START;

	if (dwRealVnum >= GUILD_SKILL_COUNT)
		return;

	CSkillProto* pkSk = CSkillManager::instance().Get(dwVnum);

	if (!pkSk)
	{
		sys_err("There is no such guild skill by number %u", dwVnum);
		return;
	}

	if (level > pkSk->bMaxLevel)
		return;

	if (point)
	{
		if (m_data.skill_point < point)
			return;
		m_data.skill_point -= point;
	}

	m_data.abySkill[dwRealVnum] = level;

	ComputeGuildPoints();
	SaveSkill();
	SendDBSkillUpdate();

	std::for_each(m_memberOnline.begin(), m_memberOnline.end(), msl::bind1st(std::mem_fn(&CGuild::SendSkillInfoPacket), this));

	sys_log(0, "Guild SetSkillLevel: %s %d level %d type %u", GetName(), pkSk->dwVnum, m_data.abySkill[dwRealVnum], pkSk->dwType);
}

DWORD CGuild::GetSkillPoint()
{
	return m_data.skill_point;
}

void CGuild::SetSkillPoint(BYTE point)
{
	m_data.skill_point = point;
	SendDBSkillUpdate();
	std::for_each(m_memberOnline.begin(), m_memberOnline.end(), msl::bind1st(std::mem_fn(&CGuild::SendSkillInfoPacket), this));
}

#endif

uint16_t CGuild::GetSkillValue(uint8_t skillIdx)
{
	return CGuildManager::instance().GetGuildSkillValue(skillIdx, m_data.abySkill[skillIdx]);
}

// From client command
bool CGuild::StartDungeon(uint8_t dungeonIdx)
{
	if (m_data.dung_idx != 0)
	{
		sys_log(0, "StartDungeon BLOCKED: guild %s (id=%u) already has dung_idx=%u active", GetName(), GetID(), m_data.dung_idx);
		return false;
	}

	SGuildDungeonInfo* dungData = CGuildManager::instance().GetDungeonInfoByIdx(dungeonIdx);
	if (!dungData)
	{
		sys_err("No GUILD DUNGEON DATA WITH GUILD DUNGEON INDEX %u", dungeonIdx);
		return false;
	}

	LPDUNGEON pDungeon = CDungeonManager::instance().Create(dungData->mapIndex);
	if (!pDungeon)
	{
		sys_err("CANNOT CREATE PRIVATE GUILD DUNGEON WITH MAP INDEX %u", dungData->mapIndex);
		return false;
	}
	
	if (m_data.tickets[GUILD_TICKETS_TYPE_FREE] > 0)
		m_data.tickets[GUILD_TICKETS_TYPE_FREE]--;
	else if (m_data.tickets[GUILD_TICKETS_TYPE_PAID] > 0)
		m_data.tickets[GUILD_TICKETS_TYPE_PAID]--;
	else
	{
		sys_err("NO AVAILABLE TICKET TO USE! Guild ID %u", m_data.guild_id);
		return false;
	}

	m_mapBossDmg.clear();
	nextTopBossDamageUpdateTime = 0;

	m_data.dung_idx = dungeonIdx;
	m_data.dung_map_idx = pDungeon->GetMapIndex();
	m_data.dung_start_time = get_global_time();

	sys_log(0, "GUILD_DUNGEON_CREATED: guild=%s (id=%u) dungeonIdx=%u origMapIdx=%u privateMapIdx=%u",
		GetName(), GetID(), dungeonIdx, dungData->mapIndex, m_data.dung_map_idx);

	SendUpdateDungeonOpenInfo(NULL, true);
	SendUpdateTicketsCount(NULL);

	std::unique_ptr<SQLMsg> pMsg(DBManager::instance().DirectQuery("UPDATE guild%s SET dung_idx=%u,dung_map_idx=%d,dung_start_time=%d,free_ticket=%u,paid_ticket=%u WHERE id=%u", get_table_postfix(), m_data.dung_idx, m_data.dung_map_idx, m_data.dung_start_time, m_data.tickets[GUILD_TICKETS_TYPE_FREE], m_data.tickets[GUILD_TICKETS_TYPE_PAID], m_data.guild_id));

	TPacketGDGuildNoticeDungeonOpened p;
	p.dwGuild = GetID();
	p.dungeonIdx = m_data.dung_idx;
	p.dungeonMapIdx = m_data.dung_map_idx;
	db_clientdesc->DBPacket(HEADER_GD_GUILD_NOTICE_DUNGEON_OPEN, 0, &p, sizeof(p));

	TPacketGDGuildNoticeTicketsChanged p2;
	p2.dwGuild = GetID();
	thecore_memcpy(p2.tickets, m_data.tickets, sizeof(uint8_t) * GUILD_TICKETS_TYPE_COUNT);
	db_clientdesc->DBPacket(HEADER_GD_GUILD_NOTICE_TICKETS_CHANGED, 0, &p2, sizeof(p2));

	return true;
}

void CGuild::EndDungeon(bool isEndSuccess)
{
	m_mapBossDmg.clear();
	nextTopBossDamageUpdateTime = 0;

	TPacketGDGuildNoticeDungeonEnd p;
	p.dwGuild = GetID();
	p.dungeonIdx = m_data.dung_idx;
	p.isEndSuccess = isEndSuccess;
	p.doneTime = get_global_time() - m_data.dung_start_time;

	if (isEndSuccess)
	{
		DungeonHistoryInsertOrUpdate(m_data.dung_idx, p.doneTime, true);
		SendOneDungeonHistoryData(NULL, m_data.dung_idx);
		
		if (false) // GUILD_SKILL_EXTRA_TICKET_CHANCE removed; slot 4 is now GUILD_SKILL_MORE_RAID_GOLD (applied in questlua guild_add_gold)
		{
			m_data.tickets[GUILD_TICKETS_TYPE_FREE]++;

			SendUpdateTicketsCount(NULL);

			TPacketGDGuildNoticeTicketsChanged p2;
			p2.dwGuild = GetID();
			thecore_memcpy(p2.tickets, m_data.tickets, sizeof(uint8_t) * GUILD_TICKETS_TYPE_COUNT);
			db_clientdesc->DBPacket(HEADER_GD_GUILD_NOTICE_TICKETS_CHANGED, 0, &p2, sizeof(p2));
		}
	}
	db_clientdesc->DBPacket(HEADER_GD_GUILD_NOTICE_DUNGEON_END, 0, &p, sizeof(p));
	
	std::unique_ptr<SQLMsg> pMsg(DBManager::instance().DirectQuery("UPDATE guild%s SET dung_idx=0,dung_map_idx=0,dung_start_time=0,free_ticket=%u WHERE id=%u", get_table_postfix(), m_data.tickets[GUILD_TICKETS_TYPE_FREE], m_data.guild_id));

	m_data.dung_idx = 0;
	m_data.dung_map_idx = 0;
	m_data.dung_start_time = 0;
	SendUpdateDungeonOpenInfo(NULL, false);
}

// From DB
void CGuild::NoticeDungeonOpen(uint8_t dungeonIdx, int dungeonMapIdx)
{
	m_data.dung_idx = dungeonIdx;
	m_data.dung_map_idx = dungeonMapIdx;

	SendUpdateDungeonOpenInfo(NULL, true);
}

// From DB
void CGuild::NoticeDungeonEnd(TPacketGDGuildNoticeDungeonEnd* dungeonEndData)
{
	if (dungeonEndData->isEndSuccess)
	{
		DungeonHistoryInsertOrUpdate(dungeonEndData->dungeonIdx, dungeonEndData->doneTime, false);
		SendOneDungeonHistoryData(NULL, dungeonEndData->dungeonIdx);
	}

	m_data.dung_idx = 0;
	m_data.dung_map_idx = 0;
	m_data.dung_start_time = 0;

	SendUpdateDungeonOpenInfo(NULL, false);
}

void CGuild::SendUpdateDungeonOpenInfo(LPCHARACTER ch, bool isShowInfo)
{
	TPacketGCGuild pack;
	pack.header = HEADER_GC_GUILD;
	pack.size = sizeof(pack) + sizeof(uint8_t) + sizeof(bool);
	pack.subheader = GUILD_SUBHEADER_GC_DUNGEON_OPEN_INFO;

	if (ch)
	{
		LPDESC d = ch->GetDesc();
		if (d)
		{
			d->BufferedPacket(&pack, sizeof(pack));
			d->BufferedPacket(&m_data.dung_idx, sizeof(uint8_t));
			d->Packet(&isShowInfo, sizeof(bool));
		}
	}
	else
	{
		for (const auto& member : m_memberOnline)
		{
			LPDESC d = member->GetDesc();
			if (d)
			{
				d->BufferedPacket(&pack, sizeof(pack));
				d->BufferedPacket(&m_data.dung_idx, sizeof(uint8_t));
				d->Packet(&isShowInfo, sizeof(bool));
			}
		}
	}
}

void CGuild::CheckRefreshTicketLimits()
{
	if (get_global_time() >= m_data.ticketRefreshTime)
	{
		m_data.ticketRefreshTime = GetMidnightTime();
		m_data.tickets[GUILD_TICKETS_TYPE_FREE] = 3;
		m_data.tickets[GUILD_TICKETS_TYPE_PAID] = 0;
		m_data.tickets[GUILD_TICKETS_TYPE_PAID_LIMIT] = 0;

		SendUpdateTicketsCount(NULL);

		std::unique_ptr<SQLMsg> pMsg(DBManager::instance().DirectQuery("UPDATE guild%s SET free_ticket=%u,paid_ticket=%u,paid_ticket_limit=%u,ticket_refresh_time=%d WHERE id=%u", get_table_postfix(), m_data.tickets[GUILD_TICKETS_TYPE_FREE], m_data.tickets[GUILD_TICKETS_TYPE_PAID], m_data.tickets[GUILD_TICKETS_TYPE_PAID_LIMIT], m_data.ticketRefreshTime, m_data.guild_id));

		TPacketGDGuildNoticeTicketsChanged p2;
		p2.dwGuild = GetID();
		thecore_memcpy(p2.tickets, m_data.tickets, sizeof(uint8_t) * GUILD_TICKETS_TYPE_COUNT);
		db_clientdesc->DBPacket(HEADER_GD_GUILD_NOTICE_TICKETS_CHANGED, 0, &p2, sizeof(p2));
	}
}

void CGuild::RequestBuyTicket(LPCHARACTER ch, bool isBuyUsingCoupon)
{
	// Max 3 paid tickets daily
	if (m_data.tickets[GUILD_TICKETS_TYPE_PAID_LIMIT] >= GUILD_TICKET_MAX_PAID_LIMIT)
		return;

	// Not possible to buy ticket if free tickets are available
	if (m_data.tickets[GUILD_TICKETS_TYPE_FREE] > 0)
		return;

	// Not possible to buy more paid tickets if any paid ticket is already bought
	if (m_data.tickets[GUILD_TICKETS_TYPE_PAID] > 0)
		return;

	if (isBuyUsingCoupon)
	{
		if (ch->CountSpecifyItem(GUILD_TICKET_BUY_COUPON_VNUM) <= 0)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "[LS;10130]");
			return;
		}

		ch->RemoveSpecifyItem(GUILD_TICKET_BUY_COUPON_VNUM, 1);
	}
	else
	{
		uint32_t myCoins = 0;
		const TAccountTable& rkTab = ch->GetDesc()->GetAccountTable();
		if (rkTab.id != 0)
		{
			std::unique_ptr<SQLMsg> pmsg(DBManager::instance().DirectQuery("SELECT coins FROM srv1_account.account WHERE id=%lu;", rkTab.id));
			if (pmsg->Get()->uiNumRows > 0)
			{
				MYSQL_ROW row = mysql_fetch_row(pmsg->Get()->pSQLResult);
				if (row != NULL)
					str_to_number(myCoins, row[0]);
			}
		}

		if (myCoins < GUILD_TICKET_SM_COST)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "[LS;10131]");
			return;
		}

		std::unique_ptr<SQLMsg> pmsgUpdate(DBManager::instance().DirectQuery("UPDATE srv1_account.account SET coins=coins-%d WHERE id=%lu;", GUILD_TICKET_SM_COST, rkTab.id));
	}

	m_data.tickets[GUILD_TICKETS_TYPE_PAID]++;
	m_data.tickets[GUILD_TICKETS_TYPE_PAID_LIMIT]++;

	SendUpdateTicketsCount(NULL);
	std::unique_ptr<SQLMsg> pMsg(DBManager::instance().DirectQuery("UPDATE guild%s SET paid_ticket=%u,paid_ticket_limit=%u WHERE id=%u", get_table_postfix(), m_data.tickets[GUILD_TICKETS_TYPE_PAID], m_data.tickets[GUILD_TICKETS_TYPE_PAID_LIMIT], m_data.guild_id));

	TPacketGDGuildNoticeTicketsChanged p2;
	p2.dwGuild = GetID();
	thecore_memcpy(p2.tickets, m_data.tickets, sizeof(uint8_t) * GUILD_TICKETS_TYPE_COUNT);
	db_clientdesc->DBPacket(HEADER_GD_GUILD_NOTICE_TICKETS_CHANGED, 0, &p2, sizeof(p2));

	GuildLog(15, ch->GetName(), ch->GetPlayerID(), NULL, isBuyUsingCoupon ? 1 : 0);
}

// From DB
void CGuild::NoticeTicketsChanged(uint8_t* tickets)
{
	thecore_memcpy(m_data.tickets, tickets, sizeof(uint8_t) * GUILD_TICKETS_TYPE_COUNT);

	SendUpdateTicketsCount(NULL);
}

void CGuild::SendUpdateTicketsCount(LPCHARACTER ch)
{
	const uint8_t size = sizeof(uint8_t) * GUILD_TICKETS_TYPE_COUNT;

	TPacketGCGuild pack;
	pack.header = HEADER_GC_GUILD;
	pack.size = sizeof(pack) + size;
	pack.subheader = GUILD_SUBHEADER_GC_REFRESH_TICKETS;

	if (ch)
	{
		LPDESC d = ch->GetDesc();
		if (d)
		{
			d->BufferedPacket(&pack, sizeof(pack));
			d->Packet(m_data.tickets, size);
		}
	}
	else
	{
		for (const auto& member : m_memberOnline)
		{
			LPDESC d = member->GetDesc();
			if (d)
			{
				d->BufferedPacket(&pack, sizeof(pack));
				d->Packet(m_data.tickets, size);
			}
		}
	}
}

void CGuild::LoadGuildDungeonHistoryData(SQLMsg* pmsg)
{
	if (!CGuildManager::instance().FindGuild(m_data.guild_id))
		return;

	if (pmsg->Get()->uiNumRows == 0)
		return;
	
	m_vecDungeonHistory.clear();

	for (uint i = 0; i < pmsg->Get()->uiNumRows; ++i)
	{
		MYSQL_ROW row = mysql_fetch_row(pmsg->Get()->pSQLResult);

		TGuildDungeonHistoryInfo historyData;
		historyData.dungeonIdx = static_cast<uint8_t>(strtoul(row[0], (char**)NULL, 10));
		historyData.doneCount = static_cast<uint16_t>(strtoul(row[1], (char**)NULL, 10));
		historyData.bestDoneTime = strtoul(row[2], (char**)NULL, 10);
		
		m_vecDungeonHistory.push_back(historyData);
	}
}

void CGuild::DungeonHistoryInsertOrUpdate(uint8_t dungeonIdx, uint32_t doneTime, bool runQuery)
{
	for (auto& data : m_vecDungeonHistory)
	{
		if (data.dungeonIdx == dungeonIdx)
		{
			data.doneCount++;
			if (doneTime < data.bestDoneTime)
				data.bestDoneTime = doneTime;

			if (runQuery)
				std::unique_ptr<SQLMsg> pMsg(DBManager::instance().DirectQuery("UPDATE guild_dungeons%s SET done_count=%u,best_time=%u WHERE guild_id=%u AND dungeon_idx=%u", get_table_postfix(), data.doneCount, data.bestDoneTime, m_data.guild_id, dungeonIdx));
			return;
		}
	}

	// if not already exists create new entry

	TGuildDungeonHistoryInfo historyData;
	historyData.dungeonIdx = dungeonIdx;
	historyData.doneCount = 1;
	historyData.bestDoneTime = doneTime;
	m_vecDungeonHistory.push_back(historyData);

	if (runQuery)
		std::unique_ptr<SQLMsg> pMsg(DBManager::instance().DirectQuery("INSERT INTO guild_dungeons%s VALUES(%u,%u,1,%u)", get_table_postfix(), m_data.guild_id, dungeonIdx, doneTime));
}

void CGuild::RefreshDungeonHistory(LPCHARACTER ch)
{
	if (ch->IsGuildDungeonHistoryLoaded())
		return;

	ch->SetIsGuildDungeonHistoryLoaded(true);

	SendAllDungeonHistoryData(ch);
}

void CGuild::SendAllDungeonHistoryData(LPCHARACTER ch)
{
	if (m_vecDungeonHistory.empty())
		return;

	const uint16_t size = sizeof(TGuildDungeonHistoryInfo) * m_vecDungeonHistory.size();

	TPacketGCGuild pack;
	pack.header = HEADER_GC_GUILD;
	pack.size = sizeof(pack) + size;
	pack.subheader = GUILD_SUBHEADER_GC_DUNGEON_HISTORY_DATA;

	if (ch)
	{
		LPDESC d = ch->GetDesc();
		if (d)
		{
			d->BufferedPacket(&pack, sizeof(pack));
			d->Packet(m_vecDungeonHistory.data(), size);
		}
	}
	else
	{
		for (const auto& member : m_memberOnline)
		{
			LPDESC d = member->GetDesc();
			if (d)
			{
				d->BufferedPacket(&pack, sizeof(pack));
				d->Packet(m_vecDungeonHistory.data(), size);
			}
		}
	}
}

void CGuild::SendOneDungeonHistoryData(LPCHARACTER ch, uint8_t dungIdx)
{
	TGuildDungeonHistoryInfo* dungInfo = NULL;
	for (auto& data : m_vecDungeonHistory)
	{
		if (data.dungeonIdx == dungIdx)
			dungInfo = &data;
	}

	if (!dungInfo)
		return;

	TPacketGCGuild pack;
	pack.header = HEADER_GC_GUILD;
	pack.size = sizeof(pack) + sizeof(TGuildDungeonHistoryInfo);
	pack.subheader = GUILD_SUBHEADER_GC_DUNGEON_HISTORY_DATA;

	if (ch)
	{
		LPDESC d = ch->GetDesc();
		if (d)
		{
			d->BufferedPacket(&pack, sizeof(pack));
			d->Packet(dungInfo, sizeof(TGuildDungeonHistoryInfo));
		}
	}
	else
	{
		for (const auto& member : m_memberOnline)
		{
			LPDESC d = member->GetDesc();
			if (d)
			{
				d->BufferedPacket(&pack, sizeof(pack));
				d->Packet(dungInfo, sizeof(TGuildDungeonHistoryInfo));
			}
		}
	}
}

bool dmgCompare(const TGuildDungeonBossDamageInfo& i1, const TGuildDungeonBossDamageInfo& i2)
{
	return (i1.totalDmg > i2.totalDmg);
}

void CGuild::DungeonUpdateMainBossDmg(uint32_t attackerPID, uint32_t addDmg, bool isForceSentUpdate)
{
	const auto& attacketIt = m_mapBossDmg.find(attackerPID);
	if (attacketIt == m_mapBossDmg.end())
		m_mapBossDmg[attackerPID] = addDmg;
	else
		attacketIt->second += addDmg;

	if (get_global_time() <= nextTopBossDamageUpdateTime && !isForceSentUpdate)
		return;

	nextTopBossDamageUpdateTime = get_global_time() + 5;

	std::vector<TGuildDungeonBossDamageInfo> m_vecBossDamage;
	m_vecBossDamage.reserve(m_mapBossDmg.size());
	for (const auto& it : m_mapBossDmg)
		m_vecBossDamage.push_back({it.first, it.second});

	std::sort(m_vecBossDamage.begin(), m_vecBossDamage.end(), dmgCompare);

	const int sizeToSent = sizeof(TGuildDungeonBossDamageInfo) * MIN(m_vecBossDamage.size(), 10);

	TPacketGCGuild pack;
	pack.header = HEADER_GC_GUILD;
	pack.size = sizeof(pack) + sizeToSent + sizeof(uint32_t);
	pack.subheader = GUILD_SUBHEADER_GC_DUNGEON_BOSS_TOP_DMG;

	const uint32_t emptyDmg = 0;
	for (const auto& member : m_memberOnline)
	{
		if (member->GetMapIndex() != m_data.dung_map_idx)
			continue;

		LPDESC d = member->GetDesc();
		if (d)
		{
			d->BufferedPacket(&pack, sizeof(pack));
			d->BufferedPacket(m_vecBossDamage.data(), sizeToSent);

			const auto& myDmgIt = m_mapBossDmg.find(member->GetPlayerID());
			if (myDmgIt == m_mapBossDmg.end())
				d->Packet(&emptyDmg, sizeof(uint32_t));
			else
				d->Packet(&myDmgIt->second, sizeof(uint32_t));
		}
	}
}

void CGuild::RefreshLogs()
{
	time_t maxLogTime = get_global_time() - 60 * 60 * 24 * 3;

	while (!guildLogsVec.empty() && guildLogsVec.begin()->time < maxLogTime)
		guildLogsVec.erase(guildLogsVec.begin());

	std::unique_ptr<SQLMsg> pmsg(DBManager::instance().DirectQuery("SELECT id,time,logid,str1,uint1,str2,uint2 FROM srv1_log.guild_log WHERE gid=%u AND time >= %u AND id > %u", m_data.guild_id, maxLogTime, lastLogID));

	uint32_t newLogID = 0;
	for (uint i = 0; i < pmsg->Get()->uiNumRows; i++)
	{
		MYSQL_ROW row = mysql_fetch_row(pmsg->Get()->pSQLResult);

		newLogID = strtoul(row[0], (char**)NULL, 10);

		TGuildLog logData;
		logData.time = strtoul(row[1], (char**)NULL, 10);
		logData.logID = static_cast<uint8_t>(strtoul(row[2], (char**)NULL, 10));

		if (row[3])
			strlcpy(logData.str1, row[3], sizeof(logData.str1));
		else
			logData.str1[0] = ' ';

		logData.uint1 = strtoul(row[4], (char**)NULL, 10);

		if (row[5])
			strlcpy(logData.str2, row[5], sizeof(logData.str2));
		else
			logData.str2[0] = ' ';

		logData.uint2 = strtoul(row[6], (char**)NULL, 10);

		guildLogsVec.push_back(logData);

		if (newLogID > lastLogID)
			lastLogID = newLogID;
	}

	while (!guildLogsVec.empty() && guildLogsVec.size() > 400)
		guildLogsVec.erase(guildLogsVec.begin());
}

void CGuild::RefreshMyTodayGivenExpData(LPCHARACTER ch)
{
	if (!ch)
		return;

	quest::PC* pPC = quest::CQuestManager::instance().GetPC(ch->GetPlayerID());
	if (!pPC)
		return;

	RefreshMyTodayGivenExpData(ch, pPC->GetFlag("guild_manage.today_exp"), pPC->GetFlag("guild_manage.exp_limit_refresh"));
}

void CGuild::RefreshMyTodayGivenExpData(LPCHARACTER ch, int todayExp, int todayExpRefreshTime)
{
	if (!ch)
		return;

	LPDESC d = ch->GetDesc();
	if (!d)
		return;

	TPacketGCGuild pack;
	pack.header = HEADER_GC_GUILD;
	pack.size = sizeof(pack) + sizeof(int) * 2;
	pack.subheader = GUILD_SUBHEADER_GC_MY_GIVEN_EXP_UPDATE;

	d->BufferedPacket(&pack, sizeof(pack));
	d->BufferedPacket(&todayExp, sizeof(int));
	d->Packet(&todayExpRefreshTime, sizeof(int));
}

void CGuild::GuildLog(uint8_t logID, const char* str1, uint32_t uint1, const char* str2, uint32_t uint2)
{
	/*
	 * 1 -> Reset skill points
	 * 2 -> Unequip guard item
	 * 3 -> Equip guard item
	 * 4 -> Guard Bonuses turn off
	 * 5 -> Guard Bonuses turn on
	 *
	 * NOT IMPLEMENTED:
	 * 6 -> ww start
	 * 7 -> ww start other side accept
	 * 8 -> ww start other side not accept
	 * 9 -> ww peace offer
	 * 10 -> ww accept again
	 *
	 * NOT IMPLEMENTED:
	 * 11 -> alliance set
	 * 12 -> alliance cancel
	 * 13 -> alliance cancel from other guild
	 * 14 -> alliance proposal from other guild
	 *
	 * 15 -> ticket buy
	 * 16 -> guild dungeon opened
	 *
	 * 17 -> change grade from recruit to member
	 *
	 * 18 -> guild invite accept
	 * 19 -> guild withdraw
	 * 20 -> guild member removed by ...
	 * 21 -> vice leader advance
	 * 22 -> guild lvlup
	 * 23 -> guard lvlup
	 * 24 -> Guild leader changed
	 * 25 -> note changed
	 * 26 -> guild logo changed
	 */

	switch (logID)
	{
		case 1:
		case 4:
		case 5:
		case 8:
		case 13:
		case 14:
		case 15:
		case 17:
		case 18:
		case 19:
		case 21:
		case 24:
		case 25:
		case 26:
		{
			LogManager::instance().GuildLog(get_global_time(), m_data.guild_id, logID, str1, uint1);
			break;
		}
		case 2:
		case 3:
		case 16:
		{
			LogManager::instance().GuildLog(get_global_time(), m_data.guild_id, logID, str1, uint1, uint2);
			break;
		}
		case 6:
		case 7:
		case 9:
		case 10:
		case 11:
		case 12:
		case 20:
		{
			LogManager::instance().GuildLog(get_global_time(), m_data.guild_id, logID, str1, uint1, str2, uint2);
			break;
		}
		case 22:
		case 23:
		{
			LogManager::instance().GuildLog(get_global_time(), m_data.guild_id, logID, uint1);
			break;
		}

		default:
			LogManager::instance().GuildLog(get_global_time(), m_data.guild_id, logID, str1, uint1, str2, uint2);
			break;
	}
}

void CGuild::SendLogs(LPCHARACTER ch, uint32_t myLastLogID)
{
	if (!ch)
		return;

	auto member = GetMember(ch->GetPlayerID());
	if (!member)
		return;

	//if (!HasGradeAuth(member->grade, GUILD_AUTH_LOGS))
	//{
	//	if (test_server)
	//		ch->ChatPacket(CHAT_TYPE_INFO, "[TEST_SERVER] You dont have privilege to check guild logs!");
	//	return;
	//}

	LPDESC d = ch->GetDesc();
	if (!d)
		return;

	if (ch->GetNextGuildLogsRefreshTime() >= get_global_time())
	{
		if (test_server)
			ch->ChatPacket(CHAT_TYPE_INFO, "[TEST_SERVER] You can not refresh logs now, there is still a cooldown!");
		return;
	}
	uint32_t nextPlayerLogsRefreshTime = static_cast<uint32_t>(get_global_time() + 60 * 5 + 5);
	ch->SetNextGuildLogsRefreshTime(nextPlayerLogsRefreshTime - 5);

	if (get_global_time() >= nextLogsRefreshTime)
	{
		RefreshLogs();
		nextLogsRefreshTime = nextPlayerLogsRefreshTime-5;
	}

	if (lastLogID == 0 || guildLogsVec.empty())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "[LS;10132]");
		return;
	}
	
	if (lastLogID == myLastLogID)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "[LS;10133]");
		return;
	}

	const uint16_t logsSize = sizeof(TGuildLog) * guildLogsVec.size();
	
	TPacketGCGuild p;
	p.header = HEADER_GC_GUILD;
	p.size = sizeof(p) + sizeof(uint32_t) + sizeof(uint32_t) + logsSize;  // Changed to uint32_t!
	p.subheader = GUILD_SUBHEADER_GC_SET_LOGS;
	
	TEMP_BUFFER buf(p.size);
	buf.write(&p, sizeof(p));
	buf.write(&lastLogID, sizeof(uint32_t));
	
	uint32_t nextRefreshTime32 = static_cast<uint32_t>(nextLogsRefreshTime);
	buf.write(&nextRefreshTime32, sizeof(uint32_t));  // Write 4 bytes instead of 8!
	
	buf.write(guildLogsVec.data(), logsSize);
	
	d->Packet(buf.read_peek(), buf.size());
}

void CGuild::LoadGuildGuardItemsData(SQLMsg* pmsg)
{
	if (pmsg->Get()->uiNumRows == 0)
		return;

	m_vecGuildItems[0].clear();
	m_vecGuildItems[1].clear();

	m_mapGuildGuardItemsBonuses.clear();

	for (uint i = 0; i < pmsg->Get()->uiNumRows; ++i)
	{
		MYSQL_ROW row = mysql_fetch_row(pmsg->Get()->pSQLResult);

		TGuildGuardItemData itemData;
		uint8_t set_id = strtoul(row[0], (char**)NULL, 10);
		str_to_number(itemData.pos, row[1]);
		str_to_number(itemData.vnum, row[2]);

		m_vecGuildItems[set_id].push_back(itemData);

		if (itemData.vnum != 0 && m_data.guard_active_set_id == set_id+1)
		{
			const TItemTable* itemTable = ITEM_MANAGER::instance().GetTable(itemData.vnum);
			if (!itemTable)
				continue;

			for (int j = 0; j < ITEM_APPLY_MAX_NUM; ++j)
			{
				if (itemTable->aApplies[j].bType == 0)
					continue;

				auto it = m_mapGuildGuardItemsBonuses.find(itemTable->aApplies[j].bType);
				if (it == m_mapGuildGuardItemsBonuses.end())
					m_mapGuildGuardItemsBonuses.insert(std::make_pair(itemTable->aApplies[j].bType, itemTable->aApplies[j].lValue));
				else
					it->second += itemTable->aApplies[j].lValue;
			}
		}
	}
}

TGuildGuardItemData* CGuild::GetGuardItemData(uint8_t setID, uint8_t pos)
{
	for (size_t i = 0; i < m_vecGuildItems[setID].size(); i++)
	{
		if (m_vecGuildItems[setID][i].pos == pos)
			return &m_vecGuildItems[setID][i];
	}

	return NULL;
}

void CGuild::RefreshGuardItems(LPCHARACTER ch)
{
	// Always send storage info so material counts and skill levels are current
	SendStorageInfo(ch);

	if (ch->IsGuildGuardItemsLoaded())
		return;

	ch->SetIsGuildGuardItemsLoaded(true);

	SendBaseGuardInfo(ch);
	SendAllGuardItemsData(ch, 0);
	SendAllGuardItemsData(ch, 1);
}

void CGuild::EquipItem(LPCHARACTER ch, uint8_t setID, LPITEM item)
{
	if (!ch)
	{
		return;
	}
	
	if (!item)
	{
		return;
	}
	
	if (setID != 0 && setID != 1)
	{
		return;
	}
	
	if (!HasGradeAuth(GetMember(ch->GetPlayerID())->grade, GUILD_AUTH_MANAGE_GUARD))
	{
		return;
	}
	
	if (!ch->CanWarp() || quest::CQuestManager::instance().GetPCForce(ch->GetPlayerID())->IsRunning())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "[LS;10126]");
		return;
	}
	
	if (item->GetWindow() != INVENTORY)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "[LS;10127]");
		return;
	}
	
	if (item->GetCell() < 0 || item->GetCell() >= INVENTORY_MAX_NUM)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "[LS;10127]");
		return;
	}
	
	if (item->IsExchanging() || item->isLocked())
	{
		return;
	}
	
	if (item->GetType() != ITEM_GUILD_GUARD)
	{
		return;
	}
	
	if (GetGuardItemData(setID, item->GetSubType()))
	{
		return;
	}
	
	if (m_data.guard_active_set_id != 0)
	{
		return;
	}
	
	if (item->GetSocket(0) != 0 && item->GetSocket(0) != m_data.guild_id)
	{
		if (get_global_time() <= item->GetSocket(1) + 60 * 60 * 24)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "[LS;10128]");
			return;
		}
	}
	
	if (m_dwLastUseGuardItem + 600 > get_dword_time())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "[LS;10122]");
		return;
	}
	
	m_dwLastUseGuardItem = get_dword_time();
	
	TGuildGuardItemData itemData;
	itemData.vnum = item->GetVnum();
	itemData.pos = item->GetSubType();
	
	m_vecGuildItems[setID].push_back(itemData);
	
	std::unique_ptr<SQLMsg> pMsg(DBManager::instance().DirectQuery(
		"INSERT INTO guild_guard_items%s VALUES(%u,%u,%u,%u)", 
		get_table_postfix(), m_data.guild_id, setID, itemData.pos, itemData.vnum));
	
	if (pMsg->Get()->uiAffectedRows == 0)
	{
		sys_err("CGuild::EquipItem - DB INSERT failed! No rows affected!");
	}
	else
	{
		sys_log(0, "CGuild::EquipItem - DB INSERT successful, affected rows: %u", pMsg->Get()->uiAffectedRows);
	}
	
	item->SetCount(0);
	
	SendOneGuardItemSetData(NULL, setID, itemData.pos);
	
	TPacketGDGuildUpdateGuardItemData p;
	p.dwGuild = m_data.guild_id;
	p.setID = setID;
	p.pos = itemData.pos;
	p.vnum = itemData.vnum;
	
	db_clientdesc->DBPacket(HEADER_GD_GUILD_UPDATE_GUARD_ITEM_DATA, 0, &p, sizeof(p));

	GuildLog(3, ch->GetName(), ch->GetPlayerID(), NULL, itemData.vnum);
}

void CGuild::UnequipItem(LPCHARACTER ch, uint8_t setID, uint8_t slot)
{
	if (!ch)
		return;

	if (setID != 0 && setID != 1)
		return;

	if (slot < 0 || slot > 8)
		return;

	if (!HasGradeAuth(GetMember(ch->GetPlayerID())->grade, GUILD_AUTH_MANAGE_GUARD))
		return;

	//if (!CGuildManager::instance().CanFullManageGuildOnThisMap(ch))
	//	return;

	if (!ch->CanWarp() || quest::CQuestManager::instance().GetPCForce(ch->GetPlayerID())->IsRunning())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "[LS;10126]");
		return;
	}

	auto itemDataIt = m_vecGuildItems[setID].begin();
	while (itemDataIt != m_vecGuildItems[setID].end())
	{
		if (itemDataIt->pos == slot)
			break;
		++itemDataIt;
	}

	if (itemDataIt == m_vecGuildItems[setID].end())
		return;
	
	if (m_data.guard_active_set_id != 0)
		return;
	
	if (m_dwLastUseGuardItem + 600 > get_dword_time())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "[LS;10122]");
		return;
	}
	m_dwLastUseGuardItem = get_dword_time();

	const TItemTable* itemTable = ITEM_MANAGER::instance().GetTable(itemDataIt->vnum);
	if (!itemTable)
		return;

	if (!ch->GetEmptyInventory(itemTable->bSize) == -1)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "[LS;10129]");
		return;
	}

	LPITEM item = ch->AutoGiveItem(itemDataIt->vnum, 1);
	if (!item)
		return;

	item->SetSocket(0, m_data.guild_id);
	item->SetSocket(1, get_global_time());

	GuildLog(2, ch->GetName(), ch->GetPlayerID(), NULL, itemDataIt->vnum);

	m_vecGuildItems[setID].erase(itemDataIt);
	std::unique_ptr<SQLMsg> pMsg(DBManager::instance().DirectQuery("DELETE FROM guild_guard_items%s WHERE guild_id=%u AND set_id=%u AND pos=%u", get_table_postfix(), m_data.guild_id, setID, slot));

	SendOneGuardItemRemoveData(NULL, setID, slot);

	TPacketGDGuildUpdateGuardItemData p;
	p.dwGuild = m_data.guild_id;
	p.setID = setID;
	p.pos = slot;
	p.vnum = 0;
	db_clientdesc->DBPacket(HEADER_GD_GUILD_UPDATE_GUARD_ITEM_DATA, 0, &p, sizeof(p));
}

// From DB
void CGuild::UpdateGuardItemData(TPacketGDGuildUpdateGuardItemData* p)
{
	// Remove
	if (p->vnum == 0)
	{
		for (auto iter = m_vecGuildItems[p->setID].begin(); iter != m_vecGuildItems[p->setID].end(); ++iter)
		{
			if (iter->pos == p->pos)
			{
				iter = m_vecGuildItems[p->setID].erase(iter);
				break;
			}
		}

		SendOneGuardItemRemoveData(NULL, p->setID, p->pos);
		return;
	}

	bool bFound = false;
	for (size_t i = 0; i < m_vecGuildItems[p->setID].size(); i++)
	{
		if (m_vecGuildItems[p->setID][i].pos == p->pos)
		{
			m_vecGuildItems[p->setID][i].vnum = p->vnum;
			bFound = true;
			break;
		}
	}

	if (!bFound)
	{
		TGuildGuardItemData itemData;
		itemData.vnum = p->vnum;
		itemData.pos = p->pos;
		m_vecGuildItems[p->setID].push_back(itemData);
	}

	SendOneGuardItemSetData(NULL, p->setID, p->pos);
}
void CGuild::SendBaseGuardInfo(LPCHARACTER ch)
{
	TPacketGCGuild pack;
	pack.header = HEADER_GC_GUILD;
	pack.size = sizeof(pack) + sizeof(TPacketGCGuildGuardInfo);
	pack.subheader = GUILD_SUBHEADER_GC_GUARD_BASE_INFO;

	TPacketGCGuildGuardInfo pack2;
	pack2.guard_active_set_id = m_data.guard_active_set_id;
	if (m_dwNextCanChangeBonusActiveStatusTime >= get_global_time())
		pack2.bonus_can_activate_again_sec = (uint32_t)(m_dwNextCanChangeBonusActiveStatusTime - get_global_time());
	else
		pack2.bonus_can_activate_again_sec = 0;

	if (m_data.guard_active_set_id != 0 && m_data.guard_active_expire_time > get_global_time())
		pack2.guard_bonus_expire_sec = (uint32_t)(m_data.guard_active_expire_time - get_global_time());
	else
		pack2.guard_bonus_expire_sec = 0;

	if (ch)
	{
		LPDESC d = ch->GetDesc();
		if (d)
		{
			d->BufferedPacket(&pack, sizeof(pack));
			d->Packet(&pack2, sizeof(pack2));
		}
	}
	else
	{
		for (const auto& member : m_memberOnline)
		{
			LPDESC d = member->GetDesc();
			if (d)
			{
				d->BufferedPacket(&pack, sizeof(pack));
				d->Packet(&pack2, sizeof(pack2));
			}
		}
	}
}

static int GetDailyDonatedCount(LPCHARACTER member, const char* cntFlag)
{
	int today   = (int)(get_global_time() / 86400);
	int lastDay = member->GetQuestFlag("guild_donation.gd_day");
	return (lastDay == today) ? member->GetQuestFlag(cntFlag) : 0;
}

void CGuild::SendStorageInfoToPlayer(LPCHARACTER member)
{
	LPDESC d = member->GetDesc();
	if (!d)
		return;

	TPacketGCGuild pack;
	pack.header    = HEADER_GC_GUILD;
	pack.size      = sizeof(pack) + sizeof(TPacketGCGuildStorageInfo);
	pack.subheader = GUILD_SUBHEADER_GC_STORAGE_INFO;

	TPacketGCGuildStorageInfo pack2;
	pack2.donate_stone   = m_data.donate_stone;
	pack2.donate_log     = m_data.donate_log;
	pack2.donate_plywood = m_data.donate_plywood;
	pack2.daily_stone    = GetDailyDonatedCount(member, "guild_donation.gd_s_cnt");
	pack2.daily_log      = GetDailyDonatedCount(member, "guild_donation.gd_l_cnt");
	pack2.daily_plywood  = GetDailyDonatedCount(member, "guild_donation.gd_p_cnt");
	for (int i = 0; i < 5; ++i)
		pack2.bonus_level[i] = m_data.abySkill[i];

	d->BufferedPacket(&pack, sizeof(pack));
	d->Packet(&pack2, sizeof(pack2));
}

void CGuild::SendStorageInfo(LPCHARACTER ch)
{
	if (ch)
	{
		SendStorageInfoToPlayer(ch);
	}
	else
	{
		for (const auto& member : m_memberOnline)
			SendStorageInfoToPlayer(member);
	}
}

void CGuild::SendAllGuardItemsData(LPCHARACTER ch, uint8_t setID)
{
	if (m_vecGuildItems[setID].empty())
		return;

	const uint16_t size = sizeof(TGuildGuardItemData) * m_vecGuildItems[setID].size();

	TPacketGCGuild pack;
	pack.header = HEADER_GC_GUILD;
	pack.size = sizeof(pack) + sizeof(uint8_t) + size;
	pack.subheader = GUILD_SUBHEADER_GC_GUARD_ITEMS_SET_DATA;

	if (ch)
	{
		LPDESC d = ch->GetDesc();
		if (d)
		{
			d->BufferedPacket(&pack, sizeof(pack));
			d->BufferedPacket(&setID, sizeof(uint8_t));
			d->Packet(m_vecGuildItems[setID].data(), size);
		}
	}
	else
	{
		for (const auto& member : m_memberOnline)
		{
			LPDESC d = member->GetDesc();
			if (d)
			{
				d->BufferedPacket(&pack, sizeof(pack));
				d->BufferedPacket(&setID, sizeof(uint8_t));
				d->Packet(m_vecGuildItems[setID].data(), size);
			}
		}
	}
}

void CGuild::SendOneGuardItemSetData(LPCHARACTER ch, uint8_t setID, uint8_t pos)
{
	TGuildGuardItemData* itemData = NULL;
	for (auto& data : m_vecGuildItems[setID])
	{
		if (data.pos == pos)
			itemData = &data;
	}

	if (!itemData)
		return;

	TPacketGCGuild pack;
	pack.header = HEADER_GC_GUILD;
	pack.size = sizeof(pack) + sizeof(uint8_t) + sizeof(TGuildGuardItemData);
	pack.subheader = GUILD_SUBHEADER_GC_GUARD_ITEMS_SET_DATA;

	if (ch)
	{
		LPDESC d = ch->GetDesc();
		if (d)
		{
			d->BufferedPacket(&pack, sizeof(pack));
			d->BufferedPacket(&setID, sizeof(uint8_t));
			d->Packet(itemData, sizeof(TGuildGuardItemData));
		}
	}
	else
	{
		for (const auto& member : m_memberOnline)
		{
			LPDESC d = member->GetDesc();
			if (d)
			{
				d->BufferedPacket(&pack, sizeof(pack));
				d->BufferedPacket(&setID, sizeof(uint8_t));
				d->Packet(itemData, sizeof(TGuildGuardItemData));
			}
		}
	}
}

void CGuild::SendOneGuardItemRemoveData(LPCHARACTER ch, uint8_t setID, uint8_t pos)
{
	TPacketGCGuild pack;
	pack.header = HEADER_GC_GUILD;
	pack.size = sizeof(pack) + sizeof(uint8_t) * 2;
	pack.subheader = GUILD_SUBHEADER_GC_GUARD_ITEM_REMOVE_DATA;

	if (ch)
	{
		LPDESC d = ch->GetDesc();
		if (d)
		{
			d->BufferedPacket(&pack, sizeof(pack));
			d->BufferedPacket(&setID, sizeof(uint8_t));
			d->Packet(&pos, sizeof(uint8_t));
		}
	}
	else
	{
		for (const auto& member : m_memberOnline)
		{
			LPDESC d = member->GetDesc();
			if (d)
			{
				d->BufferedPacket(&pack, sizeof(pack));
				d->BufferedPacket(&setID, sizeof(uint8_t));
				d->Packet(&pos, sizeof(uint8_t));
			}
		}
	}
}

bool CGuild::GuardActivateItemsSet(uint8_t setID)
{
	if (setID > 1)
	{
		return false;
	}
	
	if (m_dwNextCanChangeBonusActiveStatusTime > get_global_time())
	{
		return false;
	}
	
	if (m_vecGuildItems[setID].empty())
	{
		return false;
	}
	
	m_mapGuildGuardItemsBonuses.clear();
	
	uint32_t guardSetPower = 0;
	
	for (const auto& itemData : m_vecGuildItems[setID])
	{
		const TItemTable* itemTable = ITEM_MANAGER::instance().GetTable(itemData.vnum);
		if (!itemTable)
		{
			continue;
		}
		
		guardSetPower += itemTable->alValues[0];
		
		for (int j = 0; j < ITEM_APPLY_MAX_NUM; ++j)
		{
			if (itemTable->aApplies[j].bType == 0)
				continue;
			
			auto it = m_mapGuildGuardItemsBonuses.find(itemTable->aApplies[j].bType);
			if (it == m_mapGuildGuardItemsBonuses.end())
				m_mapGuildGuardItemsBonuses.insert(std::make_pair(itemTable->aApplies[j].bType, itemTable->aApplies[j].lValue));
			else
				it->second += itemTable->aApplies[j].lValue;
		}
	}
	
	m_data.guard_active_set_id = setID + 1;
	m_data.guard_active_expire_time = get_global_time() + GUILD_GUARD_BONUS_DURATION;

	m_dwNextCanChangeBonusActiveStatusTime = get_global_time() + 180;
	if (test_server)
		m_dwNextCanChangeBonusActiveStatusTime = get_global_time() + 5;

	SendBaseGuardInfo(NULL);

	RefreshAllMembersGuardBonuses();

	std::unique_ptr<SQLMsg> pMsg(DBManager::instance().DirectQuery(
		"UPDATE guild%s SET guard_active_set=%u, guard_active_expire_time=%d WHERE id=%u",
		get_table_postfix(), m_data.guard_active_set_id, (int)m_data.guard_active_expire_time, m_data.guild_id));

	TPacketGuildUpdateGuardActiveSetData p;
	p.dwGuild = m_data.guild_id;
	p.activeSetID = setID + 1;
	p.nextCanChangeBonusActiveStatusTime = m_dwNextCanChangeBonusActiveStatusTime;
	p.guard_active_expire_time = m_data.guard_active_expire_time;
	db_clientdesc->DBPacket(HEADER_GD_GUILD_UPDATE_GUARD_ACTIVE_SET_DATA, 0, &p, sizeof(p));

	return true;
}

bool CGuild::GuardDeactivateItemsSet()
{
	if (m_dwNextCanChangeBonusActiveStatusTime > get_global_time())
		return false;

	m_mapGuildGuardItemsBonuses.clear();
	m_data.guard_active_set_id = 0;
	m_data.guard_active_expire_time = 0;
	m_dwNextCanChangeBonusActiveStatusTime = get_global_time() + 180;
	if (test_server)
		m_dwNextCanChangeBonusActiveStatusTime = get_global_time() + 5;
	SendBaseGuardInfo(NULL);
	RefreshAllMembersGuardBonuses();

	std::unique_ptr<SQLMsg> pMsg(DBManager::instance().DirectQuery("UPDATE guild%s SET guard_active_set=0, guard_active_expire_time=0 WHERE id=%u", get_table_postfix(), m_data.guild_id));

	TPacketGuildUpdateGuardActiveSetData p;
	p.dwGuild = m_data.guild_id;
	p.activeSetID = 0;
	p.nextCanChangeBonusActiveStatusTime = m_dwNextCanChangeBonusActiveStatusTime;
	p.guard_active_expire_time = 0;
	db_clientdesc->DBPacket(HEADER_GD_GUILD_UPDATE_GUARD_ACTIVE_SET_DATA, 0, &p, sizeof(p));

	return true;
}

// From DB
void CGuild::GuardGetActiveSetData(uint8_t activeSetID, time_t nextCanChangeBonusActiveStatusTime, time_t guard_active_expire_time)
{
	m_dwNextCanChangeBonusActiveStatusTime = nextCanChangeBonusActiveStatusTime;
	m_data.guard_active_expire_time = guard_active_expire_time;
	m_mapGuildGuardItemsBonuses.clear();

	// Auto-expire if bonus duration ran out
	if (activeSetID != 0 && guard_active_expire_time != 0 && get_global_time() > guard_active_expire_time)
	{
		m_data.guard_active_set_id = 0;
		m_data.guard_active_expire_time = 0;
		DBManager::instance().Query("UPDATE guild%s SET guard_active_set=0, guard_active_expire_time=0 WHERE id=%u", get_table_postfix(), m_data.guild_id);
		SendBaseGuardInfo(NULL);
		RefreshAllMembersGuardBonuses();
		return;
	}

	m_data.guard_active_set_id = activeSetID;

	if (activeSetID != 0)
	{
		if (!m_vecGuildItems[activeSetID - 1].empty())
		{
			uint32_t guardSetPower = 0;
			for (const auto& itemData : m_vecGuildItems[activeSetID - 1])
			{
				const TItemTable* itemTable = ITEM_MANAGER::instance().GetTable(itemData.vnum);
				if (!itemTable)
					continue;

				guardSetPower += itemTable->alValues[0];

				for (int j = 0; j < ITEM_APPLY_MAX_NUM; ++j)
				{
					if (itemTable->aApplies[j].bType == 0)
						continue;

					auto it = m_mapGuildGuardItemsBonuses.find(itemTable->aApplies[j].bType);
					if (it == m_mapGuildGuardItemsBonuses.end())
						m_mapGuildGuardItemsBonuses.insert(std::make_pair(itemTable->aApplies[j].bType, itemTable->aApplies[j].lValue));
					else
						it->second += itemTable->aApplies[j].lValue;
				}
			}
		}
	}

	SendBaseGuardInfo(NULL);
	RefreshAllMembersGuardBonuses();
}

void CGuild::RefreshAllMembersGuardBonuses()
{
	for (const auto& member : m_memberOnline)
	{
		//if (IsRecruit(member->GetPlayerID()))
		//	continue;

		member->ComputePoints();
		member->PointsPacket();
	}
}
//martysama0134's ceqyqttoaf71vasf9t71218
