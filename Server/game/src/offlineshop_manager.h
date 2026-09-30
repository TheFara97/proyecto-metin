#pragma once
#ifdef ENABLE_OFFLINE_SHOP_SYSTEM
class COfflineShopManager : public singleton<COfflineShopManager>
{
	public:
		typedef std::map<DWORD, COfflineShop *> TShopMap;
		typedef std::map<DWORD, DWORD> TOfflineShopMap;
	public:
		COfflineShopManager();
		~COfflineShopManager();

		bool StartShopping(LPCHARACTER pkChr, LPCHARACTER pkChrShopKeeper);
		void StopShopping(LPCHARACTER ch);

		void Buy(LPCHARACTER ch, BYTE bPos);

		LPOFFLINESHOP	CreateOfflineShop(LPCHARACTER npc, DWORD dwOwnerPID);
		LPOFFLINESHOP	FindOfflineShop(DWORD dwVID);
		void			DestroyOfflineShop(LPCHARACTER ch, DWORD dwVID, bool bDestroyAll = false);
		void			Giveback(LPCHARACTER ch);
		void			Giveback2(LPCHARACTER ch);

		void			AddItem(LPCHARACTER ch, BYTE bDisplayPos, TItemPos itemPos, int64_t iPrice, int64_t iPrice2);
		void			WithdrawMoney(LPCHARACTER ch, int64_t dwRequiredMoney);
		void			WithdrawCheque(LPCHARACTER ch, int64_t dwRequiredCheque);
		void			RemoveItem(LPCHARACTER ch, BYTE bPos);
		void			Refresh(LPCHARACTER ch);
		void			RefreshMoney(LPCHARACTER ch);
		void			RefreshCheque(LPCHARACTER ch);

		DWORD			FindMyOfflineShop(DWORD dwPID);
		DWORD			GetPidByVid(DWORD dwVID);
		BYTE			LeftItemCount(LPCHARACTER ch);
#ifdef ENABLE_VS_SHOP_SEARCH
		void			SearchInShops(LPCHARACTER ch, int search_type, int vnum, const std::string& vnumString, int type, int subtype);
		void			ClearShopSearch(LPCHARACTER ch);
#endif
	private:
		TOfflineShopMap	m_Map_pkOfflineShopByNPC2;
		TShopMap		m_map_pkOfflineShopByNPC;
};
#endif


