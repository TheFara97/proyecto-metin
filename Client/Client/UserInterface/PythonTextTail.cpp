//
//
#include "stdafx.h"
#include "InstanceBase.h"
#include "resource.h"
#include "PythonTextTail.h"
#include "PythonCharacterManager.h"
#include "PythonGuild.h"
#include "Locale.h"
#include "MarkManager.h"
#include "PythonPlayer.h"
#include "AbstractApplication.h"
#include "AbstractPlayer.h"
#include "PythonSystem.h"
#ifdef ENABLE_PREMIUM_PRIVATE_SHOP_OFFICIAL
#include "PythonPrivateShop.h"
#include "PythonSystem.h"
#endif
#ifdef ENABLE_SHOP_IN_CITIES
#include "PythonApplication.h"
#include "../EterPythonLib/PythonGraphic.h"
#endif

const D3DXCOLOR c_TextTail_Player_Color = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
const D3DXCOLOR c_TextTail_Monster_Color = D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f);
const D3DXCOLOR c_TextTail_Item_Color = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
#ifdef ENABLE_EXTENDED_ITEMNAME_ON_GROUND
const D3DXCOLOR c_TextTail_SpecialItem_Color = D3DXCOLOR(1.0f, 0.67f, 0.0f, 1.0f); //Golden
#endif
const D3DXCOLOR c_TextTail_Chat_Color = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
const D3DXCOLOR c_TextTail_Info_Color = D3DXCOLOR(1.0f, 0.785f, 0.785f, 1.0f);
const D3DXCOLOR c_TextTail_Guild_Name_Color = 0xFFEFD3FF;
const float c_TextTail_Name_Position = -10.0f;
const float c_fxMarkPosition = 1.5f;
#ifdef ENABLE_TITLE_ACHIEVEMENT_SYSTEM
const float c_fTitleAchievementPosition = 148.0f;
#endif


const float c_fyGuildNamePosition = 25.0f;
const float c_fyMarkPosition = 25.0f + 11.0f;
const float c_fysTitlePosition = 12.0f;
const float c_fyGuildNamePositionOld = 15.0f;
const float c_fyMarkPositionOld = 15.0f + 11.0f;

// const float c_fyGuildNamePosition = 15.0f;
// const float c_fyMarkPosition = 15.0f + 11.0f;

BOOL bPKTitleEnable = TRUE;

// TEXTTAIL_LIVINGTIME_CONTROL
long gs_TextTail_LivingTime = 5000;

long TextTail_GetLivingTime()
{
	assert(gs_TextTail_LivingTime>1000);
	return gs_TextTail_LivingTime;
}

void TextTail_SetLivingTime(long livingTime)
{
	gs_TextTail_LivingTime = livingTime;
}
// END_OF_TEXTTAIL_LIVINGTIME_CONTROL

CGraphicText * ms_pFont = NULL;

void CPythonTextTail::GetInfo(std::string* pstInfo)
{
	char szInfo[256];
	sprintf(szInfo, "TextTail: ChatTail %d, ChrTail (Map %d, List %d), ItemTail (Map %d, List %d), Pool %d",
		m_ChatTailMap.size(),
		m_CharacterTextTailMap.size(), m_CharacterTextTailList.size(),
		m_ItemTextTailMap.size(), m_ItemTextTailList.size(),
		m_TextTailPool.GetCapacity());

	pstInfo->append(szInfo);
}

void CPythonTextTail::UpdateAllTextTail()
{
	CInstanceBase * pInstance = CPythonCharacterManager::Instance().GetMainInstancePtr();
	if (pInstance)
	{
		TPixelPosition pixelPos;
		pInstance->NEW_GetPixelPosition(&pixelPos);

		TTextTailMap::iterator itorMap;

		for (itorMap = m_CharacterTextTailMap.begin(); itorMap != m_CharacterTextTailMap.end(); ++itorMap)
		{
			UpdateDistance(pixelPos, itorMap->second);
		}

		for (itorMap = m_ItemTextTailMap.begin(); itorMap != m_ItemTextTailMap.end(); ++itorMap)
		{
			UpdateDistance(pixelPos, itorMap->second);
		}

#ifdef ENABLE_SHOP_IN_CITIES
		for (itorMap = m_ShopTextTailMap.begin(); itorMap != m_ShopTextTailMap.end(); ++itorMap)
		{
			UpdateDistance(pixelPos, itorMap->second);
#ifdef ENABLE_OFFLINE_SHOP
			{
				float fRange = CPythonSystem::Instance().GetOfflineShopRange();
				itorMap->second->bRender = (fRange > 0.f) && (itorMap->second->fDistanceFromPlayer < fRange);
			}
#endif
		}
#endif

		for (TChatTailMap::iterator itorChat=m_ChatTailMap.begin(); itorChat!=m_ChatTailMap.end(); ++itorChat)
		{
			UpdateDistance(pixelPos, itorChat->second);

			if (itorChat->second->bNameFlag)
			{
				DWORD dwVID = itorChat->first;
				ShowCharacterTextTail(dwVID);
			}
		}

#ifdef ENABLE_PREMIUM_PRIVATE_SHOP_OFFICIAL
		for (const auto& kv : m_PrivateShopTextTailMap)
		{
			UpdateDistance(pixelPos, kv.second);
		}
#endif
	}
}

void CPythonTextTail::UpdateShowingTextTail()
{
	TTextTailList::iterator itor;

	for (itor = m_ItemTextTailList.begin(); itor != m_ItemTextTailList.end(); ++itor)
	{
		UpdateTextTail(*itor);
	}

	for (TChatTailMap::iterator itorChat=m_ChatTailMap.begin(); itorChat!=m_ChatTailMap.end(); ++itorChat)
	{
		UpdateTextTail(itorChat->second);
	}

#ifdef ENABLE_SHOP_IN_CITIES
	for (auto itorMap = m_ShopTextTailMap.begin(); itorMap != m_ShopTextTailMap.end(); ++itorMap)
	{
		if (itorMap->second->bRender)
		{
			UpdateTextTail(itorMap->second);
		}
	}
#endif

	for (itor = m_CharacterTextTailList.begin(); itor != m_CharacterTextTailList.end(); ++itor)
	{
		TTextTail * pTextTail = *itor;
		UpdateTextTail(pTextTail);

		TChatTailMap::iterator itor = m_ChatTailMap.find(pTextTail->dwVirtualID);
		if (m_ChatTailMap.end() != itor)
		{
			TTextTail * pChatTail = itor->second;
			if (pChatTail->bNameFlag)
			{
				pTextTail->y = pChatTail->y - 17.0f;
			}
		}
	}

#ifdef ENABLE_PREMIUM_PRIVATE_SHOP_OFFICIAL
	for (const auto pTextTail : m_PrivateShopTextTailList)
	{
		UpdateTextTail(pTextTail);
	}
#endif
}

void CPythonTextTail::UpdateTextTail(TTextTail * pTextTail)
{
	if (!pTextTail->pOwner)
		return;

	/////

	CPythonGraphic & rpyGraphic = CPythonGraphic::Instance();
	rpyGraphic.Identity();

	const D3DXVECTOR3 & c_rv3Position = pTextTail->pOwner->GetPosition();
	rpyGraphic.ProjectPosition(c_rv3Position.x,
							   c_rv3Position.y,
							   c_rv3Position.z + pTextTail->fHeight,
							   &pTextTail->x,
							   &pTextTail->y,
							   &pTextTail->z);

	pTextTail->x = floorf(pTextTail->x);
	pTextTail->y = floorf(pTextTail->y);

	if (pTextTail->fDistanceFromPlayer < 1300.0f)
	{
		pTextTail->z = 0.0f;
	}
	else
	{
		pTextTail->z = pTextTail->z * CPythonGraphic::Instance().GetOrthoDepth() * -1.0f;
		pTextTail->z += 10.0f;
	}
}

void CPythonTextTail::ArrangeTextTail()
{
	TTextTailList::iterator itor;
	TTextTailList::iterator itorCompare;

	DWORD dwTime = CTimer::Instance().GetCurrentMillisecond();

	for (itor = m_ItemTextTailList.begin(); itor != m_ItemTextTailList.end(); ++itor)
	{
		TTextTail* pInsertTextTail = *itor;

		int yTemp = 5;
		int LimitCount = 0;

		for (itorCompare = m_ItemTextTailList.begin(); itorCompare != m_ItemTextTailList.end();)
		{
			TTextTail* pCompareTextTail = *itorCompare;

			if (*itorCompare == *itor)
			{
				++itorCompare;
				continue;
			}

			if (LimitCount >= 20)
				break;

			if (isIn(pInsertTextTail, pCompareTextTail))
			{
				pInsertTextTail->y = (pCompareTextTail->y + pCompareTextTail->yEnd + yTemp);

				itorCompare = m_ItemTextTailList.begin();
				++LimitCount;
				continue;
			}

			++itorCompare;
		}

		if (pInsertTextTail->pOwnerTextInstance)
		{
			pInsertTextTail->pOwnerTextInstance->SetPosition(pInsertTextTail->x, pInsertTextTail->y, pInsertTextTail->z);
			pInsertTextTail->pOwnerTextInstance->Update();

			pInsertTextTail->pTextInstance->SetColor(pInsertTextTail->Color.r, pInsertTextTail->Color.g, pInsertTextTail->Color.b);
			pInsertTextTail->pTextInstance->SetPosition(pInsertTextTail->x, pInsertTextTail->y + 15.0f, pInsertTextTail->z);
			pInsertTextTail->pTextInstance->Update();

		}
		else
		{
			pInsertTextTail->pTextInstance->SetColor(pInsertTextTail->Color.r, pInsertTextTail->Color.g, pInsertTextTail->Color.b);
			pInsertTextTail->pTextInstance->SetPosition(pInsertTextTail->x, pInsertTextTail->y, pInsertTextTail->z);
			pInsertTextTail->pTextInstance->Update();

		}
	}
#ifdef ENABLE_SHOP_IN_CITIES
	for (auto itorMap = m_ShopTextTailMap.begin(); itorMap != m_ShopTextTailMap.end(); ++itorMap)
	{
		if (!itorMap->second->bRender)
		{
			continue;
		}

		TTextTail* pInsertTextTail = itorMap->second;

		if (pInsertTextTail->pOwnerTextInstance)
		{
			pInsertTextTail->pOwnerTextInstance->SetPosition(pInsertTextTail->x, pInsertTextTail->y, pInsertTextTail->z);
			pInsertTextTail->pOwnerTextInstance->Update();
			pInsertTextTail->pTextInstance->SetColor(pInsertTextTail->Color.r, pInsertTextTail->Color.g, pInsertTextTail->Color.b);
			pInsertTextTail->pTextInstance->SetPosition(pInsertTextTail->x, pInsertTextTail->y + 15.0f, pInsertTextTail->z);
			pInsertTextTail->pTextInstance->Update();
		}
		else
		{
			pInsertTextTail->pTextInstance->SetColor(pInsertTextTail->Color.r, pInsertTextTail->Color.g, pInsertTextTail->Color.b);
			pInsertTextTail->pTextInstance->SetPosition(pInsertTextTail->x, pInsertTextTail->y, pInsertTextTail->z);
			pInsertTextTail->pTextInstance->Update();
		}
	}
#endif
	for (itor = m_CharacterTextTailList.begin(); itor != m_CharacterTextTailList.end(); ++itor)
	{
		TTextTail* pTextTail = *itor;
		CGraphicMarkInstance* pMarkInstance = pTextTail->pMarkInstance;

		float fxAdd = 0.0f;
#ifdef TITLE_SYSTEM_BYLUZER
		CGraphicTextInstance* pGrade = pTextTail->pGradeTextInstance;
		if (pGrade)
		{
			int iWidth, iHeight;
			pGrade->GetTextSize(&iWidth, &iHeight);
			pGrade->SetPosition(pTextTail->x, pTextTail->y - c_fysTitlePosition, pTextTail->z);
			pGrade->Update();
		}
#endif

#ifdef ENABLE_TITLE_ACHIEVEMENT_SYSTEM
		// Premium Title Achievement - positioned under guild (where primary title was before)
		CGraphicTextInstance* psTitleAchievementPremiumNameInstance = pTextTail->psTitleAchievementPremiumNameTextInstance;
		if (psTitleAchievementPremiumNameInstance)
		{
			int iImageHalfSize = 0;
			if (pMarkInstance)
				iImageHalfSize = pMarkInstance->GetWidth() / 2 + c_fxMarkPosition;

#ifdef TITLE_SYSTEM_BYLUZER
			if (!pGrade)
			{
				psTitleAchievementPremiumNameInstance->SetPosition(
					pTextTail->x + iImageHalfSize,
					pTextTail->y - c_fyGuildNamePositionOld,
					pTextTail->z
				);
			}
			else
			{
				psTitleAchievementPremiumNameInstance->SetPosition(
					pTextTail->x + iImageHalfSize,
					pTextTail->y - c_fyGuildNamePosition,
					pTextTail->z
				);
			}
#else
			psTitleAchievementPremiumNameInstance->SetPosition(
				pTextTail->x + iImageHalfSize,
				pTextTail->y - c_fyGuildNamePosition,
				pTextTail->z
			);
#endif
			psTitleAchievementPremiumNameInstance->Update();
		}

		// Adjust guild name position if premium title exists
		bool bHasPremiumTitle = (psTitleAchievementPremiumNameInstance != nullptr);
		float fGuildYOffset = bHasPremiumTitle ? 15.0f : 0.0f;
#endif

		// Mark and Guild Name
		CGraphicTextInstance* pGuildNameInstance = pTextTail->pGuildNameTextInstance;
		if (pMarkInstance && pGuildNameInstance)
		{
			int iWidth, iHeight;
			int iImageHalfSize = pMarkInstance->GetWidth() / 2 + c_fxMarkPosition;
			pGuildNameInstance->GetTextSize(&iWidth, &iHeight);

#ifdef TITLE_SYSTEM_BYLUZER
			if (!pGrade)
			{
#ifdef ENABLE_TITLE_ACHIEVEMENT_SYSTEM
				pMarkInstance->SetPosition(pTextTail->x - iWidth / 2 - iImageHalfSize, pTextTail->y - c_fyMarkPositionOld - fGuildYOffset);
				pGuildNameInstance->SetPosition(pTextTail->x + iImageHalfSize, pTextTail->y - c_fyGuildNamePositionOld - fGuildYOffset, pTextTail->z);
#else
				pMarkInstance->SetPosition(pTextTail->x - iWidth / 2 - iImageHalfSize, pTextTail->y - c_fyMarkPositionOld);
				pGuildNameInstance->SetPosition(pTextTail->x + iImageHalfSize, pTextTail->y - c_fyGuildNamePositionOld, pTextTail->z);
#endif
			}
			else
			{
#ifdef ENABLE_TITLE_ACHIEVEMENT_SYSTEM
				pMarkInstance->SetPosition(pTextTail->x - iWidth / 2 - iImageHalfSize, pTextTail->y - c_fyMarkPosition - fGuildYOffset);
				pGuildNameInstance->SetPosition(pTextTail->x + iImageHalfSize, pTextTail->y - c_fyGuildNamePosition - fGuildYOffset, pTextTail->z);
#else
				pMarkInstance->SetPosition(pTextTail->x - iWidth / 2 - iImageHalfSize, pTextTail->y - c_fyMarkPosition);
				pGuildNameInstance->SetPosition(pTextTail->x + iImageHalfSize, pTextTail->y - c_fyGuildNamePosition, pTextTail->z);
#endif
			}
#else
#ifdef ENABLE_TITLE_ACHIEVEMENT_SYSTEM
			pMarkInstance->SetPosition(pTextTail->x - iWidth / 2 - iImageHalfSize, pTextTail->y - c_fyMarkPosition - fGuildYOffset);
			pGuildNameInstance->SetPosition(pTextTail->x + iImageHalfSize, pTextTail->y - c_fyGuildNamePosition - fGuildYOffset, pTextTail->z);
#else
			pMarkInstance->SetPosition(pTextTail->x - iWidth / 2 - iImageHalfSize, pTextTail->y - c_fyMarkPosition);
			pGuildNameInstance->SetPosition(pTextTail->x + iImageHalfSize, pTextTail->y - c_fyGuildNamePosition, pTextTail->z);
#endif
#endif
			pGuildNameInstance->Update();
		}

#ifdef TITLE_SYSTEM_BYLUZER
		if (pGrade)
		{
			int iImageHalfSize = 0 + c_fxMarkPosition;
			int iLevelWidth, iLevelHeight;
			pGrade->GetTextSize(&iLevelWidth, &iLevelHeight);
			if (pMarkInstance && pGuildNameInstance)
			{
				pGrade->SetPosition(pTextTail->x + iImageHalfSize, pTextTail->y - c_fysTitlePosition, pTextTail->z);
				pGrade->Update();
			}
			else
			{
				pGrade->SetPosition(pTextTail->x + iImageHalfSize, pTextTail->y - c_fysTitlePosition, pTextTail->z);
				pGrade->Update();
			}
			pGrade->Update();
		}
#endif

		int iNameWidth, iNameHeight;
		pTextTail->pTextInstance->GetTextSize(&iNameWidth, &iNameHeight);

#ifdef ENABLE_TITLE_ACHIEVEMENT_SYSTEM
		CGraphicTextInstance* pTitle = pTextTail->psTitleAchievementNameTextInstance;
#else
		CGraphicTextInstance* pTitle = pTextTail->pTitleTextInstance;
#endif

		if (pTitle)
		{
			float fxAdd = 0.0f;
			int iTitleWidth, iTitleHeight;
			pTitle->GetTextSize(&iTitleWidth, &iTitleHeight);

			CGraphicTextInstance* pLevel = pTextTail->pLevelTextInstance;

#ifdef ENABLE_SECONDARY_LEVEL
			int iLevelWidth = 0, iLevelHeight = 0;
			if (pLevel)
				pLevel->GetTextSize(&iLevelWidth, &iLevelHeight);

			CGraphicTextInstance* pSecondaryLevel = pTextTail->pSecondaryLevelTextInstance;
			int iSecondaryLevelWidth = 0, iSecondaryLevelHeight = 0;
			if (pSecondaryLevel)
				pSecondaryLevel->GetTextSize(&iSecondaryLevelWidth, &iSecondaryLevelHeight);
#endif

#ifdef ENABLE_ENLIGHTENMENT_SYSTEM
			CGraphicTextInstance* pEnlightenmentLevel = pTextTail->pEnlightenmentLevelTextInstance;
			int iEnlightenmentLevelWidth = 0, iEnlightenmentLevelHeight = 0;
			if (pEnlightenmentLevel)
				pEnlightenmentLevel->GetTextSize(&iEnlightenmentLevelWidth, &iEnlightenmentLevelHeight);
#endif

			const float SPACING = 3.0f;

			// Calculate level width
			int iLevelTotalWidth = 0;
			if (pLevel)
			{
#ifndef ENABLE_SECONDARY_LEVEL
				int iLevelWidth, iLevelHeight;
				pLevel->GetTextSize(&iLevelWidth, &iLevelHeight);
#endif
				iLevelTotalWidth = iLevelWidth;
			}

			// Calculate total width: Level + 3px + Title + 3px + Name
			float fTotalWidth = iLevelTotalWidth + SPACING + iTitleWidth + SPACING + iNameWidth;

			// Starting position (leftmost edge of the group, centered around pTextTail->x)
			float fCurrentX = pTextTail->x - (fTotalWidth / 2.0f);

			// Position Level (if exists)
			if (pLevel)
			{
				pLevel->SetHorizonalAlign(CGraphicTextInstance::HORIZONTAL_ALIGN_LEFT);
				pLevel->SetVerticalAlign(CGraphicTextInstance::VERTICAL_ALIGN_BOTTOM);
				pLevel->SetPosition(fCurrentX, pTextTail->y, pTextTail->z);
				pLevel->Update();
				fCurrentX += iLevelTotalWidth + SPACING;
			}
			else
			{
				fCurrentX += SPACING; // Add spacing even if no level
			}

			// Position Title (middle)
			pTitle->SetHorizonalAlign(CGraphicTextInstance::HORIZONTAL_ALIGN_LEFT);
			pTitle->SetVerticalAlign(CGraphicTextInstance::VERTICAL_ALIGN_BOTTOM);
			pTitle->SetPosition(fCurrentX, pTextTail->y, pTextTail->z);
			pTitle->Update();
			fCurrentX += iTitleWidth + SPACING;

			// Position Name (rightmost)
			pTextTail->pTextInstance->SetHorizonalAlign(CGraphicTextInstance::HORIZONTAL_ALIGN_LEFT);
			pTextTail->pTextInstance->SetPosition(fCurrentX, pTextTail->y, pTextTail->z);
			fCurrentX += iNameWidth + SPACING;

			// NOW position Secondary Level AFTER the name
#ifdef ENABLE_SECONDARY_LEVEL
			if (pSecondaryLevel)
			{
				pSecondaryLevel->SetHorizonalAlign(CGraphicTextInstance::HORIZONTAL_ALIGN_LEFT);
				pSecondaryLevel->SetVerticalAlign(CGraphicTextInstance::VERTICAL_ALIGN_BOTTOM);
				pSecondaryLevel->SetPosition(fCurrentX, pTextTail->y, pTextTail->z);
				pSecondaryLevel->Update();
			}
#endif
#ifdef ENABLE_ENLIGHTENMENT_SYSTEM
			if (pEnlightenmentLevel)
			{
				pEnlightenmentLevel->SetHorizonalAlign(CGraphicTextInstance::HORIZONTAL_ALIGN_LEFT);
				pEnlightenmentLevel->SetVerticalAlign(CGraphicTextInstance::VERTICAL_ALIGN_BOTTOM);
				pEnlightenmentLevel->SetPosition(fCurrentX, pTextTail->y, pTextTail->z);
				pEnlightenmentLevel->Update();
			}
#endif
		}
		else
		{
			fxAdd = 4.0f;

			CGraphicTextInstance* pLevel = pTextTail->pLevelTextInstance;
#ifdef ENABLE_SECONDARY_LEVEL
			int iLevelWidth = 0, iLevelHeight = 0;
			if (pLevel)
				pLevel->GetTextSize(&iLevelWidth, &iLevelHeight);

			CGraphicTextInstance* pSecondaryLevel = pTextTail->pSecondaryLevelTextInstance;
			int iSecondaryLevelWidth = 0, iSecondaryLevelHeight = 0;
			if (pSecondaryLevel)
				pSecondaryLevel->GetTextSize(&iSecondaryLevelWidth, &iSecondaryLevelHeight);
#endif
#ifdef ENABLE_ENLIGHTENMENT_SYSTEM
			CGraphicTextInstance* pEnlightenmentLevel = pTextTail->pEnlightenmentLevelTextInstance;
			int iEnlightenmentLevelWidth = 0, iEnlightenmentLevelHeight = 0;
			if (pEnlightenmentLevel)
				pEnlightenmentLevel->GetTextSize(&iEnlightenmentLevelWidth, &iEnlightenmentLevelHeight);
#endif
			if (pLevel)
			{
#ifndef ENABLE_SECONDARY_LEVEL
				int iLevelWidth, iLevelHeight;
				pLevel->GetTextSize(&iLevelWidth, &iLevelHeight);
#endif

#ifdef ENABLE_SECONDARY_LEVEL
				if (pSecondaryLevel)
				{
					pLevel->SetPosition(pTextTail->x - (iNameWidth / 2), pTextTail->y, pTextTail->z);
					pSecondaryLevel->SetPosition(pTextTail->x + (iNameWidth / 2) + 32.0f + 12.0f, pTextTail->y, pTextTail->z);
					pSecondaryLevel->Update();
				}
				else
#endif
#ifdef ENABLE_ENLIGHTENMENT_SYSTEM
					if (pEnlightenmentLevel)
					{
						pLevel->SetPosition(pTextTail->x - (iNameWidth / 2), pTextTail->y, pTextTail->z);
						pEnlightenmentLevel->SetPosition(pTextTail->x + (iNameWidth / 2) + 2.0f + 2.0f, pTextTail->y, pTextTail->z);
						pEnlightenmentLevel->Update();
					}
					else
#endif
						pLevel->SetPosition(pTextTail->x - (iNameWidth / 2), pTextTail->y, pTextTail->z);
				pLevel->Update();
			}

			pTextTail->pTextInstance->SetPosition(pTextTail->x + fxAdd, pTextTail->y, pTextTail->z);
		}

		pTextTail->pTextInstance->SetColor(pTextTail->Color.r, pTextTail->Color.g, pTextTail->Color.b);
		pTextTail->pTextInstance->Update();
	}

#ifdef ENABLE_PREMIUM_PRIVATE_SHOP_OFFICIAL
	for (const auto pTextTail : m_PrivateShopTextTailList)
	{
		pTextTail->pTextInstance->SetColor(pTextTail->Color);
		pTextTail->pTextInstance->SetPosition(pTextTail->x, pTextTail->y, pTextTail->z);
		pTextTail->pTextInstance->Update();
	}
#endif

	for (TChatTailMap::iterator itorChat = m_ChatTailMap.begin(); itorChat != m_ChatTailMap.end();)
	{
		TTextTail* pTextTail = itorChat->second;

		if (pTextTail->LivingTime < dwTime)
		{
			DeleteTextTail(pTextTail);
			itorChat = m_ChatTailMap.erase(itorChat);
			continue;
		}
		else
			++itorChat;

		pTextTail->pTextInstance->SetColor(pTextTail->Color);
		pTextTail->pTextInstance->SetPosition(pTextTail->x, pTextTail->y, pTextTail->z);
		pTextTail->pTextInstance->Update();
	}
}

void CPythonTextTail::Render()
{
	TTextTailList::iterator itor;

	for (itor = m_CharacterTextTailList.begin(); itor != m_CharacterTextTailList.end(); ++itor)
	{
		TTextTail * pTextTail = *itor;
		pTextTail->pTextInstance->Render();
		if (pTextTail->pMarkInstance && pTextTail->pGuildNameTextInstance)
		{
			pTextTail->pMarkInstance->Render();
			pTextTail->pGuildNameTextInstance->Render();
		}
		if (pTextTail->pTitleTextInstance)
		{
			pTextTail->pTitleTextInstance->Render();
		}
		if (pTextTail->pLevelTextInstance)
		{
			pTextTail->pLevelTextInstance->Render();
		}
#ifdef TITLE_SYSTEM_BYLUZER
		if (pTextTail->pGradeTextInstance)
		{
			pTextTail->pGradeTextInstance->Render();
		}
#endif
#ifdef ENABLE_SECONDARY_LEVEL
		if (pTextTail->pSecondaryLevelTextInstance)
		{
			pTextTail->pSecondaryLevelTextInstance->Render();
		}
#endif
#ifdef ENABLE_ENLIGHTENMENT_SYSTEM
		if (pTextTail->pEnlightenmentLevelTextInstance)
		{
			pTextTail->pEnlightenmentLevelTextInstance->Render();
		}
#endif
#ifdef ENABLE_TITLE_ACHIEVEMENT_SYSTEM
		if (pTextTail->psTitleAchievementNameTextInstance)
		{
			pTextTail->psTitleAchievementNameTextInstance->Render();
		}
		if (pTextTail->psTitleAchievementPremiumNameTextInstance)
		{
			pTextTail->psTitleAchievementPremiumNameTextInstance->Render();
		}
#endif
	}

	for (itor = m_ItemTextTailList.begin(); itor != m_ItemTextTailList.end(); ++itor)
	{
		TTextTail * pTextTail = *itor;

		RenderTextTailBox(pTextTail);
		pTextTail->pTextInstance->Render();
		if (pTextTail->pOwnerTextInstance)
			pTextTail->pOwnerTextInstance->Render();
	}

#ifdef ENABLE_SHOP_IN_CITIES
#ifdef ENABLE_HIDE_MOB_SYSTEM
	if (!CPythonSystem::Instance().GetHiddenState(3))
	{
		if (CPythonSystem::Instance().IsShowSalesText())
		{
			for (auto itorMap = m_ShopTextTailMap.begin(); itorMap != m_ShopTextTailMap.end(); ++itorMap)
			{
				if (!itorMap->second->bRender)
					continue;

				auto pTextTail = itorMap->second;
				RenderTextTailBox(pTextTail);

				pTextTail->pTextInstance->Render();
				if (pTextTail->pOwnerTextInstance)
				{
					pTextTail->pOwnerTextInstance->Render();
				}
			}
		}
	}
#else
	if (CPythonSystem::Instance().IsShowSalesText())
	{
		for (auto itorMap = m_ShopTextTailMap.begin(); itorMap != m_ShopTextTailMap.end(); ++itorMap)
		{
			if (!itorMap->second->bRender)
				continue;

			auto pTextTail = itorMap->second;
			RenderTextTailBox(pTextTail);

			pTextTail->pTextInstance->Render();
			if (pTextTail->pOwnerTextInstance)
			{
				pTextTail->pOwnerTextInstance->Render();
			}
		}
	}
#endif
#endif
	for (TChatTailMap::iterator itorChat = m_ChatTailMap.begin(); itorChat!=m_ChatTailMap.end(); ++itorChat)
	{
		TTextTail * pTextTail = itorChat->second;
		if (pTextTail->pOwner->isShow())
			RenderTextTailName(pTextTail);
	}

#ifdef ENABLE_PREMIUM_PRIVATE_SHOP_OFFICIAL
	for (const auto pTextTail : m_PrivateShopTextTailList)
	{
		pTextTail->pTextInstance->Render();
	}
#endif
}


#ifdef ENABLE_SHOP_IN_CITIES
CPythonTextTail::TTextTail* CPythonTextTail::RegisterShopTextTail(DWORD dwVirtualID, const char* c_szText, CGraphicObjectInstance* pOwner)
{
	const D3DXCOLOR& c_rColor = D3DXCOLOR(255.0, 255.0, 255.0, 1.0);

	TTextTail* pTextTail = m_TextTailPool.Alloc();
	pTextTail->bIsShop = true;
	pTextTail->dwVirtualID = dwVirtualID;
	pTextTail->pOwner = pOwner;
	pTextTail->pTextInstance = CGraphicTextInstance::New();
	pTextTail->pOwnerTextInstance = NULL;
	pTextTail->fHeight = 180.f;

	pTextTail->pTextInstance->SetTextPointer(ms_pFont);
	pTextTail->pTextInstance->SetHorizonalAlign(CGraphicTextInstance::HORIZONTAL_ALIGN_CENTER);
	pTextTail->pTextInstance->SetValue(c_szText);
///#ifdef WJ_MULTI_TEXTLINE
///	pTextTail->pTextInstance->DisableEnterToken();
///#endif
	pTextTail->pTextInstance->SetColor(c_rColor.r, c_rColor.g, c_rColor.b);
	pTextTail->pTextInstance->Update();

	int xSize, ySize;
	pTextTail->pTextInstance->GetTextSize(&xSize, &ySize);
	pTextTail->xStart = (float)(-xSize / 2 - 2);
	pTextTail->yStart = -2.0f;
	pTextTail->xEnd = (float)(xSize / 2 + 2);
	pTextTail->yEnd = (float)ySize;
	pTextTail->Color = c_rColor;
	pTextTail->fDistanceFromPlayer = 0.0f;
	pTextTail->x = -100.0f;
	pTextTail->y = -100.0f;
	pTextTail->z = 0.0f;
	pTextTail->pMarkInstance = NULL;
	pTextTail->pGuildNameTextInstance = NULL;
	pTextTail->pTitleTextInstance = NULL;
	pTextTail->pLevelTextInstance = NULL;
	pTextTail->pGradeTextInstance = NULL;
#ifdef ENABLE_SECONDARY_LEVEL
	pTextTail->pSecondaryLevelTextInstance = NULL;
#endif
#ifdef ENABLE_ENLIGHTENMENT_SYSTEM
	pTextTail->pEnlightenmentLevelTextInstance = NULL;
#endif
#ifdef ENABLE_TITLE_ACHIEVEMENT_SYSTEM
	pTextTail->psTitleAchievementNameTextInstance = NULL;
	pTextTail->psTitleAchievementPremiumNameTextInstance = NULL;
#endif
	return pTextTail;
}

bool CPythonTextTail::GetPickedNewShop(DWORD* pdwVID)
{
	*pdwVID = 0;

	if (!CPythonOfflineshop::instance().GetShowNameFlag() && !CPythonSystem::instance().IsAlwaysShowName())
		return false;

	long ixMouse = 0, iyMouse = 0;

	POINT p;
	CPythonApplication::Instance().GetMousePosition(&p);

	ixMouse = p.x;
	iyMouse = p.y;

	for (auto itor = m_ShopTextTailMap.begin(); itor != m_ShopTextTailMap.end(); ++itor)
	{
		TTextTail* pTextTail = itor->second;
		if (ixMouse >= pTextTail->x + (pTextTail->xStart - 10) && ixMouse <= pTextTail->x + (pTextTail->xEnd + 10) &&
			iyMouse >= pTextTail->y + (pTextTail->yStart - 10) && iyMouse <= pTextTail->y + (pTextTail->yEnd + 10))
		{
			*pdwVID = itor->first;
			return true;
		}
	}

	return false;
}
#endif

void CPythonTextTail::RenderTextTailBox(TTextTail * pTextTail)
{
#ifdef ENABLE_OFFLINE_SHOP
#ifdef ENABLE_SHOP_IN_CITIES
	if (pTextTail->bIsShop)
	{
		CPythonGraphic& grp{ CPythonGraphic::Instance() };

		CPythonGraphic::Instance().SetDiffuseColor(0.0f, 0.0f, 0.0f, 1.0f);
		CPythonGraphic::Instance().RenderGradationBar2d(pTextTail->x + pTextTail->xStart - 10.f,
			pTextTail->y + pTextTail->yStart - 10.f,
			pTextTail->x + pTextTail->xEnd + 10.f,
			pTextTail->y + pTextTail->yEnd + 10.f,
			grp.GenerateColor(0.0f, 0.2f, 0.0f, 0.35f),
			grp.GenerateColor(0.0f, 0.0f, 0.0f, 0.3f),
			pTextTail->z);

		CPythonGraphic::Instance().SetDiffuseColor(0.0f, 0.0f, 0.0f, 0.3f);
		CPythonGraphic::Instance().RenderGradationBar2d(pTextTail->x + pTextTail->xStart - 10.f,
			pTextTail->y + pTextTail->yStart - 10.f,
			pTextTail->x + pTextTail->xEnd + 10.f,
			pTextTail->y + pTextTail->yEnd + 10.f,
			grp.GenerateColor(0.0f, 0.2f, 0.0f, 0.35f),
			grp.GenerateColor(0.0f, 0.0f, 0.0f, 0.3f),
			pTextTail->z);
		return;
	}
#endif
#endif
	CPythonGraphic::Instance().SetDiffuseColor(0.85f, 0.75f, 0.46f, 1.0f);
	CPythonGraphic::Instance().RenderBox2d(pTextTail->x + pTextTail->xStart, pTextTail->y + pTextTail->yStart, pTextTail->x + pTextTail->xEnd, pTextTail->y + pTextTail->yEnd, pTextTail->z);
	CPythonGraphic::Instance().SetDiffuseColor(0.0f, 0.0f, 0.0f, 0.3f);
	CPythonGraphic::Instance().RenderBar2d(pTextTail->x + pTextTail->xStart, pTextTail->y + pTextTail->yStart, pTextTail->x + pTextTail->xEnd, pTextTail->y + pTextTail->yEnd, pTextTail->z);
}

void CPythonTextTail::RenderTextTailName(TTextTail * pTextTail)
{
	pTextTail->pTextInstance->Render();
}

void CPythonTextTail::HideAllTextTail()
{
	m_CharacterTextTailList.clear();
	m_ItemTextTailList.clear();
#ifdef ENABLE_PREMIUM_PRIVATE_SHOP_OFFICIAL
	m_PrivateShopTextTailList.clear();
#endif

#ifdef ENABLE_SHOP_IN_CITIES
	for (auto& iter : m_ShopTextTailMap)
		iter.second->bRender = false;
#endif
}

void CPythonTextTail::UpdateDistance(const TPixelPosition & c_rCenterPosition, TTextTail * pTextTail)
{
	const D3DXVECTOR3 & c_rv3Position = pTextTail->pOwner->GetPosition();
	D3DXVECTOR2 v2Distance(c_rv3Position.x - c_rCenterPosition.x, -c_rv3Position.y - c_rCenterPosition.y);
	pTextTail->fDistanceFromPlayer = D3DXVec2Length(&v2Distance);
}

void CPythonTextTail::ShowAllTextTail()
{
	TTextTailMap::iterator itor;
	for (itor = m_CharacterTextTailMap.begin(); itor != m_CharacterTextTailMap.end(); ++itor)
	{
		TTextTail * pTextTail = itor->second;
		if (pTextTail->fDistanceFromPlayer < 3500.0f && CPythonCharacterManager::Instance().GetInstancePtr(pTextTail->dwVirtualID)->GetRace() != 30000)
			ShowCharacterTextTail(itor->first);
	}
	for (itor = m_ItemTextTailMap.begin(); itor != m_ItemTextTailMap.end(); ++itor)
	{
		TTextTail * pTextTail = itor->second;
		if (pTextTail->fDistanceFromPlayer < 3500.0f)
			ShowItemTextTail(itor->first);
	}
#ifdef ENABLE_SHOP_IN_CITIES
	for (itor = m_ShopTextTailMap.begin(); itor != m_ShopTextTailMap.end(); ++itor)
	{
		TTextTail* pTextTail = itor->second;
#ifdef ENABLE_OUTLINED_NAMES
		pTextTail->pTextInstance->SetOutline(CPythonSystem::Instance().GetNamesType());
#endif
#ifdef ENABLE_OFFLINE_SHOP
		{
			float fRange = CPythonSystem::Instance().GetOfflineShopRange();
			pTextTail->bRender = (fRange > 0.f) && (pTextTail->fDistanceFromPlayer < fRange);
		}
#else
		pTextTail->bRender = pTextTail->fDistanceFromPlayer < 3500.f;
#endif
	}
#endif

#ifdef ENABLE_PREMIUM_PRIVATE_SHOP_OFFICIAL
	for (itor = m_PrivateShopTextTailMap.begin(); itor != m_PrivateShopTextTailMap.end(); ++itor)
	{
		TTextTail* pTextTail = itor->second;
		if (pTextTail->fDistanceFromPlayer < CPythonSystem::Instance().GetPrivateShopViewDistance() * MAX_VIEW_DISTANCE)
			ShowPrivateShopTextTail(itor->first);
}
#endif
}

void CPythonTextTail::ShowCharacterTextTail(DWORD VirtualID)
{
	TTextTailMap::iterator itor = m_CharacterTextTailMap.find(VirtualID);

	if (m_CharacterTextTailMap.end() == itor)
		return;

	TTextTail * pTextTail = itor->second;

	if (m_CharacterTextTailList.end() != std::find(m_CharacterTextTailList.begin(), m_CharacterTextTailList.end(), pTextTail))
	{
		return;
	}

	if (!pTextTail->pOwner->isShow())
		return;

	CInstanceBase * pInstance = CPythonCharacterManager::Instance().GetInstancePtr(pTextTail->dwVirtualID);

#ifdef ENABLE_GRAPHIC_ON_OFF
	if (CPythonSystem::instance().IsNpcNameStatus() == 1)
		if (pInstance->IsNPC())
			return;
#endif

	if (!pInstance)
		return;

	if (pInstance->IsGuildWall())
		return;

	if (pInstance->CanPickInstance())
		m_CharacterTextTailList.push_back(pTextTail);
}

void CPythonTextTail::ShowItemTextTail(DWORD VirtualID)
{
	TTextTailMap::iterator itor = m_ItemTextTailMap.find(VirtualID);

	if (m_ItemTextTailMap.end() == itor)
		return;

	TTextTail * pTextTail = itor->second;

	if (m_ItemTextTailList.end() != std::find(m_ItemTextTailList.begin(), m_ItemTextTailList.end(), pTextTail))
	{
		return;
	}

	m_ItemTextTailList.push_back(pTextTail);
}

#ifdef ENABLE_SHOP_IN_CITIES
void CPythonTextTail::RegisterShopInstanceTextTail(DWORD dwVirtualID, const char* c_szName, CGraphicObjectInstance* pOwner)
{
	TTextTail* pTextTail = RegisterShopTextTail(dwVirtualID, c_szName, pOwner);
	m_ShopTextTailMap.insert(TTextTailMap::value_type(dwVirtualID, pTextTail));
}
#endif
bool CPythonTextTail::isIn(CPythonTextTail::TTextTail * pSource, CPythonTextTail::TTextTail * pTarget)
{
	float x1Source = pSource->x + pSource->xStart;
	float y1Source = pSource->y + pSource->yStart;
	float x2Source = pSource->x + pSource->xEnd;
	float y2Source = pSource->y + pSource->yEnd;
	float x1Target = pTarget->x + pTarget->xStart;
	float y1Target = pTarget->y + pTarget->yStart;
	float x2Target = pTarget->x + pTarget->xEnd;
	float y2Target = pTarget->y + pTarget->yEnd;

	if (x1Source <= x2Target && x2Source >= x1Target &&
	    y1Source <= y2Target && y2Source >= y1Target)
	{
		return true;
	}

	return false;
}

void CPythonTextTail::RegisterCharacterTextTail(DWORD dwGuildID, DWORD dwVirtualID, const D3DXCOLOR & c_rColor, float fAddHeight)
{
	CInstanceBase * pCharacterInstance = CPythonCharacterManager::Instance().GetInstancePtr(dwVirtualID);

	if (!pCharacterInstance)
		return;
	


#ifdef ENABLE_PVP_RANKING
	const char * newPlayerName =  pCharacterInstance->GetNameString();
	
	std::string strMapName = CPythonBackground::Instance().GetWarpMapName();
	if (strMapName == "zaris_arena")
	{
		newPlayerName = "Player";
	}
		
	TTextTail * pTextTail = RegisterTextTail(dwVirtualID,
											 newPlayerName,
											 pCharacterInstance->GetGraphicThingInstancePtr(),
											 pCharacterInstance->GetGraphicThingInstanceRef().GetHeight() + fAddHeight,
											 c_rColor);
#else

	TTextTail * pTextTail = RegisterTextTail(dwVirtualID,
											 pCharacterInstance->GetNameString(),
											 pCharacterInstance->GetGraphicThingInstancePtr(),
											 pCharacterInstance->GetGraphicThingInstanceRef().GetHeight() + fAddHeight,
											 c_rColor);
#endif

	CGraphicTextInstance * pTextInstance = pTextTail->pTextInstance;

#ifdef ENABLE_ASLAN_BUFF_NPC_SYSTEM
	if (pCharacterInstance->IsBuffNPC(pCharacterInstance->GetVirtualID()))
	{
		char szText[256];
		sprintf(szText, "|cffff82fbBuff|r %s", pCharacterInstance->GetNameString());
		pTextInstance->SetValue(szText);
		pTextInstance->Update();
	}
#endif

	pTextInstance->SetOutline(true);
	pTextInstance->SetVerticalAlign(CGraphicTextInstance::VERTICAL_ALIGN_BOTTOM);

	pTextTail->pMarkInstance=NULL;
	pTextTail->pGuildNameTextInstance=NULL;
	pTextTail->pTitleTextInstance=NULL;
	pTextTail->pLevelTextInstance=NULL;
#ifdef TITLE_SYSTEM_BYLUZER
	pTextTail->pGradeTextInstance = NULL;
#endif
#ifdef ENABLE_SECONDARY_LEVEL
	pTextTail->pSecondaryLevelTextInstance = NULL;
#endif
#ifdef ENABLE_ENLIGHTENMENT_SYSTEM
	pTextTail->pEnlightenmentLevelTextInstance = NULL;
#endif
#ifdef ENABLE_TITLE_ACHIEVEMENT_SYSTEM
	pTextTail->psTitleAchievementNameTextInstance = NULL;
	pTextTail->psTitleAchievementPremiumNameTextInstance = NULL;
#endif
	pTextTail->bIsPC = pCharacterInstance->IsPC();

#ifdef ENABLE_PVP_RANKING
	if (0 != dwGuildID && strMapName != "zaris_arena")
#else
	if (0 != dwGuildID)
#endif
	{
		pTextTail->pMarkInstance = CGraphicMarkInstance::New();

		DWORD dwMarkID = CGuildMarkManager::Instance().GetMarkID(dwGuildID);

		if (dwMarkID != CGuildMarkManager::INVALID_MARK_ID)
		{
			std::string markImagePath;

			if (CGuildMarkManager::Instance().GetMarkImageFilename(dwMarkID / CGuildMarkImage::MARK_TOTAL_COUNT, markImagePath))
			{
				pTextTail->pMarkInstance->SetImageFileName(markImagePath.c_str());
				pTextTail->pMarkInstance->Load();
				pTextTail->pMarkInstance->SetIndex(dwMarkID % CGuildMarkImage::MARK_TOTAL_COUNT);
			}
		}

		std::string strGuildName;
		if (!CPythonGuild::Instance().GetGuildName(dwGuildID, &strGuildName))
			strGuildName = "Noname";

		CGraphicTextInstance *& prGuildNameInstance = pTextTail->pGuildNameTextInstance;
		prGuildNameInstance = CGraphicTextInstance::New();
		prGuildNameInstance->SetTextPointer(ms_pFont);
		prGuildNameInstance->SetOutline(true);
		prGuildNameInstance->SetHorizonalAlign(CGraphicTextInstance::HORIZONTAL_ALIGN_CENTER);
		prGuildNameInstance->SetVerticalAlign(CGraphicTextInstance::VERTICAL_ALIGN_BOTTOM);
		prGuildNameInstance->SetValue(strGuildName.c_str());
		prGuildNameInstance->SetColor(c_TextTail_Guild_Name_Color.r, c_TextTail_Guild_Name_Color.g, c_TextTail_Guild_Name_Color.b);
		prGuildNameInstance->Update();
	}

	m_CharacterTextTailMap.insert(TTextTailMap::value_type(dwVirtualID, pTextTail));
}

#ifdef ENABLE_EXTENDED_ITEMNAME_ON_GROUND
void CPythonTextTail::RegisterItemTextTail(DWORD VirtualID, const char* c_szText, CGraphicObjectInstance* pOwner, bool bHasAttr)
#else
void CPythonTextTail::RegisterItemTextTail(DWORD VirtualID, const char* c_szText, CGraphicObjectInstance* pOwner)
#endif
{
#ifdef __DEBUG
	char szName[256];
	spritnf(szName, "%s[%d]", c_szText, VirtualID);
#endif

	D3DXCOLOR c_d3dColor = c_TextTail_Item_Color;
#ifdef ENABLE_EXTENDED_ITEMNAME_ON_GROUND
	if (bHasAttr)
		c_d3dColor = c_TextTail_SpecialItem_Color;
#endif

	TTextTail* pTextTail = RegisterTextTail(VirtualID, c_szText, pOwner, c_TextTail_Name_Position, c_d3dColor);
	m_ItemTextTailMap.insert(TTextTailMap::value_type(VirtualID, pTextTail));
}

void CPythonTextTail::RegisterChatTail(DWORD VirtualID, const char * c_szChat)
{
	CInstanceBase * pCharacterInstance = CPythonCharacterManager::Instance().GetInstancePtr(VirtualID);

	if (!pCharacterInstance)
		return;

	TChatTailMap::iterator itor = m_ChatTailMap.find(VirtualID);

	if (m_ChatTailMap.end() != itor)
	{
		TTextTail * pTextTail = itor->second;

		pTextTail->pTextInstance->SetValue(c_szChat);
		pTextTail->pTextInstance->Update();
		pTextTail->Color = c_TextTail_Chat_Color;
		pTextTail->pTextInstance->SetColor(c_TextTail_Chat_Color);

		// TEXTTAIL_LIVINGTIME_CONTROL
		pTextTail->LivingTime = CTimer::Instance().GetCurrentMillisecond() + TextTail_GetLivingTime();
		// END_OF_TEXTTAIL_LIVINGTIME_CONTROL

		pTextTail->bNameFlag = TRUE;

		return;
	}

	TTextTail * pTextTail = RegisterTextTail(VirtualID,
											 c_szChat,
											 pCharacterInstance->GetGraphicThingInstancePtr(),
											 pCharacterInstance->GetGraphicThingInstanceRef().GetHeight() + 10.0f,
											 c_TextTail_Chat_Color);

	// TEXTTAIL_LIVINGTIME_CONTROL
	pTextTail->LivingTime = CTimer::Instance().GetCurrentMillisecond() + TextTail_GetLivingTime();
	// END_OF_TEXTTAIL_LIVINGTIME_CONTROL

	pTextTail->bNameFlag = TRUE;
	pTextTail->pTextInstance->SetOutline(true);
	pTextTail->pTextInstance->SetVerticalAlign(CGraphicTextInstance::VERTICAL_ALIGN_BOTTOM);
	m_ChatTailMap.insert(TTextTailMap::value_type(VirtualID, pTextTail));
}

void CPythonTextTail::RegisterInfoTail(DWORD VirtualID, const char * c_szChat)
{
	CInstanceBase * pCharacterInstance = CPythonCharacterManager::Instance().GetInstancePtr(VirtualID);

	if (!pCharacterInstance)
		return;

	TChatTailMap::iterator itor = m_ChatTailMap.find(VirtualID);

	if (m_ChatTailMap.end() != itor)
	{
		TTextTail * pTextTail = itor->second;

		pTextTail->pTextInstance->SetValue(c_szChat);
		pTextTail->pTextInstance->Update();
		pTextTail->Color = c_TextTail_Info_Color;
		pTextTail->pTextInstance->SetColor(c_TextTail_Info_Color);

		// TEXTTAIL_LIVINGTIME_CONTROL
		pTextTail->LivingTime = CTimer::Instance().GetCurrentMillisecond() + TextTail_GetLivingTime();
		// END_OF_TEXTTAIL_LIVINGTIME_CONTROL

		pTextTail->bNameFlag = FALSE;

		return;
	}

	TTextTail * pTextTail = RegisterTextTail(VirtualID,
											 c_szChat,
											 pCharacterInstance->GetGraphicThingInstancePtr(),
											 pCharacterInstance->GetGraphicThingInstanceRef().GetHeight() + 10.0f,
											 c_TextTail_Info_Color);

	// TEXTTAIL_LIVINGTIME_CONTROL
	pTextTail->LivingTime = CTimer::Instance().GetCurrentMillisecond() + TextTail_GetLivingTime();
	// END_OF_TEXTTAIL_LIVINGTIME_CONTROL

	pTextTail->bNameFlag = FALSE;
	pTextTail->pTextInstance->SetOutline(true);
	pTextTail->pTextInstance->SetVerticalAlign(CGraphicTextInstance::VERTICAL_ALIGN_BOTTOM);
	m_ChatTailMap.insert(TTextTailMap::value_type(VirtualID, pTextTail));
}

bool CPythonTextTail::GetTextTailPosition(DWORD dwVID, float* px, float* py, float* pz)
{
	TTextTailMap::iterator itorCharacter = m_CharacterTextTailMap.find(dwVID);

	if (m_CharacterTextTailMap.end() == itorCharacter)
	{
		return false;
	}

	TTextTail * pTextTail = itorCharacter->second;
	*px=pTextTail->x;
	*py=pTextTail->y;
	*pz=pTextTail->z;

	return true;
}

bool CPythonTextTail::IsChatTextTail(DWORD dwVID)
{
	TChatTailMap::iterator itorChat = m_ChatTailMap.find(dwVID);

	if (m_ChatTailMap.end() == itorChat)
		return false;

	return true;
}

void CPythonTextTail::SetCharacterTextTailColor(DWORD VirtualID, const D3DXCOLOR & c_rColor)
{
	TTextTailMap::iterator itorCharacter = m_CharacterTextTailMap.find(VirtualID);

	if (m_CharacterTextTailMap.end() == itorCharacter)
		return;

	TTextTail * pTextTail = itorCharacter->second;
	pTextTail->pTextInstance->SetColor(c_rColor);
	pTextTail->Color = c_rColor;
}

void CPythonTextTail::SetItemTextTailOwner(DWORD dwVID, const char * c_szName)
{
	TTextTailMap::iterator itor = m_ItemTextTailMap.find(dwVID);
	if (m_ItemTextTailMap.end() == itor)
		return;

	TTextTail * pTextTail = itor->second;

	if (strlen(c_szName) > 0)
	{
		if (!pTextTail->pOwnerTextInstance)
		{
			pTextTail->pOwnerTextInstance = CGraphicTextInstance::New();
		}

		std::string strName = c_szName;
		static const string & strOwnership = ApplicationStringTable_GetString(IDS_POSSESSIVE_MORPHENE) == "" ? "'s" : ApplicationStringTable_GetString(IDS_POSSESSIVE_MORPHENE);
		strName += strOwnership;

		pTextTail->pOwnerTextInstance->SetTextPointer(ms_pFont);
		pTextTail->pOwnerTextInstance->SetHorizonalAlign(CGraphicTextInstance::HORIZONTAL_ALIGN_CENTER);
		pTextTail->pOwnerTextInstance->SetValue(strName.c_str());
#ifdef ENABLE_EXTENDED_ITEMNAME_ON_GROUND
		CInstanceBase* pInstance = CPythonCharacterManager::Instance().GetMainInstancePtr();
		if (pInstance)
		{
			if (!strcmp(pInstance->GetNameString(), c_szName))
				pTextTail->pOwnerTextInstance->SetColor(1.0f, 1.0f, 0.0f);
			else
				pTextTail->pOwnerTextInstance->SetColor(1.0f, 0.0f, 0.0f);
		}
#else
		pTextTail->pOwnerTextInstance->SetColor(1.0f, 1.0f, 0.0f);
#endif
		pTextTail->pOwnerTextInstance->Update();

		int xOwnerSize, yOwnerSize;
		pTextTail->pOwnerTextInstance->GetTextSize(&xOwnerSize, &yOwnerSize);
		pTextTail->yStart	= -2.0f;
		pTextTail->yEnd		+= float(yOwnerSize + 4);
		pTextTail->xStart	= fMIN(pTextTail->xStart, float(-xOwnerSize / 2 - 1));
		pTextTail->xEnd		= fMAX(pTextTail->xEnd, float(xOwnerSize / 2 + 1));
	}
	else
	{
		if (pTextTail->pOwnerTextInstance)
		{
			CGraphicTextInstance::Delete(pTextTail->pOwnerTextInstance);
			pTextTail->pOwnerTextInstance = NULL;
		}

		int xSize, ySize;
		pTextTail->pTextInstance->GetTextSize(&xSize, &ySize);
		pTextTail->xStart	= (float) (-xSize / 2 - 2);
		pTextTail->yStart	= -2.0f;
		pTextTail->xEnd		= (float) (xSize / 2 + 2);
		pTextTail->yEnd		= (float) ySize;
	}
}

#ifdef ENABLE_SHOP_IN_CITIES
void CPythonTextTail::DeleteShopTextTail(DWORD VirtualID)
{
	TTextTailMap::iterator itor = m_ShopTextTailMap.find(VirtualID);

	if (m_ShopTextTailMap.end() == itor)
	{
		Tracef(" CPythonTextTail::DeleteShopTextTail - None Item Text Tail\n");
		return;
	}

	DeleteTextTail(itor->second);
	m_ShopTextTailMap.erase(itor);
}
#endif
void CPythonTextTail::DeleteCharacterTextTail(DWORD VirtualID)
{
	TTextTailMap::iterator itorCharacter = m_CharacterTextTailMap.find(VirtualID);
	TTextTailMap::iterator itorChat = m_ChatTailMap.find(VirtualID);

	if (m_CharacterTextTailMap.end() != itorCharacter)
	{
		DeleteTextTail(itorCharacter->second);
		m_CharacterTextTailMap.erase(itorCharacter);
	}
	else
	{
		Tracenf("CPythonTextTail::DeleteCharacterTextTail - Find VID[%d] Error", VirtualID);
	}

	if (m_ChatTailMap.end() != itorChat)
	{
		DeleteTextTail(itorChat->second);
		m_ChatTailMap.erase(itorChat);
	}
}

void CPythonTextTail::DeleteItemTextTail(DWORD VirtualID)
{
	TTextTailMap::iterator itor = m_ItemTextTailMap.find(VirtualID);

	if (m_ItemTextTailMap.end() == itor)
	{
		Tracef(" CPythonTextTail::DeleteItemTextTail - None Item Text Tail\n");
		return;
	}

	DeleteTextTail(itor->second);
	m_ItemTextTailMap.erase(itor);
}

CPythonTextTail::TTextTail * CPythonTextTail::RegisterTextTail(DWORD dwVirtualID, const char * c_szText, CGraphicObjectInstance * pOwner, float fHeight, const D3DXCOLOR & c_rColor)
{
	TTextTail * pTextTail = m_TextTailPool.Alloc();

#if defined(ENABLE_OFFLINE_SHOP) && defined(ENABLE_SHOP_IN_CITIES)
	pTextTail->bIsShop = false;
	pTextTail->bRender = false;
#endif
	pTextTail->dwVirtualID = dwVirtualID;
	pTextTail->pOwner = pOwner;
	pTextTail->pTextInstance = CGraphicTextInstance::New();
	pTextTail->pOwnerTextInstance = NULL;
	pTextTail->fHeight = fHeight;

	pTextTail->pTextInstance->SetTextPointer(ms_pFont);
	pTextTail->pTextInstance->SetHorizonalAlign(CGraphicTextInstance::HORIZONTAL_ALIGN_CENTER);
	pTextTail->pTextInstance->SetValue(c_szText);
	pTextTail->pTextInstance->SetColor(c_rColor.r, c_rColor.g, c_rColor.b);
	pTextTail->pTextInstance->Update();

	int xSize, ySize;
	pTextTail->pTextInstance->GetTextSize(&xSize, &ySize);
	pTextTail->xStart				= (float) (-xSize / 2 - 2);
	pTextTail->yStart				= -2.0f;
	pTextTail->xEnd					= (float) (xSize / 2 + 2);
	pTextTail->yEnd					= (float) ySize;
	pTextTail->Color				= c_rColor;
	pTextTail->fDistanceFromPlayer	= 0.0f;
	pTextTail->x = -100.0f;
	pTextTail->y = -100.0f;
	pTextTail->z = 0.0f;
	pTextTail->pMarkInstance = NULL;
	pTextTail->pGuildNameTextInstance = NULL;
	pTextTail->pTitleTextInstance = NULL;
	pTextTail->pLevelTextInstance = NULL;
#ifdef TITLE_SYSTEM_BYLUZER
	pTextTail->pGradeTextInstance = NULL;
#endif
#ifdef ENABLE_SECONDARY_LEVEL
	pTextTail->pSecondaryLevelTextInstance = NULL;
#endif
#ifdef ENABLE_ENLIGHTENMENT_SYSTEM
	pTextTail->pEnlightenmentLevelTextInstance = NULL;
#endif
#ifdef ENABLE_TITLE_ACHIEVEMENT_SYSTEM
	pTextTail->psTitleAchievementNameTextInstance = NULL;
	pTextTail->psTitleAchievementPremiumNameTextInstance = NULL;
#endif
	return pTextTail;
}

void CPythonTextTail::DeleteTextTail(TTextTail * pTextTail)
{
	if (pTextTail->pTextInstance)
	{
		CGraphicTextInstance::Delete(pTextTail->pTextInstance);
		pTextTail->pTextInstance = NULL;
	}
	if (pTextTail->pOwnerTextInstance)
	{
		CGraphicTextInstance::Delete(pTextTail->pOwnerTextInstance);
		pTextTail->pOwnerTextInstance = NULL;
	}
	if (pTextTail->pMarkInstance)
	{
		CGraphicMarkInstance::Delete(pTextTail->pMarkInstance);
		pTextTail->pMarkInstance = NULL;
	}
	if (pTextTail->pGuildNameTextInstance)
	{
		CGraphicTextInstance::Delete(pTextTail->pGuildNameTextInstance);
		pTextTail->pGuildNameTextInstance = NULL;
	}
	if (pTextTail->pTitleTextInstance)
	{
		CGraphicTextInstance::Delete(pTextTail->pTitleTextInstance);
		pTextTail->pTitleTextInstance = NULL;
	}
	if (pTextTail->pLevelTextInstance)
	{
		CGraphicTextInstance::Delete(pTextTail->pLevelTextInstance);
		pTextTail->pLevelTextInstance = NULL;
	}
	
#ifdef TITLE_SYSTEM_BYLUZER
	if (pTextTail->pGradeTextInstance)
	{
		CGraphicTextInstance::Delete(pTextTail->pGradeTextInstance);
		pTextTail->pGradeTextInstance = NULL;
	}
#endif
#ifdef ENABLE_SECONDARY_LEVEL
	if (pTextTail->pSecondaryLevelTextInstance)
	{
		CGraphicTextInstance::Delete(pTextTail->pSecondaryLevelTextInstance);
		pTextTail->pSecondaryLevelTextInstance = NULL;
	}
#endif
#ifdef ENABLE_ENLIGHTENMENT_SYSTEM
	if (pTextTail->pEnlightenmentLevelTextInstance)
	{
		CGraphicTextInstance::Delete(pTextTail->pEnlightenmentLevelTextInstance);
		pTextTail->pEnlightenmentLevelTextInstance = NULL;
	}
#endif
#ifdef ENABLE_TITLE_ACHIEVEMENT_SYSTEM
	if (pTextTail->psTitleAchievementNameTextInstance)
	{
		CGraphicTextInstance::Delete(pTextTail->psTitleAchievementNameTextInstance);
		pTextTail->psTitleAchievementNameTextInstance = NULL;
	}
	if (pTextTail->psTitleAchievementPremiumNameTextInstance)
	{
		CGraphicTextInstance::Delete(pTextTail->psTitleAchievementPremiumNameTextInstance);
		pTextTail->psTitleAchievementPremiumNameTextInstance = NULL;
	}
#endif
	m_TextTailPool.Free(pTextTail);
}

int CPythonTextTail::Pick(int ixMouse, int iyMouse)
{
	for (TTextTailMap::iterator itor = m_ItemTextTailMap.begin(); itor != m_ItemTextTailMap.end(); ++itor)
	{
		TTextTail * pTextTail = itor->second;

		if (ixMouse >= pTextTail->x + pTextTail->xStart && ixMouse <= pTextTail->x + pTextTail->xEnd &&
			iyMouse >= pTextTail->y + pTextTail->yStart && iyMouse <= pTextTail->y + pTextTail->yEnd)
		{
			SelectItemName(itor->first);
			return (itor->first);
		}
	}

	return -1;
}

void CPythonTextTail::SelectItemName(DWORD dwVirtualID)
{
	TTextTailMap::iterator itor = m_ItemTextTailMap.find(dwVirtualID);

	if (m_ItemTextTailMap.end() == itor)
		return;

	TTextTail * pTextTail = itor->second;
	pTextTail->pTextInstance->SetColor(0.1f, 0.9f, 0.1f);
}

void CPythonTextTail::AttachTitle(DWORD dwVID, const char * c_szName, const D3DXCOLOR & c_rColor)
{
	if (!bPKTitleEnable)
		return;

	TTextTailMap::iterator itor = m_CharacterTextTailMap.find(dwVID);
	if (m_CharacterTextTailMap.end() == itor)
		return;

	TTextTail * pTextTail = itor->second;

	CGraphicTextInstance *& prTitle = pTextTail->pTitleTextInstance;
	if (!prTitle)
	{
		prTitle = CGraphicTextInstance::New();
		prTitle->SetTextPointer(ms_pFont);
		prTitle->SetOutline(true);

		if (LocaleService_IsEUROPE())
			prTitle->SetHorizonalAlign(CGraphicTextInstance::HORIZONTAL_ALIGN_RIGHT);
		else
			prTitle->SetHorizonalAlign(CGraphicTextInstance::HORIZONTAL_ALIGN_CENTER);
		prTitle->SetVerticalAlign(CGraphicTextInstance::VERTICAL_ALIGN_BOTTOM);
	}

	prTitle->SetValue(c_szName);
	prTitle->SetColor(c_rColor.r, c_rColor.g, c_rColor.b);
	prTitle->Update();
}

void CPythonTextTail::DetachTitle(DWORD dwVID)
{
	if (!bPKTitleEnable)
		return;

	TTextTailMap::iterator itor = m_CharacterTextTailMap.find(dwVID);
	if (m_CharacterTextTailMap.end() == itor)
		return;

	TTextTail * pTextTail = itor->second;

	if (pTextTail->pTitleTextInstance)
	{
		CGraphicTextInstance::Delete(pTextTail->pTitleTextInstance);
		pTextTail->pTitleTextInstance = NULL;
	}
}

void CPythonTextTail::EnablePKTitle(BOOL bFlag)
{
	bPKTitleEnable = bFlag;
}

void CPythonTextTail::AttachLevel(DWORD dwVID, const char * c_szText, const D3DXCOLOR & c_rColor)
{
	if (!bPKTitleEnable)
		return;

	TTextTailMap::iterator itor = m_CharacterTextTailMap.find(dwVID);
	if (m_CharacterTextTailMap.end() == itor)
		return;

	TTextTail * pTextTail = itor->second;

	CGraphicTextInstance *& prLevel = pTextTail->pLevelTextInstance;
	if (!prLevel)
	{
		prLevel = CGraphicTextInstance::New();
		prLevel->SetTextPointer(ms_pFont);
		prLevel->SetOutline(true);

		prLevel->SetHorizonalAlign(CGraphicTextInstance::HORIZONTAL_ALIGN_RIGHT);
		prLevel->SetVerticalAlign(CGraphicTextInstance::VERTICAL_ALIGN_BOTTOM);
	}

	prLevel->SetValue(c_szText);
	prLevel->SetColor(c_rColor.r, c_rColor.g, c_rColor.b);
	prLevel->Update();
}

void CPythonTextTail::DetachLevel(DWORD dwVID)
{
	if (!bPKTitleEnable)
	{
		return;
	}

	TTextTailMap::iterator itor = m_CharacterTextTailMap.find(dwVID);
	if (m_CharacterTextTailMap.end() == itor)
	{
		return;
	}

	TTextTail * pTextTail = itor->second;

	if (pTextTail->pLevelTextInstance)
	{
		CGraphicTextInstance::Delete(pTextTail->pLevelTextInstance);
		pTextTail->pLevelTextInstance = NULL;
	}
}

#ifdef ENABLE_DYNAMIC_FONTS
	extern void GraphicTextInstancesReload(CGraphicText* pkDefaultFont);
#endif
void CPythonTextTail::Initialize()
{
	// DEFAULT_FONT
	//ms_pFont = (CGraphicText *)CResourceManager::Instance().GetTypeResourcePointer(g_strDefaultFontName.c_str());

	CGraphicText* pkDefaultFont = static_cast<CGraphicText*>(DefaultFont_GetResource());
	if (!pkDefaultFont)
	{
		TraceError("CPythonTextTail::Initialize - CANNOT_FIND_DEFAULT_FONT");
		return;
	}

	ms_pFont = pkDefaultFont;
	// END_OF_DEFAULT_FONT
#ifdef ENABLE_DYNAMIC_FONTS
	GraphicTextInstancesReload(ms_pFont);
#endif
}

#ifdef TITLE_SYSTEM_BYLUZER
void CPythonTextTail::AttachGrade(DWORD dwVID, const char * c_szText, const D3DXCOLOR & c_rColor)
{
	if (!bPKTitleEnable)
		return;
	TTextTailMap::iterator itor = m_CharacterTextTailMap.find(dwVID);
	if (m_CharacterTextTailMap.end() == itor)
		return;

	TTextTail * pTextTail = itor->second;

	CGraphicTextInstance *& prGrade = pTextTail->pGradeTextInstance;
	if (!prGrade)
	{
		prGrade = CGraphicTextInstance::New();
		prGrade->SetTextPointer(ms_pFont);
		prGrade->SetOutline(true);

		prGrade->SetHorizonalAlign(CGraphicTextInstance::HORIZONTAL_ALIGN_CENTER);
		prGrade->SetVerticalAlign(CGraphicTextInstance::VERTICAL_ALIGN_BOTTOM);
	}

	prGrade->SetValue(c_szText);
	prGrade->SetColor(c_rColor.r, c_rColor.g, c_rColor.b);
	prGrade->Update();
}

void CPythonTextTail::DetachGrade(DWORD dwVID)
{
	if (!bPKTitleEnable)
		return;

	TTextTailMap::iterator itor = m_CharacterTextTailMap.find(dwVID);
	if (m_CharacterTextTailMap.end() == itor)
		return;

	TTextTail * pTextTail = itor->second;

	if (pTextTail->pGradeTextInstance)
	{
		CGraphicTextInstance::Delete(pTextTail->pGradeTextInstance);
		pTextTail->pGradeTextInstance = NULL;
	}
}
#endif


void CPythonTextTail::Destroy()
{
	m_TextTailPool.Clear();
}

void CPythonTextTail::Clear()
{
	m_CharacterTextTailMap.clear();
	m_CharacterTextTailList.clear();
	m_ItemTextTailMap.clear();
	m_ItemTextTailList.clear();
	m_ChatTailMap.clear();
#ifdef ENABLE_PREMIUM_PRIVATE_SHOP_OFFICIAL
	m_PrivateShopTextTailMap.clear();
	m_PrivateShopTextTailList.clear();
#endif
	m_TextTailPool.Clear();
#ifdef ENABLE_SHOP_IN_CITIES
	m_ShopTextTailMap.clear();
#endif
}

CPythonTextTail::CPythonTextTail()
{
	Clear();
}

CPythonTextTail::~CPythonTextTail()
{
	Destroy();
}

#ifdef ENABLE_PREMIUM_PRIVATE_SHOP_OFFICIAL
void CPythonTextTail::ShowPrivateShopTextTail(DWORD dwVirtualID)
{
	auto it = m_PrivateShopTextTailMap.find(dwVirtualID);

	if (it == m_PrivateShopTextTailMap.end())
		return;

	TTextTail* pTextTail = it->second;

	if (m_PrivateShopTextTailList.end() != std::find(m_PrivateShopTextTailList.begin(), m_PrivateShopTextTailList.end(), pTextTail))
		return;

	m_PrivateShopTextTailList.push_back(pTextTail);
}

void CPythonTextTail::RegisterPrivateShopTextTail(DWORD dwVirtualID)
{
	CPythonPrivateShop::TPrivateShopInstance* pPrivateShopInstance = CPythonPrivateShop::Instance().GetPrivateShopInstance(dwVirtualID);

	if (!pPrivateShopInstance)
		return;

	const D3DXCOLOR& c_rColor = D3DXCOLOR(1.0f, 0.41f, 0.0f, 1.0f);

	TTextTail* pTextTail = RegisterTextTail(dwVirtualID,
		pPrivateShopInstance->GetName(),
		pPrivateShopInstance->GetGraphicThingInstancePtr(),
		pPrivateShopInstance->GetGraphicThingInstancePtr()->GetHeight(),
		c_rColor);

	pTextTail->pTextInstance->SetOutline(true);
	pTextTail->pTextInstance->Update();

	m_PrivateShopTextTailMap.emplace(dwVirtualID, pTextTail);
}

void CPythonTextTail::DeletePrivateShopTextTail(DWORD dwVirtualID)
{
	auto it = m_PrivateShopTextTailMap.find(dwVirtualID);

	if (it == m_PrivateShopTextTailMap.end())
	{
		Tracef(" CPythonTextTail::DeletePrivateShopTextTail - None Item Text Tail\n");
		return;
	}

	DeleteTextTail(it->second);
	m_PrivateShopTextTailMap.erase(it);
}
#endif

#ifdef ENABLE_SECONDARY_LEVEL
void CPythonTextTail::AttachSecondaryLevel(DWORD dwVID, const char* c_szText, const D3DXCOLOR& c_rColor)
{
	if (!bPKTitleEnable)
		return;

	TTextTailMap::iterator itor = m_CharacterTextTailMap.find(dwVID);
	if (m_CharacterTextTailMap.end() == itor)
		return;

	TTextTail* pTextTail = itor->second;

	CGraphicTextInstance*& prLevel = pTextTail->pSecondaryLevelTextInstance;
	if (!prLevel)
	{
		prLevel = CGraphicTextInstance::New();
		prLevel->SetTextPointer(ms_pFont);
		prLevel->SetOutline(true);

		prLevel->SetHorizonalAlign(CGraphicTextInstance::HORIZONTAL_ALIGN_RIGHT);
		prLevel->SetVerticalAlign(CGraphicTextInstance::VERTICAL_ALIGN_BOTTOM);
	}

	prLevel->SetValue(c_szText);
	prLevel->SetColor(c_rColor.r, c_rColor.g, c_rColor.b);
	prLevel->Update();
}

void CPythonTextTail::DetachSecondaryLevel(DWORD dwVID)
{
	if (!bPKTitleEnable)
		return;

	TTextTailMap::iterator itor = m_CharacterTextTailMap.find(dwVID);
	if (m_CharacterTextTailMap.end() == itor)
		return;

	TTextTail* pTextTail = itor->second;

	if (pTextTail->pSecondaryLevelTextInstance)
	{
		CGraphicTextInstance::Delete(pTextTail->pSecondaryLevelTextInstance);
		pTextTail->pSecondaryLevelTextInstance = NULL;
	}
}
#endif

#ifdef ENABLE_ENLIGHTENMENT_SYSTEM
void CPythonTextTail::AttachEnlightenmentLevel(DWORD dwVID, const char* c_szText, const D3DXCOLOR& c_rColor)
{
	if (!bPKTitleEnable)
		return;

	TTextTailMap::iterator itor = m_CharacterTextTailMap.find(dwVID);
	if (m_CharacterTextTailMap.end() == itor)
		return;

	TTextTail* pTextTail = itor->second;

	CGraphicTextInstance*& prLevel = pTextTail->pEnlightenmentLevelTextInstance;
	if (!prLevel)
	{
		prLevel = CGraphicTextInstance::New();
		prLevel->SetTextPointer(ms_pFont);
		prLevel->SetOutline(true);

		prLevel->SetHorizonalAlign(CGraphicTextInstance::HORIZONTAL_ALIGN_RIGHT);
		prLevel->SetVerticalAlign(CGraphicTextInstance::VERTICAL_ALIGN_BOTTOM);
	}

	prLevel->SetValue(c_szText);
	prLevel->SetColor(c_rColor.r, c_rColor.g, c_rColor.b);
	prLevel->Update();
}

void CPythonTextTail::DetachEnlightenmentLevel(DWORD dwVID)
{
	if (!bPKTitleEnable)
		return;

	TTextTailMap::iterator itor = m_CharacterTextTailMap.find(dwVID);
	if (m_CharacterTextTailMap.end() == itor)
		return;

	TTextTail* pTextTail = itor->second;

	if (pTextTail->pEnlightenmentLevelTextInstance)
	{
		CGraphicTextInstance::Delete(pTextTail->pEnlightenmentLevelTextInstance);
		pTextTail->pEnlightenmentLevelTextInstance = NULL;
	}
}
#endif

#ifdef ENABLE_TITLE_ACHIEVEMENT_SYSTEM
void CPythonTextTail::AttachTitleAchievement(DWORD dwVID, const char* c_szName, const D3DXCOLOR& c_rColor)
{
	if (!bPKTitleEnable)
		return;

	TTextTailMap::iterator itor = m_CharacterTextTailMap.find(dwVID);
	if (m_CharacterTextTailMap.end() == itor)
		return;

	TTextTail* pTextTail = itor->second;
	CGraphicTextInstance*& prTitleNameInstance = pTextTail->psTitleAchievementNameTextInstance;
	if (!prTitleNameInstance)
	{
		prTitleNameInstance = CGraphicTextInstance::New();
		prTitleNameInstance->SetTextPointer(ms_pFont);
		prTitleNameInstance->SetOutline(true);
		prTitleNameInstance->SetHorizonalAlign(CGraphicTextInstance::HORIZONTAL_ALIGN_CENTER);
		prTitleNameInstance->SetVerticalAlign(CGraphicTextInstance::VERTICAL_ALIGN_BOTTOM);
	}

	prTitleNameInstance->SetValue(c_szName);
	prTitleNameInstance->SetColor(c_rColor.r, c_rColor.g, c_rColor.b);
	prTitleNameInstance->Update();
}

void CPythonTextTail::DetachTitleAchievement(DWORD dwVID)
{
	if (!bPKTitleEnable)
		return;

	TTextTailMap::iterator itor = m_CharacterTextTailMap.find(dwVID);
	if (m_CharacterTextTailMap.end() == itor)
		return;

	TTextTail* pTextTail = itor->second;
	if (pTextTail->psTitleAchievementNameTextInstance)
	{
		CGraphicTextInstance::Delete(pTextTail->psTitleAchievementNameTextInstance);
		pTextTail->psTitleAchievementNameTextInstance = NULL;
	}
}

void CPythonTextTail::AttachTitleAchievementPremium(DWORD dwVID, const char* c_szName, const D3DXCOLOR& c_rColor)
{
	if (!bPKTitleEnable)
		return;

	TTextTailMap::iterator itor = m_CharacterTextTailMap.find(dwVID);
	if (m_CharacterTextTailMap.end() == itor)
		return;

	TTextTail* pTextTail = itor->second;
	CGraphicTextInstance*& prTitleNameInstance = pTextTail->psTitleAchievementPremiumNameTextInstance;

	if (!prTitleNameInstance)
	{
		prTitleNameInstance = CGraphicTextInstance::New();
		prTitleNameInstance->SetTextPointer(ms_pFont);
		prTitleNameInstance->SetOutline(true);
		prTitleNameInstance->SetHorizonalAlign(CGraphicTextInstance::HORIZONTAL_ALIGN_CENTER);
		prTitleNameInstance->SetVerticalAlign(CGraphicTextInstance::VERTICAL_ALIGN_BOTTOM);
	}

	prTitleNameInstance->SetValue(c_szName);
	prTitleNameInstance->SetColor(c_rColor.r, c_rColor.g, c_rColor.b);
	prTitleNameInstance->Update();
}

void CPythonTextTail::DetachTitleAchievementPremium(DWORD dwVID)
{
	if (!bPKTitleEnable)
		return;

	TTextTailMap::iterator itor = m_CharacterTextTailMap.find(dwVID);
	if (m_CharacterTextTailMap.end() == itor)
		return;

	TTextTail* pTextTail = itor->second;
	if (pTextTail->psTitleAchievementPremiumNameTextInstance)
	{
		CGraphicTextInstance::Delete(pTextTail->psTitleAchievementPremiumNameTextInstance);
		pTextTail->psTitleAchievementPremiumNameTextInstance = NULL;
	}
}
#endif
