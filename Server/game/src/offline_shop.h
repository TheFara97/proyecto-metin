#pragma once

#ifdef ENABLE_OFFLINE_SHOP_SYSTEM
enum
{
	OFFLINE_SHOP_MAX_DISTANCE = 1500,
};

class COfflineShop
{
	public:
		COfflineShop();
		~COfflineShop();

		virtual void	SetOfflineShopNPC(LPCHARACTER npc);
		virtual bool	IsOfflineShopNPC(){ return m_pkOfflineShopNPC ? true : false; }

		virtual bool	AddGuest(LPCHARACTER ch, LPCHARACTER npc);
		
		void			RemoveGuest(LPCHARACTER ch);
		void			RemoveAllGuest();
		void			Destroy(LPCHARACTER npc);
		
		virtual int64_t	Buy(LPCHARACTER ch, BYTE bPos);
		
		void			BroadcastUpdateItem(BYTE bPos, DWORD dwPID, bool bDestroy = false);

		void			BroadcastUpdatePrice(BYTE bPos, int64_t dwPrice, int64_t dwPrice2);

		void			Refresh(LPCHARACTER ch);

		bool			RemoveItem(DWORD dwVID, BYTE bPos);
		BYTE			GetLeftItemCount(DWORD dwPID);

		void			SetActionPulse(DWORD dwPulse)	{ dwActionPulse = dwPulse; }
		DWORD			GetActionPulse()		{ return dwActionPulse; }
	protected:
		void			Broadcast(const void * data, int bytes);

	private:
		CGrid *				m_pGrid;

		typedef TR1_NS::unordered_map<LPCHARACTER, bool> GuestMapType;
		GuestMapType m_map_guest;

		LPCHARACTER m_pkOfflineShopNPC;
		
		DWORD		dwActionPulse;
};
#endif
