#pragma once

class CPythonMessenger : public CSingleton<CPythonMessenger>
{
	public:
		typedef std::set<std::string> TFriendNameMap;
		typedef std::map<std::string, BYTE> TGuildMemberStateMap;

		enum EMessengerGroupIndex
		{
			MESSENGER_GRUOP_INDEX_FRIEND,
			MESSENGER_GRUOP_INDEX_GUILD,
#ifdef ENABLE_MESSENGER_TEAM
			MESSENGER_GROUP_INDEX_TEAM,
#endif
#ifdef ENABLE_MESSENGER_BLOCK
			MESSENGER_GROUP_INDEX_BLOCK,
#endif
		};

	public:
		CPythonMessenger();
		virtual ~CPythonMessenger();

		void Destroy();

		// Friend
		void RemoveFriend(const char * c_szKey);
		void OnFriendLogin(const char * c_szKey);
		void OnFriendLogout(const char * c_szKey);
#ifdef ENABLE_MESSENGER_TEAM
		void OnTeamLogin(const char* c_szKey);
		void OnTeamLogout(const char* c_szKey);
#endif
		void SetMobile(const char * c_szKey, BYTE byState);
		BOOL IsFriendByKey(const char * c_szKey);
		BOOL IsFriendByName(const char * c_szName);

		// Guild
		void AppendGuildMember(const char * c_szName);
		void RemoveGuildMember(const char * c_szName);
		void RemoveAllGuildMember();
		void LoginGuildMember(const char * c_szName);
		void LogoutGuildMember(const char * c_szName);
		void RefreshGuildMember();
#ifdef ENABLE_MESSENGER_BLOCK
		// Block
		void RemoveBlock(const char* c_szKey);
		void OnBlockLogin(const char* c_szKey);
		void OnBlockLogout(const char* c_szKey);
		BOOL IsBlockByKey(const char* c_szKey);
		BOOL IsBlockFriendByName(const char* c_szName);
#endif

		void SetMessengerHandler(PyObject* poHandler);

	protected:
		TFriendNameMap m_FriendNameMap;
#ifdef ENABLE_MESSENGER_TEAM
		TFriendNameMap m_TeamNameMap;
#endif
		TGuildMemberStateMap m_GuildMemberStateMap;
#ifdef ENABLE_MESSENGER_BLOCK
		std::set<std::string> m_BlockNameMap;
#endif

	private:
		PyObject * m_poMessengerHandler;
};
//martysama0134's ceqyqttoaf71vasf9t71218
