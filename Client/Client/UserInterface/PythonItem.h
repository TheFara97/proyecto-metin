#pragma once

#include "../EterGrnLib/ThingInstance.h"

class CItemData;

class CPythonItem : public CSingleton<CPythonItem>
{
	public:
		enum
		{
			INVALID_ID = 0xffffffff,
		};

		enum
		{
			VNUM_MONEY = 1,
		};

		enum
		{
			USESOUND_NONE,
			USESOUND_DEFAULT,
			USESOUND_ARMOR,
			USESOUND_WEAPON,
			USESOUND_BOW,
			USESOUND_ACCESSORY,
			USESOUND_POTION,
			USESOUND_PORTAL,
			USESOUND_NUM,
		};

		enum
		{
			DROPSOUND_DEFAULT,
			DROPSOUND_ARMOR,
			DROPSOUND_WEAPON,
			DROPSOUND_BOW,
			DROPSOUND_ACCESSORY,
			DROPSOUND_NUM
		};

		enum EDropItemEffects
		{
			DROP_ITEM_EFFECT_NORMAL,
			DROP_ITEM_EFFECT_RARE,
			DROP_ITEM_EFFECT_EPIC,
			DROP_ITEM_EFFECT_LEGENARY,
			DROP_ITEM_EFFECT_NUM,
		};

		typedef struct SGroundItemInstance
		{
			DWORD					dwVirtualNumber;
			D3DXVECTOR3				v3EndPosition;

			D3DXVECTOR3				v3RotationAxis;
			D3DXQUATERNION			qEnd;
			D3DXVECTOR3				v3Center;
			CGraphicThingInstance	ThingInstance;
			DWORD					dwStartTime;
			DWORD					dwEndTime;

			DWORD					eDropSoundType;

			bool					bAnimEnded;
			bool Update();
			void Clear();

			DWORD					dwEffectInstanceIndex;
			std::string				stOwnership;

			static void	__PlayDropSound(DWORD eItemType, const D3DXVECTOR3& c_rv3Pos);
			static std::string		ms_astDropSoundFileName[DROPSOUND_NUM];

			SGroundItemInstance() {}
			virtual ~SGroundItemInstance() {}
		} TGroundItemInstance;

		typedef std::map<DWORD, TGroundItemInstance *>	TGroundItemInstanceMap;

	public:
		CPythonItem(void);
		virtual ~CPythonItem(void);

		// Initialize
		void	Destroy();
		void	Create();

		void	PlayUseSound(DWORD dwItemID);
		void	PlayDropSound(DWORD dwItemID);
		void	PlayUsePotionSound();

		void	SetUseSoundFileName(DWORD eItemType, const std::string& c_rstFileName);
		void	SetDropSoundFileName(DWORD eItemType, const std::string& c_rstFileName);

		void	GetInfo(std::string* pstInfo);

		void	DeleteAllItems();

		void	Render();
		void	Update(const POINT& c_rkPtMouse);

#ifdef ENABLE_EXTENDED_ITEMNAME_ON_GROUND
		void	CreateItem(DWORD dwVirtualID, DWORD dwVirtualNumber, float x, float y, float z, int count, bool bDrop = true, long alSockets[ITEM_SOCKET_SLOT_MAX_NUM] = {}, TPlayerItemAttribute aAttrs[ITEM_ATTRIBUTE_SLOT_MAX_NUM] = {});
#else
		void	CreateItem(DWORD dwVirtualID, DWORD dwVirtualNumber, float x, float y, float z, int count, bool bDrop = true);
#endif
		void	DeleteItem(DWORD dwVirtualID);
		void	SetOwnership(DWORD dwVID, const char * c_pszName);
		bool	GetOwnership(DWORD dwVID, const char ** c_pszName);

		BOOL	GetGroundItemPosition(DWORD dwVirtualID, TPixelPosition * pPosition);

		bool	GetPickedItemID(DWORD* pdwPickedItemID);

		bool	GetCloseItem(const std::string& myName, const TPixelPosition & c_rPixelPosition, DWORD* pdwItemID, DWORD dwDistance=300);
#ifdef __FORCE_PICKUP__
		std::vector<uint32_t> GetCloseItems(const TPixelPosition & c_rPixelPosition, DWORD dwDistance=300);
#endif
		bool	GetCloseMoney(const TPixelPosition & c_rPixelPosition, DWORD* dwItemID, DWORD dwDistance=300);
#ifdef ENABLE_QUICK_PICKUP
		bool	GetAllCloseItems(const std::string& myName, const TPixelPosition& c_rPixelPosition, std::vector<DWORD>& itemVidList);
#endif

		DWORD	GetVirtualNumberOfGroundItem(DWORD dwVID);

		void	BuildNoGradeNameData(int iType);
		DWORD	GetNoGradeNameDataCount();
		CItemData * GetNoGradeNameDataPtr(DWORD dwIndex);

	protected:
		DWORD	__Pick(const POINT& c_rkPtMouse);

		DWORD	__GetUseSoundType(const CItemData& c_rkItemData);
		DWORD	__GetDropSoundType(const CItemData& c_rkItemData);

		void	__RegisterEffect(int iIndex, const char* szFile);
		int		__AttachEffect(DWORD dwVnum, BYTE byType, BYTE bySubType);

	protected:
		TGroundItemInstanceMap				m_GroundItemInstanceMap;
		CDynamicPool<TGroundItemInstance>	m_GroundItemInstancePool;

		//DWORD m_dwDropItemEffectID;
		DWORD m_dwDropItemEffectID[DROP_ITEM_EFFECT_NUM];
		DWORD m_dwPickedItemID;

		int m_nMouseX;
		int m_nMouseY;

		std::string m_astUseSoundFileName[USESOUND_NUM];

		std::vector<CItemData *> m_NoGradeNameItemData;
};
//martysama0134's ceqyqttoaf71vasf9t71218
