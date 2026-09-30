#include "StdAfx.h"
#include "CRenderTarget.h"
#include "../EterLib/Camera.h"
#include "../EterLib/CRenderTargetManager.h"
#include "../EterPythonLib/PythonGraphic.h"


#include "../EterBase/CRC32.h"
#include "../GameLib/GameType.h"
#include "../GameLib/MapType.h"
#include "../GameLib/ItemData.h"
#include "../GameLib/ItemManager.h"
#include "../GameLib/ActorInstance.h"
#include "../UserInterface/InstanceBase.h"


#include "ResourceManager.h"


CRenderTarget::CRenderTarget(const DWORD width, const DWORD height) : m_pModel(nullptr),
                                 m_background(nullptr),
                                 m_modelRotation(0),
								 m_fTargetHeight(0.0f),
								 m_fZoomY(0.0f),
								 m_fEyeY(0.0f),
								 m_fTargetY(0.0f),
                                 m_visible(false) 
{
	auto pTex = new CGraphicRenderTargetTexture;
	if (!pTex->Create(width, height, D3DFMT_X8R8G8B8, D3DFMT_D16)) {
		delete pTex;
		TraceError("CRenderTarget::CRenderTarget: Could not create CGraphicRenderTargetTexture %dx%d", width, height);
		throw std::runtime_error("CRenderTarget::CRenderTarget: Could not create CGraphicRenderTargetTexture");
	}

	m_renderTargetTexture = std::unique_ptr<CGraphicRenderTargetTexture>(pTex);
}

CRenderTarget::~CRenderTarget()
{
	m_background = nullptr;
	m_fEyeY = 0.0f;
	m_fZoomY = 0.0f;
	m_fTargetHeight = 0.0f;
	m_fTargetY = 0.0f;
	m_visible = false;
}

void CRenderTarget::SetVisibility(bool isShow)
{
	m_visible = isShow;
}
void CRenderTarget::SetArmor(DWORD vnum)
{
	if (!m_visible || !m_pModel)
		return;

	m_pModel->ChangeArmor(vnum);
	SetAll();
}

void CRenderTarget::SetWeapon(DWORD vnum)
{
	if (!m_visible || !m_pModel)
		return;

	m_pModel->ChangeWeapon(vnum);
	SetAll();
}

void CRenderTarget::ChangeHair(DWORD vnum)
{
	if (!m_visible || !m_pModel)
		return;

	m_pModel->ChangeHair(vnum);
	SetAll();
}

void CRenderTarget::SetAcce(DWORD vnum)
{
	if (!m_visible || !m_pModel)
		return;

	//_pModel->SetAcce(vnum);
	m_pModel->ChangeAcce(vnum - 85000);
	SetAll();
}

void CRenderTarget::SetSkillCostume(DWORD vnum)
{
	if (!m_visible || !m_pModel)
		return;

	m_pModel->ChangeArmor(vnum);
	SetAll();

#ifdef ENABLE_SKILL_PREVIEW
	CItemData* pItemData = nullptr;
	if (CItemManager::instance().GetItemDataPointer(vnum, &pItemData))
	{
		UINT uSkill = static_cast<UINT>(pItemData->GetValue(3));
		BYTE bSkillSet = static_cast<BYTE>(pItemData->GetValue(4));
		m_pModel->SetSkillPreview(uSkill, bSkillSet);
	}
#endif
}

void CRenderTarget::ChangeEffect()
{
	if (!m_visible || !m_pModel)
		return;

	m_pModel->GetGraphicThingInstanceRef().RenderAllAttachingEffect(); 
	m_pModel->Refresh(CRaceMotionData::NAME_WAIT, true);
	SetAll();
}	

void CRenderTarget::RenderTexture() const
{
	m_renderTargetTexture->Render();
}

void CRenderTarget::SetRenderingRect(RECT* rect) const
{
	m_renderTargetTexture->SetRenderingRect(rect);
}

void CRenderTarget::CreateTextures() const
{
	m_renderTargetTexture->CreateTextures();
}

void CRenderTarget::ReleaseTextures() const
{
	m_renderTargetTexture->ReleaseTextures();
}

void CRenderTarget::ResetModel()
{
	if (m_pModel)
	{
		m_pModel->ClearWikiAffect();
		m_pModel.reset();
	}
}

void CRenderTarget::SelectModel(const DWORD index)
{
	// if (index == 0)
	// {
		//delete m_pModel;
		// m_pModel.reset();
		// return;
	// }

	CInstanceBase::SCreateData kCreateData{};

	kCreateData.m_bType = CActorInstance::TYPE_PC; // Dynamic Type
	kCreateData.m_dwRace = index;
#ifdef ENABLE_RENDER_TARGET_EFFECT
	kCreateData.m_isRenderTarget = true;
#endif

	auto model = std::make_unique<CInstanceBase>();
	if (!model->Create(kCreateData))
	{
		ResetModel();
		return;
	}

	m_pModel = std::move(model);
	m_fCustomZoom = 0.0f;
	SetAll();
}

void CRenderTarget::SetAll()
{
	m_pModel->GetGraphicThingInstancePtr()->SetSkipLod(true);
	m_modelRotation = 0.0f;
	m_pModel->NEW_SetPixelPosition(TPixelPosition(0.0f, 0.0f, 0.0f));
	m_pModel->Refresh(CRaceMotionData::NAME_WAIT, true);
	m_pModel->SetLoopMotion(CRaceMotionData::NAME_WAIT);
	m_pModel->SetAlwaysRender(true);
	m_pModel->SetRotation(0.0f);

	auto& camera_manager = CCameraManager::instance();
	camera_manager.SetCurrentCamera(CCameraManager::RENDER_TARGET_CAMERA);
	camera_manager.GetCurrentCamera()->SetTargetHeight(110.0);
	camera_manager.ResetToPreviousCamera();
}


bool CRenderTarget::CreateBackground(const char* imgPath, const DWORD width, const DWORD height)
{
	if (m_background)
		return false;

	m_background = std::make_unique<CGraphicImageInstance>();
	
	const auto graphic_image = dynamic_cast<CGraphicImage*>(CResourceManager::instance().GetResourcePointer(imgPath));
	if (!graphic_image)
	{
		m_background.reset();
		return false;
	}

	m_background->SetImagePointer(graphic_image);
	m_background->SetScale(static_cast<float>(width) / graphic_image->GetWidth(), static_cast<float>(height) / graphic_image->GetHeight());
	return true;
}


void CRenderTarget::SetZoom(bool bZoom) noexcept
{
	if (!m_pModel)
		return;

	if (bZoom)
	{
		m_fZoomY = m_fZoomY - m_fTargetHeight;
		const float v3 = -(m_fEyeY * 8.9f - m_fEyeY * 3.0f);
		m_fZoomY = fmax(v3, m_fZoomY);
	}
	else
	{
		m_fZoomY = m_fZoomY + m_fTargetHeight;
		const float v6 = 14000.0f - m_fEyeY * 8.9f;
		const float v7 = m_fEyeY * 8.9f + m_fEyeY * 5.0f;
		m_fZoomY = fmin(m_fZoomY, v6);
		m_fZoomY = fmin(m_fZoomY, v7);
	}
}

void CRenderTarget::RenderBackground() const
{
	if (!m_visible)
		return;

	if (!m_background)
		return;

	auto& rectRender = *m_renderTargetTexture->GetRenderingRect();
	m_renderTargetTexture->SetRenderTarget();

	CGraphicRenderTargetTexture::Clear();
	CPythonGraphic::Instance().SetInterfaceRenderState();

	const auto width = static_cast<float>(rectRender.right - rectRender.left);
	const auto height = static_cast<float>(rectRender.bottom - rectRender.top);

	CPythonGraphic::Instance().SetViewport(0.0f, 0.0f, width, height);

	m_background->Render();
	m_renderTargetTexture->ResetRenderTarget();
	CPythonGraphic::Instance().RestoreViewport();
}

void CRenderTarget::UpdateModel()
{
	if (!m_visible || !m_pModel)
		return;

	if (m_modelRotation < 360.0f)
		m_modelRotation += 1.0f;
	else
		m_modelRotation = 0.0f;

	m_pModel->SetRotation(m_modelRotation);
	m_pModel->Transform();
	m_pModel->GetGraphicThingInstanceRef().RotationProcess();
#ifdef ENABLE_SKILL_PREVIEW
	m_pModel->Update();
#endif
}

void CRenderTarget::DeformModel() const
{
	if (!m_visible || !m_pModel)
		return;

	// Force-show: the culling manager may have hidden the model because
	// its world position (0,0,0) is outside the main camera frustum.
	m_pModel->GetGraphicThingInstanceRef().Show();
	m_pModel->Deform();
}

void CRenderTarget::RenderModel() const  // NO parameters
{
	if (!m_visible)
	{
		return;
	}

	auto& python_graphic = CPythonGraphic::Instance();
	auto& camera_manager = CCameraManager::instance();
	auto& state_manager = CStateManager::Instance();

	auto& rectRender = *m_renderTargetTexture->GetRenderingRect();

	if (!m_pModel)
	{
		return;
	}

	m_renderTargetTexture->SetRenderTarget();

	if (!m_background)
	{
		m_renderTargetTexture->Clear();
	}

	python_graphic.ClearDepthBuffer();

	// Ensure model is visible - culling manager may have hidden it
	m_pModel->GetGraphicThingInstanceRef().Show();

	const auto fov = python_graphic.GetFOV();
	const auto aspect = python_graphic.GetAspect();
	const auto near_y = python_graphic.GetNear();
	const auto far_y = python_graphic.GetFar();

	const auto width = static_cast<float>(rectRender.right - rectRender.left);
	const auto height = static_cast<float>(rectRender.bottom - rectRender.top);

	state_manager.SetRenderState(D3DRS_FOGENABLE, FALSE);

	python_graphic.SetViewport(0.0f, 0.0f, width, height);

	python_graphic.PushState();

	CActorInstance& rkActor = m_pModel->GetGraphicThingInstanceRef();
	const float fActorHeight = rkActor.GetHeight();

	// Use custom zoom if set, otherwise auto-calculate from model height
	float zoomDistance;
	if (m_fCustomZoom > 0.0f)
		zoomDistance = m_fCustomZoom;
	else
		zoomDistance = fmax(1500.0f, fActorHeight * 5.0f);

	const D3DXVECTOR3 v3Eye(0.0f, -zoomDistance, 0.0f);
	const D3DXVECTOR3 v3Target(0.0f, m_fTargetY, fActorHeight / 2);
	const D3DXVECTOR3 v3Up(0.0f, 0.0f, 1.0f);

	camera_manager.SetCurrentCamera(CCameraManager::SHOPDECO_CAMERA);
	camera_manager.GetCurrentCamera()->SetViewParams(v3Eye, v3Target, v3Up);

	python_graphic.UpdateViewMatrix();

	python_graphic.SetPerspective(10.0f, width / height, 100.0f, 15000.0f);

	m_pModel->Render();
	m_pModel->GetGraphicThingInstanceRef().RenderAllAttachingEffect();
#ifndef ENABLE_RENDER_TARGET_EFFECT
	m_pModel->GetGraphicThingInstancePtr()->ClearAttachingEffect();
#endif

	camera_manager.ResetToPreviousCamera();
	python_graphic.RestoreViewport();
	python_graphic.PopState();
	python_graphic.SetPerspective(fov, aspect, near_y, far_y);
	m_renderTargetTexture->ResetRenderTarget();
	state_manager.SetRenderState(D3DRS_FOGENABLE, FALSE);
}
