#ifndef __INCLUDE_HEADER_PYTHON_OFFLINESHO__
#define __INCLUDE_HEADER_PYTHON_OFFLINESHO__

#ifdef ENABLE_OFFLINE_SHOP
#include "PythonBackground.h"
#ifdef ENABLE_SHOP_DECORATION
#include "InstanceBase.h"
#include "PythonCharacterManager.h"
#endif

namespace offlineshop
{
	template <class T>
	void ZeroObject(T& obj)
	{
		memset(&obj, 0, sizeof(obj));
	}

	template <class T>
	void CopyObject(T& objDest, const T& objSrc)
	{
		memcpy(&objDest, &objSrc, sizeof(objDest));
	}

	template <class T>
	void CopyContainer(T& objDest, const T& objSrc)
	{
		objDest = objSrc;
	}

	template <class T>
	void DeletePointersContainer(T& obj)
	{
		typename T::iterator it = obj.begin();
		for (; it != obj.end(); it++)
			delete(*it);
	}

	template <class T, template <class> class S, typename S<T>::iterator >
	void ForEach(S<T>& container, std::function<void(T&)> func)
	{
		S<T>::iterator it = container.begin(), iter;

		while ((iter = it++) != container.end())
			func(*iter);
	}

	template <class T, class K, template <class, class> class S, typename S<K, T>::iterator >
	void ForEach(S<K, T>& container, std::function<void(T&)> func)
	{
		S<K, T>::iterator it = container.begin(), iter;

		while ((iter = it++) != container.end())
			func(iter->second);
	}
}

namespace offlineshop
{
	enum eConstOfflineshop {
		OFFLINESHOP_DURATION_MAX_DAYS = 8,
		OFFLINESHOP_DURATION_MAX_HOURS = 23,
		OFFLINESHOP_DURATION_MAX_MINUTES = OFFLINESHOP_DURATION_MAX_DAYS * 24 * 60,
		OFFLINESHOP_MAX_FILTER_HISTORY_SIZE = 50,
	};
}

#ifdef ENABLE_SHOP_IN_CITIES
namespace offlineshop
{
	class ShopInstance
	{
	public:
		ShopInstance()
		{
			m_dwVID = 0;
			m_iType = 0;
			m_stSign.clear();
		}

		~ShopInstance()
		{
			m_dwVID = 0;
			m_iType = 0;
			m_stSign.clear();
		}

		void SetVID(DWORD dwVID)
		{
			m_dwVID = dwVID;
		}

		void SetShopType(int iType)
		{
			m_iType = iType;
		}

		void SetSign(const char* cpszSign)
		{
			m_stSign = cpszSign;
		}

		void Show(float x, float y, float z
#ifdef ENABLE_SHOP_DECORATION
		, DWORD dwShopDecoration
#endif
		)
		{
#ifdef ENABLE_SHOP_DECORATION
			m_thingInstance.SetActorType(CActorInstance::TYPE_NPC);
			m_thingInstance.SetRace(dwShopDecoration);
			m_thingInstance.SetShape(0);
			m_thingInstance.SetMotionMode(CRaceMotionData::MODE_GENERAL);
			m_thingInstance.InterceptLoopMotion(CRaceMotionData::NAME_WAIT);
			m_thingInstance.RefreshActorInstance();
			TPixelPosition c_rPixelPos;
			c_rPixelPos.x = x;
			c_rPixelPos.y = -y;
			c_rPixelPos.z = z;
			m_thingInstance.SetPixelPosition(c_rPixelPos);
			m_thingInstance.INSTANCEBASE_Transform();
			m_thingInstance.INSTANCEBASE_Deform();
#else
			m_thingInstance.Clear();
			m_thingInstance.ReserveModelThing(1);
			m_thingInstance.ReserveModelInstance(1);
			m_thingInstance.RegisterModelThing(0, (CGraphicThing*)CResourceManager::Instance().GetResourcePointer("offlineshop/shop.gr2"));
			m_thingInstance.SetModelInstance(0, 0, 0);
			m_thingInstance.SetPosition(x, -y, z);
			m_thingInstance.Show();
			m_thingInstance.Update();
			m_thingInstance.Transform();
			m_thingInstance.Deform();
#endif
		}

		DWORD GetVID() const
		{
			return m_dwVID;
		}

		std::string GetSign() const
		{
			return m_stSign;
		}

		int GetType() const
		{
			return m_iType;
		}
#ifdef ENABLE_SHOP_DECORATION
		CActorInstance* GetThingInstancePtr() 
#else
		CGraphicThingInstance* GetThingInstancePtr()
#endif
		{
			return &m_thingInstance;
		}

		void Clear()
		{
			m_thingInstance.Clear();
			m_dwVID = 0;
			m_iType = 0;
			m_stSign.clear();
		}

		void Render()
		{
			m_thingInstance.Render();
		}

		void BlendRender()
		{
			m_thingInstance.BlendRender();
		}

#ifdef ENABLE_SHOP_DECORATION
		void Deform()
		{
			m_thingInstance.INSTANCEBASE_Deform();
		}
#endif
		void Update()
		{
			m_thingInstance.Update();
#ifdef ENABLE_SHOP_DECORATION
			m_thingInstance.MotionProcess(false);
#endif
		}

	private:
#ifdef ENABLE_SHOP_DECORATION
		CActorInstance			m_thingInstance;
#else
		CGraphicThingInstance	m_thingInstance;
#endif
		DWORD					m_dwVID;
		int						m_iType;
		std::string				m_stSign;
	};
}
#endif

class CPythonOfflineshop : public CSingleton<CPythonOfflineshop>
{
public:
	typedef struct SDatetime
	{
		BYTE bMinutes;
		BYTE bHour;
		BYTE bDay;
		BYTE bMonth;
		int iYear;
	} TDatetime;

	static void GetNowAsDatetime(TDatetime& datetime)
	{
		SYSTEMTIME time;
		GetLocalTime(&time);
		datetime.bMinutes = (BYTE)time.wMinute;
		datetime.bHour = (BYTE)time.wHour;
		datetime.bDay = (BYTE)time.wDay;
		datetime.bMonth = (BYTE)time.wMonth;
		datetime.iYear = (int)time.wYear;
	}

	static int AllocPatternID()
	{
		static int id = 0;
		return id++;
	}

public:
	CPythonOfflineshop();
	~CPythonOfflineshop();

	void		SetWindowObjectPointer(PyObject* poWindow);
	PyObject* GetOfflineshopBoard();
#ifdef ENABLE_SHOP_SEARCH
	void		SetWindowObjectPointerForSearch(PyObject* poWindowForSearch);
	PyObject* GetShopSearchBoard();
#endif
	void	ShopListAddItem(const offlineshop::TShopInfo& shop);
	void	ShopListShow();
	void	ShopListClear();
	void	BuyFromSearch(DWORD dwOwnerID, DWORD dwItemID);
	void	OpenShop(const offlineshop::TShopInfo& shop, const std::vector<offlineshop::TItemInfo>& vec);
	void	OpenShopOwner(const offlineshop::TShopInfo& shop,
		const std::vector<offlineshop::TItemInfo>& vec);
	void	OpenShopOwnerNoShop();
	void	ShopClose();
	void	ShopFilterResult(const std::vector<offlineshop::TItemInfo>& vec);
	void	SafeboxRefresh(const unsigned long long& valute, const std::vector<DWORD>& ids, const std::vector<offlineshop::TItemInfoEx>& item);
	void	RefreshItemNameMap();
	void	ShopBuilding_AddInventoryItem(int iSlot);
	void	ShopBuilding_AddItem(int iWin, int iSlot);

#ifdef ENABLE_SHOP_IN_CITIES
	void	InsertEntity(DWORD dwVID, int iType, const char* szName, long x, long y, long z
#ifdef ENABLE_SHOP_DECORATION
	, DWORD dwShopDecoration
#endif
	);
	void	RemoveEntity(DWORD dwVID);
	void	RenderEntities();
	void	UpdateEntities();
	bool	GetShowNameFlag();
	void	SetShowNameFlag(bool flag);
	void	DeleteEntities();
#endif
#ifdef ENABLE_OFFLINESHOP_NOTIFICATION
	void	SendNotification(DWORD dwItemID, int64_t dwItemPrice,
#ifdef ENABLE_EXTENDED_COUNT_ITEMS
		MAX_COUNT dwItemCount
#else
		uint8_t dwItemCount
#endif
	);
#endif

#ifdef ENABLE_AVERAGE_PRICE
	void	SetAveragePrice(long long price);
#endif

private:
	PyObject* m_poWindow;
#ifdef ENABLE_SHOP_SEARCH
	PyObject* m_poWindowForSearch;
#endif

#ifdef ENABLE_SHOP_IN_CITIES
	std::vector<offlineshop::ShopInstance*>
		m_vecShopInstance;
	bool			m_bIsShowName;
#endif
};

extern void initofflineshop();
#endif
#endif