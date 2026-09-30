#pragma once
#include <string>
#include <unordered_map>
#include "../../common/singleton.h"

// Loads per-language item_names.txt files and provides name lookups.
// File format: vnum<TAB>name  (same as client locale/xx/item_names.txt)
class CLocaleItemManager : public singleton<CLocaleItemManager>
{
public:
	// Load item_names.txt for every language sub-directory found in localeDir.
	// Call once on server boot, after ITEM_MANAGER is initialised.
	void LoadAll(const char* localeDir);

	// Load a single language file. lang is e.g. "en", "cz".
	void LoadForLocale(const std::string& lang, const char* filePath);

	// Returns the item name for the given vnum in the requested language.
	// Returns an empty string if not found.
	const std::string& Find(DWORD dwVnum, const std::string& lang) const;

private:
	// lang -> (vnum -> localized name)
	std::unordered_map<std::string, std::unordered_map<DWORD, std::string>> m_map;

	static const std::string s_empty;
};
