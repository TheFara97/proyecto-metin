class CHorseNameManager : public singleton<CHorseNameManager>
{
	private :
		std::map<DWORD, std::string> m_mapHorseNames;
#ifdef ENABLE_NEW_PET_SYSTEM
		std::map<DWORD, std::string> m_mapPetNames;
		void BroadcastPetName(DWORD dwPlayerID, const char* szPetName);
#endif

		void BroadcastHorseName(DWORD dwPlayerID, const char* szHorseName);

	public :
		CHorseNameManager();

		const char* GetHorseName(DWORD dwPlayerID);

		void UpdateHorseName(DWORD dwPlayerID, const char* szHorseName, bool broadcast=false);

		void Validate(LPCHARACTER pChar);
#ifdef ENABLE_NEW_PET_SYSTEM
		const char* GetPetName(DWORD dwPlayerID);
		void UpdatePetName(DWORD dwPlayerID, const char* szPetName, bool broadcast = false);
#endif
};
//martysama0134's ceqyqttoaf71vasf9t71218
