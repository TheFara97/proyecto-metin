#include "stdafx.h"
#include "locale_item_manager.h"
#include <fstream>

const std::string CLocaleItemManager::s_empty;

// Languages to try automatically when LoadAll() is called.
static const char* s_knownLangs[] = { "en", "cz", "pl", "de", "fr", "tr", "it", "pt", "ro", "es", "hu", nullptr };

void CLocaleItemManager::LoadAll(const char* localeDir)
{
	for (int i = 0; s_knownLangs[i]; ++i)
	{
		char buf[256];
		snprintf(buf, sizeof(buf), "%s/%s/item_names.txt", localeDir, s_knownLangs[i]);
		LoadForLocale(s_knownLangs[i], buf);
	}
}

void CLocaleItemManager::LoadForLocale(const std::string& lang, const char* filePath)
{
	std::ifstream f(filePath);
	if (!f.is_open())
		return;

	auto& nameMap = m_map[lang];
	std::string line;
	while (std::getline(f, line))
	{
		if (line.empty())
			continue;
		// Strip trailing \r (CRLF files)
		if (line.back() == '\r')
			line.pop_back();

		const size_t tab = line.find('\t');
		if (tab == std::string::npos)
			continue;

		const DWORD vnum = static_cast<DWORD>(std::stoul(line.substr(0, tab)));
		nameMap[vnum] = line.substr(tab + 1);
	}

	sys_log(0, "CLocaleItemManager: loaded %zu names for locale '%s' from %s",
		nameMap.size(), lang.c_str(), filePath);
}

const std::string& CLocaleItemManager::Find(DWORD dwVnum, const std::string& lang) const
{
	const auto itLang = m_map.find(lang);
	if (itLang == m_map.end())
		return s_empty;

	const auto itItem = itLang->second.find(dwVnum);
	if (itItem == itLang->second.end())
		return s_empty;

	return itItem->second;
}
