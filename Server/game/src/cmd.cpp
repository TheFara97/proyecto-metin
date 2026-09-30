#include "stdafx.h"
#include "utils.h"
#include "config.h"
#include "char.h"
#include "locale_service.h"
#include "log.h"
#include "desc.h"
#include "../../common/PulseManager.h"
// #define ENABLE_BLOCK_CMD_SHORTCUT
#ifdef __TEAM_MEMBER_STATUS__
ACMD(do_team_list_reload);
#endif
ACMD(do_free_regen);
#ifdef _ENABLE_BATTLEPASS_
ACMD(do_battle_pass);
ACMD(do_battle_pass_admin);
#endif
#ifdef ENABLE_VOTE4BUFF
ACMD(do_vote4buff);
#endif
#ifdef __AUTO_SKILL_READER__
ACMD(do_auto_skill_reader);
#endif
#ifdef ENABLE_MULTI_FARM_BLOCK
ACMD(do_multi_farm);
#endif

#ifdef ENABLE_HWID_SYSTEM
ACMD(do_banpanel);
#endif
#ifdef __DUNGEON_INFO__
ACMD(do_dungeon_info);
#endif
#ifdef ENABLE_AUTO_SELECT_SKILL
ACMD(do_skillauto);
#endif
ACMD(do_guild_add_gold);
#ifdef __PENDANT__
ACMD(do_pendant);
#endif
#ifdef ENABLE_TITLE_ACHIEVEMENT_SYSTEM
ACMD(do_load_title_achievement);
ACMD(do_change_title_achievement);
ACMD(do_change_title_achievement_premium);
#endif
#ifdef ENABLE_MOUNT_SYSTEM
ACMD(do_user_horse_ride);
ACMD(do_user_horse_back);
#endif
#ifdef ENABLE_AFFECT_BUFF_REMOVE
ACMD(do_remove_buff);
#endif
#ifdef ENABLE_WHEEL_OF_FORTUNE
ACMD(do_wheel_of_fortune);
ACMD(do_wheel_of_fortune_gold);
#endif
#ifdef ENABLE_NOTIFICATION_SYSTEM
ACMD(do_test_notification);
#endif
ACMD(do_user_horse_feed);
// ADD_COMMAND_SLOW_STUN
ACMD(do_slow);
ACMD(do_stun);
ACMD(do_teleport);
// END_OF_ADD_COMMAND_SLOW_STUN
ACMD(do_fish_wiki);

#ifdef ENABLE_SKILL_SELECT_FEATURE
ACMD(do_selectskill_open);
ACMD(do_selectskill_select);
#endif
#ifdef ENABLE_PVP_RANKING
ACMD(do_pvp_ranking);
#endif
#ifdef ENABLE_EVENT_MANAGER
ACMD(do_event_manager);
#endif
#ifdef __SPIN_WHEEL__
ACMD(do_spin_wheel);
#endif
#ifdef ENABLE_GET_TOTAL_ONLINE_COUNT
ACMD(do_get_total_online);
#endif
#ifdef ENABLE_BOT_CONTROL
ACMD(do_bot_control);
ACMD(do_bot_control_open);
#endif

#ifdef __DUNGEON_INFO_ENABLE__
ACMD(do_dungeon_info_open_panel);
ACMD(do_dungeon_info_join_dungeon);
ACMD(do_dungeon_info_reset_cooldown_dungeon);
#ifdef __DUNGEON_RETURN_ENABLE__
ACMD(do_dungeon_info_rejoin_dungeon);
#endif
#endif

#ifdef ENABLE_COLLECT_WINDOW
ACMD(do_choose_quest);
ACMD(do_open_collect_window);
#endif
#ifdef ENABLE_NOTIFICATION_SYSTEM
ACMD(do_enable_mission_notifications);
#endif
#ifdef ENABLE_OFFLINE_SHOP_SYSTEM
ACMD(do_open_offline_shop);
ACMD(do_offline_shop_open_panel);
ACMD(do_offline_shop_open_logs);
#endif

#ifdef ENABLE_DRAGON_SOUL_TIME_RENEWAL
ACMD(do_renew_alchemy);
#endif

#ifdef ENABLE_OFFLINE_SHOP_USE_CHEQUE
ACMD(do_shop_cheque);
#endif

#ifdef __CASKET_PREVIEW_ENABLE__
	ACMD(do_fetch_casket_items);
#endif


#ifdef ENABLE_HIDE_COSTUME_SYSTEM
ACMD(do_hide_costume);
#endif

#ifdef ENABLE_ASLAN_BUFF_NPC_SYSTEM
ACMD(do_buff_set_skill);
#endif

#ifdef ENABLE_SORT_INVENTORY_ITEMS
ACMD(do_sort_inventory);
#ifdef ENABLE_SPLIT_INVENTORY_SYSTEM
ACMD(do_sort_special_inventory);
#endif
#endif

#ifdef ENABLE_SPLIT_INVENTORY_SYSTEM
ACMD(do_bezposrednie_wpadanie_test);
#endif

ACMD(do_give_item);

#ifdef ENABLE_NEW_STONE_DETACH
ACMD(do_detach_stone);
#endif

ACMD(do_dungeons);

#ifdef TITLE_SYSTEM_BYLUZER
ACMD(do_detach_title);
#endif

#ifdef ENABLE_VS_SHOP_SEARCH
ACMD(do_search_in_shops);
ACMD(do_clear_shopsearch);
#endif
// ACMD(do_fast_opener);

ACMD(do_add_attribute);

ACMD(do_warp);
ACMD(do_goto);
ACMD(do_item);
ACMD(do_mob);
ACMD(do_mob_ld);
ACMD(do_mob_aggresive);
ACMD(do_mob_coward);
ACMD(do_mob_map);
ACMD(do_purge);
ACMD(do_drop);
#ifdef ENABLE_AFFECT_POLYMORPH_REMOVE
ACMD(do_remove_polymorph);
#endif
#ifdef ENABLE_SKILL_AFFECT_REMOVE
ACMD(do_remove_skill_affect);
#endif
ACMD(do_weaken);
ACMD(do_item_purge);
ACMD(do_state);
ACMD(do_notice);
ACMD(do_map_notice);
ACMD(do_big_notice);
#ifdef ENABLE_FULL_NOTICE
ACMD(do_notice_test);
ACMD(do_big_notice_test);
ACMD(do_map_big_notice);
#endif
ACMD(do_who);
ACMD(do_user);
ACMD(do_disconnect);
ACMD(do_kill);
ACMD(do_emotion_allow);
ACMD(do_emotion);
ACMD(do_transfer);
ACMD(do_set);
ACMD(do_cmd);
ACMD(do_reset);
ACMD(do_greset);
ACMD(do_mount);
ACMD(do_fishing);
ACMD(do_refine_rod);

// REFINE_PICK
ACMD(do_max_pick);
ACMD(do_refine_pick);
// END_OF_REFINE_PICK

//ACMD(do_fishing_simul);
ACMD(do_console);
ACMD(do_restart);
ACMD(do_advance);
ACMD(do_stat);
ACMD(do_respawn);
ACMD(do_skillup);
ACMD(do_pvp);
ACMD(do_point_reset);
ACMD(do_safebox_size);
ACMD(do_safebox_close);
ACMD(do_safebox_password);
ACMD(do_safebox_change_password);
ACMD(do_mall_password);
ACMD(do_mall_close);
ACMD(do_ungroup);
ACMD(do_makeguild);
ACMD(do_deleteguild);
ACMD(do_shutdown);
ACMD(do_group);
ACMD(do_group_random);
ACMD(do_invisibility);
ACMD(do_event_flag);
ACMD(do_get_event_flag);
ACMD(do_private);
ACMD(do_qf);
ACMD(do_clear_quest);
ACMD(do_book);
ACMD(do_reload);
ACMD(do_war);
ACMD(do_nowar);
ACMD(do_setskill);
ACMD(do_setskillother);
ACMD(do_level);
ACMD(do_polymorph);
ACMD(do_polymorph_item);
/*
   ACMD(do_b1);
   ACMD(do_b2);
   ACMD(do_b3);
   ACMD(do_b4);
   ACMD(do_b5);
   ACMD(do_b6);
   ACMD(do_b7);
 */
ACMD(do_close_shop);
ACMD(do_set_walk_mode);
ACMD(do_set_run_mode);
ACMD(do_set_skill_group);
ACMD(do_set_skill_point);
ACMD(do_cooltime);
ACMD(do_detaillog);
ACMD(do_monsterlog);

ACMD(do_gwlist);
ACMD(do_stop_guild_war);
ACMD(do_cancel_guild_war);
ACMD(do_guild_state);

ACMD(do_pkmode);
ACMD(do_mobile);
ACMD(do_mobile_auth);
ACMD(do_messenger_auth);

ACMD(do_getqf);
ACMD(do_setqf);
ACMD(do_delqf);
ACMD(do_set_state);

ACMD(do_forgetme);
ACMD(do_aggregate);
ACMD(do_attract_ranger);
ACMD(do_pull_monster);
ACMD(do_setblockmode);
ACMD(do_priv_empire);
ACMD(do_priv_guild);
ACMD(do_mount_test);
ACMD(do_unmount);
ACMD(do_observer);
ACMD(do_observer_exit);
ACMD(do_socket_item);
ACMD(do_xmas);
ACMD(do_stat_minus);
ACMD(do_stat_reset);
ACMD(do_view_equip);
ACMD(do_block_chat);
ACMD(do_vote_block_chat);

// BLOCK_CHAT
ACMD(do_block_chat_list);
// END_OF_BLOCK_CHAT

ACMD(do_party_request);
ACMD(do_party_request_deny);
ACMD(do_party_request_accept);
ACMD(do_build);
ACMD(do_clear_land);

ACMD(do_horse_state);
ACMD(do_horse_level);
ACMD(do_horse_ride);
ACMD(do_horse_summon);
ACMD(do_horse_unsummon);
ACMD(do_horse_set_stat);

ACMD(do_save_attribute_to_image);
ACMD(do_affect_remove);

ACMD(do_change_attr);
ACMD(do_add_attr);
ACMD(do_add_socket);

ACMD(do_inputall)
{
	ch->ChatPacket(CHAT_TYPE_INFO, "[LS;916]");
}

ACMD(do_show_arena_list);
ACMD(do_end_all_duel);
ACMD(do_end_duel);
ACMD(do_duel);

ACMD(do_stat_plus_amount);

ACMD(do_break_marriage);

ACMD(do_oxevent_show_quiz);
ACMD(do_oxevent_log);
ACMD(do_oxevent_get_attender);

ACMD(do_effect);
ACMD(do_threeway_war_info );
ACMD(do_threeway_war_myinfo );
//

ACMD(do_monarch_warpto);
ACMD(do_monarch_transfer);
ACMD(do_monarch_info);
ACMD(do_elect);
ACMD(do_monarch_tax);
ACMD(do_monarch_mob);
ACMD(do_monarch_notice);

ACMD(do_rmcandidacy);
ACMD(do_setmonarch);
ACMD(do_rmmonarch);

ACMD(do_hair);
//gift notify quest command
ACMD(do_gift);

ACMD(do_inventory);
ACMD(do_cube);
#ifdef ENABLE_MOUNT_COSTUME_SYSTEM
ACMD(do_horseshoe);
#endif
#ifdef ENABLE_ODLAMKI_SYSTEM
ACMD(do_odlamki);
#endif
#ifdef ENABLE_SYSTEMY_KARTY_OKEY
ACMD(do_cards);
#endif

ACMD(do_siege);
ACMD(do_temp);
ACMD(do_frog);

ACMD(do_check_monarch_money);

ACMD(do_reset_subskill );
ACMD(do_flush);

ACMD(do_eclipse);

ACMD(do_event_helper);

ACMD(do_in_game_mall);

ACMD(do_get_mob_count);

ACMD(do_dice);
ACMD(do_special_item);

ACMD(do_click_mall);
ACMD(do_click_safebox);

ACMD(do_ride);
ACMD(do_get_item_id_list);
ACMD(do_set_socket);

ACMD(do_costume);
ACMD(do_set_stat);

ACMD (do_can_dead);

ACMD (do_full_set);

ACMD (do_item_full_set);

ACMD (do_attr_full_set);
ACMD (do_attr_full_set_int);
ACMD (do_attr_full_set_poh);
ACMD (do_attr_full_set_hu_prisery);
ACMD (do_attr_pvp_warrior);
ACMD (do_attr_pvp_sura_bm);
ACMD (do_attr_pvp_sura_wp);
ACMD (do_attr_pvp_shaman);
ACMD (do_pvm_skill_damage_magic);
ACMD (do_pvm_skill_damage_melee);
ACMD (do_pvm_average_damage);


ACMD (do_attr_pvm_warrior);
ACMD (do_attr_pvm_sura_bm);
ACMD (do_attr_pvm_sura_wp);
ACMD (do_attr_pvm_shaman);

ACMD (do_all_skill_master);

ACMD (do_use_item);
ACMD (do_dragon_soul);
ACMD(do_open_shop);
#ifdef __ENABLE_COLLECTIONS_SYSTEM__
	ACMD(do_add_collect_item);
#endif
#ifdef __VIP_SYSTEM__
ACMD(do_convert_vip_affect);
#endif
#ifdef ENABLE_MULTI_LANGUAGE_SYSTEM
ACMD(do_language);
#endif
ACMD (do_ds_list);
ACMD (do_clear_affect);
#ifdef DUNGEON_CARDS
ACMD(do_dungeon_cards);
#endif
#ifdef ENABLE_PROMO_CODE_SYSTEM
ACMD(do_promo_code_system);
ACMD(do_promo_code_open);
#endif

#ifdef ENABLE_NEWSTUFF
ACMD(do_change_rare_attr);
ACMD(do_add_rare_attr);

ACMD(do_force_logout);

ACMD(do_poison);
ACMD(do_rewarp);
#endif
#ifdef ENABLE_WOLFMAN_CHARACTER
ACMD(do_bleeding);
#endif
#ifdef ENABLE_POLY_SHOP
ACMD(do_buy_marble);
#endif
#ifdef ENABLE_NEW_PET_SYSTEM
ACMD(do_pet_skillup);
ACMD(do_pet_name);
#endif

ACMD(do_stat_val);
ACMD(do_toggle_antyexp);

#ifdef __PREMIUM_PRIVATE_SHOP__
ACMD(do_register_off_shop);
#endif
#ifdef __INVENTORY_BUFFERING__
	ACMD(do_quick_open);
#endif
#ifdef ENABLE_OFFLINE_SHOP
ACMD(do_offshop_change_shop_name);
ACMD(do_offshop_force_close_shop);
#endif
#ifdef ENABLE_ENLIGHTENMENT_SYSTEM
ACMD(do_enlight);
#endif

#ifdef __WORLD_BOSS_YUMA__
ACMD(do_boss_debug);
#endif

ACMD(do_refund_items);
ACMD(do_guild_dungeon);
ACMD(do_guild_guard_items);
ACMD(do_guild_donate_material);
ACMD(do_guild_view_storage);
ACMD(do_guild_upgrade_bonus);
ACMD(do_guildskillup);
ACMD(do_world_boss_request);
ACMD(do_world_boss_teleport);
#ifdef ENABLE_ITEMSHOP
ACMD(do_ishop);
#endif

struct command_info cmd_info[] =
{
	{ "!RESERVED!",	NULL,			0,			POS_DEAD,	GM_IMPLEMENTOR	},
#ifdef ENABLE_OFFLINE_SHOP_USE_CHEQUE
	{ "shop_cheque",		do_shop_cheque,				0,			POS_DEAD,	GM_PLAYER	},
#endif
#ifdef ENABLE_ASLAN_BUFF_NPC_SYSTEM
	{ "buff_skill",	do_buff_set_skill,	0,	POS_DEAD,	GM_IMPLEMENTOR },
#endif
	{ "free_regens", do_free_regen,	0,			POS_DEAD,	GM_IMPLEMENTOR	},
	{ "who",		do_who,			0,			POS_DEAD,	GM_IMPLEMENTOR	},
#ifdef ENABLE_AFFECT_POLYMORPH_REMOVE
	{ "remove_polymorph",	do_remove_polymorph,		0,			POS_DEAD,	GM_PLAYER	},
#endif
#ifdef ENABLE_SKILL_AFFECT_REMOVE
	{"remove_skill_affect", do_remove_skill_affect, 0, POS_DEAD, GM_PLAYER},
#endif
	{ "war",		do_war,			0,			POS_DEAD,	GM_PLAYER	},
	{ "warp",		do_warp,		0,			POS_DEAD,	GM_LOW_WIZARD	},
	{ "user",		do_user,		0,			POS_DEAD,	GM_IMPLEMENTOR	},
	{ "notice",		do_notice,		0,			POS_DEAD,	GM_IMPLEMENTOR	},
	{ "notice_map",	do_map_notice,	0,			POS_DEAD,	GM_IMPLEMENTOR	},
	{ "big_notice",	do_big_notice,	0,			POS_DEAD,	GM_IMPLEMENTOR	},
#ifdef ENABLE_FULL_NOTICE
	{ "big_notice_map",	do_map_big_notice,	0,	POS_DEAD,	GM_HIGH_WIZARD	},
	{ "notice_test",	do_notice_test,		0,	POS_DEAD,	GM_HIGH_WIZARD	},
	{ "big_notice_test",do_big_notice_test,	0,	POS_DEAD,	GM_HIGH_WIZARD	},
#endif
#ifdef ENABLE_SORT_INVENTORY_ITEMS
	//{ "sort_inventory", do_sort_inventory, 0, POS_DEAD, GM_PLAYER },
#ifdef ENABLE_SPLIT_INVENTORY_SYSTEM
	//{ "sort_special_inventory", do_sort_special_inventory, 0, POS_DEAD, GM_PLAYER },
#endif
#ifdef ENABLE_SPLIT_INVENTORY_SYSTEM
	{ "extended_inventory_status", do_bezposrednie_wpadanie_test, 0, POS_DEAD, GM_PLAYER },
#endif
#endif
	{ "give_item",	do_give_item,	0,	POS_DEAD,	GM_IMPLEMENTOR	},
#ifdef TITLE_SYSTEM_BYLUZER
	{ "detach_title",	do_detach_title,	0,	POS_DEAD,	GM_PLAYER	},
#endif
#ifdef ENABLE_OFFLINE_SHOP
	{ "offshop_change_shop_name", do_offshop_change_shop_name, 0,  POS_DEAD, GM_IMPLEMENTOR },
	{ "offshop_force_close_shop", do_offshop_force_close_shop, 0,  POS_DEAD, GM_IMPLEMENTOR },
#endif
	{ "drop_simul",		do_drop,		0,			POS_DEAD,	GM_IMPLEMENTOR	},
	{ "nowar",		do_nowar,		0,			POS_DEAD,	GM_PLAYER	},
	{ "purge",		do_purge,		0,			POS_DEAD,	GM_IMPLEMENTOR	},
	{ "weaken",		do_weaken,		0,			POS_DEAD,	GM_IMPLEMENTOR		},
	{ "dc",		do_disconnect,		0,			POS_DEAD,	GM_IMPLEMENTOR	},
	{ "transfer",	do_transfer,		0,			POS_DEAD,	GM_LOW_WIZARD	},
	{ "goto",		do_goto,		0,			POS_DEAD,	GM_LOW_WIZARD	},
	{ "level",		do_level,		0,			POS_DEAD,	GM_LOW_WIZARD	},
	{ "eventflag",	do_event_flag,		0,			POS_DEAD,	GM_IMPLEMENTOR	},
	{ "geteventflag",	do_get_event_flag,	0,			POS_DEAD,	GM_IMPLEMENTOR	},

	{ "item",		do_item,		0,			POS_DEAD,	GM_IMPLEMENTOR		},

	{ "mob",		do_mob,			0,			POS_DEAD,	GM_IMPLEMENTOR	},
	{ "mob_ld",		do_mob_ld,			0,			POS_DEAD,	GM_IMPLEMENTOR	},
	{ "ma",		do_mob_aggresive,	0,			POS_DEAD,	GM_IMPLEMENTOR	},
	{ "mc",		do_mob_coward,		0,			POS_DEAD,	GM_IMPLEMENTOR	},
	{ "mm",		do_mob_map,		0,			POS_DEAD,	GM_IMPLEMENTOR	},
	{ "kill",		do_kill,		0,			POS_DEAD,	GM_IMPLEMENTOR	},
	{ "ipurge",		do_item_purge,		0,			POS_DEAD,	GM_IMPLEMENTOR	},
	{ "group",		do_group,		0,			POS_DEAD,	GM_IMPLEMENTOR	},
	{ "grrandom",	do_group_random,	0,			POS_DEAD,	GM_IMPLEMENTOR	},

	{ "set",		do_set,			0,			POS_DEAD,	GM_IMPLEMENTOR	},
	{ "reset",		do_reset,		0,			POS_DEAD,	GM_IMPLEMENTOR	},
	{ "greset",		do_greset,		0,			POS_DEAD,	GM_IMPLEMENTOR	},
	{ "advance",	do_advance,		0,			POS_DEAD,	GM_LOW_WIZARD		},
	{ "book",		do_book,		0,			POS_DEAD,	GM_IMPLEMENTOR  },

	{ "console",	do_console,		0,			POS_DEAD,	GM_IMPLEMENTOR	},

	{ "shutdow",	do_inputall,		0,			POS_DEAD,	GM_IMPLEMENTOR	},
	{ "shutdown",	do_shutdown,		0,			POS_DEAD,	GM_IMPLEMENTOR	},

	{ "stat",		do_stat,		0,			POS_DEAD,	GM_PLAYER	},
	{ "stat-",		do_stat_minus,		0,			POS_DEAD,	GM_PLAYER	},
	{ "stat_reset",	do_stat_reset,		0,			POS_DEAD,	GM_LOW_WIZARD	},
	{ "state",		do_state,		0,			POS_DEAD,	GM_LOW_WIZARD	},

	// ADD_COMMAND_SLOW_STUN
	{ "stun",		do_stun,		0,			POS_DEAD,	GM_LOW_WIZARD	},
	{ "slow",		do_slow,		0,			POS_DEAD,	GM_LOW_WIZARD	},
	// END_OF_ADD_COMMAND_SLOW_STUN

	{ "respawn",	do_respawn,		0,			POS_DEAD,	GM_IMPLEMENTOR	},

	{ "makeguild",	do_makeguild,		0,			POS_DEAD,	GM_IMPLEMENTOR	},
	{ "deleteguild",	do_deleteguild,		0,			POS_DEAD,	GM_IMPLEMENTOR	},

	{ "mount",		do_mount,		0,			POS_MOUNTING,	GM_PLAYER	},
	{ "restart_here",	do_restart,		SCMD_RESTART_HERE,	POS_DEAD,	GM_PLAYER	},
	{ "restart_town",	do_restart,		SCMD_RESTART_TOWN,	POS_DEAD,	GM_PLAYER	},
	{ "phase_selec",	do_inputall,		0,			POS_DEAD,	GM_PLAYER	},
	{ "phase_select",	do_cmd,			SCMD_PHASE_SELECT,	POS_DEAD,	GM_PLAYER	},
	{ "qui",		do_inputall,		0,			POS_DEAD,	GM_PLAYER	},
	{ "quit",		do_cmd,			SCMD_QUIT,		POS_DEAD,	GM_PLAYER	},
	{ "logou",		do_inputall,		0,			POS_DEAD,	GM_PLAYER	},
	{ "logout",		do_cmd,			SCMD_LOGOUT,		POS_DEAD,	GM_PLAYER	},
	{ "skillup",	do_skillup,		0,			POS_DEAD,	GM_PLAYER	},
	{ "gskillup",	do_guildskillup,	0,			POS_DEAD,	GM_PLAYER	},
	{ "pvp",		do_pvp,			0,			POS_DEAD,	GM_PLAYER	},
	{ "safebox",	do_safebox_size,	0,			POS_DEAD,	GM_HIGH_WIZARD	},
	{ "safebox_close",	do_safebox_close,	0,			POS_DEAD,	GM_PLAYER	},
	{ "safebox_passwor",do_inputall,		0,			POS_DEAD,	GM_PLAYER	},
	{ "safebox_password",do_safebox_password,	0,			POS_DEAD,	GM_PLAYER	},
	{ "safebox_change_passwor", do_inputall,	0,			POS_DEAD,	GM_PLAYER	},
	{ "safebox_change_password", do_safebox_change_password,	0,	POS_DEAD,	GM_PLAYER	},
	{ "mall_passwor",	do_inputall,		0,			POS_DEAD,	GM_PLAYER	},
	{ "mall_password",	do_mall_password,	0,			POS_DEAD,	GM_PLAYER	},
	{ "mall_close",	do_mall_close,		0,			POS_DEAD,	GM_PLAYER	},

	// Group Command
	{ "ungroup",	do_ungroup,		0,			POS_DEAD,	GM_PLAYER	},

	// REFINE_ROD_HACK_BUG_FIX
	{ "refine_rod",	do_refine_rod,		0,			POS_DEAD,	GM_IMPLEMENTOR	},
	// END_OF_REFINE_ROD_HACK_BUG_FIX

	// REFINE_PICK
	{ "refine_pick",	do_refine_pick,		0,			POS_DEAD,	GM_IMPLEMENTOR	},
	{ "max_pick",	do_max_pick,		0,			POS_DEAD,	GM_IMPLEMENTOR	},
	// END_OF_REFINE_PICK

	//{ "fish_simul",	do_fishing_simul,	0,			POS_DEAD,	GM_IMPLEMENTOR	},
	{ "invisible",	do_invisibility,	0,			POS_DEAD,	GM_LOW_WIZARD	},
	{ "qf",		do_qf,			0,			POS_DEAD,	GM_IMPLEMENTOR	},
	{ "clear_quest",	do_clear_quest,		0,			POS_DEAD,	GM_IMPLEMENTOR	},

	{ "close_shop",	do_close_shop,		0,			POS_DEAD,	GM_PLAYER	},

	{ "set_walk_mode",	do_set_walk_mode,	0,			POS_DEAD,	GM_PLAYER	},
	{ "set_run_mode",	do_set_run_mode,	0,			POS_DEAD,	GM_PLAYER	},
	{ "setjob",do_set_skill_group,	0,			POS_DEAD,	GM_IMPLEMENTOR	},
	{ "setskill",	do_setskill,		0,			POS_DEAD,	GM_LOW_WIZARD	},
	{ "setskillother",	do_setskillother,	0,			POS_DEAD,	GM_IMPLEMENTOR	},
	{ "setskillpoint",  do_set_skill_point,	0,			POS_DEAD,	GM_IMPLEMENTOR	},
	{ "reload",		do_reload,		0,			POS_DEAD,	GM_IMPLEMENTOR	},
	{ "cooltime",	do_cooltime,		0,			POS_DEAD,	GM_IMPLEMENTOR	},

	{ "gwlist",		do_gwlist,		0,			POS_DEAD,	GM_IMPLEMENTOR	},
	{ "gwstop",		do_stop_guild_war,	0,			POS_DEAD,	GM_IMPLEMENTOR	},
	{ "gwcancel",	do_cancel_guild_war, 0,			POS_DEAD,	GM_IMPLEMENTOR	},
	{ "gstate",		do_guild_state,		0,			POS_DEAD,	GM_IMPLEMENTOR	},

	{ "pkmode",		do_pkmode,		0,			POS_DEAD,	GM_PLAYER	},
	{ "messenger_auth",	do_messenger_auth,	0,			POS_DEAD,	GM_PLAYER	},

	{ "getqf",		do_getqf,		0,			POS_DEAD,	GM_IMPLEMENTOR	},
	{ "setqf",		do_setqf,		0,			POS_DEAD,	GM_IMPLEMENTOR	},
	{ "delqf",		do_delqf,		0,			POS_DEAD,	GM_IMPLEMENTOR	},
	{ "set_state",	do_set_state,		0,			POS_DEAD,	GM_IMPLEMENTOR	},

	{ "detaillog",	do_detaillog,		0,			POS_DEAD,	GM_IMPLEMENTOR	},
	{ "monsterlog",	do_monsterlog,		0,			POS_DEAD,	GM_IMPLEMENTOR	},

	{ "forgetme",	do_forgetme,		0,			POS_DEAD,	GM_LOW_WIZARD	},
	{ "aggregate",	do_aggregate,		0,			POS_DEAD,	GM_LOW_WIZARD	},
	{ "attract_ranger",	do_attract_ranger,	0,			POS_DEAD,	GM_LOW_WIZARD	},
	{ "pull_monster",	do_pull_monster,	0,			POS_DEAD,	GM_LOW_WIZARD	},
	{ "setblockmode",	do_setblockmode,	0,			POS_DEAD,	GM_PLAYER	},
	{ "polymorph",	do_polymorph,		0,			POS_DEAD,	GM_LOW_WIZARD	},
	{ "polyitem",	do_polymorph_item,	0,			POS_DEAD,	GM_IMPLEMENTOR },
	{ "priv_empire",	do_priv_empire,		0,			POS_DEAD,	GM_IMPLEMENTOR	},
	{ "priv_guild",	do_priv_guild,		0,			POS_DEAD,	GM_IMPLEMENTOR	},
	{ "mount_test",	do_mount_test,		0,			POS_DEAD,	GM_IMPLEMENTOR	},
	{ "unmount",	do_unmount,		0,			POS_DEAD,	GM_PLAYER	},
	{ "private",	do_private,		0,			POS_DEAD,	GM_IMPLEMENTOR	},
	{ "party_request",	do_party_request,	0,			POS_DEAD,	GM_PLAYER	},
	{ "party_request_accept", do_party_request_accept,0,		POS_DEAD,	GM_PLAYER	},
	{ "party_request_deny", do_party_request_deny,0,			POS_DEAD,	GM_PLAYER	},
	{ "observer",	do_observer,		0,			POS_DEAD,	GM_IMPLEMENTOR	},
	{ "observer_exit",	do_observer_exit,	0,			POS_DEAD,	GM_PLAYER	},
	{ "socketitem",	do_socket_item,		0,			POS_DEAD,	GM_IMPLEMENTOR	},
	{ "saveati",	do_save_attribute_to_image, 0,			POS_DEAD,	GM_IMPLEMENTOR	},
	{ "xmas_boom",	do_xmas,		SCMD_XMAS_BOOM,		POS_DEAD,	GM_IMPLEMENTOR	},
	{ "xmas_snow",	do_xmas,		SCMD_XMAS_SNOW,		POS_DEAD,	GM_IMPLEMENTOR	},
	{ "xmas_santa",	do_xmas,		SCMD_XMAS_SANTA,	POS_DEAD,	GM_IMPLEMENTOR	},
	{ "view_equip",	do_view_equip,		0,			POS_DEAD,	GM_PLAYER   	},
	{ "jy",				do_block_chat,		0,			POS_DEAD,	GM_IMPLEMENTOR	},

	// BLOCK_CHAT
	{ "vote_block_chat", do_vote_block_chat,		0,			POS_DEAD,	GM_PLAYER	},
	{ "block_chat",		do_block_chat,		0,			POS_DEAD,	GM_PLAYER	},
	{ "block_chat_list",do_block_chat_list,	0,			POS_DEAD,	GM_PLAYER	},
	// END_OF_BLOCK_CHAT

	{ "build",		do_build,		0,		POS_DEAD,	GM_PLAYER	},
	{ "clear_land", do_clear_land,	0,		POS_DEAD,	GM_IMPLEMENTOR	},

	{ "affect_remove",	do_affect_remove,	0,			POS_DEAD,	GM_IMPLEMENTOR	},

	{ "horse_state",	do_horse_state,		0,			POS_DEAD,	GM_IMPLEMENTOR	},
	{ "horse_level",	do_horse_level,		0,			POS_DEAD,	GM_IMPLEMENTOR	},
	{ "horse_ride",	do_horse_ride,		0,			POS_DEAD,	GM_IMPLEMENTOR	},
#ifdef ENABLE_MOUNT_SYSTEM
	{ "horse_summon",	do_horse_summon,	0,			POS_DEAD,	GM_PLAYER	},
	{ "horse_unsummon",	do_horse_unsummon,	0,			POS_DEAD,	GM_PLAYER	},
#else
	// { "horse_summon",	do_horse_summon,	0,			POS_DEAD,	GM_HIGH_WIZARD },
	// { "horse_unsummon",	do_horse_unsummon,	0,			POS_DEAD,	GM_HIGH_WIZARD },
#endif

#ifdef ENABLE_MOUNT_SYSTEM
	{ "user_horse_ride",	do_user_horse_ride,		0,		POS_FISHING,	GM_PLAYER	},
	{ "user_horse_back",	do_user_horse_back,		0,		POS_FISHING,	GM_PLAYER	},
#endif
	{ "horse_set_stat", do_horse_set_stat,	0,			POS_DEAD,	GM_IMPLEMENTOR	},

	{ "emotion_allow",	do_emotion_allow,	0,			POS_FIGHTING,	GM_PLAYER	},
	{ "kiss",		do_emotion,		0,			POS_FIGHTING,	GM_PLAYER	},
	{ "slap",		do_emotion,		0,			POS_FIGHTING,	GM_PLAYER	},
	{ "french_kiss",	do_emotion,		0,			POS_FIGHTING,	GM_PLAYER	},
	{ "clap",		do_emotion,		0,			POS_FIGHTING,	GM_PLAYER	},
	{ "cheer1",		do_emotion,		0,			POS_FIGHTING,	GM_PLAYER	},
	{ "cheer2",		do_emotion,		0,			POS_FIGHTING,	GM_PLAYER	},

	// DANCE
	{ "dance1",		do_emotion,		0,			POS_FIGHTING,	GM_PLAYER	},
	{ "dance2",		do_emotion,		0,			POS_FIGHTING,	GM_PLAYER	},
	{ "dance3",		do_emotion,		0,			POS_FIGHTING,	GM_PLAYER	},
	{ "dance4",		do_emotion,		0,			POS_FIGHTING,	GM_PLAYER	},
	{ "dance5",		do_emotion,		0,			POS_FIGHTING,	GM_PLAYER	},
	{ "dance6",		do_emotion,		0,			POS_FIGHTING,	GM_PLAYER	},
	// END_OF_DANCE

	{ "congratulation",	do_emotion,	0,	POS_FIGHTING,	GM_PLAYER	},
	{ "forgive",		do_emotion,	0,	POS_FIGHTING,	GM_PLAYER	},
	{ "angry",		do_emotion,	0,	POS_FIGHTING,	GM_PLAYER	},
	{ "attractive",	do_emotion,	0,	POS_FIGHTING,	GM_PLAYER	},
	{ "sad",		do_emotion,	0,	POS_FIGHTING,	GM_PLAYER	},
	{ "shy",		do_emotion,	0,	POS_FIGHTING,	GM_PLAYER	},
	{ "cheerup",	do_emotion,	0,	POS_FIGHTING,	GM_PLAYER	},
	{ "banter",		do_emotion,	0,	POS_FIGHTING,	GM_PLAYER	},
	{ "joy",		do_emotion,	0,	POS_FIGHTING,	GM_PLAYER	},

	{ "change_attr",	do_change_attr,		0,			POS_DEAD,	GM_IMPLEMENTOR	},
	{ "add_attr",	do_add_attr,		0,			POS_DEAD,	GM_IMPLEMENTOR	},
	{ "add_socket",	do_add_socket,		0,			POS_DEAD,	GM_IMPLEMENTOR	},
	{ "user_horse_feed",	do_user_horse_feed,		0,		POS_FISHING,	GM_PLAYER	},

	{ "show_arena_list",	do_show_arena_list,		0,		POS_DEAD,	GM_IMPLEMENTOR	},
	{ "end_all_duel",		do_end_all_duel,		0,		POS_DEAD,	GM_IMPLEMENTOR	},
	{ "end_duel",			do_end_duel,			0,		POS_DEAD,	GM_IMPLEMENTOR	},
	{ "duel",				do_duel,				0,		POS_DEAD,	GM_IMPLEMENTOR	},

	{ "con+",			do_stat_plus_amount,	POINT_HT,	POS_DEAD,	GM_IMPLEMENTOR	},
	{ "int+",			do_stat_plus_amount,	POINT_IQ,	POS_DEAD,	GM_IMPLEMENTOR	},
	{ "str+",			do_stat_plus_amount,	POINT_ST,	POS_DEAD,	GM_IMPLEMENTOR	},
	{ "dex+",			do_stat_plus_amount,	POINT_DX,	POS_DEAD,	GM_IMPLEMENTOR	},

	{ "break_marriage",	do_break_marriage,		0,			POS_DEAD,	GM_IMPLEMENTOR	},

	{ "show_quiz",			do_oxevent_show_quiz,	0,	POS_DEAD,	GM_IMPLEMENTOR	},
	{ "log_oxevent",		do_oxevent_log,			0,	POS_DEAD,	GM_IMPLEMENTOR	},
	{ "get_oxevent_att",	do_oxevent_get_attender,0,	POS_DEAD,	GM_IMPLEMENTOR	},

	{ "effect",				do_effect,				0,	POS_DEAD,	GM_IMPLEMENTOR	},

	{ "threeway_info",		do_threeway_war_info,	0,	POS_DEAD,	GM_IMPLEMENTOR},
	{ "threeway_myinfo",	do_threeway_war_myinfo, 0,	POS_DEAD,	GM_IMPLEMENTOR},
	{ "mto",				do_monarch_warpto,		0, 	POS_DEAD,	GM_PLAYER},
	{ "mtr",				do_monarch_transfer,	0,	POS_DEAD,	GM_PLAYER},
	{ "minfo",		do_monarch_info,		0,  POS_DEAD,   GM_PLAYER},
	{ "mtax",			do_monarch_tax,			0,	POS_DEAD,	GM_PLAYER},
	{ "mmob",			do_monarch_mob,			0, 	POS_DEAD,	GM_PLAYER},
	{ "elect",				do_elect,				0,	POS_DEAD,	GM_HIGH_WIZARD},
	{ "rmcandidacy",		do_rmcandidacy,			0, 	POS_DEAD,	GM_IMPLEMENTOR},
	{ "setmonarch",			do_setmonarch,			0, 	POS_DEAD,	GM_IMPLEMENTOR},
	{ "rmmonarch",			do_rmmonarch,			0, 	POS_DEAD, 	GM_IMPLEMENTOR},
	{ "hair",				do_hair,				0,	POS_DEAD,	GM_PLAYER	},
	{ "inventory",			do_inventory,			0,	POS_DEAD,	GM_IMPLEMENTOR	},
	{ "cube",				do_cube,				0,	POS_DEAD,	GM_IMPLEMENTOR	},
#ifdef ENABLE_SYSTEMY_KARTY_OKEY	
	{ "cards",				do_cards,				0,	POS_DEAD,	GM_PLAYER	},
#endif
	{ "siege",				do_siege,				0,	POS_DEAD,	GM_IMPLEMENTOR	},
	{ "temp",				do_temp,				0,	POS_DEAD,	GM_IMPLEMENTOR	},
	{ "frog",				do_frog,				0,	POS_DEAD,	GM_HIGH_WIZARD	},
	{ "check_mmoney",		do_check_monarch_money,	0,	POS_DEAD,	GM_IMPLEMENTOR	},
	{ "reset_subskill",		do_reset_subskill,		0,	POS_DEAD,	GM_HIGH_WIZARD },
	{ "flush",				do_flush,				0,	POS_DEAD,	GM_IMPLEMENTOR },
	{ "gift",				do_gift,				0,  POS_DEAD,   GM_PLAYER	},	//gift

	{ "mnotice",			do_monarch_notice,		0,	POS_DEAD,	GM_PLAYER	},

	{ "eclipse",			do_eclipse,				0,	POS_DEAD,	GM_HIGH_WIZARD	},

	{ "eventhelper",		do_event_helper,		0,	POS_DEAD,	GM_HIGH_WIZARD	},

	{ "in_game_mall",		do_in_game_mall,		0,	POS_DEAD,	GM_PLAYER	},

	{ "get_mob_count",		do_get_mob_count,		0,	POS_DEAD,	GM_IMPLEMENTOR	},

	{ "dice",				do_dice,				0,	POS_DEAD,	GM_PLAYER		},
	{ "special_item",			do_special_item,	0,	POS_DEAD,	GM_IMPLEMENTOR		},

	{ "click_mall",			do_click_mall,			0,	POS_DEAD,	GM_PLAYER		},
	{ "click_safebox",			do_click_safebox,			0,	POS_DEAD,	GM_PLAYER		},

	{ "ride",				do_ride,				0,	POS_DEAD,	GM_PLAYER	},

	{ "item_id_list",	do_get_item_id_list,	0,	POS_DEAD,	GM_IMPLEMENTOR	},
	{ "set_socket",		do_set_socket,			0,	POS_DEAD,	GM_IMPLEMENTOR	},

	{ "costume",			do_costume, 			0,	POS_DEAD,	GM_PLAYER	},

	{ "tcon",			do_set_stat,	POINT_HT,	POS_DEAD,	GM_IMPLEMENTOR	},
	{ "tint",			do_set_stat,	POINT_IQ,	POS_DEAD,	GM_IMPLEMENTOR	},
	{ "tstr",			do_set_stat,	POINT_ST,	POS_DEAD,	GM_IMPLEMENTOR	},
	{ "tdex",			do_set_stat,	POINT_DX,	POS_DEAD,	GM_IMPLEMENTOR	},

	{ "cannot_dead",			do_can_dead,	1,	POS_DEAD,		GM_LOW_WIZARD},
	{ "can_dead",				do_can_dead,	0,	POS_DEAD,		GM_LOW_WIZARD},

	{ "full_set",	do_full_set, 0, POS_DEAD,		GM_IMPLEMENTOR},
	{ "item_full_set",	do_item_full_set, 0, POS_DEAD,		GM_IMPLEMENTOR},
	{ "attr_full_set",	do_attr_full_set, 0, POS_DEAD,		GM_IMPLEMENTOR},
	{ "attr_pvp_warrior",	do_attr_pvp_warrior, 0, POS_DEAD,		GM_IMPLEMENTOR},
	{ "attr_pvp_sura_bm",	do_attr_pvp_sura_bm, 0, POS_DEAD,		GM_IMPLEMENTOR},
	{ "attr_pvp_sura_wp",	do_attr_pvp_sura_wp, 0, POS_DEAD,		GM_IMPLEMENTOR},
	{ "attr_pvp_shaman",	do_attr_pvp_shaman, 0, POS_DEAD,		GM_IMPLEMENTOR},
	{ "attr_pvm_skill_damage_magic",	do_pvm_skill_damage_magic, 0, POS_DEAD,		GM_IMPLEMENTOR},
	{ "attr_pvm_skill_damage_melee",	do_pvm_skill_damage_melee, 0, POS_DEAD,		GM_IMPLEMENTOR},
	{ "attr_pvm_average_damage",	do_pvm_average_damage, 0, POS_DEAD,		GM_IMPLEMENTOR},
	{ "attr_full_set_int",	do_attr_full_set_int, 0, POS_DEAD,		GM_IMPLEMENTOR},
	{ "attr_full_set_poh",	do_attr_full_set_poh, 0, POS_DEAD,		GM_IMPLEMENTOR},
	{ "attr_full_set_hu_prisery",	do_attr_full_set_hu_prisery, 0, POS_DEAD,		GM_IMPLEMENTOR},
	{ "all_skill_master",	do_all_skill_master,	0,	POS_DEAD,	GM_IMPLEMENTOR},
	{ "use_item",		do_use_item,	0, POS_DEAD,		GM_IMPLEMENTOR},

	{ "toggle_antyexp",		do_toggle_antyexp,	0, POS_DEAD,		GM_PLAYER },

#ifdef __ENABLE_COLLECTIONS_SYSTEM__
	{ "do_add_collect_item", do_add_collect_item, 0, POS_DEAD, GM_PLAYER },
#endif

	{ "dragon_soul",				do_dragon_soul,				0,	POS_DEAD,	GM_PLAYER	},
	{ "ds_list",				do_ds_list,				0,	POS_DEAD,	GM_PLAYER	},
	{ "do_clear_affect", do_clear_affect, 	0, POS_DEAD,		GM_IMPLEMENTOR},
#ifdef ENABLE_OFFLINE_SHOP_SYSTEM
	{ "open_offlineshop",	do_open_offline_shop,		0,			POS_DEAD,	GM_PLAYER	},
	{ "offline_shop_panel",	do_offline_shop_open_panel,	0,			POS_DEAD,	GM_PLAYER	},	
	{ "offline_shop_logs", do_offline_shop_open_logs,	0,			POS_DEAD,	GM_PLAYER	},
#endif

// #ifdef ENABLE_MOUNT_COSTUME_SYSTEM
// 	{ "horseshoe", do_horseshoe, 0, POS_DEAD, GM_PLAYER },
// #endif
#ifdef ENABLE_ODLAMKI_SYSTEM
	{ "soul_piece_craft", do_odlamki, 0, POS_DEAD, GM_PLAYER },
#endif
#ifdef ENABLE_NEW_STONE_DETACH
	{ "detach_stone",		 do_detach_stone,	0,			POS_DEAD,	GM_PLAYER },
#endif
	{ "dungeons", 		do_dungeons,	0,		POS_DEAD,	GM_PLAYER },
#ifdef ENABLE_NEW_PET_SYSTEM
	{ "pet_skillup",	do_pet_skillup,		0,			POS_DEAD,	GM_PLAYER },
	{ "pet_name",	do_pet_name,		0,			POS_DEAD,	GM_PLAYER },
#endif
#ifdef ENABLE_NEWSTUFF
	//item
	{ "add_rare_attr",		do_add_rare_attr,			0,			POS_DEAD,	GM_IMPLEMENTOR	},
	{ "change_rare_attr",	do_change_rare_attr,		0,			POS_DEAD,	GM_IMPLEMENTOR	},
	//player
	{ "click_safebox",		do_click_safebox,			0,			POS_DEAD,	GM_IMPLEMENTOR	},
	{ "force_logout",		do_force_logout,			0,			POS_DEAD,	GM_IMPLEMENTOR	},
	{ "poison",				do_poison,					0,			POS_DEAD,	GM_IMPLEMENTOR	},
	{ "rewarp",				do_rewarp,					0,			POS_DEAD,	GM_LOW_WIZARD	},
#endif
#ifdef ENABLE_WOLFMAN_CHARACTER
	{ "bleeding",			do_bleeding,				0,			POS_DEAD,	GM_IMPLEMENTOR	},
#endif
#ifdef ENABLE_POLY_SHOP
	{ "buy_marble_item", do_buy_marble, 0, POS_DEAD, GM_PLAYER },
#endif
#ifdef ENABLE_HIDE_COSTUME_SYSTEM
	{ "hide_costume", do_hide_costume, 0, POS_DEAD, GM_PLAYER },
#endif
	{ "stat_val",		do_stat_val,		0,			POS_DEAD,	GM_PLAYER	},

	// { "fast_opener",		do_fast_opener,				0,		POS_DEAD,	GM_PLAYER		},

#ifdef ENABLE_PROMO_CODE_SYSTEM
	{ "promo_code_system",		do_promo_code_system,		0,		POS_DEAD,		GM_PLAYER	},
	{ "promo_code_open",		do_promo_code_open,			0,		POS_DEAD,		GM_PLAYER	},
#endif
#ifdef ENABLE_EVENT_MANAGER
	{ "event_manager",	do_event_manager,		0,		POS_DEAD,	GM_PLAYER },
#endif

#ifdef ENABLE_COLLECT_WINDOW
	{ "choose_quest",	do_choose_quest,		0,		POS_DEAD,	GM_PLAYER },
	{ "collect_window",	do_open_collect_window,		0,		POS_DEAD,	GM_PLAYER },
#endif
#ifdef ENABLE_NOTIFICATION_SYSTEM
	{ "enable_mission_notifications",	do_enable_mission_notifications,	0,	POS_DEAD,	GM_PLAYER },
#endif
#ifdef ENABLE_DRAGON_SOUL_TIME_RENEWAL
	{ "renew_ds",	do_renew_alchemy,		0,		POS_DEAD,		GM_PLAYER },
#endif
#ifdef ENABLE_SKILL_SELECT_FEATURE
	{ "selectskill_open",	do_selectskill_open,	0,	POS_DEAD,	GM_PLAYER	},
	{ "selectskill_select",		do_selectskill_select,			0,	POS_DEAD,	GM_PLAYER	},	
#endif

#ifdef __PREMIUM_PRIVATE_SHOP__
	{ "register_off_shop",		do_register_off_shop,			0,	POS_DEAD,	GM_PLAYER	},	
#endif
#ifdef __DUNGEON_INFO_ENABLE__
	{ "dungeon_info_open_panel",	do_dungeon_info_open_panel,	0,	POS_DEAD,	GM_PLAYER},
	{ "dungeon_info_join_dungeon",	do_dungeon_info_join_dungeon,	0,	POS_DEAD,	GM_PLAYER},
	{ "dungeon_info_reset_cooldown_dungeon", do_dungeon_info_reset_cooldown_dungeon, 0, POS_DEAD, GM_PLAYER },
#endif
#ifdef __DUNGEON_RETURN_ENABLE__
	{ "dungeon_info_rejoin_dungeon",	do_dungeon_info_rejoin_dungeon,	0,	POS_DEAD,	GM_PLAYER},
#endif
#ifdef __CASKET_PREVIEW_ENABLE__
	{ "fetch_casket_items", do_fetch_casket_items, 0, POS_DEAD, GM_PLAYER },
#endif
#ifdef __INVENTORY_BUFFERING__
	{ "quick_open", do_quick_open, 0, POS_DEAD, GM_PLAYER },
#endif
#ifdef __TEAM_MEMBER_STATUS__
	{"team_list_reload", do_team_list_reload, 0, POS_DEAD, GM_IMPLEMENTOR},
#endif
#ifdef ENABLE_GET_TOTAL_ONLINE_COUNT
	{ "total_players",						do_get_total_online,						0,				POS_DEAD,		GM_IMPLEMENTOR	},
#endif
#ifdef ENABLE_BOT_CONTROL
	{ "bot31_control_fail", do_bot_control,	0,	POS_DEAD, GM_PLAYER },
	{ "bot31_control_open", do_bot_control_open,	0,	POS_DEAD, GM_PLAYER },
#endif
#ifdef ENABLE_ENLIGHTENMENT_SYSTEM
	{ "enlight", do_enlight,		 0,			POS_DEAD, GM_PLAYER},
#endif
	{ "refund_items",		do_refund_items,			0,	POS_DEAD,	GM_IMPLEMENTOR	},	
	{ "fish_wiki", do_fish_wiki, 0, POS_DEAD, GM_PLAYER	},
	{ "gguard_items",		do_guild_guard_items,		0,			POS_DEAD,	GM_PLAYER	},
	{ "guild_dungeon",		do_guild_dungeon,			0,			POS_DEAD,	GM_PLAYER	},
	{ "guild_donate_material",	do_guild_donate_material,	0,			POS_DEAD,	GM_PLAYER	},
	{ "guild_view_storage",		do_guild_view_storage,		0,			POS_DEAD,	GM_PLAYER	},
	{ "guild_upgrade_bonus",	do_guild_upgrade_bonus,		0,			POS_DEAD,	GM_PLAYER	},
#ifdef __WORLD_BOSS_YUMA__
	{ "worldboss",				do_boss_debug,				0,				POS_DEAD,		GM_PLAYER	},
#endif
#ifdef DUNGEON_CARDS
	{ "dungeon_cards", 	do_dungeon_cards, 		0, 			POS_DEAD, 	GM_PLAYER },
#endif
#ifdef __SPIN_WHEEL__
	{ "spin_wheel",	do_spin_wheel,		0,		POS_DEAD,	GM_PLAYER },
#endif
	
	{ "add_attribute", do_add_attribute, 0, POS_DEAD, GM_IMPLEMENTOR },
	{ "teleport", do_teleport, 0, POS_DEAD, GM_PLAYER },
#ifdef ENABLE_WHEEL_OF_FORTUNE
	{ "wheel_of_fortune",	do_wheel_of_fortune,		0,		POS_DEAD,	GM_PLAYER },
	{ "wheel_of_fortune_gold", do_wheel_of_fortune_gold, 0, POS_DEAD, GM_PLAYER },
#endif
#ifdef ENABLE_PVP_RANKING
	{ "pvp_ranking",	do_pvp_ranking,		0,		POS_DEAD,	GM_IMPLEMENTOR },
#endif
#ifdef ENABLE_AFFECT_BUFF_REMOVE
	{ "remove_affect",	do_remove_buff,	0,		POS_DEAD,	GM_PLAYER },
#endif
#ifdef ENABLE_HWID_SYSTEM
	{ "banpanel", 	do_banpanel,	0,		POS_DEAD,	GM_IMPLEMENTOR	},
#endif
#ifdef __PENDANT__
	{ "pendant",	do_pendant,		0,		POS_DEAD,	GM_PLAYER },
#endif
#ifdef ENABLE_TITLE_ACHIEVEMENT_SYSTEM
	{ "load_title_achievement", do_load_title_achievement, 	0, POS_DEAD,		GM_PLAYER},
	{ "change_title_achievement", do_change_title_achievement, 	0, POS_DEAD,		GM_PLAYER},
	{ "change_title_achievement_premium", do_change_title_achievement_premium, 	0, POS_DEAD,		GM_PLAYER},
#endif
	{ "g_add_gold",			do_guild_add_gold,			0,			POS_DEAD,	GM_IMPLEMENTOR },
	{ "world_boss_request", do_world_boss_request, 0, POS_DEAD, GM_PLAYER },
	{ "world_boss_teleport", do_world_boss_teleport, 0, POS_DEAD, GM_PLAYER },
#ifdef ENABLE_ITEMSHOP
	{ "ishop", do_ishop, 0, POS_DEAD, GM_PLAYER },
#endif
#ifdef __DUNGEON_INFO__
	{ "dungeon_info",	do_dungeon_info,		0,		POS_DEAD,	GM_PLAYER },
#endif
#ifdef ENABLE_NOTIFICATION_SYSTEM
	{ "test_notification",		do_test_notification,		0,		POS_DEAD,	GM_IMPLEMENTOR },
#endif
#ifdef ENABLE_VOTE4BUFF
	{ "vote4buff",			do_vote4buff,				0,			POS_DEAD,	GM_PLAYER	},
#endif
	{ "open_shop",	do_open_shop,		0,		POS_DEAD,	GM_PLAYER },
#ifdef __VIP_SYSTEM__
	{ "convert_vip_affect",			do_convert_vip_affect,			0,					POS_DEAD,		GM_PLAYER   	},
#endif
#ifdef ENABLE_MULTI_LANGUAGE_SYSTEM
	{ "language",					do_language,					0,					POS_DEAD,		GM_PLAYER		},
#endif
#ifdef _ENABLE_BATTLEPASS_
	{ "battle_pass", do_battle_pass, 0, POS_DEAD, GM_PLAYER },
	{ "bp_fix_missions", do_battle_pass_admin, 0, POS_DEAD, GM_IMPLEMENTOR },
#endif
#ifdef ENABLE_AUTO_SELECT_SKILL
	{ "skillauto",	do_skillauto,		0,		POS_DEAD,	GM_PLAYER },
#endif
#ifdef __AUTO_SKILL_READER__
	{ "auto_skill_reader",	do_auto_skill_reader,		0,		POS_DEAD,	GM_PLAYER },
#endif
#ifdef ENABLE_MULTI_FARM_BLOCK
	{ "multi_farm",	do_multi_farm,		0,		POS_DEAD,	GM_PLAYER },
#endif


	{ "\n",		NULL,			0,			POS_DEAD,	GM_IMPLEMENTOR	}
};

void interpreter_set_privilege(const char *cmd, int lvl)
{
	int i;

	for (i = 0; *cmd_info[i].command != '\n'; ++i)
	{
		if (!str_cmp(cmd, cmd_info[i].command))
		{
			cmd_info[i].gm_level = lvl;
			sys_log(0, "Setting command privilege: %s -> %d", cmd, lvl);
			break;
		}
	}
}

void double_dollar(const char *src, size_t src_len, char *dest, size_t dest_len)
{
	const char * tmp = src;
	size_t cur_len = 0;

	dest_len -= 1;

	while (src_len-- && *tmp)
	{
		if (*tmp == '$')
		{
			if (cur_len + 1 >= dest_len)
				break;

			*(dest++) = '$';
			*(dest++) = *(tmp++);
			cur_len += 2;
		}
		else
		{
			if (cur_len >= dest_len)
				break;

			*(dest++) = *(tmp++);
			cur_len += 1;
		}
	}

	*dest = '\0';
}

void interpret_command(LPCHARACTER ch, const char * argument, size_t len)
{
	if (!ch)
	{
		sys_err("NULL CHRACTER");
		return;
	}

#ifdef ENABLE_ANTI_CMD_FLOOD
	if (ch && !PulseManager::Instance().IncreaseCount(ch->GetPlayerID(), ePulse::CommandRequest, std::chrono::milliseconds(500), 5))
	{
		if (test_server)
			ch->ChatPacket(CHAT_TYPE_INFO, "Blocked Command %s", argument);
		sys_log(0, "BLOCKED COMMAND: %s: %s", ch->GetName(), argument);
		return;
	}
#endif

	char cmd[128 + 1];
	char new_line[256 + 1];
	const char * line;
	int icmd;

	if (len == 0 || !*argument)
		return;

	double_dollar(argument, len, new_line, sizeof(new_line));

	size_t cmdlen;
	line = first_cmd(new_line, cmd, sizeof(cmd), &cmdlen);

	for (icmd = 1; *cmd_info[icmd].command != '\n'; ++icmd)
	{
		if (cmd_info[icmd].command_pointer == do_cmd)
		{
			if (!strcmp(cmd_info[icmd].command, cmd))
				break;
		}
#ifdef ENABLE_BLOCK_CMD_SHORTCUT
		else if (!strcmp(cmd_info[icmd].command, cmd))
#else
		else if (!strncmp(cmd_info[icmd].command, cmd, cmdlen))
#endif
			break;
	}

	if (ch->GetPosition() < cmd_info[icmd].minimum_position)
	{
		switch (ch->GetPosition())
		{
			case POS_MOUNTING:
				ch->ChatPacket(CHAT_TYPE_INFO, "[LS;917]");
				break;

			case POS_DEAD:
				ch->ChatPacket(CHAT_TYPE_INFO, "[LS;918]");
				break;

			case POS_SLEEPING:
				ch->ChatPacket(CHAT_TYPE_INFO, "[LS;919]");
				break;

			case POS_RESTING:
			case POS_SITTING:
				ch->ChatPacket(CHAT_TYPE_INFO, "[LS;920]");
				break;

			default:
				sys_err("unknown position %d", ch->GetPosition());
				break;
		}

		return;
	}

	if (*cmd_info[icmd].command == '\n')
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "[LS;921]");
		return;
	}

	if (cmd_info[icmd].gm_level && (cmd_info[icmd].gm_level > ch->GetGMLevel() || cmd_info[icmd].gm_level == GM_DISABLE))
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "[LS;921]");
		return;
	}

	if (strncmp("phase", cmd_info[icmd].command, 5) != 0)
		sys_log(0, "COMMAND: %s: %s", ch->GetName(), cmd_info[icmd].command);

	((*cmd_info[icmd].command_pointer) (ch, line, icmd, cmd_info[icmd].subcmd));

	if (ch->GetGMLevel() >= GM_LOW_WIZARD)
	{
		if (cmd_info[icmd].gm_level >= GM_LOW_WIZARD)
		{
			char buf[1024];
			snprintf( buf, sizeof(buf), "%s", argument );

			LogManager::instance().GMCommandLog(ch->GetPlayerID(), ch->GetName(), ch->GetDesc()->GetHostName(), g_bChannel, buf);
		}
	}
}

std::vector<std::string> ParseArguments(const std::string& arguments) {
    std::vector<std::string> args;
    boost::escaped_list_separator<char> seps("", " ", "\"\'");
    boost::tokenizer<boost::escaped_list_separator<char>> tokens(arguments, seps);

    for (const auto& token : tokens)
        args.emplace_back(token);

    if (!args.empty() && args[0].empty())
        args.erase(args.begin());

    return args;
}

