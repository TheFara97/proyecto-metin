#pragma once

#ifdef ENABLE_WIKI
#include "../gamelib/ItemManager.h"
#include "../gamelib/ItemData.h"
#include "PythonNonPlayer.h"
enum {
	MAX_REFINE_ITEM = 10,
};

enum CubeCategoryType
{
	CUBE_CAT,
	CUBE_CAT2,
	CUBE_CAT3,
	CUBE_CAT4,
	CUBE_CAT5,
	CUBE_CAT6,
	CUBE_CAT7,
	CUBE_CAT8,
	CUBE_CAT9,
	CUBE_CAT10,
	CUBE_CAT11,
	CUBE_CAT12,
};

typedef struct r_table {
    DWORD id;
    DWORD item_vnums[MAX_REFINE_ITEM];
    DWORD item_count[MAX_REFINE_ITEM];
    DWORD cost;
    DWORD cost2;  // Add this
    DWORD prob;
    DWORD refine_count;
} refineTable;

typedef struct refine_map {
	DWORD	itemVnum;
	DWORD	level;
	refine_map(DWORD _vnum, DWORD _level) :itemVnum(_vnum), level(_level) {}
} character_data;

typedef struct special_map {
	DWORD	itemVnum;
	DWORD	count;
	special_map(DWORD _vnum, DWORD _count):itemVnum(_vnum),count(_count){}
} special_data;

typedef struct sCube
{
	int	vnum_reward;
	int	count_reward;

	std::vector<special_data> upgradeVec;

	int npcVnum;

	long long itemCost;

	int gemCost;
	int wonCost;

	unsigned char upgradeChange;

	void resetAllData()
	{
		memset(this, 0, sizeof(this));
	}

} cubeTable;

class CPythonWiki : public CSingleton <CPythonWiki>
{
public:

	CPythonWiki();
	virtual ~CPythonWiki();

	void	Destroy();


	bool	LoadItemTable(const char* c_szFileName);
	bool	LoadRefineTable(const char* c_szFileName);
	bool	LoadCubeInformations(const char* c_szFileName);

	void	LoadItem(CItemData::TItemTable* item);
	void	LoadMonster(CPythonNonPlayer::TMobTable* monster);

	void	ReadData(const char* localeFile);
	BYTE	GetRefineLevel(DWORD vnum, DWORD type, DWORD subtype);

	refineTable* GetRefineItem(DWORD index);
	bool	BlackList(DWORD vnum, DWORD type, DWORD subtype);

	bool	ReadSpecialDropItemFile(const char* c_pszFileName);
	bool	ReadMobDropItemFile(const char* c_pszFileName);

	void	ListReverse();
	void	ClearItemsVec();
	void	ClearAllVectors();

	std::unordered_map<DWORD, std::vector<special_data>> GetSpecialDrop() {return m_vecSpecialDrop;}
	std::unordered_map<DWORD, std::vector<special_data>> GetMobDrop() {return m_vecMobDrop;}
	std::vector<cubeTable> GetCubeInformations() { return m_vecCube; }

	std::vector<character_data> m_vecBossCategory[3];
	std::vector<character_data> m_vecMonsterCategory[3];
	std::vector<character_data> m_vecStoneCategory[3];

	std::vector<character_data> m_vecWeapon[4];
	std::vector<character_data> m_vecArmor[4];
	std::vector<character_data> m_vecHelmets[4];
	std::vector<character_data> m_vecShields[4];
	std::vector<character_data> m_vecEarrings[4];
	std::vector<character_data> m_vecBracelet[4];
	std::vector<character_data> m_vecNecklace[4];
	std::vector<character_data> m_vecShoes[4];
	std::vector<character_data> m_vecBelt[4];
	std::vector<character_data> m_vecTalisman[4];
	std::vector<character_data> m_vecRune[4];
	std::vector<character_data> m_vecPetEquipment[4];
	std::vector<character_data> m_vecMountpment[4];

	std::unordered_map<DWORD, std::vector<special_data>> m_vecSpecialDrop;
	std::unordered_map<DWORD, std::vector<special_data>> m_vecMobDrop;
	std::vector<cubeTable> m_vecCube;

protected:
	
	std::unordered_map<DWORD, refineTable*> m_vecRefineTable;
	// 4 character... [WARRIOR-ASSASSIN-SHAMAN-SURA]
	
	
};

constexpr DWORD cubeNpcVnums[][2]{
    {25045, 20016}, // BlackSmith
	{201000, 20016}, // BlackSmith
	{201001, 20016}, // BlackSmith
	{201002, 20016}, // BlackSmith
	{79010, 20016}, // BlackSmith
	{79011, 20016}, // BlackSmith
	{203011, 20016}, // BlackSmith
	{31077, 20016}, // BlackSmith
	{79013, 20016}, // BlackSmith
	{71052, 20016}, // BlackSmith
};
constexpr DWORD cubeNpcVnums2[][2]{
    {100700, 20001}, // Alchemist
	{51001, 20001}, // Alchemist
	{50256, 20001}, // Alchemist
	{50257, 20001}, // Alchemist
	{50258, 20001}, // Alchemist
	{50259, 20001}, // Alchemist
};
constexpr DWORD cubeNpcVnums3[][2]{
	{201241, 20011}, // Uriel
	{90029, 20011}, // Uriel
	{90037, 20011}, // Uriel
	{90038, 20011}, // Uriel
	{90039, 20011}, // Uriel
	{90040, 20011}, // Uriel
    {40017, 20011}, // Uriel
	{40018, 20011}, // Uriel
	{40019, 20011}, // Uriel
	{40020, 20011}, // Uriel
	{50835, 20011}, // Uriel
	{50841, 20011}, // Uriel
	{50837, 20011}, // Uriel
	{40024, 20011}, // Uriel
	{40025, 20011}, // Uriel
	{50842, 20011}, // Uriel
	{50838, 20011}, // Uriel
	{50843, 20011}, // Uriel
	{50839, 20011}, // Uriel
	{50844, 20011}, // Uriel
	{50840, 20011}, // Uriel
	{90050, 20011}, // Uriel
};
constexpr DWORD cubeNpcVnums4[][2]{
    {50495, 20413}, // Kawuco
	{80024, 20413}, // Kawuco
	{80025, 20413}, // Kawuco
	{80023, 20413}, // Kawuco
	{80022, 20413}, // Kawuco
	{50336, 20413}, // Kawuco
	{50339, 20413}, // Kawuco
	{50342, 20413}, // Kawuco
	{50345, 20413}, // Kawuco
	{50348, 20413}, // Kawuco
	{50351, 20413}, // Kawuco
	{50354, 20413}, // Kawuco
	{50357, 20413}, // Kawuco
	{50360, 20413}, // Kawuco
	{50363, 20413}, // Kawuco
	{50366, 20413}, // Kawuco
	{50337, 20413}, // Kawuco
	{50340, 20413}, // Kawuco
	{50343, 20413}, // Kawuco
	{50346, 20413}, // Kawuco
	{50349, 20413}, // Kawuco
	{50352, 20413}, // Kawuco
	{50355, 20413}, // Kawuco
	{50358, 20413}, // Kawuco
	{50361, 20413}, // Kawuco
	{50364, 20413}, // Kawuco
	{50367, 20413}, // Kawuco
};
constexpr DWORD cubeNpcVnums5[][2]{
    {80026, 20414}, // Andastre
	{80027, 20414}, // Andastre
	{80033, 20414}, // Andastre
	{80056, 20414}, // Andastre
	{80057, 20414}, // Andastre
	{80055, 20414}, // Andastre
	{203027, 20414}, // Andastre
	{203001, 20414}, // Andastre
	{203002, 20414}, // Andastre
	{203003, 20414}, // Andastre
	{203013, 20414}, // Andastre
	{203014, 20414}, // Andastre
	{202010, 20414}, // Andastre
	{202000, 20414}, // Andastre
	{202020, 20414}, // Andastre
};
constexpr DWORD cubeNpcVnums6[][2]{
    {85001, 60003}, // Theowahdan
	{85005, 60003}, // Theowahdan
	{85011, 60003}, // Theowahdan
	{85015, 60003}, // Theowahdan
	{90001, 60003}, // Theowahdan
};
constexpr DWORD cubeNpcVnums7[][2]{
    {28930, 20091}, // Seon-Pyeong
	{28931, 20091}, // Seon-Pyeong
	{28932, 20091}, // Seon-Pyeong
	{28933, 20091}, // Seon-Pyeong
	{28934, 20091}, // Seon-Pyeong
	{28935, 20091}, // Seon-Pyeong
	{28936, 20091}, // Seon-Pyeong
	{28937, 20091}, // Seon-Pyeong
	{28938, 20091}, // Seon-Pyeong
	{28939, 20091}, // Seon-Pyeong
	{28940, 20091}, // Seon-Pyeong
	{28941, 20091}, // Seon-Pyeong
	{28942, 20091}, // Seon-Pyeong
	{28943, 20091}, // Seon-Pyeong
	{203023, 20091}, // Seon-Pyeong
	{203024, 20091}, // Seon-Pyeong
};
constexpr DWORD cubeNpcVnums8[][2]{
    {50600, 20015}, // Deokbae
	{79015, 20015}, // Deokbae
	{79016, 20015}, // Deokbae
	{50621, 20015}, // Deokbae
	{50639, 20015}, // Deokbae
	{50640, 20015}, // Deokbae
	{50641, 20015}, // Deokbae
	{50642, 20015}, // Deokbae
	{50643, 20015}, // Deokbae
	{50644, 20015}, // Deokbae
	{50647, 20015}, // Deokbae
};

constexpr DWORD cubeNpcVnums9[][2]{
    {50902, 9009}, // Fisherman
	{80400, 9009}, // Fisherman
	{80401, 9009}, // Fisherman
	{80402, 9009}, // Fisherman
	{80403, 9009}, // Fisherman
	{80404, 9009}, // Fisherman
	{80405, 9009}, // Fisherman
	{80406, 9009}, // Fisherman
	{80407, 9009}, // Fisherman
	{80408, 9009}, // Fisherman
	{80418, 9009}, // Fisherman
	{80419, 9009}, // Fisherman
	{80420, 9009}, // Fisherman
	{80421, 9009}, // Fisherman
	{80409, 9009}, // Fisherman
	{80410, 9009}, // Fisherman
	{80411, 9009}, // Fisherman
	{80412, 9009}, // Fisherman
	{80413, 9009}, // Fisherman
	{80414, 9009}, // Fisherman
	{80415, 9009}, // Fisherman
	{80416, 9009}, // Fisherman
	{80417, 9009}, // Fisherman
	{91201, 9009}, // Fisherman
	{91202, 9009}, // Fisherman
	{91205, 9009}, // Fisherman
};

constexpr DWORD cubeNpcVnums10[][2]{
	{27400, 200300}, // Balikci
};

constexpr DWORD cubeNpcVnums11[][2]{
	{27400, 200300}, // Balikci
};

constexpr DWORD cubeNpcVnums12[][2]{
	{27400, 200300}, // Balikci
};

#endif