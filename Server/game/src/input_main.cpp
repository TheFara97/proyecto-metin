#include "stdafx.h"
#include "constants.h"
#include "config.h"
#include "utils.h"
#include "desc_client.h"
#include "desc_manager.h"
#include "buffer_manager.h"
#include "packet.h"
#include "protocol.h"
#include "char.h"
#include "char_manager.h"
#include "item.h"
#include "item_manager.h"
#include "cmd.h"
#include "shop.h"
#include "shop_manager.h"
#include "safebox.h"
#include "regen.h"
#include "battle.h"
#include "exchange.h"
#include "questmanager.h"
#include "profiler.h"
#include "messenger_manager.h"
#include "party.h"
#include "p2p.h"
#include "affect.h"
#include "guild.h"
#include "guild_manager.h"
#include "log.h"
#include "banword.h"
#include "empire_text_convert.h"
#include "unique_item.h"
#include "building.h"
#include "locale_service.h"

#include "MountSystem.h"
#include "gm.h"
#include "spam.h"
#include "ani.h"
#include "motion.h"
#include "OXEvent.h"
#include "locale_service.h"
#include "DragonSoul.h"
#ifdef ENABLE_ASLAN_BUFF_NPC_SYSTEM
#include "buff_npc_system.h"
#endif
#ifdef ENABLE_RESP_SYSTEM
#include "resp_manager.h"
#endif
#include "belt_inventory_helper.h" // @fixme119
#include "../../common/CommonDefines.h"
#include "../../common/tables.h"
#include "../../common/PulseManager.h"
#ifdef ENABLE_OFFLINE_SHOP_SYSTEM
	#include "offline_shop.h"
	#include "offlineshop_manager.h"
#endif
#include "Controller.hpp"
#include "input.h"

#ifdef ENABLE_SWITCHBOT
#include "switchbot.h"
#endif
#ifdef __PREMIUM_PRIVATE_SHOP__
#include "private_shop_manager.h"
#include "private_shop.h"
#include "private_shop_util.h"
#endif
#ifdef __ENABLE_POLYMORPH_SYSTEM__
	#include "PolySystem.h"
#endif
#ifdef RANKING_SYSTEM
#include "server_ranking_manager.h"
#include "ranking_manager.h"
#endif
#ifdef ENABLE_IN_GAME_LOG_SYSTEM
#include "InGameLogManager.h"
#endif
#ifdef ENABLE_PVP_RANKING
#include "pvp_ranking.h"
#endif
#ifdef _ENABLE_BATTLEPASS_
#include "BattlePassManager.h"
#endif
#include <fmt/format.h>
#include "locale_item_manager.h"
#define ENABLE_CHAT_COLOR_SYSTEM
#define ENABLE_CHAT_LOGGING
#define ENABLE_CHAT_SPAMLIMIT
#define ENABLE_WHISPER_CHAT_SPAMLIMIT
#define ENABLE_CHECK_GHOSTMODE

#ifdef ENABLE_CHAT_LOGGING
static char	__escape_string[1024];
static char	__escape_string2[1024];
#endif

static int __deposit_limit()
{
	return (1000*10000);
}

void SendBlockChatInfo(LPCHARACTER ch, int sec)
{
	if (sec <= 0)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "[LS;652]");
		return;
	}

	int32_t hour = sec / 3600;
	sec -= hour * 3600;
	int32_t min = (sec / 60);
	sec -= min * 60;

	std::string buf;
	if (hour > 0 && min > 0)
		buf = fmt::format("[LS;1028;{};{};{}]", hour, min, sec);
	else if (hour > 0 && min == 0)
		buf = fmt::format("[LS;1029;{};{}]", hour, sec);
	else if (hour == 0 && min > 0)
		buf = fmt::format("[LS;1030;{};{}]", min, sec);
	else
		buf = fmt::format("[LS;1042;{}]", sec);

	ch->ChatPacket(CHAT_TYPE_INFO, buf.c_str());
}

EVENTINFO(spam_event_info)
{
	char host[MAX_HOST_LENGTH+1];

	spam_event_info()
	{
		::memset( host, 0, MAX_HOST_LENGTH+1 );
	}
};

typedef std::unordered_map<std::string, std::pair<unsigned int, LPEVENT> > spam_score_of_ip_t;
spam_score_of_ip_t spam_score_of_ip;

EVENTFUNC(block_chat_by_ip_event)
{
	spam_event_info* info = dynamic_cast<spam_event_info*>( event->info );

	if ( info == NULL )
	{
		sys_err( "block_chat_by_ip_event> <Factor> Null pointer" );
		return 0;
	}

	const char * host = info->host;

	spam_score_of_ip_t::iterator it = spam_score_of_ip.find(host);

	if (it != spam_score_of_ip.end())
	{
		it->second.first = 0;
		it->second.second = NULL;
	}

	return 0;
}

bool SpamBlockCheck(LPCHARACTER ch, const char* const buf, const size_t buflen)
{
	if (ch->GetLevel() < g_iSpamBlockMaxLevel)
	{
		spam_score_of_ip_t::iterator it = spam_score_of_ip.find(ch->GetDesc()->GetHostName());

		if (it == spam_score_of_ip.end())
		{
			spam_score_of_ip.emplace(ch->GetDesc()->GetHostName(), std::make_pair(0, (LPEVENT) NULL));
			it = spam_score_of_ip.find(ch->GetDesc()->GetHostName());
		}

		if (it->second.second)
		{
			SendBlockChatInfo(ch, event_time(it->second.second) / passes_per_sec);
			return true;
		}

		unsigned int score;
		const char * word = SpamManager::instance().GetSpamScore(buf, buflen, score);

		it->second.first += score;

		if (word)
			sys_log(0, "SPAM_SCORE: %s text: %s score: %u total: %u word: %s", ch->GetName(), buf, score, it->second.first, word);

		if (it->second.first >= g_uiSpamBlockScore)
		{
			spam_event_info* info = AllocEventInfo<spam_event_info>();
			strlcpy(info->host, ch->GetDesc()->GetHostName(), sizeof(info->host));

			it->second.second = event_create(block_chat_by_ip_event, info, PASSES_PER_SEC(g_uiSpamBlockDuration));
			sys_log(0, "SPAM_IP: %s for %u seconds", info->host, g_uiSpamBlockDuration);

			LogManager::instance().CharLog(ch, 0, "SPAM", word);

			SendBlockChatInfo(ch, event_time(it->second.second) / passes_per_sec);

			return true;
		}
	}

	return false;
}

enum
{
	TEXT_TAG_PLAIN,
	TEXT_TAG_TAG, // ||
	TEXT_TAG_COLOR, // |cffffffff
	TEXT_TAG_HYPERLINK_START, // |H
	TEXT_TAG_HYPERLINK_END, // |h ex) |Hitem:1234:1:1:1|h
	TEXT_TAG_RESTORE_COLOR,
};

int GetTextTag(const char * src, int maxLen, int & tagLen, std::string & extraInfo)
{
	tagLen = 1;

	if (maxLen < 2 || *src != '|')
		return TEXT_TAG_PLAIN;

	const char * cur = ++src;

	if (*cur == '|')
	{
		tagLen = 2;
		return TEXT_TAG_TAG;
	}
	else if (*cur == 'c') // color |cffffffffblahblah|r
	{
		tagLen = 2;
		return TEXT_TAG_COLOR;
	}
	else if (*cur == 'H')
	{
		tagLen = 2;
		return TEXT_TAG_HYPERLINK_START;
	}
	else if (*cur == 'h') // end of hyperlink
	{
		tagLen = 2;
		return TEXT_TAG_HYPERLINK_END;
	}

	return TEXT_TAG_PLAIN;
}

void GetTextTagInfo(const char * src, int src_len, int & hyperlinks, bool & colored)
{
	colored = false;
	hyperlinks = 0;

	int len;
	std::string extraInfo;

	for (int i = 0; i < src_len;)
	{
		int tag = GetTextTag(&src[i], src_len - i, len, extraInfo);

		if (tag == TEXT_TAG_HYPERLINK_START)
			++hyperlinks;

		if (tag == TEXT_TAG_COLOR)
			colored = true;

		i += len;
	}
}

int ProcessTextTag(LPCHARACTER ch, const char * c_pszText, size_t len)
{
	return 0;
	int hyperlinks;
	bool colored;

	GetTextTagInfo(c_pszText, len, hyperlinks, colored);

	if (colored == true && hyperlinks == 0)
		return 4;

#ifdef ENABLE_NEWSTUFF
	if (g_bDisablePrismNeed)
		return 0;
#endif
	int nPrismCount = ch->CountSpecifyItem(ITEM_PRISM);

	if (nPrismCount < hyperlinks)
		return 1;

	if (!ch->GetMyShop())
	{
		ch->RemoveSpecifyItem(ITEM_PRISM, hyperlinks);
		return 0;
	} else
	{
		int sellingNumber = ch->GetMyShop()->GetNumberByVnum(ITEM_PRISM);
		if(nPrismCount - sellingNumber < hyperlinks)
		{
			return 2;
		} else
		{
			ch->RemoveSpecifyItem(ITEM_PRISM, hyperlinks);
			return 0;
		}
	}

	return 4;
}

static bool CheckChatAllowed(LPCHARACTER ch, const char* buf);

int CInputMain::Whisper(LPCHARACTER ch, const char * data, size_t uiBytes)
{
	const TPacketCGWhisper* pinfo = reinterpret_cast<const TPacketCGWhisper*>(data);

	if (uiBytes < pinfo->wSize)
		return -1;

	int iExtraLen = pinfo->wSize - sizeof(TPacketCGWhisper);

	if (iExtraLen < 0)
	{
		sys_err("invalid packet length (len %d size %u buffer %u)", iExtraLen, pinfo->wSize, uiBytes);
		ch->GetDesc()->SetPhase(PHASE_CLOSE);
		return -1;
	}

#ifdef ENABLE_WHISPER_CHAT_SPAMLIMIT
	if (ch->IncreaseChatCounter() >= 10)
	{
		ch->GetDesc()->DelayedDisconnect(0);
		return (iExtraLen);
	}
#endif

	if (ch->FindAffect(AFFECT_BLOCK_CHAT))
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "[LS;652]");
		return (iExtraLen);
	}

	{
		char whisperBuf[CHAT_MAX_LEN + 1];
		strlcpy(whisperBuf, data + sizeof(TPacketCGWhisper), MIN(iExtraLen + 1, sizeof(whisperBuf)));
		if (!CheckChatAllowed(ch, whisperBuf))
			return iExtraLen;
	}

	LPCHARACTER pkChr = CHARACTER_MANAGER::instance().FindPC(pinfo->szNameTo);

	if (pkChr == ch)
		return (iExtraLen);

	LPDESC pkDesc = NULL;

	BYTE bOpponentEmpire = 0;

	if (test_server)
	{
		if (!pkChr)
			sys_log(0, "Whisper to %s(%s) from %s", "Null", pinfo->szNameTo, ch->GetName());
		else
			sys_log(0, "Whisper to %s(%s) from %s", pkChr->GetName(), pinfo->szNameTo, ch->GetName());
	}

	if (ch->IsBlockMode(BLOCK_WHISPER))
	{
		if (ch->GetDesc())
		{
			TPacketGCWhisper pack;
			pack.bHeader = HEADER_GC_WHISPER;
			pack.bType = WHISPER_TYPE_SENDER_BLOCKED;
			pack.wSize = sizeof(TPacketGCWhisper);
			strlcpy(pack.szNameFrom, pinfo->szNameTo, sizeof(pack.szNameFrom));
			ch->GetDesc()->Packet(&pack, sizeof(pack));
		}
		return iExtraLen;
	}

	if (!pkChr)
	{
		CCI * pkCCI = P2P_MANAGER::instance().Find(pinfo->szNameTo);

		if (pkCCI)
		{
			pkDesc = pkCCI->pkDesc;
			pkDesc->SetRelay(pinfo->szNameTo);
			bOpponentEmpire = pkCCI->bEmpire;

			if (test_server)
				sys_log(0, "Whisper to %s from %s (Channel %d Mapindex %d)", "Null", ch->GetName(), pkCCI->bChannel, pkCCI->lMapIndex);
		}
	}
	else
	{
		pkDesc = pkChr->GetDesc();
		bOpponentEmpire = pkChr->GetEmpire();
	}

	if (!pkDesc)
	{
		if (ch->GetDesc())
		{
#if defined(BL_OFFLINE_MESSAGE)
			const BYTE bDelay = 10;
			char msg[64];
			if (get_dword_time() - ch->GetLastOfflinePMTime() > bDelay * 1000)
			{
				char buf[CHAT_MAX_LEN + 1];
				strlcpy(buf, data + sizeof(TPacketCGWhisper), MIN(iExtraLen + 1, sizeof(buf)));
				const size_t buflen = strlen(buf);

				CBanwordManager::instance().ConvertString(buf, buflen);

				int processReturn = ProcessTextTag(ch, buf, buflen);
				if (0 != processReturn)
				{
					TItemTable* pTable = ITEM_MANAGER::instance().GetTable(ITEM_PRISM);

					if (pTable) {
						char buffer[128];
						int len;
						if (3 == processReturn)
							len = snprintf(buffer, sizeof(buffer), "%s", pTable->szLocaleName);
						else
							len = snprintf(buffer, sizeof(buffer), "%s", pTable->szLocaleName);

						if (len < 0 || len >= (int)sizeof(buffer))
							len = sizeof(buffer) - 1;

						++len;

						TPacketGCWhisper pack;

						pack.bHeader = HEADER_GC_WHISPER;
						pack.bType = WHISPER_TYPE_ERROR;
						pack.wSize = static_cast<WORD>(sizeof(TPacketGCWhisper) + len);
						strlcpy(pack.szNameFrom, pinfo->szNameTo, sizeof(pack.szNameFrom));

						ch->GetDesc()->BufferedPacket(&pack, sizeof(pack));
						ch->GetDesc()->Packet(buffer, len);

						sys_log(0, "WHISPER: not enough %s: char: %s", pTable->szLocaleName, ch->GetName());
					}
					return (iExtraLen);
				}

				if (buflen > 0) 
				{
					ch->SendOfflineMessage(pinfo->szNameTo, buf);
					snprintf(msg, sizeof(msg), "[LS;2370]");
				}
				else
					return (iExtraLen);
			}
			else
			{
				snprintf(msg, sizeof(msg), "[LS;2371;%d]", bDelay);
			}

			/*Info*/
			TPacketGCWhisper pack;
			int len = MIN(CHAT_MAX_LEN, strlen(msg) + 1);
			pack.bHeader = HEADER_GC_WHISPER;
			pack.wSize = static_cast<WORD>(sizeof(TPacketGCWhisper) + len);
			pack.bType = WHISPER_TYPE_SYSTEM;
			strlcpy(pack.szNameFrom, pinfo->szNameTo, sizeof(pack.szNameFrom));

			TEMP_BUFFER buf;
			buf.write(&pack, sizeof(TPacketGCWhisper));
			buf.write(msg, len);
			ch->GetDesc()->Packet(buf.read_peek(), buf.size());

#else
			TPacketGCWhisper pack;
			pack.bHeader = HEADER_GC_WHISPER;
			pack.bType = WHISPER_TYPE_NOT_EXIST;
			pack.wSize = sizeof(TPacketGCWhisper);
			strlcpy(pack.szNameFrom, pinfo->szNameTo, sizeof(pack.szNameFrom));
			ch->GetDesc()->Packet(&pack, sizeof(TPacketGCWhisper));
			sys_log(0, "WHISPER: no player");
#endif
		}
	}
	else
	{
		if (ch->IsBlockMode(BLOCK_WHISPER))
		{
			if (ch->GetDesc())
			{
				TPacketGCWhisper pack;
				pack.bHeader = HEADER_GC_WHISPER;
				pack.bType = WHISPER_TYPE_SENDER_BLOCKED;
				pack.wSize = sizeof(TPacketGCWhisper);
				strlcpy(pack.szNameFrom, pinfo->szNameTo, sizeof(pack.szNameFrom));
				ch->GetDesc()->Packet(&pack, sizeof(pack));
			}
		}
		else if (pkChr && pkChr->IsBlockMode(BLOCK_WHISPER))
		{
			if (ch->GetDesc())
			{
				TPacketGCWhisper pack;
				pack.bHeader = HEADER_GC_WHISPER;
				pack.bType = WHISPER_TYPE_TARGET_BLOCKED;
				pack.wSize = sizeof(TPacketGCWhisper);
				strlcpy(pack.szNameFrom, pinfo->szNameTo, sizeof(pack.szNameFrom));
				ch->GetDesc()->Packet(&pack, sizeof(pack));
			}
		}
#ifdef ENABLE_MESSENGER_BLOCK
		else if (pkDesc && MessengerManager::instance().IsBlocked(ch->GetName(), pinfo->szNameTo))
		{
			if (ch->GetDesc())
			{
				TPacketGCWhisper pack;

				char msg[CHAT_MAX_LEN + 1];
				snprintf(msg, sizeof(msg), "Unlock %s to continue.", pinfo->szNameTo);
				int len = MIN(CHAT_MAX_LEN, strlen(msg) + 1);

				pack.bHeader = HEADER_GC_WHISPER;
				pack.wSize = sizeof(TPacketGCWhisper) + len;
				pack.bType = WHISPER_TYPE_SYSTEM;
				strlcpy(pack.szNameFrom, pinfo->szNameTo, sizeof(pack.szNameFrom));

				TEMP_BUFFER buf;

				buf.write(&pack, sizeof(TPacketGCWhisper));
				buf.write(msg, len);
				ch->GetDesc()->Packet(buf.read_peek(), buf.size());
			}
		}
		else if (pkDesc && MessengerManager::instance().IsBlocked(pinfo->szNameTo, ch->GetName()))
		{
			if (ch->GetDesc())
			{
				TPacketGCWhisper pack;

				char msg[CHAT_MAX_LEN + 1];
				snprintf(msg, sizeof(msg), "%s has blocked you.", pinfo->szNameTo);
				int len = MIN(CHAT_MAX_LEN, strlen(msg) + 1);

				pack.bHeader = HEADER_GC_WHISPER;
				pack.wSize = sizeof(TPacketGCWhisper) + len;
				pack.bType = WHISPER_TYPE_SYSTEM;
				strlcpy(pack.szNameFrom, pinfo->szNameTo, sizeof(pack.szNameFrom));

				TEMP_BUFFER buf;

				buf.write(&pack, sizeof(TPacketGCWhisper));
				buf.write(msg, len);
				ch->GetDesc()->Packet(buf.read_peek(), buf.size());
			}
		}
#endif
		else
		{
			BYTE bType = WHISPER_TYPE_NORMAL;

			char buf[CHAT_MAX_LEN + 1];
			strlcpy(buf, data + sizeof(TPacketCGWhisper), MIN(iExtraLen + 1, sizeof(buf)));
			const size_t buflen = strlen(buf);

			if (true == SpamBlockCheck(ch, buf, buflen))
			{
				if (!pkChr)
				{
					CCI * pkCCI = P2P_MANAGER::instance().Find(pinfo->szNameTo);

					if (pkCCI)
					{
						pkDesc->SetRelay("");
					}
				}
				return iExtraLen;
			}

			CBanwordManager::instance().ConvertString(buf, buflen);

			if (g_bEmpireWhisper)
				if (!ch->IsEquipUniqueGroup(UNIQUE_GROUP_RING_OF_LANGUAGE))
					if (!(pkChr && pkChr->IsEquipUniqueGroup(UNIQUE_GROUP_RING_OF_LANGUAGE)))
						if (bOpponentEmpire != ch->GetEmpire() && ch->GetEmpire() && bOpponentEmpire
								&& ch->GetGMLevel() == GM_PLAYER && gm_get_level(pinfo->szNameTo) == GM_PLAYER)

						{
							if (!pkChr)
							{
								bType = ch->GetEmpire() << 4;
							}
							else
							{
								ConvertEmpireText(ch->GetEmpire(), buf, buflen, 10 + 2 * pkChr->GetSkillPower(SKILL_LANGUAGE1 + ch->GetEmpire() - 1));
							}
						}

			int processReturn = ProcessTextTag(ch, buf, buflen);
			if (0!=processReturn)
			{
				if (ch->GetDesc())
				{
					TItemTable * pTable = ITEM_MANAGER::instance().GetTable(ITEM_PRISM);

					if (pTable)
					{
#ifdef ENABLE_MULTI_LANGUAGE_SYSTEM
						const std::string& localName = CLocaleItemManager::instance().Find(ITEM_PRISM, ch->GetLanguage());
						const char* prismName = !localName.empty() ? localName.c_str() : pTable->szLocaleName;
#else
						const char* prismName = pTable->szLocaleName;
#endif
						char buf[128];
						int len;
						if (3==processReturn)
							len = snprintf(buf, sizeof(buf), "[LS;432]", prismName);
						else
							len = snprintf(buf, sizeof(buf), "[LS;1027;%s]", prismName);

						if (len < 0 || len >= (int) sizeof(buf))
							len = sizeof(buf) - 1;

						++len;

						TPacketGCWhisper pack;

						pack.bHeader = HEADER_GC_WHISPER;
						pack.bType = WHISPER_TYPE_ERROR;
						pack.wSize = sizeof(TPacketGCWhisper) + len;
						strlcpy(pack.szNameFrom, pinfo->szNameTo, sizeof(pack.szNameFrom));

						ch->GetDesc()->BufferedPacket(&pack, sizeof(pack));
						ch->GetDesc()->Packet(buf, len);

						sys_log(0, "WHISPER: not enough %s: char: %s", pTable->szLocaleName, ch->GetName());
					}
				}

				pkDesc->SetRelay("");
				return (iExtraLen);
			}

			if (ch->IsGM())
				bType = (bType & 0xF0) | WHISPER_TYPE_GM;

			if (buflen > 0)
			{
				TPacketGCWhisper pack;

				pack.bHeader = HEADER_GC_WHISPER;
				pack.wSize = sizeof(TPacketGCWhisper) + buflen;
				pack.bType = bType;
				strlcpy(pack.szNameFrom, ch->GetName(), sizeof(pack.szNameFrom));
#if defined(__BL_CLIENT_LOCALE_STRING__)
				pack.bCanFormat = false;
#endif
				TEMP_BUFFER tmpbuf;

				tmpbuf.write(&pack, sizeof(pack));
				tmpbuf.write(buf, buflen);

				pkDesc->Packet(tmpbuf.read_peek(), tmpbuf.size());

				// @warme006
				// sys_log(0, "WHISPER: %s -> %s : %s", ch->GetName(), pinfo->szNameTo, buf);
#ifdef ENABLE_CHAT_LOGGING
				if (ch->IsGM())
				{
					LogManager::instance().EscapeString(__escape_string, sizeof(__escape_string), buf, buflen);
					LogManager::instance().EscapeString(__escape_string2, sizeof(__escape_string2), pinfo->szNameTo, sizeof(pack.szNameFrom));
					LogManager::instance().ChatLog(ch->GetMapIndex(), ch->GetPlayerID(), ch->GetName(), 0, __escape_string2, "WHISPER", __escape_string, ch->GetDesc() ? ch->GetDesc()->GetHostName() : "");
				}
#endif
			}
		}
	}
	if(pkDesc)
		pkDesc->SetRelay("");

	return (iExtraLen);
}

struct RawPacketToCharacterFunc
{
	const void * m_buf;
	int	m_buf_len;

	RawPacketToCharacterFunc(const void * buf, int buf_len) : m_buf(buf), m_buf_len(buf_len)
	{
	}

	void operator () (LPCHARACTER c)
	{
		if (!c->GetDesc())
			return;

		c->GetDesc()->Packet(m_buf, m_buf_len);
	}
};

struct FEmpireChatPacket
{
	packet_chat& p;
	const char* orig_msg;
	int orig_len;
	char converted_msg[CHAT_MAX_LEN+1];

	BYTE bEmpire;
	int iMapIndex;
	int namelen;

	FEmpireChatPacket(packet_chat& p, const char* chat_msg, int len, BYTE bEmpire, int iMapIndex, int iNameLen)
		: p(p), orig_msg(chat_msg), orig_len(len), bEmpire(bEmpire), iMapIndex(iMapIndex), namelen(iNameLen)
	{
		memset( converted_msg, 0, sizeof(converted_msg) );
	}

	void operator () (LPDESC d)
	{
		if (!d->GetCharacter())
			return;

		if (d->GetCharacter()->GetMapIndex() != iMapIndex)
			return;

		d->BufferedPacket(&p, sizeof(packet_chat));

		if (d->GetEmpire() == bEmpire ||
			bEmpire == 0 ||
			d->GetCharacter()->GetGMLevel() > GM_PLAYER ||
			d->GetCharacter()->IsEquipUniqueGroup(UNIQUE_GROUP_RING_OF_LANGUAGE))
		{
			d->Packet(orig_msg, orig_len);
		}
		else
		{
			size_t len = strlcpy(converted_msg, orig_msg, sizeof(converted_msg));

			if (len >= sizeof(converted_msg))
				len = sizeof(converted_msg) - 1;

			ConvertEmpireText(bEmpire, converted_msg + namelen, len - namelen, 10 + 2 * d->GetCharacter()->GetSkillPower(SKILL_LANGUAGE1 + bEmpire - 1));
			d->Packet(converted_msg, orig_len);
		}
	}
};

struct FYmirChatPacket
{
	packet_chat& packet;
	const char* m_szChat;
	size_t m_lenChat;
	const char* m_szName;

	int m_iMapIndex;
	BYTE m_bEmpire;
	bool m_ring;

	char m_orig_msg[CHAT_MAX_LEN+1];
	int m_len_orig_msg;
	char m_conv_msg[CHAT_MAX_LEN+1];
	int m_len_conv_msg;

	FYmirChatPacket(packet_chat& p, const char* chat, size_t len_chat, const char* name, size_t len_name, int iMapIndex, BYTE empire, bool ring)
		: packet(p),
		m_szChat(chat), m_lenChat(len_chat),
		m_szName(name),
		m_iMapIndex(iMapIndex), m_bEmpire(empire),
		m_ring(ring)
	{
		m_len_orig_msg = snprintf(m_orig_msg, sizeof(m_orig_msg), "%s : %s", m_szName, m_szChat) + 1;

		if (m_len_orig_msg < 0 || m_len_orig_msg >= (int) sizeof(m_orig_msg))
			m_len_orig_msg = sizeof(m_orig_msg) - 1;

		m_len_conv_msg = snprintf(m_conv_msg, sizeof(m_conv_msg), "??? : %s", m_szChat) + 1;

		if (m_len_conv_msg < 0 || m_len_conv_msg >= (int) sizeof(m_conv_msg))
			m_len_conv_msg = sizeof(m_conv_msg) - 1;

		ConvertEmpireText(m_bEmpire, m_conv_msg + 6, m_len_conv_msg - 6, 10);
	}

	void operator() (LPDESC d)
	{
		if (!d->GetCharacter())
			return;

		if (d->GetCharacter()->GetMapIndex() != m_iMapIndex)
			return;

		if (m_ring ||
			d->GetEmpire() == m_bEmpire ||
			d->GetCharacter()->GetGMLevel() > GM_PLAYER ||
			d->GetCharacter()->IsEquipUniqueGroup(UNIQUE_GROUP_RING_OF_LANGUAGE))
		{
			packet.size = m_len_orig_msg + sizeof(TPacketGCChat);

			d->BufferedPacket(&packet, sizeof(packet_chat));
			d->Packet(m_orig_msg, m_len_orig_msg);
		}
		else
		{
			packet.size = m_len_conv_msg + sizeof(TPacketGCChat);

			d->BufferedPacket(&packet, sizeof(packet_chat));
			d->Packet(m_conv_msg, m_len_conv_msg);
		}
	}
};

// -----------------------------------------------------------------------
// RMT / yang-seller protection helpers
// -----------------------------------------------------------------------
#define CHAT_MIN_PLAYTIME_MINUTES 30

static bool IsRMTMessage(const char* msg)
{
	// Normalise: lowercase alphanumerics + dot only (strips accents, apostrophes, spaces …)
	char norm[CHAT_MAX_LEN + 1];
	int j = 0;
	for (int i = 0; msg[i] && j < (int)sizeof(norm) - 1; ++i)
	{
		unsigned char c = (unsigned char)msg[i];
		if (c >= 'A' && c <= 'Z')      c += 32;
		if ((c >= 'a' && c <= 'z') || (c >= '0' && c <= '9') || c == '.')
			norm[j++] = (char)c;
	}
	norm[j] = '\0';

	static const char* s_blocked[] = {
		"yangmonster",
		"yangseller",
		"yangsell",
		"buyyang",
		"sellyang",
		"cheapyang",
		"yangshop",
		"yangstore",
		NULL
	};

	for (int i = 0; s_blocked[i]; ++i)
		if (strstr(norm, s_blocked[i]))
			return true;

	return false;
}

static bool CheckChatAllowed(LPCHARACTER ch, const char* buf)
{
	if (ch->IsGM())
		return true;

	// --- Playtime gate ---
	DWORD totalMinutes = ch->GetRealPoint(POINT_PLAYTIME);
	if (totalMinutes < CHAT_MIN_PLAYTIME_MINUTES)
	{
		int remaining = CHAT_MIN_PLAYTIME_MINUTES - (int)totalMinutes;
		ch->ChatPacket(CHAT_TYPE_INFO, "You need at least %d more minute(s) of playtime to use chat.", remaining);
		return false;
	}

	// --- RMT keyword gate ---
	if (IsRMTMessage(buf))
	{
		sys_log(0, "RMT_BLOCK [%s] (playtime=%u min): %s", ch->GetName(), totalMinutes, buf);
		ch->ChatPacket(CHAT_TYPE_INFO, "Your message was blocked by the anti-spam filter.");
		return false;
	}

	return true;
}
// -----------------------------------------------------------------------

int CInputMain::Chat(LPCHARACTER ch, const char * data, size_t uiBytes)
{
	const TPacketCGChat* pinfo = reinterpret_cast<const TPacketCGChat*>(data);

	if (uiBytes < pinfo->size)
		return -1;

	const int iExtraLen = pinfo->size - sizeof(TPacketCGChat);

	if (iExtraLen < 0)
	{
		sys_err("invalid packet length (len %d size %u buffer %u)", iExtraLen, pinfo->size, uiBytes);
		ch->GetDesc()->SetPhase(PHASE_CLOSE);
		return -1;
	}

	char buf[CHAT_MAX_LEN - (CHARACTER_NAME_MAX_LEN + 3) + 1];
	strlcpy(buf, data + sizeof(TPacketCGChat), MIN(iExtraLen + 1, sizeof(buf)));
	const size_t buflen = strlen(buf);

	if (buflen > 1 && *buf == '/')
	{
		interpret_command(ch, buf + 1, buflen - 1);
		return iExtraLen;
	}
#ifdef ENABLE_CHAT_SPAMLIMIT
	if (ch->IncreaseChatCounter() >= 4)
	{
		if (ch->GetChatCounter() == 10)
			ch->GetDesc()->DelayedDisconnect(0);
		return iExtraLen;
	}
#else
	if (ch->IncreaseChatCounter() >= 10)
	{
		if (ch->GetChatCounter() == 10)
		{
			sys_log(0, "CHAT_HACK: %s", ch->GetName());
			ch->GetDesc()->DelayedDisconnect(5);
		}

		return iExtraLen;
	}
#endif

	const CAffect* pAffect = ch->FindAffect(AFFECT_BLOCK_CHAT);

	if (pAffect != NULL)
	{
		SendBlockChatInfo(ch, pAffect->lDuration);
		return iExtraLen;
	}

	if (true == SpamBlockCheck(ch, buf, buflen))
	{
		return iExtraLen;
	}

	if (!CheckChatAllowed(ch, buf))
		return iExtraLen;

	// @fixme133 begin
	CBanwordManager::instance().ConvertString(buf, buflen);

	int processReturn = ProcessTextTag(ch, buf, buflen);
	if (0!=processReturn)
	{
		const TItemTable* pTable = ITEM_MANAGER::instance().GetTable(ITEM_PRISM);

		if (NULL != pTable)
		{
#ifdef ENABLE_MULTI_LANGUAGE_SYSTEM
			const std::string& localName = CLocaleItemManager::instance().Find(ITEM_PRISM, ch->GetLanguage());
			const char* prismName = !localName.empty() ? localName.c_str() : pTable->szLocaleName;
#else
			const char* prismName = pTable->szLocaleName;
#endif
			if (3==processReturn)
				ch->ChatPacket(CHAT_TYPE_INFO, "[LS;432]", prismName);
			else
				ch->ChatPacket(CHAT_TYPE_INFO, "[LS;1027;%s]", prismName);
		}

		return iExtraLen;
	}
	// @fixme133 end

	char chatbuf[CHAT_MAX_LEN + 1];
#ifdef ENABLE_CHAT_COLOR_SYSTEM
	static const char* colorbuf[] = {"|cFFffa200|H|h[GM]|h|r", "|cFFFF6060[Shinsoo]|r", "|cFFFFFF00[Chunjo]|r", "|cFF0080FF[Jinno]|r"};
	int len = snprintf(chatbuf, sizeof(chatbuf), "%s %s : %s", (ch->IsGM()?colorbuf[0]:colorbuf[MINMAX(0, ch->GetEmpire(), 3)]), ch->GetName(), buf);
#else
	int len = snprintf(chatbuf, sizeof(chatbuf), "%s : %s", ch->GetName(), buf);
#endif

	if (CHAT_TYPE_SHOUT == pinfo->type)
	{
		LogManager::instance().ShoutLog(g_bChannel, ch->GetEmpire(), chatbuf);
	}

	if (len < 0 || len >= (int) sizeof(chatbuf))
		len = sizeof(chatbuf) - 1;

	if (pinfo->type == CHAT_TYPE_SHOUT)
	{
		if (ch->GetLevel() < g_iShoutLimitLevel)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "[LS;654;%d]", g_iShoutLimitLevel);
			return (iExtraLen);
		}

		if (thecore_heart->pulse - (int) ch->GetLastShoutPulse() < passes_per_sec * 15)
			return (iExtraLen);

		ch->SetLastShoutPulse(thecore_heart->pulse);
#ifdef _ENABLE_BATTLEPASS_
		BattlePassManager::Instance().Notify(MISSION_TYPE_MESSAGES, ch, 0, 1);
#endif

		char chatbuf_global[CHAT_MAX_LEN + 1];
		const char * ColoredEmpireNames[4] = {" ", "|cFFFF6060[Shinsoo]|r", "|cFFFFFF00[Chunjo]|r", "|cFF0080FF[Jinno]|r",};
		int len = snprintf(chatbuf_global, sizeof(chatbuf_global), "%s [Lv. %d] %s : %s |cffffc700|Hszept:%s|h[PM]|h|r", ColoredEmpireNames[ch->GetEmpire()], ch->GetLevel(), ch->GetName(), buf, ch->GetName());

		TPacketGGShout p;

		p.bHeader = HEADER_GG_SHOUT;
		p.bEmpire = ch->GetEmpire();
		strlcpy(p.szText, chatbuf_global, sizeof(p.szText));

		P2P_MANAGER::instance().Send(&p, sizeof(TPacketGGShout));

		SendShout(chatbuf_global, ch->GetEmpire());

		return (iExtraLen);
	}

	TPacketGCChat pack_chat;

	pack_chat.header = HEADER_GC_CHAT;
	pack_chat.size = sizeof(TPacketGCChat) + len;
	pack_chat.type = pinfo->type;
	pack_chat.id = ch->GetVID();
#if defined(__BL_CLIENT_LOCALE_STRING__)
	pack_chat.bCanFormat = false;
#endif
	switch (pinfo->type)
	{
		case CHAT_TYPE_TALKING:
			{
				const DESC_MANAGER::DESC_SET & c_ref_set = DESC_MANAGER::instance().GetClientSet();

				if (false)
				{
					std::for_each(c_ref_set.begin(), c_ref_set.end(),
							FYmirChatPacket(pack_chat,
								buf,
								strlen(buf),
								ch->GetName(),
								strlen(ch->GetName()),
								ch->GetMapIndex(),
								ch->GetEmpire(),
								ch->IsEquipUniqueGroup(UNIQUE_GROUP_RING_OF_LANGUAGE)));
				}
				else
				{
					std::for_each(c_ref_set.begin(), c_ref_set.end(),
							FEmpireChatPacket(pack_chat,
								chatbuf,
								len,
								(ch->GetGMLevel() > GM_PLAYER ||
								 ch->IsEquipUniqueGroup(UNIQUE_GROUP_RING_OF_LANGUAGE)) ? 0 : ch->GetEmpire(),
								ch->GetMapIndex(), strlen(ch->GetName())));
#ifdef ENABLE_CHAT_LOGGING
					if (ch->IsGM())
					{
						LogManager::instance().EscapeString(__escape_string, sizeof(__escape_string), chatbuf, len);
						LogManager::instance().ChatLog(ch->GetMapIndex(), ch->GetPlayerID(), ch->GetName(), 0, "", "NORMAL", __escape_string, ch->GetDesc() ? ch->GetDesc()->GetHostName() : "");
					}
#endif
				}
			}
			break;

		case CHAT_TYPE_PARTY:
			{
				if (!ch->GetParty())
					ch->ChatPacket(CHAT_TYPE_INFO, "[LS;655]");
				else
				{
					TEMP_BUFFER tbuf;

					tbuf.write(&pack_chat, sizeof(pack_chat));
					tbuf.write(chatbuf, len);

					RawPacketToCharacterFunc f(tbuf.read_peek(), tbuf.size());
					ch->GetParty()->ForEachOnlineMember(f);
#ifdef ENABLE_CHAT_LOGGING
					if (ch->IsGM())
					{
						LogManager::instance().EscapeString(__escape_string, sizeof(__escape_string), chatbuf, len);
						LogManager::instance().ChatLog(ch->GetMapIndex(), ch->GetPlayerID(), ch->GetName(), ch->GetParty()->GetLeaderPID(), "", "PARTY", __escape_string, ch->GetDesc() ? ch->GetDesc()->GetHostName() : "");
					}
#endif
				}
			}
			break;

		case CHAT_TYPE_GUILD:
			{
				if (!ch->GetGuild())
					ch->ChatPacket(CHAT_TYPE_INFO, "[LS;656]");
				else
				{
					ch->GetGuild()->Chat(chatbuf);
#ifdef ENABLE_CHAT_LOGGING
					if (ch->IsGM())
					{
						LogManager::instance().EscapeString(__escape_string, sizeof(__escape_string), chatbuf, len);
						LogManager::instance().ChatLog(ch->GetMapIndex(), ch->GetPlayerID(), ch->GetName(), ch->GetGuild()->GetID(), ch->GetGuild()->GetName(), "GUILD", __escape_string, ch->GetDesc() ? ch->GetDesc()->GetHostName() : "");
					}
#endif
				}
			}
			break;

		default:
			sys_err("Unknown chat type %d", pinfo->type);
			break;
	}

	return (iExtraLen);
}

void CInputMain::ItemUse(LPCHARACTER ch, const char * data)
{
	ch->UseItem(((struct command_item_use *) data)->Cell);
}

void CInputMain::ItemToItem(LPCHARACTER ch, const char * pcData)
{
	TPacketCGItemUseToItem * p = (TPacketCGItemUseToItem *) pcData;
	if (ch)
		ch->UseItem(p->Cell, p->TargetCell);
}

void CInputMain::ItemDrop(LPCHARACTER ch, const char * data)
{
	struct command_item_drop * pinfo = (struct command_item_drop *) data;

	if (!ch)
		return;

	if (pinfo->gold > 0)
		ch->DropGold(pinfo->gold);
#ifdef ENABLE_CHEQUE_SYSTEM
	else if (pinfo->cheque > 0)
		ch->DropCheque(pinfo->cheque);
#endif
	else
		ch->DropItem(pinfo->Cell);
}

void CInputMain::ItemDrop2(LPCHARACTER ch, const char * data)
{
	TPacketCGItemDrop2 * pinfo = (TPacketCGItemDrop2 *) data;

	if (!ch)
		return;

	if (pinfo->gold > 0)
		ch->DropGold(pinfo->gold);
	else
		ch->DropItem(pinfo->Cell, pinfo->count);
}

void CInputMain::ItemMove(LPCHARACTER ch, const char * data)
{
	struct command_item_move * pinfo = (struct command_item_move *) data;

	if (ch)
		ch->MoveItem(pinfo->Cell, pinfo->CellTo, pinfo->count);
}

void CInputMain::ItemPickup(LPCHARACTER ch, const char * data)
{
	struct command_item_pickup * pinfo = (struct command_item_pickup*) data;
	if (ch)
		ch->PickupItem(pinfo->vid);
}

void CInputMain::QuickslotAdd(LPCHARACTER ch, const char * data)
{
	struct command_quickslot_add * pinfo = (struct command_quickslot_add *) data;
	ch->SetQuickslot(pinfo->pos, pinfo->slot);
}

void CInputMain::QuickslotDelete(LPCHARACTER ch, const char * data)
{
	struct command_quickslot_del * pinfo = (struct command_quickslot_del *) data;
	ch->DelQuickslot(pinfo->pos);
}

void CInputMain::QuickslotSwap(LPCHARACTER ch, const char * data)
{
	struct command_quickslot_swap * pinfo = (struct command_quickslot_swap *) data;
	ch->SwapQuickslot(pinfo->pos, pinfo->change_pos);
}

int CInputMain::Messenger(LPCHARACTER ch, const char* c_pData, size_t uiBytes)
{
	TPacketCGMessenger* p = (TPacketCGMessenger*) c_pData;

	if (uiBytes < sizeof(TPacketCGMessenger))
		return -1;

	c_pData += sizeof(TPacketCGMessenger);
	uiBytes -= sizeof(TPacketCGMessenger);

	switch (p->subheader)
	{
		case MESSENGER_SUBHEADER_CG_ADD_BY_VID:
			{
				if (uiBytes < sizeof(TPacketCGMessengerAddByVID))
					return -1;

				TPacketCGMessengerAddByVID * p2 = (TPacketCGMessengerAddByVID *) c_pData;
				LPCHARACTER ch_companion = CHARACTER_MANAGER::instance().Find(p2->vid);

				if (!ch_companion)
					return sizeof(TPacketCGMessengerAddByVID);

				if (ch->IsObserverMode())
					return sizeof(TPacketCGMessengerAddByVID);

				if (ch_companion->IsBlockMode(BLOCK_MESSENGER_INVITE))
				{
					ch->ChatPacket(CHAT_TYPE_INFO, "[LS;657]");
					return sizeof(TPacketCGMessengerAddByVID);
				}

				LPDESC d = ch_companion->GetDesc();

				if (!d)
					return sizeof(TPacketCGMessengerAddByVID);

				if (ch->GetGMLevel() == GM_PLAYER && ch_companion->GetGMLevel() != GM_PLAYER)
				{
					ch->ChatPacket(CHAT_TYPE_INFO, "[LS;659]");
					return sizeof(TPacketCGMessengerAddByVID);
				}
#ifdef ENABLE_MESSENGER_BLOCK
				if (MessengerManager::instance().IsBlocked(ch->GetName(), ch_companion->GetName()))
				{
					ch->ChatPacket(CHAT_TYPE_INFO, "You need to unblock %s do to that.", ch_companion->GetName());
					return sizeof(TPacketCGMessengerAddByVID);
				}
				if (MessengerManager::instance().IsBlocked(ch_companion->GetName(), ch->GetName()))
				{
					ch->ChatPacket(CHAT_TYPE_INFO, "%s has blocked you.", ch_companion->GetName());
					return sizeof(TPacketCGMessengerAddByVID);
				}
#endif

				if (ch->GetDesc() == d)
					return sizeof(TPacketCGMessengerAddByVID);

				MessengerManager::instance().RequestToAdd(ch, ch_companion);
				//MessengerManager::instance().AddToList(ch->GetName(), ch_companion->GetName());
			}
			return sizeof(TPacketCGMessengerAddByVID);

		case MESSENGER_SUBHEADER_CG_ADD_BY_NAME:
			{
				if (uiBytes < CHARACTER_NAME_MAX_LEN)
					return -1;

				char name[CHARACTER_NAME_MAX_LEN + 1];
				strlcpy(name, c_pData, sizeof(name));

				if (ch->GetGMLevel() == GM_PLAYER && gm_get_level(name) != GM_PLAYER)
				{
					ch->ChatPacket(CHAT_TYPE_INFO, "[LS;659]");
					return CHARACTER_NAME_MAX_LEN;
				}

				LPCHARACTER tch = CHARACTER_MANAGER::instance().FindPC(name);

				if (!tch)
					ch->ChatPacket(CHAT_TYPE_INFO, "[LS;660;%s]", name);
				else
				{
					if (tch == ch)
						return CHARACTER_NAME_MAX_LEN;

#ifdef ENABLE_MESSENGER_BLOCK
					if (MessengerManager::instance().IsBlocked(ch->GetName(), tch->GetName()))
					{
						ch->ChatPacket(CHAT_TYPE_INFO, "You need to unblock %s do to that.", tch->GetName());
						return CHARACTER_NAME_MAX_LEN;
					}
					if (MessengerManager::instance().IsBlocked(tch->GetName(), ch->GetName()))
					{
						ch->ChatPacket(CHAT_TYPE_INFO, "%s has blocked you.", tch->GetName());
						return CHARACTER_NAME_MAX_LEN;
					}
#endif

					if (tch->IsBlockMode(BLOCK_MESSENGER_INVITE) == true)
					{
						ch->ChatPacket(CHAT_TYPE_INFO, "[LS;657]");
					}
					else
					{
						MessengerManager::instance().RequestToAdd(ch, tch);
						//MessengerManager::instance().AddToList(ch->GetName(), tch->GetName());
					}
				}
			}
			return CHARACTER_NAME_MAX_LEN;

		case MESSENGER_SUBHEADER_CG_REMOVE:
			{
				if (uiBytes < CHARACTER_NAME_MAX_LEN)
					return -1;

				char char_name[CHARACTER_NAME_MAX_LEN + 1];
				strlcpy(char_name, c_pData, sizeof(char_name));
				MessengerManager::instance().RemoveFromList(ch->GetName(), char_name);
				MessengerManager::instance().RemoveFromList(char_name, ch->GetName()); // @fixme183
			}
			return CHARACTER_NAME_MAX_LEN;
			
#ifdef ENABLE_MESSENGER_BLOCK
		case MESSENGER_SUBHEADER_CG_BLOCK_ADD_BY_VID:
		{
			if (uiBytes < sizeof(TPacketCGMessengerAddBlockByVID))
				return -1;

			TPacketCGMessengerAddBlockByVID* p2 = (TPacketCGMessengerAddBlockByVID*)c_pData;
			LPCHARACTER ch_companion = CHARACTER_MANAGER::instance().Find(p2->vid);

			if (!ch_companion)
				return sizeof(TPacketCGMessengerAddBlockByVID);

			if (ch->IsObserverMode())
				return sizeof(TPacketCGMessengerAddBlockByVID);

			if (ch_companion == ch)
				return sizeof(TPacketCGMessengerAddBlockByVID);

			if(!ch_companion->IsPC())
				return sizeof(TPacketCGMessengerAddBlockByVID);

			LPDESC d = ch_companion->GetDesc();

			if (!d)
				return sizeof(TPacketCGMessengerAddByVID);

			LPCHARACTER pkPartner = ch->GetMarryPartner();
			if (pkPartner)
			{
				if (strcmp(ch_companion->GetName(), pkPartner->GetName()) == 0)
				{
					ch->ChatPacket(CHAT_TYPE_INFO, "You can't block your spouse.");
					return sizeof(TPacketCGMessengerAddBlockByVID);
				}
			}

			if (MessengerManager::instance().IsInFriendList(ch->GetName(), ch_companion->GetName()))
			{
				ch->ChatPacket(CHAT_TYPE_INFO, "Remove %s from your friends list to continue.", ch_companion->GetName());
				return sizeof(TPacketCGMessengerAddBlockByVID);
			}

			if (MessengerManager::instance().IsBlocked(ch->GetName(), ch_companion->GetName()))
			{
				ch->ChatPacket(CHAT_TYPE_INFO, "%s is already blocked.", ch_companion->GetName());
				return sizeof(TPacketCGMessengerAddBlockByVID);
			}

			if (ch->GetGMLevel() == GM_PLAYER && ch_companion->GetGMLevel() != GM_PLAYER && !test_server)
			{
				ch->ChatPacket(CHAT_TYPE_INFO, "You can't block this player.");
				return sizeof(TPacketCGMessengerAddByVID);
			}

			if (thecore_pulse() > ch->GetBlockAntiFloodPulse() + PASSES_PER_SEC(1))
			{
				ch->SetBlockAntiFloodCount(0);
				ch->SetBlockAntiFloodPulse(thecore_pulse());
			}

			if (ch->IncreaseBlockAntiFloodCount() >= 5)
			{
				// ch->GetDesc()->SetDisconnectReason("MESSENGER_BLOCK_SPAM");
				ch->GetDesc()->SetPhase(PHASE_CLOSE);
				return sizeof(TPacketCGMessengerAddByVID);
			}

			MessengerManager::instance().AddToBlockList(ch->GetName(), ch_companion->GetName());
		}
		return sizeof(TPacketCGMessengerAddBlockByVID);

		case MESSENGER_SUBHEADER_CG_BLOCK_ADD_BY_NAME:
		{
			if (uiBytes < CHARACTER_NAME_MAX_LEN)
				return -1;

			char name[CHARACTER_NAME_MAX_LEN + 1];
			strlcpy(name, c_pData, sizeof(name));

			if (gm_get_level(name) != GM_PLAYER && !test_server)
			{
				ch->ChatPacket(CHAT_TYPE_INFO, "You can't block this player.");
				return CHARACTER_NAME_MAX_LEN;
			}

			LPCHARACTER pkChr = CHARACTER_MANAGER::instance().FindPC(name);
			if (pkChr == ch)
				return CHARACTER_NAME_MAX_LEN;
						
			LPDESC pkDesc = nullptr;
			if (!pkChr)
			{
				CCI* pkCCI = P2P_MANAGER::instance().Find(name);
				if (pkCCI)
					pkDesc = pkCCI->pkDesc;
			}
			else
				pkDesc = pkChr->GetDesc();

			if (!pkDesc)
			{
				ch->ChatPacket(CHAT_TYPE_INFO, "%s is not online.", name);
				return CHARACTER_NAME_MAX_LEN;
			}

			LPCHARACTER pkPartner = ch->GetMarryPartner();
			if (pkPartner)
			{
				if (strcmp(pkPartner->GetName(), name) == 0)
				{
					ch->ChatPacket(CHAT_TYPE_INFO, "You can't block your spouse.");
					return CHARACTER_NAME_MAX_LEN;
				}
			}

			if (MessengerManager::instance().IsInFriendList(ch->GetName(), name))
			{
				ch->ChatPacket(CHAT_TYPE_INFO, "Remove %s from your friends list to continue.", name);
				return CHARACTER_NAME_MAX_LEN;
			}

			if (MessengerManager::instance().IsBlocked(ch->GetName(), name))
			{
				ch->ChatPacket(CHAT_TYPE_INFO, "%s is already blocked.", name);
				return CHARACTER_NAME_MAX_LEN;
			}

			if (thecore_pulse() > ch->GetBlockAntiFloodPulse() + PASSES_PER_SEC(1))
			{
				ch->SetBlockAntiFloodCount(0);
				ch->SetBlockAntiFloodPulse(thecore_pulse());
			}

			if (ch->IncreaseBlockAntiFloodCount() >= 5)
			{
				// ch->GetDesc()->SetDisconnectReason("MESSENGER_BLOCK_SPAM");
				ch->GetDesc()->SetPhase(PHASE_CLOSE);
				return CHARACTER_NAME_MAX_LEN;
			}

			MessengerManager::instance().AddToBlockList(ch->GetName(), name);
		}
		return CHARACTER_NAME_MAX_LEN;

		case MESSENGER_SUBHEADER_CG_BLOCK_REMOVE:
		{
			if (uiBytes < CHARACTER_NAME_MAX_LEN)
				return -1;

			char char_name[CHARACTER_NAME_MAX_LEN + 1];
			strlcpy(char_name, c_pData, sizeof(char_name));

			if (!MessengerManager::instance().IsBlocked(ch->GetName(), char_name))
				return CHARACTER_NAME_MAX_LEN;

			if (thecore_pulse() > ch->GetBlockAntiFloodPulse() + PASSES_PER_SEC(1))
			{
				ch->SetBlockAntiFloodCount(0);
				ch->SetBlockAntiFloodPulse(thecore_pulse());
			}

			if (ch->IncreaseBlockAntiFloodCount() >= 5)
			{
				// ch->GetDesc()->SetDisconnectReason("MESSENGER_BLOCK_SPAM");
				ch->GetDesc()->SetPhase(PHASE_CLOSE);
				return CHARACTER_NAME_MAX_LEN;
			}

			MessengerManager::instance().RemoveFromBlockList(ch->GetName(), char_name);
		}
		return CHARACTER_NAME_MAX_LEN;

		case MESSENGER_SUBHEADER_CG_BLOCK_REMOVE_BY_VID:
		{
			if (uiBytes < sizeof(TPacketCGMessengerRemoveBlockByVID))
				return -1;

			TPacketCGMessengerRemoveBlockByVID* p2 = (TPacketCGMessengerRemoveBlockByVID*)c_pData;
			LPCHARACTER ch_companion = CHARACTER_MANAGER::instance().Find(p2->vid);
			if (!ch_companion)
				return sizeof(TPacketCGMessengerRemoveBlockByVID);

			if (ch->IsObserverMode())
				return sizeof(TPacketCGMessengerRemoveBlockByVID);

			if (ch_companion == ch || !ch_companion->IsPC() || !ch_companion->GetDesc())
				return sizeof(TPacketCGMessengerRemoveBlockByVID);

			if (!MessengerManager::instance().IsBlocked(ch->GetName(), ch_companion->GetName()))
				return sizeof(TPacketCGMessengerRemoveBlockByVID);

			if (thecore_pulse() > ch->GetBlockAntiFloodPulse() + PASSES_PER_SEC(1))
			{
				ch->SetBlockAntiFloodCount(0);
				ch->SetBlockAntiFloodPulse(thecore_pulse());
			}

			if (ch->IncreaseBlockAntiFloodCount() >= 5)
			{
				// ch->GetDesc()->SetDisconnectReason("MESSENGER_BLOCK_SPAM");
				ch->GetDesc()->SetPhase(PHASE_CLOSE);
				return sizeof(TPacketCGMessengerRemoveBlockByVID);
			}

			MessengerManager::instance().RemoveFromBlockList(ch->GetName(), ch_companion->GetName());
		}
		return sizeof(TPacketCGMessengerRemoveBlockByVID);
#endif

		default:
			sys_err("CInputMain::Messenger : Unknown subheader %d : %s", p->subheader, ch->GetName());
			break;
	}

	return 0;
}

int CInputMain::Shop(LPCHARACTER ch, const char * data, size_t uiBytes)
{
	TPacketCGShop * p = (TPacketCGShop *) data;

	if (uiBytes < sizeof(TPacketCGShop))
		return -1;

	if (test_server)
		sys_log(0, "CInputMain::Shop() ==> SubHeader %d", p->subheader);

	const char * c_pData = data + sizeof(TPacketCGShop);
	uiBytes -= sizeof(TPacketCGShop);

	switch (p->subheader)
	{
		case SHOP_SUBHEADER_CG_END:
			sys_log(1, "INPUT: %s SHOP: END", ch->GetName());
			CShopManager::instance().StopShopping(ch);
			return 0;

		case SHOP_SUBHEADER_CG_BUY:
			{
				if (uiBytes < sizeof(BYTE) + sizeof(BYTE))
					return -1;

				BYTE bPos = *(c_pData + 1);
				sys_log(1, "INPUT: %s SHOP: BUY %d", ch->GetName(), bPos);
				CShopManager::instance().Buy(ch, bPos);
				return (sizeof(BYTE) + sizeof(BYTE));
			}

		case SHOP_SUBHEADER_CG_SELL:
			{
				if (uiBytes < sizeof(BYTE))
					return -1;

				BYTE pos = *c_pData;

				sys_log(0, "INPUT: %s SHOP: SELL", ch->GetName());
				CShopManager::instance().Sell(ch, pos);
				return sizeof(BYTE);
			}

		case SHOP_SUBHEADER_CG_SELL2:
			{
				const TPacketCGShopSell* p = reinterpret_cast<const TPacketCGShopSell*>(c_pData);

				sys_log(0, "INPUT: %s SHOP: SELL2", ch->GetName());

				CShopManager::instance().Sell(ch, p->wPos, p->sCount, p->bType);
				return sizeof(TPacketCGShopSell);
			}

		default:
			sys_err("CInputMain::Shop : Unknown subheader %d : %s", p->subheader, ch->GetName());
			break;
	}

	return 0;
}

void CInputMain::OnClick(LPCHARACTER ch, const char * data)
{
	struct command_on_click *	pinfo = (struct command_on_click *) data;
	LPCHARACTER			victim;

	if ((victim = CHARACTER_MANAGER::instance().Find(pinfo->vid)))
		victim->OnClick(ch);
	else if (test_server)
	{
		sys_err("CInputMain::OnClick %s.Click.NOT_EXIST_VID[%d]", ch->GetName(), pinfo->vid);
	}
}

void CInputMain::Exchange(LPCHARACTER ch, const char * data)
{
	struct command_exchange * pinfo = (struct command_exchange *) data;
	LPCHARACTER	to_ch = NULL;

	if (!ch->CanHandleItem())
		return;

	int iPulse = thecore_pulse();

	if ((to_ch = CHARACTER_MANAGER::instance().Find(pinfo->arg1)))
	{
		if (iPulse - to_ch->GetSafeboxLoadTime() < PASSES_PER_SEC(g_nPortalLimitTime))
		{
			to_ch->ChatPacket(CHAT_TYPE_INFO, "[LS;661;%d]", g_nPortalLimitTime);
			return;
		}

		if( true == to_ch->IsDead() )
		{
			return;
		}
	}

	sys_log(0, "CInputMain()::Exchange()  SubHeader %d ", pinfo->sub_header);

	if (iPulse - ch->GetSafeboxLoadTime() < PASSES_PER_SEC(g_nPortalLimitTime))
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "[LS;661;%d]", g_nPortalLimitTime);
		return;
	}

	switch (pinfo->sub_header)
	{
		case EXCHANGE_SUBHEADER_CG_START:	// arg1 == vid of target character
			if (!ch->GetExchange())
			{
				if ((to_ch = CHARACTER_MANAGER::instance().Find(pinfo->arg1)))
				{
					//MONARCH_LIMIT

					if (iPulse - ch->GetSafeboxLoadTime() < PASSES_PER_SEC(g_nPortalLimitTime))
					{
						ch->ChatPacket(CHAT_TYPE_INFO, "[LS;662;%d]", g_nPortalLimitTime);

						if (test_server)
							ch->ChatPacket(CHAT_TYPE_INFO, "[TestOnly][Safebox]Pulse %d LoadTime %d PASS %d", iPulse, ch->GetSafeboxLoadTime(), PASSES_PER_SEC(g_nPortalLimitTime));
						return;
					}

					if (true == to_ch->IsDead())
					{
						return;
					}

					if (iPulse - to_ch->GetSafeboxLoadTime() < PASSES_PER_SEC(g_nPortalLimitTime))
					{
						to_ch->ChatPacket(CHAT_TYPE_INFO, "[LS;662;%d]", g_nPortalLimitTime);

						if (test_server)
							to_ch->ChatPacket(CHAT_TYPE_INFO, "[TestOnly][Safebox]Pulse %d LoadTime %d PASS %d", iPulse, to_ch->GetSafeboxLoadTime(), PASSES_PER_SEC(g_nPortalLimitTime));
						return;
					}

					if (ch->GetGold() >= GOLD_MAX)
					{
						ch->ChatPacket(CHAT_TYPE_INFO, "[LS;663]");

#ifdef ENABLE_LONG_LONG
						sys_err("[OVERFLOG_GOLD] START (%llu) id %u name %s ", ch->GetGold(), ch->GetPlayerID(), ch->GetName());
#else
						sys_err("[OVERFLOG_GOLD] START (%lld) id %u name %s ", ch->GetGold(), ch->GetPlayerID(), ch->GetName());
#endif
						return;
					}

#ifdef ENABLE_CHEQUE_SYSTEM
					if (ch->GetCheque() > CHEQUE_MAX)
					{
						ch->ChatPacket(CHAT_TYPE_INFO, "[LS;663]");

						sys_err("[OVERFLOW_CHEQUE] START (%lld) id %u name %s ", ch->GetCheque(), ch->GetPlayerID(), ch->GetName());
						return;
					}
#endif

					if (to_ch->IsPC())
					{
						if (quest::CQuestManager::instance().GiveItemToPC(ch->GetPlayerID(), to_ch))
						{
							sys_log(0, "Exchange canceled by quest %s %s", ch->GetName(), to_ch->GetName());
							return;
						}
					}

					if (ch->GetMyShop() || ch->IsOpenSafebox() || ch->GetShopOwner() || ch->IsCubeOpen()
#ifdef __ENABLE_POLYMORPH_SYSTEM__
						|| ch->IsPolyShopping()
#endif	
#ifdef ENABLE_OFFLINE_SHOP_SYSTEM
						|| ch->GetOfflineShopOwner()
#endif	
#ifdef ENABLE_BOT_CONTROL
						|| ch->IsBotControl()
#endif
					)
					{
						ch->ChatPacket(CHAT_TYPE_INFO, "[LS;664]");
						return;
					}

					ch->ExchangeStart(to_ch);
				}
			}
			break;

		case EXCHANGE_SUBHEADER_CG_ITEM_ADD:	// arg1 == position of item, arg2 == position in exchange window
			if (ch->GetExchange())
			{
				if (ch->GetExchange()->GetCompany()->GetAcceptStatus() != true)
#if defined(ITEM_CHECKINOUT_UPDATE)
					ch->GetExchange()->AddItem(pinfo->Pos, pinfo->arg2, pinfo->SelectPosAuto);
#else
					ch->GetExchange()->AddItem(pinfo->Pos, pinfo->arg2);
#endif
			}
			break;

		case EXCHANGE_SUBHEADER_CG_ITEM_DEL:	// arg1 == position of item
			if (ch->GetExchange())
			{
				if (ch->GetExchange()->GetCompany()->GetAcceptStatus() != true)
					ch->GetExchange()->RemoveItem(pinfo->arg1);
			}
			break;

		case EXCHANGE_SUBHEADER_CG_ELK_ADD:	// arg1 == amount of gold
			if (ch->GetExchange())
			{
#ifdef ENABLE_LONG_LONG
				const int64_t nTotalGold = static_cast<int64_t>(ch->GetExchange()->GetCompany()->GetOwner()->GetGold()) + static_cast<int64_t>(pinfo->arg1);
#else
				const int64_t nTotalGold = static_cast<int64_t>(ch->GetExchange()->GetCompany()->GetOwner()->GetGold()) + static_cast<int64_t>(pinfo->arg1);
#endif
				if (GOLD_MAX <= nTotalGold)
				{
					ch->ChatPacket(CHAT_TYPE_INFO, "[LS;665]");

					sys_err("[OVERFLOW_GOLD] ELK_ADD (%lld) id %u name %s ",
							ch->GetExchange()->GetCompany()->GetOwner()->GetGold(),
							ch->GetExchange()->GetCompany()->GetOwner()->GetPlayerID(),
						   	ch->GetExchange()->GetCompany()->GetOwner()->GetName());

					return;
				}

				if (ch->GetExchange()->GetCompany()->GetAcceptStatus() != true)
					ch->GetExchange()->AddGold(pinfo->arg1);
			}
			break;

#ifdef ENABLE_CHEQUE_SYSTEM
		case EXCHANGE_SUBHEADER_CG_CHEQUE_ADD: // arg1 == amount of cheque
			if (ch->GetExchange())
			{
				const int64_t nTotalCheque = static_cast<int64_t>(ch->GetExchange()->GetCompany()->GetOwner()->GetCheque()) + static_cast<int64_t>(pinfo->arg1);

				if (CHEQUE_MAX <= nTotalCheque)
				{
					ch->ChatPacket(CHAT_TYPE_INFO, "[LS;665]");

					sys_err("[OVERFLOW_CHEQUE] CHEQUE_ADD (%lld) id %u name %s ",
						ch->GetExchange()->GetCompany()->GetOwner()->GetCheque(),
						ch->GetExchange()->GetCompany()->GetOwner()->GetPlayerID(),
						ch->GetExchange()->GetCompany()->GetOwner()->GetName());

					return;
				}

				if (ch->GetExchange()->GetCompany()->GetAcceptStatus() != true)
					ch->GetExchange()->AddCheque(pinfo->arg1);
			}
			break;
#endif

		case EXCHANGE_SUBHEADER_CG_ACCEPT:	// arg1 == not used
			if (ch->GetExchange())
			{
				sys_log(0, "CInputMain()::Exchange() ==> ACCEPT ");
				ch->GetExchange()->Accept(true);
			}

			break;

		case EXCHANGE_SUBHEADER_CG_CANCEL:	// arg1 == not used
			if (ch->GetExchange())
				ch->GetExchange()->Cancel();
			break;
	}
}

void CInputMain::Position(LPCHARACTER ch, const char * data)
{
	struct command_position * pinfo = (struct command_position *) data;

	switch (pinfo->position)
	{
		case POSITION_GENERAL:
			ch->Standup();
			break;

		case POSITION_SITTING_CHAIR:
			ch->Sitdown(0);
			break;

		case POSITION_SITTING_GROUND:
			ch->Sitdown(1);
			break;
	}
}

static const int ComboSequenceBySkillLevel[3][8] =
{
	// 0   1   2   3   4   5   6   7
	{ 14, 15, 16, 17,  0,  0,  0,  0 },
	{ 14, 15, 16, 18, 20,  0,  0,  0 },
	{ 14, 15, 16, 18, 19, 17,  0,  0 },
};

#define COMBO_HACK_ALLOWABLE_MS	100

bool CheckComboHack(LPCHARACTER ch, BYTE bArg, DWORD dwTime, bool CheckSpeedHack)
{
	if(!gHackCheckEnable) return false;

	//	if (IsStun() || IsDead()) return false;

	if (ch->IsStun() || ch->IsDead())
		return false;
	int ComboInterval = dwTime - ch->GetLastComboTime();
	int HackScalar = 0;
#if 0
	sys_log(0, "COMBO: %s arg:%u seq:%u delta:%d checkspeedhack:%d",
			ch->GetName(), bArg, ch->GetComboSequence(), ComboInterval - ch->GetValidComboInterval(), CheckSpeedHack);
#endif

	if (bArg == 14)
	{
		if (CheckSpeedHack && ComboInterval > 0 && ComboInterval < ch->GetValidComboInterval() - COMBO_HACK_ALLOWABLE_MS)
		{
			//HackScalar = 1 + (ch->GetValidComboInterval() - ComboInterval) / 300;

			//sys_log(0, "COMBO_HACK: 2 %s arg:%u interval:%d valid:%u atkspd:%u riding:%s",
			//		ch->GetName(),
			//		bArg,
			//		ComboInterval,
			//		ch->GetValidComboInterval(),
			//		ch->GetPoint(POINT_ATT_SPEED),
			//	    ch->IsRiding() ? "yes" : "no");
		}

		ch->SetComboSequence(1);
		ch->SetValidComboInterval((int) (ani_combo_speed(ch, 1) / (ch->GetPoint(POINT_ATT_SPEED) / 100.f)));
		ch->SetLastComboTime(dwTime);
	}
	else if (bArg > 14 && bArg < 22)
	{
		int idx = MIN(2, ch->GetComboIndex());

		if (ch->GetComboSequence() > 5)
		{
			HackScalar = 1;
			ch->SetValidComboInterval(300);
			sys_log(0, "COMBO_HACK: 5 %s combo_seq:%d", ch->GetName(), ch->GetComboSequence());
		}

		else if (bArg == 21 &&
				 idx == 2 &&
				 ch->GetComboSequence() == 5 &&
				 ch->GetJob() == JOB_ASSASSIN &&
				 ch->GetWear(WEAR_WEAPON) &&
				 ch->GetWear(WEAR_WEAPON)->GetSubType() == WEAPON_DAGGER)
			ch->SetValidComboInterval(300);
#ifdef ENABLE_WOLFMAN_CHARACTER
		else if (bArg == 21 && idx == 2 && ch->GetComboSequence() == 5 && ch->GetJob() == JOB_WOLFMAN && ch->GetWear(WEAR_WEAPON) && ch->GetWear(WEAR_WEAPON)->GetSubType() == WEAPON_CLAW)
			ch->SetValidComboInterval(300);
#endif
		else if (ComboSequenceBySkillLevel[idx][ch->GetComboSequence()] != bArg)
		{
			HackScalar = 1;
			ch->SetValidComboInterval(300);

			sys_log(0, "COMBO_HACK: 3 %s arg:%u valid:%u combo_idx:%d combo_seq:%d",
					ch->GetName(),
					bArg,
					ComboSequenceBySkillLevel[idx][ch->GetComboSequence()],
					idx,
					ch->GetComboSequence());
		}
		else
		{
			if (CheckSpeedHack && ComboInterval < ch->GetValidComboInterval() - COMBO_HACK_ALLOWABLE_MS)
			{
				HackScalar = 1 + (ch->GetValidComboInterval() - ComboInterval) / 100;

				sys_log(0, "COMBO_HACK: 2 %s arg:%u interval:%d valid:%u atkspd:%u riding:%s",
						ch->GetName(),
						bArg,
						ComboInterval,
						ch->GetValidComboInterval(),
						ch->GetPoint(POINT_ATT_SPEED),
						ch->IsRiding() ? "yes" : "no");
			}

			//if (ch->IsHorseRiding())
			if (ch->IsRiding())
				ch->SetComboSequence(ch->GetComboSequence() == 1 ? 2 : 1);
			else
				ch->SetComboSequence(ch->GetComboSequence() + 1);

			ch->SetValidComboInterval((int) (ani_combo_speed(ch, bArg - 13) / (ch->GetPoint(POINT_ATT_SPEED) / 100.f)));
			ch->SetLastComboTime(dwTime);
		}
	}
	else if (bArg == 13)
	{
		if (CheckSpeedHack && ComboInterval > 0 && ComboInterval < ch->GetValidComboInterval() - COMBO_HACK_ALLOWABLE_MS)
		{
			//HackScalar = 1 + (ch->GetValidComboInterval() - ComboInterval) / 100;

			//sys_log(0, "COMBO_HACK: 6 %s arg:%u interval:%d valid:%u atkspd:%u",
			//		ch->GetName(),
			//		bArg,
			//		ComboInterval,
			//		ch->GetValidComboInterval(),
			//		ch->GetPoint(POINT_ATT_SPEED));
		}

		if (ch->GetRaceNum() >= MAIN_RACE_MAX_NUM)
		{
			// POLYMORPH_BUG_FIX

			// DELETEME

			float normalAttackDuration = CMotionManager::instance().GetNormalAttackDuration(ch->GetRaceNum());
			int k = (int) (normalAttackDuration / ((float) ch->GetPoint(POINT_ATT_SPEED) / 100.f) * 900.f);
			ch->SetValidComboInterval(k);
			ch->SetLastComboTime(dwTime);
			// END_OF_POLYMORPH_BUG_FIX
		}
		else
		{
			//if (ch->GetDesc()->DelayedDisconnect(number(2, 9)))
			//{
			//	LogManager::instance().HackLog("Hacker", ch);
			//	sys_log(0, "HACKER: %s arg %u", ch->GetName(), bArg);
			//}

			// by rtsummit
		}
	}
	else
	{
		if (ch->GetDesc()->DelayedDisconnect(number(2, 9)))
		{
			LogManager::instance().HackLog("Hacker", ch);
			sys_log(0, "HACKER: %s arg %u", ch->GetName(), bArg);
		}

		HackScalar = 10;
		ch->SetValidComboInterval(300);
	}

	if (HackScalar)
	{
		if (get_dword_time() - ch->GetLastMountTime() > 1500)
			ch->IncreaseComboHackCount(1 + HackScalar);

		ch->SkipComboAttackByTime(ch->GetValidComboInterval());
	}

	return HackScalar;
}

void CInputMain::Move(LPCHARACTER ch, const char * data)
{
	if (!ch->CanMove())
		return;

	struct command_move * pinfo = (struct command_move *) data;

	if (pinfo->bFunc >= FUNC_MAX_NUM && !(pinfo->bFunc & 0x80))
	{
		sys_err("invalid move type: %s", ch->GetName());
		return;
	}

	//enum EMoveFuncType
	//{
	//	FUNC_WAIT,
	//	FUNC_MOVE,
	//	FUNC_ATTACK,
	//	FUNC_COMBO,
	//	FUNC_MOB_SKILL,
	//	_FUNC_SKILL,
	//	FUNC_MAX_NUM,
	//	FUNC_SKILL = 0x80,
	//};

	{
		// #define ENABLE_TP_SPEED_CHECK
		#ifdef ENABLE_TP_SPEED_CHECK
		const float fDist = DISTANCE_SQRT((ch->GetX() - pinfo->lX) / 100, (ch->GetY() - pinfo->lY) / 100);
		// if (((false == ch->IsRiding() && fDist > 25) || fDist > 60) && OXEVENT_MAP_INDEX != ch->GetMapIndex()) // @fixme106 (changed 40 to 60)
		if (((false == ch->IsRiding() && fDist > 60) || fDist > 125) && OXEVENT_MAP_INDEX != ch->GetMapIndex())
		{
			#ifdef ENABLE_HACK_TELEPORT_LOG // @warme006
			{
				const PIXEL_POSITION & warpPos = ch->GetWarpPosition();

				if (warpPos.x == 0 && warpPos.y == 0)
					LogManager::instance().HackLog("Teleport", ch);
			}
			#endif
			sys_log(0, "MOVE: %s trying to move too far (dist: %.1fm) Riding(%d)", ch->GetName(), fDist, ch->IsRiding());

			ch->Show(ch->GetMapIndex(), ch->GetX(), ch->GetY(), ch->GetZ());
			ch->Stop();
			return;
		}
		#endif
		#ifdef ENABLE_CHECK_GHOSTMODE
		if (ch->IsPC() && ch->IsDead())
		{
			sys_log(0, "MOVE: %s trying to move as dead", ch->GetName());

			ch->Show(ch->GetMapIndex(), ch->GetX(), ch->GetY(), ch->GetZ());
			ch->Stop();
			return;
		}
		#endif

		DWORD dwCurTime = get_dword_time();

		bool CheckSpeedHack = (false == ch->GetDesc()->IsHandshaking() && dwCurTime - ch->GetDesc()->GetClientTime() > 7000);

		if (CheckSpeedHack)
		{
			int iDelta = (int) (pinfo->dwTime - ch->GetDesc()->GetClientTime());
			int iServerDelta = (int) (dwCurTime - ch->GetDesc()->GetClientTime());

			iDelta = (int) (dwCurTime - pinfo->dwTime);

			if (iDelta >= 30000)
			{
				sys_log(0, "SPEEDHACK: slow timer name %s delta %d", ch->GetName(), iDelta);
				ch->GetDesc()->DelayedDisconnect(3);
			}

			else if (iDelta < -(iServerDelta / 50))
			{
				sys_log(0, "SPEEDHACK: DETECTED! %s (delta %d %d)", ch->GetName(), iDelta, iServerDelta);
				ch->GetDesc()->DelayedDisconnect(3);
			}
		}

		if (pinfo->bFunc == FUNC_COMBO && g_bCheckMultiHack)
		{
			CheckComboHack(ch, pinfo->bArg, pinfo->dwTime, CheckSpeedHack);
		}
	}

    if (ch->GetCheatController())
        ch->GetCheatController()->ReceiveMovePacket(pinfo->dwTime);
		
	if (pinfo->bFunc == FUNC_MOVE)
	{
		if (ch->GetLimitPoint(POINT_MOV_SPEED) == 0)
			return;

		ch->SetRotation(pinfo->bRot * 5);
		ch->ResetStopTime();				// ""

		ch->Goto(pinfo->lX, pinfo->lY);
	}
	else
	{
#ifndef ENABLE_ANTY_CHEAT
		if (pinfo->bFunc == FUNC_ATTACK || pinfo->bFunc == FUNC_COMBO)
			ch->OnMove(true);
#else
		if (pinfo->bFunc == FUNC_ATTACK || pinfo->bFunc == FUNC_COMBO)
		{
			if (pinfo->bFunc == FUNC_COMBO)
			{
				DWORD now = get_dword_time();
				DWORD attackCooltimeTime = ch->GetLastAttackTime() + 50;
				int check_this = attackCooltimeTime;
				if (check_this < 0)
				{
					if (test_server)
						sys_err("Player %s BLOCKED IS COOLTIME %d ", ch->GetName(), check_this);
					attackCooltimeTime = get_dword_time()-100;
				}

				if (attackCooltimeTime > now)
					return;

				ch->SetFuncComboLimterTime(now);
				ch->SetLastAttackPosition(pinfo->lX, pinfo->lY);
			}
			ch->OnMove(true);

			ch->GetAbuseController()->ReceiveMoveAttackPacket();
		}
#endif

		else if (pinfo->bFunc & FUNC_SKILL)
		{
			const int MASK_SKILL_MOTION = 0x7F;
			unsigned int motion = pinfo->bFunc & MASK_SKILL_MOTION;

			if (!ch->IsUsableSkillMotion(motion))
			{
				const char* name = ch->GetName();
				unsigned int job = ch->GetJob();
				unsigned int group = ch->GetSkillGroup();

				char szBuf[256];
				snprintf(szBuf, sizeof(szBuf), "SKILL_HACK: name=%s, job=%d, group=%d, motion=%d", name, job, group, motion);
				LogManager::instance().HackLog(szBuf, ch->GetDesc()->GetAccountTable().login, ch->GetName(), ch->GetDesc()->GetHostName());
				sys_log(0, "%s", szBuf);

				if (test_server)
				{
					ch->GetDesc()->DelayedDisconnect(number(2, 8));
					ch->ChatPacket(CHAT_TYPE_INFO, szBuf);
				}
				else
				{
					ch->GetDesc()->DelayedDisconnect(number(150, 500));
				}
			}

			ch->OnMove();
		}

		ch->SetRotation(pinfo->bRot * 5);
		ch->ResetStopTime();				// ""

		ch->Move(pinfo->lX, pinfo->lY);
		ch->Stop();
		ch->StopStaminaConsume();
	}

	TPacketGCMove pack;

	pack.bHeader      = HEADER_GC_MOVE;
	pack.bFunc        = pinfo->bFunc;
	pack.bArg         = pinfo->bArg;
	pack.bRot         = pinfo->bRot;
	pack.dwVID        = ch->GetVID();
	pack.lX           = pinfo->lX;
	pack.lY           = pinfo->lY;
	pack.dwTime       = pinfo->dwTime;
	pack.dwDuration   = (pinfo->bFunc == FUNC_MOVE) ? ch->GetCurrentMoveDuration() : 0;

	ch->PacketAround(&pack, sizeof(TPacketGCMove), ch);

	/*
	sys_log(0,
			"MOVE: %s Func:%u Arg:%u Pos:%dx%d Time:%u Dist:%.1f",
			ch->GetName(),
			pinfo->bFunc,
			pinfo->bArg,
			pinfo->lX / 100,
			pinfo->lY / 100,
			pinfo->dwTime,
			fDist);
	*/
}

void CInputMain::Attack(LPCHARACTER ch, const BYTE header, const char* data)
{
	if (NULL == ch)
		return;

	struct type_identifier
	{
		BYTE header;
		BYTE type;
	};

	const struct type_identifier* const type = reinterpret_cast<const struct type_identifier*>(data);

	if (type->type > 0)
	{
		if (false == ch->CanUseSkill(type->type))
		{
			return;
		}

		switch (type->type)
		{
			case SKILL_GEOMPUNG:
			case SKILL_SANGONG:
			case SKILL_YEONSA:
			case SKILL_KWANKYEOK:
			case SKILL_HWAJO:
			case SKILL_GIGUNG:
			case SKILL_PABEOB:
			case SKILL_MARYUNG:
			case SKILL_TUSOK:
			case SKILL_MAHWAN:
			case SKILL_BIPABU:
			case SKILL_NOEJEON:
			case SKILL_CHAIN:
			case SKILL_HORSE_WILDATTACK_RANGE:
				if (HEADER_CG_SHOOT != type->header)
				{
					if (test_server)
						ch->ChatPacket(CHAT_TYPE_INFO, "Attack :name[%s] Vnum[%d] can't use skill by attack(warning)", type->type);
					return;
				}
				break;
		}
	}

	switch (header)
	{
		case HEADER_CG_ATTACK:
			{
				if (NULL == ch->GetDesc())
					return;

				const TPacketCGAttack* const packMelee = reinterpret_cast<const TPacketCGAttack*>(data);

				ch->GetDesc()->AssembleCRCMagicCube(packMelee->bCRCMagicCubeProcPiece, packMelee->bCRCMagicCubeFilePiece);

				LPCHARACTER	victim = CHARACTER_MANAGER::instance().Find(packMelee->dwVID);

				if (NULL == victim || ch == victim)
					return;

				switch (victim->GetCharType())
				{
					case CHAR_TYPE_NPC:
					case CHAR_TYPE_WARP:
					case CHAR_TYPE_GOTO:
					case CHAR_TYPE_HORSE:
						return;
				}

				if (packMelee->bType > 0)
				{
					if (false == ch->CheckSkillHitCount(packMelee->bType, victim->GetVID()))
					{
						return;
					}
				}

				ch->Attack(victim, packMelee->bType);
			}
			break;

		case HEADER_CG_SHOOT:
			{
				const TPacketCGShoot* const packShoot = reinterpret_cast<const TPacketCGShoot*>(data);

				ch->Shoot(packShoot->bType);
			}
			break;
	}
}

int CInputMain::SyncPosition(LPCHARACTER ch, const char * c_pcData, size_t uiBytes)
{
	const TPacketCGSyncPosition* pinfo = reinterpret_cast<const TPacketCGSyncPosition*>( c_pcData );

	if (uiBytes < pinfo->wSize)
		return -1;

	int iExtraLen = pinfo->wSize - sizeof(TPacketCGSyncPosition);

	if (iExtraLen < 0)
	{
		sys_err("invalid packet length (len %d size %u buffer %u)", iExtraLen, pinfo->wSize, uiBytes);
		ch->GetDesc()->SetPhase(PHASE_CLOSE);
		return -1;
	}

	if (0 != (iExtraLen % sizeof(TPacketCGSyncPositionElement)))
	{
		sys_err("invalid packet length %d (name: %s)", pinfo->wSize, ch->GetName());
		return iExtraLen;
	}

	int iCount = iExtraLen / sizeof(TPacketCGSyncPositionElement);

	if (iCount <= 0)
		return iExtraLen;

	static const int nCountLimit = 16;

	if( iCount > nCountLimit )
	{
		//LogManager::instance().HackLog( "SYNC_POSITION_HACK", ch );
		sys_err( "Too many SyncPosition Count(%d) from Name(%s)", iCount, ch->GetName() );
		//ch->GetDesc()->SetPhase(PHASE_CLOSE);
		//return -1;
		iCount = nCountLimit;
	}

	TEMP_BUFFER tbuf;
	LPBUFFER lpBuf = tbuf.getptr();

	TPacketGCSyncPosition * pHeader = (TPacketGCSyncPosition *) buffer_write_peek(lpBuf);
	buffer_write_proceed(lpBuf, sizeof(TPacketGCSyncPosition));

	const TPacketCGSyncPositionElement* e =
		reinterpret_cast<const TPacketCGSyncPositionElement*>(c_pcData + sizeof(TPacketCGSyncPosition));

	timeval tvCurTime;
	gettimeofday(&tvCurTime, NULL);

	for (int i = 0; i < iCount; ++i, ++e)
	{
		LPCHARACTER victim = CHARACTER_MANAGER::instance().Find(e->dwVID);

		if (!victim)
			continue;

		switch (victim->GetCharType())
		{
			case CHAR_TYPE_NPC:
			case CHAR_TYPE_WARP:
			case CHAR_TYPE_GOTO:
			case CHAR_TYPE_HORSE:
				continue;
		}

		if (!victim->SetSyncOwner(ch))
			continue;

		const float fDistWithSyncOwner = DISTANCE_SQRT( (victim->GetX() - ch->GetX()) / 100, (victim->GetY() - ch->GetY()) / 100 );
		static const float fLimitDistWithSyncOwner = 2500.f + 1000.f;

		if (fDistWithSyncOwner > fLimitDistWithSyncOwner)
		{
			if (ch->GetSyncHackCount() < g_iSyncHackLimitCount)
			{
				ch->SetSyncHackCount(ch->GetSyncHackCount() + 1);
				continue;
			}
			else
			{
				LogManager::instance().HackLog( "SYNC_POSITION_HACK", ch );

				sys_err( "Too far SyncPosition DistanceWithSyncOwner(%f)(%s) from Name(%s) CH(%d,%d) VICTIM(%d,%d) SYNC(%d,%d)",
					fDistWithSyncOwner, victim->GetName(), ch->GetName(), ch->GetX(), ch->GetY(), victim->GetX(), victim->GetY(),
					e->lX, e->lY );

				//ch->GetDesc()->SetPhase(PHASE_CLOSE);

				continue;
			}
		}

		const float fDist = DISTANCE_SQRT( (victim->GetX() - e->lX) / 100, (victim->GetY() - e->lY) / 100 );
		static const int32_t g_lValidSyncInterval = 100 * 1000; // 100ms
		const timeval &tvLastSyncTime = victim->GetLastSyncTime();
		timeval *tvDiff = timediff(&tvCurTime, &tvLastSyncTime);

		if (tvDiff->tv_sec == 0 && tvDiff->tv_usec < g_lValidSyncInterval)
		{
			if (ch->GetSyncHackCount() < g_iSyncHackLimitCount)
			{
				ch->SetSyncHackCount(ch->GetSyncHackCount() + 1);
				continue;
			}
			else
			{
				LogManager::instance().HackLog( "SYNC_POSITION_HACK", ch );

				sys_err( "Too often SyncPosition Interval(%ldms)(%s) from Name(%s) VICTIM(%d,%d) SYNC(%d,%d)",
					tvDiff->tv_sec * 1000 + tvDiff->tv_usec / 1000, victim->GetName(), ch->GetName(), victim->GetX(), victim->GetY(),
					e->lX, e->lY );

				//ch->GetDesc()->SetPhase(PHASE_CLOSE);

				continue;
			}
		}
		else if( fDist > 25.0f )
		{
			LogManager::instance().HackLog( "SYNC_POSITION_HACK", ch );

			sys_err( "Too far SyncPosition Distance(%f)(%s) from Name(%s) CH(%d,%d) VICTIM(%d,%d) SYNC(%d,%d)",
				   	fDist, victim->GetName(), ch->GetName(), ch->GetX(), ch->GetY(), victim->GetX(), victim->GetY(),
				  e->lX, e->lY );

			//ch->GetDesc()->SetPhase(PHASE_CLOSE);

			continue;
		}
		else
		{
			victim->SetLastSyncTime(tvCurTime);
			victim->Sync(e->lX, e->lY);
			buffer_write(lpBuf, e, sizeof(TPacketCGSyncPositionElement));
		}
	}

	if (buffer_size(lpBuf) != sizeof(TPacketGCSyncPosition))
	{
		pHeader->bHeader = HEADER_GC_SYNC_POSITION;
		pHeader->wSize = buffer_size(lpBuf);

		ch->PacketAround(buffer_read_peek(lpBuf), buffer_size(lpBuf), ch);
	}

	return iExtraLen;
}

void CInputMain::FlyTarget(LPCHARACTER ch, const char * pcData, BYTE bHeader)
{
	TPacketCGFlyTargeting * p = (TPacketCGFlyTargeting *) pcData;
	ch->FlyTarget(p->dwTargetVID, p->x, p->y, bHeader);
}

void CInputMain::UseSkill(LPCHARACTER ch, const char * pcData)
{
	TPacketCGUseSkill * p = (TPacketCGUseSkill *) pcData;
	ch->UseSkill(p->dwVnum, CHARACTER_MANAGER::instance().Find(p->dwVID));
}

void CInputMain::ScriptButton(LPCHARACTER ch, const void* c_pData)
{
	TPacketCGScriptButton * p = (TPacketCGScriptButton *) c_pData;
	sys_log(0, "QUEST ScriptButton pid %d idx %u", ch->GetPlayerID(), p->idx);

	quest::PC* pc = quest::CQuestManager::instance().GetPCForce(ch->GetPlayerID());
	if (pc && pc->IsConfirmWait())
	{
		quest::CQuestManager::instance().Confirm(ch->GetPlayerID(), quest::CONFIRM_TIMEOUT);
	}
	else if (p->idx & 0x80000000)
	{
		quest::CQuestManager::Instance().QuestInfo(ch->GetPlayerID(), p->idx & 0x7fffffff);
	}
	else
	{
#ifdef ENABLE_COLLECT_WINDOW
		quest::CQuestManager::Instance().QuestButton(ch->GetPlayerID(), p->idx, p->button);
#else
		quest::CQuestManager::Instance().QuestButton(ch->GetPlayerID(), p->idx);
#endif
	}
}

void CInputMain::ScriptAnswer(LPCHARACTER ch, const void* c_pData)
{
	TPacketCGScriptAnswer * p = (TPacketCGScriptAnswer *) c_pData;
	sys_log(0, "QUEST ScriptAnswer pid %d answer %d", ch->GetPlayerID(), p->answer);

	if (p->answer > 250)
	{
		quest::CQuestManager::Instance().Resume(ch->GetPlayerID());
	}
	else
	{
		quest::CQuestManager::Instance().Select(ch->GetPlayerID(),  p->answer);
	}
}

// SCRIPT_SELECT_ITEM
void CInputMain::ScriptSelectItem(LPCHARACTER ch, const void* c_pData)
{
	TPacketCGScriptSelectItem* p = (TPacketCGScriptSelectItem*) c_pData;
	sys_log(0, "QUEST ScriptSelectItem pid %d answer %d", ch->GetPlayerID(), p->selection);
	quest::CQuestManager::Instance().SelectItem(ch->GetPlayerID(), p->selection);
}
// END_OF_SCRIPT_SELECT_ITEM

void CInputMain::QuestInputString(LPCHARACTER ch, const void* c_pData)
{
	TPacketCGQuestInputString * p = (TPacketCGQuestInputString*) c_pData;

	char msg[65];
	strlcpy(msg, p->msg, sizeof(msg));
	sys_log(0, "QUEST InputString pid %u msg %s", ch->GetPlayerID(), msg);

	quest::CQuestManager::Instance().Input(ch->GetPlayerID(), msg);
}

void CInputMain::QuestConfirm(LPCHARACTER ch, const void* c_pData)
{
	TPacketCGQuestConfirm* p = (TPacketCGQuestConfirm*) c_pData;
	LPCHARACTER ch_wait = CHARACTER_MANAGER::instance().FindByPID(p->requestPID);
	if (p->answer)
		p->answer = quest::CONFIRM_YES;
	sys_log(0, "QuestConfirm from %s pid %u name %s answer %d", ch->GetName(), p->requestPID, (ch_wait)?ch_wait->GetName():"", p->answer);
	if (ch_wait)
	{
		quest::CQuestManager::Instance().Confirm(ch_wait->GetPlayerID(), (quest::EQuestConfirmType) p->answer, ch->GetPlayerID());
	}
}

void CInputMain::Target(LPCHARACTER ch, const char * pcData)
{
	TPacketCGTarget * p = (TPacketCGTarget *) pcData;

	building::LPOBJECT pkObj = building::CManager::instance().FindObjectByVID(p->dwVID);

	if (pkObj)
	{
		TPacketGCTarget pckTarget;
		pckTarget.header = HEADER_GC_TARGET;
		pckTarget.dwVID = p->dwVID;
		ch->GetDesc()->Packet(&pckTarget, sizeof(TPacketGCTarget));
	}
	else
		ch->SetTarget(CHARACTER_MANAGER::instance().Find(p->dwVID));
}

void CInputMain::Warp(LPCHARACTER ch, const char * pcData)
{
	ch->WarpEnd();
}

void CInputMain::SafeboxCheckin(LPCHARACTER ch, const char * c_pData)
{
	if (quest::CQuestManager::instance().GetPCForce(ch->GetPlayerID())->IsRunning() == true)
		return;

	TPacketCGSafeboxCheckin * p = (TPacketCGSafeboxCheckin *) c_pData;

	if (!ch->CanHandleItem())
		return;

	CSafebox * pkSafebox = ch->GetSafebox();
	LPITEM pkItem = ch->GetItem(p->ItemPos);

	if (!pkSafebox || !pkItem)
		return;

	if (pkItem->GetCell() >= INVENTORY_MAX_NUM && IS_SET(pkItem->GetFlag(), ITEM_FLAG_IRREMOVABLE))
	{
	    return;
	}

	if (!pkSafebox->IsEmpty(p->bSafePos, pkItem->GetSize()))
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "[LS;666]");
		return;
	}

	if (pkItem->GetVnum() == UNIQUE_ITEM_SAFEBOX_EXPAND)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "[LS;667]");
		return;
	}

	if( IS_SET(pkItem->GetAntiFlag(), ITEM_ANTIFLAG_SAFEBOX) )
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "[LS;667]");
		return;
	}

	if (true == pkItem->isLocked())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "[LS;667]");
		return;
	}

#ifdef ENABLE_WEAPON_COSTUME_SYSTEM
	if (pkItem->IsEquipped())
	{
		int iWearCell = pkItem->FindEquipCell(ch);
		if (iWearCell == WEAR_WEAPON)
		{
			LPITEM costumeWeapon = ch->GetWear(WEAR_COSTUME_WEAPON);
			if (costumeWeapon && !ch->UnequipItem(costumeWeapon))
			{
				ch->ChatPacket(CHAT_TYPE_INFO, "You cannot unequip the costume weapon. Not enough space.");
				return;
			}
		}
	}
#endif

	// @fixme140 BEGIN
	if (ITEM_BELT == pkItem->GetType() && CBeltInventoryHelper::IsExistItemInBeltInventory(ch))
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "[LS;1095]");
		return;
	}
	// @fixme140 END

	pkItem->RemoveFromCharacter();
	if (!pkItem->IsDragonSoul())
		ch->SyncQuickslot(QUICKSLOT_TYPE_ITEM, p->ItemPos.cell, 255);
	pkSafebox->Add(p->bSafePos, pkItem);

	char szHint[128];
	snprintf(szHint, sizeof(szHint), "%s %u", pkItem->GetName(), pkItem->GetCount());
	LogManager::instance().ItemLog(ch, pkItem, "SAFEBOX PUT", szHint);
}

void CInputMain::SafeboxCheckout(LPCHARACTER ch, const char * c_pData, bool bMall)
{
	TPacketCGSafeboxCheckout * p = (TPacketCGSafeboxCheckout *) c_pData;

	if (!ch->CanHandleItem())
		return;

	CSafebox * pkSafebox;

	if (bMall)
		pkSafebox = ch->GetMall();
	else
		pkSafebox = ch->GetSafebox();

	if (!pkSafebox)
		return;

	LPITEM pkItem = pkSafebox->Get(p->bSafePos);

	if (!pkItem)
		return;

	if (!ch->IsEmptyItemGrid(p->ItemPos, pkItem->GetSize()))
		return;

	if (pkItem->IsDragonSoul())
	{
		if (bMall)
		{
			DSManager::instance().DragonSoulItemInitialize(pkItem);
		}

		if (DRAGON_SOUL_INVENTORY != p->ItemPos.window_type)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "[LS;666]");
			return;
		}

		TItemPos DestPos = p->ItemPos;
		if (!DSManager::instance().IsValidCellForThisItem(pkItem, DestPos))
		{
			int iCell = ch->GetEmptyDragonSoulInventory(pkItem);
			if (iCell < 0)
			{
				ch->ChatPacket(CHAT_TYPE_INFO, "[LS;666]");
				return ;
			}
			DestPos = TItemPos (DRAGON_SOUL_INVENTORY, iCell);
		}

		pkSafebox->Remove(p->bSafePos);
		pkItem->AddToCharacter(ch, DestPos);
		ITEM_MANAGER::instance().FlushDelayedSave(pkItem);
	}
#ifdef ENABLE_SPLIT_INVENTORY_SYSTEM
	else if (pkItem->IsSkillBook())
	{
		if (DRAGON_SOUL_INVENTORY == p->ItemPos.window_type)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "[LS;666]");
			return;
		}
		
		if (p->ItemPos.IsSkillBookInventoryPosition() == false && p->ItemPos.IsDefaultInventoryPosition() == false)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "You cannot move item here.");
			return;
		}
		
		pkSafebox->Remove(p->bSafePos);
		pkItem->AddToCharacter(ch, p->ItemPos);
		ITEM_MANAGER::instance().FlushDelayedSave(pkItem);
	}
	else if (pkItem->IsUpgradeItem())
	{
		if (DRAGON_SOUL_INVENTORY == p->ItemPos.window_type)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "[LS;666]");
			return;
		}
		
		if (p->ItemPos.IsUpgradeItemsInventoryPosition() == false && p->ItemPos.IsDefaultInventoryPosition() == false)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "You cannot move item here.");
			return;
		}
		
		pkSafebox->Remove(p->bSafePos);
		pkItem->AddToCharacter(ch, p->ItemPos);
		ITEM_MANAGER::instance().FlushDelayedSave(pkItem);
	}
	else if (pkItem->IsStone())
	{
		if (DRAGON_SOUL_INVENTORY == p->ItemPos.window_type)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "[LS;666]");
			return;
		}
		
		if (p->ItemPos.IsStoneInventoryPosition() == false && p->ItemPos.IsDefaultInventoryPosition() == false)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "You cannot move item here.");
			return;
		}
		
		pkSafebox->Remove(p->bSafePos);
		pkItem->AddToCharacter(ch, p->ItemPos);
		ITEM_MANAGER::instance().FlushDelayedSave(pkItem);
	}
	else if (pkItem->IsBox())
	{
		if (DRAGON_SOUL_INVENTORY == p->ItemPos.window_type)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "[LS;666]");
			return;
		}
		
		if (p->ItemPos.IsBoxInventoryPosition() == false && p->ItemPos.IsDefaultInventoryPosition() == false)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "You cannot move item here.");
			return;
		}
		
		pkSafebox->Remove(p->bSafePos);
		pkItem->AddToCharacter(ch, p->ItemPos);
		ITEM_MANAGER::instance().FlushDelayedSave(pkItem);
	}
	else if (pkItem->IsEfsun())
	{
		if (DRAGON_SOUL_INVENTORY == p->ItemPos.window_type)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "[LS;666]");
			return;
		}
		
		if (p->ItemPos.IsEfsunInventoryPosition() == false && p->ItemPos.IsDefaultInventoryPosition() == false)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "You cannot move item here.");
			return;
		}
		
		pkSafebox->Remove(p->bSafePos);
		pkItem->AddToCharacter(ch, p->ItemPos);
		ITEM_MANAGER::instance().FlushDelayedSave(pkItem);
	}
	else if (pkItem->IsCicek())
	{
		if (DRAGON_SOUL_INVENTORY == p->ItemPos.window_type)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "[LS;666]");
			return;
		}
		
		if (p->ItemPos.IsCicekInventoryPosition() == false && p->ItemPos.IsDefaultInventoryPosition() == false)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "You cannot move item here.");
			return;
		}
		
		pkSafebox->Remove(p->bSafePos);
		pkItem->AddToCharacter(ch, p->ItemPos);
		ITEM_MANAGER::instance().FlushDelayedSave(pkItem);
	}
#endif
	else
	{
		if (DRAGON_SOUL_INVENTORY == p->ItemPos.window_type)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "[LS;666]");
			return;
		}
		// @fixme119
		if (p->ItemPos.IsBeltInventoryPosition() && false == CBeltInventoryHelper::CanMoveIntoBeltInventory(pkItem))
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "[LS;1097]");
			return;
		}

		pkSafebox->Remove(p->bSafePos);
		pkItem->AddToCharacter(ch, p->ItemPos);
		ITEM_MANAGER::instance().FlushDelayedSave(pkItem);
	}

	DWORD dwID = pkItem->GetID();
	db_clientdesc->DBPacketHeader(HEADER_GD_ITEM_FLUSH, 0, sizeof(DWORD));
	db_clientdesc->Packet(&dwID, sizeof(DWORD));

	char szHint[128];
	snprintf(szHint, sizeof(szHint), "%s %u", pkItem->GetName(), pkItem->GetCount());
	if (bMall)
		LogManager::instance().ItemLog(ch, pkItem, "MALL GET", szHint);
	else
		LogManager::instance().ItemLog(ch, pkItem, "SAFEBOX GET", szHint);
}

void CInputMain::SafeboxItemMove(LPCHARACTER ch, const char * data)
{
	struct command_item_move * pinfo = (struct command_item_move *) data;

	if (!ch->CanHandleItem())
		return;

	if (!ch->GetSafebox())
		return;

	ch->GetSafebox()->MoveItem(pinfo->Cell.cell, pinfo->CellTo.cell, pinfo->count);
}

// PARTY_JOIN_BUG_FIX
void CInputMain::PartyInvite(LPCHARACTER ch, const char * c_pData)
{
	if (ch->GetArena())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "[LS;403]");
		return;
	}
#ifdef ENABLE_PVP_RANKING
	if (ch->GetMapIndex() == CPvPRanking::instance().GetMapIndex() && quest::CQuestManager::instance().GetEventFlag("pvp_ranking") > 0)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("V arene nelze pozvat hrace do skupiny."));
		return;
	}
#endif

	TPacketCGPartyInvite * p = (TPacketCGPartyInvite*) c_pData;

	LPCHARACTER pInvitee = CHARACTER_MANAGER::instance().Find(p->vid);

	if (!pInvitee || !ch->GetDesc() || !pInvitee->GetDesc())
	{
		sys_err("PARTY Cannot find invited character");
		return;
	}
#ifdef ENABLE_MESSENGER_BLOCK
	if (MessengerManager::instance().IsBlocked(ch->GetName(), pInvitee->GetName()))
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "You need to unblock %s do to that.", pInvitee->GetName());
		return;
	}
	if (MessengerManager::instance().IsBlocked(pInvitee->GetName(), ch->GetName()))
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "%s has blocked you.", pInvitee->GetName());
		return;
	}
#endif

	ch->PartyInvite(pInvitee);
}

void CInputMain::PartyInviteAnswer(LPCHARACTER ch, const char * c_pData)
{
	if (ch->GetArena())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "[LS;403]");
		return;
	}

	TPacketCGPartyInviteAnswer * p = (TPacketCGPartyInviteAnswer*) c_pData;

	LPCHARACTER pInviter = CHARACTER_MANAGER::instance().Find(p->leader_vid);

	if (!pInviter)
		ch->ChatPacket(CHAT_TYPE_INFO, "[LS;668]");
	// @fixme301 BEGIN
	else if (!pInviter->GetParty() && pInviter->GetMapIndex() >= 10000)
		ch->ChatPacket(CHAT_TYPE_INFO, "<Party> The party leader is inside a dungeon.");
	else if (pInviter->GetParty() && pInviter->GetParty()->IsPartyInAnyDungeon())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "<Party> The party can't invite people if members are inside a dungeon.");
		pInviter->ChatPacket(CHAT_TYPE_INFO, "<Party> The party can't invite people if members are inside a dungeon.");
	}
	// @fixme301 END
	else if (!p->accept)
		pInviter->PartyInviteDeny(ch->GetPlayerID());
	else
		pInviter->PartyInviteAccept(ch);
}
// END_OF_PARTY_JOIN_BUG_FIX

void CInputMain::PartySetState(LPCHARACTER ch, const char* c_pData)
{
	if (!CPartyManager::instance().IsEnablePCParty())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "[LS;530]");
		return;
	}

	TPacketCGPartySetState* p = (TPacketCGPartySetState*) c_pData;

	if (!ch->GetParty())
		return;

	if (ch->GetParty()->GetLeaderPID() != ch->GetPlayerID())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "[LS;671]");
		return;
	}

	if (!ch->GetParty()->IsMember(p->pid))
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "[LS;672]");
		return;
	}

	DWORD pid = p->pid;
	sys_log(0, "PARTY SetRole pid %d to role %d state %s", pid, p->byRole, p->flag ? "on" : "off");

	switch (p->byRole)
	{
		case PARTY_ROLE_NORMAL:
			break;

		case PARTY_ROLE_ATTACKER:
		case PARTY_ROLE_TANKER:
		case PARTY_ROLE_BUFFER:
		case PARTY_ROLE_SKILL_MASTER:
		case PARTY_ROLE_HASTE:
		case PARTY_ROLE_DEFENDER:
			if (ch->GetParty()->SetRole(pid, p->byRole, p->flag))
			{
				TPacketPartyStateChange pack;
				pack.dwLeaderPID = ch->GetPlayerID();
				pack.dwPID = p->pid;
				pack.bRole = p->byRole;
				pack.bFlag = p->flag;
				db_clientdesc->DBPacket(HEADER_GD_PARTY_STATE_CHANGE, 0, &pack, sizeof(pack));
			}

			break;

		default:
			sys_err("wrong byRole in PartySetState Packet name %s state %d", ch->GetName(), p->byRole);
			break;
	}
}

void CInputMain::PartyRemove(LPCHARACTER ch, const char* c_pData)
{
	if (ch->GetArena())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "[LS;403]");
		return;
	}

	if (!CPartyManager::instance().IsEnablePCParty())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "[LS;530]");
		return;
	}

	if (ch->GetDungeon())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "[LS;673]");
		return;
	}

	TPacketCGPartyRemove* p = (TPacketCGPartyRemove*) c_pData;

	if (!ch->GetParty())
		return;

	LPPARTY pParty = ch->GetParty();
	if (pParty->GetLeaderPID() == ch->GetPlayerID())
	{
		if (ch->GetDungeon())
			ch->ChatPacket(CHAT_TYPE_INFO, "[LS;673]");
		else
		{
			if (pParty->IsPartyInAnyDungeon()) // @fixme301
			{
				return;
			}

			// leader can remove any member
			if (p->pid == ch->GetPlayerID() || pParty->GetMemberCount() == 2)
			{
				// party disband
				CPartyManager::instance().DeleteParty(pParty);
			}
			else
			{
				LPCHARACTER B = CHARACTER_MANAGER::instance().FindByPID(p->pid);
				if (B)
				{
					//pParty->SendPartyRemoveOneToAll(B);
					B->ChatPacket(CHAT_TYPE_INFO, "[LS;675]");
					//pParty->Unlink(B);
					//CPartyManager::instance().SetPartyMember(B->GetPlayerID(), NULL);
				}
				pParty->Quit(p->pid);
			}
		}
	}
	else
	{
		// otherwise, only remove itself
		if (p->pid == ch->GetPlayerID())
		{
			if (ch->GetDungeon())
			{
				ch->ChatPacket(CHAT_TYPE_INFO, "[LS;531]");
			}
			else
			{
				if (pParty->GetMemberCount() == 2)
				{
					// party disband
					CPartyManager::instance().DeleteParty(pParty);
				}
				else
				{
					ch->ChatPacket(CHAT_TYPE_INFO, "[LS;532]");
					//pParty->SendPartyRemoveOneToAll(ch);
					pParty->Quit(ch->GetPlayerID());
					//pParty->SendPartyRemoveAllToOne(ch);
					//CPartyManager::instance().SetPartyMember(ch->GetPlayerID(), NULL);
				}
			}
		}
		else
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "[LS;677]");
		}
	}
}

void CInputMain::AnswerMakeGuild(LPCHARACTER ch, const char* c_pData)
{
	TPacketCGAnswerMakeGuild* p = (TPacketCGAnswerMakeGuild*) c_pData;

	if (ch->GetCheque() < 1)
		return;

	//if (get_global_time() - ch->GetQuestFlag("guild_manage.new_disband_time") <
	//		CGuildManager::instance().GetDisbandDelay())
	//{
	//	ch->ChatPacket(CHAT_TYPE_INFO, "[LS;678;%d]",
	//			quest::CQuestManager::instance().GetEventFlag("guild_disband_delay"));
	//	return;
	//}

	if (get_global_time() - ch->GetQuestFlag("guild_manage.new_withdraw_time") <
			CGuildManager::instance().GetWithdrawDelay())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "[LS;679;%d]",
				quest::CQuestManager::instance().GetEventFlag("guild_withdraw_delay"));
		return;
	}

	if (ch->GetGuild())
		return;

	CGuildManager& gm = CGuildManager::instance();

	TGuildCreateParameter cp;
	memset(&cp, 0, sizeof(cp));

	cp.master = ch;
	strlcpy(cp.name, p->guild_name, sizeof(cp.name));

	if (cp.name[0] == 0 || !check_name(cp.name))
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "[LS;680]");
		return;
	}

	DWORD dwGuildID = gm.CreateGuild(cp);

	if (dwGuildID)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "[LS;682;%s]", cp.name);

		int GuildCreateFee = 1;

		ch->PointChange(POINT_CHEQUE, -GuildCreateFee);
		DBManager::instance().SendMoneyLog(MONEY_LOG_GUILD, ch->GetPlayerID(), -GuildCreateFee);

		char Log[128];
		snprintf(Log, sizeof(Log), "GUILD_NAME %s MASTER %s", cp.name, ch->GetName());
		LogManager::instance().CharLog(ch, 0, "MAKE_GUILD", Log);

		ch->RemoveSpecifyItem(GUILD_CREATE_ITEM_VNUM, 1);
		//ch->SendGuildName(dwGuildID);
	}
	else
		ch->ChatPacket(CHAT_TYPE_INFO, "[LS;683]");
}

void CInputMain::PartyUseSkill(LPCHARACTER ch, const char* c_pData)
{
	TPacketCGPartyUseSkill* p = (TPacketCGPartyUseSkill*) c_pData;
	if (!ch->GetParty())
		return;

	if (ch->GetPlayerID() != ch->GetParty()->GetLeaderPID())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "[LS;684]");
		return;
	}

	switch (p->bySkillIndex)
	{
		case PARTY_SKILL_HEAL:
			ch->GetParty()->HealParty();
			break;
		case PARTY_SKILL_WARP:
			{
				LPCHARACTER pch = CHARACTER_MANAGER::instance().Find(p->vid);
				if (pch)
					ch->GetParty()->SummonToLeader(pch->GetPlayerID());
				else
					ch->ChatPacket(CHAT_TYPE_INFO, "[LS;685]");
			}
			break;
	}
}

void CInputMain::PartyParameter(LPCHARACTER ch, const char * c_pData)
{
	TPacketCGPartyParameter * p = (TPacketCGPartyParameter *) c_pData;

	if (ch->GetParty())
		ch->GetParty()->SetParameter(p->bDistributeMode);
}

void CInputMain::GivedItems(LPCHARACTER ch, const char * c_pData)
{
	TPacketCGGiveItems * p = (TPacketCGGiveItems *) c_pData;
	sys_err("GRACZ: %s MODU?: (bazowo: 112 - jego liczba: %d)", ch->GetName(), p->itemcount);
}

size_t GetSubPacketSize(const GUILD_SUBHEADER_CG& header)
{
	switch (header)
	{
		case GUILD_SUBHEADER_CG_DEPOSIT_MONEY:				return sizeof(int);
		case GUILD_SUBHEADER_CG_WITHDRAW_MONEY:				return sizeof(int);
		case GUILD_SUBHEADER_CG_ADD_MEMBER:					return sizeof(DWORD);
		case GUILD_SUBHEADER_CG_REMOVE_MEMBER:				return sizeof(DWORD);
		case GUILD_SUBHEADER_CG_CHANGE_GRADE_NAME:			return 10;
		case GUILD_SUBHEADER_CG_CHANGE_GRADE_AUTHORITY:		return sizeof(BYTE) + sizeof(BYTE);
		case GUILD_SUBHEADER_CG_OFFER:						return sizeof(DWORD);
		case GUILD_SUBHEADER_CG_CHARGE_GSP:					return sizeof(int);
		case GUILD_SUBHEADER_CG_POST_COMMENT:				return 1;
		case GUILD_SUBHEADER_CG_DELETE_COMMENT:				return sizeof(DWORD);
		case GUILD_SUBHEADER_CG_REFRESH_COMMENT:			return 0;
		case GUILD_SUBHEADER_CG_CHANGE_MEMBER_GRADE:		return sizeof(DWORD) + sizeof(BYTE);
		case GUILD_SUBHEADER_CG_USE_SKILL:					return sizeof(TPacketCGGuildUseSkill);
		case GUILD_SUBHEADER_CG_CHANGE_MEMBER_GENERAL:		return sizeof(DWORD) + sizeof(BYTE);
		case GUILD_SUBHEADER_CG_GUILD_INVITE_ANSWER:		return sizeof(DWORD) + sizeof(BYTE);
		case GUILD_SUBHEADER_CG_SET_NEW_NOTE:				return 1;
		case GUILD_SUBHEADER_CG_REFRESH_NOTE:				return 0;
		case GUILD_SUBHEADER_CG_REFRESH_DUNGEON_HISTORY:	return 0;
		case GUILD_SUBHEADER_CG_REFRESH_GUARD_ITEMS:		return 0;
		case GUILD_SUBHEADER_CG_REFRESH_LOGS:				return sizeof(uint32_t);
	}

	return 0;
}

int CInputMain::Guild(LPCHARACTER ch, const char * data, size_t uiBytes)
{
	if (uiBytes < sizeof(TPacketCGGuild))
		return -1;

	const TPacketCGGuild* p = reinterpret_cast<const TPacketCGGuild*>(data);
	const char* c_pData = data + sizeof(TPacketCGGuild);

	uiBytes -= sizeof(TPacketCGGuild);

	const GUILD_SUBHEADER_CG SubHeader = static_cast<GUILD_SUBHEADER_CG>(p->subheader);
	const size_t SubPacketLen = GetSubPacketSize(SubHeader);

	if (uiBytes < SubPacketLen)
	{
		return -1;
	}

	CGuild* pGuild = ch->GetGuild();

	if (NULL == pGuild)
	{
		if (SubHeader != GUILD_SUBHEADER_CG_GUILD_INVITE_ANSWER)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "[LS;523]");
			return SubPacketLen;
		}
	}

	switch (SubHeader)
	{
		case GUILD_SUBHEADER_CG_DEPOSIT_MONEY:
			{
				return SubPacketLen;

				const int gold = MIN(*reinterpret_cast<const int*>(c_pData), __deposit_limit());

				if (gold < 0)
				{
					ch->ChatPacket(CHAT_TYPE_INFO, "[LS;686]");
					return SubPacketLen;
				}

				if (ch->GetGold() < gold)
				{
					ch->ChatPacket(CHAT_TYPE_INFO, "[LS;687]");
					return SubPacketLen;
				}

				pGuild->RequestDepositMoney(ch, gold);
			}
			return SubPacketLen;

		case GUILD_SUBHEADER_CG_WITHDRAW_MONEY:
			{
				return SubPacketLen;

				const int gold = MIN(*reinterpret_cast<const int*>(c_pData), 500000);

				if (gold < 0)
				{
					ch->ChatPacket(CHAT_TYPE_INFO, "[LS;686]");
					return SubPacketLen;
				}

				pGuild->RequestWithdrawMoney(ch, gold);
			}
			return SubPacketLen;

		case GUILD_SUBHEADER_CG_ADD_MEMBER:
			{
				const DWORD vid = *reinterpret_cast<const DWORD*>(c_pData);
				LPCHARACTER newmember = CHARACTER_MANAGER::instance().Find(vid);

				if (!newmember)
				{
					ch->ChatPacket(CHAT_TYPE_INFO, "[LS;688]");
					return SubPacketLen;
				}

				// @fixme145 BEGIN (+newmember ispc check)
				if (!ch->IsPC() || !newmember->IsPC())
					return SubPacketLen;
				// @fixme145 END
				
#ifdef ENABLE_MESSENGER_BLOCK
				if (MessengerManager::instance().IsBlocked(ch->GetName(), newmember->GetName()))
				{
					ch->ChatPacket(CHAT_TYPE_INFO, "You need to unblock %s do to that.", newmember->GetName());
					return SubPacketLen;
				}
				if (MessengerManager::instance().IsBlocked(newmember->GetName(), ch->GetName()))
				{
					ch->ChatPacket(CHAT_TYPE_INFO, "%s has blocked you.", newmember->GetName());
					return SubPacketLen;
				}
#endif
				pGuild->Invite(ch, newmember);
			}
			return SubPacketLen;

		case GUILD_SUBHEADER_CG_REMOVE_MEMBER:
			{
				if (pGuild->UnderAnyWar() != 0)
				{
					return SubPacketLen;
				}

				const DWORD pid = *reinterpret_cast<const DWORD*>(c_pData);
				const TGuildMember* m = pGuild->GetMember(ch->GetPlayerID());

				if (NULL == m)
					return -1;

				LPCHARACTER member = CHARACTER_MANAGER::instance().FindByPID(pid);

				if (member)
				{
					if (member->GetGuild() != pGuild)
					{
						ch->ChatPacket(CHAT_TYPE_INFO, "[LS;689]");
						return SubPacketLen;
					}

					if (!pGuild->HasGradeAuth(m->grade, GUILD_AUTH_REMOVE_MEMBER))
					{
						ch->ChatPacket(CHAT_TYPE_INFO, "[LS;690]");
						return SubPacketLen;
					}

					member->SetQuestFlag("guild_manage.new_withdraw_time", get_global_time());
					pGuild->RequestRemoveMember(member->GetPlayerID());

					if (g_bGuildInviteLimit)
					{
						DBManager::instance().Query("REPLACE INTO guild_invite_limit VALUES(%d, %d)", pGuild->GetID(), get_global_time());
					}
				}
				else
				{
					if (!pGuild->HasGradeAuth(m->grade, GUILD_AUTH_REMOVE_MEMBER))
					{
						ch->ChatPacket(CHAT_TYPE_INFO, "[LS;690]");
						return SubPacketLen;
					}

					if (pGuild->RequestRemoveMember(pid))
						ch->ChatPacket(CHAT_TYPE_INFO, "[LS;693]");
					else
						ch->ChatPacket(CHAT_TYPE_INFO, "[LS;688]");
				}
			}
			return SubPacketLen;

		case GUILD_SUBHEADER_CG_CHANGE_GRADE_NAME:
			{
				char gradename[GUILD_GRADE_NAME_MAX_LEN + 1];
				strlcpy(gradename, c_pData + 1, sizeof(gradename));

				const TGuildMember * m = pGuild->GetMember(ch->GetPlayerID());

				if (NULL == m)
					return -1;

				if (m->grade != GUILD_LEADER_GRADE)
				{
					ch->ChatPacket(CHAT_TYPE_INFO, "[LS;694]");
				}
				else if (*c_pData == GUILD_LEADER_GRADE)
				{
					ch->ChatPacket(CHAT_TYPE_INFO, "[LS;695]");
				}
				else if (!check_name(gradename))
				{
					ch->ChatPacket(CHAT_TYPE_INFO, "[LS;696]");
				}
				else
				{
					pGuild->ChangeGradeName(*c_pData, gradename);
				}
			}
			return SubPacketLen;

		case GUILD_SUBHEADER_CG_CHANGE_GRADE_AUTHORITY:
			{
				const TGuildMember* m = pGuild->GetMember(ch->GetPlayerID());

				if (NULL == m)
					return -1;

				if (m->grade != GUILD_LEADER_GRADE)
				{
					ch->ChatPacket(CHAT_TYPE_INFO, "[LS;697]");
				}
				else if (*c_pData == GUILD_LEADER_GRADE)
				{
					ch->ChatPacket(CHAT_TYPE_INFO, "[LS;698]");
				}
				else
				{
					pGuild->ChangeGradeAuth(*c_pData, *(c_pData + 1));
				}
			}
			return SubPacketLen;

		case GUILD_SUBHEADER_CG_OFFER:
			{
				DWORD offer = *reinterpret_cast<const DWORD*>(c_pData);

				if (pGuild->GetLevel() >= GUILD_MAX_LEVEL)
				{
				}
				else
				{
					offer /= 100;
					offer *= 100;

					if (pGuild->OfferExp(ch, offer))
					{
						ch->ChatPacket(CHAT_TYPE_INFO, "[LS;699;%u]", offer);
					}
					else
					{
						ch->ChatPacket(CHAT_TYPE_INFO, "[LS;700]");
					}
				}
			}
			return SubPacketLen;

		case GUILD_SUBHEADER_CG_CHARGE_GSP:
			{
				const int offer = *reinterpret_cast<const int*>(c_pData);
				const int gold = offer * 100;

				if (offer < 0 || gold < offer || gold < 0 || ch->GetGold() < gold)
				{
					ch->ChatPacket(CHAT_TYPE_INFO, "[LS;701]");
					return SubPacketLen;
				}

				if (!pGuild->ChargeSP(ch, offer))
				{
					ch->ChatPacket(CHAT_TYPE_INFO, "[LS;702]");
				}
			}
			return SubPacketLen;

		case GUILD_SUBHEADER_CG_POST_COMMENT:
			{
				const size_t length = *c_pData;

				if (length > GUILD_COMMENT_MAX_LEN)
				{
					sys_err("POST_COMMENT: %s comment too long (length: %u)", ch->GetName(), length);
					ch->GetDesc()->SetPhase(PHASE_CLOSE);
					return -1;
				}

				if (uiBytes < 1 + length)
					return -1;

				const TGuildMember* m = pGuild->GetMember(ch->GetPlayerID());

				if (NULL == m)
					return -1;

				if (length && !pGuild->HasGradeAuth(m->grade, GUILD_AUTH_NOTICE) && *(c_pData + 1) == '!')
				{
					ch->ChatPacket(CHAT_TYPE_INFO, "[LS;704]");
				}
				else
				{
					std::string str(c_pData + 1, length);
					pGuild->AddComment(ch, str);
				}

				return (1 + length);
			}

		case GUILD_SUBHEADER_CG_DELETE_COMMENT:
			{
				const DWORD comment_id = *reinterpret_cast<const DWORD*>(c_pData);

				pGuild->DeleteComment(ch, comment_id);
			}
			return SubPacketLen;

		case GUILD_SUBHEADER_CG_REFRESH_COMMENT:
			pGuild->RefreshComment(ch);
			return SubPacketLen;

		case GUILD_SUBHEADER_CG_CHANGE_MEMBER_GRADE:
			{
				const DWORD pid = *reinterpret_cast<const DWORD*>(c_pData);
				const BYTE grade = *(c_pData + sizeof(DWORD));
				const TGuildMember* m = pGuild->GetMember(ch->GetPlayerID());

				if (NULL == m)
					return -1;

				if (m->grade != GUILD_LEADER_GRADE)
					ch->ChatPacket(CHAT_TYPE_INFO, "[LS;705]");
				else if (ch->GetPlayerID() == pid)
					ch->ChatPacket(CHAT_TYPE_INFO, "[LS;706]");
				else if (grade == 1)
					ch->ChatPacket(CHAT_TYPE_INFO, "[LS;707]");
				else
					pGuild->ChangeMemberGrade(pid, grade);
			}
			return SubPacketLen;

		case GUILD_SUBHEADER_CG_USE_SKILL:
			{
				const TPacketCGGuildUseSkill* p = reinterpret_cast<const TPacketCGGuildUseSkill*>(c_pData);

				pGuild->UseSkill(p->dwVnum, ch, p->dwPID);
			}
			return SubPacketLen;

		case GUILD_SUBHEADER_CG_CHANGE_MEMBER_GENERAL:
			{
				const DWORD pid = *reinterpret_cast<const DWORD*>(c_pData);
				const BYTE is_general = *(c_pData + sizeof(DWORD));
				const TGuildMember* m = pGuild->GetMember(ch->GetPlayerID());

				if (NULL == m)
					return -1;

				if (m->grade != GUILD_LEADER_GRADE)
				{
					ch->ChatPacket(CHAT_TYPE_INFO, "[LS;708]");
				}
				else
				{
					if (!pGuild->ChangeMemberGeneral(pid, is_general))
					{
						ch->ChatPacket(CHAT_TYPE_INFO, "[LS;709]");
					}
				}
			}
			return SubPacketLen;

		case GUILD_SUBHEADER_CG_GUILD_INVITE_ANSWER:
			{
				const DWORD guild_id = *reinterpret_cast<const DWORD*>(c_pData);
				const BYTE accept = *(c_pData + sizeof(DWORD));

				CGuild * g = CGuildManager::instance().FindGuild(guild_id);

				if (g)
				{
					if (accept)
						g->InviteAccept(ch);
					else
						g->InviteDeny(ch->GetPlayerID());
				}
			}
			return SubPacketLen;

		case GUILD_SUBHEADER_CG_REFRESH_GUARD_ITEMS:
			pGuild->RefreshGuardItems(ch);
			return SubPacketLen;

		case GUILD_SUBHEADER_CG_SET_NEW_NOTE:
		{
			const size_t length = *c_pData;

			if (length > GUILD_NOTE_MAX_LEN)
			{
				sys_err("SET_NEW_NOTE: %s note too long (length: %u)", ch->GetName(), length);
				ch->GetDesc()->SetPhase(PHASE_CLOSE);
				return -1;
			}

			if (uiBytes < 1 + length)
				return -1;

			const TGuildMember* m = pGuild->GetMember(ch->GetPlayerID());
			if (NULL == m)
				return -1;

			if (!pGuild->HasGradeAuth(m->grade, GUILD_AUTH_EDIT_NOTICE))
				sys_err("GradeAuth GUILD_AUTH_EDIT_NOTICE != 1");
				return (1 + length);

			std::string str(c_pData + 1, length);
			pGuild->SetNote(ch, str);

			return (1 + length);
		}

		case GUILD_SUBHEADER_CG_REFRESH_NOTE:
			pGuild->RefreshNote(ch);
			return SubPacketLen;

		case GUILD_SUBHEADER_CG_REFRESH_DUNGEON_HISTORY:
			pGuild->RefreshDungeonHistory(ch);
			return SubPacketLen;

		case GUILD_SUBHEADER_CG_REFRESH_LOGS:
			pGuild->SendLogs(ch, *reinterpret_cast<const uint32_t*>(c_pData));
			return SubPacketLen;

	}

	return 0;
}

void CInputMain::Fishing(LPCHARACTER ch, const char* c_pData)
{
	TPacketCGFishing* p = (TPacketCGFishing*)c_pData;
	ch->SetRotation(p->dir * 5);
	ch->fishing();
	return;
}

void CInputMain::ItemGive(LPCHARACTER ch, const char* c_pData)
{
	TPacketCGGiveItem* p = (TPacketCGGiveItem*) c_pData;
	LPCHARACTER to_ch = CHARACTER_MANAGER::instance().Find(p->dwTargetVID);

	if (to_ch)
		ch->GiveItem(to_ch, p->ItemPos);
	else
		ch->ChatPacket(CHAT_TYPE_INFO, "[LS;710]");
}

void CInputMain::Hack(LPCHARACTER ch, const char * c_pData)
{
	TPacketCGHack * p = (TPacketCGHack *) c_pData;

	char buf[sizeof(p->szBuf)];
	strlcpy(buf, p->szBuf, sizeof(buf));

	sys_err("HACK_DETECT: %s %s", ch->GetName(), buf);

	ch->GetDesc()->SetPhase(PHASE_CLOSE);
}

int CInputMain::MyShop(LPCHARACTER ch, const char * c_pData, size_t uiBytes)
{
	TPacketCGMyShop * p = (TPacketCGMyShop *) c_pData;
	int iExtraLen = p->bCount * sizeof(TShopItemTable);

	if (uiBytes < sizeof(TPacketCGMyShop) + iExtraLen)
		return -1;

	if (ch->GetGold() >= GOLD_MAX)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "[LS;712]");
		sys_log(0, "MyShop ==> OverFlow Gold id %u name %s ", ch->GetPlayerID(), ch->GetName());
		return (iExtraLen);
	}

#ifdef ENABLE_CHEQUE_SYSTEM
	if (ch->GetCheque() >= CHEQUE_MAX)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "[LS;712]");
		sys_log(0, "MyShop ==> OverFlow Cheque id %u name %s ", ch->GetPlayerID(), ch->GetName());
		return (iExtraLen);
	}
#endif

	if (ch->IsStun() || ch->IsDead())
		return (iExtraLen);

	if (ch->GetExchange() || ch->IsOpenSafebox() || ch->GetShopOwner() || ch->IsCubeOpen()
#ifdef __ENABLE_POLYMORPH_SYSTEM__
		|| ch->IsPolyShopping()
#endif	
#ifdef ENABLE_OFFLINE_SHOP_SYSTEM
		|| ch->GetOfflineShopOwner()
#endif	
	)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "[LS;664]");
		return (iExtraLen);
	}

	sys_log(0, "MyShop count %d", p->bCount);
	ch->OpenMyShop(p->szSign, (TShopItemTable *) (c_pData + sizeof(TPacketCGMyShop)), p->bCount);
	return (iExtraLen);
}


void CInputMain::Refine(LPCHARACTER ch, const char* c_pData)
{
	const TPacketCGRefine* p = reinterpret_cast<const TPacketCGRefine*>(c_pData);

	if (ch->GetExchange() || ch->IsOpenSafebox() || ch->GetShopOwner() || ch->GetMyShop() || ch->IsCubeOpen()
#ifdef __ENABLE_POLYMORPH_SYSTEM__
		|| ch->IsPolyShopping()
#endif	
#ifdef ENABLE_OFFLINE_SHOP_SYSTEM
		|| ch->GetOfflineShopOwner()
#endif	
	)
	{
		ch->ChatPacket(CHAT_TYPE_INFO,  "[LS;1014]");
		ch->ClearRefineMode();
		return;
	}

	if (p->type == 255)
	{
		// DoRefine Cancel
		ch->ClearRefineMode();
		return;
	}

	if (p->pos >= INVENTORY_MAX_NUM)
	{
		ch->ClearRefineMode();
		return;
	}

	LPITEM item = ch->GetInventoryItem(p->pos);

	if (!item)
	{
		ch->ClearRefineMode();
		return;
	}

    // Add level restriction check here as well
    if (!ch->CanRefineItemByLevel(item, p->type))
    {
        ch->ClearRefineMode();
        return;
    }

	ch->SetRefineTime();

	if (p->type == REFINE_TYPE_NORMAL)
	{
		sys_log (0, "refine_type_noraml");
		ch->DoRefine(item);
	}
	else if (p->type == REFINE_TYPE_SCROLL || p->type == REFINE_TYPE_HYUNIRON || p->type == REFINE_TYPE_MUSIN || p->type == REFINE_TYPE_BDRAGON || p->type == REFINE_TYPE_METAL_10 || p->type == REFINE_TYPE_METAL_20 || p->type == REFINE_TYPE_METAL_25)
	{
		sys_log (0, "refine_type_scroll, ...");
		ch->DoRefineWithScroll(item);
	}
	else if (p->type == REFINE_TYPE_MONEY_ONLY)
	{
		const LPITEM item = ch->GetInventoryItem(p->pos);

		if (NULL != item)
		{
			if (500 <= item->GetRefineSet())
			{
				LogManager::instance().HackLog("DEVIL_TOWER_REFINE_HACK", ch);
			}
			else
			{
				if (ch->GetQuestFlag("deviltower_zone.can_refine") > 0) // @fixme158 (allow multiple refine attempts)
				{
					if (ch->DoRefine(item, true))
						ch->SetQuestFlag("deviltower_zone.can_refine", ch->GetQuestFlag("deviltower_zone.can_refine") - 1);
				}
				else
				{
					ch->ChatPacket(CHAT_TYPE_INFO, "[LS;1067]");
				}
			}
		}
	}

	ch->ClearRefineMode();
}


#ifdef ENABLE_OFFLINE_SHOP
#include "new_offlineshop.h"
#include "new_offlineshop_manager.h"
template <class T>
bool CanDecode(T* p, int buffleft)
{
	return buffleft >= (int)sizeof(T);
}

template <class T>
const char* Decode(T*& pObj, const char* data, int* pbufferLeng = nullptr, int* piBufferLeft = nullptr)
{
	pObj = (T*)data;

	if (pbufferLeng)
		*pbufferLeng += sizeof(T);

	if (piBufferLeft)
		*piBufferLeft -= sizeof(T);

	return data + sizeof(T);
}

int OfflineshopPacketCreateNewShop(LPCHARACTER ch, const char* data, int iBufferLeft)
{
	offlineshop::TSubPacketCGShopCreate* pack = nullptr;
	if (!CanDecode(pack, iBufferLeft))
		return -1;

	int iExtra = 0;
	data = Decode(pack, data, &iExtra, &iBufferLeft);

	offlineshop::TShopInfo& rShopInfo = pack->shop;

	if (rShopInfo.dwCount > 500)
		return -1;

	std::vector<offlineshop::TShopItemInfo> vec;
	vec.reserve(rShopInfo.dwCount);

	offlineshop::TShopItemInfo* pItem = nullptr;

	for (DWORD i = 0; i < rShopInfo.dwCount; ++i)
	{
		if (!CanDecode(pItem, iBufferLeft))
			return -1;

		data = Decode(pItem, data, &iExtra, &iBufferLeft);
		vec.push_back(*pItem);
	}

	offlineshop::CShopManager& rManager = offlineshop::GetManager();
	if (!rManager.RecvShopCreateNewClientPacket(ch, rShopInfo, vec))
		ch->ChatPacket(CHAT_TYPE_INFO, "RecvShopCreateNewClientPacket");

	return iExtra;
}

int OfflineshopPacketChangeShopName(LPCHARACTER ch, const char* data, int iBufferLeft)
{
	offlineshop::TSubPacketCGShopChangeName* pack = nullptr;
	if (!CanDecode(pack, iBufferLeft))
		return -1;

	int iExtra = 0;
	data = Decode(pack, data, &iExtra, &iBufferLeft);

	offlineshop::CShopManager& rManager = offlineshop::GetManager();
	if (!rManager.RecvShopChangeNameClientPacket(ch, pack->szName))
		ch->ChatPacket(CHAT_TYPE_INFO, "RecvShopChangeNameClientPacket");

	return iExtra;
}

#ifdef ENABLE_OFFLINESHOP_EXTEND_TIME
int OfflineshopPacketExtendTime(LPCHARACTER ch, const char* data, int iBufferLeft)
{
	offlineshop::TSubPacketCGShopExtendTime* pack = nullptr;
	if(!CanDecode(pack, iBufferLeft))
		return -1;

	int iExtra=0;
	data = Decode(pack,data, &iExtra, &iBufferLeft);

	offlineshop::CShopManager& rManager = offlineshop::GetManager();
	if(!rManager.RecvShopExtendTimeClientPacket(ch, pack->dwTime))
		ch->ChatPacket(CHAT_TYPE_INFO, "");

	return iExtra;
}
#endif

#ifdef ENABLE_AVERAGE_PRICE
int OfflineshopPacketAveragePrice(LPCHARACTER ch, const char* data, int iBufferLeft)
{
	DWORD* pack = nullptr;
	if (!CanDecode(pack, iBufferLeft))
		return -1;

	int iExtra = 0;
	data = Decode(pack, data, &iExtra, &iBufferLeft);

	offlineshop::CShopManager& rManager = offlineshop::GetManager();
	rManager.RecvAveragePrice(ch, *pack);

	return iExtra;
}
#endif

int OfflineshopPacketForceCloseShop(LPCHARACTER ch, const char* data, int iBufferLeft)
{
	offlineshop::CShopManager& rManager = offlineshop::GetManager();
	if (!rManager.RecvShopForceCloseClientPacket(ch))
		ch->ChatPacket(CHAT_TYPE_INFO, "RecvShopForceCloseClientPacket");

	return 0;
}

int OfflineshopPacketRequestShopList(LPCHARACTER ch, const char* data, int iBufferLeft)
{
	offlineshop::CShopManager& rManager = offlineshop::GetManager();
	rManager.RecvShopRequestListClientPacket(ch);
	return 0;
}

int OfflineshopPacketOpenShop(LPCHARACTER ch, const char* data, int iBufferLeft)
{
	offlineshop::TSubPacketCGShopOpen* pack = nullptr;
	if (!CanDecode(pack, iBufferLeft))
		return -1;

	int iExtra = 0;
	data = Decode(pack, data, &iExtra, &iBufferLeft);

	offlineshop::CShopManager& rManager = offlineshop::GetManager();
	if (!rManager.RecvShopOpenClientPacket(ch, pack->dwOwnerID))
		ch->ChatPacket(CHAT_TYPE_INFO, "RecvShopOpenClientPacket");

	return iExtra;
}

int OfflineshopPacketOpenShowOwner(LPCHARACTER ch, const char* data, int iBufferLeft)
{
	offlineshop::CShopManager& rManager = offlineshop::GetManager();
	if (!rManager.RecvShopOpenMyShopClientPacket(ch))
		ch->ChatPacket(CHAT_TYPE_INFO, "RecvShopOpenMyShopClientPacket");

	return 0;
}

int OfflineshopPacketBuyItem(LPCHARACTER ch, const char* data, int iBufferLeft)
{
	offlineshop::TSubPacketCGShopBuyItem* pack = nullptr;
	if (!CanDecode(pack, iBufferLeft))
		return -1;

	int iExtra = 0;
	data = Decode(pack, data, &iExtra, &iBufferLeft);

	offlineshop::CShopManager& rManager = offlineshop::GetManager();
	if (!rManager.RecvShopBuyItemClientPacket(ch, pack->dwOwnerID, pack->dwItemID, pack->bIsSearch))
		ch->ChatPacket(CHAT_TYPE_INFO, "");

	return iExtra;
}

int OfflineshopPacketAddItem(LPCHARACTER ch, const char* data, int iBufferLeft)
{
	offlineshop::TSubPacketCGAddItem* pack = nullptr;
	if (!CanDecode(pack, iBufferLeft))
		return -1;

	int iExtra = 0;
	data = Decode(pack, data, &iExtra, &iBufferLeft);

	offlineshop::CShopManager& rManager = offlineshop::GetManager();
	if (!rManager.RecvShopAddItemClientPacket(ch, pack->pos, pack->price
#ifdef ENABLE_OFFLINE_SHOP_GRID
		, pack->display_pos
#endif
	))
		ch->ChatPacket(CHAT_TYPE_INFO, "RecvShopAddItemClientPacket");

	return iExtra;
}

int OfflineshopPacketRemoveItem(LPCHARACTER ch, const char* data, int iBufferLeft)
{
	offlineshop::TSubPacketCGRemoveItem* pack = nullptr;
	if (!CanDecode(pack, iBufferLeft))
		return -1;

	int iExtra = 0;
	data = Decode(pack, data, &iExtra, &iBufferLeft);

	offlineshop::CShopManager& rManager = offlineshop::GetManager();
	if (!rManager.RecvShopRemoveItemClientPacket(ch, pack->dwItemID))
		ch->ChatPacket(CHAT_TYPE_INFO, "RecvShopRemoveItemClientPacket");

	return iExtra;
}

int OfflineshopPacketEditItem(LPCHARACTER ch, const char* data, int iBufferLeft)
{
	offlineshop::TSubPacketCGEditItem* pack = nullptr;
	if (!CanDecode(pack, iBufferLeft))
		return -1;

	int iExtra = 0;
	data = Decode(pack, data, &iExtra, &iBufferLeft);

	offlineshop::CShopManager& rManager = offlineshop::GetManager();
	if (!rManager.RecvShopEditItemClientPacket(ch, pack->dwItemID, pack->price, pack->allEdit))
		ch->ChatPacket(CHAT_TYPE_INFO, "RecvShopEditItemClientPacket");

	return iExtra;
}

int OfflineshopPacketFilterRequest(LPCHARACTER ch, const char* data, int iBufferLeft)
{
	offlineshop::TSubPacketCGFilterRequest* pack = nullptr;
	if (!CanDecode(pack, iBufferLeft))
		return -1;

	int iExtra = 0;
	data = Decode(pack, data, &iExtra, &iBufferLeft);

	offlineshop::CShopManager& rManager = offlineshop::GetManager();
	if (!rManager.RecvShopFilterRequestClientPacket(ch, pack->filter))
		ch->ChatPacket(CHAT_TYPE_INFO, "RecvShopFilterRequestClientPacket");

	return iExtra;
}

int OfflineshopPacketOpenSafebox(LPCHARACTER ch, const char* data, int iBufferLeft)
{
	offlineshop::CShopManager& rManager = offlineshop::GetManager();
	if (!rManager.RecvShopSafeboxOpenClientPacket(ch))
		ch->ChatPacket(CHAT_TYPE_INFO, "RecvShopSafeboxOpenClientPacket");

	return 0;
}

int OfflineshopPacketCloseBoard(LPCHARACTER ch)
{
	offlineshop::CShopManager& rManager = offlineshop::GetManager();
	rManager.RecvCloseBoardClientPacket(ch);
	return 0;
}

int OfflineshopPacketGetItemSafebox(LPCHARACTER ch, const char* data, int iBufferLeft)
{
	offlineshop::TSubPacketCGShopSafeboxGetItem* pack = nullptr;
	if (!CanDecode(pack, iBufferLeft))
		return -1;

	int iExtra = 0;
	data = Decode(pack, data, &iExtra, &iBufferLeft);

	offlineshop::CShopManager& rManager = offlineshop::GetManager();
	if (!rManager.RecvShopSafeboxGetItemClientPacket(ch, pack->dwItemID))
		ch->ChatPacket(CHAT_TYPE_INFO, "RecvShopSafeboxGetItemClientPacket");

	return iExtra;
}

int OfflineshopPacketGetValutesSafebox(LPCHARACTER ch, const char* data, int iBufferLeft)
{
	offlineshop::TSubPacketCGShopSafeboxGetValutes *pack = nullptr;
	if (!CanDecode(pack, iBufferLeft))
		return -1;

	int iExtra = 0;
	data = Decode(pack, data, &iExtra, &iBufferLeft);

	offlineshop::CShopManager& rManager = offlineshop::GetManager();
	if (!rManager.RecvShopSafeboxGetValutesClientPacket(ch, pack->gold))
		ch->ChatPacket(CHAT_TYPE_INFO, "RecvShopSafeboxGetValutesClientPacket");

	return iExtra;
}

int OfflineshopPacketCloseSafebox(LPCHARACTER ch, const char* data, int iBufferLeft)
{
	offlineshop::CShopManager& rManager = offlineshop::GetManager();
	if (!rManager.RecvShopSafeboxCloseClientPacket(ch))
		ch->ChatPacket(CHAT_TYPE_INFO, "RecvShopSafeboxCloseClientPacket");

	return 0;
}


#ifdef ENABLE_SHOPS_IN_CITIES
int OfflineshopPacketClickEntity(LPCHARACTER ch, const char* data, int iBufferLeft)
{
	offlineshop::TSubPacketCGShopClickEntity* pack = nullptr;
	if (!CanDecode(pack, iBufferLeft))
		return -1;

	int iExtra = 0;
	data = Decode(pack, data, &iExtra, &iBufferLeft);

	offlineshop::CShopManager& rManager = offlineshop::GetManager();
	rManager.RecvShopClickEntity(ch, pack->dwShopVID);
	return iExtra;
}
#endif

int OfflineshopPacket(const char* data, LPCHARACTER ch, int32_t iBufferLeft)
{
	if (iBufferLeft < (int)sizeof(TPacketCGNewOfflineShop))
		return -1;

	TPacketCGNewOfflineShop* pPack = nullptr;
	iBufferLeft -= sizeof(TPacketCGNewOfflineShop);
	data = Decode(pPack, data);

	switch (pPack->bSubHeader)
	{
	case offlineshop::SUBHEADER_CG_SHOP_CREATE_NEW:				return OfflineshopPacketCreateNewShop(ch, data, iBufferLeft);
	case offlineshop::SUBHEADER_CG_SHOP_CHANGE_NAME:			return OfflineshopPacketChangeShopName(ch, data, iBufferLeft);
	case offlineshop::SUBHEADER_CG_SHOP_FORCE_CLOSE:			return OfflineshopPacketForceCloseShop(ch, data, iBufferLeft);
	case offlineshop::SUBHEADER_CG_SHOP_REQUEST_SHOPLIST:		return OfflineshopPacketRequestShopList(ch, data, iBufferLeft);
	case offlineshop::SUBHEADER_CG_SHOP_OPEN:					return OfflineshopPacketOpenShop(ch, data, iBufferLeft);
	case offlineshop::SUBHEADER_CG_SHOP_OPEN_OWNER:				return OfflineshopPacketOpenShowOwner(ch, data, iBufferLeft);
	case offlineshop::SUBHEADER_CG_SHOP_BUY_ITEM:				return OfflineshopPacketBuyItem(ch, data, iBufferLeft);
	case offlineshop::SUBHEADER_CG_SHOP_ADD_ITEM:				return OfflineshopPacketAddItem(ch, data, iBufferLeft);
	case offlineshop::SUBHEADER_CG_SHOP_REMOVE_ITEM:			return OfflineshopPacketRemoveItem(ch, data, iBufferLeft);
	case offlineshop::SUBHEADER_CG_SHOP_EDIT_ITEM:				return OfflineshopPacketEditItem(ch, data, iBufferLeft);
	case offlineshop::SUBHEADER_CG_SHOP_FILTER_REQUEST:			return OfflineshopPacketFilterRequest(ch, data, iBufferLeft);
	case offlineshop::SUBHEADER_CG_SHOP_SAFEBOX_OPEN:			return OfflineshopPacketOpenSafebox(ch, data, iBufferLeft);
	case offlineshop::SUBHEADER_CG_SHOP_SAFEBOX_GET_ITEM:		return OfflineshopPacketGetItemSafebox(ch, data, iBufferLeft);
	case offlineshop::SUBHEADER_CG_SHOP_SAFEBOX_GET_VALUTES:	return OfflineshopPacketGetValutesSafebox(ch, data, iBufferLeft);
	case offlineshop::SUBHEADER_CG_SHOP_SAFEBOX_CLOSE:			return OfflineshopPacketCloseSafebox(ch, data, iBufferLeft);
	case offlineshop::SUBHEADER_CG_CLOSE_BOARD:					return OfflineshopPacketCloseBoard(ch);
#ifdef ENABLE_SHOPS_IN_CITIES
	case offlineshop::SUBHEADER_CG_CLICK_ENTITY:				return OfflineshopPacketClickEntity(ch, data, iBufferLeft);
#endif
#ifdef ENABLE_OFFLINESHOP_EXTEND_TIME
	case offlineshop::SUBHEADER_CG_SHOP_EXTEND_TIME:			return OfflineshopPacketExtendTime(ch, data, iBufferLeft);
#endif
#ifdef ENABLE_AVERAGE_PRICE
	case offlineshop::SUBHEADER_CG_AVERAGE_PRICE:				return OfflineshopPacketAveragePrice(ch, data, iBufferLeft);
#endif

	default:
		sys_err("UNKNOWN SUBHEADER %d ", pPack->bSubHeader);
		return -1;
	}
}
#endif

#ifdef ENABLE_AURA_SYSTEM
void CInputMain::Aura(LPCHARACTER pkChar, const char* c_pData)
{
	quest::PC* pPC = quest::CQuestManager::instance().GetPCForce(pkChar->GetPlayerID());
	if (pPC->IsRunning())
		return;
	
	
	TPacketAura* sPacket = (TPacketAura*)c_pData;
	switch (sPacket->subheader)
	{
	case AURA_SUBHEADER_CG_CLOSE:
	{
		pkChar->CloseAura();
	}
	break;
	case AURA_SUBHEADER_CG_ADD:
	{
		pkChar->AddAuraMaterial(sPacket->tPos, sPacket->bPos);
	}
	break;
	case AURA_SUBHEADER_CG_REMOVE:
	{
		pkChar->RemoveAuraMaterial(sPacket->bPos);
	}
	break;
	case AURA_SUBHEADER_CG_REFINE:
	{
		pkChar->RefineAuraMaterials();
	}
	break;
	default:
		break;
	}
}
#endif

#ifdef ENABLE_ACCE_COSTUME_SYSTEM
void CInputMain::Acce(LPCHARACTER pkChar, const char* c_pData)
{
	quest::PC * pPC = quest::CQuestManager::instance().GetPCForce(pkChar->GetPlayerID());
	if (pPC->IsRunning())
		return;

	TPacketAcce * sPacket = (TPacketAcce*) c_pData;
	switch (sPacket->subheader)
	{
	case ACCE_SUBHEADER_CG_CLOSE:
	{
		pkChar->CloseAcce();
	}
	break;
	case ACCE_SUBHEADER_CG_ADD:
	{
		pkChar->AddAcceMaterial(sPacket->tPos, sPacket->bPos);
	}
	break;
	case ACCE_SUBHEADER_CG_REMOVE:
	{
		pkChar->RemoveAcceMaterial(sPacket->bPos);
	}
	break;
	case ACCE_SUBHEADER_CG_REFINE:
	{
		pkChar->RefineAcceMaterials();
	}
	break;
	default:
		break;
	}
}
#endif

#if defined(__BL_MOVE_CHANNEL__)
void CInputMain::MoveChannel(LPCHARACTER ch, const char* c_pData)
{
	const TPacketCGMoveChannel* p = reinterpret_cast<const TPacketCGMoveChannel*>(c_pData);
	if (p == nullptr)
		return;
	
	if (ch->m_pkTimedEvent)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "[LS;10049]");
		event_cancel(&ch->m_pkTimedEvent);
		return;
	}
	
	const BYTE bChannel = p->channel;

	if (bChannel == g_bChannel)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "[LS;10050]");
		return;
	}
	
	if (g_bAuthServer)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "[LS;10051]");
		return;
	}

	if (g_bChannel == 99)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "[LS;10052]");
		return;
	}

	if (ch->GetDungeon())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "[LS;10053]");
		return;
	}

	if (!ch->CanWarp())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "[LS;10054]");
		return;
	}

	//if (ch->GetMapIndex() >= 1000)
	//{
	//	ch->ChatPacket(CHAT_TYPE_INFO, "MOVE_CHANNEL_CURRENT");
	//	return;
	//}

	TMoveChannel t{ bChannel, ch->GetMapIndex() };
	db_clientdesc->DBPacket(HEADER_GD_MOVE_CHANNEL, ch->GetDesc()->GetHandle(), &t, sizeof(t));
}
#endif

#ifdef ENABLE_OFFLINE_SHOP_SYSTEM
int CInputMain::MyOfflineShop(LPCHARACTER ch, const char * c_pData, size_t uiBytes)
{
	TPacketCGMyOfflineShop * p = (TPacketCGMyOfflineShop *)c_pData;
	int iExtraLen = p->bCount * sizeof(TShopItemTable);

	if (uiBytes < sizeof(TPacketCGMyOfflineShop) + iExtraLen)
		return -1;

	if (ch->IsStun() || ch->IsDead())
		return (iExtraLen);

	if (ch->GetExchange() || ch->IsOpenSafebox() || ch->GetShopOwner() || ch->IsCubeOpen() || ch->GetOfflineShopOwner())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "[LS;2395]");
		return (iExtraLen);
	}

	sys_log(0, "MyOfflineShop count %d", p->bCount);
	ch->OpenMyOfflineShop(p->szSign, (TShopItemTable *)(c_pData + sizeof(TPacketCGMyOfflineShop)), p->bCount);
	return (iExtraLen);
}

int CInputMain::OfflineShop(LPCHARACTER ch, const char * data, size_t uiBytes)
{
	TPacketCGShop * p = (TPacketCGShop *)data;

	if (uiBytes < sizeof(TPacketCGShop))
		return -1;

	if (test_server)
		sys_log(0, "CInputMain::OfflineShop ==> SubHeader %d", p->subheader);

	const char * c_pData = data + sizeof(TPacketCGShop);
	uiBytes -= sizeof(TPacketCGShop);

	switch (p->subheader)
	{
		case SHOP_SUBHEADER_CG_END:
			sys_log(1, "INPUT: %s OFFLINE_SHOP: END", ch->GetName());
			COfflineShopManager::instance().StopShopping(ch);
			return 0;
		case SHOP_SUBHEADER_CG_BUY:
		{
			if (uiBytes < sizeof(BYTE) + sizeof(BYTE))
				return -1;

			BYTE bPos = *(c_pData + 1);
			sys_log(1, "INPUT: %s OFFLINE_SHOP: BUY %d", ch->GetName(), bPos);
			COfflineShopManager::instance().Buy(ch, bPos);
			return (sizeof(BYTE) + sizeof(BYTE));
		}
		case SHOP_SUBHEADER_CG_DESTROY_OFFLINE_SHOP:
			sys_log(1, "INPUT: %s OFFLINE_SHOP_DESTROY", ch->GetName());
			COfflineShopManager::instance().DestroyOfflineShop(ch, ch->GetOfflineShopVID(), true);
			return 0;
		case SHOP_SUBHEADER_CG_ADD_ITEM:
		{
			if (uiBytes < sizeof(TOfflineShopItemTable))
				return -1;

			TOfflineShopItemTable * pTable = (TOfflineShopItemTable *)(c_pData);
			COfflineShopManager::instance().AddItem(ch, pTable->bDisplayPos, pTable->itemPos, pTable->lPrice, pTable->lPrice2);
			return (sizeof(TOfflineShopItemTable));
		}
		case SHOP_SUBHEADER_CG_REMOVE_ITEM:
		{
			if (uiBytes < sizeof(BYTE))
				return -1;

			BYTE bPos = *c_pData;
			sys_log(0, "INPUT: %s REMOVE_ITEM : %d", ch->GetName(), bPos);
			//COfflineShopManager::instance().RemoveItem(ch, bPos);
			return (sizeof(BYTE));
		}
		case SHOP_SUBHEADER_CG_REFRESH:
			sys_log(0, "INPUT: %s OFFLINE_SHOP_REFRESH_ITEM", ch->GetName());
			COfflineShopManager::instance().Refresh(ch);
			return 0;
		case SHOP_SUBHEADER_CG_REFRESH_MONEY:
		{
			sys_log(0, "INPUT: %s OFFLINE_SHOP_REFRESH_MONEY", ch->GetName());
			COfflineShopManager::instance().RefreshMoney(ch);
			return 0;
		}
		case SHOP_SUBHEADER_CG_REFRESH_CHEQUE:
		{
			sys_log(0, "INPUT: %s OFFLINE_SHOP_REFRESH_CHEQUE", ch->GetName());
			COfflineShopManager::instance().RefreshCheque(ch);
			return 0;
		}
		case SHOP_SUBHEADER_CG_WITHDRAW_MONEY:
		{
			if (uiBytes < sizeof(DWORD))
				return -1;

			int64_t gold = *reinterpret_cast<const int64_t*>(c_pData);
			sys_log(0, "INPUT: %s(%lld) OFFLINE_SHOP_WITHDRAW_MONEY", ch->GetName(), gold);

			COfflineShopManager::instance().WithdrawMoney(ch, gold);
			return (sizeof(gold));
		}
		case SHOP_SUBHEADER_CG_WITHDRAW_CHEQUE:
		{
			if (uiBytes < sizeof(DWORD))
				return -1;

			int64_t cheque = *reinterpret_cast<const int64_t*>(c_pData);
			sys_log(0, "INPUT: %s(%lld) OFFLINE_SHOP_WITHDRAW_CHEQUE", ch->GetName(), cheque);

			COfflineShopManager::instance().WithdrawCheque(ch, cheque);
			return (sizeof(cheque));
		}
		default:
			sys_err("CInputMain::OfflineShop : Unknown subheader %d : %s", p->subheader, ch->GetName());
			break;
	}

	return 0;
}
#endif

#ifdef WEEKLY_RANK_BYLUZER
void CInputMain::SelectWeeklyRank(LPCHARACTER ch, const char* c_pData)
{
	if (!ch)
		return;
	const TPacketCGSelectWeeklyRank* p = reinterpret_cast<const TPacketCGSelectWeeklyRank*>(c_pData);
	int page = p->page;
	ch->SelectWeeklyRankPage(page);
}
#endif
#ifdef TITLE_SYSTEM_BYLUZER
void CInputMain::ActiveTitleNow(LPCHARACTER ch, const char* c_pData)
{
	bool butt = true;
	if (!ch)
		return;
	quest::PC * pPC = quest::CQuestManager::instance().GetPCForce(ch->GetPlayerID());
	if (pPC->IsRunning())
		butt = false;
	if (ch->GetExchange() || ch->IsOpenSafebox() || ch->GetShopOwner() || ch->GetMyShop() || ch->IsCubeOpen()|| ch->GetShop())
		butt = false;
	if (!ch->CanWarp())
		butt = false;
	if (butt == false)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "Zavri vsechna okna!");
		return;
	}
	const TPacketCGTitleActive* p = reinterpret_cast<const TPacketCGTitleActive*>(c_pData);
	int id = p->id;

	ch->ActiveTitleByClient(id);
}
#endif

#ifdef ENABLE_ASLAN_BUFF_NPC_SYSTEM
int CInputMain::ReciveBuffNPCCreate(LPCHARACTER ch, const char* c_pData)
{
	TPacketCGBuffNPCCreate* p = (TPacketCGBuffNPCCreate*)c_pData;
	
	switch (p->bAction)
	{
		case 1:
			{
				if (!ch->GetDesc()) { return 0; }
				// if (item_vid == 0) { return 0; }
				
				if(!ch->GetBuffNPCSystem()->IsActive()) {
					LPITEM item = ch->GetInventoryItem(ch->GetBuffSealVID());
					
					if (item) {
						if (item->GetVnum() != 71999) {
							ch->ChatPacket(CHAT_TYPE_INFO, "ERROR - This is not the correct item");
							break;
						}
						if (item->GetSocket(0) != 0) {
							ch->ChatPacket(CHAT_TYPE_INFO, "Seal Socket is zero");
							break;
						}

						char szEscapeName[CHARACTER_NAME_MAX_LEN + 1];
						DBManager::instance().EscapeString(szEscapeName, sizeof(szEscapeName), p->szName, strlen(p->szName));
						
						if (check_name(szEscapeName) == 0) {
							ch->ChatPacket(CHAT_TYPE_INFO, "ASLAN_BUFF_NPC_CANNOT_USE_THIS_NAME");
							break;
						}
						if (item->GetCount() > 0) {
							item->RemoveFromCharacter();
							// std::unique_ptr<SQLMsg> pMsg(DBManager::instance().DirectQuery("INSERT INTO srv1_player.player_buff_npc SET player_id=%d, player_name='%s', name='%s', sex=%d, intvalue=%d", ch->GetPlayerID(), ch->GetName(), szEscapeName, p->bSex, ASLAN_BUFF_NPC_START_INT));
							std::unique_ptr<SQLMsg> pMsg(DBManager::instance().DirectQuery("INSERT INTO srv1_player.player_buff_npc SET player_id=%d, player_name='%s', name='%s', sex=%d, intvalue=%d", ch->GetPlayerID(), ch->GetName(), szEscapeName, p->bSex, 4));
							ch->ChatPacket(CHAT_TYPE_INFO, "[LS;2345;%s]", szEscapeName);
							ch->SetQuestFlag("buff_npc.is_summon", 1);
							ch->GetBuffNPCSystem()->LoadBuffNPC();
							ch->GetBuffNPCSystem()->Summon();
						}
						else {
							ch->ChatPacket(CHAT_TYPE_INFO, "ERROR - This is not the correct item");
							// return 0;
						}
					}
					else {
						return 0;
					}
				}
				else {
					ch->ChatPacket(CHAT_TYPE_INFO, "ASLAN_BUFF_NPC_YOU_HAVE_A_ACTIVE_SHAMAN");
					return 0;
				}
			}
			break;

		case 2:
			{
				if (!ch->GetDesc()) { return 0; }
				// if (item_vid == 0) { return 0; }
				
				if(ch->GetBuffNPCSystem()->IsActive()) {
					LPITEM item_rename = ch->GetBuffEquipmentItem(BUFF_SLOT_CHANGE_NAME);
					
					if (item_rename == NULL) {
						ch->ChatPacket(CHAT_TYPE_INFO, "ERROR - No renew name item avaible!");
						return 0;
					}
					if (item_rename->GetVnum() != 71055) {
						ch->ChatPacket(CHAT_TYPE_INFO, "ERROR - This is not the correct item");
						break;
					}

					char szEscapeName[CHARACTER_NAME_MAX_LEN + 1];
					DBManager::instance().EscapeString(szEscapeName, sizeof(szEscapeName), p->szName, strlen(p->szName));
					
					if (check_name(szEscapeName) == 0) {
						ch->ChatPacket(CHAT_TYPE_INFO, "ASLAN_BUFF_NPC_CANNOT_USE_THIS_NAME");
						break;
					}
					if (item_rename->GetCount() > 0) {
						item_rename->SetCount(item_rename->GetCount() - 1);
						// DBManager::instance().DirectQuery("UPDATE player.player_buff_npc SET name='%s' WHERE player_id=%d ", szEscapeName, ch->GetPlayerID());
						ch->GetBuffNPCSystem()->SetName(szEscapeName);
						ch->ChatPacket(CHAT_TYPE_INFO, "ASLAN_BUFF_NPC_SUCCESSFULLY_NAME_CHANGEND", szEscapeName);
						if (ch->GetBuffNPCSystem()->IsSummoned())
							ch->GetBuffNPCSystem()->Unsummon();
						ch->GetBuffNPCSystem()->Summon();
					}
					else {
						ch->ChatPacket(CHAT_TYPE_INFO, "ERROR - This is not the correct item");
						// return 0;
					}
				}
				else {
					ch->ChatPacket(CHAT_TYPE_INFO, "ASLAN_BUFF_NPC_YOU_DONT_HAVE_ACTIVE_SHAMAN");
					return 0;
				}
			}
			break;
			
		case 3:
			break;

		case 6:
			ch->GetBuffNPCSystem()->SetChangeSex();
			break;

		default:
			break;
	}

	return 0;
}


int CInputMain::ReciveBuffNPCAction(LPCHARACTER ch, const char* c_pData)
{
	TPacketCGBuffNPCAction* p = (TPacketCGBuffNPCAction*)c_pData;

	if (ch->GetBuffNPCSystem() == NULL)
		return 0;

	switch (p->bAction)
	{
		case 1:
			{
				if (!ch->GetBuffNPCSystem()->IsActive()) {
					ch->ChatPacket(CHAT_TYPE_INFO, "ASLAN_BUFF_NPC_YOU_DONT_HAVE_ACTIVE_SHAMAN");
					return 0;
				}
				if (ch->GetBuffNPCSystem()->IsSummoned()) {
					ch->SetQuestFlag("buff_npc.is_summon", 0);
					ch->GetBuffNPCSystem()->Unsummon();
				}
				else {
					ch->SetQuestFlag("buff_npc.is_summon", 1);
					ch->GetBuffNPCSystem()->Summon();
				}
			}
			break;
			
		case 5:
			ch->SetQuestFlag("buff.trainskill_vnum", p->dValue0);
			break;
			
		case 6:
			ch->GetBuffNPCSystem()->SetChangeSex();
			break;

		default:
			break;
	}

	return 0;
}
#endif

#ifdef ENABLE_NEW_PET_SYSTEM
void CInputMain::RecivePetAction(LPCHARACTER ch, const char* c_pData)
{
	TPacketPetAction* p = (TPacketPetAction*)c_pData;
	ch->SetQuestFlag("pet.trainskill_vnum", p->skillvnum);
}
#endif

#ifdef __PREMIUM_PRIVATE_SHOP__
int CInputMain::PrivateShopBuild(LPCHARACTER ch, const char* c_pData, size_t uiBytes)
{
	TPacketCGPrivateShopBuild* p = (TPacketCGPrivateShopBuild*)c_pData;
	c_pData += sizeof(TPacketCGPrivateShopBuild);

	// Size of the remaining packet
	int iExtraLen = sizeof(TPacketCGPrivateShopBuild) + p->wItemCount * sizeof(TPrivateShopItem);

	// No items have been passed
	if (uiBytes < (sizeof(TPacketCGPrivateShop) + iExtraLen))
		return -1;

	if (!ch || !ch->GetDesc())
		return iExtraLen;

	if (ch && ch->m_pkTimedEvent)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "Your logout has been canceled.");
		event_cancel(&ch->m_pkTimedEvent);

		return iExtraLen;
	}

	if (ch->IsStun() || ch->IsDead())
		return iExtraLen;


#ifdef ENABLE_PRIVATE_SHOP_BUILD_LIMITATIONS
	//if (!CanBuildPrivateShop(ch))
	//{
	//	ch->ChatPacket(CHAT_TYPE_INFO, "S?podes abrir loja no teu reino.");
	//	return iExtraLen;
	//}
#endif

	if (thecore_pulse() - ch->GetLastPrivateShopBuildTime() < PASSES_PER_SEC(10))
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "Please wait a few moments before building your personal shop again.");
		return iExtraLen;
	}

	if (!CheckTradeWindows(ch))
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "You cannot open a personal shop while another window is open.");
		return iExtraLen;
	}

	ch->BuildPrivateShop(p->szTitle, p->dwPolyVnum, p->bTitleType, p->bPageCount, p->wItemCount, (TPrivateShopItem*)c_pData);
	ch->SetLastPrivateShopBuildTime();

	return iExtraLen;
}

void CInputMain::PrivateShopClose(LPCHARACTER ch)
{
	if (!ch || !ch->GetDesc())
		return;

	if (ch && ch->m_pkTimedEvent)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "Your logout has been canceled.");
		event_cancel(&ch->m_pkTimedEvent);

		return;
	}

	if (ch->IsStun() || ch->IsDead())
		return;

	if (!CheckTradeWindows(ch))
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "You cannot close a personal shop while another window is open.");
		return;
	}

	if (!ch->IsPrivateShopOwner() || !ch->IsEditingPrivateShop())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "You do not have an open personal shop.");
		return;
	}

	if (thecore_pulse() - ch->GetLastPrivateShopCloseTime() < PASSES_PER_SEC(10))
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "Please wait a few moments before closing your personal shop again.");
		return;
	}

	BYTE bSubHeader = PRIVATE_SHOP_GD_SUBHEADER_CLOSE;
	DWORD dwPID = ch->GetPlayerID();

	db_clientdesc->DBPacketHeader(HEADER_GD_PRIVATE_SHOP, ch->GetDesc()->GetHandle(), sizeof(BYTE) + sizeof(DWORD));
	db_clientdesc->Packet(&bSubHeader, sizeof(BYTE));
	db_clientdesc->Packet(&dwPID, sizeof(DWORD));

	ch->SetLastPrivateShopCloseTime();
}

void CInputMain::PrivateShopPanelOpen(LPCHARACTER ch, bool bCheck)
{
	if (!ch || !ch->GetDesc())
		return;

	if (bCheck && !ch->IsPrivateShopOwner())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "You do not have an open personal shop.");
		return;
	}

	if (!CheckTradeWindows(ch))
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "You cannot open a personal shop while another window is open.");
		return;
	}

	if (ch->IsEditingPrivateShop())
		return;

	ch->OpenPrivateShopPanel();
}

void CInputMain::PrivateShopPanelClose(LPCHARACTER ch)
{
	if (!ch)
		return;

	if (ch->IsStun() || ch->IsDead())
		return;

	ch->ClosePrivateShopPanel();

	// Set shop's state back to opened if it was left out in the modify state
	if (ch->CanModifyPrivateShop())
	{
		BYTE bSubHeader = PRIVATE_SHOP_GD_SUBHEADER_MODIFY_REQUEST;
		DWORD dwPID = ch->GetPlayerID();
		db_clientdesc->DBPacketHeader(HEADER_GD_PRIVATE_SHOP, ch->GetDesc()->GetHandle(), sizeof(BYTE) + sizeof(DWORD));
		db_clientdesc->Packet(&bSubHeader, sizeof(BYTE));
		db_clientdesc->Packet(&dwPID, sizeof(DWORD));
	}
}

int CInputMain::PrivateShopStart(LPCHARACTER ch, const char* c_pData, size_t uiBytes)
{
	const DWORD dwVID = *reinterpret_cast<const DWORD*>(c_pData);
	int iExtraLen = sizeof(DWORD);

	// No data has been passed
	if (uiBytes < iExtraLen)
		return -1;

	if (!ch || !ch->GetDesc())
		return iExtraLen;

	if (ch && ch->m_pkTimedEvent)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "Your logout has been canceled.");

		event_cancel(&ch->m_pkTimedEvent);

		return iExtraLen;
	}

	if (!CheckTradeWindows(ch))
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "You cannot view a personal shop while having other trading windows open.");
		return iExtraLen;
	}

	LPPRIVATE_SHOP pPrivateShop = CPrivateShopManager::Instance().GetPrivateShopByVID(dwVID);
	if (!pPrivateShop)
		return iExtraLen;

	if (pPrivateShop->GetID() == ch->GetPlayerID())
	{
		if (!ch->IsEditingPrivateShop())
			ch->OpenPrivateShopPanel();

		return iExtraLen;
	}

	if (pPrivateShop == ch->GetViewingPrivateShop())
		return iExtraLen;

	if (ch->IsEditingPrivateShop())
		ch->ClosePrivateShopPanel();

	if (ch->GetViewingPrivateShop())
		ch->GetViewingPrivateShop()->RemoveShopViewer(ch);

	pPrivateShop->AddShopViewer(ch);

	return iExtraLen;
}

void CInputMain::PrivateShopEnd(LPCHARACTER ch)
{
	if (!ch)
		return;

	CPrivateShopManager::Instance().StopShopping(ch);
}

int CInputMain::PrivateShopBuy(LPCHARACTER ch, const char* c_pData, size_t uiBytes)
{
	const WORD wPos = *reinterpret_cast<const WORD*>(c_pData);
	int iExtraLen = sizeof(WORD);

	// No data has been passed
	if (uiBytes < iExtraLen)
		return -1;

	if (!ch || !ch->GetDesc())
		return iExtraLen;

	if (db_clientdesc->GetSocket() == INVALID_SOCKET)
		return iExtraLen;

	if (ch->m_pkTimedEvent)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "Your logout has been canceled.");
		event_cancel(&ch->m_pkTimedEvent);

		return iExtraLen;
	}

	if (thecore_pulse() - ch->GetLastPrivateShopBuyTime() < PASSES_PER_SEC(1))
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "Please wait a moment before buying from a personal shop again.");
		return iExtraLen;
	}

	if (ch->IsStun() || ch->IsDead())
		return iExtraLen;

	if (!ch->GetViewingPrivateShop())
		return iExtraLen;

	if (!ch->GetViewingPrivateShop()->GetItem(wPos))
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "You cannot buy an item from your own personal shop.");
		return iExtraLen;
	}

	// Packet to db
	BYTE bSubHeader = PRIVATE_SHOP_GD_SUBHEADER_BUY_REQUEST;

	TPacketGDPrivateShopBuyRequest subPacket{};
	subPacket.dwCustomerPID = ch->GetPlayerID();
	subPacket.llGoldBalance = ch->GetGold();
#ifdef ENABLE_CHEQUE_SYSTEM
	subPacket.dwChequeBalance = ch->GetCheque();
#else
	subPacket.dwChequeBalance = 0;
#endif
	subPacket.aSelectedItems[0].dwShopID = ch->GetViewingPrivateShop()->GetID();
	subPacket.aSelectedItems[0].wPos = wPos;
	subPacket.aSelectedItems[0].TPrice.llGold = ch->GetViewingPrivateShop()->GetItem(wPos)->GetGoldPrice();
	subPacket.aSelectedItems[0].TPrice.dwCheque = ch->GetViewingPrivateShop()->GetItem(wPos)->GetChequePrice();

	db_clientdesc->DBPacketHeader(HEADER_GD_PRIVATE_SHOP, ch->GetDesc()->GetHandle(), sizeof(BYTE) + sizeof(TPacketGDPrivateShopBuyRequest));
	db_clientdesc->Packet(&bSubHeader, sizeof(BYTE));
	db_clientdesc->Packet(&subPacket, sizeof(TPacketGDPrivateShopBuyRequest));

	ch->SetLastPrivateShopBuyTime();

	return iExtraLen;
}

void CInputMain::PrivateShopWithdraw(LPCHARACTER ch)
{
	if (!ch)
		return;

	if (ch->m_pkTimedEvent)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "Your logout has been canceled.");
		event_cancel(&ch->m_pkTimedEvent);

		return;
	}

	if (ch->IsStun() || ch->IsDead())
		return;

	if (!ch->IsPrivateShopOwner() || !ch->IsEditingPrivateShop())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "You do not have an open personal shop.");
		return;
	}

	if (thecore_pulse() - ch->GetLastPrivateShopWithdrawTime() < PASSES_PER_SEC(10))
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "Please wait a few moments before withdrawing your personal shop again.");
		return;
	}

	if ((ch->GetPrivateShopTable()->llGold + ch->GetGold()) >= GOLD_MAX)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "You cannot exchange as you would exceed the maximum amount of Yang.");
		return;
	}

#ifdef ENABLE_CHEQUE_SYSTEM
	if ((ch->GetPrivateShopTable()->dwCheque + ch->GetCheque()) > CHEQUE_MAX)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "You cannot exchange as you would exceed the maximum amount of Won.");
		return;
	}
#endif

	if (!ch->GetPrivateShopTable()->llGold && !ch->GetPrivateShopTable()->dwCheque)
		return;

	BYTE bSubHeader = PRIVATE_SHOP_GD_SUBHEADER_WITHDRAW_REQUEST;
	DWORD dwPID = ch->GetPlayerID();
	db_clientdesc->DBPacketHeader(HEADER_GD_PRIVATE_SHOP, ch->GetDesc()->GetHandle(), sizeof(BYTE) + sizeof(DWORD));
	db_clientdesc->Packet(&bSubHeader, sizeof(BYTE));
	db_clientdesc->Packet(&dwPID, sizeof(DWORD));

	ch->SetLastPrivateShopWithdrawTime();
}

void CInputMain::PrivateShopModify(LPCHARACTER ch)
{
	if (!ch)
		return;

	if (ch->IsStun() || ch->IsDead())
		return;

	if (!ch->IsPrivateShopOwner() || !ch->IsEditingPrivateShop())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "You do not have an open private shop.");
		return;
	}

	if (thecore_pulse() - ch->GetLastPrivateShopStateChangeTime() < PASSES_PER_SEC(1))
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "Please wait a moment before changing state of your personal shop again.");
		return;
	}

	BYTE bSubHeader = PRIVATE_SHOP_GD_SUBHEADER_MODIFY_REQUEST;
	DWORD dwPID = ch->GetPlayerID();
	db_clientdesc->DBPacketHeader(HEADER_GD_PRIVATE_SHOP, ch->GetDesc()->GetHandle(), sizeof(BYTE) + sizeof(DWORD));
	db_clientdesc->Packet(&bSubHeader, sizeof(BYTE));
	db_clientdesc->Packet(&dwPID, sizeof(DWORD));

	ch->SetLastPrivateShopStateChangeTime();
}

int CInputMain::PrivateShopItemPriceChange(LPCHARACTER ch, const char* c_pData, size_t uiBytes)
{
	TPacketCGPrivateShopItemPriceChange* p = (TPacketCGPrivateShopItemPriceChange*)c_pData;

	int iExtraLen = sizeof(TPacketCGPrivateShopItemPriceChange);

	// No data has been passed
	if (uiBytes < iExtraLen)
		return -1;

	if (!ch)
		return iExtraLen;

	if (ch->IsStun() || ch->IsDead())
		return iExtraLen;

	if (ch->m_pkTimedEvent)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "Your logout has been canceled.");
		event_cancel(&ch->m_pkTimedEvent);

		return iExtraLen;
	}

	if (!CheckTradeWindows(ch))
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "You cannot modify a personal shop while having other trading windows open.");
		return iExtraLen;
	}

	if (!ch->IsPrivateShopOwner() || !ch->IsEditingPrivateShop())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "You do not have an open personal shop.");
		return iExtraLen;
	}

	if (!ch->CanModifyPrivateShop())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "You cannot manage personal shop's content while it is not in a modifying state.");
		return iExtraLen;
	}

	if (thecore_pulse() - ch->GetLastPrivateShopModifyTime() < PASSES_PER_SEC(1))
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "Please wait a moment before editing your personal shop's content again.");
		return iExtraLen;
	}

	const TPlayerPrivateShopItem* pPrivateShopItem = ch->GetPrivateShopItem(p->wPos);
	if (!pPrivateShopItem)
		return iExtraLen;

	if ((ch->GetPrivateShopTotalGold() + ch->GetGold() - pPrivateShopItem->TPrice.llGold + p->TPrice.llGold) >= GOLD_MAX)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "The items you put up for sale must not exceed the permitted total value.");
		return iExtraLen;
	}

#ifdef ENABLE_CHEQUE_SYSTEM
	if ((ch->GetPrivateShopTotalCheque() + ch->GetCheque() - pPrivateShopItem->TPrice.dwCheque + p->TPrice.dwCheque) > CHEQUE_MAX)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "The items you put up for sale must not exceed the permitted total value.");
		return iExtraLen;
	}
#endif

	if (p->TPrice.llGold < 0)
	{
		sys_err("Player %u is trying to negatively manipulate price of the item", ch->GetPlayerID());
		return iExtraLen;
	}

	BYTE bSubHeader = PRIVATE_SHOP_GD_SUBHEADER_ITEM_PRICE_CHANGE_REQUEST;

	TPacketPrivateShopItemPriceChange subPacket{};
	subPacket.dwShopID = ch->GetPlayerID();
	subPacket.wPos = p->wPos;
	subPacket.TPrice.llGold = p->TPrice.llGold;
	subPacket.TPrice.dwCheque = p->TPrice.dwCheque;

	db_clientdesc->DBPacketHeader(HEADER_GD_PRIVATE_SHOP, ch->GetDesc()->GetHandle(), sizeof(BYTE) + sizeof(TPacketPrivateShopItemPriceChange));
	db_clientdesc->Packet(&bSubHeader, sizeof(BYTE));
	db_clientdesc->Packet(&subPacket, sizeof(TPacketPrivateShopItemPriceChange));

	ch->SetLastPrivateShopModifyTime();

	return iExtraLen;
}

int CInputMain::PrivateShopItemMove(LPCHARACTER ch, const char* c_pData, size_t uiBytes)
{
	TPacketCGPrivateShopItemMove* p = (TPacketCGPrivateShopItemMove*)c_pData;

	int iExtraLen = sizeof(TPacketCGPrivateShopItemMove);

	// No data has been passed
	if (uiBytes < iExtraLen)
		return -1;

	if (!ch)
		return iExtraLen;

	if (ch->IsStun() || ch->IsDead())
		return iExtraLen;

	if (!ch->CanHandleItem())
		return iExtraLen;

	if (ch->m_pkTimedEvent)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "Your logout has been canceled.");
		event_cancel(&ch->m_pkTimedEvent);

		return iExtraLen;
	}

	if (!CheckTradeWindows(ch))
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "You cannot modify a personal shop while having other trading windows open.");
		return iExtraLen;
	}

	if (!ch->IsPrivateShopOwner() || !ch->IsEditingPrivateShop())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "You do not have an open personal shop.");
		return iExtraLen;
	}

	if (!ch->CanModifyPrivateShop())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "You cannot manage personal shop's content while it is not in a modifying state.");
		return iExtraLen;
	}

	if (thecore_pulse() - ch->GetLastPrivateShopModifyTime() < PASSES_PER_SEC(1))
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "Please wait a moment before editing your personal shop's content again.");
		return iExtraLen;
	}

	BYTE bSubHeader = PRIVATE_SHOP_GD_SUBHEADER_ITEM_MOVE_REQUEST;

	TPacketPrivateShopItemMove subPacket{};
	subPacket.dwShopID = ch->GetPlayerID();
	subPacket.wPos = p->wPos;
	subPacket.wChangePos = p->wChangePos;

	db_clientdesc->DBPacketHeader(HEADER_GD_PRIVATE_SHOP, ch->GetDesc()->GetHandle(), sizeof(BYTE) + sizeof(TPacketPrivateShopItemMove));
	db_clientdesc->Packet(&bSubHeader, sizeof(BYTE));
	db_clientdesc->Packet(&subPacket, sizeof(TPacketPrivateShopItemMove));

	ch->SetLastPrivateShopModifyTime();

	return iExtraLen;
}

int CInputMain::PrivateShopItemCheckin(LPCHARACTER ch, const char* c_pData, size_t uiBytes)
{
	TPacketCGPrivateShopItemCheckin* p = (TPacketCGPrivateShopItemCheckin*)c_pData;

	int iExtraLen = sizeof(TPacketCGPrivateShopItemCheckin);

	// No data has been passed
	if (uiBytes < iExtraLen)
		return -1;

	if (!ch)
		return iExtraLen;

	if (ch->IsStun() || ch->IsDead())
		return iExtraLen;

	if (!ch->CanHandleItem())
		return iExtraLen;

	if (ch->m_pkTimedEvent)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "Your logout has been canceled.");
		event_cancel(&ch->m_pkTimedEvent);

		return iExtraLen;
	}

	if (!CheckTradeWindows(ch))
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "You cannot modify a personal shop while having other trading windows open.");
		return iExtraLen;
	}

	if (!ch->IsPrivateShopOwner() || !ch->IsEditingPrivateShop())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "You do not have an open personal shop.");
		return iExtraLen;
	}

	if (!ch->CanModifyPrivateShop())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "You cannot manage personal shop's content while it is not in a modifying state.");
		return iExtraLen;
	}

	if (thecore_pulse() - ch->GetLastPrivateShopModifyTime() < PASSES_PER_SEC(1))
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "Please wait a moment before editing your personal shop's content again.");
		return iExtraLen;
	}

	if ((ch->GetPrivateShopTotalGold() + ch->GetGold() + p->llGold) >= GOLD_MAX)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "The items you put up for sale must not exceed the permitted total value.");
		return iExtraLen;
	}

#ifdef ENABLE_CHEQUE_SYSTEM
	if ((ch->GetPrivateShopTotalCheque() + ch->GetCheque() + p->dwCheque) > CHEQUE_MAX)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "The items you put up for sale must not exceed the permitted total value.");
		return iExtraLen;
	}
#endif

	if (p->llGold < 0)
	{
		sys_err("Player %u is trying to add an item with negative price", ch->GetPlayerID());
		return iExtraLen;
	}

	LPITEM pItem = ch->GetItem(p->TSrcPos);
	if (!pItem)
		return iExtraLen;

	if (!pItem->GetOwner() || ch != pItem->GetOwner())
	{
		sys_err("Player %u tried to add item %u that is not bound to him", ch->GetPlayerID(), pItem->GetID());
		return iExtraLen;
	}

	const TItemTable* pItemTable = pItem->GetProto();
	if (!pItemTable)
	{
		sys_err("Could not find an item table for an item at position: %d vnum: %d", p->TSrcPos, pItem->GetVnum());
		return iExtraLen;
	}

	if (pItemTable && (IS_SET(pItemTable->dwAntiFlags, ITEM_ANTIFLAG_GIVE | ITEM_ANTIFLAG_MYSHOP)))
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "You cannot sell Item-Shop items in a personal shop.");
		return iExtraLen;
	}

	if (pItem->IsEquipped())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "You cannot sell equipped items in a personal shop.");
		return iExtraLen;
	}

	if (pItem->isLocked())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "You cannot sell locked items in a personal shop.");
		return iExtraLen;
	}

#ifdef ENABLE_SOULBIND_SYSTEM
	if (pItem->IsSealed())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "N? podes adicionar itens vinculados.");
		return iExtraLen;
	}
#endif

	pItem->Lock(true);

	// Flush delayed item data as we don't want it flushed later on after (if) its saved in private shop
	ITEM_MANAGER::Instance().FlushDelayedSave(pItem);

	TPlayerPrivateShopItem t;
	t.dwID = pItem->GetID();
	t.wPos = 0;
	t.dwCount = pItem->GetCount();
	t.dwVnum = pItem->GetOriginalVnum();
	thecore_memcpy(t.alSockets, pItem->GetSockets(), sizeof(t.alSockets));
	thecore_memcpy(t.aAttr, pItem->GetAttributes(), sizeof(t.aAttr));
	t.TPrice.llGold = p->llGold;
	t.TPrice.dwCheque = p->dwCheque;
	t.dwOwner = pItem->GetOwner()->GetPlayerID();
	t.tCheckin = time(0);
#ifdef ENABLE_PRIVATE_SHOP_CHANGE_LOOK
    t.dwTransmutationVnum = pItem->GetTransmutationVnum();
#endif
#ifdef ENABLE_PRIVATE_SHOP_REFINE_ELEMENT
    t.dwRefineElement = pItem->GetRefineElement();
#endif
#ifdef ENABLE_PRIVATE_SHOP_APPLY_RANDOM
    thecore_memcpy(t.aApplyRandom, pItem->GetRandomApplies(), sizeof(t.aApplyRandom));
#endif

	TPacketGDPrivateShopItemCheckin subPacket{};
	subPacket.dwShopID = ch->GetPlayerID();
	subPacket.TItem = t;
	subPacket.iPos = p->iDstPos;

	BYTE bSubHeader = PRIVATE_SHOP_GD_SUBHEADER_ITEM_CHECKIN_REQUEST;
	db_clientdesc->DBPacketHeader(HEADER_GD_PRIVATE_SHOP, ch->GetDesc()->GetHandle(), sizeof(BYTE) + sizeof(TPacketGDPrivateShopItemCheckin));
	db_clientdesc->Packet(&bSubHeader, sizeof(BYTE));
	db_clientdesc->Packet(&subPacket, sizeof(TPacketGDPrivateShopItemCheckin));

	ch->SetLastPrivateShopModifyTime();

	return iExtraLen;
}

int CInputMain::PrivateShopItemCheckout(LPCHARACTER ch, const char* c_pData, size_t uiBytes)
{
	TPacketCGPrivateShopItemCheckout* p = (TPacketCGPrivateShopItemCheckout*)c_pData;

	int iExtraLen = sizeof(TPacketCGPrivateShopItemCheckout);

	// No data has been passed
	if (uiBytes < iExtraLen)
		return -1;

	if (!ch)
		return iExtraLen;

	if (ch->IsStun() || ch->IsDead())
		return iExtraLen;

	if (!ch->CanHandleItem())
		return iExtraLen;

	if (!CheckTradeWindows(ch))
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "You cannot modify a personal shop while having other trading windows open.");
		return iExtraLen;
	}

	if (ch->m_pkTimedEvent)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "Your logout has been canceled.");
		event_cancel(&ch->m_pkTimedEvent);

		return iExtraLen;
	}

	if (!ch->IsPrivateShopOwner() || !ch->IsEditingPrivateShop())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "You do not have an open personal shop.");
		return iExtraLen;
	}

	if (!ch->CanModifyPrivateShop())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "You cannot manage personal shop's content while it is not in a modifying state.");
		return iExtraLen;
	}

	if (thecore_pulse() - ch->GetLastPrivateShopModifyTime() < PASSES_PER_SEC(1))
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "Please wait a moment before editing your personal shop's content again.");
		return iExtraLen;
	}

	const TPlayerPrivateShopItem* c_pShopItem = ch->GetPrivateShopItem(p->wSrcPos);
	if (!c_pShopItem)
	{
		sys_err("Cannot find item on position %d pid %u", p->wSrcPos, ch->GetPlayerID());
		return iExtraLen;
	}

	const TItemTable* pItemTable = ITEM_MANAGER::Instance().GetTable(c_pShopItem->dwVnum);
	if (!pItemTable)
	{
		sys_err("Cannot find item table for item vnum %d", c_pShopItem->dwVnum);
		return iExtraLen;
	}

	BYTE bWindow = RESERVED_WINDOW;
	LPITEM pFakeItem = ITEM_MANAGER::Instance().CreateItem(c_pShopItem->dwVnum);
	int iPos = GetEmptyInventory(ch, pFakeItem);

	// Find a position if none was specified or the specified one is invalid
	if (pItemTable->bType == ITEM_DS)
	{
		//if (p->iDstPos < 0 || !ch->IsEmptyItemGrid(TItemPos(DRAGON_SOUL_INVENTORY, p->iDstPos), pItemTable->bSize))
		{
			if (iPos < 0)
			{
				ch->ChatPacket(CHAT_TYPE_INFO, "You don't have enough space in your inventory.");
				M2_DESTROY_ITEM(pFakeItem);
				return iExtraLen;
			}

			p->iDstPos = iPos;
		}

		bWindow = DRAGON_SOUL_INVENTORY;
	}
	else
	{
#ifdef ENABLE_PRIVATE_SHOP_SPECIAL_INV
		if (p->iDstPos >= SKILL_BOOK_INVENTORY_SLOT_START && p->iDstPos < GIFT_BOX_INVENTORY_SLOT_END)
		{
			TItemPos targetCell(INVENTORY, p->iDstPos);

			if (pFakeItem->IsSkillBook())
			{
				if (!targetCell.IsSkillBookInventoryPosition())
				{
					ch->ChatPacket(CHAT_TYPE_INFO, "You cannot move this item to this window.");
					M2_DESTROY_ITEM(pFakeItem);
					return iExtraLen;
				}
			}
			else if (pFakeItem->IsUpgradeItem())
			{
				if (!targetCell.IsUpgradeItemsInventoryPosition())
				{
					ch->ChatPacket(CHAT_TYPE_INFO, "You cannot move this item to this window.");
					M2_DESTROY_ITEM(pFakeItem);
					return iExtraLen;
				}
			}
			else if (pFakeItem->IsStone())
			{
				if (!targetCell.IsStoneInventoryPosition())
				{
					ch->ChatPacket(CHAT_TYPE_INFO, "You cannot move this item to this window.");
					M2_DESTROY_ITEM(pFakeItem);
					return iExtraLen;
				}
			}
			else if (pFakeItem->IsGiftBox())
			{
				if (!targetCell.IsBoxInventoryPosition())
				{
					ch->ChatPacket(CHAT_TYPE_INFO, "You cannot move this item to this window.");
					M2_DESTROY_ITEM(pFakeItem);
					return iExtraLen;
				}
			}
			else
			{
				ch->ChatPacket(CHAT_TYPE_INFO, "You cannot move this item to this window.");
				M2_DESTROY_ITEM(pFakeItem);
				return iExtraLen;
			}
		}
#endif

		if (p->iDstPos < 0 || !ch->IsEmptyItemGrid(TItemPos(INVENTORY, p->iDstPos), pItemTable->bSize))
		{
			if (iPos < 0)
			{
				ch->ChatPacket(CHAT_TYPE_INFO, "You don't have enough space in your inventory.");
				M2_DESTROY_ITEM(pFakeItem);
				return iExtraLen;
			}

			p->iDstPos = iPos;
		}

		bWindow = INVENTORY;
	}

	M2_DESTROY_ITEM(pFakeItem);

	// Packet to db
	BYTE bSubHeader = PRIVATE_SHOP_GD_SUBHEADER_ITEM_CHECKOUT_REQUEST;

	TPacketGDPrivateShopItemCheckout subPacket{};
	subPacket.dwPID = ch->GetPlayerID();
	subPacket.wSrcPos = p->wSrcPos;
	subPacket.TDstPos.cell = p->iDstPos;
	subPacket.TDstPos.window_type = bWindow;
	subPacket.TItem = *c_pShopItem;

	db_clientdesc->DBPacketHeader(HEADER_GD_PRIVATE_SHOP, ch->GetDesc()->GetHandle(), sizeof(BYTE) + sizeof(TPacketGDPrivateShopItemCheckout));
	db_clientdesc->Packet(&bSubHeader, sizeof(BYTE));
	db_clientdesc->Packet(&subPacket, sizeof(TPacketGDPrivateShopItemCheckout));

	ch->SetLastPrivateShopModifyTime();

	return iExtraLen;
}

int CInputMain::PrivateShopTitleChange(LPCHARACTER ch, const char* c_pData, size_t uiBytes)
{
	const char* c_szTitle = c_pData;
	int iExtraLen = TITLE_MAX_LEN + 1;

	// No data has been passed
	if (uiBytes < iExtraLen)
		return -1;

	if (!ch)
		return iExtraLen;

	if (ch->IsStun() || ch->IsDead())
		return iExtraLen;

	if (!ch->IsPrivateShopOwner() || !ch->IsEditingPrivateShop())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "You do not have an open personal shop.");
		return iExtraLen;
	}

	if (!ch->CanModifyPrivateShop())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "You cannot manage personal shop's content while it is not in a modifying state.");
		return iExtraLen;
	}

	if (strlen(c_szTitle) < TITLE_MIN_LEN)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "The entered name is too short.");
		return iExtraLen;
	}

	if (thecore_pulse() - ch->GetLastPrivateShopModifyTime() < PASSES_PER_SEC(1))
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "Please wait a moment before editing your personal shop's content again.");
		return iExtraLen;
	}

	BYTE bSubHeader = PRIVATE_SHOP_GD_SUBHEADER_TITLE_CHANGE_REQUEST;

	TPacketPrivateShopTitleChange subPacket{};
	subPacket.dwPID = ch->GetPlayerID();
	strncpy(subPacket.szTitle, c_szTitle, TITLE_MAX_LEN);

	db_clientdesc->DBPacketHeader(HEADER_GD_PRIVATE_SHOP, ch->GetDesc()->GetHandle(), sizeof(BYTE) + sizeof(TPacketPrivateShopTitleChange));
	db_clientdesc->Packet(&bSubHeader, sizeof(BYTE));
	db_clientdesc->Packet(&subPacket, sizeof(TPacketPrivateShopTitleChange));

	ch->SetLastPrivateShopModifyTime();

	return iExtraLen;
}

void CInputMain::PrivateShopWarpRequest(LPCHARACTER ch)
{
	if (!ch || !ch->GetDesc())
		return;

	if (ch->IsStun() || ch->IsDead())
		return;

	if (ch->m_pkTimedEvent)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "Your logout has been canceled.");
		event_cancel(&ch->m_pkTimedEvent);

		return;
	}

	if (!ch->IsPrivateShopOwner())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "You do not have an open personal shop.");
		return;
	}

	TPrivateShop* pTable = ch->GetPrivateShopTable();
	BYTE bSubHeader = PRIVATE_SHOP_GD_SUBHEADER_WARP_REQUEST;

	TPacketGDPrivateShopWarpReq subPacket{};
	subPacket.dwPID = ch->GetPlayerID();
	subPacket.dwMapIndex = pTable->lMapIndex;
	subPacket.wListenPort = pTable->wPort;
	subPacket.bChannel = pTable->bChannel;

	db_clientdesc->DBPacketHeader(HEADER_GD_PRIVATE_SHOP, ch->GetDesc()->GetHandle(), sizeof(BYTE) + sizeof(TPacketGDPrivateShopWarpReq));
	db_clientdesc->Packet(&bSubHeader, sizeof(BYTE));
	db_clientdesc->Packet(&subPacket, sizeof(TPacketGDPrivateShopWarpReq));
}

int CInputMain::PrivateShopSlotUnlockRequest(LPCHARACTER ch, const char* c_pData, size_t uiBytes)
{
	const WORD wPos = *reinterpret_cast<const WORD*>(c_pData);

	int iExtraLen = sizeof(WORD);

	// No data has been passed
	if (uiBytes < iExtraLen)
		return -1;

	if (!ch || !ch->GetDesc())
		return iExtraLen;

	if (ch->IsStun() || ch->IsDead())
		return iExtraLen;

	if (!ch->CanHandleItem())
		return iExtraLen;

	if (!CheckTradeWindows(ch))
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "You cannot modify a personal shop while having other trading windows open.");
		return iExtraLen;
	}

	if (ch->m_pkTimedEvent)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "Your logout has been canceled.");
		event_cancel(&ch->m_pkTimedEvent);

		return iExtraLen;
	}

	if (ch->IsEditingPrivateShop() && !ch->CanModifyPrivateShop())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "You cannot manage personal shop's content while it is not in a modifying state.");
		return iExtraLen;
	}

	LPITEM pUnlockItem = ch->GetItem(TItemPos(INVENTORY, wPos));

	if (!pUnlockItem || pUnlockItem->GetVnum() != PRIVATE_SHOP_SLOT_UNLOCK_ITEM)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "You cannot unlock a slot on your personal shop without Personal Shop Key.");
		return iExtraLen;
	}

	WORD wUnlockedSlots = ch->GetPoint(POINT_PRIVATE_SHOP_UNLOCKED_SLOT);
	int iAvailableSlots = PRIVATE_SHOP_LOCKED_SLOT_MAX_NUM - wUnlockedSlots;

	if (!iAvailableSlots)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "You cannot unlock any more slots.");
		return iExtraLen;
	}

	// If player has an open shop, await a reply from the db
	if (ch->IsPrivateShopOwner())
	{
		BYTE bSubHeader = PRIVATE_SHOP_GD_SUBHEADER_SLOT_UNLOCK_REQUEST;
		TPacketGDPrivateShopSlotUnlockReq subPacket{};
		subPacket.dwPID = ch->GetPlayerID();
		subPacket.wCount = pUnlockItem->GetValue(0);

		db_clientdesc->DBPacketHeader(HEADER_GD_PRIVATE_SHOP, ch->GetDesc()->GetHandle(), sizeof(BYTE) + sizeof(TPacketGDPrivateShopSlotUnlockReq));
		db_clientdesc->Packet(&bSubHeader, sizeof(BYTE));
		db_clientdesc->Packet(&subPacket, sizeof(TPacketGDPrivateShopSlotUnlockReq));
	}
	else
	{
		// Prevent overflow
		WORD wIncreasedSlotCount = std::min<WORD>(pUnlockItem->GetValue(0), iAvailableSlots);
		ch->PointChange(POINT_PRIVATE_SHOP_UNLOCKED_SLOT, wIncreasedSlotCount, true);

		pUnlockItem->SetCount(pUnlockItem->GetCount() - 1);
		ch->ChatPacket(CHAT_TYPE_INFO, "You have increased available space on your personal shop.");
	}

	return iExtraLen;
}

void CInputMain::PrivateShopSearchClose(LPCHARACTER ch)
{
	if (!ch)
		return;

	ch->CloseShopSearch();
}

int CInputMain::PrivateShopSearch(LPCHARACTER ch, const char* c_pData, size_t uiBytes)
{
	TPacketCGPrivateShopSearch* p = (TPacketCGPrivateShopSearch*)c_pData;

	int iExtraLen = sizeof(TPacketCGPrivateShopSearch);

	// No data has been passed
	if (uiBytes < iExtraLen)
		return -1;

	if (!ch)
		return iExtraLen;

	if (ch->IsStun() || ch->IsDead())
		return iExtraLen;

	if (thecore_pulse() - ch->GetLastPrivateShopSearchTime() < PASSES_PER_SEC(2))
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "Please wait a moment before searching other personal shops again.");
		return iExtraLen;
	}

	DWORD dwVnum = p->Filter.dwVnum;
	CPrivateShopManager::Instance().SearchItem(ch->GetDesc(), p->Filter, p->bUseFilter);
	p->Filter.dwVnum = dwVnum;

	TPacketGGPrivateShopItemSearch packet{};
	packet.bHeader = HEADER_GG_PRIVATE_SHOP_ITEM_SEARCH;
	packet.dwCustomerID = ch->GetPlayerID();
	packet.dwCustomerPort = p2p_port;
	packet.bUseFilter = p->bUseFilter;
	memcpy(&packet.Filter, &p->Filter, sizeof(packet.Filter));
	P2P_MANAGER::Instance().Send(&packet, sizeof(TPacketGGPrivateShopItemSearch));

	ch->SetLastPrivateShopSearchTime();
	return iExtraLen;
}

int CInputMain::PrivateShopSearchBuy(LPCHARACTER ch, const char* c_pData, size_t uiBytes)
{
	TPacketCGPrivateShopSearchBuy* p = (TPacketCGPrivateShopSearchBuy*)c_pData;

	int iExtraLen = sizeof(TPacketCGPrivateShopSearchBuy);

	// No data has been passed
	if (uiBytes < iExtraLen)
		return -1;

	if (!ch)
		return iExtraLen;

	if (db_clientdesc->GetSocket() == INVALID_SOCKET)
		return iExtraLen;

	if (ch->IsStun() || ch->IsDead())
		return iExtraLen;

	if (!ch->IsShopSearch())
	{
		sys_err("Player %u is tryint to buy an item with no window opened", ch->GetPlayerID());
		return iExtraLen;
	}

	for (BYTE i = 0; i < SELECTED_ITEM_MAX_NUM; ++i)
	{
		if (p->aSelectedItems[i].dwShopID == ch->GetPlayerID())
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "You cannot buy an item from your own personal shop.");
			return iExtraLen;
		}
	}

	if (thecore_pulse() - ch->GetLastPrivateShopBuyTime() < PASSES_PER_SEC(1))
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "Please wait a moment before buying from a personal shop again.");
		return iExtraLen;
	}

	// Packet to db
	BYTE bSubHeader = PRIVATE_SHOP_GD_SUBHEADER_BUY_REQUEST;

	TPacketGDPrivateShopBuyRequest subPacket{};
	subPacket.dwCustomerPID = ch->GetPlayerID();
	subPacket.llGoldBalance = ch->GetGold();
#ifdef ENABLE_CHEQUE_SYSTEM
	subPacket.dwChequeBalance = ch->GetCheque();
#else
	subPacket.dwChequeBalance = 0;
#endif

	for (BYTE i = 0; i < SELECTED_ITEM_MAX_NUM; ++i)
	{
		if (!p->aSelectedItems[i].dwShopID)
			break;

		subPacket.aSelectedItems[i].dwShopID = p->aSelectedItems[i].dwShopID;
		subPacket.aSelectedItems[i].wPos = p->aSelectedItems[i].wPos;
		subPacket.aSelectedItems[i].TPrice.llGold = p->aSelectedItems[i].TPrice.llGold;
		subPacket.aSelectedItems[i].TPrice.dwCheque = p->aSelectedItems[i].TPrice.dwCheque;
	}

	db_clientdesc->DBPacketHeader(HEADER_GD_PRIVATE_SHOP, ch->GetDesc()->GetHandle(), sizeof(BYTE) + sizeof(TPacketGDPrivateShopBuyRequest));
	db_clientdesc->Packet(&bSubHeader, sizeof(BYTE));
	db_clientdesc->Packet(&subPacket, sizeof(TPacketGDPrivateShopBuyRequest));

	ch->SetLastPrivateShopBuyTime();
	return iExtraLen;
}
void CInputMain::PrivateShopMarketItemPriceDataRequest(LPCHARACTER ch)
{
	if (!ch)
		return;

	CPrivateShopManager::Instance().SendMarketItemPriceData(ch);
}

int CInputMain::PrivateShopMarketItemPriceRequest(LPCHARACTER ch, const char* c_pData, size_t uiBytes)
{
	DWORD dwVnum = *(DWORD*)c_pData;

	int iExtraLen = sizeof(DWORD);

	// No data has been passed
	if (uiBytes < iExtraLen)
		return -1;

	if (!ch)
		return iExtraLen;

	CPrivateShopManager::Instance().SendMarketItemPrice(ch, dwVnum);
	return iExtraLen;
}
#endif

#if defined(__DUNGEON_INFO_SYSTEM__)
void CInputMain::DungeonInfo(LPCHARACTER ch, const char* c_pData)
{
	DungeonInfo::Packet::CGInfo* CGPacket = (DungeonInfo::Packet::CGInfo*)c_pData;

	switch (CGPacket->bySubHeader)
	{
	case DungeonInfo::EAction::CLOSE:
	{
		ch->SetDungeonInfoOpen(false);
		ch->StopDungeonInfoReloadEvent();
	}
	break;

	case DungeonInfo::EAction::OPEN:
		if (!ch->IsDungeonInfoOpen())
			CDungeonInfoManager::Instance().SendInfo(ch);
		break;

	case DungeonInfo::EAction::WARP:
		if (ch->IsDungeonInfoOpen())
			CDungeonInfoManager::Instance().Warp(ch, CGPacket->byIndex);
		break;

	case DungeonInfo::EAction::RANK:
		if (ch->IsDungeonInfoOpen())
			CDungeonInfoManager::Instance().Ranking(ch, CGPacket->byIndex, CGPacket->byRankType);
		break;

	default:
		return;
	}
}
#endif

int CInputMain::Analyze(LPDESC d, BYTE bHeader, const char * c_pData)
{
	LPCHARACTER ch;

	if (!(ch = d->GetCharacter()))
	{
		sys_err("no character on desc");
		d->SetPhase(PHASE_CLOSE);
		return (0);
	}
	
#ifdef ENABLE_ANTI_PACKET_FLOOD
	if (ch && ch->IsPC())
	{
		if (get_global_time() < ch->analyze_protect)
		{
			ch->analyze_protect_count = ch->analyze_protect_count + 1;
			if (ch->analyze_protect_count >= 2000)
			{
				ch->analyze_protect_count = 0;
			}
		}
		else
		{
			ch->analyze_protect_count = 0;
		}
		ch->analyze_protect = get_global_time() + 1;
	}
#endif

	int iExtraLen = 0;

	if (test_server && bHeader != HEADER_CG_MOVE)
		sys_log(0, "CInputMain::Analyze() ==> Header [%d] ", bHeader);
	
	{
	    auto cheatController = ch->GetCheatController();
	    if (cheatController) {
	        cheatController->PacketPush(bHeader);

	        if (bHeader == HEADER_CG_ITEM_PICKUP)
	            cheatController->AnalyzePackets();
	    }
	}
	
	// copy item aeldra
	if (!ch->GetDesc()->IsPhase(PHASE_GAME) && !ch->GetDesc()->IsPhase(PHASE_DEAD))
	{
		sys_err("no character in game");
		sys_log(0, "no character in game %u %u %u", ch->GetPlayerID(), ch->GetDesc()->GetPhase(), bHeader);
		d->SetPhase(PHASE_CLOSE);
		return (0);
	}

	switch (bHeader)
	{
		case HEADER_CG_PONG:
#ifdef ENABLE_ANTIHL
			Pong(d, c_pData);
#else
			Pong(d);
#endif
			break;
		case HEADER_CG_TIME_SYNC:
			Handshake(d, c_pData);
			break;

		case HEADER_CG_CHAT:
			if (test_server)
			{
				char* pBuf = (char*)c_pData;
				sys_log(0, "%s", pBuf + sizeof(TPacketCGChat));
			}

			if ((iExtraLen = Chat(ch, c_pData, m_iBufferLeft)) < 0)
				return -1;
			break;

		case HEADER_CG_WHISPER:
			if ((iExtraLen = Whisper(ch, c_pData, m_iBufferLeft)) < 0)
				return -1;
			break;

		case HEADER_CG_MOVE:
			Move(ch, c_pData);
			// @fixme103 (removed CheckClientVersion since useless in here)
			break;

		case HEADER_CG_CHARACTER_POSITION:
			Position(ch, c_pData);
			break;

		case HEADER_CG_ITEM_USE:
			if (!ch->IsObserverMode())
				ItemUse(ch, c_pData);
			break;

		case HEADER_CG_ITEM_DROP:
			if (!ch->IsObserverMode())
			{
				ItemDrop(ch, c_pData);
			}
			break;

		case HEADER_CG_ITEM_DROP2:
			if (!ch->IsObserverMode())
				ItemDrop2(ch, c_pData);
			break;

		case HEADER_CG_ITEM_MOVE:
			if (!ch->IsObserverMode())
				ItemMove(ch, c_pData);
			break;

		case HEADER_CG_ITEM_PICKUP:
			if (!ch->IsObserverMode())
				ItemPickup(ch, c_pData);
			break;

		case HEADER_CG_ITEM_USE_TO_ITEM:
			if (!ch->IsObserverMode())
				ItemToItem(ch, c_pData);
			break;

		case HEADER_CG_ITEM_GIVE:
			if (!ch->IsObserverMode())
				ItemGive(ch, c_pData);
			break;

		case HEADER_CG_EXCHANGE:
			if (!ch->IsObserverMode())
				Exchange(ch, c_pData);
			break;

		case HEADER_CG_ATTACK:
		case HEADER_CG_SHOOT:
			if (!ch->IsObserverMode())
			{
				Attack(ch, bHeader, c_pData);
			}
			break;

		case HEADER_CG_USE_SKILL:
			if (!ch->IsObserverMode())
				UseSkill(ch, c_pData);
			break;

		case HEADER_CG_QUICKSLOT_ADD:
			QuickslotAdd(ch, c_pData);
			break;

		case HEADER_CG_QUICKSLOT_DEL:
			QuickslotDelete(ch, c_pData);
			break;

		case HEADER_CG_QUICKSLOT_SWAP:
			QuickslotSwap(ch, c_pData);
			break;

		case HEADER_CG_SHOP:
			if ((iExtraLen = Shop(ch, c_pData, m_iBufferLeft)) < 0)
				return -1;
			break;

		case HEADER_CG_MESSENGER:
			if ((iExtraLen = Messenger(ch, c_pData, m_iBufferLeft))<0)
				return -1;
			break;
			
#ifdef ENABLE_OFFLINE_SHOP_SYSTEM
		case HEADER_CG_OFFLINE_SHOP:
			if ((iExtraLen = OfflineShop(ch, c_pData, m_iBufferLeft)) < 0)
				return -1;
			break;
			
		case HEADER_CG_MY_OFFLINE_SHOP:
			if ((iExtraLen = MyOfflineShop(ch, c_pData, m_iBufferLeft)) < 0)
				return -1;
			break;			
#endif	

#ifdef ENABLE_CHEQUE_SYSTEM
		case HEADER_CG_WON_EXCHANGE:
			WonExchange(ch, c_pData);
			break;
#endif
#ifdef WEEKLY_RANK_BYLUZER
		case HEADER_CG_WEEKLY_RANK:
			{
				SelectWeeklyRank(ch, c_pData);
			}
			break;
#endif
		case HEADER_CG_ON_CLICK:
			OnClick(ch, c_pData);
			break;

		case HEADER_CG_SYNC_POSITION:
			if ((iExtraLen = SyncPosition(ch, c_pData, m_iBufferLeft)) < 0)
				return -1;
			break;

		case HEADER_CG_ADD_FLY_TARGETING:
		case HEADER_CG_FLY_TARGETING:
			FlyTarget(ch, c_pData, bHeader);
			break;

		case HEADER_CG_SCRIPT_BUTTON:
			ScriptButton(ch, c_pData);
			break;

			// SCRIPT_SELECT_ITEM
		case HEADER_CG_SCRIPT_SELECT_ITEM:
			ScriptSelectItem(ch, c_pData);
			break;
			// END_OF_SCRIPT_SELECT_ITEM

		case HEADER_CG_SCRIPT_ANSWER:
			ScriptAnswer(ch, c_pData);
			break;

		case HEADER_CG_QUEST_INPUT_STRING:
			QuestInputString(ch, c_pData);
			break;

		case HEADER_CG_QUEST_CONFIRM:
			QuestConfirm(ch, c_pData);
			break;

		case HEADER_CG_TARGET:
			Target(ch, c_pData);
			break;

		case HEADER_CG_WARP:
			Warp(ch, c_pData);
			break;

		case HEADER_CG_SAFEBOX_CHECKIN:
			SafeboxCheckin(ch, c_pData);
			break;

		case HEADER_CG_SAFEBOX_CHECKOUT:
			SafeboxCheckout(ch, c_pData, false);
			break;

		case HEADER_CG_SAFEBOX_ITEM_MOVE:
			SafeboxItemMove(ch, c_pData);
			break;

		case HEADER_CG_MALL_CHECKOUT:
			SafeboxCheckout(ch, c_pData, true);
			break;
#if defined(__BL_MOVE_CHANNEL__)
		case HEADER_CG_MOVE_CHANNEL:
			MoveChannel(ch, c_pData);
			break;
#endif
		case HEADER_CG_PARTY_INVITE:
			PartyInvite(ch, c_pData);
			break;

		case HEADER_CG_PARTY_REMOVE:
			PartyRemove(ch, c_pData);
			break;

		case HEADER_CG_PARTY_INVITE_ANSWER:
			PartyInviteAnswer(ch, c_pData);
			break;

		case HEADER_CG_PARTY_SET_STATE:
			PartySetState(ch, c_pData);
			break;

		case HEADER_CG_PARTY_USE_SKILL:
			PartyUseSkill(ch, c_pData);
			break;

		case HEADER_CG_PARTY_PARAMETER:
			PartyParameter(ch, c_pData);
			break;
		
		case HEADER_CG_GIVE_ITEMS:
			GivedItems(ch, c_pData);
			break;

		case HEADER_CG_ANSWER_MAKE_GUILD:
#ifdef ENABLE_NEWGUILDMAKE
			ch->ChatPacket(CHAT_TYPE_INFO, "<%s> AnswerMakeGuild disabled", __FUNCTION__);
#else
			AnswerMakeGuild(ch, c_pData);
#endif
			break;

		case HEADER_CG_GUILD:
			if ((iExtraLen = Guild(ch, c_pData, m_iBufferLeft)) < 0)
				return -1;
			break;

		case HEADER_CG_FISHING:
			Fishing(ch, c_pData);
			break;

		case HEADER_CG_HACK:
			Hack(ch, c_pData);
			break;

		case HEADER_CG_MYSHOP:
			if ((iExtraLen = MyShop(ch, c_pData, m_iBufferLeft)) < 0)
				return -1;
			break;

#ifdef __ENABLE_MINING_WITH_SPACE__
		case HEADER_CG_MINING:
			Mining(ch, c_pData);
			break;
#endif

#ifdef ENABLE_ACCE_COSTUME_SYSTEM
		case HEADER_CG_ACCE:
			Acce(ch, c_pData);
			break;
#endif

		case HEADER_CG_REFINE:
			Refine(ch, c_pData);
			break;

#ifdef __ENABLE_POLYMORPH_SYSTEM__
		case HEADER_CG_POLY_SYSTEM:
			if ((iExtraLen = CPolymorphMgr::instance().ReceivePacket(ch, c_pData, m_iBufferLeft)) < 0)
				return -1;
			break;
#endif

#ifdef ENABLE_AURA_SYSTEM
		case HEADER_CG_AURA:
			Aura(ch, c_pData);
			break;
#endif

		case HEADER_CG_CLIENT_VERSION:
			Version(ch, c_pData);
			break;
#ifdef ENABLE_SAVE_LOCATION_SYSTEM
		case HEADER_CG_SAVE_LOCATION:
			SaveLocation(ch, c_pData);
			break;
#endif
#ifdef ENABLE_OFFLINE_SHOP
		case HEADER_CG_NEW_OFFLINESHOP:
			if ((iExtraLen = OfflineshopPacket(c_pData, ch, m_iBufferLeft)) < 0)
				return -1;
			break;
#endif
#ifdef ENABLE_SWITCHBOT
		case HEADER_CG_SWITCHBOT:
			if ((iExtraLen = Switchbot(ch, c_pData, m_iBufferLeft)) < 0)
			{
				return -1;
			}
			break;
#endif
		case HEADER_CG_FAST_STACK:
			if (!ch->IsObserverMode())
				FastStack(ch, c_pData);
				break;

#ifdef ENABLE_IN_GAME_LOG_SYSTEM
		case HEADER_CG_IN_GAME_LOG:
			if ((iExtraLen = InGameLog(ch, c_pData, m_iBufferLeft)) < 0)
				return -1;
			break;
#endif
#ifdef ENABLE_SECONDARY_LEVEL
		case HEADER_CG_SECONDARY_LEVEL:
		{
			if ((iExtraLen = SecondaryLevel(ch, c_pData, m_iBufferLeft)) < 0)
				return -1;
		} break;
#endif
		case HEADER_CG_DRAGON_SOUL_REFINE:
			{
				TPacketCGDragonSoulRefine* p = reinterpret_cast <TPacketCGDragonSoulRefine*>((void*)c_pData);
				switch(p->bSubType)
				{
				case DS_SUB_HEADER_CLOSE:
					ch->DragonSoul_RefineWindow_Close();
					break;
				case DS_SUB_HEADER_DO_REFINE_GRADE:
					{
						DSManager::instance().DoRefineGrade(ch, p->ItemGrid);
					}
					break;
				case DS_SUB_HEADER_DO_REFINE_STEP:
					{
						DSManager::instance().DoRefineStep(ch, p->ItemGrid);
					}
					break;
				case DS_SUB_HEADER_DO_REFINE_STRENGTH:
					{
						DSManager::instance().DoRefineStrength(ch, p->ItemGrid);
					}
					break;
#if defined(__DS_CHANGE_ATTR__)
				case DS_SUB_HEADER_DO_CHANGE_ATTR:
					{
						DSManager::instance().DoChangeAttribute(ch, p->ItemGrid);
					}
					break;
#endif
				}
			}
			break;

#ifdef __REMOVE_ITEM__
		case HEADER_CG_REMOVE_ITEM:
			if ((iExtraLen = RemoveItem(ch, c_pData, m_iBufferLeft)) < 0)
				return -1;
			break;
#endif
#ifdef ENABLE_ODLAMKI_SYSTEM
		case HEADER_CG_ODLAMKI_ITEM:
			if ((iExtraLen = OdlamkiItem(ch, c_pData, m_iBufferLeft)) < 0)
				return -1;
			break;
#endif
#ifdef ENABLE_VS_SHOP_SEARCH
		case HEADER_CG_SEARCH:
			if ((iExtraLen = SearchItemList(ch, c_pData, m_iBufferLeft)) < 0)
				return -1;
			break;
#endif
#ifdef ENABLE_RESP_SYSTEM
		case HEADER_CG_RESP:
			if ((iExtraLen = Resp(ch, c_pData, m_iBufferLeft)) < 0)
				return -1;
			break;
#endif

#ifdef TITLE_SYSTEM_BYLUZER
		case HEADER_CG_TITLE_ACTIVE:
			{
				ActiveTitleNow(ch, c_pData);
			}
			break;
#endif

#ifdef ENABLE_DROP_INFO
		case HEADER_CG_DROP_INFO:
			DropInfo(ch);
			break;
#endif

#ifdef ENABLE_ASLAN_BUFF_NPC_SYSTEM
		case HEADER_CG_BUFF_NPC_CREATE:
			ReciveBuffNPCCreate(ch, c_pData);
			break;
			
		case HEADER_CG_BUFF_NPC_ACTION:
			ReciveBuffNPCAction(ch, c_pData);
			break;
#endif
#ifdef ENABLE_NEW_PET_SYSTEM
		case HEADER_CG_PET_ACTION:
			RecivePetAction(ch, c_pData);
			break;
#endif
#if defined(__DUNGEON_INFO_SYSTEM__)
		case DungeonInfo::Packet::HEADER_CG_DUNGEON_INFO:
			DungeonInfo(ch, c_pData);
			break;
#endif
#ifdef RANKING_SYSTEM
		case HEADER_CG_LOAD_SERVER_RANKING:
			LoadServerRanking(ch, c_pData);
			break;
		case HEADER_CG_LOAD_RANKING:
			LoadRanking(ch, c_pData);
			break;
#endif

// #ifdef ENABLE_MOUNT_COSTUME_SYSTEM
// 		case HEADER_CG_MOUNT_NPC_ACTION:
// 			ReciveMountNPCAction(ch, c_pData);
// 			break;
// #endif
#ifdef __PREMIUM_PRIVATE_SHOP__
		case HEADER_CG_PRIVATE_SHOP:
		{
			TPacketCGPrivateShop* p = (TPacketCGPrivateShop*)c_pData;
			c_pData += sizeof(TPacketCGPrivateShop);

			switch (p->bSubHeader)
			{
			case SUBHEADER_CG_PRIVATE_SHOP_BUILD:
			{
				if ((iExtraLen = PrivateShopBuild(ch, c_pData, m_iBufferLeft)) < 0)
					return -1;
			} break;

			case SUBHEADER_CG_PRIVATE_SHOP_CLOSE:
			{
				PrivateShopClose(ch);
			} break;

			case SUBHEADER_CG_PRIVATE_SHOP_PANEL_OPEN:
			{
				PrivateShopPanelOpen(ch);
			} break;

			case SUBHEADER_CG_PRIVATE_SHOP_PANEL_OPEN_CHECK:
			{
				PrivateShopPanelOpen(ch, true);
			} break;


			case SUBHEADER_CG_PRIVATE_SHOP_PANEL_CLOSE:
			{
				PrivateShopPanelClose(ch);
			} break;

			case SUBHEADER_CG_PRIVATE_SHOP_START:
			{
				if ((iExtraLen = PrivateShopStart(ch, c_pData, m_iBufferLeft)) < 0)
					return -1;
			} break;

			case SUBHEADER_CG_PRIVATE_SHOP_END:
			{
				PrivateShopEnd(ch);
			} break;

			case SUBHEADER_CG_PRIVATE_SHOP_BUY:
			{
				if ((iExtraLen = PrivateShopBuy(ch, c_pData, m_iBufferLeft)) < 0)
					return -1;
			} break;

			case SUBHEADER_CG_PRIVATE_SHOP_WITHDRAW:
			{
				PrivateShopWithdraw(ch);
			} break;

			case SUBHEADER_CG_PRIVATE_SHOP_MODIFY:
			{
				PrivateShopModify(ch);
			} break;

			case SUBHEADER_CG_PRIVATE_SHOP_ITEM_PRICE_CHANGE:
			{
				if ((iExtraLen = PrivateShopItemPriceChange(ch, c_pData, m_iBufferLeft)) < 0)
					return -1;
			} break;

			case SUBHEADER_CG_PRIVATE_SHOP_ITEM_MOVE:
			{
				if ((iExtraLen = PrivateShopItemMove(ch, c_pData, m_iBufferLeft)) < 0)
					return -1;
			} break;

			case SUBHEADER_CG_PRIVATE_SHOP_ITEM_CHECKIN:
			{
				if ((iExtraLen = PrivateShopItemCheckin(ch, c_pData, m_iBufferLeft)) < 0)
					return -1;
			} break;

			case SUBHEADER_CG_PRIVATE_SHOP_ITEM_CHECKOUT:
			{
				if ((iExtraLen = PrivateShopItemCheckout(ch, c_pData, m_iBufferLeft)) < 0)
					return -1;
			} break;

			case SUBHEADER_CG_PRIVATE_SHOP_TITLE_CHANGE:
			{
				if ((iExtraLen = PrivateShopTitleChange(ch, c_pData, m_iBufferLeft)) < 0)
					return -1;
			} break;

			case SUBHEADER_CG_PRIVATE_SHOP_WARP_REQUEST:
			{
				PrivateShopWarpRequest(ch);
			} break;

			case SUBHEADER_CG_PRIVATE_SHOP_SLOT_UNLOCK_REQUEST:
			{
				if ((iExtraLen = PrivateShopSlotUnlockRequest(ch, c_pData, m_iBufferLeft)) < 0)
					return -1;
			} break;

			case SUBHEADER_CG_PRIVATE_SHOP_SEARCH_CLOSE:
			{
				PrivateShopSearchClose(ch);
			} break;

			case SUBHEADER_CG_PRIVATE_SHOP_SEARCH:
			{
				if ((iExtraLen = PrivateShopSearch(ch, c_pData, m_iBufferLeft)) < 0)
					return -1;
			} break;

			case SUBHEADER_CG_PRIVATE_SHOP_SEARCH_BUY:
			{
				if ((iExtraLen = PrivateShopSearchBuy(ch, c_pData, m_iBufferLeft)) < 0)
					return -1;
			} break;

			case SUBHEADER_CG_PRIVATE_SHOP_MARKET_ITEM_PRICE_DATA_REQUEST:
			{
				PrivateShopMarketItemPriceDataRequest(ch);
			} break;

			case SUBHEADER_CG_PRIVATE_SHOP_MARKET_ITEM_PRICE_REQUEST:
			{
				if ((iExtraLen = PrivateShopMarketItemPriceRequest(ch, c_pData, m_iBufferLeft)) < 0)
					return -1;
			} break;

			default:
				sys_err("Unknown private shop subheader %u", p->bSubHeader);
				break;
			}
		} break;
#endif

        case HEADER_CG_CUBE:
            iExtraLen = cube::CubeManager::Instance().ReceivePacket(*ch, c_pData);
            if (iExtraLen == -2)
                return -2;
            else if (iExtraLen < 0)
                return -1;

            break;

#ifdef ENABLE_EVENT_MANAGER
		case HEADER_CG_EVENT_ACTIVATE:
			EventActivate(ch, c_pData);
			break;
#endif
	}
	return (iExtraLen);
}

int CInputDead::Analyze(LPDESC d, BYTE bHeader, const char * c_pData)
{
	LPCHARACTER ch;

	if (!(ch = d->GetCharacter()))
	{
		sys_err("no character on desc");
		return 0;
	}

	int iExtraLen = 0;

	switch (bHeader)
	{
		case HEADER_CG_PONG:
#ifdef ENABLE_ANTIHL
			Pong(d, c_pData);
#else
			Pong(d);
#endif
			break;

		case HEADER_CG_TIME_SYNC:
			Handshake(d, c_pData);
			break;

		case HEADER_CG_CHAT:
			if ((iExtraLen = Chat(ch, c_pData, m_iBufferLeft)) < 0)
				return -1;

			break;

		case HEADER_CG_WHISPER:
			if ((iExtraLen = Whisper(ch, c_pData, m_iBufferLeft)) < 0)
				return -1;

			break;

		case HEADER_CG_HACK:
			Hack(ch, c_pData);
			break;

		default:
			return (0);
	}

	return (iExtraLen);
}
#ifdef ENABLE_DROP_INFO
void CInputMain::DropInfo(LPCHARACTER ch)
{
	if (!ch || !ch->GetDesc())
		return;

	const auto pkTarget = ch->GetTarget();

	if (!pkTarget)
		return;

	if (pkTarget->IsMonster() || pkTarget->IsStone())
	{
		std::vector<TDropInfoItem> vecDropInfo;
		const auto bHasDropInfo = ITEM_MANAGER::Instance().CreateDropItemInfo(pkTarget, ch, vecDropInfo);

		if (bHasDropInfo)
		{
			TEMP_BUFFER buf;

			for (const auto& it : vecDropInfo)
			{
				TPacketDropInfoItem item;
				item.dwVnum = it.dwVnum;
				item.byMinCount = it.byMinCount;
				item.byMaxCount = it.byMaxCount;
#ifdef ENABLE_DROP_INFO_PCT
				item.pct = it.pct;
#endif
				buf.write(&it, sizeof(TPacketDropInfoItem));
			}

			TPacketDropInfo packet;
			packet.header = HEADER_GC_DROP_INFO;
			packet.size = sizeof(TPacketDropInfo) + buf.size();
			packet.item_count = vecDropInfo.size();

			const auto& tTable = pkTarget->GetMobTable();
			packet.gold_min = tTable.dwGoldMin;
			packet.gold_max = tTable.dwGoldMax;

			packet.mob_vnum = pkTarget->GetRaceNum();

			ch->GetDesc()->BufferedPacket(&packet, sizeof(TPacketDropInfo));
			ch->GetDesc()->Packet(buf.read_peek(), buf.size());
		}
	}
}
#endif
#ifdef ENABLE_SWITCHBOT
int CInputMain::Switchbot(LPCHARACTER ch, const char* data, size_t uiBytes)
{
	const TPacketCGSwitchbot* p = reinterpret_cast<const TPacketCGSwitchbot*>(data);
	if (uiBytes < sizeof(TPacketCGSwitchbot))
	{
		return -1;
	}
	
	const char* c_pData = data + sizeof(TPacketCGSwitchbot);
	uiBytes -= sizeof(TPacketCGSwitchbot);
	
	switch (p->subheader)
	{
	case SUBHEADER_CG_SWITCHBOT_START:
	{
		size_t extraLen = sizeof(TSwitchbotAttributeAlternativeTable) * SWITCHBOT_ALTERNATIVE_COUNT;
		if (uiBytes < extraLen)
		{
			return -1;
		}
		
		std::vector<TSwitchbotAttributeAlternativeTable> vec_alternatives;
		for (BYTE alternative = 0; alternative < SWITCHBOT_ALTERNATIVE_COUNT; ++alternative)
		{
			const TSwitchbotAttributeAlternativeTable* pAttr = reinterpret_cast<const TSwitchbotAttributeAlternativeTable*>(c_pData);
			c_pData += sizeof(TSwitchbotAttributeAlternativeTable);
			vec_alternatives.emplace_back(*pAttr);
		}
		
		CSwitchbotManager::Instance().Start(ch->GetPlayerID(), p->slot, vec_alternatives);
		// Updated to include switchItemVnum parameter
		//CSwitchbotManager::Instance().Start(ch->GetPlayerID(), p->slot, p->switchingItemVnum, vec_alternatives);
		return extraLen;
	}
	case SUBHEADER_CG_SWITCHBOT_STOP:
	{
		CSwitchbotManager::Instance().Stop(ch->GetPlayerID(), p->slot);
		return 0;
	}
	}
	return 0;
}
#endif
#ifdef ENABLE_RESP_SYSTEM
int CInputMain::Resp(LPCHARACTER ch, const char* c_pData, size_t uiBytes)
{
	if (!ch || !ch->GetDesc())
		return -1;

	TPacketCGRespHeader* p = (TPacketCGRespHeader*)c_pData;

	if (uiBytes < sizeof(TPacketCGRespHeader))
		return -1;

	c_pData += sizeof(TPacketCGRespHeader);
	uiBytes -= sizeof(TPacketCGRespHeader);

	switch (p->subheader)
	{
		case RESP_CG_SUBHEADER_FETCH_RESP:
		case RESP_CG_SUBHEADER_FETCH_DROP:
		{
			if (uiBytes < sizeof(uint32_t))
				return -1;

			uint32_t mobVnum = *(uint32_t*)c_pData;
			c_pData += sizeof(uint32_t);
			uiBytes -= sizeof(uint32_t);

			if (ch->CanActRespManager())
				CRespManager::instance().FetchData(ch, p->subheader, mobVnum);

			return sizeof(uint32_t);
		}

		case RESP_CG_SUBHEADER_TELEPORT:
		{
			if (uiBytes < sizeof(uint32_t))
				return -1;

			uint32_t id = *(uint32_t*)c_pData;
			c_pData += sizeof(int64_t);
			uiBytes -= sizeof(uint32_t);

			if (ch->CanActRespManager())
				CRespManager::instance().Teleport(ch, id);

			return sizeof(uint32_t);
		}

		default:
			sys_err("Unknown subheader %d", p->subheader);
			return -1;
	}
}
#endif

#ifdef ENABLE_CHEQUE_SYSTEM
void CInputMain::WonExchange(LPCHARACTER ch, const char* pcData)
{
	const TPacketCGWonExchange* p = reinterpret_cast<const TPacketCGWonExchange*>(pcData);
	const EWonExchangeCGSubHeader SubHeader = static_cast<EWonExchangeCGSubHeader>(p->bSubHeader);
	switch (SubHeader)
	{
	case WON_EXCHANGE_CG_SUBHEADER_SELL:
	case WON_EXCHANGE_CG_SUBHEADER_BUY:
		ch->WonExchange(SubHeader, p->wValue);
		break;
	default:
		sys_err("invalid won exchange subheader: %u value: %u", SubHeader, p->wValue);
		break;
	}
}
#endif

#ifdef __REMOVE_ITEM__
int CInputMain::RemoveItem(LPCHARACTER ch, const char* data, size_t uiBytes)
{
	if (!ch || !ch->GetDesc())
		return -1;

	TPacketCGRemoveItem* p = (TPacketCGRemoveItem*)data;

	if (uiBytes < sizeof(TPacketCGRemoveItem))
		return -1;

	const char* c_pData = data + sizeof(TPacketCGRemoveItem);
	uiBytes -= sizeof(TPacketCGRemoveItem);

	if (p->item_count > REMOVE_ITEM_SLOT_COUNT_X * REMOVE_ITEM_SLOT_COUNT_Y)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "remove item too much items");
		return 0;
	}

	if (uiBytes < sizeof(TItemPos) * p->item_count)
		return -1;

	int next_time = ch->GetQuestFlag("__remove_item.next_time");
	if (next_time - get_global_time() > 0 || !ch->CanHandleItem())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "remove item wait a second");
		return sizeof(TItemPos) * p->item_count;
	}

	if (ch->GetExchange() || ch->GetMyShop() || ch->GetShopOwner() || ch->IsOpenSafebox() || ch->IsCubeOpen()
#ifdef __COSTUME_ACCE__
				|| ch->IsAcceWindowOpen()
#endif
    )
    {
        ch->ChatPacket(CHAT_TYPE_INFO, "remove item wait a second");
		return sizeof(TItemPos) * p->item_count;
    }

	for (int i = 0; i < p->item_count; i++)
	{
		TItemPos item_pos = *(TItemPos*)c_pData;
		c_pData += sizeof(TItemPos);
		uiBytes -= sizeof(TItemPos);

		auto item = ch->GetItem(item_pos);
		if (!item)
		{
			sys_err("%s tried to remove non existing item on position [%d, %d]", ch->GetName(), item_pos.window_type, item_pos.cell);
			continue;
		}

		const auto item_table = item->GetProto();

		// remove comment if you have antiflag for remove
		/*if (item_table && (IS_SET(item_table->dwAntiFlags, ITEM_ANTIFLAG_REMOVE)))
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "locale_string_0629");
			return;
		}*/

		if (item->IsEquipped())
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "remove item can remove equiped %s", item->GetName());
			continue;
		}

		if (item->isLocked())
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "remove item item locked %s", item->GetName());
			continue;
		}
#ifdef _ENABLE_BATTLEPASS_
		BattlePassManager::Instance().Notify(MISSION_TYPE_ITEM_DESTROY, ch, item->GetVnum(), item->GetCount());
#endif
		ch->ChatPacket(CHAT_TYPE_INFO, "remove item remove %s x %d", item->GetName(), item->GetCount());
		ITEM_MANAGER::instance().RemoveItem(item);
	}

	TPacketGCRemoveItem packet;
	packet.header = HEADER_GC_REMOVE_ITEM;

	ch->GetDesc()->Packet(&packet, sizeof(packet));
	ch->SetQuestFlag("__remove_item.next_time", get_global_time() + 3);

	return sizeof(TItemPos) * p->item_count;
}
#endif

#ifdef ENABLE_ODLAMKI_SYSTEM
int CInputMain::OdlamkiItem(LPCHARACTER ch, const char* data, size_t uiBytes)
{
	if (!ch || !ch->GetDesc())
		return -1;

	TPacketCGOdlamki* p = (TPacketCGOdlamki*)data;

	if (uiBytes < sizeof(TPacketCGOdlamki))
		return -1;

	const char* c_pData = data + sizeof(TPacketCGOdlamki);
	uiBytes -= sizeof(TPacketCGOdlamki);

	if (p->item_count > 18)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "too much items");
		return 0;
	}

	if (uiBytes < sizeof(TItemPos) * p->item_count)
		return -1;

	if (ch->GetExchange() || ch->GetMyShop() || ch->GetShopOwner() || ch->IsOpenSafebox() || ch->IsCubeOpen())
    {
        ch->ChatPacket(CHAT_TYPE_INFO, "close all window");
		return sizeof(TItemPos) * p->item_count;
    }

	int count = 0;

	for (int i = 0; i < p->item_count; i++)
	{
		TItemPos item_pos = *(TItemPos*)c_pData;
		c_pData += sizeof(TItemPos);
		uiBytes -= sizeof(TItemPos);

		auto item = ch->GetItem(item_pos);
		if (!item)
		{
			sys_err("%s tried to remove non existing item on position [%d, %d]", ch->GetName(), item_pos.window_type, item_pos.cell);
			continue;
		}

		if (item->GetVnum() >= 28530 && item->GetVnum() <= 28543)
            count = 2 * item->GetCount();
        else if (item->GetVnum() >= 28630 && item->GetVnum() <= 28643)
            count = 3 * item->GetCount();
        else if (item->GetVnum() >= 28730 && item->GetVnum() <= 28743)
            count = 4 * item->GetCount();
        else if (item->GetVnum() >= 28830 && item->GetVnum() <= 28843)
            count = 5 * item->GetCount();
        else if (item->GetVnum() >= 28900 && item->GetVnum() <= 28913)
            count = 6 * item->GetCount();

		if (count <= 0)
            return 0;

		item->RemoveFromCharacter();
		ITEM_MANAGER::Instance().DestroyItem(item);
        while (count > 5000)
        {
#ifdef ENABLE_HIGHLIGHT_SYSTEM
			ch->AutoGiveItem(28961, 5000, -1, false, false);
#else
			ch->AutoGiveItem(28961, 5000, -1, false);
#endif
            count -= 5000;
        }
    
        if (count > 0)
#ifdef ENABLE_HIGHLIGHT_SYSTEM
			ch->AutoGiveItem(28961, count, -1, false, false);
#else
			ch->AutoGiveItem(28961, count, -1, false);
#endif
	}

	TPacketGCOdlamki packet;
	packet.header = HEADER_GC_ODLAMKI_ITEM;

	ch->GetDesc()->Packet(&packet, sizeof(packet));
	return sizeof(TItemPos) * p->item_count;
}
#endif

#ifdef ENABLE_SAVE_LOCATION_SYSTEM
void CInputMain::SaveLocation(LPCHARACTER pkChar, const char* c_pData)
{
	quest::PC* pPC = quest::CQuestManager::instance().GetPCForce(pkChar->GetPlayerID());
	if (pPC->IsRunning())
		return;

	TPacketSaveLocation * p = (TPacketSaveLocation *) c_pData;
	switch (p->subheader)
	{
	case SAVELOCATION_SUBHEADER_CG_ADD:
		pkChar->AddSaveLocation(p->pos, p->location[0].szName);
		break;
	case SAVELOCATION_SUBHEADER_CG_DEL:
		pkChar->DeleteSaveLocation(p->pos);
		break;
	case SAVELOCATION_SUBHEADER_CG_WARP:
		pkChar->WarpToSaveLocation(p->pos);
		break;
	case SAVELOCATION_SUBHEADER_CG_RENAME:
		pkChar->RenameSaveLocation(p->pos, p->location[0].szName);
		break;
	default:
		break;
	}
}
#endif


#ifdef ENABLE_VS_SHOP_SEARCH
int CInputMain::SearchItemList(LPCHARACTER ch, const char* data, size_t uiBytes)
{
	if (!ch || !ch->GetDesc())
		return -1;

	TPacketCGSearch* p = (TPacketCGSearch*)data;

	if (uiBytes < sizeof(TPacketCGSearch))
		return -1;

	const char* c_pData = data + sizeof(TPacketCGSearch);
	uiBytes -= sizeof(TPacketCGSearch);

	if (p->items > 36)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "[LS;2396]");
		return 0;
	}

	if (uiBytes < sizeof(TSearchItems) * p->items)
		return -1;

	if (ch->GetExchange() || ch->GetMyShop() || ch->GetShopOwner() || ch->IsOpenSafebox() || ch->IsCubeOpen()
	|| ch->GetOfflineShopOwner()
    )
    {
		ch->ChatPacket(CHAT_TYPE_INFO, "[LS;2397]");
		return sizeof(TSearchItems) * p->items;
    }

    std::vector<int> vnumList;

    for (int i = 0; i < p->items; i++)
    {
        TSearchItems item = *(TSearchItems*)c_pData;
        c_pData += sizeof(TSearchItems);
        uiBytes -= sizeof(TSearchItems);

        vnumList.push_back(item.vnum);
    }

    std::ostringstream oss;
    for (size_t i = 0; i < vnumList.size(); ++i)
    {
        oss << vnumList[i];
        if (i != vnumList.size() - 1)
        {
            oss << ", ";
        }
    }
    std::string vnumString = oss.str();

	COfflineShopManager::Instance().ClearShopSearch(ch);
    COfflineShopManager::instance().SearchInShops(ch, 2, 0, vnumString, 0, 0);

    return sizeof(TPacketCGSearch) + (sizeof(TSearchItems) * p->items);
}
#endif

#ifdef RANKING_SYSTEM
void CInputMain::LoadServerRanking(LPCHARACTER ch, const char* data)
{
	const TPacketCGLoadServerRanking* pinfo = (TPacketCGLoadServerRanking*)data;
	if (pinfo->catIdx >= 0 && pinfo->catIdx < SERVER_RANK_TYPE_MAX_NUM)
	{
		CServerRankingManager::instance().LoadServerRanking(ch, pinfo->catIdx);
	}
}
void CInputMain::LoadRanking(LPCHARACTER ch, const char* data)
{
	const TPacketCGLoadRanking* pinfo = (TPacketCGLoadRanking*)data;
	if (pinfo->catIdx >= 0 && pinfo->catIdx < RANK_TYPE_MAX_NUM)
	{
		CRankingManager::instance().LoadRanking(ch, pinfo->catIdx);
	}
}
#endif

#ifdef ENABLE_SECONDARY_LEVEL
int CInputMain::SecondaryLevel(LPCHARACTER ch, const void* c_pData, size_t uiBytes)
{
	const TPacketCGSecondaryLevel* p = reinterpret_cast<const TPacketCGSecondaryLevel*>(c_pData);

	if (uiBytes < sizeof(TPacketCGSecondaryLevel))
		return -1;

	const char* pData = static_cast<const char*>(c_pData);

	pData += sizeof(TPacketCGSecondaryLevel);
	uiBytes -= sizeof(TPacketCGSecondaryLevel);

	switch (p->bSubHeader)
	{
	case SUBHEADER_SECONDARY_LEVEL_CG_DO_UPGRADE:
	{
		if (uiBytes < sizeof(TPacketCGSecondaryLevelUpgrade))
			return -1;

		const TPacketCGSecondaryLevelUpgrade i = *reinterpret_cast<const TPacketCGSecondaryLevelUpgrade*>(pData);

		CHARACTER_MANAGER::Instance().UpgradeSecondaryLevel(ch, i.bOption);
		return sizeof(TPacketCGSecondaryLevelUpgrade);
	} break;
	case SUBHEADER_SECONDARY_LEVEL_CG_MAX:
	default:
		sys_err("CInputMain::SecondaryLevel() ~ Unhandled packet header: %d", p->bSubHeader);
		break;
	}
	return 0;
}
#endif

#ifdef __ENABLE_MINING_WITH_SPACE__
void CInputMain::Mining(LPCHARACTER pChar, const char* c_pData)
{
	if (!pChar || !pChar->IsPC())
		return;

	const TPacketCGMining* p = reinterpret_cast<const TPacketCGMining*>(c_pData);
	pChar->MiningNeareastOre();
}
#endif

#ifdef ENABLE_IN_GAME_LOG_SYSTEM
int CInputMain::InGameLog(LPCHARACTER ch, const char* data, UINT uiBytes)
{
	if (!ch)
	{
		return -1;
	}

	if (uiBytes < sizeof(InGameLog::TPacketCGInGameLog))
	{
		return -1;
	}
	
	InGameLog::TPacketCGInGameLog* p = (InGameLog::TPacketCGInGameLog*)data;
	data += sizeof(InGameLog::TPacketCGInGameLog);
	uiBytes -= sizeof(InGameLog::TPacketCGInGameLog);

	InGameLog::InGameLogManager& rIglMgr = InGameLog::GetManager();

	return rIglMgr.RecvPacketCG(ch,p->subHeader, p->logID);
}
#endif

void CInputMain::FastStack(LPCHARACTER ch, const char* c_pData)
{
	TPacketCGFastStack* p = (TPacketCGFastStack*)c_pData;


	if (ch)
		ch->FastStack(p->pos);
}

#ifdef ENABLE_EVENT_MANAGER
void CInputMain::EventActivate(LPCHARACTER ch, const char* c_pData)
{
	if (!ch)
		return;

	const TPacketCGEventActivate* p = reinterpret_cast<const TPacketCGEventActivate*>(c_pData);

	TPacketGCEventActivateResult result;
	result.header = HEADER_GC_EVENT_ACTIVATE_RESULT;
	result.eventID = p->eventID;

	// Verify event exists in the manager
	bool eventFound = false;
	const auto& eventData = CHARACTER_MANAGER::instance().GetEventData();
	for (const auto& [dayIdx, dayVec] : eventData)
	{
		for (const auto& ev : dayVec)
		{
			if (ev.eventID == p->eventID)
			{
				// Check empire/channel restrictions
				if (ev.empireFlag != 0 && ev.empireFlag != ch->GetEmpire())
					continue;
				if (ev.channelFlag != 0 && ev.channelFlag != g_bChannel)
					continue;
				// Check event is valid today (startTime <= now <= endTime)
				const int curTime = static_cast<int>(get_global_time());
				if (curTime < ev.startTime || curTime > ev.endTime)
					continue;
				eventFound = true;
				break;
			}
		}
		if (eventFound)
			break;
	}

	if (!eventFound)
	{
		result.result = EVENT_ACTIVATE_FAIL;
		ch->GetDesc()->Packet(&result, sizeof(result));
		return;
	}

	if (ch->ActivatePersonalEvent(p->eventID))
	{
		result.result = EVENT_ACTIVATE_SUCCESS;
		CHARACTER_MANAGER::instance().CheckBonusEvent(ch);
		const std::string hwid = ch->GetCompositeHWID();
		if (!hwid.empty())
		{
			DBManager::instance().Query(
				"INSERT IGNORE INTO srv1_player.event_hwid_activations (hwid_composite, event_id, activated_at) VALUES ('%s', %u, %d)",
				hwid.c_str(), (DWORD)p->eventID, static_cast<int>(get_global_time()));
		}
	}
	else
	{
		result.result = EVENT_ACTIVATE_ALREADY;
	}

	ch->GetDesc()->Packet(&result, sizeof(result));
}
#endif

//martysama0134's ceqyqttoaf71vasf9t71218
