#pragma once
#include "desc_client.h"

class CLoginSim
{
	public:
		CLoginSim()
		{
#ifdef __AUTH_QUEUEING__
			memset(&auth, 0, sizeof(TPacketGDAuthLogin));
			memset(&login, 0, sizeof(TPacketGDLoginByKey));
			memset(&load, 0, sizeof(TPlayerLoadPacket));
			memset(&logout, 0, sizeof(TLogoutPacket));
			handle_ = 0;
			loginKey_ = 0;
#else
			memset(&auth, 0, sizeof(auth));
			memset(&login, 0, sizeof(login));
			memset(&load, 0, sizeof(load));
			memset(&logout, 0, sizeof(logout));
#endif
			vecIdx = 0;
			bCheck = false;
		}
	
		void AddPlayer(DWORD dwID)
		{
			vecID.push_back(dwID);
			sys_log(0, "AddPlayer %lu", dwID);
		}
	
		bool IsCheck()
		{
			return bCheck;
		}
	
		void SendLogin()
		{
			bCheck = true;
	
#ifdef __AUTH_QUEUEING__
			if (vecIdx > 0 && IsDone())
				return;
#else
			if (IsDone())
				return;
#endif
	
			if (vecIdx == 0)
			{
#ifdef __AUTH_QUEUEING__
				db_clientdesc->DBPacket(HEADER_GD_AUTH_LOGIN, handle_, &auth, sizeof(TPacketGDAuthLogin));
				sys_log(0, "AuthLoginSend handle: [%d]", handle_);
#else
				db_clientdesc->DBPacket(HEADER_GD_AUTH_LOGIN, 0, &auth, sizeof(TPacketGDAuthLogin));
#endif
				return;
			}
	
			load.player_id = vecID[vecIdx++];
#ifdef __AUTH_QUEUEING__
			db_clientdesc->DBPacket(HEADER_GD_LOGIN_BY_KEY, handle_, &login, sizeof(TPacketGDLoginByKey));
#else
			db_clientdesc->DBPacket(HEADER_GD_LOGIN_BY_KEY, 0, &login, sizeof(TPacketGDLoginByKey));
#endif
		}
	
		void SendLoad()
		{
#ifdef __AUTH_QUEUEING__
			db_clientdesc->DBPacket(HEADER_GD_PLAYER_LOAD, handle_, &load, sizeof(TPlayerLoadPacket));
#else
			db_clientdesc->DBPacket(HEADER_GD_PLAYER_LOAD, 0, &load, sizeof(TPlayerLoadPacket));
#endif
		}
	
		void SendLogout()
		{
#ifdef __AUTH_QUEUEING__
			db_clientdesc->DBPacket(HEADER_GD_LOGOUT, handle_, &logout, sizeof(TLogoutPacket));
#else
			db_clientdesc->DBPacket(HEADER_GD_LOGOUT, 0, &logout, sizeof(TLogoutPacket));
#endif
			SendLogin();
		}
	
		bool IsDone()
		{
			if (vecIdx >= vecID.size())
				return true;
	
			return false;
		}
	
#ifdef __AUTH_QUEUEING__
		void CopyAuthPacket(TPacketGDAuthLogin packet)
		{
			thecore_memcpy(&auth, &packet, sizeof(TPacketGDAuthLogin));
		}
	
		uint32_t GetLoginKey() { return loginKey_; }
		void SetLoginKey(uint32_t key) { loginKey_ = key; }
		void SetHandle(uint32_t handle) { handle_ = handle; }
#endif
	
		TPacketGDAuthLogin auth;
		TPacketGDLoginByKey login;
		TPlayerLoadPacket load;
		TLogoutPacket logout;
	
		std::vector<DWORD> vecID;
		uint32_t loginKey_, handle_;
		unsigned int vecIdx;
		bool bCheck;
};
