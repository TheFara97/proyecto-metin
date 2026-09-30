#pragma once

#include <memory>
#include "GrpRenderTargetTexture.h"

class CInstanceBase;
class CGraphicImageInstance;

class CRenderTarget
{
	using TCharacterInstanceMap = std::map<DWORD, CInstanceBase*>;
	
	public:
		CRenderTarget(DWORD width, DWORD height);
		~CRenderTarget();


		void SetVisibility(bool isShow);
		void RenderTexture() const;
		void SetRenderingRect(RECT* rect) const;

		void SelectModel(DWORD index);
		void ResetModel();
		bool CreateBackground(const char* imgPath, DWORD width, DWORD height);
		void SetZoom(bool bZoom) noexcept;
		void RenderBackground() const;
		void UpdateModel();
		void DeformModel() const;
		void RenderModel() const;
		void SetArmor(DWORD vnum);
		void ChangeHair(DWORD vnum);
		void SetSkillCostume(DWORD vnum);
		void ChangeEffect();
		void SetWeapon(DWORD vnum);
		void SetAcce(DWORD vnum);
		void CreateTextures() const;
		void ReleaseTextures() const;
		void SetAll();
		void SetCustomZoom(float zoom) { m_fCustomZoom = zoom; }
		float GetCustomZoom() const { return m_fCustomZoom; }
	
	private:
		std::unique_ptr<CInstanceBase> m_pModel; 
		std::unique_ptr<CGraphicImageInstance> m_background;
		std::unique_ptr<CGraphicRenderTargetTexture> m_renderTargetTexture;
		float m_modelRotation;
		float m_fTargetHeight;
		float m_fZoomY;
		float m_fTargetY;
		float m_fEyeY;
		bool m_visible;
		float m_fCustomZoom;
};
