#pragma once

#include "AbstractPlayer.h"
#include "Packet.h"
#include "PythonSkill.h"
#include "Locale_inc.h"

class CInstanceBase;

enum
{
	MAIN_RACE_WARRIOR_M,
	MAIN_RACE_ASSASSIN_W,
	MAIN_RACE_SURA_M,
	MAIN_RACE_SHAMAN_W,
	MAIN_RACE_WARRIOR_W,
	MAIN_RACE_ASSASSIN_M,
	MAIN_RACE_SURA_W,
	MAIN_RACE_SHAMAN_M,
#ifdef ENABLE_WOLFMAN_CHARACTER
	MAIN_RACE_WOLFMAN_M,
#endif
	MAIN_RACE_MAX_NUM,
};

#ifdef ENABLE_PERFORMANCE_IMPROVEMENTS_NEW
extern std::map<int, CGraphicImage*> m_kMap_iEmotionIndex_pkIconImage;

extern const DWORD c_iSkillIndex_Tongsol;
extern const DWORD c_iSkillIndex_Combo;
extern const DWORD c_iSkillIndex_Fishing;
extern const DWORD c_iSkillIndex_Mining;
extern const DWORD c_iSkillIndex_Making;
extern const DWORD c_iSkillIndex_Language1;
extern const DWORD c_iSkillIndex_Language2;
extern const DWORD c_iSkillIndex_Language3;
extern const DWORD c_iSkillIndex_Polymorph;
extern const DWORD c_iSkillIndex_Riding;
extern const DWORD c_iSkillIndex_Summon;
#endif


class CPythonPlayer : public CSingleton<CPythonPlayer>, public IAbstractPlayer
{
	public:
		enum
		{
			CATEGORY_NONE		= 0,
			CATEGORY_ACTIVE		= 1,
			CATEGORY_PASSIVE	= 2,
			CATEGORY_MAX_NUM	= 3,

			STATUS_INDEX_ST = 1,
			STATUS_INDEX_DX = 2,
			STATUS_INDEX_IQ = 3,
			STATUS_INDEX_HT = 4,
		};

		enum
		{
			MBT_LEFT,
			MBT_RIGHT,
			MBT_MIDDLE,
			MBT_NUM,
		};

		enum
		{
			MBF_SMART,
			MBF_MOVE,
			MBF_CAMERA,
			MBF_ATTACK,
			MBF_SKILL,
			MBF_AUTO,
#ifdef ENABLE_METIN_QUEUE
			MBF_QUEUE_ON,
#endif
		};

		enum
		{
			MBS_CLICK,
			MBS_PRESS,
		};

		enum EMode
		{
			MODE_NONE,
			MODE_CLICK_POSITION,
			MODE_CLICK_ITEM,
			MODE_CLICK_ACTOR,
			MODE_USE_SKILL,
#ifdef ENABLE_PREMIUM_PRIVATE_SHOP_OFFICIAL
			MODE_CLICK_PRIVATE_SHOP,
#endif
		};

		enum EEffect
		{
			EFFECT_PICK,
			EFFECT_NUM,
		};

		enum EMetinSocketType
		{
			METIN_SOCKET_TYPE_NONE,
			METIN_SOCKET_TYPE_SILVER,
			METIN_SOCKET_TYPE_GOLD,
		};

#ifdef ENABLE_PERFORMANCE_IMPROVEMENTS_NEW
		enum EEmotions
		{
			EMOTION_CLAP = 1,
			EMOTION_CONGRATULATION,
			EMOTION_FORGIVE,
			EMOTION_ANGRY,
			EMOTION_ATTRACTIVE,
			EMOTION_SAD,
			EMOTION_SHY,
			EMOTION_CHEERUP,
			EMOTION_BANTER,
			EMOTION_JOY,
			EMOTION_CHEERS_1,
			EMOTION_CHEERS_2,
			EMOTION_DANCE_1,
			EMOTION_DANCE_2,
			EMOTION_DANCE_3,
			EMOTION_DANCE_4,
			EMOTION_DANCE_5,
			EMOTION_DANCE_6,
			EMOTION_KISS = 51,
			EMOTION_FRENCH_KISS,
			EMOTION_SLAP,
		};
#endif

		typedef struct SSkillInstance
		{
			DWORD dwIndex;
			int iType;
			int iGrade;
			int iLevel;
			float fcurEfficientPercentage;
			float fnextEfficientPercentage;
			BOOL isCoolTime;

			float fCoolTime;
			float fLastUsedTime;
			BOOL bActive;
		} TSkillInstance;

		enum EKeyBoard_UD
		{
			KEYBOARD_UD_NONE,
			KEYBOARD_UD_UP,
			KEYBOARD_UD_DOWN,
		};

		enum EKeyBoard_LR
		{
			KEYBOARD_LR_NONE,
			KEYBOARD_LR_LEFT,
			KEYBOARD_LR_RIGHT,
		};

		enum
		{
			DIR_UP,
			DIR_DOWN,
			DIR_LEFT,
			DIR_RIGHT,
		};

		typedef struct SPlayerStatus
		{
			TItemData			aItem[c_Inventory_Count];
			TItemData			aDSItem[c_DragonSoul_Inventory_Count];
#ifdef ENABLE_ASLAN_BUFF_NPC_SYSTEM
			TItemData 			aBuffEquipmentItem[CItemData::BUFF_WINDOW_SLOT_MAX_NUM];
#endif
#ifdef ENABLE_SWITCHBOT
			TItemData			aSwitchbotItem[SWITCHBOT_SLOT_COUNT];
#endif
			TQuickSlot			aQuickSlot[QUICKSLOT_MAX_NUM];
			TSkillInstance		aSkill[SKILL_MAX_NUM];
#ifdef ENABLE_LONG_LONG
			long long 			m_alPoint[POINT_MAX_NUM];
#else
			long 				m_alPoint[POINT_MAX_NUM];
#endif
			long				lQuickPageIndex;

#ifdef ENABLE_LONG_LONG
			void SetPoint(UINT ePoint, long long lPoint);
#else
			void SetPoint(UINT ePoint, long lPoint);
#endif
#ifdef ENABLE_LONG_LONG
			long long GetPoint(UINT ePoint);
#else
			long GetPoint(UINT ePoint);
#endif
		} TPlayerStatus;

		typedef struct SPartyMemberInfo
		{
#if defined(__BL_PARTY_POSITION__)
			SPartyMemberInfo(DWORD _dwPID, const char* c_szName) : dwPID(_dwPID), strName(c_szName), dwVID(0), lX(0), lY(0), fMinimapX(0.0f), fMinimapY(0.0f) {}
#else
			SPartyMemberInfo(DWORD _dwPID, const char* c_szName) : dwPID(_dwPID), strName(c_szName), dwVID(0) {}
#endif

			DWORD dwVID;
			DWORD dwPID;
			std::string strName;
			BYTE byState;
			BYTE byHPPercentage;
			short sAffects[PARTY_AFFECT_SLOT_MAX_NUM];
#if defined(__BL_PARTY_POSITION__)
			long lX;
			long lY;
			float fMinimapX;
			float fMinimapY;
#endif
		} TPartyMemberInfo;

		enum EPartyRole
		{
			PARTY_ROLE_NORMAL,
			PARTY_ROLE_LEADER,
			PARTY_ROLE_ATTACKER,
			PARTY_ROLE_TANKER,
			PARTY_ROLE_BUFFER,
			PARTY_ROLE_SKILL_MASTER,
			PARTY_ROLE_BERSERKER,
			PARTY_ROLE_DEFENDER,
			PARTY_ROLE_MAX_NUM,
		};

		enum
		{
			SKILL_NORMAL,
			SKILL_MASTER,
			SKILL_GRAND_MASTER,
			SKILL_PERFECT_MASTER,
		};

		struct SAutoPotionInfo
		{
			SAutoPotionInfo() : bActivated(false), totalAmount(0), currentAmount(0) {}

			bool bActivated;
			long currentAmount;
			long totalAmount;
			long inventorySlotIndex;
		};

		enum EAutoPotionType
		{
			AUTO_POTION_TYPE_HP = 0,
			AUTO_POTION_TYPE_SP = 1,
			AUTO_POTION_TYPE_NUM
		};

	public:
		CPythonPlayer(void);
		virtual ~CPythonPlayer(void);

		void	PickCloseMoney();
		void	PickCloseItem();
#ifdef ENABLE_QUICK_PICKUP
		void	PickAllCloseItems();
#endif

		void	SetGameWindow(PyObject * ppyObject);

		void	SetObserverMode(bool isEnable);
		bool	IsObserverMode();

		void	SetFreeCameraMode(bool isEnable);
		bool	IsFreeCameraMode();

		void	SetQuickCameraMode(bool isEnable);

		void	SetAttackKeyState(bool isPress);

		void	NEW_GetMainActorPosition(TPixelPosition* pkPPosActor);

		bool	RegisterEffect(DWORD dwEID, const char* c_szEftFileName, bool isCache);

		bool	NEW_SetMouseState(int eMBType, int eMBState);
		bool	NEW_SetMouseFunc(int eMBType, int eMBFunc);
		int		NEW_GetMouseFunc(int eMBT);
		void	NEW_SetMouseMiddleButtonState(int eMBState);

		void	NEW_SetAutoCameraRotationSpeed(float fRotSpd);
		void	NEW_ResetCameraRotation();

		void	NEW_SetSingleDirKeyState(int eDirKey, bool isPress);
		void	NEW_SetSingleDIKKeyState(int eDIKKey, bool isPress);
		void	NEW_SetMultiDirKeyState(bool isLeft, bool isRight, bool isUp, bool isDown);

		void	NEW_Attack();
		void	NEW_Fishing();
		bool	NEW_CancelFishing();

#ifdef __ENABLE_MINING_WITH_SPACE__
		void	NEW_Mining();
#endif

		void	NEW_LookAtFocusActor();
		bool	NEW_IsAttackableDistanceFocusActor();

		bool	NEW_MoveToDestPixelPositionDirection(const TPixelPosition& c_rkPPosDst);
		bool	NEW_MoveToMousePickedDirection();
		bool	NEW_MoveToMouseScreenDirection();
		bool	NEW_MoveToDirection(float fDirRot, bool isBackwards);
		void	NEW_Stop();

		// Reserved
		bool	NEW_IsEmptyReservedDelayTime(float fElapsedtime);

		// Dungeon
		void	SetDungeonDestinationPosition(int ix, int iy);
		void	AlarmHaveToGo();

		CInstanceBase* NEW_FindActorPtr(DWORD dwVID);
		CInstanceBase* NEW_GetMainActorPtr();

		// flying target set
		void	Clear();
		void	ClearSkillDict();
		void	NEW_ClearSkillData(bool bAll = false);

		void	Update();

		// Play Time
		DWORD	GetPlayTime();
		void	SetPlayTime(DWORD dwPlayTime);
#ifdef ENABLE_OFFLINE_SHOP_SYSTEM
		void		SetCurrentOfflineShopMoney(long long dwMoney);
		void		SetCurrentOfflineShopStatus(int IsOnline);
		void		SetCurrentOfflineShopChequeMoney(long long dwCheque);
		void		SetCurrentOfflineShopZarobek(long long zarobekYang);
		void		SetCurrentOfflineShopZarobekCheque(long long zarobekCheque);
		long long	GetCurrentOfflineShopMoney();
		int			GetCurrentOfflineShopStatus();
		long long	GetCurrentOfflineShopChequeMoney();
		long long	GetCurrentOfflineShopZarobek();
		long long	GetCurrentOfflineShopZarobekCheque();
#endif
		// System
		void	SetMainCharacterIndex(int iIndex);

		DWORD	GetMainCharacterIndex();
		bool	IsMainCharacterIndex(DWORD dwIndex);
		DWORD	GetGuildID();
		void	NotifyDeletingCharacterInstance(DWORD dwVID);
		void	NotifyCharacterDead(DWORD dwVID);
		void	NotifyCharacterUpdate(DWORD dwVID);
		void	NotifyDeadMainCharacter();
		void	NotifyChangePKMode();

		// Player Status
		const char *	GetName();
		void	SetName(const char *name);

		void	SetRace(DWORD dwRace);
		DWORD	GetRace();

		void        SetOriginalRace(uint32_t race) { m_OriginalRace = race; }
		uint32_t    GetOriginalRace() const { return m_OriginalRace; }

		void	SetWeaponPower(DWORD dwMinPower, DWORD dwMaxPower, DWORD dwMinMagicPower, DWORD dwMaxMagicPower, DWORD dwAddPower);
#ifdef ENABLE_LONG_LONG
		void	SetStatus(DWORD dwType, long long lValue);
		long long		GetStatus(DWORD dwType);
#else
		void	SetStatus(DWORD dwType, long lValue);
		int		GetStatus(DWORD dwType);
#endif

		// Item
		void	MoveItemData(TItemPos SrcCell, TItemPos DstCell);
		void	SetItemData(TItemPos Cell, const TItemData & c_rkItemInst);
		const TItemData * GetItemData(TItemPos Cell) const;
#ifdef ENABLE_EXTENDED_ITEM_COUNT
		void	SetItemCount(TItemPos Cell, uint16_t byCount);
#else
		void	SetItemCount(TItemPos Cell, BYTE byCount);
#endif
		void	SetItemMetinSocket(TItemPos Cell, DWORD dwMetinSocketIndex, DWORD dwMetinNumber);
		void	SetItemAttribute(TItemPos Cell, DWORD dwAttrIndex, BYTE byType, short sValue);

		DWORD	GetItemIndex(TItemPos Cell);
		DWORD	GetItemFlags(TItemPos Cell);
		DWORD	GetItemAntiFlags(TItemPos Cell);

#ifdef __AUTO_SKILL_READER__
		DWORD	GetItemCountByVnumNew(DWORD dwVnum, int socket0);
		int		GetSkillLevelNew(DWORD dwSkillIndex);
		int		GetSkillGradeNew(DWORD dwSkillIndex);
#endif
		BYTE	GetItemTypeBySlot(TItemPos Cell);
		BYTE	GetItemSubTypeBySlot(TItemPos Cell);
		int		GetItemSlotIndex(DWORD dwVnum);
		DWORD	GetItemCount(TItemPos Cell);
		DWORD	GetItemCountByVnum(DWORD dwVnum);
		DWORD	GetItemMetinSocket(TItemPos Cell, DWORD dwMetinSocketIndex);
		void	GetItemAttribute(TItemPos Cell, DWORD dwAttrSlotIndex, BYTE * pbyType, short * psValue);
		void	SendClickItemPacket(DWORD dwIID);

		void	RequestAddLocalQuickSlot(DWORD dwLocalSlotIndex, DWORD dwWndType, DWORD dwWndItemPos);
		void	RequestAddToEmptyLocalQuickSlot(DWORD dwWndType, DWORD dwWndItemPos);
		void	RequestMoveGlobalQuickSlotToLocalQuickSlot(DWORD dwGlobalSrcSlotIndex, DWORD dwLocalDstSlotIndex);
		void	RequestDeleteGlobalQuickSlot(DWORD dwGlobalSlotIndex);
		void	RequestUseLocalQuickSlot(DWORD dwLocalSlotIndex);
		DWORD	LocalQuickSlotIndexToGlobalQuickSlotIndex(DWORD dwLocalSlotIndex);

		void	GetGlobalQuickSlotData(DWORD dwGlobalSlotIndex, DWORD* pdwWndType, DWORD* pdwWndItemPos);
		void	GetLocalQuickSlotData(DWORD dwSlotPos, DWORD* pdwWndType, DWORD* pdwWndItemPos);
		void	RemoveQuickSlotByValue(int iType, int iPosition);

		char	IsItem(TItemPos SlotIndex);

#ifdef ENABLE_NEW_EQUIPMENT_SYSTEM
		bool    IsBeltInventorySlot(TItemPos Cell);
#endif
		bool	IsInventorySlot(TItemPos SlotIndex);
		bool	IsEquipmentSlot(TItemPos SlotIndex);

#ifdef WJ_SPLIT_INVENTORY_SYSTEM
		bool	IsSkillBookInventorySlot(TItemPos Cell);
		bool	IsUpgradeItemsInventorySlot(TItemPos Cell);
		bool	IsStoneInventorySlot(TItemPos Cell);
		bool	IsBoxInventorySlot(TItemPos Cell);
		bool	IsEfsunInventorySlot(TItemPos Cell);
		bool	IsCicekInventorySlot(TItemPos Cell);
#endif

		bool	IsEquipItemInSlot(TItemPos iSlotIndex);

		// Quickslot
		int		GetQuickPage();
		int		GetItemFirstIndexByVnum(DWORD dwVnum);
		void	SetQuickPage(int nPageIndex);
		void	AddQuickSlot(int QuickslotIndex, char IconType, char IconPosition);
		void	DeleteQuickSlot(int QuickslotIndex);
		void	MoveQuickSlot(int Source, int Target);

		// Skill
		void	SetSkill(DWORD dwSlotIndex, DWORD dwSkillIndex);
		bool	GetSkillSlotIndex(DWORD dwSkillIndex, DWORD* pdwSlotIndex);
		int		GetSkillIndex(DWORD dwSlotIndex);
		int		GetSkillGrade(DWORD dwSlotIndex);
		int		GetSkillLevel(DWORD dwSlotIndex);
		float	GetSkillCurrentEfficientPercentage(DWORD dwSlotIndex);
		float	GetSkillNextEfficientPercentage(DWORD dwSlotIndex);
		void	SetSkillLevel(DWORD dwSlotIndex, DWORD dwSkillLevel);
		void	SetSkillLevel_(DWORD dwSkillIndex, DWORD dwSkillGrade, DWORD dwSkillLevel);
		BOOL	IsToggleSkill(DWORD dwSlotIndex);
		void	ClickSkillSlot(DWORD dwSlotIndex);
		void	ChangeCurrentSkillNumberOnly(DWORD dwSlotIndex);
		bool	FindSkillSlotIndexBySkillIndex(DWORD dwSkillIndex, DWORD * pdwSkillSlotIndex);

		void	SetSkillCoolTime(DWORD dwSkillIndex);
		void	EndSkillCoolTime(DWORD dwSkillIndex);

		float	GetSkillCoolTime(DWORD dwSlotIndex);
		float	GetSkillElapsedCoolTime(DWORD dwSlotIndex);
		BOOL	IsSkillActive(DWORD dwSlotIndex);
		BOOL	IsSkillCoolTime(DWORD dwSlotIndex);
		void	UseGuildSkill(DWORD dwSkillSlotIndex);
		bool	AffectIndexToSkillSlotIndex(UINT uAffect, DWORD* pdwSkillSlotIndex);
		bool	AffectIndexToSkillIndex(DWORD dwAffectIndex, DWORD * pdwSkillIndex);
#if defined(ENABLE_RENEWAL_AFFECT_SHOWER)
		bool	SkillIndexToAffectIndex(DWORD dwSkillIndex, DWORD * pdwAffectIndex);
#endif

		void	SetAffect(UINT uAffect);
		void	ResetAffect(UINT uAffect);
		void	ClearAffects();

		// Target
		void	SetTarget(DWORD dwVID, BOOL bForceChange = TRUE);
		void	OpenCharacterMenu(DWORD dwVictimActorID);
		DWORD	GetTargetVID();

		// Party
		void	ExitParty();
		void	AppendPartyMember(DWORD dwPID, const char * c_szName);
		void	LinkPartyMember(DWORD dwPID, DWORD dwVID);
		void	UnlinkPartyMember(DWORD dwPID);
		void	UpdatePartyMemberInfo(DWORD dwPID, BYTE byState, BYTE byHPPercentage);
		void	UpdatePartyMemberAffect(DWORD dwPID, BYTE byAffectSlotIndex, short sAffectNumber);
		void	RemovePartyMember(DWORD dwPID);
		bool	IsPartyMemberByVID(DWORD dwVID);
		bool	IsPartyMemberByName(const char * c_szName);
		bool	GetPartyMemberPtr(DWORD dwPID, TPartyMemberInfo ** ppPartyMemberInfo);
		bool	PartyMemberPIDToVID(DWORD dwPID, DWORD * pdwVID);
		bool	PartyMemberVIDToPID(DWORD dwVID, DWORD * pdwPID);
		bool	IsSamePartyMember(DWORD dwVID1, DWORD dwVID2);
#if defined(__BL_PARTY_POSITION__)
		void		UpdatePartyMemberPosition(DWORD dwPID, long lX, long lY);
		void		UpdatePartyMemberMinimapPosition(DWORD dwPID, float fX, float fY);
		size_t		GetPartyMemberCount();
		DWORD		GetPartyMemberPIDByIndex(size_t index);
		bool		GetPartyMemberPosition(DWORD dwPID, D3DXVECTOR2* v2Position);
		bool		GetPartyMemberMinimapPosition(DWORD dwPID, D3DXVECTOR2* v2Position);
		bool		IsPartyPositionExist(DWORD dwPID);
		std::string	GetPartyMemberName(DWORD dwPID);
#endif
		// Fight
		void	RememberChallengeInstance(DWORD dwVID);
		void	RememberRevengeInstance(DWORD dwVID);
		void	RememberCantFightInstance(DWORD dwVID);
		void	ForgetInstance(DWORD dwVID);
		bool	IsChallengeInstance(DWORD dwVID);
		bool	IsRevengeInstance(DWORD dwVID);
		bool	IsCantFightInstance(DWORD dwVID);

		// Private Shop
		void	OpenPrivateShop();
		void	ClosePrivateShop();
		bool	IsOpenPrivateShop();
#if defined(ENABLE_KEYCHANGE_SYSTEM)
		// Keyboard Controls
	public:
		void OpenKeyChangeWindow();
		void IsOpenKeySettingWindow(BOOL bSet) { m_isOpenKeySettingWindow = bSet; };

		void KeySetting(int iKey, int iKeyFunction) { m_keySettingMap[iKey] = iKeyFunction; }
		void KeySettingClear() { m_keySettingMap.clear(); }

		void OnKeyDown(int iKey);
		void OnKeyUp(int iKey);

		using KeySettingMap = std::map<int, int>;
	private:
		int m_iKeyBuffer;
		BOOL m_isOpenKeySettingWindow;
		KeySettingMap m_keySettingMap;
		bool m_bAutoRun;
	public:
#endif

		// Stamina
		void	StartStaminaConsume(DWORD dwConsumePerSec, DWORD dwCurrentStamina);
		void	StopStaminaConsume(DWORD dwCurrentStamina);

		// PK Mode
		DWORD	GetPKMode();

		// Mobile
		void	SetMobileFlag(BOOL bFlag);
		BOOL	HasMobilePhoneNumber();

		// Combo
		void	SetComboSkillFlag(BOOL bFlag);

		// System
		void	SetMovableGroundDistance(float fDistance);

		// Emotion
		void	ActEmotion(DWORD dwEmotionID);
		void	StartEmotionProcess();
		void	EndEmotionProcess();

		// Function Only For Console System
		BOOL	__ToggleCoolTime();
		BOOL	__ToggleLevelLimit();

		__inline const	SAutoPotionInfo& GetAutoPotionInfo(int type) const	{ return m_kAutoPotionInfo[type]; }
		__inline		SAutoPotionInfo& GetAutoPotionInfo(int type)		{ return m_kAutoPotionInfo[type]; }
		__inline void					 SetAutoPotionInfo(int type, const SAutoPotionInfo& info)	{ m_kAutoPotionInfo[type] = info; }

#ifdef ENABLE_BOSS_SHOWER_ON_ATLAS
		bool	LoadBossIconList();
		std::vector<TBossAtlas> GetBossIconVector() { return m_bossIconVec; }
#endif

	protected:
		TQuickSlot &	__RefLocalQuickSlot(int SlotIndex);
		TQuickSlot &	__RefGlobalQuickSlot(int SlotIndex);

		DWORD	__GetLevelAtk();
		DWORD	__GetStatAtk();
		DWORD	__GetWeaponAtk(DWORD dwWeaponPower);
		DWORD	__GetTotalAtk(DWORD dwWeaponPower, DWORD dwRefineBonus);
		DWORD	__GetRaceStat();
		DWORD	__GetHitRate();
		DWORD	__GetEvadeRate();

		void	__UpdateBattleStatus();

		void	__DeactivateSkillSlot(DWORD dwSlotIndex);
		void	__ActivateSkillSlot(DWORD dwSlotIndex);

		void	__OnPressSmart(CInstanceBase& rkInstMain, bool isAuto);
		void	__OnClickSmart(CInstanceBase& rkInstMain, bool isAuto);

		void	__OnPressItem(CInstanceBase& rkInstMain, DWORD dwPickedItemID);
		void	__OnPressActor(CInstanceBase& rkInstMain, DWORD dwPickedActorID, bool isAuto);
		void	__OnPressGround(CInstanceBase& rkInstMain, const TPixelPosition& c_rkPPosPickedGround);
		void	__OnPressScreen(CInstanceBase& rkInstMain);

		void	__OnClickActor(CInstanceBase& rkInstMain, DWORD dwPickedActorID, bool isAuto);
		void	__OnClickItem(CInstanceBase& rkInstMain, DWORD dwPickedItemID);
		void	__OnClickGround(CInstanceBase& rkInstMain, const TPixelPosition& c_rkPPosPickedGround);

		bool	__IsMovableGroundDistance(CInstanceBase& rkInstMain, const TPixelPosition& c_rkPPosPickedGround);

		bool	__GetPickedActorPtr(CInstanceBase** pkInstPicked);

		bool	__GetPickedActorID(DWORD* pdwActorID);
		bool	__GetPickedItemID(DWORD* pdwItemID);
		bool	__GetPickedGroundPos(TPixelPosition* pkPPosPicked);

		void	__ClearReservedAction();
		void	__ReserveClickItem(DWORD dwItemID);
		void	__ReserveClickActor(DWORD dwActorID);
		void	__ReserveClickGround(const TPixelPosition& c_rkPPosPickedGround);
		void	__ReserveUseSkill(DWORD dwActorID, DWORD dwSkillSlotIndex, DWORD dwRange);

		void	__ReserveProcess_ClickActor();

		void	__ShowPickedEffect(const TPixelPosition& c_rkPPosPickedGround);
		void	__SendClickActorPacket(CInstanceBase& rkInstVictim);

		void	__ClearAutoAttackTargetActorID();
		void	__SetAutoAttackTargetActorID(DWORD dwActorID);

		void	NEW_ShowEffect(int dwEID, TPixelPosition kPPosDst);

		void	NEW_SetMouseSmartState(int eMBS, bool isAuto);
		void	NEW_SetMouseMoveState(int eMBS);
		void	NEW_SetMouseCameraState(int eMBS);
		void	NEW_GetMouseDirRotation(float fScrX, float fScrY, float* pfDirRot);
		void	NEW_GetMultiKeyDirRotation(bool isLeft, bool isRight, bool isUp, bool isDown, float* pfDirRot);

		float	GetDegreeFromDirection(int iUD, int iLR);
		float	GetDegreeFromPosition(int ix, int iy, int iHalfWidth, int iHalfHeight);

		bool	CheckCategory(int iCategory);
		bool	CheckAbilitySlot(int iSlotIndex);

		void	RefreshKeyWalkingDirection();
		void	NEW_RefreshMouseWalkingDirection();

		// Instances
		void	RefreshInstances();

		bool	__CanShot(CInstanceBase& rkInstMain, CInstanceBase& rkInstTarget);
		bool	__CanUseSkill();

		bool	__CanMove();

		bool	__CanAttack();
		bool	__CanChangeTarget();

		bool	__CheckSkillUsable(DWORD dwSlotIndex);
		void	__UseCurrentSkill();
		void	__UseChargeSkill(DWORD dwSkillSlotIndex);
		bool	__UseSkill(DWORD dwSlotIndex);
		bool	__CheckSpecialSkill(DWORD dwSkillIndex);

		bool	__CheckRestSkillCoolTime(DWORD dwSkillSlotIndex);
		bool	__CheckShortLife(TSkillInstance & rkSkillInst, CPythonSkill::TSkillData& rkSkillData);
		bool	__CheckShortMana(TSkillInstance & rkSkillInst, CPythonSkill::TSkillData& rkSkillData);
		bool	__CheckShortArrow(TSkillInstance & rkSkillInst, CPythonSkill::TSkillData& rkSkillData);
		bool	__CheckDashAffect(CInstanceBase& rkInstMain);

		void	__SendUseSkill(DWORD dwSkillSlotIndex, DWORD dwTargetVID);
		void	__RunCoolTime(DWORD dwSkillSlotIndex);

		BYTE	__GetSkillType(DWORD dwSkillSlotIndex);

		bool	__IsReservedUseSkill(DWORD dwSkillSlotIndex);
		bool	__IsMeleeSkill(CPythonSkill::TSkillData& rkSkillData);
		bool	__IsChargeSkill(CPythonSkill::TSkillData& rkSkillData);
		DWORD	__GetSkillTargetRange(CPythonSkill::TSkillData& rkSkillData);
		bool	__SearchNearTarget();
		bool	__IsUsingChargeSkill();

		bool	__ProcessEnemySkillTargetRange(CInstanceBase& rkInstMain, CInstanceBase& rkInstTarget, CPythonSkill::TSkillData& rkSkillData, DWORD dwSkillSlotIndex);

		// Item
		bool	__HasEnoughArrow();
		bool	__HasItem(DWORD dwItemID);
		DWORD	__GetPickableDistance();

		// Target
		CInstanceBase*		__GetTargetActorPtr();
		void				__ClearTarget();
		DWORD				__GetTargetVID();
		void				__SetTargetVID(DWORD dwVID);
		bool				__IsSameTargetVID(DWORD dwVID);
		bool				__IsTarget();
		bool				__ChangeTargetToPickedInstance();

		CInstanceBase *		__GetSkillTargetInstancePtr(CPythonSkill::TSkillData& rkSkillData);
		CInstanceBase *		__GetAliveTargetInstancePtr();
		CInstanceBase *		__GetDeadTargetInstancePtr();

		BOOL				__IsRightButtonSkillMode();
#ifdef ENABLE_METIN_QUEUE
		bool				__IsLeftButtonQueueMode();
#endif

		// Update
		void				__Update_AutoAttack();
		void				__Update_NotifyGuildAreaEvent();

		// Emotion
		BOOL				__IsProcessingEmotion();

#ifdef ENABLE_PREMIUM_PRIVATE_SHOP_OFFICIAL
	public:
		void	__OnClickPrivateShop(DWORD dwPickedPrivateShopID);
		void	__OnPressPrivateShop(CInstanceBase& rkInstMain, DWORD dwPickedPrivateShopID);
		void	SendClickPrivateShopPacket(DWORD dwPrivateShopID);

		bool	__GetPickedPrivateShopID(DWORD* pdwPrivateShopID);
		void	__ReserveClickPrivateShop(DWORD dwPrivateShopID);
	private:
		DWORD					m_dwPSIDReserved;
#endif

#ifdef __ENABLE_POLYMORPH_SYSTEM__
	public:
		void SetPolySkin(BYTE polySkin) { m_bPolySkin = polySkin; }
		BYTE GetPolySkin() const { return m_bPolySkin; }

		void UnlockPolySkin(BYTE polySkin, BYTE status);
		bool IsUnlockedPolySkin(BYTE polySkin) const;

	private:
		BYTE m_bPolySkin = 0;
		std::map<BYTE, bool> m_ActivePolySkin;
#endif

	protected:
		PyObject *				m_ppyGameWindow;

		// Client Player Data
		std::map<DWORD, DWORD>	m_skillSlotDict;
		std::string				m_stName;
		DWORD					m_dwMainCharacterIndex;
		DWORD					m_dwRace;
		uint32_t                m_OriginalRace;
		DWORD					m_dwWeaponMinPower;
		DWORD					m_dwWeaponMaxPower;
		DWORD					m_dwWeaponMinMagicPower;
		DWORD					m_dwWeaponMaxMagicPower;
		DWORD					m_dwWeaponAddPower;

		// Todo
		DWORD					m_dwSendingTargetVID;
		float					m_fTargetUpdateTime;

		// Attack
		DWORD					m_dwAutoAttackTargetVID;

		// NEW_Move
		EMode					m_eReservedMode;
		float					m_fReservedDelayTime;

		float					m_fMovDirRot;

		bool					m_isUp;
		bool					m_isDown;
		bool					m_isLeft;
		bool					m_isRight;
		bool					m_isAtkKey;
		bool					m_isDirKey;
		bool					m_isCmrRot;
		bool					m_isSmtMov;
		bool					m_isDirMov;

		float					m_fCmrRotSpd;

		TPlayerStatus			m_playerStatus;

		UINT					m_iComboOld;
		DWORD					m_dwVIDReserved;
		DWORD					m_dwIIDReserved;

		DWORD					m_dwcurSkillSlotIndex;
		DWORD					m_dwSkillSlotIndexReserved;
		DWORD					m_dwSkillRangeReserved;

		TPixelPosition			m_kPPosInstPrev;
		TPixelPosition			m_kPPosReserved;

		// Emotion
		BOOL					m_bisProcessingEmotion;

		// Dungeon
		BOOL					m_isDestPosition;
		int						m_ixDestPos;
		int						m_iyDestPos;
		int						m_iLastAlarmTime;

		// Party
		std::map<DWORD, TPartyMemberInfo>	m_PartyMemberMap;

		// PVP
		std::set<DWORD>			m_ChallengeInstanceSet;
		std::set<DWORD>			m_RevengeInstanceSet;
		std::set<DWORD>			m_CantFightInstanceSet;

		// Private Shop
		bool					m_isOpenPrivateShop;
		bool					m_isObserverMode;

		bool					m_isFreeCameraMode;
		// Stamina
		BOOL					m_isConsumingStamina;
		float					m_fCurrentStamina;
		float					m_fConsumeStaminaPerSec;

		// Guild
		DWORD					m_inGuildAreaID;

		// Mobile
		BOOL					m_bMobileFlag;

		// System
		BOOL					m_sysIsCoolTime;
		BOOL					m_sysIsLevelLimit;

	protected:
		// Game Cursor Data
		TPixelPosition			m_MovingCursorPosition;
		float					m_fMovingCursorSettingTime;
		DWORD					m_adwEffect[EFFECT_NUM];

		DWORD					m_dwVIDPicked;
		DWORD					m_dwIIDPicked;
		int						m_aeMBFButton[MBT_NUM];

		DWORD					m_dwTargetVID;
		DWORD					m_dwTargetEndTime;
		DWORD					m_dwPlayTime;
#ifdef ENABLE_OFFLINE_SHOP_SYSTEM
		long long				m_dwCurrentOfflineShopMoney;
		long long				m_dwCurrentOfflineShopZarobek;
		long long				m_dwCurrentOfflineShopZarobekCheque;
		int						m_dwCurrentOfflineShopStatus;
		long long				m_dwCurrentOfflineShopChequeMoney;
		long long				m_dwCurrentOfflineShopPrzewidywalnyZarobekYang;
		long long				m_dwCurrentOfflineShopPrzewidywalnyZarobekCheque;
#endif
#ifdef ENABLE_BOSS_SHOWER_ON_ATLAS
		std::vector<TBossAtlas> m_bossIconVec;
#endif
		SAutoPotionInfo			m_kAutoPotionInfo[AUTO_POTION_TYPE_NUM];

	protected:
		float					MOVABLE_GROUND_DISTANCE;

	private:
		std::map<DWORD, DWORD> m_kMap_dwAffectIndexToSkillIndex;
#if defined(ENABLE_RENEWAL_AFFECT_SHOWER)
		std::map<DWORD, DWORD> m_kMap_dwSkillIndexToAffectIndex;
#endif
#ifdef ENABLE_OFFLINE_SHOP
	public:
		void	OpenPrivateMessage(const char* szOwnerName);
#endif

#ifdef ENABLE_BOT_CONTROL
	public:
		void OpenBotControlWindow();
		void BotControlTimeUpdate();
		void FailBotControl();
		bool GetBotControlState() const;
		void SetBotControlState(bool state);
		bool GetBotControlWindow() const { return m_bBotControlWindow; }
		void SetBotControWindow(bool state);
	protected:
		DWORD m_bBotControlState;
		bool m_bBotControlWindow;
#endif
#ifdef ENABLE_METIN_QUEUE
	public:
		void AddQueueList(CInstanceBase* victim);
		void UpdateQueue();
		void ClearQueueVector(BYTE idx);
		bool GetQueue() { return m_isQueue; }
		void CancelQueue();
	
		DWORD GetQueueNextTargetVID() const { return m_attackedStoneVID; };
		void SetTotalAutoFarmCount(int count) { m_iTotalAutoFarmCount = count; }
		int GetTotalAutoFarmCount() const { return m_iTotalAutoFarmCount; }
	
	private:
		bool m_isQueue;
		std::vector<DWORD> m_queueList;
		DWORD m_attackedStoneVID;
		int m_iTotalAutoFarmCount = 5;
#endif
};

extern const int c_iFastestSendingCount;
extern const int c_iSlowestSendingCount;
extern const float c_fFastestSendingDelay;
extern const float c_fSlowestSendingDelay;
extern const float c_fRotatingStepTime;

extern const float c_fComboDistance;
extern const float c_fPickupDistance;
extern const float c_fClickDistance;
//martysama0134's ceqyqttoaf71vasf9t71218
