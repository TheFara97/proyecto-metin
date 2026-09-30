#include "StdAfx.h"

#ifdef ENABLE_ANTIHL
#include "antihl.h"
#include "PythonNetworkStream.h"
#include <tchar.h>

HookDet::HookDet()
{
	if (m_vecDetectionIDs.empty())
		m_vecDetectionIDs.clear();
};
HookDet::~HookDet() {};

void HookDet::Init(HINSTANCE hInstance)
{
	m_hInstance = hInstance;
	HLDetected = false;
	COMDetected = false;
}
//#define __ENABLE_NEW_HOOK__
#ifdef __ENABLE_NEW_HOOK__
bool HookDet::MacroDetected = false;

LRESULT HookDet::HookProcedure(int nCode, WPARAM wParam, LPARAM lParam)
{
	if (nCode >= 0)
	{
		if (wParam == WM_LBUTTONDOWN)
		{
			MSLLHOOKSTRUCT* mouseStruct = (MSLLHOOKSTRUCT*)lParam;
			if ((mouseStruct->flags & LLMHF_INJECTED) && !MacroDetected)
			{
				HookDet::Instance().AppendDetection(MACRO);
				MacroDetected = true;
			}
		}
	}
	return CallNextHookEx(NULL, nCode, wParam, lParam);
}
#endif

void HookDet::DetectHLHook()
{
	if (HLDetected)
		return;

	if (!HookDet::Instance().GetSendOnClickPacketAddress())
	{
		typedef bool (CPythonNetworkStream::* SendOnClickPacketFunc)(DWORD);
		SendOnClickPacketFunc pSendOnClickPacket = &CPythonNetworkStream::SendOnClickPacket;
		sendOnClickPacketAddress = *(DWORD*)&pSendOnClickPacket;
#ifdef __ENABLE_NEW_HOOK__
		HHOOK mouseHook = SetWindowsHookEx(WH_MOUSE_LL, HookProcedure, GetModuleHandle(NULL), NULL);
#endif
	}
	uint8_t uFirstByte = reinterpret_cast<uint8_t*>(HookDet::Instance().GetSendOnClickPacketAddress())[0];
	if (uFirstByte == 0xE9 || uFirstByte == 0x68 || uFirstByte == 0xC3 || uFirstByte == 0x90)
	{
		HookDet::Instance().AppendDetection(HLBOT);
		HLDetected = true;
	}
}

void HookDet::DetectCOM()
{
	if (COMDetected)
		return;
	
	if (GetModuleHandleA("COM2Magical.dll"))
	{
		HookDet::Instance().AppendDetection(COM2);
		COMDetected = true;
	}
}

void HookDet::DetectREX()
{
	if (REXDetected)
		return;

	if (GetModuleHandleA("ubot.dll"))
	{
		HookDet::Instance().AppendDetection(REX);
		REXDetected = true;
	}
}

BOOL CALLBACK EnumReksati(HWND hwnd, LPARAM lParam)
{
	static TCHAR buffer[64];
	GetWindowText(hwnd, buffer, 64);
	if (_tcsstr(buffer, "Reksati"))
	{
		*((PBYTE)0x0) = 0xd3adb33f;
		assert(false);
		exit(3);
		return FALSE;
	}
	return TRUE;
}

void HookDet::DetectReksati()
{
	EnumWindows(EnumReksati, NULL);
}

void HookDet::AppendDetection(DWORD dwDetectionID)
{
	if (std::find(m_vecDetectionIDs.begin(), m_vecDetectionIDs.end(), dwDetectionID) == m_vecDetectionIDs.end())
	{
		m_vecDetectionIDs.push_back(dwDetectionID);
	}
}

int RandomRange(int min, int max)
{
	return min + (rand() & (int)(max - min + 1));
}

DWORD HookDet::GetRandomCleanID()
{
	return m_vecCleanIDs[RandomRange(0, m_vecCleanIDs.size() - 1)];
}

DWORD HookDet::GetDetectionID()
{
	HookDet::Instance().DetectHLHook();
	HookDet::Instance().DetectCOM();
	HookDet::Instance().DetectREX();
	HookDet::Instance().DetectReksati();

	if (m_vecDetectionIDs.empty())
		return HookDet::Instance().GetRandomCleanID();

	DWORD dwDetectionID = m_vecDetectionIDs.front();
	m_vecDetectionIDs.erase(m_vecDetectionIDs.begin());

	return dwDetectionID;
}

#endif