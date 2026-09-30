#pragma once
#include "Packet.h"
#ifdef ENABLE_AVERAGE_PRICE
class COfflineShopAveragePrice : public singleton<COfflineShopAveragePrice>
{
public:
	COfflineShopAveragePrice();
	virtual ~COfflineShopAveragePrice();

	long long GetAveragePrice(DWORD vnum);
	void	SetAveragePrice(DWORD vnum, long long price);
private:
	std::unordered_map<DWORD, long long> m_mapAveragePrice;
};
#endif