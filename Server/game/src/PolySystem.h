#pragma once

#ifdef __ENABLE_POLYMORPH_SYSTEM__

class CPolymorphMgr : public singleton<CPolymorphMgr>
{
public:
	CPolymorphMgr() = default;
	~CPolymorphMgr() = default;

	// [Methods] Stage/Shop >> Related
	bool LoadPolyStage(TPolymorphStage* pStage, uint16_t wSize);
	void BuyPolymorph(CHARACTER* pChar, uint8_t bStage);


	// [Methods] Skins >> Related
	bool LoadPolySkins(TPolymorphSkin* pSkin, uint16_t wSize);
	bool IsUnlockedSkin(CHARACTER* pChar, uint8_t bySkinIndex);
	bool UnlockSkin(CHARACTER* pChar, uint8_t bySkinIndex);
	void ChangeSkin(CHARACTER* pChar, uint8_t bySkinIndex);
	uint32_t GetSkinVnum(uint8_t bySkinIndex) const;


	// [Methods] Networking >> Related
	int32_t ReceivePacket(CHARACTER* pChar, const char* c_pData, size_t uiBytes);
	void SendOpen(CHARACTER* pChar);
	void SendClose(CHARACTER* pChar);
	void SendData(CHARACTER* pChar);
	void SendUnlockSkin(CHARACTER* pChar, uint8_t bySkinIndex);
	void SendChangeSkin(CHARACTER* pChar);

private:
	std::map<uint8_t, TPolymorphStage> m_StageMap;
	std::map<uint8_t, TPolymorphSkin> m_SkinsMap;
};



#endif
