#include "StdAfx.h"
#include "../eterBase/Utils.h"
#include "GrpText.h"
#include "../UserInterface/Locale_inc.h"

CGraphicText::CGraphicText(const char* c_szFileName) : CResource(c_szFileName)
{
}

CGraphicText::~CGraphicText()
{
}

bool CGraphicText::CreateDeviceObjects()
{
	return m_fontTexture.CreateDeviceObjects();
}

void CGraphicText::DestroyDeviceObjects()
{
	m_fontTexture.DestroyDeviceObjects();
}

CGraphicFontTexture* CGraphicText::GetFontTexturePointer()
{
	return &m_fontTexture;
}

CGraphicText::TType CGraphicText::Type()
{
	static TType s_type = StringToType("CGraphicText");
	return s_type;
}

bool CGraphicText::OnLoad(int /*iSize*/, const void* /*c_pvBuf*/)
{
#ifdef ENABLE_DYNAMIC_FONTS
	char strName[32];
#else
	static char strName[32];
#endif
	int size;
	bool bItalic = false;
	bool bBold = false;

	// format
	const char * p = strrchr(GetFileName(), ':');

	if (p)
	{
		strncpy(strName, GetFileName(), MIN(31, p - GetFileName()));
		++p;

#ifdef ENABLE_DYNAMIC_FONTS
		char num[8];
#else
		static char num[8];
#endif

		int i = 0;
		while (*p && isdigit(*p))
		{
			num[i++] = *(p++);
		}

		num[i] = '\0';
		if(*p == 'i')
			bItalic = true;
		if(*p == 'b')
			bBold = true;
		size = atoi(num);
	}
	else
	{
		p = strrchr(GetFileName(), '.');

		if (!p)
		{
			assert(!"CGraphicText::OnLoadFromFile there is no extension (ie: .fnt)");
			strName[0] = '\0';
		}
		else
			strncpy(strName, GetFileName(), MIN(31, p - GetFileName()));

		size = 12;
	}

	if (!m_fontTexture.Create(strName, size, bItalic, bBold))
		return false;

	return true;
}

void CGraphicText::OnClear()
{
	m_fontTexture.Destroy();
}

bool CGraphicText::OnIsEmpty() const
{
	return m_fontTexture.IsEmpty();
}

bool CGraphicText::OnIsType(TType type)
{
	if (CGraphicText::Type() == type)
		return true;

	return CResource::OnIsType(type);
}
//martysama0134's ceqyqttoaf71vasf9t71218
