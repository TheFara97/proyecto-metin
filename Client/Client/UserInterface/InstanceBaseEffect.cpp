#include "StdAfx.h"
#include "InstanceBase.h"
#include "PythonTextTail.h"
#include "AbstractApplication.h"
#include "AbstractPlayer.h"
#include "PythonPlayer.h"
#include "PythonSystem.h"
#include "PythonNonPlayer.h"

#include "../EffectLib/EffectManager.h"
#include "../EffectLib/ParticleSystemData.h"
#include "../EterLib/Camera.h"
#ifdef TITLE_SYSTEM_BYLUZER
#include "title_name.h"
#endif
float CInstanceBase::ms_fDustGap;
float CInstanceBase::ms_fHorseDustGap;
DWORD CInstanceBase::ms_adwCRCAffectEffect[CInstanceBase::EFFECT_NUM];
std::string CInstanceBase::ms_astAffectEffectAttachBone[EFFECT_NUM];

#define BYTE_COLOR_TO_D3DX_COLOR(r, g, b) D3DXCOLOR(float(r)/255.0f, float(g)/255.0f, float(b)/255.0f, 1.0f)

/*
D3DXCOLOR CInstanceBase::ms_kD3DXClrPC(0xFFFFD84D);//1.0f, 0.8470f, 0.3f, 1.0f
D3DXCOLOR CInstanceBase::ms_kD3DXClrNPC(0xFF7BE85E);//0.4823f, 0.9098f, 0.3686f, 1.0f
D3DXCOLOR CInstanceBase::ms_kD3DXClrMOB(0xFFEC170a);//0.9254f, 0.0901f, 0.0392f, 1.0f
D3DXCOLOR CInstanceBase::ms_kD3DXClrPVP(0xFF8532D9);
D3DXCOLOR CInstanceBase::ms_kD3DXClrPVPSelf(0xFFEE36DF);
D3DXCOLOR CInstanceBase::ms_kD3DXClrKiller = BYTE_COLOR_TO_D3DX_COLOR(180, 100, 0);
D3DXCOLOR CInstanceBase::ms_kD3DXClrTitle[CInstanceBase::TITLE_MAX_NUM] =
{
	BYTE_COLOR_TO_D3DX_COLOR(  0, 204, 255),
	BYTE_COLOR_TO_D3DX_COLOR(  0, 144, 255),
	BYTE_COLOR_TO_D3DX_COLOR( 92, 110, 255),
	BYTE_COLOR_TO_D3DX_COLOR(155, 155, 255),
	0xFFFFFFFF, // None
	BYTE_COLOR_TO_D3DX_COLOR(207, 117,   0),
	BYTE_COLOR_TO_D3DX_COLOR(235,  83,   0),
	BYTE_COLOR_TO_D3DX_COLOR(227,   0,   0),
	BYTE_COLOR_TO_D3DX_COLOR(255,   0,   0),
};
*/

D3DXCOLOR g_akD3DXClrTitle[CInstanceBase::TITLE_NUM];
D3DXCOLOR g_akD3DXClrName[CInstanceBase::NAMECOLOR_NUM];

std::map<int, std::string> g_TitleNameMap;
std::set<DWORD> g_kSet_dwPVPReadyKey;
std::set<DWORD> g_kSet_dwPVPKey;
std::set<DWORD> g_kSet_dwGVGKey;
std::set<DWORD> g_kSet_dwDUELKey;
#ifdef ENABLE_TITLE_ACHIEVEMENT_SYSTEM
std::map<BYTE, std::string> g_sTitleAchievementNameMap;
#endif

bool g_isEmpireNameMode=false;

void  CInstanceBase::SetEmpireNameMode(bool isEnable)
{
	g_isEmpireNameMode=isEnable;

	if (isEnable)
	{
		g_akD3DXClrName[NAMECOLOR_MOB]=g_akD3DXClrName[NAMECOLOR_EMPIRE_MOB];
		g_akD3DXClrName[NAMECOLOR_NPC]=g_akD3DXClrName[NAMECOLOR_EMPIRE_NPC];
		g_akD3DXClrName[NAMECOLOR_PC]=g_akD3DXClrName[NAMECOLOR_NORMAL_PC];

		for (UINT uEmpire=1; uEmpire<EMPIRE_NUM; ++uEmpire)
			g_akD3DXClrName[NAMECOLOR_PC+uEmpire]=g_akD3DXClrName[NAMECOLOR_EMPIRE_PC+uEmpire];

	}
	else
	{
		g_akD3DXClrName[NAMECOLOR_MOB]=g_akD3DXClrName[NAMECOLOR_NORMAL_MOB];
		g_akD3DXClrName[NAMECOLOR_NPC]=g_akD3DXClrName[NAMECOLOR_NORMAL_NPC];

		for (UINT uEmpire=0; uEmpire<EMPIRE_NUM; ++uEmpire)
			g_akD3DXClrName[NAMECOLOR_PC+uEmpire]=g_akD3DXClrName[NAMECOLOR_NORMAL_PC];
	}
}

const D3DXCOLOR& CInstanceBase::GetIndexedNameColor(UINT eNameColor)
{
	if (eNameColor>=NAMECOLOR_NUM)
	{
		static D3DXCOLOR s_kD3DXClrNameDefault(0xffffffff);
		return s_kD3DXClrNameDefault;
	}

	return g_akD3DXClrName[eNameColor];
}

void CInstanceBase::AddDamageEffect(long long damage, BYTE flag, BOOL bSelf, BOOL bTarget)
{
	if (CPythonSystem::Instance().IsShowDamage())
	{
		SEffectDamage sDamage;
		sDamage.bSelf = bSelf;
		sDamage.bTarget = bTarget;
		sDamage.damage = damage;
		sDamage.flag = flag;
		m_DamageQueue.push(sDamage);
		if (m_DamageQueue.size() > 10) // @fixme025
			m_DamageQueue.pop();
	}
}

void CInstanceBase::ProcessDamage()
{
	if (m_DamageQueue.empty())
		return;

	SEffectDamage sDamage = m_DamageQueue.front();
	m_DamageQueue.pop();

	long long damage = sDamage.damage;
	BYTE flag = sDamage.flag;
	BOOL bSelf = sDamage.bSelf;
	BOOL bTarget = sDamage.bTarget;

	CCamera* pCamera = CCameraManager::Instance().GetCurrentCamera();
	float cameraAngle = GetDegreeFromPosition2(pCamera->GetTarget().x, pCamera->GetTarget().y, pCamera->GetEye().x, pCamera->GetEye().y);

	DWORD FONT_WIDTH = 30;

	CEffectManager& rkEftMgr = CEffectManager::Instance();

	D3DXVECTOR3 v3Pos = m_GraphicThingInstance.GetPosition();
	v3Pos.z += float(m_GraphicThingInstance.GetHeight());

	D3DXVECTOR3 v3Rot = D3DXVECTOR3(0.0f, 0.0f, cameraAngle);

	if ((flag & DAMAGE_DODGE) || (flag & DAMAGE_BLOCK))
	{
		if (bSelf)
			rkEftMgr.CreateEffect(ms_adwCRCAffectEffect[EFFECT_DAMAGE_MISS], v3Pos, v3Rot);
		else
			rkEftMgr.CreateEffect(ms_adwCRCAffectEffect[EFFECT_DAMAGE_TARGETMISS], v3Pos, v3Rot);
		//__AttachEffect(EFFECT_DAMAGE_MISS);
		return;
	}
	else if (flag & DAMAGE_CRITICAL)
	{
		//rkEftMgr.CreateEffect(ms_adwCRCAffectEffect[EFFECT_DAMAGE_CRITICAL],v3Pos,v3Rot);
	}

	string strDamageType;
	DWORD rdwCRCEft = 0;
	
#ifdef ENABLE_EFFECT_DAMAGE_POISON
	if ( (flag & DAMAGE_POISON) )
	{
		strDamageType = "poison_";
		rdwCRCEft = EFFECT_DAMAGE_POISON;
	}
#endif
#ifdef ENABLE_EFFECT_DAMAGE_FIRE	
	else if ( (flag & DAMAGE_FIRE) )
	{
		strDamageType = "burn_";
		rdwCRCEft = EFFECT_DAMAGE_FIRE;
	}
#endif	
#ifdef ENABLE_EFFECT_DAMAGE_BLEEDING	
	else if ( (flag & DAMAGE_BLEEDING) )
	{
		strDamageType = "bleed_";
		rdwCRCEft = EFFECT_DAMAGE_BLEEDING;
	}
#endif	
#if defined(ENABLE_EFFECT_DAMAGE_POISON) || defined(ENABLE_EFFECT_DAMAGE_BLEEDING) || defined(ENABLE_EFFECT_DAMAGE_FIRE)
	else
	{
#endif
		if (bSelf)
		{
			if (m_bDamageEffectType == 0)
				rdwCRCEft = EFFECT_DAMAGE_SELFDAMAGE;
			else
				rdwCRCEft = EFFECT_DAMAGE_SELFDAMAGE2;
	
#ifdef ENABLE_EFFECT_DAMAGE_CRITICAL_AND_PENETRATE	
			if (IS_SET(flag, DAMAGE_CRITICAL) && IS_SET(flag, DAMAGE_PENETRATE))
				strDamageType = "damage_crit_pen_";
#endif
#ifdef ENABLE_EFFECT_DAMAGE_CRITICAL
			else if (IS_SET(flag, DAMAGE_CRITICAL))
				strDamageType = "damage_crit_";
#endif
#ifdef ENABLE_EFFECT_DAMAGE_PENETRATE
			else if (IS_SET(flag, DAMAGE_PENETRATE))
				strDamageType = "damage_pen_";
#endif
#if defined(ENABLE_EFFECT_DAMAGE_CRITICAL_AND_PENETRATE) || defined(ENABLE_EFFECT_DAMAGE_CRITICAL) || defined(ENABLE_EFFECT_DAMAGE_PENETRATE)
			else
				strDamageType = "damage_";
#else
			strDamageType = "damage_";
#endif	
			m_bDamageEffectType = !m_bDamageEffectType;
		}
		else if (bTarget == false)
		{
			strDamageType = "nontarget_";
			rdwCRCEft = EFFECT_DAMAGE_NOT_TARGET;
			return;
		}
		else
		{
			rdwCRCEft = EFFECT_DAMAGE_TARGET;
#ifdef ENABLE_EFFECT_TARGET_CRITICAL_AND_PENETRATE
			if (IS_SET(flag, DAMAGE_CRITICAL) && IS_SET(flag, DAMAGE_PENETRATE))
				strDamageType = "target_crit_pen_";
#endif			
#ifdef ENABLE_EFFECT_TARGET_CRITICAL			
			else if (IS_SET(flag, DAMAGE_CRITICAL))
				strDamageType = "target_crit_";
#endif
#ifdef ENABLE_EFFECT_TARGET_PENETRATE			
			else if (IS_SET(flag, DAMAGE_PENETRATE))
				strDamageType = "target_pen_";
#endif
#if defined(ENABLE_EFFECT_TARGET_CRITICAL_AND_PENETRATE) || defined(ENABLE_EFFECT_TARGET_CRITICAL) || defined(ENABLE_EFFECT_TARGET_PENETRATE)			
			else
				strDamageType = "target_";
#else
			strDamageType = "target_";
#endif	
		}
#if defined(ENABLE_EFFECT_DAMAGE_POISON) || defined(ENABLE_EFFECT_DAMAGE_BLEEDING) || defined(ENABLE_EFFECT_DAMAGE_FIRE)
	}
#endif

	long long index = 0;
	long long num = 0;
	std::vector<string> textures;
#ifdef DOTTED_DMG
	BYTE idxMultiplier = 0;
#endif
	while (damage > 0)
	{
		if (index > 10)
		{
			TraceError("ProcessDamage Possibility of endless loop");
			break;
		}
		num = damage % 10;
		damage /= 10;
		char numBuf[MAX_PATH];
		sprintf(numBuf, "%d.dds", num);
		textures.push_back("d:/ymir work/effect/affect/damagevalue/" + strDamageType + numBuf);

		rkEftMgr.SetEffectTextures(ms_adwCRCAffectEffect[rdwCRCEft], textures);

		D3DXMATRIX matrix, matTrans;
		D3DXMatrixIdentity(&matrix);
		matrix._41 = v3Pos.x;
		matrix._42 = v3Pos.y;
		matrix._43 = v3Pos.z;
		D3DXMatrixTranslation(&matrix, v3Pos.x, v3Pos.y, v3Pos.z);
		D3DXMatrixMultiply(&matrix, &pCamera->GetInverseViewMatrix(), &matrix);
#ifdef DOTTED_DMG
		D3DXMatrixTranslation(&matTrans, FONT_WIDTH * idxMultiplier, 0, 0);
#else
		D3DXMatrixTranslation(&matTrans, FONT_WIDTH * index, 0, 0);
#endif
		matTrans._41 = -matTrans._41;
		matrix = matTrans * matrix;
		D3DXMatrixMultiply(&matrix, &pCamera->GetViewMatrix(), &matrix);

		rkEftMgr.CreateEffect(ms_adwCRCAffectEffect[rdwCRCEft], D3DXVECTOR3(matrix._41, matrix._42, matrix._43)
			, v3Rot);

		textures.clear();

		index++;
#ifdef DOTTED_DMG
		idxMultiplier++;
		if (damage > 0 && (index % 3) == 0)
		{
			textures.emplace_back("d:/ymir work/effect/affect/damagevalue/" + strDamageType + "dot.dds");
			rkEftMgr.SetEffectTextures(ms_adwCRCAffectEffect[rdwCRCEft], textures);
			D3DXMATRIX matrix, matTrans;
			D3DXMatrixIdentity(&matrix);
			matrix._41 = v3Pos.x;
			matrix._42 = v3Pos.y;
			matrix._43 = v3Pos.z;
			D3DXMatrixTranslation(&matrix, v3Pos.x, v3Pos.y, v3Pos.z);
			D3DXMatrixMultiply(&matrix, &pCamera->GetInverseViewMatrix(), &matrix);
			D3DXMatrixTranslation(&matTrans, FONT_WIDTH * idxMultiplier, 0, 0);
			matTrans._41 = -matTrans._41;
			matrix = matTrans * matrix;
			D3DXMatrixMultiply(&matrix, &pCamera->GetViewMatrix(), &matrix);
			rkEftMgr.CreateEffect(ms_adwCRCAffectEffect[rdwCRCEft], D3DXVECTOR3(matrix._41, matrix._42, matrix._43), v3Rot);
			textures.clear();
			idxMultiplier++;
		}
#endif
	}
}

void CInstanceBase::AttachSpecialEffect(DWORD effect)
{
	__AttachEffect(effect);
}

void CInstanceBase::LevelUp()
{
	__AttachEffect(EFFECT_LEVELUP);
}

void CInstanceBase::SkillUp()
{
	__AttachEffect(EFFECT_SKILLUP);
}

void CInstanceBase::CreateSpecialEffect(DWORD iEffectIndex)
{
	const D3DXMATRIX & c_rmatGlobal = m_GraphicThingInstance.GetTransform();

	DWORD dwEffectIndex = CEffectManager::Instance().GetEmptyIndex();
	DWORD dwEffectCRC = ms_adwCRCAffectEffect[iEffectIndex];
	CEffectManager::Instance().CreateEffectInstance(dwEffectIndex, dwEffectCRC);
	CEffectManager::Instance().SelectEffectInstance(dwEffectIndex);
	CEffectManager::Instance().SetEffectInstanceGlobalMatrix(c_rmatGlobal);
}

void CInstanceBase::__EffectContainer_Destroy()
{
	SEffectContainer::Dict& rkDctEftID=__EffectContainer_GetDict();

	SEffectContainer::Dict::iterator i;
	for (i=rkDctEftID.begin(); i!=rkDctEftID.end(); ++i)
		__DetachEffect(i->second);

	rkDctEftID.clear();
}

void CInstanceBase::__EffectContainer_Initialize()
{
	SEffectContainer::Dict& rkDctEftID=__EffectContainer_GetDict();
	rkDctEftID.clear();
}

CInstanceBase::SEffectContainer::Dict& CInstanceBase::__EffectContainer_GetDict()
{
	return m_kEffectContainer.m_kDct_dwEftID;
}

DWORD CInstanceBase::__EffectContainer_AttachEffect(DWORD dwEftKey)
{
	SEffectContainer::Dict& rkDctEftID=__EffectContainer_GetDict();
	SEffectContainer::Dict::iterator f=rkDctEftID.find(dwEftKey);
	if (rkDctEftID.end()!=f)
		return 0;

	DWORD dwEftID=__AttachEffect(dwEftKey);
	rkDctEftID.insert(SEffectContainer::Dict::value_type(dwEftKey, dwEftID));
	return dwEftID;
}

void CInstanceBase::__EffectContainer_DetachEffect(DWORD dwEftKey)
{
	SEffectContainer::Dict& rkDctEftID=__EffectContainer_GetDict();
	SEffectContainer::Dict::iterator f=rkDctEftID.find(dwEftKey);
	if (rkDctEftID.end()==f)
		return;

	__DetachEffect(f->second);

	rkDctEftID.erase(f);
}

void CInstanceBase::__AttachEmpireEffect(DWORD eEmpire)
{
	if (!__IsExistMainInstance())
		return;

	CInstanceBase* pkInstMain=__GetMainInstancePtr();

	if (IsWarp())
		return;
	if (IsObject())
		return;
	if (IsFlag())
		return;
	if (IsResource())
		return;

#ifdef HIDE_EMPIRE_FLAG
	if (GetRace() == 30000)
		return;
	
	if (GetRace() > 53001 < 53026 || 53900)
		return;
#endif

#ifdef ENABLE_ASLAN_BUFF_NPC_SYSTEM
	if (IsBuffNPC(GetVirtualID()))
		return;
#endif

	if (pkInstMain->IsGameMaster())
	{
	}
	else
	{
		if (pkInstMain->IsSameEmpire(*this))
			return;

		// HIDE_OTHER_EMPIRE_EUNHYEONG_ASSASSIN
		if (IsAffect(AFFECT_EUNHYEONG))
			return;
		// END_OF_HIDE_OTHER_EMPIRE_EUNHYEONG_ASSASSIN
	}

	if (IsGameMaster())
		return;

	__EffectContainer_AttachEffect(EFFECT_EMPIRE+eEmpire);
}

void CInstanceBase::__AttachSelectEffect()
{
	__EffectContainer_AttachEffect(EFFECT_SELECT);
}

void CInstanceBase::__DetachSelectEffect()
{
	__EffectContainer_DetachEffect(EFFECT_SELECT);
}

void CInstanceBase::__AttachTargetEffect()
{
	__EffectContainer_AttachEffect(EFFECT_TARGET);
}

void CInstanceBase::__DetachTargetEffect()
{
	__EffectContainer_DetachEffect(EFFECT_TARGET);
}

void CInstanceBase::__StoneSmoke_Inialize()
{
	m_kStoneSmoke.m_dwEftID=0;
}

void CInstanceBase::__StoneSmoke_Destroy()
{
	if (!m_kStoneSmoke.m_dwEftID)
		return;

	__DetachEffect(m_kStoneSmoke.m_dwEftID);
	m_kStoneSmoke.m_dwEftID=0;
}

void CInstanceBase::__StoneSmoke_Create(DWORD eSmoke)
{
	m_kStoneSmoke.m_dwEftID=m_GraphicThingInstance.AttachSmokeEffect(eSmoke);
}

void CInstanceBase::SetAlpha(float fAlpha)
{
	__SetBlendRenderingMode();
	__SetAlphaValue(fAlpha);
}

bool CInstanceBase::UpdateDeleting()
{
	Update();
	Transform();

	IAbstractApplication& rApp=IAbstractApplication::GetSingleton();

	float fAlpha = __GetAlphaValue() - (rApp.GetGlobalElapsedTime() * 1.5f);
	__SetAlphaValue(fAlpha);

	if (fAlpha < 0.0f)
		return false;

	return true;
}

void CInstanceBase::DeleteBlendOut()
{
	__SetBlendRenderingMode();
	__SetAlphaValue(1.0f);
	DetachTextTail();

	IAbstractPlayer& rkPlayer=IAbstractPlayer::GetSingleton();
	rkPlayer.NotifyDeletingCharacterInstance(GetVirtualID());
}

void CInstanceBase::ClearPVPKeySystem()
{
	g_kSet_dwPVPReadyKey.clear();
	g_kSet_dwPVPKey.clear();
	g_kSet_dwGVGKey.clear();
	g_kSet_dwDUELKey.clear();
}

void CInstanceBase::InsertPVPKey(DWORD dwVIDSrc, DWORD dwVIDDst)
{
	DWORD dwPVPKey=__GetPVPKey(dwVIDSrc, dwVIDDst);

	g_kSet_dwPVPKey.insert(dwPVPKey);
}

void CInstanceBase::InsertPVPReadyKey(DWORD dwVIDSrc, DWORD dwVIDDst)
{
	DWORD dwPVPReadyKey=__GetPVPKey(dwVIDSrc, dwVIDDst);

	g_kSet_dwPVPKey.insert(dwPVPReadyKey);
}

void CInstanceBase::RemovePVPKey(DWORD dwVIDSrc, DWORD dwVIDDst)
{
	DWORD dwPVPKey=__GetPVPKey(dwVIDSrc, dwVIDDst);

	g_kSet_dwPVPKey.erase(dwPVPKey);
}

void CInstanceBase::InsertGVGKey(DWORD dwSrcGuildVID, DWORD dwDstGuildVID)
{
	DWORD dwGVGKey = __GetPVPKey(dwSrcGuildVID, dwDstGuildVID);
	g_kSet_dwGVGKey.insert(dwGVGKey);
}

void CInstanceBase::RemoveGVGKey(DWORD dwSrcGuildVID, DWORD dwDstGuildVID)
{
	DWORD dwGVGKey = __GetPVPKey(dwSrcGuildVID, dwDstGuildVID);
	g_kSet_dwGVGKey.erase(dwGVGKey);
}

void CInstanceBase::InsertDUELKey(DWORD dwVIDSrc, DWORD dwVIDDst)
{
	DWORD dwPVPKey=__GetPVPKey(dwVIDSrc, dwVIDDst);

	g_kSet_dwDUELKey.insert(dwPVPKey);
}

DWORD CInstanceBase::__GetPVPKey(DWORD dwVIDSrc, DWORD dwVIDDst)
{
	if (dwVIDSrc>dwVIDDst)
		std::swap(dwVIDSrc, dwVIDDst);

	DWORD awSrc[2];
	awSrc[0]=dwVIDSrc;
	awSrc[1]=dwVIDDst;

    const BYTE * s = (const BYTE *) awSrc;
    const BYTE * end = s + sizeof(awSrc);
    unsigned long h = 0;

    while (s < end)
    {
        h *= 16777619;
        h ^= (BYTE) *(BYTE *) (s++);
    }

    return h;
}

bool CInstanceBase::__FindPVPKey(DWORD dwVIDSrc, DWORD dwVIDDst)
{
	DWORD dwPVPKey=__GetPVPKey(dwVIDSrc, dwVIDDst);

	if (g_kSet_dwPVPKey.end()==g_kSet_dwPVPKey.find(dwPVPKey))
		return false;

	return true;
}

bool CInstanceBase::__FindPVPReadyKey(DWORD dwVIDSrc, DWORD dwVIDDst)
{
	DWORD dwPVPKey=__GetPVPKey(dwVIDSrc, dwVIDDst);

	if (g_kSet_dwPVPReadyKey.end()==g_kSet_dwPVPReadyKey.find(dwPVPKey))
		return false;

	return true;
}
bool CInstanceBase::__FindGVGKey(DWORD dwSrcGuildID, DWORD dwDstGuildID)
{
	DWORD dwGVGKey=__GetPVPKey(dwSrcGuildID, dwDstGuildID);

	if (g_kSet_dwGVGKey.end()==g_kSet_dwGVGKey.find(dwGVGKey))
		return false;

	return true;
}
bool CInstanceBase::__FindDUELKey(DWORD dwVIDSrc, DWORD dwVIDDst)
{
	DWORD dwDUELKey=__GetPVPKey(dwVIDSrc, dwVIDDst);

	if (g_kSet_dwDUELKey.end()==g_kSet_dwDUELKey.find(dwDUELKey))
		return false;

	return true;
}

bool CInstanceBase::IsPVPInstance(CInstanceBase& rkInstSel)
{
	DWORD dwVIDSrc=GetVirtualID();
	DWORD dwVIDDst=rkInstSel.GetVirtualID();

	DWORD dwGuildIDSrc=GetGuildID();
	DWORD dwGuildIDDst=rkInstSel.GetGuildID();

	if (GetDuelMode())
		return true;

	return __FindPVPKey(dwVIDSrc, dwVIDDst) || __FindGVGKey(dwGuildIDSrc, dwGuildIDDst);
											//__FindDUELKey(dwVIDSrc, dwVIDDst);
}

const D3DXCOLOR& CInstanceBase::GetNameColor()
{
	return GetIndexedNameColor(GetNameColorIndex());
}

UINT CInstanceBase::GetNameColorIndex()
{
	if (IsPC())
	{
		if (m_isKiller)
		{
			return NAMECOLOR_PK;
		}

#ifdef ENABLE_ASLAN_BUFF_NPC_SYSTEM
		if (IsBuffNPC(GetVirtualID()))
			return NAMECOLOR_NPC;
#endif

		if (__IsExistMainInstance() && !__IsMainInstance())
		{
			CInstanceBase* pkInstMain=__GetMainInstancePtr();
			if (!pkInstMain)
			{
				TraceError("CInstanceBase::GetNameColorIndex - MainInstance is NULL");
				return NAMECOLOR_PC;
			}
			DWORD dwVIDMain=pkInstMain->GetVirtualID();
			DWORD dwVIDSelf=GetVirtualID();
#ifdef ENABLE_PVP_RANKING
			if (pkInstMain->GetPKMode() == PK_MODE_BATTLE)
			{
				return NAMECOLOR_PVP;
			}
#endif

			if (pkInstMain->GetDuelMode())
			{
				switch(pkInstMain->GetDuelMode())
				{
				case DUEL_CANNOTATTACK:
					return NAMECOLOR_PC + GetEmpireID();
				case DUEL_START:
					if(__FindDUELKey(dwVIDMain, dwVIDSelf))
						return NAMECOLOR_PVP;
					else
						return NAMECOLOR_PC + GetEmpireID();
				}
			}

			if (pkInstMain->IsSameEmpire(*this))
			{
				if (__FindPVPKey(dwVIDMain, dwVIDSelf))
				{
					return NAMECOLOR_PVP;
				}

				DWORD dwGuildIDMain=pkInstMain->GetGuildID();
				DWORD dwGuildIDSelf=GetGuildID();
				if (__FindGVGKey(dwGuildIDMain, dwGuildIDSelf))
				{
					return NAMECOLOR_PVP;
				}
				/*
				if (__FindDUELKey(dwVIDMain, dwVIDSelf))
				{
					return NAMECOLOR_PVP;
				}
				*/
			}
			else
			{
				return NAMECOLOR_PVP;
			}
		}

		IAbstractPlayer& rPlayer=IAbstractPlayer::GetSingleton();
		if (rPlayer.IsPartyMemberByVID(GetVirtualID()))
			return NAMECOLOR_PARTY;

		return NAMECOLOR_PC + GetEmpireID();

	}
	else if (IsNPC())
	{
#ifdef ENABLE_OFFLINE_SHOP_SYSTEM
		if (GetRace() == 30000)
			return NAMECOLOR_NPC;
			// return NAMECOLOR_OFFLINE_SHOP;
		else
			return NAMECOLOR_NPC;
#else
		return NAMECOLOR_NPC;
#endif
	}
	else if (IsEnemy())
	{
		return NAMECOLOR_MOB;
	}
	else if (IsPoly())
	{
		return NAMECOLOR_MOB;
	}

	return D3DXCOLOR(0xffffffff);
}

const D3DXCOLOR& CInstanceBase::GetTitleColor()
{
	UINT uGrade = GetAlignmentGrade();
	if ( uGrade >= TITLE_NUM)
	{
		static D3DXCOLOR s_kD3DXClrTitleDefault(0xffffffff);
		return s_kD3DXClrTitleDefault;
	}

	return g_akD3DXClrTitle[uGrade];
}

void CInstanceBase::AttachTextTail()
{
	if (m_isTextTail)
	{
		TraceError("CInstanceBase::AttachTextTail - VID [%d] ALREADY EXIST", GetVirtualID());
		return;
	}

#ifdef ENABLE_RENDER_TARGET_EFFECT
	if (m_GraphicThingInstance.IsRenderTarget())
		return;
#endif

	m_isTextTail=true;

	DWORD dwVID=GetVirtualID();

	float fTextTailHeight=IsMountingHorse() ? 110.0f : 10.0f;

	static D3DXCOLOR s_kD3DXClrTextTail=D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

#ifdef STONE_SCALE
	if (IsStone())
	{
		const float fStoneScale = CPythonSystem::Instance().GetStoneScale() * 100;

		if (CPythonSystem::Instance().GetStoneScale() <= 0.70)
			CPythonTextTail::Instance().RegisterCharacterTextTail(m_dwGuildID, dwVID, s_kD3DXClrTextTail, fTextTailHeight - 75.0f);
		else if (CPythonSystem::Instance().GetStoneScale() == 1.0)
			CPythonTextTail::Instance().RegisterCharacterTextTail(m_dwGuildID, dwVID, s_kD3DXClrTextTail, fTextTailHeight);
		else if (CPythonSystem::Instance().GetStoneScale() > 1.45)
			CPythonTextTail::Instance().RegisterCharacterTextTail(m_dwGuildID, dwVID, s_kD3DXClrTextTail, fTextTailHeight + (fStoneScale - 50));
		else
			CPythonTextTail::Instance().RegisterCharacterTextTail(m_dwGuildID, dwVID, s_kD3DXClrTextTail, fTextTailHeight);

		if (m_dwLevel)
			UpdateTextTailLevel(m_dwLevel);

		return;
	}
#endif

	CPythonTextTail::Instance().RegisterCharacterTextTail(m_dwGuildID, dwVID, s_kD3DXClrTextTail, fTextTailHeight);
#ifdef TITLE_SYSTEM_BYLUZER
 	if (m_title_active != 99 && IsPC())
 		UpdateTextTailGrade(m_title_active);
#endif
	// CHARACTER_LEVEL
	if (m_dwLevel)
	{
		UpdateTextTailLevel(m_dwLevel);
	}
	
#ifdef ENABLE_SECONDARY_LEVEL
	if (m_dwSecondaryLevel)
	{
		UpdateTextTailSecondaryLevel(m_dwSecondaryLevel);
	}
#endif
	
#ifdef ENABLE_ENLIGHTENMENT_SYSTEM
	if (m_dwEnlightLevel)
	{
		UpdateTextTailEnlightenmentLevel(m_dwEnlightLevel);
	}
#endif
}

void CInstanceBase::DetachTextTail()
{
	if (!m_isTextTail)
		return;

	m_isTextTail=false;
	CPythonTextTail::Instance().DeleteCharacterTextTail(GetVirtualID());
}

void CInstanceBase::UpdateTextTailLevel(DWORD level)
{
#ifdef ENABLE_RENDER_TARGET_EFFECT
	if (m_GraphicThingInstance.IsRenderTarget())
		return;
#endif
#ifdef ENABLE_NEW_PET_SYSTEM
	if (!level)
	{
		return;
	}
#endif
	//static D3DXCOLOR s_kLevelColor = D3DXCOLOR(119.0f/255.0f, 246.0f/255.0f, 168.0f/255.0f, 1.0f);
	static D3DXCOLOR s_kLevelColor = D3DXCOLOR(152.0f/255.0f, 255.0f/255.0f, 51.0f/255.0f, 1.0f);

	char szText[256];
	if (!IsBuffNPC(GetVirtualID())) {
		sprintf(szText, "Lv %d ", level);
		CPythonTextTail::Instance().AttachLevel(GetVirtualID(), szText, s_kLevelColor);
	}
}

#ifdef ENABLE_ENLIGHTENMENT_SYSTEM
void CInstanceBase::UpdateTextTailEnlightenmentLevel(DWORD level)
{
	//static D3DXCOLOR s_kLevelColor = D3DXCOLOR(119.0f/255.0f, 246.0f/255.0f, 168.0f/255.0f, 1.0f);
	static D3DXCOLOR s_kLevelColor = D3DXCOLOR(102.0f / 255.0f, 178.0f / 255.0f, 255.0f / 255.0f, 1.0f);

	char szText[256];
	sprintf(szText, "[%d] ", level);
	CPythonTextTail::Instance().AttachEnlightenmentLevel(GetVirtualID(), szText, s_kLevelColor);
}
#endif

#ifdef ENABLE_SECONDARY_LEVEL
void CInstanceBase::UpdateTextTailSecondaryLevel(DWORD level)
{
	//static D3DXCOLOR s_kLevelColor = D3DXCOLOR(119.0f/255.0f, 246.0f/255.0f, 168.0f/255.0f, 1.0f);
	static D3DXCOLOR s_kLevelColor = D3DXCOLOR(102.0f / 255.0f, 178.0f / 255.0f, 255.0f / 255.0f, 1.0f);

	char szText[256];
	sprintf(szText, "|Esecondary_level/%d|e", level);
	CPythonTextTail::Instance().AttachSecondaryLevel(GetVirtualID(), szText, s_kLevelColor);
}
#endif

void CInstanceBase::RefreshTextTail()
{
#ifdef ENABLE_RENDER_TARGET_EFFECT
	if (m_GraphicThingInstance.IsRenderTarget())
		return;
#endif
#ifdef ENABLE_ASLAN_BUFF_NPC_SYSTEM
	if (IsBuffNPC(GetVirtualID())) {
		CPythonTextTail::Instance().SetCharacterTextTailColor(GetVirtualID(), GetIndexedNameColor(NAMECOLOR_PC));
	}
	else
		CPythonTextTail::Instance().SetCharacterTextTailColor(GetVirtualID(), GetNameColor());
#else
	CPythonTextTail::Instance().SetCharacterTextTailColor(GetVirtualID(), GetNameColor());
#endif

	int iAlignmentGrade = GetAlignmentGrade();
	if (TITLE_NONE == iAlignmentGrade)
		CPythonTextTail::Instance().DetachTitle(GetVirtualID());
	else
	{
		auto itor = g_TitleNameMap.find(iAlignmentGrade);
		if (g_TitleNameMap.end() != itor)
		{
			const std::string & c_rstrTitleName = itor->second;
			CPythonTextTail::Instance().AttachTitle(GetVirtualID(), c_rstrTitleName.c_str(), GetTitleColor());
		}
	}
	

#ifdef ENABLE_TITLE_ACHIEVEMENT_SYSTEM
	if (IsPC())
	{
		// Primary title
		const BYTE TitleAchievement = GetTitleAchievement();
		if (TitleAchievement == 0)
		{
			CPythonTextTail::Instance().DetachTitleAchievement(GetVirtualID());
		}
		else
		{
			const auto& itor = g_sTitleAchievementNameMap.find(TitleAchievement);
			if (g_sTitleAchievementNameMap.end() != itor)
			{
				const std::string& c_rstrTitleName = itor->second;
				CPythonTextTail::Instance().AttachTitleAchievement(
					GetVirtualID(),
					c_rstrTitleName.c_str(),
					GetTitleAchievementColor(TitleAchievement)
				);
			}
		}

		const BYTE TitleAchievementPremium = GetTitleAchievementPremium();
		if (TitleAchievementPremium == 0)
		{
			CPythonTextTail::Instance().DetachTitleAchievementPremium(GetVirtualID());
		}
		else
		{
			const auto& itor = g_sTitleAchievementNameMap.find(TitleAchievementPremium);
			if (g_sTitleAchievementNameMap.end() != itor)
			{
				const std::string& c_rstrTitleName = itor->second;
				CPythonTextTail::Instance().AttachTitleAchievementPremium(
					GetVirtualID(),
					c_rstrTitleName.c_str(),
					GetTitleAchievementColor(TitleAchievementPremium)
				);
			}
		}
	}
#endif
}

D3DXCOLOR CInstanceBase::GetTitleAchievementColor(BYTE bTitleIndex)
{
	// Map matching your Python colors
	static const std::map<BYTE, D3DXCOLOR> colorMap = {
		{1,  D3DCOLOR_XRGB(101, 240, 138)},
		{2,  D3DCOLOR_XRGB(110, 245, 146)},
		{3,  D3DCOLOR_XRGB(120, 255, 156)},
		{4,  D3DCOLOR_XRGB(240, 101, 101)},
		{5,  D3DCOLOR_XRGB(242, 104, 104)},
		{6,  D3DCOLOR_XRGB(173, 43, 43)},
		{7,  D3DCOLOR_XRGB(84, 247, 220)},
		{8,  D3DCOLOR_XRGB(232, 237, 142)},
		{9,  D3DCOLOR_XRGB(245, 250, 155)},
		{10, D3DCOLOR_XRGB(203, 126, 247)},
		{11, D3DCOLOR_XRGB(43, 207, 57)},
		{12, D3DCOLOR_XRGB(99, 129, 247)},
		{13, D3DCOLOR_XRGB(140, 142, 143)},
		{14, D3DCOLOR_XRGB(158, 162, 163)},
		{15, D3DCOLOR_XRGB(182, 185, 186)},
		{16, D3DCOLOR_XRGB(123, 168, 232)},
		{17, D3DCOLOR_XRGB(137, 179, 240)},
		{18, D3DCOLOR_XRGB(146, 187, 247)},
		{19, D3DCOLOR_XRGB(117, 100, 116)},
		{20, D3DCOLOR_XRGB(82, 33, 33)},
		{21, D3DCOLOR_XRGB(107, 84, 106)},
		{22, D3DCOLOR_XRGB(4, 178, 184)},
		{23, D3DCOLOR_XRGB(180, 237, 212)},
		{24, D3DCOLOR_XRGB(255, 168, 254)}
	};

	auto it = colorMap.find(bTitleIndex);
	if (it != colorMap.end())
		return it->second;

	// Default color if not found
	return D3DCOLOR_XRGB(255, 255, 255);
}

void CInstanceBase::RefreshTextTailTitle()
{
	RefreshTextTail();
}

#ifdef TITLE_SYSTEM_BYLUZER
void CInstanceBase::UpdateTextTailGrade(BYTE grade)
{
	if (grade == 99)
		return;
	static D3DXCOLOR s_kGradeColor0 = D3DXCOLOR(219.0f/255.0f, 219.0f/255.0f, 219.0f/255.0f, 1.0f);
	static D3DXCOLOR s_kGradeColor1 = D3DXCOLOR(168.0f/255.0f, 230.0f/255.0f, 76.0f/255.0f, 1.0f);
	static D3DXCOLOR s_kGradeColor2 = D3DXCOLOR(255.0f/255.0f, 215.0f/255.0f, 76.0f/255.0f, 1.0f);
	char szText[128];
	sprintf(szText, "%s", title_name[grade]);
	if (IsPC())
		if (grade == 2||grade==5||grade==8||grade==11||grade==14||grade==17||grade==20) {
			CPythonTextTail::Instance().AttachGrade(GetVirtualID(), szText, s_kGradeColor2);
		}
		else if (grade == 1||grade == 4||grade==7||grade==10||grade==13||grade==16||grade==19) {
			CPythonTextTail::Instance().AttachGrade(GetVirtualID(), szText, s_kGradeColor1);
		}
		else {
			CPythonTextTail::Instance().AttachGrade(GetVirtualID(), szText, s_kGradeColor0);
		}
	else
		CPythonTextTail::Instance().DetachGrade(GetVirtualID());
	RefreshTextTail();
}
void CInstanceBase::SetGradeID(BYTE grade)
{
	if (IsPC())
	{
		m_title_active = grade;
		if (m_title_active != 99)
			UpdateTextTailGrade(m_title_active);
	}
}
#endif

/////////////////////////////////////////////////
void CInstanceBase::__ClearAffectFlagContainer()
{
	m_kAffectFlagContainer.Clear();
}

void CInstanceBase::__ClearAffects()
{
	if (IsStone())
	{
		__StoneSmoke_Destroy();
	}
	else
	{
		for (int iAffect=0; iAffect<AFFECT_NUM; ++iAffect)
		{
			__DetachEffect(m_adwCRCAffectEffect[iAffect]);
			m_adwCRCAffectEffect[iAffect]=0;
		}

		__ClearAffectFlagContainer();
	}

	m_GraphicThingInstance.__OnClearAffects();
}

/////////////////////////////////////////////////

void CInstanceBase::__SetNormalAffectFlagContainer(const CAffectFlagContainer& c_rkAffectFlagContainer)
{
	for (int i=0; i<CAffectFlagContainer::BIT_SIZE; ++i)
	{
		bool isOldSet=m_kAffectFlagContainer.IsSet(i);
		bool isNewSet=c_rkAffectFlagContainer.IsSet(i);

		if (isOldSet != isNewSet)
		{
			__SetAffect(i, isNewSet);

			if (isNewSet)
				m_GraphicThingInstance.__OnSetAffect(i);
			else
				m_GraphicThingInstance.__OnResetAffect(i);
		}
	}

	m_kAffectFlagContainer.CopyInstance(c_rkAffectFlagContainer);
}

void CInstanceBase::__SetStoneSmokeFlagContainer(const CAffectFlagContainer& c_rkAffectFlagContainer)
{
	m_kAffectFlagContainer.CopyInstance(c_rkAffectFlagContainer);

	DWORD eSmoke;
	if (m_kAffectFlagContainer.IsSet(STONE_SMOKE8))
		eSmoke=3;
	else if (m_kAffectFlagContainer.IsSet(STONE_SMOKE5)|m_kAffectFlagContainer.IsSet(STONE_SMOKE6)|m_kAffectFlagContainer.IsSet(STONE_SMOKE7))
		eSmoke=2;
	else if (m_kAffectFlagContainer.IsSet(STONE_SMOKE2)|m_kAffectFlagContainer.IsSet(STONE_SMOKE3)|m_kAffectFlagContainer.IsSet(STONE_SMOKE4))
		eSmoke=1;
	else
		eSmoke=0;

	__StoneSmoke_Destroy();
	__StoneSmoke_Create(eSmoke);
}

void CInstanceBase::SetAffectFlagContainer(const CAffectFlagContainer& c_rkAffectFlagContainer)
{
	if (IsBuilding())
	{
		return;
	}
	else if (IsStone())
	{
		__SetStoneSmokeFlagContainer(c_rkAffectFlagContainer);
	}
	else
	{
		__SetNormalAffectFlagContainer(c_rkAffectFlagContainer);
#ifdef __ENABLE_STEALTH_FIX__
		if (!__MainCanSeeHiddenThing() &&
			(c_rkAffectFlagContainer.IsSet(AFFECT_INVISIBILITY)
				|| c_rkAffectFlagContainer.IsSet(AFFECT_REVIVE_INVISIBILITY)
				|| c_rkAffectFlagContainer.IsSet(AFFECT_EUNHYEONG))
		)
			m_GraphicThingInstance.HideAllAttachingEffect();
#endif
	}
}

void CInstanceBase::SCRIPT_SetAffect(UINT eAffect, bool isVisible)
{
	__SetAffect(eAffect, isVisible);
}

void CInstanceBase::__SetReviveInvisibilityAffect(bool isVisible)
{
	if (isVisible)
	{
		if (IsWearingDress())
			return;

		m_GraphicThingInstance.BlendAlphaValue(0.5f, 1.0f);
	}
	else
	{
		m_GraphicThingInstance.BlendAlphaValue(1.0f, 1.0f);
	}
}

void CInstanceBase::__Assassin_SetEunhyeongAffect(bool isVisible)
{
	if (isVisible)
	{
		if (IsWearingDress())
			return;

		if (__IsMainInstance() || __MainCanSeeHiddenThing())
		{
			m_GraphicThingInstance.BlendAlphaValue(0.5f, 1.0f);
		}
		else
		{
			m_GraphicThingInstance.BlendAlphaValue(0.0f, 1.0f);
			m_GraphicThingInstance.HideAllAttachingEffect();
		}
	}
	else
	{
#ifdef ENABLE_CANSEEHIDDENTHING_FOR_GM
		if (IsAffect(AFFECT_INVISIBILITY) && __MainCanSeeHiddenThing())
			return;
#endif
		m_GraphicThingInstance.BlendAlphaValue(1.0f, 1.0f);
		m_GraphicThingInstance.ShowAllAttachingEffect();
	}
}

void CInstanceBase::__Shaman_SetParalysis(bool isParalysis)
{
	m_GraphicThingInstance.SetParalysis(isParalysis);
}

void CInstanceBase::__Warrior_SetGeomgyeongAffect(bool isVisible)
{
	if (isVisible)
	{
		if (IsWearingDress())
			return;

		if (m_kWarrior.m_dwGeomgyeongEffect)
			__DetachEffect(m_kWarrior.m_dwGeomgyeongEffect);

		m_GraphicThingInstance.SetReachScale(1.5f);
		if (m_GraphicThingInstance.IsTwoHandMode())
			m_kWarrior.m_dwGeomgyeongEffect=__AttachEffect(EFFECT_WEAPON+WEAPON_TWOHAND);
		else
			m_kWarrior.m_dwGeomgyeongEffect=__AttachEffect(EFFECT_WEAPON+WEAPON_ONEHAND);
	}
	else
	{
		m_GraphicThingInstance.SetReachScale(1.0f);

		__DetachEffect(m_kWarrior.m_dwGeomgyeongEffect);
		m_kWarrior.m_dwGeomgyeongEffect=0;
	}
}

void CInstanceBase::__SetAffect(UINT eAffect, bool isVisible)
{
	switch (eAffect)
	{
		case AFFECT_YMIR:
#ifdef ENABLE_CANSEEHIDDENTHING_FOR_GM
			if (IsAffect(AFFECT_INVISIBILITY) && !__MainCanSeeHiddenThing())
				return;
#else
			if (IsAffect(AFFECT_INVISIBILITY))
				return;
#endif
#ifdef MT2009_CINEMATIC
			return;
#endif
			break;

		case AFFECT_CHEONGEUN:
			m_GraphicThingInstance.SetResistFallen(isVisible);
			break;
		case AFFECT_GEOMGYEONG:
			__Warrior_SetGeomgyeongAffect(isVisible);
			return;
			break;
		case AFFECT_REVIVE_INVISIBILITY:
			__Assassin_SetEunhyeongAffect(isVisible);
			break;
		case AFFECT_EUNHYEONG:
			__Assassin_SetEunhyeongAffect(isVisible);
			break;
		case AFFECT_GYEONGGONG:
		case AFFECT_KWAESOK:
			if (isVisible)
				if (!IsWalking())
					return;
			break;
		case AFFECT_INVISIBILITY:
#ifdef ENABLE_CANSEEHIDDENTHING_FOR_GM
			if (__MainCanSeeHiddenThing())
			{
				if (isVisible)
					m_GraphicThingInstance.BlendAlphaValue(0.5f, 1.0f);
				else
					m_GraphicThingInstance.BlendAlphaValue(1.0f, 1.0f);
				break;
			}
#endif
			if (isVisible)
			{
				#ifdef __ENABLE_STEALTH_FIX__
				m_GraphicThingInstance.HideAllAttachingEffect();
				#else
				m_GraphicThingInstance.ClearAttachingEffect();
				#endif
				__EffectContainer_Destroy();
				DetachTextTail();
			}
			else
			{
				m_GraphicThingInstance.BlendAlphaValue(1.0f, 1.0f);
#ifdef __ENABLE_STEALTH_FIX__
#if defined(ENABLE_GRAPHIC_ON_OFF)
				m_GraphicThingInstance.SetActiveAllAttachingEffect();
#else
				m_GraphicThingInstance.ShowAllAttachingEffect();
#endif
#endif
				AttachTextTail();
				RefreshTextTail();
			}
			return;
			break;
//		case AFFECT_FAINT:
//			m_GraphicThingInstance.SetFaint(isVisible);
//			break;
//		case AFFECT_SLEEP:
//			m_GraphicThingInstance.SetSleep(isVisible);
//			break;
		case AFFECT_STUN:
			m_GraphicThingInstance.SetSleep(isVisible);
			break;
	}

	if (eAffect>=AFFECT_NUM)
	{
		TraceError("CInstanceBase[VID:%d]::SetAffect(eAffect:%d<AFFECT_NUM:%d, isVisible=%d)", GetVirtualID(), eAffect, isVisible);
		return;
	}

	if (isVisible)
	{
		if (!m_adwCRCAffectEffect[eAffect])
		{
			m_adwCRCAffectEffect[eAffect]=__AttachEffect(EFFECT_AFFECT+eAffect);
		}
	}
	else
	{
		if (m_adwCRCAffectEffect[eAffect])
		{
			__DetachEffect(m_adwCRCAffectEffect[eAffect]);
			m_adwCRCAffectEffect[eAffect]=0;
		}
	}
}

int CInstanceBase::HasAffect(DWORD dwIndex)
{
	if (dwIndex >= AFFECT_NUM)
	{
		TraceError("CInstanceBase[VID:%d]::HasAffect(index = %d)", GetVirtualID(), dwIndex);
		return 0;
	}

	return m_adwCRCAffectEffect[dwIndex];
}

bool CInstanceBase::IsPossibleEmoticon()
{
	CEffectManager& rkEftMgr=CEffectManager::Instance();
	for(DWORD eEmoticon = 0; eEmoticon < EMOTICON_NUM; eEmoticon++)
	{
		DWORD effectID = ms_adwCRCAffectEffect[EFFECT_EMOTICON+eEmoticon];
		if( effectID &&	rkEftMgr.IsAliveEffect(effectID) )
			return false;
	}

	if(ELTimer_GetMSec() - m_dwEmoticonTime < 1000)
	{
		TraceError("ELTimer_GetMSec() - m_dwEmoticonTime");
		return false;
	}

	return true;
}

void CInstanceBase::SetFishEmoticon()
{
	SetEmoticon(EMOTICON_FISH);
}

void CInstanceBase::SetEmoticon(UINT eEmoticon)
{
	if (eEmoticon>=EMOTICON_NUM)
	{
		TraceError("CInstanceBase[VID:%d]::SetEmoticon(eEmoticon:%d<EMOTICON_NUM:%d, isVisible=%d)",
			GetVirtualID(), eEmoticon);
		return;
	}
	if (IsPossibleEmoticon())
	{
		D3DXVECTOR3 v3Pos = m_GraphicThingInstance.GetPosition();
		v3Pos.z += float(m_GraphicThingInstance.GetHeight());

		//CEffectManager& rkEftMgr=CEffectManager::Instance();
		CCamera * pCamera = CCameraManager::Instance().GetCurrentCamera();

		D3DXVECTOR3 v3Dir = (pCamera->GetEye()-v3Pos)*9/10;
		v3Pos = pCamera->GetEye()-v3Dir;

		v3Pos = D3DXVECTOR3(0,0,0);
		v3Pos.z += float(m_GraphicThingInstance.GetHeight());

		//rkEftMgr.CreateEffect(ms_adwCRCAffectEffect[EFFECT_EMOTICON+eEmoticon],v3Pos,D3DXVECTOR3(0,0,0));
		m_GraphicThingInstance.AttachEffectByID(0, NULL, ms_adwCRCAffectEffect[EFFECT_EMOTICON+eEmoticon],&v3Pos);
		m_dwEmoticonTime = ELTimer_GetMSec();
	}
}

void CInstanceBase::SetDustGap(float fDustGap)
{
	ms_fDustGap=fDustGap;
}

void CInstanceBase::SetHorseDustGap(float fDustGap)
{
	ms_fHorseDustGap=fDustGap;
}

void CInstanceBase::__DetachEffect(DWORD dwEID)
{
	m_GraphicThingInstance.DettachEffect(dwEID);
}

DWORD CInstanceBase::__AttachEffect(UINT eEftType)
{
#ifdef ENABLE_CANSEEHIDDENTHING_FOR_GM
	if (IsAffect(AFFECT_INVISIBILITY) && !__MainCanSeeHiddenThing())
		return 0;
#else
	if (IsAffect(AFFECT_INVISIBILITY))
		return 0;
#endif

	if (eEftType>=EFFECT_NUM)
		return 0;
	


#ifdef ENABLE_SKILL_COSTUME_SYSTEM
	BYTE bEffectIndex = 0;
	switch (eEftType)
	{
		case EFFECT_AFFECT + AFFECT_HOSIN:
			bEffectIndex = GetSkillMotion(ESkillMotionLength::BUFF_BEGIN + 0);
			break;
		case EFFECT_AFFECT + AFFECT_BOHO:
			bEffectIndex = GetSkillMotion(ESkillMotionLength::BUFF_BEGIN + 1);
			break;
		case EFFECT_AFFECT + AFFECT_GICHEON:
			bEffectIndex = GetSkillMotion(ESkillMotionLength::BUFF_BEGIN + 2);
			break;
		case EFFECT_AFFECT + AFFECT_KWAESOK:
			bEffectIndex = GetSkillMotion(ESkillMotionLength::BUFF_BEGIN + 3);
			break;
		case EFFECT_AFFECT + AFFECT_JEUNGRYEOK:
			bEffectIndex = GetSkillMotion(ESkillMotionLength::BUFF_BEGIN + 4);
			break;
		case EFFECT_AFFECT + AFFECT_GONGPO:
		case EFFECT_AFFECT + AFFECT_JUMAGAP:
		case EFFECT_AFFECT + AFFECT_HEUKSIN:
		case EFFECT_AFFECT + AFFECT_MUYEONG:
		case EFFECT_AFFECT + AFFECT_FIRE:
		case EFFECT_AFFECT + AFFECT_PABEOP:
		case EFFECT_AFFECT + AFFECT_CHEONGEUN:
		case EFFECT_AFFECT + AFFECT_FALLEN_CHEONGEUN:
		case EFFECT_AFFECT + AFFECT_GWIGEOM:
		case EFFECT_AFFECT + AFFECT_GYEONGGONG:
		case EFFECT_WEAPON + WEAPON_ONEHAND:
		case EFFECT_WEAPON + WEAPON_TWOHAND:
			bEffectIndex = GetSkillMotion(GetSkillIndexByID(eEftType));
			break;
	}
	if (bEffectIndex != 0 && (eEftType != EFFECT_AFFECT + AFFECT_HOSIN || eEftType != EFFECT_AFFECT + AFFECT_BOHO || eEftType != EFFECT_AFFECT + AFFECT_GICHEON || eEftType != EFFECT_AFFECT + AFFECT_KWAESOK || eEftType != EFFECT_AFFECT + AFFECT_JEUNGRYEOK))
		eEftType += SKILL_COSTUME_EFFECT_START + (100 * (bEffectIndex - 1));
#endif

	if (ms_astAffectEffectAttachBone[eEftType].empty())
	{
		return m_GraphicThingInstance.AttachEffectByID(0, NULL, ms_adwCRCAffectEffect[eEftType]);
	}
	else
	{
		std::string & rstrBoneName = ms_astAffectEffectAttachBone[eEftType];
		const char * c_szBoneName;
		if (0 == rstrBoneName.compare("PART_WEAPON"))
		{
			if (m_GraphicThingInstance.GetAttachingBoneName(CRaceData::PART_WEAPON, &c_szBoneName))
			{
				return m_GraphicThingInstance.AttachEffectByID(0, c_szBoneName, ms_adwCRCAffectEffect[eEftType]);
			}
		}
		else if (0 == rstrBoneName.compare("PART_WEAPON_LEFT"))
		{
			if (m_GraphicThingInstance.GetAttachingBoneName(CRaceData::PART_WEAPON_LEFT, &c_szBoneName))
			{
				return m_GraphicThingInstance.AttachEffectByID(0, c_szBoneName, ms_adwCRCAffectEffect[eEftType]);
			}
		}
#ifdef ENABLE_COSTUME_EMBLEMAT
		else if (0 == rstrBoneName.compare("PART_EMBLEMAT"))
		{
			if (m_GraphicThingInstance.GetAttachingBoneName(CRaceData::PART_EMBLEMAT, &c_szBoneName))
			{
				return m_GraphicThingInstance.AttachEffectByID(0, c_szBoneName, ms_adwCRCAffectEffect[eEftType]);
			}
		}
#endif
		else
		{
			return m_GraphicThingInstance.AttachEffectByID(0, rstrBoneName.c_str(), ms_adwCRCAffectEffect[eEftType]);
		}
	}

	return 0;
}

#ifdef ENABLE_TITLE_ACHIEVEMENT_SYSTEM
void CInstanceBase::RegisterTitleAchievementName(BYTE iIndex, const char* c_szTitleName)
{
	g_sTitleAchievementNameMap.insert(std::make_pair(iIndex, c_szTitleName));
}
#endif

void CInstanceBase::__ComboProcess()
{
	/*
	DWORD dwcurComboIndex = m_GraphicThingInstance.GetComboIndex();

	if (0 != dwcurComboIndex)
	{
		if (m_dwLastComboIndex != m_GraphicThingInstance.GetComboIndex())
		{
			if (!m_GraphicThingInstance.IsHandMode() & IsAffect(AFFECT_HWAYEOM))
			{
				__AttachEffect(EFFECT_FLAME_ATTACK);
			}
		}
	}

	m_dwLastComboIndex = dwcurComboIndex;
	*/
}

bool CInstanceBase::RegisterEffect(UINT eEftType, const char* c_szEftAttachBone, const char* c_szEftName, bool isCache)
{
	if (eEftType>=EFFECT_NUM)
		return false;

	ms_astAffectEffectAttachBone[eEftType]=c_szEftAttachBone;

	DWORD& rdwCRCEft=ms_adwCRCAffectEffect[eEftType];
	if (!CEffectManager::Instance().RegisterEffect2(c_szEftName, &rdwCRCEft, isCache))
	{
		TraceError("CInstanceBase::RegisterEffect(eEftType=%d, c_szEftAttachBone=%s, c_szEftName=%s, isCache=%d) - Error", eEftType, c_szEftAttachBone, c_szEftName, isCache);
		rdwCRCEft=0;
		return false;
	}

	return true;
}

void CInstanceBase::RegisterTitleName(int iIndex, const char * c_szTitleName)
{
	g_TitleNameMap.emplace(iIndex, c_szTitleName);
}

D3DXCOLOR __RGBToD3DXColoru(UINT r, UINT g, UINT b)
{
	DWORD dwColor=0xff;dwColor<<=8;
	dwColor|=r;dwColor<<=8;
	dwColor|=g;dwColor<<=8;
	dwColor|=b;

	return D3DXCOLOR(dwColor);
}

bool CInstanceBase::RegisterNameColor(UINT uIndex, UINT r, UINT g, UINT b)
{
	if (uIndex>=NAMECOLOR_NUM)
		return false;

	g_akD3DXClrName[uIndex]=__RGBToD3DXColoru(r, g, b);
	return true;
}

bool CInstanceBase::RegisterTitleColor(UINT uIndex, UINT r, UINT g, UINT b)
{
	if (uIndex>=TITLE_NUM)
		return false;

	g_akD3DXClrTitle[uIndex]=__RGBToD3DXColoru(r, g, b);
	return true;
}

void CInstanceBase::__AttachEffectBoss()
{
	if (!__IsExistMainInstance())
		return;

	CInstanceBase * pkInstMain = __GetMainInstancePtr();

	if (IsWarp())
		return;
	if (IsObject())
		return;
	if (IsFlag())
		return;
	if (IsResource())
		return;

	__EffectContainer_AttachEffect(EFFECT_BOSS);
}
//martysama0134's ceqyqttoaf71vasf9t71218
