#include "StdAfx.h"
#include "GameType.h"
#ifdef ENABLE_DYNAMIC_FONTS
	#include "PythonTextTail.h"
#endif

#include <boost/algorithm/hex.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/archive/iterators/binary_from_base64.hpp>
#include <boost/archive/iterators/base64_from_binary.hpp>
#include <boost/archive/iterators/transform_width.hpp>
#include <string>

std::string g_strResourcePath = "d:/ymir work/";
std::string g_strImagePath = "d:/ymir work/ui/";

std::string g_strGuildSymbolPathName = "mark/10/";

// DEFAULT_FONT
static std::string gs_strDefaultFontName = "±¼¸²Ã¼:12.fnt";
static std::string gs_strDefaultItalicFontName = "±¼¸²Ã¼:12i.fnt";
static CResource* gs_pkDefaultFont = NULL;
static CResource* gs_pkDefaultItalicFont = NULL;

static bool gs_isReloadDefaultFont = false;

void DefaultFont_Startup()
{
	gs_pkDefaultFont = NULL;
}

void DefaultFont_Cleanup()
{
	if (gs_pkDefaultFont)
		gs_pkDefaultFont->Release();
}

void DefaultFont_SetName(const char * c_szFontName
#ifdef ENABLE_DYNAMIC_FONTS
	, bool reload
#endif
						)
{
	gs_strDefaultFontName = c_szFontName;
	gs_strDefaultFontName += ".fnt";

	gs_strDefaultItalicFontName = c_szFontName;
	if (strchr(c_szFontName, ':'))
	{
		gs_strDefaultItalicFontName += "i";
	}
	gs_strDefaultItalicFontName += ".fnt";

	gs_isReloadDefaultFont = true;
#ifdef ENABLE_DYNAMIC_FONTS
	if (reload)
	{
		DefaultFont_GetResource();
		CPythonTextTail::Instance().Initialize();
	}
#endif
}

bool ReloadDefaultFonts()
{
	CResourceManager& rkResMgr = CResourceManager::Instance();

	gs_isReloadDefaultFont = false;

	CResource* pkNewFont = rkResMgr.GetResourcePointer(gs_strDefaultFontName.c_str());
	pkNewFont->AddReference();
	if (gs_pkDefaultFont)
		gs_pkDefaultFont->Release();
	gs_pkDefaultFont = pkNewFont;

	CResource* pkNewItalicFont = rkResMgr.GetResourcePointer(gs_strDefaultItalicFontName.c_str());
	pkNewItalicFont->AddReference();
	if (gs_pkDefaultItalicFont)
		gs_pkDefaultItalicFont->Release();
	gs_pkDefaultItalicFont = pkNewItalicFont;

	return true;
}

CResource* DefaultFont_GetResource()
{
	if (!gs_pkDefaultFont || gs_isReloadDefaultFont)
		ReloadDefaultFonts();
	return gs_pkDefaultFont;
}

CResource* DefaultItalicFont_GetResource()
{
	if (!gs_pkDefaultItalicFont || gs_isReloadDefaultFont)
		ReloadDefaultFonts();
	return gs_pkDefaultItalicFont;
}

// END_OF_DEFAULT_FONT

void SetGuildSymbolPath(const char * c_szPathName)
{
	g_strGuildSymbolPathName = "mark/";
	g_strGuildSymbolPathName += c_szPathName;
	g_strGuildSymbolPathName += "/";
}

const char * GetGuildSymbolFileName(DWORD dwGuildID)
{
	return _getf("%s%03d.jpg", g_strGuildSymbolPathName.c_str(), dwGuildID);
}

BYTE c_aSlotTypeToInvenType[SLOT_TYPE_MAX] =
{
	RESERVED_WINDOW,		// SLOT_TYPE_NONE
	INVENTORY,				// SLOT_TYPE_INVENTORY
	RESERVED_WINDOW,		// SLOT_TYPE_SKILL
	RESERVED_WINDOW,		// SLOT_TYPE_EMOTION
	RESERVED_WINDOW,		// SLOT_TYPE_SHOP
	RESERVED_WINDOW,		// SLOT_TYPE_EXCHANGE_OWNER
	RESERVED_WINDOW,		// SLOT_TYPE_EXCHANGE_TARGET
	RESERVED_WINDOW,		// SLOT_TYPE_QUICK_SLOT
	RESERVED_WINDOW,
	RESERVED_WINDOW,		// SLOT_TYPE_PRIVATE_SHOP
	RESERVED_WINDOW,
	DRAGON_SOUL_INVENTORY,	// SLOT_TYPE_DRAGON_SOUL_INVENTORY
#ifdef ENABLE_ASLAN_BUFF_NPC_SYSTEM
	BUFF_EQUIPMENT,			// SLOT_TYPE_BUFF_EQUIPMENT
#endif
#ifdef ENABLE_SWITCHBOT
	SWITCHBOT,
#endif
#ifdef WJ_SPLIT_INVENTORY_SYSTEM
	INVENTORY,
	INVENTORY,
	INVENTORY,
	INVENTORY,
	INVENTORY,
	INVENTORY,
#endif
};

BYTE SlotTypeToInvenType(BYTE bSlotType)
{
	if (bSlotType >= SLOT_TYPE_MAX)
		return RESERVED_WINDOW;
	else
		return c_aSlotTypeToInvenType[bSlotType];
}

namespace CryptoGraphy {
std::string EncodeBase64(const std::string& sRaw) {
    using namespace boost::archive::iterators;
    std::stringstream ss;
    std::string sEncoded;

    typedef base64_from_binary<
        transform_width<std::string::const_iterator, 6, 8>>
        base64_enc;

    std::copy(base64_enc(sRaw.begin()), base64_enc(sRaw.end()),
              std::ostream_iterator<char>(ss));
    sEncoded = ss.str();
    boost::replace_all(sEncoded, "\n", "");

    return sEncoded;
}

std::string DecodeBase64(const std::string& sRaw) {
    using namespace boost::archive::iterators;
    std::stringstream ss;
    std::string sDecoded;

    typedef transform_width<binary_from_base64<std::string::const_iterator>, 8,
                            6>
        base64_dec;

    std::copy(base64_dec(sRaw.begin()), base64_dec(sRaw.end()),
              std::ostream_iterator<char>(ss));
    sDecoded = ss.str();

    return sDecoded;
}
}  // namespace CryptoGraphy