#ifndef __INC_METIN_II_GAME_SHOP_H__
#define __INC_METIN_II_GAME_SHOP_H__

enum
{
	SHOP_MAX_DISTANCE = 1000
};

class CGrid;

/* ---------------------------------------------------------------------------------- */
class CShop
{
	public:
		typedef struct shop_item
		{
			DWORD	vnum;
#ifdef ENABLE_LONG_LONG
			int64_t		price;
#else
			int32_t		price;
#endif
#ifdef ENABLE_CHEQUE_SYSTEM
			int64_t		cheque;
#endif
#ifdef __EXTENDED_ITEM_COUNT__
			uint16_t	count;
#else
			BYTE		count;
#endif
			LPITEM	pkItem;
			int		itemid;
#ifdef ENABLE_BUY_WITH_ITEM
			TShopItemPrice	itemprice[MAX_SHOP_PRICES];
#endif
			shop_item()
			{
				vnum = 0;
				price = 0;
#ifdef ENABLE_CHEQUE_SYSTEM
				cheque = 0;
#endif
				count = 0;
				itemid = 0;
				pkItem = NULL;
#ifdef ENABLE_BUY_WITH_ITEM
				memset(itemprice, 0, sizeof(itemprice));
#endif
			}
		} SHOP_ITEM;

		CShop();
		virtual ~CShop(); // @fixme139 (+virtual)

		bool	Create(DWORD dwVnum, DWORD dwNPCVnum, TShopItemTable * pItemTable);

#ifdef __EXTENDED_ITEM_COUNT__
		void			SetShopItems(TShopItemTable* pItemTable, uint16_t bItemCount);
#else
		void			SetShopItems(TShopItemTable* pItemTable, BYTE bItemCount);
#endif
		virtual void	SetPCShop(LPCHARACTER ch);
		virtual bool	IsPCShop()	{ return m_pkPC ? true : false; }

		virtual bool	AddGuest(LPCHARACTER ch,DWORD owner_vid, bool bOtherEmpire);
		void			RemoveGuest(LPCHARACTER ch);
		void			RemoveAllGuests();
#ifdef ENABLE_LONG_LONG
		virtual int64_t	Buy(LPCHARACTER ch, BYTE pos);
#else
		virtual int	Buy(LPCHARACTER ch, BYTE pos);
#endif
		void			BroadcastUpdateItem(BYTE pos);
		int				GetNumberByVnum(DWORD dwVnum);
		virtual bool	IsSellingItem(DWORD itemID);

		DWORD	GetVnum() { return m_dwVnum; }
		DWORD	GetNPCVnum() { return m_dwNPCVnum; }

	protected:
		void	Broadcast(const void * data, int bytes);

	protected:
		DWORD				m_dwVnum;
		DWORD				m_dwNPCVnum;

#ifdef ENABLE_PUNKTY_OSIAGNIEC
		bool				m_IsPktOsiagShop;
#endif

		CGrid *				m_pGrid;

		typedef std::unordered_map<LPCHARACTER, bool> GuestMapType;
		GuestMapType m_map_guest;
		std::vector<SHOP_ITEM>		m_itemVector;

		LPCHARACTER			m_pkPC;
};

#endif
//martysama0134's ceqyqttoaf71vasf9t71218
