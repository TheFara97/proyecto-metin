#pragma once

#include <memory>
#include "GrpRenderTargetTexture.h"

class CInstanceBase;
class CGraphicExpandedImageInstance;

class CRenderTarget
{
	using TCharacterInstanceMap = std::map<DWORD, CInstanceBase*>;
	
	public:
		CRenderTarget(DWORD width, DWORD height);
		~CRenderTarget();

		void SetVisibility(bool isShow);
		void RenderTexture() const;
		void SetRenderingRect(RECT* rect) const;

		void SelectModel(int index);
		bool CreateBackground(const char* imgPath, DWORD width, DWORD height);

		void ChangeArmor(DWORD vnum);
		void ChangeWeapon(DWORD vnum);
		void ChangeHair(DWORD vnum);
		void SetSkillCostume(DWORD vnum);

		void RenderBackground() const;
		void UpdateModel();
		void DeformModel() const;
		void RenderModel() const;

		void CreateTextures() const;
		void ReleaseTextures() const;
	
	private:
		std::unique_ptr<CInstanceBase> m_pModel; 
		std::unique_ptr<CGraphicExpandedImageInstance> m_background;
		std::unique_ptr<CGraphicRenderTargetTexture> m_renderTargetTexture;
		float m_modelRotation;
		bool m_visible;
		DWORD m_currentRace;
};
