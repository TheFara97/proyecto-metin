#include "stdafx.h"
#include <sstream>
#include "utils.h"
#include "char.h"
#include "log.h"
#include "dev_log.h"
#include "config.h"
#include "offlineshop_config.h"

#ifdef ENABLE_OFFLINE_SHOP_SYSTEM
using namespace std;

BYTE g_bOfflineShopSaveTime = 5;
int g_iTotalOfflineShopCount = 1000;

// Offline Shop Need Money
bool g_bNeedMoney_01 = false;
int64_t g_dwNeedMoney_01 = 0;
bool g_bNeedMoney_02 = false;
int64_t g_dwNeedMoney_02 = 0;
bool g_bNeedMoney_03 = false;
int64_t g_dwNeedMoney_03 = 0;
bool g_bNeedMoney_04 = false;
int64_t g_dwNeedMoney_04 = 0;
bool g_bNeedMoney_05 = false;
int64_t g_dwNeedMoney_05 = 0;
bool g_bNeedMoney_06 = false;
int64_t g_dwNeedMoney_06 = 0;
bool g_bNeedMoney_07 = false;
int64_t g_dwNeedMoney_07 = 0;
bool g_bNeedMoney_08 = false;
int64_t g_dwNeedMoney_08 = 0;
// End Of Offline Shop Need Money

// Offline Shop Need Item
bool g_bNeedItem_01 = false;
int g_iItemVnum_01 = 0;
BYTE g_bItemCount_01 = 0;
bool g_bNeedItem_02 = false;
int g_iItemVnum_02 = 0;
BYTE g_bItemCount_02 = 0;
bool g_bNeedItem_03 = false;
int g_iItemVnum_03 = 0;
BYTE g_bItemCount_03 = 0;
bool g_bNeedItem_04 = false;
int g_iItemVnum_04 = 0;
BYTE g_bItemCount_04 = 0;
bool g_bNeedItem_05 = false;
int g_iItemVnum_05 = 0;
BYTE g_bItemCount_05 = 0;
bool g_bNeedItem_06 = false;
int g_iItemVnum_06 = 0;
BYTE g_bItemCount_06 = 0;
bool g_bNeedItem_07 = false;
int g_iItemVnum_07 = 0;
BYTE g_bItemCount_07 = 0;
bool g_bNeedItem_08 = false;
int g_iItemVnum_08 = 0;
BYTE g_bItemCount_08 = 0;
// End Of Offline Shop Need Item

bool g_bOfflineShopMapAllowLimit = false;
static set<int> s_set_offlineshop_map_allows;

bool g_bEnableRespawnOfflineShop = true;
BYTE g_bOfflineShopSocketMax = 3;
bool offlineshop_map_allow_find(int mapIndex)
{
	if (g_bAuthServer)
		return false;
	
	if (s_set_offlineshop_map_allows.find(mapIndex) == s_set_offlineshop_map_allows.end())
		return false;
	
	return true;
}

void offlineshop_map_allow_add(int mapIndex)
{
	if (offlineshop_map_allow_find(mapIndex))
	{
		fprintf(stderr, "Multiple map allow detected!");
		exit(1);
	}
	
	fprintf(stdout, "OFFLINESHOP_MAP_ALLOW: %d\n", mapIndex);
	s_set_offlineshop_map_allows.insert(mapIndex);
}

void offlineshop_config_init()
{
	if (!g_bAuthServer)
	{
		FILE * file;

		char buf[256], token_string[256], value_string[256];

		if (!(file = fopen("OFFLINE_SHOP_CONFIG", "r")))
		{
			fprintf(stderr, "Can not open [OFFLINE_SHOP_CONFIG]\n");
			exit(1);
		}

		while (fgets(buf, 256, file))
		{
			parse_token(buf, token_string, value_string);

			TOKEN("OFFLINE_SHOP_SAVE_TIME")
			{
				g_bOfflineShopSaveTime = MINMAX(1, g_bOfflineShopSaveTime, 10);
				str_to_number(g_bOfflineShopSaveTime, value_string);
				sys_log(0, "OFFLINE_SHOP_SAVE_TIME: %d", g_bOfflineShopSaveTime);
			}

			TOKEN("OFFLINE_SHOP_NEED_MONEY_01")
			{
				char arg1[256];
				char arg2[256];
				two_arguments(value_string, arg1, sizeof(arg1), arg2, sizeof(arg2));

				if (!*arg1 || !*arg2)
				{
					fprintf(stderr, "OFFLINE_SHOP_NEED_MONEY_01 syntax: offline_shop_need_money <disable or enable> <money>");
					exit(1);
				}
				else if (!isnhdigit(*arg2))
				{
					fprintf(stderr, "Second argument must be integer!");
					exit(1);
				}

				if (!strcmp(arg1, "enable"))
					g_bNeedMoney_01 = true;
				else if (!strcmp(arg1, "disable"))
					g_bNeedMoney_01 = false;
				else if (isnhdigit(*arg1))
					str_to_number(g_bNeedMoney_01, arg1);
				
				// Check overflow and configure the money.
				g_dwNeedMoney_01 = MINMAX(1, g_dwNeedMoney_01, 2000000000);

				str_to_number(g_dwNeedMoney_01, arg2);
				sys_log(0, "OFFLINE_SHOP_NEED_MONEY_01: %s - Money %u", g_bNeedMoney_01 ? "Enabled" : "Disabled", g_dwNeedMoney_01);
			}

			TOKEN("OFFLINE_SHOP_NEED_MONEY_02")
			{
				char arg1[256];
				char arg2[256];
				two_arguments(value_string, arg1, sizeof(arg1), arg2, sizeof(arg2));

				if (!*arg1 || !*arg2)
				{
					fprintf(stderr, "OFFLINE_SHOP_NEED_MONEY_02 syntax: offline_shop_need_money <disable or enable> <money>");
					exit(1);
				}
				else if (!isnhdigit(*arg2))
				{
					fprintf(stderr, "Second argument must be integer!");
					exit(1);
				}

				if (!strcmp(arg1, "enable"))
					g_bNeedMoney_02 = true;
				else if (!strcmp(arg1, "disable"))
					g_bNeedMoney_02 = false;
				else if (isnhdigit(*arg1))
					str_to_number(g_bNeedMoney_02, arg1);
				
				// Check overflow and configure the money.
				g_dwNeedMoney_02 = MINMAX(1, g_dwNeedMoney_02, 2000000000);

				str_to_number(g_dwNeedMoney_02, arg2);
				sys_log(0, "OFFLINE_SHOP_NEED_MONEY_02: %s - Money %u", g_bNeedMoney_02 ? "Enabled" : "Disabled", g_dwNeedMoney_02);
			}

			TOKEN("OFFLINE_SHOP_NEED_MONEY_03")
			{
				char arg1[256];
				char arg2[256];
				two_arguments(value_string, arg1, sizeof(arg1), arg2, sizeof(arg2));

				if (!*arg1 || !*arg2)
				{
					fprintf(stderr, "OFFLINE_SHOP_NEED_MONEY_03 syntax: offline_shop_need_money <disable or enable> <money>");
					exit(1);
				}
				else if (!isnhdigit(*arg2))
				{
					fprintf(stderr, "Second argument must be integer!");
					exit(1);
				}

				if (!strcmp(arg1, "enable"))
					g_bNeedMoney_03 = true;
				else if (!strcmp(arg1, "disable"))
					g_bNeedMoney_03 = false;
				else if (isnhdigit(*arg1))
					str_to_number(g_bNeedMoney_03, arg1);
				
				// Check overflow and configure the money.
				g_dwNeedMoney_03 = MINMAX(1, g_dwNeedMoney_03, 2000000000);

				str_to_number(g_dwNeedMoney_03, arg2);
				sys_log(0, "OFFLINE_SHOP_NEED_MONEY_03: %s - Money %u", g_bNeedMoney_03 ? "Enabled" : "Disabled", g_dwNeedMoney_03);
			}

			TOKEN("OFFLINE_SHOP_NEED_MONEY_04")
			{
				char arg1[256];
				char arg2[256];
				two_arguments(value_string, arg1, sizeof(arg1), arg2, sizeof(arg2));

				if (!*arg1 || !*arg2)
				{
					fprintf(stderr, "OFFLINE_SHOP_NEED_MONEY_04 syntax: offline_shop_need_money <disable or enable> <money>");
					exit(1);
				}
				else if (!isnhdigit(*arg2))
				{
					fprintf(stderr, "Second argument must be integer!");
					exit(1);
				}

				if (!strcmp(arg1, "enable"))
					g_bNeedMoney_04 = true;
				else if (!strcmp(arg1, "disable"))
					g_bNeedMoney_04 = false;
				else if (isnhdigit(*arg1))
					str_to_number(g_bNeedMoney_04, arg1);
				
				// Check overflow and configure the money.
				g_dwNeedMoney_04 = MINMAX(1, g_dwNeedMoney_04, 2000000000);

				str_to_number(g_dwNeedMoney_04, arg2);
				sys_log(0, "OFFLINE_SHOP_NEED_MONEY_04: %s - Money %u", g_bNeedMoney_04 ? "Enabled" : "Disabled", g_dwNeedMoney_04);
			}

			TOKEN("OFFLINE_SHOP_NEED_MONEY_05")
			{
				char arg1[256];
				char arg2[256];
				two_arguments(value_string, arg1, sizeof(arg1), arg2, sizeof(arg2));

				if (!*arg1 || !*arg2)
				{
					fprintf(stderr, "OFFLINE_SHOP_NEED_MONEY_0 syntax: offline_shop_need_money <disable or enable> <money>");
					exit(1);
				}
				else if (!isnhdigit(*arg2))
				{
					fprintf(stderr, "Second argument must be integer!");
					exit(1);
				}

				if (!strcmp(arg1, "enable"))
					g_bNeedMoney_05 = true;
				else if (!strcmp(arg1, "disable"))
					g_bNeedMoney_05 = false;
				else if (isnhdigit(*arg1))
					str_to_number(g_bNeedMoney_05, arg1);
				
				// Check overflow and configure the money.
				g_dwNeedMoney_05 = MINMAX(1, g_dwNeedMoney_05, 2000000000);

				str_to_number(g_dwNeedMoney_05, arg2);
				sys_log(0, "OFFLINE_SHOP_NEED_MONEY_05: %s - Money %u", g_bNeedMoney_05 ? "Enabled" : "Disabled", g_dwNeedMoney_05);
			}

			TOKEN("OFFLINE_SHOP_NEED_MONEY_06")
			{
				char arg1[256];
				char arg2[256];
				two_arguments(value_string, arg1, sizeof(arg1), arg2, sizeof(arg2));

				if (!*arg1 || !*arg2)
				{
					fprintf(stderr, "OFFLINE_SHOP_NEED_MONEY_06 syntax: offline_shop_need_money <disable or enable> <money>");
					exit(1);
				}
				else if (!isnhdigit(*arg2))
				{
					fprintf(stderr, "Second argument must be integer!");
					exit(1);
				}

				if (!strcmp(arg1, "enable"))
					g_bNeedMoney_06 = true;
				else if (!strcmp(arg1, "disable"))
					g_bNeedMoney_06 = false;
				else if (isnhdigit(*arg1))
					str_to_number(g_bNeedMoney_06, arg1);
				
				// Check overflow and configure the money.
				g_dwNeedMoney_06 = MINMAX(1, g_dwNeedMoney_06, 2000000000);

				str_to_number(g_dwNeedMoney_06, arg2);
				sys_log(0, "OFFLINE_SHOP_NEED_MONEY_06: %s - Money %u", g_bNeedMoney_06 ? "Enabled" : "Disabled", g_dwNeedMoney_06);
			}

			TOKEN("OFFLINE_SHOP_NEED_MONEY_07")
			{
				char arg1[256];
				char arg2[256];
				two_arguments(value_string, arg1, sizeof(arg1), arg2, sizeof(arg2));

				if (!*arg1 || !*arg2)
				{
					fprintf(stderr, "OFFLINE_SHOP_NEED_MONEY_07 syntax: offline_shop_need_money <disable or enable> <money>");
					exit(1);
				}
				else if (!isnhdigit(*arg2))
				{
					fprintf(stderr, "Second argument must be integer!");
					exit(1);
				}

				if (!strcmp(arg1, "enable"))
					g_bNeedMoney_07 = true;
				else if (!strcmp(arg1, "disable"))
					g_bNeedMoney_07 = false;
				else if (isnhdigit(*arg1))
					str_to_number(g_bNeedMoney_07, arg1);
				
				// Check overflow and configure the money.
				g_dwNeedMoney_07 = MINMAX(1, g_dwNeedMoney_07, 2000000000);

				str_to_number(g_dwNeedMoney_07, arg2);
				sys_log(0, "OFFLINE_SHOP_NEED_MONEY_07: %s - Money %u", g_bNeedMoney_07 ? "Enabled" : "Disabled", g_dwNeedMoney_07);
			}

			TOKEN("OFFLINE_SHOP_NEED_MONEY_08")
			{
				char arg1[256];
				char arg2[256];
				two_arguments(value_string, arg1, sizeof(arg1), arg2, sizeof(arg2));

				if (!*arg1 || !*arg2)
				{
					fprintf(stderr, "OFFLINE_SHOP_NEED_MONEY_08 syntax: offline_shop_need_money <disable or enable> <money>");
					exit(1);
				}
				else if (!isnhdigit(*arg2))
				{
					fprintf(stderr, "Second argument must be integer!");
					exit(1);
				}

				if (!strcmp(arg1, "enable"))
					g_bNeedMoney_08 = true;
				else if (!strcmp(arg1, "disable"))
					g_bNeedMoney_08 = false;
				else if (isnhdigit(*arg1))
					str_to_number(g_bNeedMoney_08, arg1);
				
				// Check overflow and configure the money.
				g_dwNeedMoney_08 = MINMAX(1, g_dwNeedMoney_08, 2000000000);

				str_to_number(g_dwNeedMoney_08, arg2);
				sys_log(0, "OFFLINE_SHOP_NEED_MONEY_08: %s - Money %u", g_bNeedMoney_08 ? "Enabled" : "Disabled", g_dwNeedMoney_08);
			}

			TOKEN("OFFLINE_SHOP_TOTAL_COUNT")
			{
				str_to_number(g_iTotalOfflineShopCount, value_string);
				sys_log(0, "OFFLINE_SHOP_TOTAL_COUNT: %d", g_iTotalOfflineShopCount);
			}

			TOKEN("OFFLINE_SHOP_NEED_ITEM_01")
			{
				char arg1[256];
				char arg2[256];
				char arg3[256];
				three_arguments(value_string, arg1, sizeof(arg1), arg2, sizeof(arg2), arg3, sizeof(arg3));

				if (!*arg1 || !*arg2 || !*arg3)
				{
					fprintf(stderr, "OFFLINE_SHOP_NEED_ITEM_01 syntax: offline_shop_need_item <enable or disable> <item_vnum> <item_count>");
					exit(1);
				}
				else if (!isnhdigit(*arg2) || !isnhdigit(*arg3))
				{
					fprintf(stderr, "Second argument and third argument must be integer!");
					exit(1);
				}

				if (!strcmp(arg1, "enable"))
					g_bNeedItem_01 = true;
				else if (!strcmp(arg1, "disable"))
					g_bNeedItem_01 = false;
				else if (isnhdigit(*arg1))
					str_to_number(g_bNeedItem_01, arg1);
				
				
				// Item count can be maximum 200.
				g_bItemCount_01 = MINMAX(1, g_bItemCount_01, ITEM_MAX_COUNT);

				str_to_number(g_iItemVnum_01, arg2);
				str_to_number(g_bItemCount_01, arg3);
				sys_log(0, "OFFLINE_SHOP_NEED_ITEM_01: %s %d %d", g_bNeedItem_01 ? "Enabled" : "Disabled", g_iItemVnum_01, g_bItemCount_01);
			}

			TOKEN("OFFLINE_SHOP_NEED_ITEM_02")
			{
				char arg1[256];
				char arg2[256];
				char arg3[256];
				three_arguments(value_string, arg1, sizeof(arg1), arg2, sizeof(arg2), arg3, sizeof(arg3));

				if (!*arg1 || !*arg2 || !*arg3)
				{
					fprintf(stderr, "OFFLINE_SHOP_NEED_ITEM_02 syntax: offline_shop_need_item <enable or disable> <item_vnum> <item_count>");
					exit(1);
				}
				else if (!isnhdigit(*arg2) || !isnhdigit(*arg3))
				{
					fprintf(stderr, "Second argument and third argument must be integer!");
					exit(1);
				}

				if (!strcmp(arg1, "enable"))
					g_bNeedItem_02 = true;
				else if (!strcmp(arg1, "disable"))
					g_bNeedItem_02 = false;
				else if (isnhdigit(*arg1))
					str_to_number(g_bNeedItem_02, arg1);
				
				
				// Item count can be maximum 200.
				g_bItemCount_02 = MINMAX(1, g_bItemCount_02, ITEM_MAX_COUNT);

				str_to_number(g_iItemVnum_02, arg2);
				str_to_number(g_bItemCount_02, arg3);
				sys_log(0, "OFFLINE_SHOP_NEED_ITEM_02: %s %d %d", g_bNeedItem_02 ? "Enabled" : "Disabled", g_iItemVnum_02, g_bItemCount_02);
			}

			TOKEN("OFFLINE_SHOP_NEED_ITEM_03")
			{
				char arg1[256];
				char arg2[256];
				char arg3[256];
				three_arguments(value_string, arg1, sizeof(arg1), arg2, sizeof(arg2), arg3, sizeof(arg3));

				if (!*arg1 || !*arg2 || !*arg3)
				{
					fprintf(stderr, "OFFLINE_SHOP_NEED_ITEM_03 syntax: offline_shop_need_item <enable or disable> <item_vnum> <item_count>");
					exit(1);
				}
				else if (!isnhdigit(*arg2) || !isnhdigit(*arg3))
				{
					fprintf(stderr, "Second argument and third argument must be integer!");
					exit(1);
				}

				if (!strcmp(arg1, "enable"))
					g_bNeedItem_03 = true;
				else if (!strcmp(arg1, "disable"))
					g_bNeedItem_03 = false;
				else if (isnhdigit(*arg1))
					str_to_number(g_bNeedItem_03, arg1);
				
				
				// Item count can be maximum 200.
				g_bItemCount_03 = MINMAX(1, g_bItemCount_03, ITEM_MAX_COUNT);

				str_to_number(g_iItemVnum_03, arg2);
				str_to_number(g_bItemCount_03, arg3);
				sys_log(0, "OFFLINE_SHOP_NEED_ITEM_03: %s %d %d", g_bNeedItem_03 ? "Enabled" : "Disabled", g_iItemVnum_03, g_bItemCount_03);
			}

			TOKEN("OFFLINE_SHOP_NEED_ITEM_04")
			{
				char arg1[256];
				char arg2[256];
				char arg3[256];
				three_arguments(value_string, arg1, sizeof(arg1), arg2, sizeof(arg2), arg3, sizeof(arg3));

				if (!*arg1 || !*arg2 || !*arg3)
				{
					fprintf(stderr, "OFFLINE_SHOP_NEED_ITEM_04 syntax: offline_shop_need_item <enable or disable> <item_vnum> <item_count>");
					exit(1);
				}
				else if (!isnhdigit(*arg2) || !isnhdigit(*arg3))
				{
					fprintf(stderr, "Second argument and third argument must be integer!");
					exit(1);
				}

				if (!strcmp(arg1, "enable"))
					g_bNeedItem_04 = true;
				else if (!strcmp(arg1, "disable"))
					g_bNeedItem_04 = false;
				else if (isnhdigit(*arg1))
					str_to_number(g_bNeedItem_04, arg1);
				
				
				// Item count can be maximum 200.
				g_bItemCount_04 = MINMAX(1, g_bItemCount_04, ITEM_MAX_COUNT);

				str_to_number(g_iItemVnum_04, arg2);
				str_to_number(g_bItemCount_04, arg3);
				sys_log(0, "OFFLINE_SHOP_NEED_ITEM_04: %s %d %d", g_bNeedItem_04 ? "Enabled" : "Disabled", g_iItemVnum_04, g_bItemCount_04);
			}

			TOKEN("OFFLINE_SHOP_NEED_ITEM_05")
			{
				char arg1[256];
				char arg2[256];
				char arg3[256];
				three_arguments(value_string, arg1, sizeof(arg1), arg2, sizeof(arg2), arg3, sizeof(arg3));

				if (!*arg1 || !*arg2 || !*arg3)
				{
					fprintf(stderr, "OFFLINE_SHOP_NEED_ITEM_05 syntax: offline_shop_need_item <enable or disable> <item_vnum> <item_count>");
					exit(1);
				}
				else if (!isnhdigit(*arg2) || !isnhdigit(*arg3))
				{
					fprintf(stderr, "Second argument and third argument must be integer!");
					exit(1);
				}

				if (!strcmp(arg1, "enable"))
					g_bNeedItem_05 = true;
				else if (!strcmp(arg1, "disable"))
					g_bNeedItem_05 = false;
				else if (isnhdigit(*arg1))
					str_to_number(g_bNeedItem_05, arg1);
				
				
				// Item count can be maximum 200.
				g_bItemCount_05 = MINMAX(1, g_bItemCount_05, ITEM_MAX_COUNT);

				str_to_number(g_iItemVnum_05, arg2);
				str_to_number(g_bItemCount_05, arg3);
				sys_log(0, "OFFLINE_SHOP_NEED_ITEM_05: %s %d %d", g_bNeedItem_05 ? "Enabled" : "Disabled", g_iItemVnum_05, g_bItemCount_05);
			}

			TOKEN("OFFLINE_SHOP_NEED_ITEM_06")
			{
				char arg1[256];
				char arg2[256];
				char arg3[256];
				three_arguments(value_string, arg1, sizeof(arg1), arg2, sizeof(arg2), arg3, sizeof(arg3));

				if (!*arg1 || !*arg2 || !*arg3)
				{
					fprintf(stderr, "OFFLINE_SHOP_NEED_ITEM_06 syntax: offline_shop_need_item <enable or disable> <item_vnum> <item_count>");
					exit(1);
				}
				else if (!isnhdigit(*arg2) || !isnhdigit(*arg3))
				{
					fprintf(stderr, "Second argument and third argument must be integer!");
					exit(1);
				}

				if (!strcmp(arg1, "enable"))
					g_bNeedItem_06 = true;
				else if (!strcmp(arg1, "disable"))
					g_bNeedItem_06 = false;
				else if (isnhdigit(*arg1))
					str_to_number(g_bNeedItem_06, arg1);
				
				
				// Item count can be maximum 200.
				g_bItemCount_06 = MINMAX(1, g_bItemCount_06, ITEM_MAX_COUNT);

				str_to_number(g_iItemVnum_06, arg2);
				str_to_number(g_bItemCount_06, arg3);
				sys_log(0, "OFFLINE_SHOP_NEED_ITEM_06: %s %d %d", g_bNeedItem_06 ? "Enabled" : "Disabled", g_iItemVnum_06, g_bItemCount_06);
			}

			TOKEN("OFFLINE_SHOP_NEED_ITEM_07")
			{
				char arg1[256];
				char arg2[256];
				char arg3[256];
				three_arguments(value_string, arg1, sizeof(arg1), arg2, sizeof(arg2), arg3, sizeof(arg3));

				if (!*arg1 || !*arg2 || !*arg3)
				{
					fprintf(stderr, "OFFLINE_SHOP_NEED_ITEM_07 syntax: offline_shop_need_item <enable or disable> <item_vnum> <item_count>");
					exit(1);
				}
				else if (!isnhdigit(*arg2) || !isnhdigit(*arg3))
				{
					fprintf(stderr, "Second argument and third argument must be integer!");
					exit(1);
				}

				if (!strcmp(arg1, "enable"))
					g_bNeedItem_07 = true;
				else if (!strcmp(arg1, "disable"))
					g_bNeedItem_07 = false;
				else if (isnhdigit(*arg1))
					str_to_number(g_bNeedItem_07, arg1);
				
				
				// Item count can be maximum 200.
				g_bItemCount_07 = MINMAX(1, g_bItemCount_07, ITEM_MAX_COUNT);

				str_to_number(g_iItemVnum_07, arg2);
				str_to_number(g_bItemCount_07, arg3);
				sys_log(0, "OFFLINE_SHOP_NEED_ITEM_07: %s %d %d", g_bNeedItem_07 ? "Enabled" : "Disabled", g_iItemVnum_07, g_bItemCount_07);
			}

			TOKEN("OFFLINE_SHOP_NEED_ITEM_08")
			{
				char arg1[256];
				char arg2[256];
				char arg3[256];
				three_arguments(value_string, arg1, sizeof(arg1), arg2, sizeof(arg2), arg3, sizeof(arg3));

				if (!*arg1 || !*arg2 || !*arg3)
				{
					fprintf(stderr, "OFFLINE_SHOP_NEED_ITEM_08 syntax: offline_shop_need_item <enable or disable> <item_vnum> <item_count>");
					exit(1);
				}
				else if (!isnhdigit(*arg2) || !isnhdigit(*arg3))
				{
					fprintf(stderr, "Second argument and third argument must be integer!");
					exit(1);
				}

				if (!strcmp(arg1, "enable"))
					g_bNeedItem_08 = true;
				else if (!strcmp(arg1, "disable"))
					g_bNeedItem_08 = false;
				else if (isnhdigit(*arg1))
					str_to_number(g_bNeedItem_08, arg1);
				
				
				// Item count can be maximum 200.
				g_bItemCount_08 = MINMAX(1, g_bItemCount_08, ITEM_MAX_COUNT);

				str_to_number(g_iItemVnum_08, arg2);
				str_to_number(g_bItemCount_08, arg3);
				sys_log(0, "OFFLINE_SHOP_NEED_ITEM_08: %s %d %d", g_bNeedItem_08 ? "Enabled" : "Disabled", g_iItemVnum_08, g_bItemCount_08);
			}

			TOKEN("OFFLINE_SHOP_MAP_ALLOW_LIMIT")
			{
				char arg1[256];
				one_argument(value_string, arg1, sizeof(arg1));

				if (!*arg1)
				{
					fprintf(stderr, "OFFLINE_SHOP_MAP_ALLOW_LIMIT syntax: offline_shop_map_allow_limit <enable or disable> or < 0 or 1 >");
					exit(1);
				}

				if (!strcmp(arg1, "enable"))
					g_bOfflineShopMapAllowLimit = true;
				else if(!strcmp(arg1, "disable"))
					g_bOfflineShopMapAllowLimit = false;
				else if(isnhdigit(*arg1))
					str_to_number(g_bOfflineShopMapAllowLimit, arg1);
			}

			TOKEN("OFFLINE_SHOP_MAP_ALLOW")
			{
				if (!g_bOfflineShopMapAllowLimit)
				{
					fprintf(stderr, "OFFLINE_SHOP_MAP_ALLOW_LIMIT must be enable for this option!");
					exit(1);
				}

				char * p = value_string;
				string stNum;

				for(; *p; p++)
				{
					if (isnhspace(*p))
					{
						if (stNum.length())
						{
							int index = 0;
							str_to_number(index, stNum.c_str());
							offlineshop_map_allow_add(index);
							stNum.clear();
						}
					}
					else
						stNum += *p;
				}

				if (stNum.length())
				{
					int index = 0;
					str_to_number(index, stNum.c_str());
					offlineshop_map_allow_add(index);
				}

				continue;
			}
			
			TOKEN("OFFLINE_SHOP_RESPAWN")
			{
				char arg1[256];
				one_argument(value_string, arg1, sizeof(arg1));
				
				if (!*arg1)
				{
					fprintf(stderr, "OFFLINE_SHOP_RESPAWN syntax : offline_shop_respawn: <string or integer>");
					exit(1);
				}
				
				if (!strcmp(arg1, "enable"))
					g_bEnableRespawnOfflineShop = true;
				else if (!strcmp(arg1, "disable"))
					g_bEnableRespawnOfflineShop = false;
				else if (isnhdigit(*arg1))
					str_to_number(g_bEnableRespawnOfflineShop, value_string);
			}
			
			TOKEN("OFFLINE_SHOP_SOCKET_MAX")
			{
				str_to_number(g_bOfflineShopSocketMax, value_string);
				g_bOfflineShopSocketMax = MINMAX(3, g_bOfflineShopSocketMax, 3);
				fprintf(stderr, "OFFLINE_SHOP_SOCKET_MAX: %d", g_bOfflineShopSocketMax);				
			}
		}
	}
}
#endif

