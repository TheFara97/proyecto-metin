#include "StdAfx.h"

#ifdef ENABLE_AVERAGE_PRICE
#include "OfflineShopAveragePrice.h"

COfflineShopAveragePrice::COfflineShopAveragePrice()
{
	m_mapAveragePrice.clear();
}

COfflineShopAveragePrice::~COfflineShopAveragePrice()
{
	m_mapAveragePrice.clear();
}

long long COfflineShopAveragePrice::GetAveragePrice(DWORD vnum)
{
	auto it = m_mapAveragePrice.find(vnum);
	if (it != m_mapAveragePrice.end())
	{
		return it->second;
	}
	
	return -1;
}

void COfflineShopAveragePrice::SetAveragePrice(DWORD vnum, long long price)
{
	m_mapAveragePrice.insert(std::make_pair(vnum, price));
}

#endif