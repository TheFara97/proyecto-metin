import uiScriptLocale

GUILD_PATH = "elvion/game/guild{}"

window = {
	"name" : "GuildWindow_BaseInfoPage",

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
		
			"image" : GUILD_PATH.format("/guild_raids/bg.png"),
		},

		{
			"name" : "guild_raids_header",
			"type" : "window",
			"x" : 10, "y" : 35,
			"width" : 197, "height" : 26,
			"children":
			(
				{ 
					"name":"Guild_Info_Point_Value",
					"type":"text",
					"x":0,
					"y":0,
					"text":uiScriptLocale.GUILD_INFO_RAIDS,
					"outline" : 1,
					"fontname":"Tahoma:12",
					"color":0xFFdcc2a9,
					"all_align" : "center",
				},
			),
		},

		{
			"name" : "guild_raid_information_header",
			"type" : "window",
			"x" : 209, "y" : 35,
			"width" : 197, "height" : 26,
			"children":
			(
				{ 
					"name":"Guild_Info_Point_Value",
					"type":"text",
					"x":0,
					"y":0,
					"text":uiScriptLocale.GUILD_INFO_RAID_INFORMATION,
					"outline" : 1,
					"fontname":"Tahoma:12",
					"color":0xFFdcc2a9,
					"all_align" : "center",
				},
			),
		},

		{
			"name" : "guild_selected_raid_header",
			"type" : "window",
			"x" : 408, "y" : 35,
			"width" : 194, "height" : 26,
			"children":
			(
				{ 
					"name":"Guild_Info_Point_Value",
					"type":"text",
					"x":0,
					"y":0,
					"text":uiScriptLocale.GUILD_INFO_SELECTED_RAID,
					"outline" : 1,
					"fontname":"Tahoma:12",
					"color":0xFFdcc2a9,
					"all_align" : "center",
				},
			),
		},

		{
			"name" : "guild_possible_drop_header",
			"type" : "window",
			"x" : 209, "y" : 185,
			"width" : 196, "height" : 26,
			"children":
			(
				{ 
					"name":"Guild_Info_Point_Value",
					"type":"text",
					"x":0,
					"y":0,
					"text":uiScriptLocale.GUILD_INFO_RAID_DROP,
					"outline" : 1,
					"fontname":"Tahoma:12",
					"color":0xFFdcc2a9,
					"all_align" : "center",
				},
			),
		},
		
		{
			"name": "DungeonsScrollBar",
			"type": "modern_scrollbar",  # NEW
			"x" : 201,
			"y" : 64,
			"width": 4,
			"size": 243,  # Height of visible area
			"content_height": 250,  # Total scrollable content height (will be updated dynamically)
			"smooth": 1,
			"scroll_step": 0.30,
		},
		
		{
			"name" : "DungeonsListBox",
			"type" : "listboxex",

			"x" : 13,
			"y" : 67,
			
			"width" : 186,
			"height" : 242,
			
			"item_width" : 186,
			"item_height" : 91,
			"item_step" : 91,
			"view_count" : 3,
		},
		
		{ "name":"Dungeon_Recommended", "type":"text", "fontname":"Verdana:12b", "x":0, "y":-90, "all_align": "center", "outline": 1, "color":0xFFeed8c3, "text":"Recommended:", },
		
		{ "name":"Dungeon_Suggest_MembersCountType", "type":"text", "fontname":"Verdana:12", "x":216, "y":85, "outline": 1, "color":0xFF797978, "text":"Guild members", },
		{ "name":"Dungeon_Suggest_MembersCount", "type":"text", "fontname":"Verdana:12b", "x":76, "y":-72, "all_align": "center", "outline": 1, "text":"1", },
		{ "name":"Dungeon_Suggest_MonsterDefBonusType", "type":"text", "fontname":"Verdana:12", "x":216, "y":99, "outline": 1, "color":0xFF797978,"text":"Defence against monsters", },
		{ "name":"Dungeon_Suggest_MonsterDefBonus", "type":"text", "fontname":"Verdana:12b", "x":76, "y":-44, "all_align": "center", "outline": 1,"text":"1", },
		{ "name":"Dungeon_Suggest_LordAttBonusType", "type":"text", "fontname":"Verdana:12", "x":216, "y":113, "outline": 1, "color":0xFF797978,"text":"Strong against Emperors", },
		{ "name":"Dungeon_Suggest_LordAttBonus", "type":"text", "fontname":"Verdana:12b", "x":76, "y":-58, "all_align": "center", "outline": 1,"text":"1", },
		#{ "name":"Dungeon_Suggest_LordDefBonus", "type":"text", "fontname":"Tahoma:14", "x":446, "y":101, "text":"1", },
		#{ "name":"Dungeon_MaxLimitTime", "type":"text", "fontname":"Tahoma:14", "x":360, "y":115, "text":"1", },
		#{ "name":"Dungeon_DoneCount", "type":"text", "fontname":"Tahoma:14", "x":311, "y":129, "text":"1", },
		#{ "name":"Dungeon_BestDoneTime", "type":"text", "fontname":"Tahoma:14", "x":397, "y":144, "text":"1", },
		{ "name":"Dungeon_Recommended", "type":"text", "fontname":"Verdana:12b", "x":0, "y":-24, "all_align": "center", "outline": 1, "color":0xFFeed8c3, "text":"Guild gold drop amount:", },
		{ "name":"Dungeon_CanGetYangCount", "type":"text", "fontname":"Verdana:12", "x":0, "y":-7, "all_align": "center", "outline": 1, "color":0xFF9b8c72, "text":"100.000 Golds", },
		#{ "name":"Dungeon_CanGetExpCount", "type":"text", "fontname":"Tahoma:14", "x":423, "y":187, "text":"1", },
		
		{
			"name" : "dung_bg",
			"type" : "image",
			"x" : 411,
			"y" : 64,
			"image" : GUILD_PATH.format("/guild_raids/raid_img/raid_1.png"),
		},
		{
			"name" : "DungeonOpenButton",
			"type" : "button",
			"x" : 413,
			"y" : 244,
			"text" : "Entries 2/3",
			"default_image" : GUILD_PATH.format("/guild_raids/buy_ticket_norm.png"),
			"over_image" : GUILD_PATH.format("/guild_raids/buy_ticket_hover.png"),
			"down_image" : GUILD_PATH.format("/guild_raids/buy_ticket_down.png"),
			"disable_image" : GUILD_PATH.format("/guild_raids/buy_ticket_down.png"),
		},
		{
			"name" : "DungeonTeleportButton",
			"type" : "button",
			"x" : 413,
			"y" : 275,
			"text" : "Teleport",
			"default_image" : GUILD_PATH.format("/guild_raids/teleport_norm.png"),
			"over_image" : GUILD_PATH.format("/guild_raids/teleport_hover.png"),
			"down_image" : GUILD_PATH.format("/guild_raids/teleport_down.png"),
			"disable_image" : GUILD_PATH.format("/guild_raids/teleport_down.png"),
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