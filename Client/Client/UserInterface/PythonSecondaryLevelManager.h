#pragma once
#include "StdAfx.h"

#ifdef ENABLE_SECONDARY_LEVEL
#include "Packet.h"

class CSecondaryLevelManager : public CSingleton<CSecondaryLevelManager>
{
public:
	CSecondaryLevelManager();
	~CSecondaryLevelManager();

	void	Initialize();

	void	AppendSecondaryLevelInformation(const TPacketGCSecondaryLevelInfo* pSecondaryLevelInfo);

	// getters
	DWORD	GetApplyType(int32_t num) const;
	DWORD	GetNextApplyType(int32_t num) const;
	long	GetApplyValue(int32_t num) const;
	long	GetNextApplyValue(int32_t num) const;
	DWORD	GetRequiredItemVnum(int32_t num) const;
	long	GetRequiredItemCount(int32_t num) const;
	int64_t	GetRequiredGold(int32_t option) const;
	DWORD	GetChance(int32_t option) const;
private:
	std::vector<TPacketGCSecondaryLevelInfo>  m_SecondaryLevelsVector;
};
#endif