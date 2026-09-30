#pragma once

#include "../eterLib/StdAfx.h"
#include "../eterGrnLib/StdAfx.h"

#include "../UserInterface/Locale_inc.h"
#ifdef AT
#undef AT // @warme667
#endif

#ifdef _DEBUG
	#undef _DEBUG
	#include <python27/Python.h>
	#define _DEBUG
#else
	#include <python27/Python.h>
#endif
#include <python27/node.h>
#include <python27/grammar.h>
#include <python27/token.h>
#include <python27/parsetok.h>
#include <python27/errcode.h>
#include <python27/compile.h>
#include <python27/eval.h>
#include <python27/marshal.h>

#ifdef AT
#undef AT // @warme667
#endif
#undef BYTE
#include "PythonUtils.h"
#include "PythonLauncher.h"
#include "PythonMarshal.h"
#include "Resource.h"

void initdbg();

// PYTHON_EXCEPTION_SENDER
class IPythonExceptionSender
{
	public:
		void Clear()
		{
			m_strExceptionString = "";
		}

		void RegisterExceptionString(const char * c_szString)
		{
			m_strExceptionString += c_szString;
		}

		virtual void Send() = 0;

	protected:
		std::string m_strExceptionString;
};

extern IPythonExceptionSender * g_pkExceptionSender;

void SetExceptionSender(IPythonExceptionSender * pkExceptionSender);
// END_OF_PYTHON_EXCEPTION_SENDER
//martysama0134's ceqyqttoaf71vasf9t71218
