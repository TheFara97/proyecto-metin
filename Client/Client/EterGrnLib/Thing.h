#pragma once

#include "Model.h"
#include "Motion.h"

#include <memory>

class CGraphicThing : public CResource
{
	public:
		typedef CRef<CGraphicThing> TRef;

	public:
		static CGraphicThing::TType Type();

	public:
		CGraphicThing(const char * c_szFileName);
		virtual ~CGraphicThing();

		virtual bool			CreateDeviceObjects();
		virtual void			DestroyDeviceObjects();

		bool					CheckModelIndex(int iModel) const;
		CGrannyModel *			GetModelPointer(int iModel);
		int						GetModelCount() const;

		bool					CheckMotionIndex(int iMotion) const;
		std::shared_ptr<CGrannyMotion> GetMotionPointer(int iMotion);
		int						GetMotionCount() const;
		virtual void			LoadNew(granny_file* file, bool cached);

	protected:
		void					Initialize();

		bool					LoadModels();
		bool					LoadMotions();

	protected:
		bool					OnLoad(int iSize, const void* c_pvBuf);
		void					OnClear();
		bool					OnIsEmpty() const;
		bool					OnIsType(TType type);

	protected:
		granny_file *			m_pgrnFile;
		granny_file_info *		m_pgrnFileInfo;

		granny_animation *		m_pgrnAni;

		CGrannyModel *			m_models;
		std::vector<std::shared_ptr<CGrannyMotion>>			m_motions;
		bool					m_bIsCached;
};
//martysama0134's ceqyqttoaf71vasf9t71218
