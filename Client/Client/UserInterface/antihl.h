#pragma once

#ifdef ENABLE_ANTIHL

enum eCheatDetections
{
	HLBOT = 0x188222,
	COM2,
	REX,
	MACRO,
	UNKOWN
};

class HookDet : public CSingleton<HookDet>
{
public:
	HookDet();
	virtual ~HookDet();

	void Init(HINSTANCE hInstance);
	void DetectHLHook();
	void DetectCOM();
	void DetectREX();
	void DetectReksati();
	void AppendDetection(DWORD dwDetectionID);
	DWORD GetDetectionID();
	DWORD GetSendOnClickPacketAddress() { return sendOnClickPacketAddress; }
	static LRESULT CALLBACK HookProcedure(int nCode, WPARAM wParam, LPARAM lParam);

private:
	bool HLDetected;
	bool COMDetected;
	bool REXDetected;
	static bool MacroDetected;
	HINSTANCE m_hInstance;
	DWORD sendOnClickPacketAddress = 0;
	std::vector<DWORD> m_vecDetectionIDs;
	DWORD GetRandomCleanID();
	const std::vector<DWORD> m_vecCleanIDs{ 0x1337, 0x2137, 0x5894, 0x215874, 0x235698, 0x224853 };
};

#endif
