#pragma once

#include "AbstractSingleton.h"
#include "../UserInterface/locale_inc.h"

class CInstanceBase;

#ifdef ENABLE_PERFORMANCE_IMPROVEMENTS_NEW
namespace NPlayerData
{
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
};
#endif

class IAbstractPlayer : public TAbstractSingleton<IAbstractPlayer>
{
	public:
		IAbstractPlayer() {}
		virtual ~IAbstractPlayer() {}

		virtual DWORD	GetMainCharacterIndex() = 0;
		virtual void	SetMainCharacterIndex(int iIndex) = 0;
		virtual bool	IsMainCharacterIndex(DWORD dwIndex) = 0;

#ifdef ENABLE_LONG_LONG
		virtual long long		GetStatus(DWORD dwType) = 0;
#else
		virtual int		GetStatus(DWORD dwType) = 0;
#endif

		virtual const char *	GetName() = 0;

		virtual void	SetRace(DWORD dwRace) = 0;
		virtual void    SetOriginalRace(uint32_t race) = 0;

		virtual void	StartStaminaConsume(DWORD dwConsumePerSec, DWORD dwCurrentStamina) = 0;
		virtual void	StopStaminaConsume(DWORD dwCurrentStamina) = 0;

		virtual bool	IsPartyMemberByVID(DWORD dwVID) = 0;
		virtual bool	PartyMemberVIDToPID(DWORD dwVID, DWORD * pdwPID) = 0;
		virtual bool	IsSamePartyMember(DWORD dwVID1, DWORD dwVID2) = 0;

		virtual void	SetItemData(TItemPos itemPos, const TItemData & c_rkItemInst) = 0;
#ifdef ENABLE_EXTENDED_ITEM_COUNT
		virtual void	SetItemCount(TItemPos itemPos, uint16_t byCount) = 0;
#else
		virtual void	SetItemCount(TItemPos itemPos, BYTE byCount) = 0;
#endif
		virtual void	SetItemMetinSocket(TItemPos itemPos, DWORD dwMetinSocketIndex, DWORD dwMetinNumber) = 0;
		virtual void	SetItemAttribute(TItemPos itemPos, DWORD dwAttrIndex, BYTE byType, short sValue) = 0;

		virtual DWORD	GetItemIndex(TItemPos itemPos) = 0;
		virtual DWORD	GetItemFlags(TItemPos itemPos) = 0;
		virtual DWORD	GetItemCount(TItemPos itemPos) = 0;

		virtual bool	IsEquipItemInSlot(TItemPos itemPos) = 0;

		virtual void	AddQuickSlot(int QuickslotIndex, char IconType, char IconPosition) = 0;
		virtual void	DeleteQuickSlot(int QuickslotIndex) = 0;
		virtual void	MoveQuickSlot(int Source, int Target) = 0;

		virtual void	SetWeaponPower(DWORD dwMinPower, DWORD dwMaxPower, DWORD dwMinMagicPower, DWORD dwMaxMagicPower, DWORD dwAddPower) = 0;

		virtual void	SetTarget(DWORD dwVID, BOOL bForceChange = TRUE) = 0;
		virtual void	NotifyCharacterUpdate(DWORD dwVID) = 0;
		virtual void	NotifyCharacterDead(DWORD dwVID) = 0;
		virtual void	NotifyDeletingCharacterInstance(DWORD dwVID) = 0;
		virtual void	NotifyChangePKMode() = 0;

		virtual void	SetObserverMode(bool isEnable) = 0;
		virtual void	SetMobileFlag(BOOL bFlag) = 0;
		virtual void	SetComboSkillFlag(BOOL bFlag) = 0;

		virtual void	StartEmotionProcess() = 0;
		virtual void	EndEmotionProcess() = 0;

		virtual CInstanceBase* NEW_GetMainActorPtr() = 0;
};
//martysama0134's ceqyqttoaf71vasf9t71218
