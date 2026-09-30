import uiScriptLocale

SMALL_VALUE_FILE = "d:/ymir work/ui/public/Parameter_Slot_01.sub"
LARGE_VALUE_FILE = "d:/ymir work/ui/public/Parameter_Slot_03.sub"
XLARGE_VALUE_FILE = "d:/ymir work/ui/public/Parameter_Slot_04.sub"
GUILD_PATH = "elvion/game/guild{}"

window = {
	"name" : "GuildWindow_GuildInfoPage",

	"x" : 39,
	"y" : 0,

	"width" : 656-39,
	"height" : 323,

	"children" :
	(
		{
			"name" : "Board",
			"type" : "image",
		
			"x" : 0,
			"y" : 0,
		
			"image" : GUILD_PATH.format("/home/bg.png"),
		},

		{
			"name" : "guild_information_header",
			"type" : "window",
			"x" : 8, "y" : 35,
			"width" : 317, "height" : 26,
			"children":
			(
				{ 
					"name":"Guild_Info_Point_Value",
					"type":"text",
					"x":0,
					"y":0,
					"text":uiScriptLocale.GUILD_INFO,
					"outline" : 1,
					"fontname":"Tahoma:12",
					"color":0xFFdcc2a9,
					"all_align" : "center",
				},
			),
		},

		{
			"name" : "guild_skill_header",
			"type" : "window",
			"x" : 329, "y" : 35,
			"width" : 271, "height" : 26,
			"children":
			(
				{ 
					"name":"Guild_Info_Point_Value",
					"type":"text",
					"x":0,
					"y":0,
					"text":uiScriptLocale.GUILD_INFO_SKILL,
					"outline" : 1,
					"fontname":"Tahoma:12",
					"color":0xFFdcc2a9,
					"all_align" : "center",
				},
			),
		},

		{
			"name" : "guild_storage_items_header",
			"type" : "window",
			"x" : 329, "y" : 215,
			"width" : 271, "height" : 26,
			"children":
			(
				{ 
					"name":"Guild_Info_Point_Value",
					"type":"text",
					"x":0,
					"y":0,
					"text":uiScriptLocale.GUILD_INFO_STORAGE_ITEMS,
					"outline" : 1,
					"fontname":"Tahoma:12",
					"color":0xFFdcc2a9,
					"all_align" : "center",
				},
			),
		},

		# Guild Name
		{
			"name" : "guild_name_header",
			"type" : "window",
			"x" : 13, "y" : 69,
			"width" : 110, "height" : 18,
			"children":
			(
				{
					"name" : "GuildName",
					"type" : "text",
					"x" : 0,
					"y" : -2,
					"text" : uiScriptLocale.GUILD_INFO_NAME,
					"outline" : 1,
					"fontname":"Tahoma:12",
					"color":0xFFeed8c3,
					"all_align": "center",
				},
			),
		},
		{
			"name" : "guild_name_value_slot",
			"type" : "window",
			"x" : 13, "y" : 88,
			"width" : 110, "height" : 18,
			"children":
			(
				{
					"name" : "GuildNameValue",
					"type":"text",
					"x":0,
					"y":0,
					"text":uiScriptLocale.GUILD_INFO_NAME_VALUE,
					"outline" : 1,
					"fontname":"Tahoma:12",
					"color":0xFFeed8c3,
					"all_align": "center",
				},
			),
		},
		# Guild Upload Mark
		{
			"name" : "LargeGuildMarkSlot",
			"type" : "slotbar",
			"x" : 147,
			"y" : 68,
			"width" : 48+1,
			"height" : 36+1,
			"children" :
			(
				{
					"name" : "LargeGuildMark",
					"type" : "mark",
					"x" : 1,
					"y" : 1,
				},
			),
		},
		{
			"name" : "UploadGuildMarkButton",
			"type" : "button",
			"x" : 219,
			"y" : 67,
			"text" : uiScriptLocale.GUILD_INFO_UPLOAD_MARK,
			"default_image" : GUILD_PATH.format("/home/upload_norm.png"),
			"over_image" : GUILD_PATH.format("/home/upload_hover.png"),
			"down_image" : GUILD_PATH.format("/home/upload_down.png"),
		},
		# Guild level
		{
			"name" : "GuildLevel",
			"type" : "text",
			"x" : 19,
			"y" : 135,
			"text" : uiScriptLocale.GUILD_INFO_LEVEL,
			"outline" : 1,
			"fontname":"Tahoma:12",
			"color":0xFFeed8c3,
		},
		{
			"name" : "GuildLevelSlot",
			"type" : "window",
			"x" : 274,
			"y" : 132,
			"width" : 45,
			"height" : 17,
			"children" :
			(
				{
					"name" : "GuildLevelValue",
					"type":"text",
					"text":"30",
					"x":0,
					"y":0,
					"all_align":"center",
					"outline" : 1,
					"fontname":"Tahoma:12",
					"color":0xFFf5e4d4,
				},
			),
		},
		# Guild Leader Name
		{
			"name" : "GuildMaster",
			"type" : "text",
			"x" : 19,
			"y" : 165,
			"text" : uiScriptLocale.GUILD_INFO_MASTER,
			"outline" : 1,
			"fontname":"Tahoma:12",
			"color":0xFFeed8c3,
		},
		{
			"name" : "GuildMasterNameSlot",
			"type" : "window",
			"x" : 246,
			"y" : 161,
			"width" : 72,
			"height" : 18,
			"children" :
			(
				{
					"name" : "GuildMasterNameValue",
					"type":"text",
					"text": uiScriptLocale.GUILD_INFO_MASTER_VALUE,
					"x":0,
					"y":0,
					"all_align":"center",
					"outline" : 1,
					"fontname":"Tahoma:12",
					"color":0xFFf5e4d4,
				},
			),
		},
		# Guild Member Count
		{
			"name" : "GuildMemberCount",
			"type" : "text",
			"x" : 19,
			"y" : 195,
			"text" : uiScriptLocale.GUILD_INFO_MEMBER_NUM,
			"outline" : 1,
			"fontname":"Tahoma:12",
			"color":0xFFeed8c3,
		},
		{
			"name" : "GuildMemberCountSlot",
			"type" : "window",
			"x" : 274,
			"y" : 192,
			"width" : 45,
			"height" : 17,
			"children" :
			(
				{
					"name" : "GuildMemberCountValue",
					"type":"text",
					"text":"30 / 32",
					"x":0,
					"y":0,
					"all_align":"center",
					"outline" : 1,
					"fontname":"Tahoma:12",
					"color":0xFFf5e4d4,
				},
			),
		},
		# Guild Experience
		{
			"name" : "CurrentExperience",
			"type" : "text",
			"x" : 19,
			"y" : 225,
			"text" : uiScriptLocale.GUILD_INFO_CUR_EXP,
			"outline" : 1,
			"fontname":"Tahoma:12",
			"color":0xFFeed8c3,
		},
		{
			"name" : "CurrentExperienceSlot",
			"type" : "window",
			"x" : 246,
			"y" : 222,
			"width" : 72,
			"height" : 18,
			"children" :
			(
				{
					"name" : "CurrentExperienceValue",
					"type":"text",
					"text":"0 / 100000",
					"x":0,
					"y":0,
					"all_align":"center",
					"outline" : 1,
					"fontname":"Tahoma:12",
					"color":0xFFf5e4d4,
				},
			),
		},
		{
			"name":"info_exp",
			"type":"button",
			"x" : 225,
			"y" : 224,
			"default_image" : "d:/ymir work/ui/pattern/q_mark_01.tga",
			"over_image" : "d:/ymir work/ui/pattern/q_mark_02.tga",
			"down_image" : "d:/ymir work/ui/pattern/q_mark_01.tga",
		},
		# Guild gold
		{
			"name" : "CurrentGold",
			"type" : "text",
			"x" : 19,
			"y" : 255,
			"text" : uiScriptLocale.GUILD_INFO_GOLD,
			"outline" : 1,
			"fontname":"Tahoma:12",
			"color":0xFFeed8c3,
		},
		{
			"name" : "CurrentGoldSlot",
			"type" : "window",
			"x" : 246,
			"y" : 252,
			"width" : 72,
			"height" : 18,
			"children" :
			(
				{
					"name" : "GuildMoneyValue",
					"type":"text",
					"text":"0",
					"x":0,
					"y":0,
					"all_align":"center",
					"outline" : 1,
					"fontname":"Tahoma:12",
					"color":0xFFf5e4d4,
				},
			),
		},
		{
			"name" : "OfferGoldButton",
			"type" : "button",
			"x" : 216,
			"y" : 253,
			#"text" : uiScriptLocale.GUILD_INFO_OFFER_EXP,
			"default_image" : GUILD_PATH.format("/home/give_gold_norm.png"),
			"over_image" : GUILD_PATH.format("/home/give_gold_hover.png"),
			"down_image" : GUILD_PATH.format("/home/give_gold_down.png"),
		},
		# Guild Average Level
		{
			"name" : "GuildMemberLevelAverage",
			"type" : "text",
			"x" : 19,
			"y" : 285,
			"text" : uiScriptLocale.GUILD_INFO_MEMBER_AVG_LEVEL,
			"outline" : 1,
			"fontname":"Tahoma:12",
			"color":0xFFeed8c3,
		},
		{
			"name" : "GuildMemberLevelAverageSlot",
			"type" : "window",
			"x" : 274,
			"y" : 282,
			"width" : 45,
			"height" : 17,
			"children" :
			(
				{
					"name" : "GuildMemberLevelAverageValue",
					"type":"text",
					"text":"185",
					"x":0,
					"y":0,
					"all_align":"center",
					"outline" : 1,
					"fontname":"Tahoma:12",
					"color":0xFFf5e4d4,
				},
			),
		},
		# Guild passive skills bonus
		{
			"name" : "window_event",
			"type" : "window",
			"width": 260,
			"height": 150,
			"x" :334,
			"y" : 65,
			"children":
			(
				{
					"name":"skill_bonus_1",
					"type":"image",
					"x" : 0,
					"y" : 0,
					"image": GUILD_PATH.format("/home/guild_bonus_slot.png"),
					"children" :
					(
						{
							"name" : "double_drop_chance_bonus",
							"type":"text",
							"text":"Max guild members",
							"x":25,
							"y":7,
							"outline" : 1,
							"fontname":"Tahoma:12",
							"color":0xFFa3bf85,
						},
						{
							"name" : "double_drop_chance_value_slot",
							"type" : "window",
							"width": 65,
							"height": 22,
							"x" :150,
							"y" : 2,
							"children":
							(
								{
									"name" : "double_drop_chance_value",
									"type":"text",
									"text":"Lv.0",
									"x":2,
									"y":0,
									"all_align":"center",
									"outline" : 1,
									"fontname":"Tahoma:12",
									"color":0xFFa3bf85,
								},
							),
						},
							{
								"name" : "upgrade_btn_0",
								"type" : "button",
								"x" : 218,
								"y" : 5,
								"text" : "+",
								"default_image" : "d:/ymir work/ui/public/small_button_01.sub",
								"over_image" : "d:/ymir work/ui/public/small_button_02.sub",
								"down_image" : "d:/ymir work/ui/public/small_button_03.sub",
							},
					),
				},
				{
					"name":"skill_bonus_2",
					"type":"image",
					"x" : 0,
					"y" : 30,
					"image": GUILD_PATH.format("/home/guild_bonus_slot.png"),
					"children" :
					(
						{
							"name" : "stone_bonus",
							"type":"text",
							"text":"Strong against stones",
							"x":25,
							"y":7,
							"outline" : 1,
							"fontname":"Tahoma:12",
							"color":0xFFa3bf85,
						},
						{
							"name" : "stone_value_slot",
							"type" : "window",
							"width": 65,
							"height": 22,
							"x" :150,
							"y" : 2,
							"children":
							(
								{
									"name" : "stone_value",
									"type":"text",
									"text":"Lv.0",
									"x":2,
									"y":0,
									"all_align":"center",
									"outline" : 1,
									"fontname":"Tahoma:12",
									"color":0xFFa3bf85,
								},
							),
						},
							{
								"name" : "upgrade_btn_1",
								"type" : "button",
								"x" : 218,
								"y" : 5,
								"text" : "+",
								"default_image" : "d:/ymir work/ui/public/small_button_01.sub",
								"over_image" : "d:/ymir work/ui/public/small_button_02.sub",
								"down_image" : "d:/ymir work/ui/public/small_button_03.sub",
							},
					),
				},
				{
					"name":"skill_bonus_3",
					"type":"image",
					"x" : 0,
					"y" : 60,
					"image": GUILD_PATH.format("/home/guild_bonus_slot.png"),
					"children" :
					(
						{
							"name" : "boss_bonus",
							"type":"text",
							"text":"Strong against bosses",
							"x":25,
							"y":7,
							"outline" : 1,
							"fontname":"Tahoma:12",
							"color":0xFFa3bf85,
						},
						{
							"name" : "boss_value_slot",
							"type" : "window",
							"width": 65,
							"height": 22,
							"x" :150,
							"y" : 2,
							"children":
							(
								{
									"name" : "boss_value",
									"type":"text",
									"text":"Lv.0",
									"x":2,
									"y":0,
									"all_align":"center",
									"outline" : 1,
									"fontname":"Tahoma:12",
									"color":0xFFa3bf85,
								},
							),
						},
							{
								"name" : "upgrade_btn_2",
								"type" : "button",
								"x" : 218,
								"y" : 5,
								"text" : "+",
								"default_image" : "d:/ymir work/ui/public/small_button_01.sub",
								"over_image" : "d:/ymir work/ui/public/small_button_02.sub",
								"down_image" : "d:/ymir work/ui/public/small_button_03.sub",
							},
					),
				},
				{
					"name":"skill_bonus_4",
					"type":"image",
					"x" : 0,
					"y" : 90,
					"image": GUILD_PATH.format("/home/guild_bonus_slot.png"),
					"children" :
					(
						{
							"name" : "daily_raid_limit_bonus",
							"type":"text",
							"text":"Double drop chance",
							"x":25,
							"y":7,
							"outline" : 1,
							"fontname":"Tahoma:12",
							"color":0xFFa3bf85,
						},
						{
							"name" : "daily_raid_limit_value_slot",
							"type" : "window",
							"width": 65,
							"height": 22,
							"x" :150,
							"y" : 2,
							"children":
							(
								{
									"name" : "daily_raid_limit_value",
									"type":"text",
									"text":"Lv.0",
									"x":2,
									"y":0,
									"all_align":"center",
									"outline" : 1,
									"fontname":"Tahoma:12",
									"color":0xFFa3bf85,
								},
							),
						},
							{
								"name" : "upgrade_btn_3",
								"type" : "button",
								"x" : 218,
								"y" : 5,
								"text" : "+",
								"default_image" : "d:/ymir work/ui/public/small_button_01.sub",
								"over_image" : "d:/ymir work/ui/public/small_button_02.sub",
								"down_image" : "d:/ymir work/ui/public/small_button_03.sub",
							},
					),
				},
				{
					"name":"skill_bonus_5",
					"type":"image",
					"x" : 0,
					"y" : 120,
					"image": GUILD_PATH.format("/home/guild_bonus_slot.png"),
					"children" :
					(
						{
							"name" : "chance_double_fish_ore_bonus",
							"type":"text",
							"text":"Raid Gold Bonus",
							"x":25,
							"y":7,
							"outline" : 1,
							"fontname":"Tahoma:12",
							"color":0xFFa3bf85,
						},
						{
							"name" : "chance_double_fish_ore_value_slot",
							"type" : "window",
							"width": 65,
							"height": 22,
							"x" :150,
							"y" : 2,
							"children":
							(
								{
									"name" : "chance_double_fish_ore_value",
									"type":"text",
									"text":"Lv.0",
									"x":2,
									"y":0,
									"all_align":"center",
									"outline" : 1,
									"fontname":"Tahoma:12",
									"color":0xFFa3bf85,
								},
							),
						},
							{
								"name" : "upgrade_btn_4",
								"type" : "button",
								"x" : 218,
								"y" : 5,
								"text" : "+",
								"default_image" : "d:/ymir work/ui/public/small_button_01.sub",
								"over_image" : "d:/ymir work/ui/public/small_button_02.sub",
								"down_image" : "d:/ymir work/ui/public/small_button_03.sub",
							},
					),
				},
			),
		},
		{
			"name": "ItemScrollbarevent",
			"type": "modern_scrollbar",
			"x": 595,
			"y": 66,
			"width": 4,
			"size": 148,
			"content_height": 146,
			"smooth": 1,
			"scroll_step": 0.30,
		},
		{
			"name":"item_slot_1",
			"type":"image",
			"x" : 345,
			"y" : 243,
			"image": GUILD_PATH.format("/home/item_slot.png"),
			"children":
			(
				{
					"name" : "item_1_count",
					"type":"text",
					"text":"1000",
					"x":0,
					"y":22,
					"all_align":"center",
					"outline" : 1,
					"fontname":"Tahoma:12",
					"color":0xFFeed8c3,
				},
			),
		},
		{
			"name":"item_slot_2",
			"type":"image",
			"x" : 348+86,
			"y" : 243,
			"image": GUILD_PATH.format("/home/item_slot.png"),
			"children":
			(
				{
					"name" : "item_2_count",
					"type":"text",
					"text":"1000",
					"x":0,
					"y":22,
					"all_align":"center",
					"outline" : 1,
					"fontname":"Tahoma:12",
					"color":0xFFeed8c3,
				},
			),
		},
		{
			"name":"item_slot_3",
			"type":"image",
			"x" : 348+89+85,
			"y" : 243,
			"image": GUILD_PATH.format("/home/item_slot.png"),
			"children":
			(
				{
					"name" : "item_3_count",
					"type":"text",
					"text":"1000",
					"x":0,
					"y":22,
					"all_align":"center",
					"outline" : 1,
					"fontname":"Tahoma:12",
					"color":0xFFeed8c3,
				},
			),
		},
		{
			"name" : "GuildMaterialSlot",
			"type" : "slot",
			"x" : 343,
			"y" : 245,
			"width" : 265,
			"height" : 32,
			"slot" : (
				{"index":0, "x":15, "y":10, "width":32, "height":32},
				{"index":1, "x":104, "y":10, "width":32, "height":32},
				{"index":2, "x":192, "y":10, "width":32, "height":32},
			),
		},
		{
			"name" : "DonateItem1",
			"type" : "button",
			"x" : 404,
			"y" : 286,
			#"text" : uiScriptLocale.GUILD_INFO_DECALRE_WAR,
			"default_image" : GUILD_PATH.format("/home/give_item_norm.png"),
			"over_image" : GUILD_PATH.format("/home/give_item_hover.png"),
			"down_image" : GUILD_PATH.format("/home/give_item_down.png"),
		},
		{
			"name" : "DonateItem2",
			"type" : "button",
			"x" : 407+86,
			"y" : 286,
			#"text" : uiScriptLocale.GUILD_INFO_DECALRE_WAR,
			"default_image" : GUILD_PATH.format("/home/give_item_norm.png"),
			"over_image" : GUILD_PATH.format("/home/give_item_hover.png"),
			"down_image" : GUILD_PATH.format("/home/give_item_down.png"),
		},
		{
			"name" : "DonateItem3",
			"type" : "button",
			"x" : 407+89+85,
			"y" : 286,
			#"text" : uiScriptLocale.GUILD_INFO_DECALRE_WAR,
			"default_image" : GUILD_PATH.format("/home/give_item_norm.png"),
			"over_image" : GUILD_PATH.format("/home/give_item_hover.png"),
			"down_image" : GUILD_PATH.format("/home/give_item_down.png"),
		},
		## Keep the count text overlays
		#{
		#	"name" : "item_1_count",
		#	"type":"text",
		#	"text":"0",
		#	"x":362,
		#	"y":277,
		#	"all_align":"center",
		#	"outline" : 1,
		#	"fontname":"Tahoma:12",
		#	"color":0xFFeed8c3,
		#},
		#{
		#	"name" : "item_2_count",
		#	"type":"text",
		#	"text":"0",
		#	"x":451,
		#	"y":277,
		#	"all_align":"center",
		#	"outline" : 1,
		#	"fontname":"Tahoma:12",
		#	"color":0xFFeed8c3,
		#},
		#{
		#	"name" : "item_3_count",
		#	"type":"text",
		#	"text":"0",
		#	"x":539,
		#	"y":277,
		#	"all_align":"center",
		#	"outline" : 1,
		#	"fontname":"Tahoma:12",
		#	"color":0xFFeed8c3,
		#},
		#{
		#	"name" : "UploadGuildSymbolButton",
		#	"type" : "button",
		#	"x" : 260,
		#	"y" : 33 + 23,
		#	"text" : uiScriptLocale.GUILD_INFO_UPLOAD_SYMBOL,
		#	"default_image" : "d:/ymir work/ui/public/large_button_01.sub",
		#	"over_image" : "d:/ymir work/ui/public/large_button_02.sub",
		#	"down_image" : "d:/ymir work/ui/public/large_button_03.sub",
		#},
		#{
		#	"name" : "GuildMemberLevelAverage", "type" : "text", "x" : 3, "y" : 197, "text" : uiScriptLocale.GUILD_INFO_MEMBER_AVG_LEVEL,
		#	"children" :
		#	(
		#		{
		#			"name" : "GuildMemberLevelAverageSlot",
		#			"type" : "image",
		#			"x" : 108,
		#			"y" : -2,
		#			"image" : SMALL_VALUE_FILE,
		#			"children" :
		#			(
		#				{"name" : "GuildMemberLevelAverageValue", "type":"text", "text":"53", "x":0, "y":0, "all_align":"center"},
		#			),
		#		},
		#	),
		#},
		#
		#{
		#	"name" : "OfferButton",
		#	"type" : "button",
		#	"x" : 127,
		#	"y" : 100,
		#	"text" : uiScriptLocale.GUILD_INFO_OFFER_EXP,
		#	"default_image" : "d:/ymir work/ui/public/small_button_01.sub",
		#	"over_image" : "d:/ymir work/ui/public/small_button_02.sub",
		#	"down_image" : "d:/ymir work/ui/public/small_button_03.sub",
		#},
		#
		#
		#{
		#	"name":"Guild_Mark_Title_Bar", "type":"horizontalbar", "x":188, "y":85, "width":167,
		#	"children" :
		#	(
		#
		#		{ "name":"Guild_Mark_Title", "type":"text", "x":8, "y":3, "text":uiScriptLocale.GUILD_INFO_ENEMY_GUILD, },
		#
		#		{
		#			"name" : "EnemyGuildSlot1",
		#			"type" : "image",
		#			"x" : 4,
		#			"y" : 27 + 26*0,
		#			"image" : XLARGE_VALUE_FILE,
		#			"children" :
		#			(
		#				{"name" : "EnemyGuildName1", "type":"text", "text":uiScriptLocale.GUILD_INFO_ENEMY_GUILD_EMPTY, "x":0, "y":0, "all_align":"center"},
		#			),
		#		},
		#		{
		#			"name" : "EnemyGuildSlot2",
		#			"type" : "image",
		#			"x" : 4,
		#			"y" : 27 + 26*1,
		#			"image" : XLARGE_VALUE_FILE,
		#			"children" :
		#			(
		#				{"name" : "EnemyGuildName2", "type":"text", "text":uiScriptLocale.GUILD_INFO_ENEMY_GUILD_EMPTY, "x":0, "y":0, "all_align":"center"},
		#			),
		#		},
		#		{
		#			"name" : "EnemyGuildSlot3",
		#			"type" : "image",
		#			"x" : 4,
		#			"y" : 27 + 26*2,
		#			"image" : XLARGE_VALUE_FILE,
		#			"children" :
		#			(
		#				{"name" : "EnemyGuildName3", "type":"text", "text":uiScriptLocale.GUILD_INFO_ENEMY_GUILD_EMPTY, "x":0, "y":0, "all_align":"center"},
		#			),
		#		},
		#		{
		#			"name" : "EnemyGuildSlot4",
		#			"type" : "image",
		#			"x" : 4,
		#			"y" : 27 + 26*3,
		#			"image" : XLARGE_VALUE_FILE,
		#			"children" :
		#			(
		#				{"name" : "EnemyGuildName4", "type":"text", "text":uiScriptLocale.GUILD_INFO_ENEMY_GUILD_EMPTY, "x":0, "y":0, "all_align":"center"},
		#			),
		#		},
		#		{
		#			"name" : "EnemyGuildSlot5",
		#			"type" : "image",
		#			"x" : 4,
		#			"y" : 27 + 26*4,
		#			"image" : XLARGE_VALUE_FILE,
		#			"children" :
		#			(
		#				{"name" : "EnemyGuildName5", "type":"text", "text":uiScriptLocale.GUILD_INFO_ENEMY_GUILD_EMPTY, "x":0, "y":0, "all_align":"center"},
		#			),
		#		},
		#		{
		#			"name" : "EnemyGuildSlot6",
		#			"type" : "image",
		#			"x" : 4,
		#			"y" : 27 + 26*5,
		#			"image" : XLARGE_VALUE_FILE,
		#			"children" :
		#			(
		#				{"name" : "EnemyGuildName6", "type":"text", "text":uiScriptLocale.GUILD_INFO_ENEMY_GUILD_EMPTY, "x":0, "y":0, "all_align":"center"},
		#			),
		#		},
		#
		#	),
		#},
		#
		#{
		#	"name" : "EnemyGuildCancel1",
		#	"type" : "button",
		#	"x" : 310,
		#	"y" : 111 + 26*0,
		#	"text" : uiScriptLocale.CANCEL,
		#	"default_image" : "d:/ymir work/ui/public/small_button_01.sub",
		#	"over_image" : "d:/ymir work/ui/public/small_button_02.sub",
		#	"down_image" : "d:/ymir work/ui/public/small_button_03.sub",
		#},
		#{
		#	"name" : "EnemyGuildCancel2",
		#	"type" : "button",
		#	"x" : 310,
		#	"y" : 111 + 26*1,
		#	"text" : uiScriptLocale.CANCEL,
		#	"default_image" : "d:/ymir work/ui/public/small_button_01.sub",
		#	"over_image" : "d:/ymir work/ui/public/small_button_02.sub",
		#	"down_image" : "d:/ymir work/ui/public/small_button_03.sub",
		#},
		#{
		#	"name" : "EnemyGuildCancel3",
		#	"type" : "button",
		#	"x" : 310,
		#	"y" : 111 + 26*2,
		#	"text" : uiScriptLocale.CANCEL,
		#	"default_image" : "d:/ymir work/ui/public/small_button_01.sub",
		#	"over_image" : "d:/ymir work/ui/public/small_button_02.sub",
		#	"down_image" : "d:/ymir work/ui/public/small_button_03.sub",
		#},
		#{
		#	"name" : "EnemyGuildCancel4",
		#	"type" : "button",
		#	"x" : 310,
		#	"y" : 111 + 26*3,
		#	"text" : uiScriptLocale.CANCEL,
		#	"default_image" : "d:/ymir work/ui/public/small_button_01.sub",
		#	"over_image" : "d:/ymir work/ui/public/small_button_02.sub",
		#	"down_image" : "d:/ymir work/ui/public/small_button_03.sub",
		#},
		#{
		#	"name" : "EnemyGuildCancel5",
		#	"type" : "button",
		#	"x" : 310,
		#	"y" : 111 + 26*4,
		#	"text" : uiScriptLocale.CANCEL,
		#	"default_image" : "d:/ymir work/ui/public/small_button_01.sub",
		#	"over_image" : "d:/ymir work/ui/public/small_button_02.sub",
		#	"down_image" : "d:/ymir work/ui/public/small_button_03.sub",
		#},
		#{
		#	"name" : "EnemyGuildCancel6",
		#	"type" : "button",
		#	"x" : 310,
		#	"y" : 111 + 26*5,
		#	"text" : uiScriptLocale.CANCEL,
		#	"default_image" : "d:/ymir work/ui/public/small_button_01.sub",
		#	"over_image" : "d:/ymir work/ui/public/small_button_02.sub",
		#	"down_image" : "d:/ymir work/ui/public/small_button_03.sub",
		#},
		#
		{
			"name" : "DeclareWarButton",
			"type" : "button",
			"x" : 250 + 15,
			"y" : 264,
			"text" : uiScriptLocale.GUILD_INFO_DECALRE_WAR,
			"default_image" : "d:/ymir work/ui/public/large_button_01.sub",
			"over_image" : "d:/ymir work/ui/public/large_button_02.sub",
			"down_image" : "d:/ymir work/ui/public/large_button_03.sub",
		},
		{
			"name" : "TitleBar",
			"type" : "titlebar",
			"style" : ("attach",),
	
			"x" : 21,
			"y" : 12,
	
			"width" : 571,
			"color" : "red",
	
			"children" :
			(
				{
					"name" : "TitleName",
					"type" : "text",
					"text" : "Guild",
					"horizontal_align" : "center",
					"text_horizontal_align" : "center",
					"outline": 1,
					"x" : 0,
					"y" : 2,
				},
			),
		},
	),
}
