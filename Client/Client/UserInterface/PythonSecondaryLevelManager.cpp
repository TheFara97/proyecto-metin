#include "StdAfx.h"

#ifdef ENABLE_SECONDARY_LEVEL
#include "PythonSecondaryLevelManager.h"
#include "Packet.h"
#include "PythonPlayer.h"
#include "InstanceBase.h"

CSecondaryLevelManager::CSecondaryLevelManager()
{
	Initialize();
}

CSecondaryLevelManager::~CSecondaryLevelManager() {}

void CSecondaryLevelManager::Initialize()
{
	ZeroMemory(&m_SecondaryLevelsVector, sizeof(m_SecondaryLevelsVector));
	
	std::vector<TPacketGCSecondaryLevelInfo> tmpSecondaryLevels =
	{
		// Schemat:
		// {
		//      X - POZIOM
		//      {   BONUS_1,    BONUS_2,    BONUS_3,    BONUS_4,    BONUS_5,    BONUS_6     },
		//      {   WARTOŒÆ_1,    WARTOŒÆ_2,    WARTOŒÆ_3,    WARTOŒÆ_4,    WARTOŒÆ_5,    WARTOŒÆ_6 },
		//      {   ITEM_1,     ITEM_2,     ... },
		//      {   ILOŒÆ_1,  ILOŒÆ_2,  ... },
		//      {   AP_1,       AP_2        },
		//      {   SZANSA_1,   SZANSA_2    },
		//  }
		{
			1,
			{   106,      108,     63,      0,      0   },  
			{   1,        1,      1,      0,      0   },  
			{   0   },  
			{   0   },  
			{   500, 1000   },
			{   50,     100     },
		},
		{
			2,
			{   106,      108,     63,      0,      0   },  
			{   2,       2,      3,      0,      0   },  
			{   0   },  
			{   0   },  
			{   1500, 3000  },
			{   50,     100     },
		},
		{
			3,
			{   106,      108,     63,      0,      0   },  
			{   3,       3,      5,      0,      0   },  
			{   0   },  
			{   0   },  
			{   2500, 5000  },
			{   50,     100     },
		},
		{
			4,
			{   106,      108,     63,      0,      0   },  
			{   4,       4,      7,      0,      0   },  
			{   0   },  
			{   0   },  
			{   5000, 10000 },
			{   50,     100     },
		},
		{
			5,
			{   106,      108,     63,      0,      0   },  
			{   5,       5,      9,      0,      0   },  
			{   0   },  
			{   0   },  
			{   7500, 15000 },
			{   50,     100     },
		},
		{
			6,
			{   106,      108,     63,      0,      0   },  
			{   6,       6,      11,      0,      0   },  
			{   0   },  
			{   0   },  
			{   12500, 25000    },
			{   50,     100     },
		},
		{
			7,
			{   106,      108,     63,      0,      0   },  
			{   7,       7,      14,      0,      0   },  
			{   0   },  
			{   0   },  
			{   15000, 30000    },
			{   50,     100     },
		},
		{
			8,
			{   106,      108,     63,      0,      0   },  
			{   8,       8,      17,      0,      0   },  
			{   0   },  
			{   0   },  
			{   18500, 37000    },
			{   50,     100     },
		},
		{
			9,
			{   106,      108,     63,      0,      0   },  
			{   9,       9,      20,      0,      0   },  
			{   0   },  
			{   0   },  
			{   22500, 45000    },
			{   50,     100     },
		},
		{
			10,
			{   106,      108,     63,      0,      0   },  
			{   10,       10,     23,      0,      0   },  
			{   0   },  
			{   0   },  
			{   27500, 55000    },
			{   50,     100     },
		},
		{
			11,
			{   106,      108,     63,     0,      0   },  
			{   11,       11,     26,      0,      0   },  
			{   0   },  
			{   0   },  
			{   37500, 75000    },
			{   50,     100     },
		},
		{
			12,
			{   106,      108,     63,     0,      0   },  
			{   12,       12,     29,      0,      0   },  
			{   0   },  
			{   0   },  
			{   42500, 85000    },
			{   50,     100     },
		},
		{
			13,
			{   106,      108,     63,     0,      0   },  
			{   13,       13,     32,      0,      0   },  
			{   0   },  
			{   0   },  
			{   50000, 100000   },
			{   50,     100     },
		},
		{
			14,
			{   106,      108,     63,     0,      0   },  
			{   14,      14,     35,      0,      0   },  
			{   0   },  
			{   0   },  
			{   60000, 120000   },
			{   50,     100     },
		},
		{
			15,
			{   106,      108,     63,     0,      0   },  
			{   15,      15,     38,     0,      0   },  
			{   0   },  
			{   0   },  
			{   70000, 140000   },
			{   50,     100     },
		},
		{
			16,
			{   106,      108,     63,     107,        0 },  
			{   16,      16,     41,     1,      0   },  
			{   0   },  
			{   0   },  
			{   90000, 180000   },
			{   50,     100     },
		},
		{
			17,
			{   106,      108,     63,     107,        0 },  
			{   17,      17,     44,     2,      0   },  
			{   0   },  
			{   0   },  
			{   105000, 210000  },
			{   50,     100     },
		},
		{
			18,
			{   106,      108,     63,     107,        0 },  
			{   18,      18,     47,     3,      0   },  
			{   0   },  
			{   0   },  
			{   120000, 240000  },
			{   50,     100     },
		},
		{
			19,
			{   106,      108,     63,     107,        0 },
			{   19,      19,     50,     4,      0   },
			{   0   },  
			{   0   },  
			{   140000, 280000  },
			{   50,     100     },
		},
		{
			20,
			{   106,      108,     63,     107,        0 },
			{   20,      20,     53,     5,      0   },
			{   0   },  
			{   0   },  
			{   165000, 330000  },
			{   50,     100     },
		},
		{
			21,
			{   106,      108,     63,     107,        0 },
			{   21,      21,     56,     6,      0   },
			{   0   },  
			{   0   },  
			{   200000, 400000  },
			{   50,     100     },
		},
		{
			22,
			{   106,      108,     63,     107,        0 },
			{   22,      22,     59,     7,      0   },
			{   0   },  
			{   0   },
			{   225000, 450000  },
			{   50,     100     },
		},
		{
			23,
			{   106,      108,     63,     107,        0 },
			{   23,      23,     62,     8,      0   },
			{   0   },  
			{   0   },  
			{   260000, 520000  },
			{   50,     100     },
		},
		{
			24,
			{   106,      108,     63,     107,        0 },
			{   24,      24,     65,     9,      0   },
			{   0   },  
			{   0   },  
			{   300000, 600000  },
			{   50,     100     },
		},
		{
			25,
			{   106,      108,     63,     107,        0 },
			{   25,      25,     70,     10,     0   },
			{   0   },  
			{   0   },  
			{   350000, 700000  },
			{   50,     100     },
		},
		{
			26,
			{   106,      108,     63,     130,        107 },
			{   26,      26,     75,     2,     11   },
			{   0   },  
			{   0   },  
			{   450000, 900000  },
			{   50,     100     },
		},
		{
			27,
			{   106,      108,     63,     130,        107 },
			{   27,      27,     80,     4,     12   },
			{   0   },  
			{   0   },  
			{   600000, 1200000  },
			{   50,     100     },
		},
		{
			28,
			{   106,      108,     63,     130,        107 },
			{   28,      28,     85,     6,     13   },
			{   0   },  
			{   0   },  
			{   800000, 1600000  },
			{   50,     100     },
		},
		{
			29,
			{   106,      108,     63,     130,        107 },
			{   29,      29,     90,     8,     14   },
			{   0   },  
			{   0   },  
			{   1050000, 2100000  },
			{   50,     100     },
		},
		{
			30,
			{   106,      108,     63,     130,        107 },
			{   30,      30,     100,     10,     15   },
			{   0   },  
			{   0   },  
			{   1500000, 3000000  },
			{   50,     100     },
		},
	};
	 
	m_SecondaryLevelsVector = tmpSecondaryLevels;
}

struct SCompareSecondaryLevel {
	BYTE	bLevel;

	SCompareSecondaryLevel(BYTE _bLevel) : bLevel(_bLevel) {}
	bool operator () (const TPacketGCSecondaryLevelInfo& a) const
	{
		return (bLevel == a.bLevel);
	}
};

void CSecondaryLevelManager::AppendSecondaryLevelInformation(const TPacketGCSecondaryLevelInfo* pSecondaryLevelInfo)
{
	m_SecondaryLevelsVector.push_back(*pSecondaryLevelInfo);
}

DWORD CSecondaryLevelManager::GetApplyType(int32_t num) const
{
	const BYTE playerLevel = CPythonPlayer::Instance().GetStatus(POINT_SECONDARY_LEVEL);
	std::vector<TPacketGCSecondaryLevelInfo>::const_iterator it = std::find_if(m_SecondaryLevelsVector.begin(), m_SecondaryLevelsVector.end(), SCompareSecondaryLevel(playerLevel));
	if (it == m_SecondaryLevelsVector.end())
		return 0;

	TPacketGCSecondaryLevelInfo pSecondaryLevel = *it;

	return pSecondaryLevel.dwApplyType[num];
}

DWORD CSecondaryLevelManager::GetNextApplyType(int32_t num) const
{
	const BYTE playerLevel = CPythonPlayer::Instance().GetStatus(POINT_SECONDARY_LEVEL) + 1;
	std::vector<TPacketGCSecondaryLevelInfo>::const_iterator it = std::find_if(m_SecondaryLevelsVector.begin(), m_SecondaryLevelsVector.end(), SCompareSecondaryLevel(playerLevel));
	if (it == m_SecondaryLevelsVector.end())
		return 0;

	TPacketGCSecondaryLevelInfo pSecondaryLevel = *it;

	return pSecondaryLevel.dwApplyType[num];
}

long CSecondaryLevelManager::GetApplyValue(int32_t num) const
{
	const BYTE playerLevel = CPythonPlayer::Instance().GetStatus(POINT_SECONDARY_LEVEL);
	std::vector<TPacketGCSecondaryLevelInfo>::const_iterator it = std::find_if(m_SecondaryLevelsVector.begin(), m_SecondaryLevelsVector.end(), SCompareSecondaryLevel(playerLevel));
	if (it == m_SecondaryLevelsVector.end())
		return 0;

	TPacketGCSecondaryLevelInfo pSecondaryLevel = *it;

	return pSecondaryLevel.lApplyValue[num];
}

long CSecondaryLevelManager::GetNextApplyValue(int32_t num) const
{
	const BYTE playerLevel = CPythonPlayer::Instance().GetStatus(POINT_SECONDARY_LEVEL) + 1;
	std::vector<TPacketGCSecondaryLevelInfo>::const_iterator it = std::find_if(m_SecondaryLevelsVector.begin(), m_SecondaryLevelsVector.end(), SCompareSecondaryLevel(playerLevel));
	if (it == m_SecondaryLevelsVector.end())
		return 0;

	TPacketGCSecondaryLevelInfo pSecondaryLevel = *it;

	return pSecondaryLevel.lApplyValue[num];
}

DWORD CSecondaryLevelManager::GetRequiredItemVnum(int32_t num) const
{
	const BYTE playerLevel = CPythonPlayer::Instance().GetStatus(POINT_SECONDARY_LEVEL) + 1;
	std::vector<TPacketGCSecondaryLevelInfo>::const_iterator it = std::find_if(m_SecondaryLevelsVector.begin(), m_SecondaryLevelsVector.end(), SCompareSecondaryLevel(playerLevel));
	if (it == m_SecondaryLevelsVector.end())
		return 0;

	TPacketGCSecondaryLevelInfo pSecondaryLevel = *it;

	return pSecondaryLevel.dwRequiredItemVnum[num];
}

long CSecondaryLevelManager::GetRequiredItemCount(int32_t num) const
{
	const BYTE playerLevel = CPythonPlayer::Instance().GetStatus(POINT_SECONDARY_LEVEL) + 1;
	std::vector<TPacketGCSecondaryLevelInfo>::const_iterator it = std::find_if(m_SecondaryLevelsVector.begin(), m_SecondaryLevelsVector.end(), SCompareSecondaryLevel(playerLevel));
	if (it == m_SecondaryLevelsVector.end())
		return 0;

	TPacketGCSecondaryLevelInfo pSecondaryLevel = *it;

	return pSecondaryLevel.lRequiredItemCount[num];
}

int64_t CSecondaryLevelManager::GetRequiredGold(int32_t option) const
{
	const BYTE playerLevel = CPythonPlayer::Instance().GetStatus(POINT_SECONDARY_LEVEL) + 1;
	std::vector<TPacketGCSecondaryLevelInfo>::const_iterator it = std::find_if(m_SecondaryLevelsVector.begin(), m_SecondaryLevelsVector.end(), SCompareSecondaryLevel(playerLevel));
	if (it == m_SecondaryLevelsVector.end())
		return 0;

	TPacketGCSecondaryLevelInfo pSecondaryLevel = *it;

	return pSecondaryLevel.llRequiredGold[option];
}

DWORD CSecondaryLevelManager::GetChance(int32_t option) const
{
	const BYTE playerLevel = CPythonPlayer::Instance().GetStatus(POINT_SECONDARY_LEVEL) + 1;
	std::vector<TPacketGCSecondaryLevelInfo>::const_iterator it = std::find_if(m_SecondaryLevelsVector.begin(), m_SecondaryLevelsVector.end(), SCompareSecondaryLevel(playerLevel));
	if (it == m_SecondaryLevelsVector.end())
		return 0;

	TPacketGCSecondaryLevelInfo pSecondaryLevel = *it;

	return pSecondaryLevel.dwChance[option];
}
#endif