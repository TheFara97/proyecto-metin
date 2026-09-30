#pragma once

#ifdef ENABLE_ACCE_COSTUME_SYSTEM
#include "Packet.h"

class CPythonAcce : public CSingleton<CPythonAcce>
{
public:
	long long	dwPrice;
	long long	dwCheque;
	BYTE	bChance;
	typedef std::vector<TAcceMaterial> TAcceMaterials;

public:
	CPythonAcce();
	virtual ~CPythonAcce();

	void	Clear();
	void	AddMaterial(long long dwRefPrice, long long dwRefPrice2, BYTE chance, BYTE bPos, TItemPos tPos);
	void	AddResult(DWORD dwItemVnum, DWORD dwMinAbs, DWORD dwMaxAbs);
	void	RemoveMaterial(long long dwRefPrice, long long dwRefPrice2, BYTE chance, BYTE bPos);
	long long	GetPrice() {return dwPrice;}
	long long	GetCheque() { return dwCheque; }
	BYTE	GetChance() { return bChance; }
	bool	GetAttachedItem(BYTE bPos, BYTE & bHere, WORD & wCell);
	void	GetResultItem(DWORD & dwItemVnum, DWORD & dwMinAbs, DWORD & dwMaxAbs);

protected:
	TAcceResult	m_vAcceResult;
	TAcceMaterials	m_vAcceMaterials;
};
#endif
//martysama0134's ceqyqttoaf71vasf9t71218
