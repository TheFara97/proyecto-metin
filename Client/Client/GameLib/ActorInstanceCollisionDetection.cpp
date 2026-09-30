#include "StdAfx.h"
#include "../eterLib/GrpMath.h"
#include "ActorInstance.h"
#include "../UserInterface/PythonBackground.h"

extern "C" {
	bool IsMetinQueueActive();
	DWORD GetMetinQueueNextTarget();
}

void CActorInstance::__InitializeCollisionData()
{
	m_canSkipCollision=false;
}

void CActorInstance::EnableSkipCollision()
{
	m_canSkipCollision=true;
}

void CActorInstance::DisableSkipCollision()
{
	m_canSkipCollision=false;
}

bool CActorInstance::CanSkipCollision()
{
	return m_canSkipCollision;
}

void CActorInstance::UpdatePointInstance()
{
	TCollisionPointInstanceListIterator itor;
	for (itor = m_DefendingPointInstanceList.begin(); itor != m_DefendingPointInstanceList.end(); ++itor)
		UpdatePointInstance(&(*itor));
}

BOOL CActorInstance::IsInSafe(CActorInstance& ptr)
{
	const TPixelPosition& c_rkPPosCur = ptr.NEW_GetCurPixelPositionRef();
	if (CPythonBackground::Instance().isAttrOn(c_rkPPosCur.x, c_rkPPosCur.y, (1 << 2)))
		return true;

	return false;
}

void CActorInstance::UpdatePointInstance(TCollisionPointInstance * pPointInstance)
{
	if (!pPointInstance)
	{
		assert(!"CActorInstance::UpdatePointInstance - pPointInstance is NULL");
		return;
	}

	D3DXMATRIX matBone;

	if (pPointInstance->isAttached)
	{
		if (pPointInstance->dwModelIndex>=m_LODControllerVector.size())
		{
			//Tracenf("CActorInstance::UpdatePointInstance - rInstance.dwModelIndex=%d >= m_LODControllerVector.size()=%d",
			//		pPointInstance->dwModelIndex>m_LODControllerVector.size());
			return;
		}

		CGrannyLODController* pGrnLODController=m_LODControllerVector[pPointInstance->dwModelIndex];
		if (!pGrnLODController)
		{
			//Tracenf("CActorInstance::UpdatePointInstance - m_LODControllerVector[pPointInstance->dwModelIndex=%d] is NULL", pPointInstance->dwModelIndex);
			return;
		}

		CGrannyModelInstance * pModelInstance = pGrnLODController->GetModelInstance();
		if (!pModelInstance)
		{
			//Tracenf("CActorInstance::UpdatePointInstance - pGrnLODController->GetModelInstance() is NULL");
			return;
		}

		D3DXMATRIX * pmatBone = (D3DXMATRIX *)pModelInstance->GetBoneMatrixPointer(pPointInstance->dwBoneIndex);
		matBone = *(D3DXMATRIX *)pModelInstance->GetCompositeBoneMatrixPointer(pPointInstance->dwBoneIndex);
		matBone._41 = pmatBone->_41;
		matBone._42 = pmatBone->_42;
		matBone._43 = pmatBone->_43;
		matBone *= m_worldMatrix;
	}
	else
	{
		matBone = m_worldMatrix;
	}

	// Update Collsion Sphere
	CSphereCollisionInstanceVector::const_iterator sit = pPointInstance->c_pCollisionData->SphereDataVector.begin();
	CDynamicSphereInstanceVector::iterator dit=pPointInstance->SphereInstanceVector.begin();
	for (;sit!=pPointInstance->c_pCollisionData->SphereDataVector.end();++sit,++dit)
	{
		const TSphereData & c = sit->GetAttribute();//c_pCollisionData->SphereDataVector[j].GetAttribute();

		D3DXMATRIX matPoint;
		D3DXMatrixTranslation(&matPoint, c.v3Position.x, c.v3Position.y, c.v3Position.z);
		matPoint = matPoint * matBone;

		dit->v3LastPosition = dit->v3Position;
		dit->v3Position.x = matPoint._41;
		dit->v3Position.y = matPoint._42;
		dit->v3Position.z = matPoint._43;
	}
}

void CActorInstance::UpdateAdvancingPointInstance()
{
	D3DXVECTOR3 v3Movement = m_v3Movement;
	if (m_pkHorse)
		v3Movement = m_pkHorse->m_v3Movement;

	if (m_pkHorse)
		m_pkHorse->UpdateAdvancingPointInstance();

	D3DXMATRIX matPoint;
	D3DXMATRIX matCenter;

	TCollisionPointInstanceListIterator itor = m_BodyPointInstanceList.begin();
	for (; itor != m_BodyPointInstanceList.end(); ++itor)
	{
		TCollisionPointInstance & rInstance = *itor;

		if (rInstance.isAttached)
		{
			if (rInstance.dwModelIndex>=m_LODControllerVector.size())
			{
				Tracenf("CActorInstance::UpdateAdvancingPointInstance - rInstance.dwModelIndex=%d >= m_LODControllerVector.size()=%d",
					rInstance.dwModelIndex, m_LODControllerVector.size());
				continue;
			}

			CGrannyLODController* pGrnLODController=m_LODControllerVector[rInstance.dwModelIndex];
			if (!pGrnLODController)
			{
				Tracenf("CActorInstance::UpdateAdvancingPointInstance - m_LODControllerVector[rInstance.dwModelIndex=%d] is NULL", rInstance.dwModelIndex);
				continue;
			}

			CGrannyModelInstance * pModelInstance = pGrnLODController->GetModelInstance();
			if (!pModelInstance)
			{
				//Tracenf("CActorInstance::UpdateAdvancingPointInstance - pGrnLODController->GetModelInstance() is NULL");
				continue;
			}

			matCenter = *(D3DXMATRIX *)pModelInstance->GetBoneMatrixPointer(rInstance.dwBoneIndex);
			matCenter *= m_worldMatrix;
		}
		else
		{
			matCenter = m_worldMatrix;
		}

		// Update Collision Sphere
		const NRaceData::TCollisionData * c_pCollisionData = rInstance.c_pCollisionData;
		if (c_pCollisionData)
		{
			for (DWORD j = 0; j < c_pCollisionData->SphereDataVector.size(); ++j)
			{
				const TSphereData & c = c_pCollisionData->SphereDataVector[j].GetAttribute();
				CDynamicSphereInstance & rSphereInstance = rInstance.SphereInstanceVector[j];

				D3DXMatrixTranslation(&matPoint, c.v3Position.x, c.v3Position.y, c.v3Position.z);
				matPoint = matPoint * matCenter;

				rSphereInstance.v3LastPosition.x = matPoint._41;
				rSphereInstance.v3LastPosition.y = matPoint._42;
				rSphereInstance.v3LastPosition.z = matPoint._43;
				rSphereInstance.v3Position = rSphereInstance.v3LastPosition;
				rSphereInstance.v3Position += v3Movement;
			}
		}
	}
}

bool CActorInstance::CheckCollisionDetection(const CDynamicSphereInstanceVector * c_pAttackingSphereVector, D3DXVECTOR3 * pv3Position)
{
	if (!c_pAttackingSphereVector)
	{
		assert(!"CActorInstance::CheckCollisionDetection - c_pAttackingSphereVector is NULL");
		return false;
	}

	TCollisionPointInstanceListIterator itor;
	for (itor = m_DefendingPointInstanceList.begin(); itor != m_DefendingPointInstanceList.end(); ++itor)
	{
		const CDynamicSphereInstanceVector * c_pDefendingSphereVector = &(*itor).SphereInstanceVector;

		for (DWORD i = 0; i < c_pAttackingSphereVector->size(); ++i)
		for (DWORD j = 0; j < c_pDefendingSphereVector->size(); ++j)
		{
			const CDynamicSphereInstance & c_rAttackingSphere = c_pAttackingSphereVector->at(i);
			const CDynamicSphereInstance & c_rDefendingSphere = c_pDefendingSphereVector->at(j);

			if (DetectCollisionDynamicSphereVSDynamicSphere(c_rAttackingSphere, c_rDefendingSphere))
			{
				*pv3Position = (c_rAttackingSphere.v3Position + c_rDefendingSphere.v3Position) / 2.0f;
				return true;
			}
		}
	}

	return false;
}

bool CActorInstance::CreateCollisionInstancePiece(DWORD dwAttachingModelIndex, const NRaceData::TAttachingData * c_pAttachingData, TCollisionPointInstance * pPointInstance)
{
	if (!c_pAttachingData)
	{
		assert(!"CActorInstance::CreateCollisionInstancePiece - c_pAttachingData is NULL");
		return false;
	}

	if (!c_pAttachingData->pCollisionData)
	{
		assert(!"CActorInstance::CreateCollisionInstancePiece - c_pAttachingData->pCollisionData is NULL");
		return false;
	}

	if (!pPointInstance)
	{
		assert(!"CActorInstance::CreateCollisionInstancePiece - pPointInstance is NULL");
		return false;
	}

	pPointInstance->dwModelIndex = dwAttachingModelIndex;
	pPointInstance->isAttached = FALSE;
	pPointInstance->dwBoneIndex = 0;
	pPointInstance->c_pCollisionData = c_pAttachingData->pCollisionData;

	if (c_pAttachingData->isAttaching)
	{
		int iAttachingBoneIndex;

		CGrannyModelInstance * pModelInstance = m_LODControllerVector[dwAttachingModelIndex]->GetModelInstance();

		if (pModelInstance && pModelInstance->GetBoneIndexByName(c_pAttachingData->strAttachingBoneName.c_str(),
												&iAttachingBoneIndex))
		{
			pPointInstance->isAttached = TRUE;
			pPointInstance->dwBoneIndex = iAttachingBoneIndex;
		}
		else
		{
			//TraceError("CActorInstance::CreateCollisionInstancePiece: Cannot get matrix of bone %s ModelInstance 0x%p",	c_pAttachingData->strAttachingBoneName.c_str(), pModelInstance);
			pPointInstance->isAttached = TRUE;
			pPointInstance->dwBoneIndex = 0;
		}
	}

	const CSphereCollisionInstanceVector & c_rSphereDataVector = c_pAttachingData->pCollisionData->SphereDataVector;

	pPointInstance->SphereInstanceVector.clear();
	pPointInstance->SphereInstanceVector.reserve(c_rSphereDataVector.size());

	CSphereCollisionInstanceVector::const_iterator it;
	CDynamicSphereInstance dsi;

	dsi.v3LastPosition = D3DXVECTOR3(0.0f,0.0f,0.0f);
	dsi.v3Position = D3DXVECTOR3(0.0f,0.0f,0.0f);
	for (it = c_rSphereDataVector.begin(); it!=c_rSphereDataVector.end(); ++it)
	{
		const TSphereData & c_rSphereData = it->GetAttribute();
		dsi.fRadius = c_rSphereData.fRadius;
		pPointInstance->SphereInstanceVector.push_back(dsi);
	}

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

BOOL CActorInstance::__SplashAttackProcess(CActorInstance & rVictim)
{
	D3DXVECTOR3 v3Distance(rVictim.m_x - m_x, rVictim.m_z - m_z, rVictim.m_z - m_z);
	float fDistance = D3DXVec3LengthSq(&v3Distance);
	if (fDistance >= 1000.0f*1000.0f)
		return FALSE;

	// Check Distance
	if (!__IsInSplashTime())
		return FALSE;

	const CRaceMotionData::TMotionAttackingEventData * c_pAttackingEvent = m_kSplashArea.c_pAttackingEvent;
	const NRaceData::TAttackData & c_rAttackData = c_pAttackingEvent->AttackData;
	THittedInstanceMap & rHittedInstanceMap = m_kSplashArea.HittedInstanceMap;

	if (rHittedInstanceMap.end() != rHittedInstanceMap.find(&rVictim))
	{
		return FALSE;
	}

	if (NRaceData::ATTACK_TYPE_SNIPE == c_rAttackData.iAttackType)
	{
		if (__IsFlyTargetPC())
			if (!__IsSameFlyTarget(&rVictim))
				return FALSE;

	}

	D3DXVECTOR3 v3HitPosition;
	if (rVictim.CheckCollisionDetection(&m_kSplashArea.SphereInstanceVector, &v3HitPosition))
	{
		rHittedInstanceMap.emplace(&rVictim, GetLocalTime()+c_rAttackData.fInvisibleTime);

		int iCurrentHitCount = rHittedInstanceMap.size();
		//int iMaxHitCount = (0 == c_rAttackData.iHitLimitCount ? 16 : c_rAttackData.iHitLimitCount);
		int iMaxHitCount = (0 == c_rAttackData.iHitLimitCount ? 30 : c_rAttackData.iHitLimitCount);
		//Tracef(" ------------------- Splash Hit : %d\n", iCurrentHitCount);

		if (iCurrentHitCount > iMaxHitCount)
		{
			//Tracef(" ------------------- OVER FLOW :: Splash Hit Count : %d\n", iCurrentHitCount);
			return FALSE;
		}

		NEW_SetAtkPixelPosition(NEW_GetCurPixelPositionRef());
		__ProcessDataAttackSuccess(c_rAttackData, rVictim, v3HitPosition, m_kSplashArea.uSkill, m_kSplashArea.isEnableHitProcess);
		return TRUE;
	}

	return FALSE;
}



BOOL CActorInstance::__NormalAttackProcess(CActorInstance & rVictim)
{
	// Check Distance
	D3DXVECTOR3 v3Distance(rVictim.m_x - m_x, rVictim.m_z - m_z, rVictim.m_z - m_z);
	float fDistance = D3DXVec3LengthSq(&v3Distance);

	extern bool IS_HUGE_RACE(unsigned int vnum);
	if (IS_HUGE_RACE(rVictim.GetRace()))
	{
		if (fDistance >= 500.0f*500.0f)
			return FALSE;
	}
	else
	{
		if (IsPoly()) {
			if (fDistance >= 360.0f * 360.0f)
				return FALSE;
		}
		else {
			if (fDistance >= 300.0f * 300.0f)
				return FALSE;
		}
	}

	if (!isValidAttacking())
		return FALSE;

	float c_fAttackRadius;
	if (__IsMountingHorse()) {
		c_fAttackRadius = 80.0f;
	}
	else if (IsPoly()) {
		c_fAttackRadius = 30.0f;
	}
	else {
		c_fAttackRadius = 20.0f;
	}

	const NRaceData::TMotionAttackData * pad = m_pkCurRaceMotionData->GetMotionAttackDataPointer();

	const float motiontime = GetAttackingElapsedTime();

	NRaceData::THitDataContainer::const_iterator itorHitData = pad->HitDataContainer.begin();
	for (; itorHitData != pad->HitDataContainer.end(); ++itorHitData)
	{
		const NRaceData::THitData & c_rHitData = *itorHitData;

		THitDataMap::iterator itHitData = m_HitDataMap.find(&c_rHitData);
		if (itHitData != m_HitDataMap.end())
		{
			THittedInstanceMap & rHittedInstanceMap = itHitData->second;

			THittedInstanceMap::iterator itInstance;
			if ((itInstance=rHittedInstanceMap.find(&rVictim)) != rHittedInstanceMap.end())
			{
				if (pad->iMotionType==NRaceData::MOTION_TYPE_COMBO || itInstance->second > GetLocalTime())
					continue;
			}
		}

		NRaceData::THitTimePositionMap::const_iterator range_start, range_end;
		range_start = c_rHitData.mapHitPosition.lower_bound(motiontime-CTimer::Instance().GetElapsedSecond());
		range_end = c_rHitData.mapHitPosition.upper_bound(motiontime);
		float c = cosf(D3DXToRadian(GetRotation()));
		float s = sinf(D3DXToRadian(GetRotation()));

		for(;range_start!=range_end;++range_start)
		{
			const CDynamicSphereInstance& dsiSrc=range_start->second;

			CDynamicSphereInstance dsi;
			dsi = dsiSrc;

			dsi.fRadius = c_fAttackRadius;

			{
				D3DXVECTOR3 v3SrcDir=dsiSrc.v3Position-dsiSrc.v3LastPosition;
				v3SrcDir*=__GetReachScale();
				const D3DXVECTOR3& v3Src = dsiSrc.v3LastPosition+v3SrcDir;
				D3DXVECTOR3& v3Dst = dsi.v3Position;
				v3Dst.x = v3Src.x * c - v3Src.y * s;
				v3Dst.y = v3Src.x * s + v3Src.y * c;
				v3Dst += GetPosition();
			}
			{
				const D3DXVECTOR3& v3Src = dsiSrc.v3LastPosition;
				D3DXVECTOR3& v3Dst = dsi.v3LastPosition;
				v3Dst.x = v3Src.x * c - v3Src.y * s;
				v3Dst.y = v3Src.x * s + v3Src.y * c;
				v3Dst += GetPosition();
			}
			
			TCollisionPointInstanceList::iterator cpit;
			for(cpit = rVictim.m_DefendingPointInstanceList.begin(); cpit!=rVictim.m_DefendingPointInstanceList.end();++cpit)
			{
				int index = 0;
				const CDynamicSphereInstanceVector & c_DefendingSphereVector = cpit->SphereInstanceVector;
				CDynamicSphereInstanceVector::const_iterator dsit;
				for(dsit = c_DefendingSphereVector.begin(); dsit!= c_DefendingSphereVector.end();++dsit, ++index)
				{
					const CDynamicSphereInstance& sub = *dsit;
					if (DetectCollisionDynamicZCylinderVSDynamicZCylinder(dsi, sub))
					{
						THitDataMap::iterator itHitData = m_HitDataMap.find(&c_rHitData);
						if (itHitData == m_HitDataMap.end())
						{
							THittedInstanceMap HittedInstanceMap;
							HittedInstanceMap.emplace(&rVictim, GetLocalTime()+pad->fInvisibleTime);
							//HittedInstanceMap.emplace(&rVictim, GetLocalTime()+HIT_COOL_TIME);
							m_HitDataMap.emplace(&c_rHitData, HittedInstanceMap);

							//Tracef(" ----------- First Hit\n");
						}
						else
						{
							itHitData->second.emplace(&rVictim, GetLocalTime()+pad->fInvisibleTime);
							//itHitData->second.emplace(&rVictim, GetLocalTime()+HIT_COOL_TIME);

							//Tracef(" ----------- Next Hit : %d\n", itHitData->second.size());

							int iCurrentHitCount = itHitData->second.size();
							if (NRaceData::MOTION_TYPE_COMBO == pad->iMotionType || NRaceData::MOTION_TYPE_NORMAL == pad->iMotionType)
							{
								//if (iCurrentHitCount > 16)
								if (iCurrentHitCount > 30)
								{
									//Tracef(" Type NORMAL :: Overflow - Can't process, skip\n");
									return FALSE;
								}
							}
							else
							{
								if (iCurrentHitCount > pad->iHitLimitCount)
								{
									//Tracef(" Type SKILL :: Overflow - Can't process, skip\n");
									return FALSE;
								}
							}
						}

						D3DXVECTOR3 v3HitPosition = (GetPosition() + rVictim.GetPosition()) *0.5f;

						extern bool IS_HUGE_RACE(unsigned int vnum);
						if (IS_HUGE_RACE(rVictim.GetRace()))
						{
							v3HitPosition = (GetPosition() + sub.v3Position) * 0.5f;
						}

						__ProcessDataAttackSuccess(*pad, rVictim, v3HitPosition, m_kCurMotNode.uSkill);
						return TRUE;
					}
				}
			}
		}
	}

	return FALSE;
}

BOOL CActorInstance::AttackingProcess(CActorInstance & rVictim)
{
	if (rVictim.__isInvisible())
		return FALSE;

	if (__SplashAttackProcess(rVictim))
		return TRUE;

	if (__NormalAttackProcess(rVictim))
		return TRUE;

	return FALSE;
}

BOOL CActorInstance::TestPhysicsBlendingCollision(CActorInstance & rVictim)
{
	if (rVictim.IsDead())
		return FALSE;

	TPixelPosition kPPosLast;
	GetBlendingPosition( &kPPosLast );

	D3DXVECTOR3 v3Distance = D3DXVECTOR3(rVictim.m_x - kPPosLast.x, rVictim.m_y - kPPosLast.y, rVictim.m_z - kPPosLast.z);
	float fDistance = D3DXVec3LengthSq(&v3Distance);
	if (fDistance > 800.0f*800.0f)
		return FALSE;

	TCollisionPointInstanceList * pMainList;
	TCollisionPointInstanceList * pVictimList;
	if (isAttacking() || IsWaiting())
	{
		pMainList = &m_DefendingPointInstanceList;
		pVictimList = &rVictim.m_DefendingPointInstanceList;
	}
	else
	{
		pMainList = &m_BodyPointInstanceList;
		pVictimList = &rVictim.m_BodyPointInstanceList;
	}

	TPixelPosition kPDelta;
	m_PhysicsObject.GetLastPosition(&kPDelta);

	D3DXVECTOR3 prevLastPosition, prevPosition;
	const int nSubCheckCount = 50;

	TCollisionPointInstanceListIterator itorMain = pMainList->begin();
	TCollisionPointInstanceListIterator itorVictim = pVictimList->begin();
	for (; itorMain != pMainList->end(); ++itorMain)
	{
		for (; itorVictim != pVictimList->end(); ++itorVictim)
		{
			CDynamicSphereInstanceVector & c_rMainSphereVector = (*itorMain).SphereInstanceVector;
			CDynamicSphereInstanceVector & c_rVictimSphereVector = (*itorVictim).SphereInstanceVector;

			for (DWORD i = 0; i < c_rMainSphereVector.size(); ++i)
			{
				CDynamicSphereInstance & c_rMainSphere = c_rMainSphereVector[i];
				//adjust main sphere center
				prevLastPosition = c_rMainSphere.v3LastPosition;
				prevPosition	 = c_rMainSphere.v3Position;

				c_rMainSphere.v3LastPosition = prevPosition;

				for( int i = 1; i <= nSubCheckCount; ++ i )
				{
					c_rMainSphere.v3Position = prevPosition + (float)(i/(float)nSubCheckCount) * kPDelta;

					for (DWORD j = 0; j < c_rVictimSphereVector.size(); ++j)
					{
						CDynamicSphereInstance & c_rVictimSphere = c_rVictimSphereVector[j];

						if (DetectCollisionDynamicSphereVSDynamicSphere(c_rMainSphere, c_rVictimSphere))
						{
							BOOL bResult = GetVector3Distance(c_rMainSphere.v3Position, c_rVictimSphere.v3Position) <= GetVector3Distance(c_rMainSphere.v3LastPosition, c_rVictimSphere.v3Position);

							c_rMainSphere.v3LastPosition = prevLastPosition;
							c_rMainSphere.v3Position	 = prevPosition;

							return bResult;
						}
					}
				}

				//restore
				c_rMainSphere.v3LastPosition = prevLastPosition;
				c_rMainSphere.v3Position	 = prevPosition;
			}
		}
	}

	return FALSE;
}

#define ENABLE_STOP_COLISSION_GLOBAL
BOOL CActorInstance::TestActorCollision(CActorInstance& rVictim)
{
#ifdef ENABLE_METIN_QUEUE
	if (IsMetinQueueActive())
	{
		DWORD queueTarget = GetMetinQueueNextTarget();

		// If queue is active and this victim is not our target, skip collision
		if (rVictim.GetVirtualID() != queueTarget)
			return FALSE;
	}
#endif
#ifdef ENABLE_STOP_COLISSION_GLOBAL
	/*********************************************************************
	* date		: 2016.02.16
	* function	: Stop Colission
	* developer	: VegaS
	* skype		: sacadatt.amazon
	* description : Checks if the victim is one of the examples below you can easily configure. If the victim was found
					success as vnum site / breed ve you could go through it no longer block.
	*/
	/************
	* The first value is the minimum value and the second value is the maximum value of pet vnum (mob_proto) - change 34051 with your max vnum of pet */
	int pListPet[2] = { 34001, 34099 };
	int pListShops[2] = { 30001, 30009 };
	int pListMounts[2] = { 20101, 20950 };

	int pListMobs[2] = { 101, 554 };
	int pListMobs1[2] = { 701, 993 };
	int pListMobs2[2] = { 1001, 1071 };
	int pListMobs3[2] = { 595, 690 };
	int pListMobs4[2] = { 1101, 1177 };
	int pListMobs5[2] = { 2001, 2076 };
	int pListMobs6[2] = { 2102, 2415 };
	int pListMobs7[2] = { 2501, 2547 };
	int pListMobs8[2] = { 3001, 3105 };
	int pListMobs9[2] = { 3201, 3205 };
	int pListMobs10[2] = { 3301, 3405 };
	/************
	* You can add whatever you like vnum of npc or monster (mob_proto) */
	int pListGlobal[] = { 10, 11, 6781, 6782, 6771, 6774, 6777, 6779, 6770, 6004, 6005, 6006, 6007, 6003, 6002, 9673, 9680, 9671, 9672, 9674, 9676, 9679, 3301, 3302, 3303, 3304,
	16185, 16186, 16187, 16188, 16189, 16190, 16191, 16192, 16193, 16194, 16195, 16163, 16164, 16165, 16166, 16167, 16168, 16169, 16170, 16171, 16172, 16173, 9690, 9691, 9692, 9693,
	9697, 9698, 9699, 9700, 16150, 16152, 16153, 29750, 29751, 200040, 1001, 1002, 9702, 9703, 9704, 9705, 9706, 201000, 201001, 201002, 201003, 201010, 201011, 201012, 201013,
	34916, 34924, 201025, 201005, 201029, 34925, 34926, 200301, 200302, 200303, 200304, 34925, 34926, 34927, 34928, 34929, 34930, 34931, 34932, 34933, 34934, 34935, 34936, 201040,
	201041, 201042, 201043, 201044, 201045, 201046, 201047, 201048, 201049, 201050, 201051, 201052, 201053, 201054, 201055, 201056, 201057, 201058, 201059, 201060, 201061, 201062,
	201063, 201064, 201065, 201066, 201067, 201068, 202100, 202101, 202102, 202203, 202204, 202205, 202209, 202210, 202211 };
	/************
	* You can add what mapname you want for enable this stop collission global like pet / npc */
	const char* strMapListGlobal[] = { "metin2_map_a1", "metin2_map_a3", "metin2_map_c1", "metin2_map_c3",
									"10d_spiderqueen", "ateop_map_orc", "metin2_map_anglar_dungeon_01", "6d_sanktuarium",
									"ateop_fm_4", "ateop_fm_3", "ateop_fm_2", "2d_beran", "dt_glador", "plechito_scorpion_dungeon", "plechito_pirate_ship",
									"plechito_pirate_ship2", "5d_ezoty", "plechito_chamber_of_wisdom", "plechito_demon_dungeon", "plechito_ancient", "ateop_map_flame",
									"9d_trytony", "metin2_map_sohan", "metin2_map_las", "map_mining", "zaris_fish", "8d_zlote", "4d_orki",
									"mehok_map_worldboss_2", "zaris_easter_map_2025", "zaris_easter_dungeon_2025", "7d_pieklo", "alune_ac3_party_1", "alune_ac3_party_2", "alune_ac3_party_3", "alune2_akademia_party1",
									"alune2_akademia_party2", "alune_ac3_party_4", "alune_map24_exp_light", "alune_map24_exp_dark", "map_element_dung_mehok", "alune_legend25", "valygos_legend_1", "map_magma_mehok", "zaris_summer_map", "zaris_summer_dungeon", "plechito_mirage_island"};
	/************
	* Location name of the map where the event takes place ox */
	const char* strMapEventOx = "season1/metin2_map_oxevent";
	const char* strMapLegend1 = "asenis_legenda_1";
	const char* strMapLegend2 = "asenis_legenda_2";
	const char* strMapLegend3 = "asenis_legenda_3";
	const char* strMapLegend4 = "asenis_legenda_4";
	const char* strWorldBoss1 = "alune_legend25";
	const char* strWorldBoss2 = "map_element_dung_mehok";
	const char* strWorldBoss3 = "valygos_legend_1";
	const char* strWorldBoss4 = "map_magma_mehok";
	const char* strWorldBoss5 = "mehok_map_worldboss_2";
	const char* strRaidDungeon1 = "alune_ac3_party_1";


	std::string stringName = CPythonBackground::Instance().GetWarpMapName();

	for (int i = 0; i < _countof(strMapListGlobal); i++)
	{
#ifdef ENABLE_STOP_COLLISION_PLAYER_OX
		if (strMapEventOx == stringName) // Check if u are place in map ox
		{
			if (0 <= rVictim.GetRace() && rVictim.GetRace() <= 7) // Check if the victim through which pass over a player (change 7 with 8 if u have wolfman)
				return FALSE;	// Stop collission for player --> You can go through players now successfully without lock yourself		
		}
		if (strMapLegend1 == stringName) // Check if u are place in map ox
		{
			if (0 <= rVictim.GetRace() && rVictim.GetRace() <= 7) // Check if the victim through which pass over a player (change 7 with 8 if u have wolfman)
				return FALSE;	// Stop collission for player --> You can go through players now successfully without lock yourself		
		}
		if (strMapLegend2 == stringName) // Check if u are place in map ox
		{
			if (0 <= rVictim.GetRace() && rVictim.GetRace() <= 7) // Check if the victim through which pass over a player (change 7 with 8 if u have wolfman)
				return FALSE;	// Stop collission for player --> You can go through players now successfully without lock yourself		
		}
		if (strMapLegend3 == stringName) // Check if u are place in map ox
		{
			if (0 <= rVictim.GetRace() && rVictim.GetRace() <= 7) // Check if the victim through which pass over a player (change 7 with 8 if u have wolfman)
				return FALSE;	// Stop collission for player --> You can go through players now successfully without lock yourself		
		}
		if (strMapLegend4 == stringName) // Check if u are place in map ox
		{
			if (0 <= rVictim.GetRace() && rVictim.GetRace() <= 7) // Check if the victim through which pass over a player (change 7 with 8 if u have wolfman)
				return FALSE;	// Stop collission for player --> You can go through players now successfully without lock yourself		
		}
#endif		
		if (strWorldBoss1 == stringName) // Check if u are place in map ox
		{
			if (0 <= rVictim.GetRace() && rVictim.GetRace() <= 7) // Check if the victim through which pass over a player (change 7 with 8 if u have wolfman)
				return FALSE;	// Stop collission for player --> You can go through players now successfully without lock yourself		
		}
		if (strWorldBoss2 == stringName) // Check if u are place in map ox
		{
			if (0 <= rVictim.GetRace() && rVictim.GetRace() <= 7) // Check if the victim through which pass over a player (change 7 with 8 if u have wolfman)
				return FALSE;	// Stop collission for player --> You can go through players now successfully without lock yourself		
		}
		if (strWorldBoss3 == stringName) // Check if u are place in map ox
		{
			if (0 <= rVictim.GetRace() && rVictim.GetRace() <= 7) // Check if the victim through which pass over a player (change 7 with 8 if u have wolfman)
				return FALSE;	// Stop collission for player --> You can go through players now successfully without lock yourself		
		}
		if (strWorldBoss4 == stringName) // Check if u are place in map ox
		{
			if (0 <= rVictim.GetRace() && rVictim.GetRace() <= 7) // Check if the victim through which pass over a player (change 7 with 8 if u have wolfman)
				return FALSE;	// Stop collission for player --> You can go through players now successfully without lock yourself		
		}
		if (strWorldBoss5 == stringName) // Check if u are place in map ox
		{
			if (0 <= rVictim.GetRace() && rVictim.GetRace() <= 7) // Check if the victim through which pass over a player (change 7 with 8 if u have wolfman)
				return FALSE;	// Stop collission for player --> You can go through players now successfully without lock yourself		
		}
		if (strRaidDungeon1 == stringName) // Check if u are place in map ox
		{
			if (0 <= rVictim.GetRace() && rVictim.GetRace() <= 7) // Check if the victim through which pass over a player (change 7 with 8 if u have wolfman)
				return FALSE;	// Stop collission for player --> You can go through players now successfully without lock yourself		
		}
		if (strMapListGlobal[i] == stringName) // Check if you are in one of the maps listed in the global list
		{
			for (int i = 0; i < _countof(pListGlobal); i++)
			{
				if (rVictim.GetRace() == pListGlobal[i] || pListPet[0] <= rVictim.GetRace() && rVictim.GetRace() <= pListPet[1]) // Verify that the victim is npc vnum listed above, or if a pet.
					return FALSE;	// Stop collission for global vnum like a pet or npc							
				if (rVictim.GetRace() == pListGlobal[i] || pListShops[0] <= rVictim.GetRace() && rVictim.GetRace() <= pListShops[1]) // Verify that the victim is npc vnum listed above, or if a pet.
					return FALSE;	// Stop collission for global vnum like a pet or npc							
				if (rVictim.GetRace() == pListGlobal[i] || pListMounts[0] <= rVictim.GetRace() && rVictim.GetRace() <= pListMounts[1]) // Verify that the victim is npc vnum listed above, or if a pet.
					return FALSE;	// Stop collission for global vnum like a pet or npc							
				if (rVictim.GetRace() == pListGlobal[i] || pListMobs[0] <= rVictim.GetRace() && rVictim.GetRace() <= pListMobs[1]) // Verify that the victim is npc vnum listed above, or if a pet.
					return FALSE;	// Stop collission for global vnum like a pet or npc										
				if (rVictim.GetRace() == pListGlobal[i] || pListMobs1[0] <= rVictim.GetRace() && rVictim.GetRace() <= pListMobs1[1]) // Verify that the victim is npc vnum listed above, or if a pet.
					return FALSE;	// Stop collission for global vnum like a pet or npc										
				if (rVictim.GetRace() == pListGlobal[i] || pListMobs2[0] <= rVictim.GetRace() && rVictim.GetRace() <= pListMobs2[1]) // Verify that the victim is npc vnum listed above, or if a pet.
					return FALSE;	// Stop collission for global vnum like a pet or npc										
				if (rVictim.GetRace() == pListGlobal[i] || pListMobs3[0] <= rVictim.GetRace() && rVictim.GetRace() <= pListMobs3[1]) // Verify that the victim is npc vnum listed above, or if a pet.
					return FALSE;	// Stop collission for global vnum like a pet or npc										
				if (rVictim.GetRace() == pListGlobal[i] || pListMobs4[0] <= rVictim.GetRace() && rVictim.GetRace() <= pListMobs4[1]) // Verify that the victim is npc vnum listed above, or if a pet.
					return FALSE;	// Stop collission for global vnum like a pet or npc										
				if (rVictim.GetRace() == pListGlobal[i] || pListMobs5[0] <= rVictim.GetRace() && rVictim.GetRace() <= pListMobs5[1]) // Verify that the victim is npc vnum listed above, or if a pet.
					return FALSE;	// Stop collission for global vnum like a pet or npc										
				if (rVictim.GetRace() == pListGlobal[i] || pListMobs6[0] <= rVictim.GetRace() && rVictim.GetRace() <= pListMobs6[1]) // Verify that the victim is npc vnum listed above, or if a pet.
					return FALSE;	// Stop collission for global vnum like a pet or npc										
				if (rVictim.GetRace() == pListGlobal[i] || pListMobs7[0] <= rVictim.GetRace() && rVictim.GetRace() <= pListMobs7[1]) // Verify that the victim is npc vnum listed above, or if a pet.
					return FALSE;	// Stop collission for global vnum like a pet or npc										
				if (rVictim.GetRace() == pListGlobal[i] || pListMobs8[0] <= rVictim.GetRace() && rVictim.GetRace() <= pListMobs8[1]) // Verify that the victim is npc vnum listed above, or if a pet.
					return FALSE;	// Stop collission for global vnum like a pet or npc										
				if (rVictim.GetRace() == pListGlobal[i] || pListMobs9[0] <= rVictim.GetRace() && rVictim.GetRace() <= pListMobs9[1]) // Verify that the victim is npc vnum listed above, or if a pet.
					return FALSE;	// Stop collission for global vnum like a pet or npc							
				if (rVictim.GetRace() == pListGlobal[i] || pListMobs10[0] <= rVictim.GetRace() && rVictim.GetRace() <= pListMobs10[1]) // Verify that the victim is npc vnum listed above, or if a pet.
					return FALSE;	// Stop collission for global vnum like a pet or npc		
			}
		}
	}
#endif
	/*
		if (m_pkHorse)
		{
			if (m_pkHorse->TestActorCollision(rVictim))
				return TRUE;

			return FALSE;
		}
	*/

	if (rVictim.IsDead())
		return FALSE;

	// Check Distance
	// NOTE : Ŕű´çČ÷ ¸Ö¸é ĂĽĹ© ľČÇÔ
	//        ÇÁ·ąŔÓ ˝şĹµ˝ĂłŞ ´ë»ó żŔşęÁ§Ć®ŔÇ Ĺ©±â°ˇ Ĺ¬°ćżě ą®Á¦°ˇ »ý±ć ż©Áö°ˇ ŔÖŔ˝
	//        Äł¸ŻĹÍ°ˇ ŔÚ˝ĹŔÇ Body Sphere Radius ş¸´Ů ´ő Ĺ©°Ô ŔĚµżÇß´ÂÁö¸¦ ĂĽĹ©ÇĎ°í,
	//        ¸¸ľŕ ±×·¸Áö ľĘ´Ů¸é °Ĺ¸®·Î ĂĽĹ©ÇŘĽ­ °É·ŻÁŘ´Ů.
	D3DXVECTOR3 v3Distance = D3DXVECTOR3(rVictim.m_x - m_x, rVictim.m_y - m_y, rVictim.m_z - m_z);
	float fDistance = D3DXVec3LengthSq(&v3Distance);
	if (fDistance > 800.0f * 800.0f)
		return FALSE;

	// NOTE : °ř°Ý ÁßŔĎ¶§´Â Defending Sphere·Î Collision Check¸¦ ÇŐ´Ď´Ů.
	// NOTE : Wait·Î şí·»µů µÇ´Â µµÁßżˇ ¶Ő°í µéľî°ˇ´Â ą®Á¦°ˇ ŔÖľîĽ­.. - [levites]
	TCollisionPointInstanceList* pMainList;
	TCollisionPointInstanceList* pVictimList;
	if (isAttacking() || IsWaiting())
	{
		pMainList = &m_DefendingPointInstanceList;
		pVictimList = &rVictim.m_DefendingPointInstanceList;
	}
	else
	{
		pMainList = &m_BodyPointInstanceList;
		pVictimList = &rVictim.m_BodyPointInstanceList;
	}

	TCollisionPointInstanceListIterator itorMain = pMainList->begin();
	TCollisionPointInstanceListIterator itorVictim = pVictimList->begin();
	for (; itorMain != pMainList->end(); ++itorMain)
		for (; itorVictim != pVictimList->end(); ++itorVictim)
		{
			const CDynamicSphereInstanceVector& c_rMainSphereVector = (*itorMain).SphereInstanceVector;
			const CDynamicSphereInstanceVector& c_rVictimSphereVector = (*itorVictim).SphereInstanceVector;

			for (DWORD i = 0; i < c_rMainSphereVector.size(); ++i)
				for (DWORD j = 0; j < c_rVictimSphereVector.size(); ++j)
				{
					const CDynamicSphereInstance& c_rMainSphere = c_rMainSphereVector[i];
					const CDynamicSphereInstance& c_rVictimSphere = c_rVictimSphereVector[j];

					if (DetectCollisionDynamicSphereVSDynamicSphere(c_rMainSphere, c_rVictimSphere))
					{
						if (GetVector3Distance(c_rMainSphere.v3Position, c_rVictimSphere.v3Position) <=
							GetVector3Distance(c_rMainSphere.v3LastPosition, c_rVictimSphere.v3Position))
						{
							return TRUE;
						}
						return FALSE;
					}
				}
		}

	return FALSE;
}

bool CActorInstance::AvoidObject(const CGraphicObjectInstance& c_rkBGObj)
{
#ifdef __MOVIE_MODE__
	if (IsMovieMode())
		return false;
#endif

	if (this==&c_rkBGObj)
		return false;

	if (!__TestObjectCollision(&c_rkBGObj))
		return false;

	__AdjustCollisionMovement(&c_rkBGObj);
	return true;
}

bool CActorInstance::IsBlockObject(const CGraphicObjectInstance& c_rkBGObj)
{
	if (this==&c_rkBGObj)
		return false;

	if (!__TestObjectCollision(&c_rkBGObj))
		return false;

	return true;
}

void CActorInstance::BlockMovement()
{
	if (m_pkHorse)
	{
		m_pkHorse->__InitializeMovement();
		return;
	}

	__InitializeMovement();
}

BOOL CActorInstance::__TestObjectCollision(const CGraphicObjectInstance * c_pObjectInstance)
{
	if (m_pkHorse)
	{
		if (m_pkHorse->__TestObjectCollision(c_pObjectInstance))
			return TRUE;

		return FALSE;
	}

	if (m_canSkipCollision)
		return FALSE;

	if (m_v3Movement.x == 0.0f && m_v3Movement.y == 0.0f && m_v3Movement.z == 0.0f)
		return FALSE;

	TCollisionPointInstanceListIterator itorMain = m_BodyPointInstanceList.begin();
	for (; itorMain != m_BodyPointInstanceList.end(); ++itorMain)
	{
		const CDynamicSphereInstanceVector & c_rMainSphereVector = (*itorMain).SphereInstanceVector;
		for (DWORD i = 0; i < c_rMainSphereVector.size(); ++i)
		{
			const CDynamicSphereInstance & c_rMainSphere = c_rMainSphereVector[i];

			if (c_pObjectInstance->MovementCollisionDynamicSphere(c_rMainSphere))
			{
				//const D3DXVECTOR3 & c_rv3Position = c_pObjectInstance->GetPosition();
				//if (GetVector3Distance(c_rMainSphere.v3Position, c_rv3Position) <
				//	GetVector3Distance(c_rMainSphere.v3LastPosition, c_rv3Position))
				{
					return TRUE;
				}

				//return FALSE;
			}
		}
	}

	return FALSE;
}

bool CActorInstance::TestCollisionWithDynamicSphere(const CDynamicSphereInstance & dsi)
{
	TCollisionPointInstanceListIterator itorMain = m_BodyPointInstanceList.begin();
	for (; itorMain != m_BodyPointInstanceList.end(); ++itorMain)
	{
		const CDynamicSphereInstanceVector & c_rMainSphereVector = (*itorMain).SphereInstanceVector;
		for (DWORD i = 0; i < c_rMainSphereVector.size(); ++i)
		{
			const CDynamicSphereInstance & c_rMainSphere = c_rMainSphereVector[i];

			if (DetectCollisionDynamicSphereVSDynamicSphere(c_rMainSphere, dsi))
			{
				return true;
			}
		}
	}

	return false;
}
//martysama0134's ceqyqttoaf71vasf9t71218
