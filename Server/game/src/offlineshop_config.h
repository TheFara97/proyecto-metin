#ifndef __INC_METIN_II_GAME_OFFLINE_SHOP_CONFIG_H__
#define __INC_METIN_II_GAME_OFFLINE_SHOP_CONFIG_H__
#ifdef ENABLE_OFFLINE_SHOP_SYSTEM
void offlineshop_config_init();


extern BYTE g_bOfflineShopSaveTime;

extern int g_iTotalOfflineShopCount;

extern bool g_bNeedMoney_01;
extern int64_t g_dwNeedMoney_01;
extern bool g_bNeedItem_01;
extern int g_iItemVnum_01;
extern BYTE g_bItemCount_01;


extern bool g_bOfflineShopMapAllowLimit;
extern bool g_bEnableRespawnOfflineShop;
extern BYTE g_bOfflineShopSocketMax;
#endif
#endif

