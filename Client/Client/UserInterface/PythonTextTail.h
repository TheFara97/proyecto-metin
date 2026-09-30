#pragma once

#include "../eterBase/Singleton.h"

class CPythonTextTail : public CSingleton<CPythonTextTail>
{
	public:
		typedef struct STextTail
		{
			CGraphicTextInstance*			pTextInstance;
			CGraphicTextInstance*			pOwnerTextInstance;

			CGraphicMarkInstance*			pMarkInstance;
			CGraphicTextInstance*			pGuildNameTextInstance;

			CGraphicTextInstance*			pTitleTextInstance;
			CGraphicTextInstance*			pLevelTextInstance;
#ifdef TITLE_SYSTEM_BYLUZER
			CGraphicTextInstance*			pGradeTextInstance;
#endif
#ifdef ENABLE_SECONDARY_LEVEL
			CGraphicTextInstance*			pSecondaryLevelTextInstance;
#endif
#ifdef ENABLE_ENLIGHTENMENT_SYSTEM
			CGraphicTextInstance*			pEnlightenmentLevelTextInstance;
#endif
#ifdef ENABLE_TITLE_ACHIEVEMENT_SYSTEM
			CGraphicTextInstance*			psTitleAchievementNameTextInstance;
			CGraphicTextInstance*			psTitleAchievementPremiumNameTextInstance;
#endif
			CGraphicObjectInstance *		pOwner;

			DWORD							dwVirtualID;

			float							x, y, z;
			float							fDistanceFromPlayer;
			D3DXCOLOR						Color;
			BOOL							bNameFlag;

			float							xStart, yStart;
			float							xEnd, yEnd;

			DWORD							LivingTime;
#ifdef ENABLE_OFFLINE_SHOP
#ifdef ENABLE_SHOP_IN_CITIES
			bool							bIsShop;
			bool							bRender;
#endif
#endif

			float							fHeight;

			BOOL                            bIsPC;

			STextTail() {}
			virtual ~STextTail() {}
		} TTextTail;

		typedef std::map<DWORD, TTextTail*>		TTextTailMap;
		typedef std::list<TTextTail*>			TTextTailList;
		typedef TTextTailMap					TChatTailMap;

	public:
		CPythonTextTail(void);
		virtual ~CPythonTextTail(void);

		void GetInfo(std::string* pstInfo);

		void Initialize();
		void Destroy();
		void Clear();

		void UpdateAllTextTail();
		void UpdateShowingTextTail();
		void Render();

#ifdef ENABLE_SHOP_IN_CITIES
		void RegisterShopInstanceTextTail(DWORD dwVirtualID, const char* c_szName, CGraphicObjectInstance* pOwner);
		void DeleteShopTextTail(DWORD VirtualID);
		TTextTail* RegisterShopTextTail(DWORD dwVirtualID, const char* c_szText, CGraphicObjectInstance* pOwner);
		bool GetPickedNewShop(DWORD* pdwVID);
#endif
		void ArrangeTextTail();
		void HideAllTextTail();
		void ShowAllTextTail();
		void ShowCharacterTextTail(DWORD VirtualID);
		void ShowItemTextTail(DWORD VirtualID);

		void RegisterCharacterTextTail(DWORD dwGuildID, DWORD dwVirtualID, const D3DXCOLOR & c_rColor, float fAddHeight=10.0f);
#ifdef ENABLE_EXTENDED_ITEMNAME_ON_GROUND
		void RegisterItemTextTail(DWORD VirtualID, const char* c_szText, CGraphicObjectInstance* pOwner, bool bHasAttr = false);
#else
		void RegisterItemTextTail(DWORD VirtualID, const char* c_szText, CGraphicObjectInstance* pOwner);
#endif
		void RegisterChatTail(DWORD VirtualID, const char * c_szChat);
		void RegisterInfoTail(DWORD VirtualID, const char * c_szChat);
		void SetCharacterTextTailColor(DWORD VirtualID, const D3DXCOLOR & c_rColor);
		void SetItemTextTailOwner(DWORD dwVID, const char * c_szName);
		void DeleteCharacterTextTail(DWORD VirtualID);
		void DeleteItemTextTail(DWORD VirtualID);

		int Pick(int ixMouse, int iyMouse);
		void SelectItemName(DWORD dwVirtualID);

		bool GetTextTailPosition(DWORD dwVID, float* px, float* py, float* pz);
		bool IsChatTextTail(DWORD dwVID);

		void EnablePKTitle(BOOL bFlag);
		void AttachTitle(DWORD dwVID, const char * c_szName, const D3DXCOLOR& c_rColor);
		void DetachTitle(DWORD dwVID);

		void AttachLevel(DWORD dwVID, const char* c_szText, const D3DXCOLOR& c_rColor);
		void DetachLevel(DWORD dwVID);
#ifdef TITLE_SYSTEM_BYLUZER
		void AttachGrade(DWORD dwVID, const char* c_szText, const D3DXCOLOR& c_rColor);
		void DetachGrade(DWORD dwVID);
#endif
#ifdef ENABLE_SECONDARY_LEVEL
		void AttachSecondaryLevel(DWORD dwVID, const char* c_szText, const D3DXCOLOR& c_rColor);
		void DetachSecondaryLevel(DWORD dwVID);
#endif
#ifdef ENABLE_ENLIGHTENMENT_SYSTEM
		void AttachEnlightenmentLevel(DWORD dwVID, const char* c_szText, const D3DXCOLOR& c_rColor);
		void DetachEnlightenmentLevel(DWORD dwVID);
#endif
#ifdef ENABLE_TITLE_ACHIEVEMENT_SYSTEM
		void AttachTitleAchievement(DWORD dwVID, const char* c_szName, const D3DXCOLOR& c_rColor);
		void DetachTitleAchievement(DWORD dwVID);
		void AttachTitleAchievementPremium(DWORD dwVID, const char* c_szName, const D3DXCOLOR& c_rColor);
		void DetachTitleAchievementPremium(DWORD dwVID);
#endif

	protected:
		TTextTail * RegisterTextTail(DWORD dwVirtualID, const char * c_szText, CGraphicObjectInstance * pOwner, float fHeight, const D3DXCOLOR & c_rColor);
		void DeleteTextTail(TTextTail * pTextTail);

		void UpdateTextTail(TTextTail * pTextTail);
		void RenderTextTailBox(TTextTail * pTextTail);
		void RenderTextTailName(TTextTail * pTextTail);
		void UpdateDistance(const TPixelPosition & c_rCenterPosition, TTextTail * pTextTail);

		bool isIn(TTextTail * pSource, TTextTail * pTarget);

	protected:
		TTextTailMap				m_CharacterTextTailMap;
		TTextTailMap				m_ItemTextTailMap;
		TChatTailMap				m_ChatTailMap;

		TTextTailList				m_CharacterTextTailList;
		TTextTailList				m_ItemTextTailList;
#ifdef ENABLE_SHOP_IN_CITIES
		TTextTailMap				m_ShopTextTailMap;
#endif

	private:
		CDynamicPool<STextTail>		m_TextTailPool;


#ifdef ENABLE_PREMIUM_PRIVATE_SHOP_OFFICIAL
public:
	void ShowPrivateShopTextTail(DWORD dwVirtualID);
	void RegisterPrivateShopTextTail(DWORD dwVirtualID);
	void DeletePrivateShopTextTail(DWORD dwVirtualID);

protected:
	TTextTailMap				m_PrivateShopTextTailMap;
	TTextTailList				m_PrivateShopTextTailList;
#endif
};
//martysama0134's ceqyqttoaf71vasf9t71218
