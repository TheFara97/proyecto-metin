#include "StdAfx.h"

#ifdef ENABLE_WIKI
#include "PythonWiki.h"
#include "PythonNetworkStream.h"
#include "PythonPlayer.h"
#include "../GameLib/ItemManager.h"
#include <map>
#include "../eterPack/EterPackManager.h"

bool printLocalDatainSysser = false;

CPythonWiki::~CPythonWiki(){Destroy();}
CPythonWiki::CPythonWiki(){Destroy();}

void CPythonWiki::Destroy()
{
	auto it = m_vecRefineTable.begin();
	while (it != m_vecRefineTable.end())
	{
		delete it->second;
		it = m_vecRefineTable.erase(it);
	}
	m_vecRefineTable.clear();
}


constexpr DWORD bossChests[][2] = {
	{50069,591},
	{50070,691},
	{88099,1901},
	{50073,2091},
	{50098,3191},
	{88092,3190},
	{88091,8213},
	{88090,8214},
	{88093,6783},
	{88093,6789},
	{88096,34601},
	{88095,34600},
	{88098,3391},
	{88097,3390},
};

constexpr DWORD dangeonChests[][2] = {
	{90058,693},
	{90059,1093},
	{90060,2598},
	{90061,202202},
	{90063,2092},
};

constexpr DWORD otherChests[][2] = {
	{50000,0},
	{90029,0},
	{80020,0},
	{11024,0},
};

constexpr DWORD eventChests[][2] = {
	{203010,0},
	{50011,0},
};

constexpr DWORD eventBoss[] = {
	1,
};

const std::map<BYTE, std::vector<DWORD>> m_CostumeData = {
	
	{0, //Weapon
		{
			49054,
			49055,
			49056,
			49057,
			49058,
			49059,
		}
	},

	{1, //Armor
		{
			49060,
			49061,
		}
	},
	{2, //Hair
		{
			49062,
			49063,
		}
	},

	{3, //Sash
		{
			86067,
		}
	},
	{4, //Shining
		{
			65000,
			65001,
			65002,
			65003,
			65004,
			65100,
			65101,
			65102,
			65103,
			65104,
		}
	},
	{5, //Mount
		{
			71124,
			71502,
			71505,
			71508,
			71511,
			71243,
			71261,
			71262,
			71247,
			71253,
			71252,
			71246,
			71255,
			71254,
			71248,
			71249,
			71236,
			71235,
			71234,
			71233,
			71230,
		}
	},
	{6, //Pet
		{
			53001,
			53004,
			53007,
			53010,
			53013,
		}
	},
	{7, //Levelable Pet
		{
			55701,
			55703,
			55704,
			55702,
			55707,
			55705,
			55706,
		}
	},
};

void CPythonWiki::ReadData(const char* localeFile)
{
	if (!LoadRefineTable("locale/common/refine_table.txt"))
		TraceError("CPythonApplication - CPythonWiki::LoadRefineTable(locale/common/refine_table.txt)");
	char szItemProto[256];
	snprintf(szItemProto, sizeof(szItemProto), "%s/item_proto", localeFile);
	if (!LoadItemTable(szItemProto))
		TraceError("CPythonWiki - LoadItemTable(%s) Error", szItemProto);
	ReadSpecialDropItemFile("locale/common/special_item_group.txt");
	ReadMobDropItemFile("locale/common/mob_drop_item.txt");

	LoadCubeInformations("locale/common/cube.txt");
}

bool CPythonWiki::ReadSpecialDropItemFile(const char* c_pszFileName)
{
	CTextFileLoader loader;
	if (!loader.Load(c_pszFileName))
		return false;

	int iVnum;
	std::vector<special_data> vecSpecialData;
	std::string stName;
	TTokenVector* pTok;

	for (DWORD i = 0; i < loader.GetChildNodeCount(); ++i)
	{
		loader.SetChildNode(i);
		loader.GetCurrentNodeName(&stName);
		if (!loader.GetTokenInteger("vnum", &iVnum))
		{
			TraceError("ReadSpecialDropItemFile:Syntax error %s : no vnum, node %s", c_pszFileName, stName.c_str());
			loader.SetParentNode();
			continue;
		}
		vecSpecialData.clear();
		for (int k = 1; k < 256; ++k)
		{
			char buf[4];
			snprintf(buf, sizeof(buf), "%d", k);
			if (loader.GetTokenVector(buf, &pTok))
			{
				vecSpecialData.push_back(special_data(std::stoi(pTok->at(0)), std::stoi(pTok->at(1))));
				continue;
			}
			break;
		}

		if (vecSpecialData.size())
		{
			if (printLocalDatainSysser)
			{
				TraceError("                ");
				TraceError("ChestIndex: %d", iVnum);
				for (DWORD j = 0; j < vecSpecialData.size(); ++j)
					TraceError("Chest Data - Index: %d  ItemVnum: %d ItemCount: %d", j, vecSpecialData[j].itemVnum, vecSpecialData[j].count);
			}
			m_vecSpecialDrop.emplace(iVnum, vecSpecialData);
		}
		loader.SetParentNode();
	}
	return true;
}

bool CPythonWiki::ReadMobDropItemFile(const char* c_pszFileName)
{
	CTextFileLoader loader;
	if (!loader.Load(c_pszFileName))
		return false;

	int iVnum;
	std::vector<special_data> vecSpecialData;
	std::string stName;
	TTokenVector* pTok;
	
	for (DWORD i = 0; i < loader.GetChildNodeCount(); ++i)
	{
		loader.SetChildNode(i);
		loader.GetCurrentNodeName(&stName);
		if (!loader.GetTokenInteger("mob", &iVnum))
		{
			TraceError("ReadMobDropItemFile:Syntax error %s : no vnum, node %s", c_pszFileName, stName.c_str());
			loader.SetParentNode();
			continue;
		}
		vecSpecialData.clear();
		for (int k = 1; k < 256; ++k)
		{
			char buf[4];
			snprintf(buf, sizeof(buf), "%d", k);
			if (loader.GetTokenVector(buf, &pTok))
			{
				vecSpecialData.push_back(special_data(std::stoi(pTok->at(0)), std::stoi(pTok->at(1))));
				continue;
			}
			break;
		}
		if (vecSpecialData.size())
		{
			if (printLocalDatainSysser)
			{
				TraceError("                ");
				TraceError("MobIndex: %d", iVnum);
				for (DWORD j = 0; j < vecSpecialData.size(); ++j)
					TraceError("Mob Data - Index: %d  ItemVnum: %d ItemCount: %d", j, vecSpecialData[j].itemVnum, vecSpecialData[j].count);
			}
			m_vecMobDrop.emplace(iVnum, vecSpecialData);
		}
		loader.SetParentNode();
	}
	return true;
}

bool CompareLevel(character_data i, character_data j)
{
	return (i.level > j.level);
}

bool CompareLevelLow(character_data i, character_data j)
{
	return (i.level < j.level);
}

void CPythonWiki::ClearAllVectors()
{
	std::vector<std::vector<character_data>*> allVectors = {
		&m_vecBossCategory[0],
		&m_vecBossCategory[1],
		&m_vecBossCategory[2],
		&m_vecMonsterCategory[0],
		&m_vecMonsterCategory[1],
		&m_vecMonsterCategory[2],
		&m_vecStoneCategory[0],
		&m_vecStoneCategory[1],
		&m_vecStoneCategory[2],
		&m_vecStoneCategory[3],
		&m_vecWeapon[0],
		&m_vecWeapon[1],
		&m_vecWeapon[2],
		&m_vecWeapon[3],
		&m_vecArmor[0],
		&m_vecArmor[1],
		&m_vecArmor[2],
		&m_vecArmor[3],
		&m_vecHelmets[0],
		&m_vecHelmets[1],
		&m_vecHelmets[2],
		&m_vecHelmets[3],
		&m_vecShields[0],
		&m_vecShields[1],
		&m_vecShields[2],
		&m_vecShields[3],
		&m_vecEarrings[0],
		&m_vecEarrings[1],
		&m_vecEarrings[2],
		&m_vecEarrings[3],
		&m_vecBracelet[0],
		&m_vecBracelet[1],
		&m_vecBracelet[2],
		&m_vecBracelet[3],
		&m_vecNecklace[0],
		&m_vecNecklace[1],
		&m_vecNecklace[2],
		&m_vecNecklace[3],
		&m_vecShoes[0],
		&m_vecShoes[1],
		&m_vecShoes[2],
		&m_vecShoes[3],
		&m_vecBelt[0],
		&m_vecBelt[1],
		&m_vecBelt[2],
		&m_vecBelt[3],
		&m_vecTalisman[0],
		& m_vecRune[0],
		& m_vecPetEquipment[0],
		& m_vecMountpment[0]
	};

	for (auto vec : allVectors) {
		vec->clear();
	}
}


bool CPythonWiki::LoadItemTable(const char* c_szFileName)
{
	CMappedFile file;
	LPCVOID pvData;
	if (!CEterPackManager::Instance().Get(file, c_szFileName, &pvData))
		return false;
	DWORD dwFourCC, dwElements, dwDataSize, dwVersion = 0, dwStride = 0;
	DWORD s_adwItemProtoKey[4] =
	{
		173217,
		72619434,
		408587239,
		27973291
	};
	file.Read(&dwFourCC, sizeof(DWORD));
	if (dwFourCC == MAKEFOURCC('M', 'I', 'P', 'X'))
	{
		file.Read(&dwVersion, sizeof(DWORD));
		file.Read(&dwStride, sizeof(DWORD));
		if (dwVersion != 1)
		{
			TraceError("CPythonItem::LoadItemTable: invalid item_proto[%s] VERSION[%d]", c_szFileName, dwVersion);
			return false;
		}
		if (dwStride != sizeof(CItemData::TItemTable))
		{
			TraceError("CPythonItem::LoadItemTable: invalid item_proto[%s] STRIDE[%d] != sizeof(SItemTable)",c_szFileName, dwStride, sizeof(CItemData::TItemTable));
			return false;
		}
	}
	else if (dwFourCC != MAKEFOURCC('M', 'I', 'P', 'T'))
	{
		TraceError("CPythonItem::LoadItemTable: invalid item proto type %s", c_szFileName);
		return false;
	}
	file.Read(&dwElements, sizeof(DWORD));
	file.Read(&dwDataSize, sizeof(DWORD));
	BYTE* pbData = new BYTE[dwDataSize];
	file.Read(pbData, dwDataSize);
	CLZObject zObj;
	if (!CLZO::Instance().Decompress(zObj, pbData, s_adwItemProtoKey))
	{
		delete[] pbData;
		return false;
	}
	CItemData::TItemTable* table = (CItemData::TItemTable*)zObj.GetBuffer();
	ClearItemsVec(); // FixMe2023
	for (DWORD i = 0; i < dwElements; ++i, ++table)
		CPythonWiki::Instance().LoadItem(table);
	delete[] pbData;

	for (DWORD j = 0; j < 4; j++)
	{
		std::stable_sort(m_vecWeapon[j].begin(), m_vecWeapon[j].end(), CompareLevel);
		std::stable_sort(m_vecArmor[j].begin(), m_vecArmor[j].end(), CompareLevel);
		std::stable_sort(m_vecHelmets[j].begin(), m_vecHelmets[j].end(), CompareLevel);
		std::stable_sort(m_vecShields[j].begin(), m_vecShields[j].end(), CompareLevel);
		std::stable_sort(m_vecEarrings[j].begin(), m_vecEarrings[j].end(), CompareLevel);
		std::stable_sort(m_vecBracelet[j].begin(), m_vecBracelet[j].end(), CompareLevel);
		std::stable_sort(m_vecNecklace[j].begin(), m_vecNecklace[j].end(), CompareLevel);
		std::stable_sort(m_vecShoes[j].begin(), m_vecShoes[j].end(), CompareLevel);
		std::stable_sort(m_vecBelt[j].begin(), m_vecBelt[j].end(), CompareLevel);
		std::stable_sort(m_vecTalisman[j].begin(), m_vecTalisman[j].end(), CompareLevel);
		std::stable_sort(m_vecRune[j].begin(), m_vecRune[j].end(), CompareLevel);
		std::stable_sort(m_vecPetEquipment[j].begin(), m_vecPetEquipment[j].end(), CompareLevel);
		std::stable_sort(m_vecMountpment[j].begin(), m_vecMountpment[j].end(), CompareLevel);
	}

	return true;
}

bool CPythonWiki::BlackList(DWORD vnum, DWORD type, DWORD subtype)
{
	// Talisman blacklist.
	//if (type == CItemData::ITEM_TYPE_ARMOR && subtype == CItemData::ARMOR_PENDANT)
	//	return false;

	switch (vnum)
	{
		// warrior & weapon blacklist. begin
		case 529:
		case 269:
		case 3189:
		case 579:
		case 569:
		case 359:
		case 3249:
		case 26299:
		case 26219:
		case 24099:
		case 24199:
		case 24299:
		case 24399:
		case 26099:
		case 26399:
		case 26499:
		case 26599:
		case 26699:
		case 32090:
		case 32091:
		case 32092:
		case 32093:
		case 32094:
		case 32095:
		case 32096:
		case 32097:
		case 32098:
		case 32099:
		case 32199:
		case 32299:
		case 32399:
		case 32499:
		case 32599:
		case 24019:
		case 24119:
		case 24219:
		case 24319:
		case 26019:
		case 26319:
		case 26419:
		case 26519:
		case 26619:
		case 26759:
		case 26709:
		case 26719:
		case 26769:
		case 26779:
		case 26789:
		case 26739:
		case 26799:
		case 26749:
		case 24459:
		case 24449:
		case 24479:
		case 24469:
		case 24439:
		case 24429:
		case 24419:
		case 24409:
		case 32739:
		case 32669:
		case 32789:
		case 32719:
		case 32769:
		case 32699:
		case 32749:
		case 32679:
		case 32759:
		case 32689:
		case 32709:
		case 32799:
		case 32729:
		case 32779:
		case 32019:
		case 32119:
		case 32219:
		case 32319:
		case 32419:
		case 32519:
		case 32659:
		case 1909:
		case 1949:
		case 709:
		case 749:
		case 809:
		case 849:
		case 909:
		case 949:
		case 3209:
		case 3200:
		case 229:
		case 239:
		case 3179:
		case 88109:
		case 88409:
		case 88809:
		case 90209:
		case 11209:
		case 11219:
		case 11229:
		case 11239:
		case 11249:
		case 11259:
		case 11269:
		case 11279:
		case 11289:
		case 11409:
		case 11419:
		case 11429:
		case 11439:
		case 11449:
		case 11459:
		case 11469:
		case 11479:
		case 11489:
		case 11609:
		case 11619:
		case 11629:
		case 11639:
		case 11649:
		case 11659:
		case 11669:
		case 11679:
		case 11689:
		case 11809:
		case 11819:
		case 11829:
		case 11839:
		case 11849:
		case 11859:
		case 11869:
		case 11879:
		case 11889:
		case 94009:
		case 97009:
		case 18109:
		case 18039:
		case 18119:
		case 102009:
		case 105009:
		case 108009:
		case 109009:
		case 115009:
		case 5119:
		case 7169:
		case 5009:
		case 7009:
		case 19:
		case 299:
		case 2009:
		case 3009:
		case 3219:
		case 1009:
		case 12209:
		case 12349:
		case 12489:
		case 12509:
		case 12629:
		case 13009:
		case 13029:
		case 13049:
		case 13199:
		case 13209:
		case 14009:
		case 15009:
		case 16009:
		case 16129:
		case 16149:
		case 17009:
		case 17109:
		// warrior & Armour blacklist. begin
		// warrior & Helmets blacklist. begi
		// Ninja & weapon blacklist. begin
		case 1209:
		case 1149:
		case 539:
		case 519:
		case 2229:
		case 1929:
		case 1939:
		case 729:
		case 739:
		case 829:
		case 929:
		case 939:
		case 1920:
		case 1930:
		case 930:
		case 920:
		case 720:
		case 730:
		case 530:
		case 4039:
		case 1159:
		case 839:
		// Ninja & Armour blacklist. begin
		// Ninja & Helmets blacklist. begi
		// Shaman & weapon blacklist. begin
		case 5189:
		case 509:
		case 5159:
		case 7329:
		case 1959:
		case 759:
		case 859:
		case 869:
		case 959:
		case 1950:
		case 950:
		case 750:
		case 7189:
		case 5139:
		case 5149:
		case 7179:
		case 1969:
		// Shaman & Armour blacklist. begin
		case 12009:
		// Ninja & Helmets blacklist. begi
		// Sura & weapon blacklist. begin
		case 559:
		case 219:
		case 1919:
		case 719:
		case 819:
		case 919:
		case 190:
		case 710:
		case 209:
		case 910:
		// Sura & Armour blacklist. begin
		// Sura & Helmets blacklist. begin
		// Shields blacklist. begin
		case 13189:
		case 17579:
		case 17589:
		case 17609:
		case 14589:
		case 14599:
		case 14619:
		case 16589:
		case 16599:
		case 16619:
		case 13309:
		case 13239:
		case 13249:
		// pendientes blacklist begin
		case 17309:
		// brazaletes blacklist begin
		case 14309:
		//collares blacklist begin
		case 16309:
		// Belts blacklist. begin
		case 18129:
		case 18139:
		case 18149:
		case 18159:
		// Shoes blacklist. begin
		case 15469:
		case 15249:
			return false;
	}


	return true;
}

BYTE CPythonWiki::GetRefineLevel(DWORD vnum, DWORD type, DWORD subtype)
{
	if (type == CItemData::ITEM_TYPE_ARMOR &&
		(subtype == CItemData::ARMOR_PENDANT_FIRE ||
			subtype == CItemData::ARMOR_PENDANT_ICE ||
			subtype == CItemData::ARMOR_PENDANT_THUNDER ||
			subtype == CItemData::ARMOR_PENDANT_WIND ||
			subtype == CItemData::ARMOR_PENDANT_EARTH ||
			subtype == CItemData::ARMOR_PENDANT_DARK))
		return 25;

	// Ymir new weapon and armor set!

	constexpr DWORD darkVnums[] = {
		320,
		340,
		1190,
		2210,
		3230,
		5170,
		7310,
		21210,
		21230,
		21250,
		21310,
		21270,
		360,
		3260,
		2230,
		1220,
		5200,
		7330,
		21310,
		21330,
		21370,
		21350
	};
	for (UINT i = 0; i < _countof(darkVnums); i++)
	{
		if (vnum >= darkVnums[i] && vnum <= darkVnums[i] + 15)
			return 15;
	}
	return 9;
}

refineTable* CPythonWiki::GetRefineItem(DWORD index)
{
	const auto it = m_vecRefineTable.find(index);
	if (it != m_vecRefineTable.end())
	{
		return it->second;
	}
	return NULL;
}

bool CPythonWiki::LoadRefineTable(const char* c_szFileName)
{
	CMappedFile File;
	LPCVOID pData;
	if (!CEterPackManager::Instance().Get(File, c_szFileName, &pData))
		return false;

	CMemoryTextFileLoader textFileLoader;
	textFileLoader.Bind(File.Size(), pData);

	CTokenVector TokenVector;
	for (DWORD i = 0; i < textFileLoader.GetLineCount(); ++i)
	{
		if (textFileLoader.GetLineString(i)[0] == '#')
			continue;
		if (!textFileLoader.SplitLine(i, &TokenVector, "\t"))
			continue;

		// Updated to handle 27 fields: id(1) + vnum/count pairs(20) + costs(3) + src_vnum(1) + result_vnum(1) + prob(1)
		if (TokenVector.size() != 27)
		{
			TraceError("CPythonWiki::LoadRefineTable(%s) - StrangeLine in %d, expected 27 fields, got %d\n",
				c_szFileName, i, TokenVector.size());
			continue;
		}

		refineTable* p = new refineTable;
		memset(p, 0, sizeof(refineTable));

		p->id = (DWORD)atoi(TokenVector[0].c_str());
		p->refine_count = 0;

		int row = 1;
		for (DWORD j = 0; j < MAX_REFINE_ITEM; ++j)
		{
			p->item_vnums[j] = (DWORD)atoi(TokenVector[row].c_str());

			if (p->item_vnums[j] != 0) {
				p->refine_count = j + 1;
			}

			row += 1;
			p->item_count[j] = (DWORD)atoi(TokenVector[row].c_str());
			row += 1;
		}

		// row is now at index 21
		p->cost = (DWORD)atoi(TokenVector[21].c_str());   // cost
		p->cost2 = (DWORD)atoi(TokenVector[22].c_str());  // cost2  
		// Skip src_vnum (TokenVector[24]) and result_vnum (TokenVector[25]) if not needed
		p->prob = (DWORD)atoi(TokenVector[26].c_str());   // prob

		m_vecRefineTable.insert(std::make_pair(p->id, p));
	}
	return true;
}



bool CPythonWiki::LoadCubeInformations(const char* c_szFileName)
{
	m_vecCube.clear();

	CMappedFile File;
	LPCVOID pData;
	if (!CEterPackManager::Instance().Get(File, c_szFileName, &pData))
		return false;

	cubeTable cubeT{};
	cubeT.resetAllData();

	CMemoryTextFileLoader textFileLoader;
	textFileLoader.Bind(File.Size(), pData);
	CTokenVector TokenVector;

	int index{ 0 };

	for (DWORD i{}; i < textFileLoader.GetLineCount(); ++i)
	{
		if (!textFileLoader.SplitLine(i, &TokenVector, "\t"))
			continue;

		if (strcmp(TokenVector[0].c_str(), "//") == 0)
			continue;

		else if (strcmp(TokenVector[0].c_str(), "#") == 0)
			continue;

		else if (strcmp(TokenVector[0].c_str(), "section") == 0) // start
		{
			cubeT.resetAllData();
			continue;
		}

		else if (strcmp(TokenVector[0].c_str(), "end") == 0) // end
		{
			m_vecCube.push_back(cubeT);
			cubeT.upgradeVec.clear();
			continue;
		}

		else if (strcmp(TokenVector[0].c_str(), "npc") == 0) // end
		{
			if (TokenVector.size() < 2)
				continue;

			cubeT.npcVnum = std::stoi(TokenVector[1].c_str());
			continue;
		}

		else if (strcmp(TokenVector[0].c_str(), "item") == 0) // end
		{
			if (TokenVector.size() < 3)
				continue;

			if (cubeT.upgradeVec.size() == 10)
				continue;

			DWORD itemVnum{ static_cast<DWORD>(std::stoi(TokenVector[1].c_str())) };
			DWORD itemCount{ static_cast<DWORD>(std::stoi(TokenVector[2].c_str())) };

			cubeT.upgradeVec.push_back({ itemVnum, itemCount });
			continue;

		}
		else if (strcmp(TokenVector[0].c_str(), "reward") == 0) // end
		{
			if (TokenVector.size() < 3)
				continue;

			cubeT.vnum_reward = std::stoi(TokenVector[1].c_str());
			cubeT.count_reward = std::stoi(TokenVector[2].c_str());
			continue;
		}

		else if (strcmp(TokenVector[0].c_str(), "gold") == 0) // end
		{
			if (TokenVector.size() < 2)
				continue;

			cubeT.itemCost = std::stoll(TokenVector[1].c_str());
			continue;
		}

		else if (strcmp(TokenVector[0].c_str(), "percent") == 0) // end
		{
			if (TokenVector.size() < 2)
				continue;

			cubeT.upgradeChange = std::stoi(TokenVector[1].c_str());
			continue;
		}

		else if (strcmp(TokenVector[0].c_str(), "gem") == 0) // end
		{
			if (TokenVector.size() < 2)
				continue;

			cubeT.gemCost = std::stoi(TokenVector[1].c_str());
			continue;
		}

		else if (strcmp(TokenVector[0].c_str(), "won") == 0) // end
		{
			if (TokenVector.size() < 2)
				continue;

			cubeT.wonCost = std::stoi(TokenVector[1].c_str());
			continue;
		}
	}

	return true;
}

bool isFirstItem(DWORD itemVnum, const char* szname)
{
	std::string str = szname;
	//str += std::to_string(szname);
	if (str.find("+0") != std::string::npos)
		return true;
	return false;
}

bool isLastItem(DWORD itemVnum, const char* szname, BYTE itemType, BYTE itemSubtype)
{
	std::string str = szname;
	//str += std::to_string(itemVnum);

	DWORD maxRefine = CPythonWiki::Instance().GetRefineLevel(itemVnum, itemType, itemSubtype);
	std::string strrefine = "+";
	strrefine += std::to_string(maxRefine);
	//TraceError(" %s %s", str.c_str(), strrefine.c_str());
	if (str.find(strrefine.c_str()) != std::string::npos)
		return true;
	return false;
}

void CPythonWiki::ListReverse()
{
	for (DWORD j = 0; j < 3; j++)
	{
		std::stable_sort(m_vecStoneCategory[j].begin(), m_vecStoneCategory[j].end(), CompareLevelLow);
		std::stable_sort(m_vecBossCategory[j].begin(), m_vecBossCategory[j].end(), CompareLevelLow);
		std::stable_sort(m_vecMonsterCategory[j].begin(), m_vecMonsterCategory[j].end(), CompareLevelLow);
	}
}

bool BlackListMonster(DWORD vnum)
{
	//metines
	if (vnum >= 8007 && vnum <= 8009)
		return false;
	if (vnum >= 8052 && vnum <= 8053)
		return false;
	if (vnum == 8207)
		return false;
	if (vnum == 8210)
		return false;
	if (vnum == 6798)
		return false;
	if (vnum == 8203)
		return false;
	if (vnum == 6801)
		return false;
	if (vnum == 6799)
		return false;
	if (vnum == 6799)
		return false;
	if (vnum == 6802)
		return false;
	if (vnum == 6803)
		return false;
	if (vnum == 201014)
		return false;
	if (vnum == 201018)
		return false;
	if (vnum == 201019)
		return false;
	if (vnum == 8213)
		return false;
	if (vnum == 8214)
		return false;
	if (vnum == 2771)
		return false;
	if (vnum == 6783)
		return false;
	if (vnum == 6789)
		return false;
	if (vnum == 6791)
		return false;
	if (vnum == 6791)
		return false;
	if (vnum == 34600)
		return false;
	if (vnum == 34601)
		return false;
	if (vnum == 2820)
		return false;
	if (vnum == 6801)
		return false;
	if (vnum == 6799)
		return false;
	if (vnum == 3391)
		return false;
	if (vnum == 3390)
		return false;
	if (vnum == 2760)
		return false;
	if (vnum == 6802)
		return false;
	if (vnum == 6803)
		return false;
	if (vnum == 6804)
		return false;
	if (vnum == 202202)
		return false;
	if (vnum == 202208)
		return false;
	if (vnum == 202214)
		return false;
	if (vnum == 202218)
		return false;
	if (vnum == 202223) 
		return false;
	if (vnum == 202230)
		return false;
	// if (vnum >= 8062 && vnum <= 8065)
		// return false;
	// if (vnum >= 8469
		// return false;
	switch (vnum)
	{
		//monsters
		case 691:
		case 693:
		case 1093:
		case 2598:
		case 3190:
		case 3191:
		
		return false;
	}
	return true;
}

void CPythonWiki::LoadMonster(CPythonNonPlayer::TMobTable* monster)
{
	if (BlackListMonster(monster->dwVnum))
		return;

	if (monster->bType == 0 || monster->bType == 2) // monster & stone
	{
		if (monster->bLevel >= 1 && monster->bLevel <= 99)
		{
			if (monster->bType == 2)
				m_vecStoneCategory[0].emplace_back(character_data(monster->dwVnum, monster->bLevel));
			else if (monster->bRank >= CPythonNonPlayer::MOB_RANK_BOSS)
				m_vecBossCategory[0].emplace_back(character_data(monster->dwVnum, monster->bLevel));
			else
				m_vecMonsterCategory[0].emplace_back(character_data(monster->dwVnum, monster->bLevel));
		}
		else if (monster->bLevel >= 100 && monster->bLevel <= 210)
		{
			if (monster->bType == 2)
				m_vecStoneCategory[1].emplace_back(character_data(monster->dwVnum, monster->bLevel));
			else if (monster->bRank >= CPythonNonPlayer::MOB_RANK_BOSS)
				m_vecBossCategory[1].emplace_back(character_data(monster->dwVnum, monster->bLevel));
			else
				m_vecMonsterCategory[1].emplace_back(character_data(monster->dwVnum, monster->bLevel));
		}
		else if (monster->bLevel >= 151)
		{
			if (monster->bType == 2)
				m_vecStoneCategory[2].emplace_back(character_data(monster->dwVnum, monster->bLevel));
			else if (monster->bRank >= CPythonNonPlayer::RACE_FLAG_LEGENDA)
				m_vecBossCategory[2].emplace_back(character_data(monster->dwVnum, monster->bLevel));
			else
				m_vecMonsterCategory[2].emplace_back(character_data(monster->dwVnum, monster->bLevel));
		}
	}
}
void CPythonWiki::ClearItemsVec()
{
	m_vecWeapon->clear();
	m_vecArmor->clear();
	m_vecHelmets->clear();
	m_vecShields->clear();
	m_vecEarrings->clear();
	m_vecBracelet->clear();
	m_vecNecklace->clear();
	m_vecShoes->clear();
	m_vecBelt->clear();
	m_vecTalisman->clear();
	m_vecRune->clear();
	m_vecPetEquipment->clear();
	m_vecMountpment->clear();
}

void CPythonWiki::LoadItem(CItemData::TItemTable* item)
{
	constexpr BYTE flagList[NRaceData::JOB_MAX_NUM] = { CItemData::ITEM_ANTIFLAG_WARRIOR , CItemData::ITEM_ANTIFLAG_ASSASSIN ,CItemData::ITEM_ANTIFLAG_SHAMAN ,CItemData::ITEM_ANTIFLAG_SURA };

	if (isLastItem(item->dwVnum, item->szLocaleName, item->bType, item->bSubType))
	{
		if (!BlackList(item->dwVnum, item->bType, item->bSubType))
			return;

		DWORD itemLevel = 0;

#ifdef ENABLE_YOHARA_SYSTEM
		for (DWORD j = 0; j < CItemData::ITEM_LIMIT_MAX_NUM; j++)
		{
			if (item->aLimits[j].bType == CItemData::LIMIT_CONQUEROR_LEVEL && item->aLimits[j].lValue > 0)
			{
				itemLevel = 120 + item->aLimits[j].lValue;
				break;
			}
		}

		if (itemLevel == 0)
		{
#endif
		for (DWORD j = 0; j < CItemData::ITEM_LIMIT_MAX_NUM; j++)
		{
			if (item->aLimits[j].bType == CItemData::LIMIT_LEVEL)
			{
				itemLevel = item->aLimits[j].lValue;
				break;
			}
		}
#ifdef ENABLE_YOHARA_SYSTEM
		}
#endif

		if (item->bType == CItemData::ITEM_TYPE_WEAPON)
		{
			for (BYTE j = 0; j < NRaceData::JOB_MAX_NUM; ++j)
			{
				if (!(item->dwAntiFlags & flagList[j]))
					m_vecWeapon[j].emplace_back(character_data(item->dwVnum, itemLevel));
			}
		}
		else if (item->bType == CItemData::ITEM_TYPE_ARMOR)
		{
			if (item->bSubType == CItemData::ARMOR_BODY)
			{
				for (BYTE j = 0; j < NRaceData::JOB_MAX_NUM; ++j)
				{
					if (!(item->dwAntiFlags & flagList[j]))
						m_vecArmor[j].emplace_back(character_data(item->dwVnum, itemLevel));
				}
			}
			else if (item->bSubType == CItemData::ARMOR_HEAD)
			{
				for (BYTE j = 0; j < NRaceData::JOB_MAX_NUM; ++j)
				{
					if (!(item->dwAntiFlags & flagList[j]))
						m_vecHelmets[j].emplace_back(character_data(item->dwVnum, itemLevel));
				}
			}
			else if (item->bSubType == CItemData::ARMOR_SHIELD)
			{
				for (BYTE j = 0; j < NRaceData::JOB_MAX_NUM; ++j)
				{
					if (!(item->dwAntiFlags & flagList[j]))
						m_vecShields[j].emplace_back(character_data(item->dwVnum, itemLevel));
				}
			}
			else if (item->bSubType == CItemData::ARMOR_WRIST)
			{
				for (BYTE j = 0; j < NRaceData::JOB_MAX_NUM; ++j)
				{
					if (!(item->dwAntiFlags & flagList[j]))
						m_vecBracelet[j].emplace_back(character_data(item->dwVnum, itemLevel));
				}
			}
			else if (item->bSubType == CItemData::ARMOR_FOOTS)
			{
				for (BYTE j = 0; j < NRaceData::JOB_MAX_NUM; ++j)
				{
					if (!(item->dwAntiFlags & flagList[j]))
						m_vecShoes[j].emplace_back(character_data(item->dwVnum, itemLevel));
				}
			}
			else if (item->bSubType == CItemData::ARMOR_NECK)
			{
				for (BYTE j = 0; j < NRaceData::JOB_MAX_NUM; ++j)
				{
					if (!(item->dwAntiFlags & flagList[j]))
						m_vecNecklace[j].emplace_back(character_data(item->dwVnum, itemLevel));
				}
			}
			else if (item->bSubType == CItemData::ARMOR_EAR)
			{
				for (BYTE j = 0; j < NRaceData::JOB_MAX_NUM; ++j)
				{
					if (!(item->dwAntiFlags & flagList[j]))
						m_vecEarrings[j].emplace_back(character_data(item->dwVnum, itemLevel));
				}
			}
			else if (item->bSubType == CItemData::ARMOR_PENDANT_FIRE ||
				item->bSubType == CItemData::ARMOR_PENDANT_ICE ||
				item->bSubType == CItemData::ARMOR_PENDANT_THUNDER ||
				item->bSubType == CItemData::ARMOR_PENDANT_WIND ||
				item->bSubType == CItemData::ARMOR_PENDANT_EARTH ||
				item->bSubType == CItemData::ARMOR_PENDANT_DARK)
			{
				for (BYTE j = 0; j < NRaceData::JOB_MAX_NUM; ++j)
				{
					if (!(item->dwAntiFlags & flagList[j]))
						m_vecTalisman[j].emplace_back(character_data(item->dwVnum, itemLevel));
				}
			}
			
		}
		else if (item->bType == CItemData::ITEM_TYPE_BELT)
		{
			for (BYTE j = 0; j < NRaceData::JOB_MAX_NUM; ++j)
			{
				if (!(item->dwAntiFlags & flagList[j]))
					m_vecBelt[j].emplace_back(character_data(item->dwVnum, itemLevel));
			}
		}
		else if (item->bType == CItemData::ITEM_TYPE_RUNE ||
			item->bType == CItemData::ITEM_TYPE_RUNE_RED ||
			item->bType == CItemData::ITEM_TYPE_RUNE_BLUE ||
			item->bType == CItemData::ITEM_TYPE_RUNE_GREEN ||
			item->bType == CItemData::ITEM_TYPE_RUNE_YELLOW ||
			item->bType == CItemData::ITEM_TYPE_RUNE_BLACK)
		{
			for (BYTE j = 0; j < NRaceData::JOB_MAX_NUM; ++j)
			{
				if (!(item->dwAntiFlags & flagList[j]))
					m_vecRune[j].emplace_back(character_data(item->dwVnum, itemLevel));
			}
		}
		else if (item->bType == CItemData::ITEM_NEW_PET_EQ)
		{
			for (BYTE j = 0; j < NRaceData::JOB_MAX_NUM; ++j)
			{
				if (!(item->dwAntiFlags & flagList[j]))
					m_vecPetEquipment[j].emplace_back(character_data(item->dwVnum, itemLevel));
			}
		}
		else if (item->dwVnum >= 202000 && item->dwVnum <= 202029)
		{
			for (BYTE j = 0; j < NRaceData::JOB_MAX_NUM; ++j)
			{
				if (!(item->dwAntiFlags & flagList[j]))
					m_vecMountpment[j].emplace_back(character_data(item->dwVnum, itemLevel));
			}
		}
	}
}

PyObject* wikiGetRefineItems(PyObject* poSelf, PyObject* poArgs)
{
	int iRefineIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iRefineIndex))
		return Py_BadArgument();
	const auto item = CPythonWiki::Instance().GetRefineItem(iRefineIndex);
	if (!item)
		return Py_BuildValue("i", 0);

	// Return 25 values to match the 26 arguments expected by InsertRefine()
	return Py_BuildValue("iiiiiiiiiiiiiiiiiiiiiiiii", // 25 i's
		item->id,
		item->item_vnums[0], item->item_count[0],  // items 0
		item->item_vnums[1], item->item_count[1],  // items 1
		item->item_vnums[2], item->item_count[2],  // items 2
		item->item_vnums[3], item->item_count[3],  // items 3
		item->item_vnums[4], item->item_count[4],  // items 4
		item->item_vnums[5], item->item_count[5],  // items 5
		item->item_vnums[6], item->item_count[6],  // items 6
		item->item_vnums[7], item->item_count[7],  // items 7
		item->item_vnums[8], item->item_count[8],  // items 8
		item->item_vnums[9], item->item_count[9],  // items 9
		item->cost,                                 // cost
		item->cost2,                               // cost2
		item->prob,                                // prob
		item->refine_count                         // refine_count
	);
}

PyObject* wikiIsBlackList(PyObject* poSelf, PyObject* poArgs)
{
	int iItemVnum;
	if (!PyTuple_GetInteger(poArgs, 0, &iItemVnum))
		return Py_BadArgument();
	int iType;
	if (!PyTuple_GetInteger(poArgs, 1, &iType))
		return Py_BadArgument();
	int iSubtype;
	if (!PyTuple_GetInteger(poArgs, 2, &iSubtype))
		return Py_BadArgument();
	return Py_BuildValue("i", CPythonWiki::Instance().BlackList(iItemVnum, iType, iSubtype));
}

PyObject* wikiGetCategorySize(PyObject* poSelf, PyObject* poArgs)
{
	int iType;
	if (!PyTuple_GetInteger(poArgs, 0, &iType))
		return Py_BadArgument();
	int iCategoryType;
	if (!PyTuple_GetInteger(poArgs, 1, &iCategoryType))
		return Py_BadArgument();
	if (iType >= 0 && iType <= 3)
	{
		if (iCategoryType == 0)
			return Py_BuildValue("i", CPythonWiki::Instance().m_vecWeapon[iType].size());
		else if (iCategoryType == 1)
			return Py_BuildValue("i", CPythonWiki::Instance().m_vecArmor[iType].size());
		else if (iCategoryType == 2)
			return Py_BuildValue("i", CPythonWiki::Instance().m_vecHelmets[iType].size());
		else if (iCategoryType == 3)
			return Py_BuildValue("i", CPythonWiki::Instance().m_vecShields[iType].size());
		else if (iCategoryType == 4)
			return Py_BuildValue("i", CPythonWiki::Instance().m_vecEarrings[iType].size());
		else if (iCategoryType == 5)
			return Py_BuildValue("i", CPythonWiki::Instance().m_vecBracelet[iType].size());
		else if (iCategoryType == 6)
			return Py_BuildValue("i", CPythonWiki::Instance().m_vecNecklace[iType].size());
		else if (iCategoryType == 7)
			return Py_BuildValue("i", CPythonWiki::Instance().m_vecShoes[iType].size());
		else if (iCategoryType == 8)
			return Py_BuildValue("i", CPythonWiki::Instance().m_vecBelt[iType].size());
		else if (iCategoryType == 9)
			return Py_BuildValue("i", CPythonWiki::Instance().m_vecTalisman[iType].size());
		else if (iCategoryType == 10)
			return Py_BuildValue("i", CPythonWiki::Instance().m_vecRune[iType].size());
		else if (iCategoryType == 11)
			return Py_BuildValue("i", CPythonWiki::Instance().m_vecPetEquipment[iType].size());
		else if (iCategoryType == 12)
			return Py_BuildValue("i", CPythonWiki::Instance().m_vecMountpment[iType].size());
	}
	return Py_BuildValue("i", 0);
}

PyObject* wikiGetCategoryData(PyObject* poSelf, PyObject* poArgs)
{
	int iType;
	if (!PyTuple_GetInteger(poArgs, 0, &iType))
		return Py_BadArgument();
	int iCategoryType;
	if (!PyTuple_GetInteger(poArgs, 1, &iCategoryType))
		return Py_BadArgument();
	int iIndex;
	if (!PyTuple_GetInteger(poArgs, 2, &iIndex))
		return Py_BadArgument();
	if (iType >= 0 && iType <= 3)
	{
		if (iCategoryType == 0)
			return Py_BuildValue("i", CPythonWiki::Instance().m_vecWeapon[iType][iIndex].itemVnum);
		else if (iCategoryType == 1)
			return Py_BuildValue("i", CPythonWiki::Instance().m_vecArmor[iType][iIndex].itemVnum);
		else if (iCategoryType == 2)
			return Py_BuildValue("i", CPythonWiki::Instance().m_vecHelmets[iType][iIndex].itemVnum);
		else if (iCategoryType == 3)
			return Py_BuildValue("i", CPythonWiki::Instance().m_vecShields[iType][iIndex].itemVnum);
		else if (iCategoryType == 4)
			return Py_BuildValue("i", CPythonWiki::Instance().m_vecEarrings[iType][iIndex].itemVnum);
		else if (iCategoryType == 5)
			return Py_BuildValue("i", CPythonWiki::Instance().m_vecBracelet[iType][iIndex].itemVnum);
		else if (iCategoryType == 6)
			return Py_BuildValue("i", CPythonWiki::Instance().m_vecNecklace[iType][iIndex].itemVnum);
		else if (iCategoryType == 7)
			return Py_BuildValue("i", CPythonWiki::Instance().m_vecShoes[iType][iIndex].itemVnum);
		else if (iCategoryType == 8)
			return Py_BuildValue("i", CPythonWiki::Instance().m_vecBelt[iType][iIndex].itemVnum);
		else if (iCategoryType == 9)
			return Py_BuildValue("i", CPythonWiki::Instance().m_vecTalisman[iType][iIndex].itemVnum);
		else if (iCategoryType == 10)
			return Py_BuildValue("i", CPythonWiki::Instance().m_vecRune[iType][iIndex].itemVnum);
		else if (iCategoryType == 11)
			return Py_BuildValue("i", CPythonWiki::Instance().m_vecPetEquipment[iType][iIndex].itemVnum);
		else if (iCategoryType == 12)
			return Py_BuildValue("i", CPythonWiki::Instance().m_vecMountpment[iType][iIndex].itemVnum);
	}
	return Py_BuildValue("i", 0);
}

PyObject* wikiReadData(PyObject* poSelf, PyObject* poArgs)
{
	char* locale;
	if (!PyTuple_GetString(poArgs, 0, &locale))
		return Py_BuildException();
	CPythonWiki::Instance().ReadData(locale);
	return Py_BuildNone();
}

PyObject* wikiGetRefineMaxLevel(PyObject* poSelf, PyObject* poArgs)
{
	int iItemVnum;
	if (!PyTuple_GetInteger(poArgs, 0, &iItemVnum))
		return Py_BuildException();
	CItemManager::Instance().SelectItemData(iItemVnum);
	CItemData* pItemData = CItemManager::Instance().GetSelectedItemDataPointer();
	if (!pItemData)
		return Py_BuildValue("i", CPythonWiki::Instance().GetRefineLevel(iItemVnum, 0, 0));
	return Py_BuildValue("i", CPythonWiki::Instance().GetRefineLevel(iItemVnum, pItemData->GetType(), pItemData->GetSubType()));
}

PyObject* wikiGetChestSize(PyObject* poSelf, PyObject* poArgs)
{
	int iType;
	if (!PyTuple_GetInteger(poArgs, 0, &iType))
		return Py_BuildException();
	if (iType == 0)
		return Py_BuildValue("i", _countof(bossChests));
	else if (iType == 1)
		return Py_BuildValue("i", _countof(dangeonChests));
	else if (iType == 2)
		return Py_BuildValue("i", _countof(otherChests));
	else if (iType == 3)
		return Py_BuildValue("i", _countof(eventChests));
	return Py_BuildValue("i", 0);
}

PyObject* wikiGetChestData(PyObject* poSelf, PyObject* poArgs)
{
	int iType;
	if (!PyTuple_GetInteger(poArgs, 0, &iType))
		return Py_BuildException();
	int iIndex;
	if (!PyTuple_GetInteger(poArgs, 1, &iIndex))
		return Py_BuildException();
	if (iType == 0)
		return Py_BuildValue("ii", bossChests[iIndex][0], bossChests[iIndex][1]);
	else if (iType == 1)
		return Py_BuildValue("ii", dangeonChests[iIndex][0], dangeonChests[iIndex][1]);
	else if (iType == 2)
		return Py_BuildValue("ii", otherChests[iIndex][0], otherChests[iIndex][1]);
	else if (iType == 3)
		return Py_BuildValue("ii", eventChests[iIndex][0], eventChests[iIndex][1]);
	return Py_BuildValue("ii", 0,0);
}

PyObject* wikiGetSpecialInfoSize(PyObject* poSelf, PyObject* poArgs)
{
	int iItemVnum;
	if (!PyTuple_GetInteger(poArgs, 0, &iItemVnum))
		return Py_BuildException();
	const auto it = CPythonWiki::Instance().m_vecSpecialDrop.find(iItemVnum);
	if (it != CPythonWiki::Instance().m_vecSpecialDrop.end())
		return Py_BuildValue("i", it->second.size());
	return Py_BuildValue("i", 0);
}

PyObject* wikiGetSpecialInfoData(PyObject* poSelf, PyObject* poArgs)
{
	int iItemVnum;
	if (!PyTuple_GetInteger(poArgs, 0, &iItemVnum))
		return Py_BuildException();
	int iIndex;
	if (!PyTuple_GetInteger(poArgs, 1, &iIndex))
		return Py_BuildException();
	const auto it = CPythonWiki::Instance().m_vecSpecialDrop.find(iItemVnum);
	if (it != CPythonWiki::Instance().m_vecSpecialDrop.end())
		return Py_BuildValue("ii", it->second[iIndex].itemVnum, it->second[iIndex].count);
	return Py_BuildValue("ii", 0, 0);
}

PyObject* wikiGetMobInfoSize(PyObject* poSelf, PyObject* poArgs)
{
	int iItemVnum;
	if (!PyTuple_GetInteger(poArgs, 0, &iItemVnum))
		return Py_BuildException();
	const auto it = CPythonWiki::Instance().m_vecMobDrop.find(iItemVnum);
	if (it != CPythonWiki::Instance().m_vecMobDrop.end())
		return Py_BuildValue("i", it->second.size());
	return Py_BuildValue("i", 0);
}

PyObject* wikiGetMobInfoData(PyObject* poSelf, PyObject* poArgs)
{
	int iItemVnum;
	if (!PyTuple_GetInteger(poArgs, 0, &iItemVnum))
		return Py_BuildException();
	int iIndex;
	if (!PyTuple_GetInteger(poArgs, 1, &iIndex))
		return Py_BuildException();
	const auto it = CPythonWiki::Instance().m_vecMobDrop.find(iItemVnum);
	if (it == CPythonWiki::Instance().m_vecMobDrop.end())
		return Py_BuildValue("ii",0,0);
	return Py_BuildValue("ii", it->second[iIndex].itemVnum, it->second[iIndex].count);
}

PyObject* wikiGetBossSize(PyObject* poSelf, PyObject* poArgs)
{
	int iType;
	if (!PyTuple_GetInteger(poArgs, 0, &iType))
		return Py_BadArgument();
	if (iType == 3) // event boss
		return Py_BuildValue("i", (sizeof(eventBoss) / sizeof(eventBoss[0])));
	return Py_BuildValue("i", CPythonWiki::Instance().m_vecBossCategory[iType].size());
}

PyObject* wikiGetBossData(PyObject* poSelf, PyObject* poArgs)
{
	int iType;
	if (!PyTuple_GetInteger(poArgs, 0, &iType))
		return Py_BadArgument();
	int iIndex;
	if (!PyTuple_GetInteger(poArgs, 1, &iIndex))
		return Py_BadArgument();
	if (iType == 3) // event boss
		return Py_BuildValue("i", eventBoss[iIndex]);
	return Py_BuildValue("i", CPythonWiki::Instance().m_vecBossCategory[iType][iIndex]);
}

PyObject* wikiCostumeSize(PyObject* poSelf, PyObject* poArgs)
{
	int iType;
	if (!PyTuple_GetInteger(poArgs, 0, &iType))
		return Py_BadArgument();
	const auto it = m_CostumeData.find(iType);
	if (it != m_CostumeData.end())
		return Py_BuildValue("i", it->second.size());
	return Py_BuildValue("i", 0);
}
PyObject* wikiCostumeData(PyObject* poSelf, PyObject* poArgs)
{
	int iType;
	if (!PyTuple_GetInteger(poArgs, 0, &iType))
		return Py_BadArgument();
	int iIndex;
	if (!PyTuple_GetInteger(poArgs, 1, &iIndex))
		return Py_BadArgument();
	const auto it = m_CostumeData.find(iType);
	if (it != m_CostumeData.end())
		return Py_BuildValue("i", it->second[iIndex]);
	return Py_BuildValue("i", 0);
}

PyObject* wikiGetMonsterSize(PyObject* poSelf, PyObject* poArgs)
{
	int iType;
	if (!PyTuple_GetInteger(poArgs, 0, &iType))
		return Py_BadArgument();
	return Py_BuildValue("i", CPythonWiki::Instance().m_vecMonsterCategory[iType].size());
}

PyObject* wikiGetMonsterData(PyObject* poSelf, PyObject* poArgs)
{
	int iType;
	if (!PyTuple_GetInteger(poArgs, 0, &iType))
		return Py_BadArgument();
	int iIndex;
	if (!PyTuple_GetInteger(poArgs, 1, &iIndex))
		return Py_BadArgument();
	return Py_BuildValue("i", CPythonWiki::Instance().m_vecMonsterCategory[iType][iIndex]);
}

PyObject* wikiGetStoneSize(PyObject* poSelf, PyObject* poArgs)
{
	int iType;
	if (!PyTuple_GetInteger(poArgs, 0, &iType))
		return Py_BadArgument();
	return Py_BuildValue("i", CPythonWiki::Instance().m_vecStoneCategory[iType].size());
}

PyObject* wikiGetStoneData(PyObject* poSelf, PyObject* poArgs)
{
	int iType;
	if (!PyTuple_GetInteger(poArgs, 0, &iType))
		return Py_BadArgument();
	int iIndex;
	if (!PyTuple_GetInteger(poArgs, 1, &iIndex))
		return Py_BadArgument();
	return Py_BuildValue("i", CPythonWiki::Instance().m_vecStoneCategory[iType][iIndex]);
}

PyObject* wikiGetItemDropFromChest(PyObject* poSelf, PyObject* poArgs)
{
	int szItemVnum = 0;
	if (!PyTuple_GetInteger(poArgs, 0, &szItemVnum))
		return Py_BadArgument();
	bool szIsRefineItem = false;
	if (!PyTuple_GetBoolean(poArgs, 1, &szIsRefineItem))
		return Py_BadArgument();

	PyObject* poList = PyList_New(0);

	const BYTE refineLevel = szIsRefineItem ? CPythonWiki::Instance().GetRefineLevel(szItemVnum, 0, 0) : 0;
	const auto m_vec_ItemRange = CPythonWiki::Instance().GetSpecialDrop();

	for (const auto& [chestVnum, chestData] : m_vec_ItemRange)
	{
		if (chestData.size() > 0)
		{
			if (szIsRefineItem)
			{
				for (const auto& dataItem : chestData)
				{
					if (szItemVnum >= dataItem.itemVnum && szItemVnum <= dataItem.itemVnum + refineLevel)
					{
						const auto obj = Py_BuildValue("i", chestVnum);
						PyList_Append(poList, obj);
#ifdef ENABLE_LEAK_FIX
						fix_leak.push_back(obj);
#endif
						break;
					}
				}
			}
			else
			{
				for (const auto& dataItem : chestData)
				{
					if (szItemVnum == dataItem.itemVnum)
					{
						const auto obj = Py_BuildValue("i", chestVnum);
						PyList_Append(poList, obj);
#ifdef ENABLE_LEAK_FIX
						fix_leak.push_back(obj);
#endif
						break;
					}

				}
			}

		}
	}
#ifdef ENABLE_LEAK_FIX
	fix_leak.push_back(poList);
#endif
	return Py_BuildValue("O", poList);
}

PyObject* wikiGetItemDropFromMonster(PyObject* poSelf, PyObject* poArgs)
{
	int szItemVnum = 0;
	if (!PyTuple_GetInteger(poArgs, 0, &szItemVnum))
		return Py_BadArgument();
	bool szIsRefineItem = false;
	if (!PyTuple_GetBoolean(poArgs, 1, &szIsRefineItem))
		return Py_BadArgument();

	PyObject* poList = PyList_New(0);
	const BYTE refineLevel = szIsRefineItem ? CPythonWiki::Instance().GetRefineLevel(szItemVnum, 0, 0) : 0;
	const auto m_vec_ItemRange = CPythonWiki::Instance().GetMobDrop();
	for (const auto& [mobVnum, mobData] : m_vec_ItemRange)
	{
		if (mobData.size() > 0)
		{
			if (szIsRefineItem)
			{
				for (const auto& dataItem : mobData)
				{
					if (szItemVnum >= dataItem.itemVnum && szItemVnum <= dataItem.itemVnum + refineLevel)
					{
						const auto obj = Py_BuildValue("i", mobVnum);
						PyList_Append(poList, obj);
#ifdef ENABLE_LEAK_FIX
						fix_leak.push_back(obj);
#endif

						break;
					}
				}
			}
			else
			{
				for (const auto& dataItem : mobData)
				{
					if (szItemVnum == dataItem.itemVnum)
					{
						const auto obj = Py_BuildValue("i", mobVnum);
						PyList_Append(poList, obj);
#ifdef ENABLE_LEAK_FIX
						fix_leak.push_back(obj);
#endif
						break;
					}

				}
			}
		}
	}
#ifdef ENABLE_LEAK_FIX
	fix_leak.push_back(poList);
#endif
	return Py_BuildValue("O", poList);
}

PyObject* wikiGetItemInfosCube(PyObject* poSelf, PyObject* poArgs)
{
	int szItemVnum{};
	if (!PyTuple_GetInteger(poArgs, 0, &szItemVnum))
		return Py_BadArgument();

	PyObject* poList{ PyList_New(0) };

	const auto& m_vec_ItemRange{ CPythonWiki::Instance().GetCubeInformations() };

	for (const auto& informations : m_vec_ItemRange)
	{
		sCube cubeData;
		cubeData.vnum_reward = informations.vnum_reward;
		cubeData.count_reward = informations.count_reward;
		cubeData.upgradeVec = informations.upgradeVec;
		cubeData.npcVnum = informations.npcVnum;

		PyObject* poCubeDict = PyDict_New();

		PyObject* poVnumReward = PyLong_FromLong(cubeData.vnum_reward);
		PyDict_SetItemString(poCubeDict, "vnum_reward", poVnumReward);
		Py_DECREF(poVnumReward);

		PyObject* poCountReward = PyLong_FromLong(cubeData.count_reward);
		PyDict_SetItemString(poCubeDict, "count_reward", poCountReward);
		Py_DECREF(poCountReward);

		PyObject* poUpgradeVec = PyList_New(cubeData.upgradeVec.size());

		for (size_t i = 0; i < cubeData.upgradeVec.size(); ++i)
		{
			const special_data& upgradeData = cubeData.upgradeVec[i];

			PyObject* poUpgradeData = PyDict_New();

			PyObject* poUpgradeVnum = PyLong_FromLong(upgradeData.itemVnum);
			PyDict_SetItemString(poUpgradeData, "upgradeVnum", poUpgradeVnum);
			Py_DECREF(poUpgradeVnum);

			PyObject* poUpgradeCount = PyLong_FromLong(upgradeData.count);
			PyDict_SetItemString(poUpgradeData, "upgradeCount", poUpgradeCount);
			Py_DECREF(poUpgradeCount);

			PyList_SetItem(poUpgradeVec, i, poUpgradeData);
		}
		PyDict_SetItemString(poCubeDict, "upgradeVec", poUpgradeVec);
		Py_DECREF(poUpgradeVec);

		PyObject* poNpcVnum = PyLong_FromLong(cubeData.npcVnum);
		PyDict_SetItemString(poCubeDict, "npcVnum", poNpcVnum);
		Py_DECREF(poNpcVnum);

		PyList_Append(poList, poCubeDict);
		Py_DECREF(poCubeDict);
	}

	return Py_BuildValue("O", poList);
}


PyObject* wikiGetCubeData(PyObject* poSelf, PyObject* poArgs)
{
	int iType{};
	if (!PyTuple_GetInteger(poArgs, 0, &iType))
		return Py_BuildException();

	int iIndex{};
	if (!PyTuple_GetInteger(poArgs, 1, &iIndex))
		return Py_BuildException();

	const auto& m_vecCube = CPythonWiki::Instance().m_vecCube;

	long long cubeCost{};
	int gemCost{};
	int wonCost{};
	unsigned char upgradeChange{};

	// Helper lambda: finds the Nth cube entry matching a reward vnum
	auto findCubeByIndex = [&](const DWORD targetVnum) -> bool
		{
			int occurrence = 0;
			for (const auto& cubeItem : m_vecCube)
			{
				if (cubeItem.vnum_reward == targetVnum)
				{
					if (occurrence == iIndex)
					{
						cubeCost = cubeItem.itemCost;
						upgradeChange = cubeItem.upgradeChange;
						gemCost = cubeItem.gemCost;
						wonCost = cubeItem.wonCost;
						return true;
					}
					++occurrence;
				}
			}
			return false;
		};

	const DWORD(*pTable)[2] = nullptr;
	size_t tableSize = 0;

	if (iType == CUBE_CAT) { pTable = cubeNpcVnums;   tableSize = _countof(cubeNpcVnums); }
	else if (iType == CUBE_CAT2) { pTable = cubeNpcVnums2;  tableSize = _countof(cubeNpcVnums2); }
	else if (iType == CUBE_CAT3) { pTable = cubeNpcVnums3;  tableSize = _countof(cubeNpcVnums3); }
	else if (iType == CUBE_CAT4) { pTable = cubeNpcVnums4;  tableSize = _countof(cubeNpcVnums4); }
	else if (iType == CUBE_CAT5) { pTable = cubeNpcVnums5;  tableSize = _countof(cubeNpcVnums5); }
	else if (iType == CUBE_CAT6) { pTable = cubeNpcVnums6;  tableSize = _countof(cubeNpcVnums6); }
	else if (iType == CUBE_CAT7) { pTable = cubeNpcVnums7;  tableSize = _countof(cubeNpcVnums7); }
	else if (iType == CUBE_CAT8) { pTable = cubeNpcVnums8;  tableSize = _countof(cubeNpcVnums8); }
	else if (iType == CUBE_CAT9) { pTable = cubeNpcVnums9;  tableSize = _countof(cubeNpcVnums9); }
	else if (iType == CUBE_CAT10) { pTable = cubeNpcVnums10; tableSize = _countof(cubeNpcVnums10); }
	else if (iType == CUBE_CAT11) { pTable = cubeNpcVnums11; tableSize = _countof(cubeNpcVnums11); }
	else if (iType == CUBE_CAT12) { pTable = cubeNpcVnums12; tableSize = _countof(cubeNpcVnums12); }

	if (!pTable || iIndex < 0 || (size_t)iIndex >= tableSize)
		return Py_BuildValue("iiLiii", 0, 0, 0LL, 0, 0, 0);

	const DWORD rewardVnum = pTable[iIndex][0];
	const DWORD npcVnum = pTable[iIndex][1];

	// Count how many times this same vnum appears before iIndex in the table
	// so we pick the correct occurrence in m_vecCube
	int targetOccurrence = 0;
	for (int k = 0; k < iIndex; ++k)
	{
		if (pTable[k][0] == rewardVnum)
			++targetOccurrence;
	}

	// Find the targetOccurrence-th match in m_vecCube
	int occurrence = 0;
	for (const auto& cubeItem : m_vecCube)
	{
		if (cubeItem.vnum_reward == rewardVnum)
		{
			if (occurrence == targetOccurrence)
			{
				cubeCost = cubeItem.itemCost;
				upgradeChange = cubeItem.upgradeChange;
				gemCost = cubeItem.gemCost;
				wonCost = cubeItem.wonCost;
				break;
			}
			++occurrence;
		}
	}

	return Py_BuildValue("iiLiii", rewardVnum, npcVnum, cubeCost, wonCost, gemCost, upgradeChange);
}

PyObject* wikiGetCubeSize(PyObject* poSelf, PyObject* poArgs)
{
	int iType{};
	if (!PyTuple_GetInteger(poArgs, 0, &iType))
		return Py_BuildException();

	if (iType == CUBE_CAT)
		return Py_BuildValue("i", _countof(cubeNpcVnums));
	else if (iType == CUBE_CAT2)
		return Py_BuildValue("i", _countof(cubeNpcVnums2));
	else if (iType == CUBE_CAT3)
		return Py_BuildValue("i", _countof(cubeNpcVnums3));
	else if (iType == CUBE_CAT4)
		return Py_BuildValue("i", _countof(cubeNpcVnums4));
	else if (iType == CUBE_CAT5)
		return Py_BuildValue("i", _countof(cubeNpcVnums5));
	else if (iType == CUBE_CAT6)
		return Py_BuildValue("i", _countof(cubeNpcVnums6));
	else if (iType == CUBE_CAT7)
		return Py_BuildValue("i", _countof(cubeNpcVnums7));
	else if (iType == CUBE_CAT8)
		return Py_BuildValue("i", _countof(cubeNpcVnums8));
	else if (iType == CUBE_CAT9)
		return Py_BuildValue("i", _countof(cubeNpcVnums9));
	else if (iType == CUBE_CAT10)
		return Py_BuildValue("i", _countof(cubeNpcVnums10));
	else if (iType == CUBE_CAT11)
		return Py_BuildValue("i", _countof(cubeNpcVnums11));
	else if (iType == CUBE_CAT12)
		return Py_BuildValue("i", _countof(cubeNpcVnums12));


	return Py_BuildValue("i", 0);
}

PyObject* wikiGetCubeInfoSize(PyObject* poSelf, PyObject* poArgs)
{
	int iItemVnum{};

	if (!PyTuple_GetInteger(poArgs, 0, &iItemVnum))
		return Py_BuildException();

	const auto& m_vecCube = CPythonWiki::Instance().m_vecCube;

	size_t vecSize = 0;

	for (const auto& cubeItem : m_vecCube)
	{
		if (cubeItem.vnum_reward == iItemVnum)
		{
			vecSize = cubeItem.upgradeVec.size();
			break;
		}
	}

	return Py_BuildValue("i", static_cast<int>(vecSize));
}


PyObject* wikiGetCubeInfoData(PyObject* poSelf, PyObject* poArgs)
{

	int iItemVnum{};
	if (!PyTuple_GetInteger(poArgs, 0, &iItemVnum))
		return Py_BuildException();

	int iIndex{};
	if (!PyTuple_GetInteger(poArgs, 1, &iIndex))
		return Py_BuildException();

	const auto& m_vecCube = CPythonWiki::Instance().m_vecCube;

	for (const auto& cubeItem : m_vecCube)
	{
		if (cubeItem.vnum_reward == iItemVnum && iIndex >= 0 && iIndex < cubeItem.upgradeVec.size())
		{
			return Py_BuildValue("ii", cubeItem.upgradeVec[iIndex].itemVnum, cubeItem.upgradeVec[iIndex].count);
		}
	}

	return Py_BuildValue("ii", 0, 0, 0);
}

void initWiki()
{
	static PyMethodDef s_methods[] =
	{
		{ "GetRefineItems",	wikiGetRefineItems,	METH_VARARGS },
		{ "IsBlackList",	wikiIsBlackList,	METH_VARARGS },

		{ "GetCategorySize",	wikiGetCategorySize,	METH_VARARGS },
		{ "GetCategoryData",	wikiGetCategoryData,	METH_VARARGS },

		{ "GetBossSize",	wikiGetBossSize,	METH_VARARGS },
		{ "GetBossData",	wikiGetBossData,	METH_VARARGS },

		{ "GetMonsterSize",	wikiGetMonsterSize,	METH_VARARGS },
		{ "GetMonsterData",	wikiGetMonsterData,	METH_VARARGS },

		{ "GetCostumeSize",	wikiCostumeSize,	METH_VARARGS },
		{ "GetCostumeData",	wikiCostumeData,	METH_VARARGS },

		{ "GetStoneSize",	wikiGetStoneSize,	METH_VARARGS },
		{ "GetStoneData",	wikiGetStoneData,	METH_VARARGS },

		{ "GetChestSize",	wikiGetChestSize,	METH_VARARGS },
		{ "GetChestData",	wikiGetChestData,	METH_VARARGS },

		{ "GetSpecialInfoSize",	wikiGetSpecialInfoSize,	METH_VARARGS },
		{ "GetSpecialInfoData",	wikiGetSpecialInfoData,	METH_VARARGS },

		{ "GetMobInfoSize",	wikiGetMobInfoSize,	METH_VARARGS },
		{ "GetMobInfoData",	wikiGetMobInfoData,	METH_VARARGS },

		{ "ReadData",	wikiReadData,	METH_VARARGS },

		{ "GetRefineMaxLevel",	wikiGetRefineMaxLevel,	METH_VARARGS },

		{ "GetItemDropFromChest",	wikiGetItemDropFromChest,	METH_VARARGS },
		{ "GetItemDropFromMonster",	wikiGetItemDropFromMonster,	METH_VARARGS },

		{ "GetItemInfosCube",	wikiGetItemInfosCube,	METH_VARARGS },
		{ "GetCubeData",	wikiGetCubeData,	METH_VARARGS },
		{ "GetCubeSize",	wikiGetCubeSize,	METH_VARARGS },
		{ "GetCubeInfoSize",	wikiGetCubeInfoSize,	METH_VARARGS },
		{ "GetCubeInfoData",	wikiGetCubeInfoData,	METH_VARARGS },

		{ NULL, NULL, NULL },
	};
	PyObject* poModule = Py_InitModule("wiki", s_methods);
}
#endif

