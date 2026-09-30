class CHARACTER;

#ifdef ENABLE_ASLAN_BUFF_NPC_SYSTEM
struct SBuffNPCAbility
{
};

class CBuffNPCActor
{
	public:
		enum EBuffNPCOptions
		{
			EBuffNPCOption_Followable		= 1 << 0,
			EBuffNPCOption_Mountable		= 1 << 1,
			EBuffNPCOption_Summonable		= 1 << 2,
			EBuffNPCOption_Combatable		= 1 << 3,		
		};

		CBuffNPCActor(LPCHARACTER owner, DWORD options = EBuffNPCOption_Followable | EBuffNPCOption_Summonable);
		virtual ~CBuffNPCActor();

		virtual bool	Update(DWORD deltaTime);
		virtual bool	_UpdateFollowAI();
		virtual bool	_UpdatAloneActionAI();

	private:
		bool Follow(float fMinDistance = 150.f);

	public:
		void		Destroy();
		
		LPCHARACTER		GetCharacter()	const					{ return m_pkChar; }
		LPCHARACTER		GetOwner()	const						{ return m_pkOwner; }
		DWORD			GetVID() const							{ return m_dwVID; }
		DWORD			GetVnum() const							{ return m_dwVnum; }

		bool				HasOption(EBuffNPCOptions option) const		{ return m_dwOptions & option; }
		
		void				SetName(const std::string& name);

		void				SetInt(DWORD value);

		void				LoadBuffNPC();
		void				ClearBuffNPC();

		DWORD				Summon();
		void				Unsummon();
		void				UpdateBuffEquipment();

		bool				IsSummoned() const			{ return 0 != m_pkChar; }
		
		void				SetLastSkillTime(DWORD time)	{ m_dwLastSkillTime = time; }
		DWORD				GetLastSkillTime()	{ return m_dwLastSkillTime; }
		
		void				SetIsActive(bool isActive)	{ m_bIsActive = isActive; }
		DWORD				IsActive()	{ return m_bIsActive; }

		DWORD				GetSex() { return m_dwSex; }
		DWORD				GetIntValue() const { return m_dwIntValue; }

		int					GetSkillID(int iSkillVnum);
		int					GetSkillGrade(int iSkillLevel);
		int					GetSkillLevel(int iSkillVnum);

		void				SetChangeSex();

		void				SetSkill(DWORD dwSkillSlot, DWORD dwSkillLevel);

		void				UpdateTime();
		void				SaveSQL();
		bool				UseSkill();

	private:
		LPCHARACTER			m_pkChar;
		LPCHARACTER			m_pkOwner;

		short				m_originalMoveSpeed;

		bool				m_bIsActive;

		DWORD				m_dwVnum;
		DWORD				m_dwVID;
		DWORD				m_dwOptions;
		DWORD				m_dwLastSkillTime;
		DWORD				m_dwLastActionTime;
			
		std::string			m_name;
		DWORD				m_dwSex;
		DWORD				m_dwIntValue;
		DWORD				m_dwSkill[3];
		DWORD				m_dwSkillNextUse[3];
		DWORD				m_dwSkillInUse[3];

		DWORD				m_dwUpdatePeriod;
		DWORD				m_dwLastUpdateTime;
		LPEVENT				m_pkBuffNPCSystemExpireEvent;
};

#endif
