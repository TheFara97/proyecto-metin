#include "StdAfx.h"
#include "PythonNetworkStream.h"
#ifdef ENABLE_BADGE_NOTIFICATION_MANAGER
#include "BadgeNotificationManager.h"
#endif

void CPythonNetworkStream::OnRemoteDisconnect()
{
	PyCallClassMemberFunc(m_poHandler, "SetLoginPhase", Py_BuildValue("()"));
#ifdef ENABLE_BADGE_NOTIFICATION_MANAGER
	static auto& badge_notification_manager = CBadgeNotificationManager::Instance();
	badge_notification_manager.Reset();
#endif
}

void CPythonNetworkStream::OnDisconnect()
{
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
// Main Game
void CPythonNetworkStream::OnScriptEventStart(int iSkin, int iIndex)
{
	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "OpenQuestWindow", Py_BuildValue("(ii)", iSkin, iIndex));
}
//martysama0134's ceqyqttoaf71vasf9t71218
