#pragma once
#ifdef __TEAM_MEMBER_STATUS__

class CTeamListManager : public singleton<CTeamListManager>
{
  public:
	CTeamListManager();
	void Initialize();
	virtual ~CTeamListManager() {}

  public:
	void RegisterTeamMember(const LPCHARACTER pChar);
	void RegisterTeamMember(const TPacketGGTeamList* pPacket);
	void RemoveTeamMember(const std::string& sName);
	void RemoveTeamMember(const TPacketGGTeamList* pPacket);
	void ClearTeamMember();
	void RequestReload();
	void SendGMList(const LPCHARACTER pChar, const std::string& rKey = "");
	void ClearGMList(const LPCHARACTER pChar);

  private:
	std::map<std::string, std::pair<std::string, bool>> m_team_members;
};
#endif
