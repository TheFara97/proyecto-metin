// vim: ts=8 sw=4
#ifndef __INC_MONEY_LOG
#define __INC_MONEY_LOG

#include <map>

class CMoneyLog : public singleton<CMoneyLog>
{
    public:
	CMoneyLog();
	virtual ~CMoneyLog();

	void Save();
#ifdef ENABLE_LONG_LONG
	void AddLog(BYTE utype, DWORD dwVnum, int64_t iGold);
#else
	void AddLog(BYTE bType, DWORD dwVnum, int iGold);
#endif

    private:
#ifdef ENABLE_LONG_LONG
	std::map<DWORD, int> m_MoneyLogContainer[MONEY_LOG_TYPE_MAX_NUM];
#else
	std::map<DWORD, int64_t> m_MoneyLogContainer[MONEY_LOG_TYPE_MAX_NUM];
#endif
};

#endif
//martysama0134's ceqyqttoaf71vasf9t71218
