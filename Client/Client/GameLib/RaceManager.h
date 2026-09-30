#pragma once

#include "RaceData.h"

class CRaceManager : public CSingleton<CRaceManager>
{
public:
	typedef std::map<DWORD, CRaceData*> TRaceDataMap;
	typedef TRaceDataMap::iterator TRaceDataIterator;

public:
	CRaceManager();
	virtual ~CRaceManager();

	void Create();
	void Destroy();

	void RegisterRaceName(DWORD dwRaceIndex, const char* c_szName);

#ifdef ENABLE_PERFORMANCE_IMPROVEMENTS_NEW
	void RegisterRaceSrcName(const std::string& c_szName, const std::string& c_szSrcName);
	void SetPathName(const std::string c_szPathName);
#else
	void RegisterRaceSrcName(const char* c_szName, const char* c_szSrcName);
	void SetPathName(const char* c_szPathName);
#endif

	const char* GetFullPathFileName(const char* c_szFileName);

	// Handling
	void CreateRace(DWORD dwRaceIndex);
	void SelectRace(DWORD dwRaceIndex);
		// Handling
	CRaceData* GetSelectedRaceDataPointer();
	// Handling

	BOOL GetRaceDataPointer(DWORD dwRaceIndex, CRaceData** ppRaceData);

protected:
	CRaceData* __LoadRaceData(DWORD dwRaceIndex);
	bool __LoadRaceMotionList(CRaceData& rkRaceData, const char* pathName, const char* motionListFileName);

	void __Initialize();
	void __DestroyRaceDataMap();

protected:
	TRaceDataMap					m_RaceDataMap;

	std::map<std::string, std::string> m_kMap_stRaceName_stSrcName;
	std::map<DWORD, std::string>	m_kMap_dwRaceKey_stRaceName;

private:
	std::string						m_strPathName;
	CRaceData* m_pSelectedRaceData;
};
//martysama0134's ceqyqttoaf71vasf9t71218
