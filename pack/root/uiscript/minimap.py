#-*- coding: iso-8859-1 -*-
import emoji
import app

ROOT = "d:/ymir work/ui/minimap/"
NEW_PATH = "d:/ymir work/ui/game/NEW_PATH/"
NEW_PATH = "d:/ymir work/ui/minimap/"

TEXT_POS_X = 60

window = {
	"name" : "MiniMap",

	"x" : SCREEN_WIDTH - 176,
	"y" : 0,

	"width" : 186,
	"height" : 150,

	"children" :
	(
		{
			"name" : "OpenWindow",
			"type" : "window",

			"x" : 0,
			"y" : 0,

			"width" : 186,
			"height" : 150,

			"children" :
			(
				{
					"name" : "OpenWindowBGI",
					"type" : "image",
					"x" : 20,
					"y" : 1,
					"image" : NEW_PATH + "minimapa_bg.png",
				},
				{
					"name" : "MiniMapWindow",
					"type" : "window",

					"x" : 35,
					"y" : 20,

					"width" : 128,
					"height" : 128,
				},
				{
					"name" : "ScaleUpButton",
					"type" : "button",

					"x" : 94+24+20,
					"y" : 118,

					"default_image" : NEW_PATH + "plus_btn0.png",
					"over_image" : NEW_PATH + "plus_btn1.png",
					"down_image" : NEW_PATH + "plus_btn2.png",
				},
				{
					"name" : "ScaleDownButton",
					"type" : "button",

					"x" : 108+24+20,
					"y" : 104,

					"default_image" : NEW_PATH + "minus_btn0.png",
					"over_image" : NEW_PATH + "minus_btn1.png",
					"down_image" : NEW_PATH + "minus_btn2.png",
				},
				{
					"name" : "MiniMapHideButton",
					"type" : "button",

					"x" : 104+24+20,
					"y" : 7,

					"default_image" : NEW_PATH + "close_btn0.png",
					"over_image" : NEW_PATH + "close_btn1.png",
					"down_image" : NEW_PATH + "close_btn2.png",
				},
				{
					"name" : "AtlasShowButton",
					"type" : "button",

					"x" : 138+20,
					"y" : 61,

					"default_image" : NEW_PATH + "open_atlas_btn.png",
					"over_image" : NEW_PATH + "open_atlas_btn1.png",
					"down_image" : NEW_PATH + "open_atlas_btn2.png",
				},
				{
					"name" : "DungeonButton",
					"type" : "button",

					"x" : 29,
					"y" : 1,

					"default_image" : NEW_PATH + "dungeon_norm.png",
					"over_image" : NEW_PATH + "dungeon_hover.png",
					"down_image" : NEW_PATH + "dungeon_down.png",
				},
				{
					"name" : "MissionButton",
					"type" : "button",

					"x" : 14,
					"y" : 28,

					"default_image" : NEW_PATH + "mission_norm.png",
					"over_image" : NEW_PATH + "mission_hover.png",
					"down_image" : NEW_PATH + "mission_down.png",
				},
				{
					"name" : "TeleportButton",
					"type" : "button",

					"x" : 8,
					"y" : 59,

					"default_image" : NEW_PATH + "teleport_norm.png",
					"over_image" : NEW_PATH + "teleport_hover.png",
					"down_image" : NEW_PATH + "teleport_down.png",
				},
				{
					"name" : "CollectionButton",
					"type" : "button",

					"x" : 14,
					"y" : 90,

					"default_image" : NEW_PATH + "collection_norm.png",
					"over_image" : NEW_PATH + "collection_hover.png",
					"down_image" : NEW_PATH + "collection_down.png",
				},
				{
					"name" : "TitleButton",
					"type" : "button",

					"x" : 26,
					"y" : 118,

					"default_image" : NEW_PATH + "titles_norm.png",
					"over_image" : NEW_PATH + "titles_hover.png",
					"down_image" : NEW_PATH + "titles_down.png",
				},
				#{
				#	"name" : "Channel1",
				#	"type" : "radio_button",
				#	"x" : 5,
				#	"y" : 52,
				#	"default_image" : NEW_PATH + "ch1_0.png",
				#	"over_image" : NEW_PATH + "ch1_1.png",
				#	"down_image" : NEW_PATH + "ch1_2.png",
				#
				#	"tooltip_text_new" : emoji.AppendEmoji("icon/emoji/key_alt.png")+"+"+emoji.AppendEmoji("icon/emoji/key_f1.png"),
				#	"tooltip_text_color" : 0xfff1e6c0,
				#	"tooltip_x" : 0,
				#	"tooltip_y" : -40,
				#},
				#{
				#	"name" : "Channel2",
				#	"type" : "radio_button",
				#	"x" : 1,
				#	"y" : 72,
				#	"default_image" : NEW_PATH + "ch2_0.png",
				#	"over_image" : NEW_PATH + "ch2_1.png",
				#	"down_image" : NEW_PATH + "ch2_2.png",
				#
				#	"tooltip_text_new" : emoji.AppendEmoji("icon/emoji/key_alt.png")+"+"+emoji.AppendEmoji("icon/emoji/key_f2.png"),
				#	"tooltip_text_color" : 0xfff1e6c0,
				#	"tooltip_x" : 0,
				#	"tooltip_y" : -40,
				#},
				#{
				#	"name" : "Channel3",
				#	"type" : "radio_button",
				#	"x" : 4,
				#	"y" : 93,
				#	"default_image" : NEW_PATH + "ch3_0.png",
				#	"over_image" : NEW_PATH + "ch3_1.png",
				#	"down_image" : NEW_PATH + "ch3_2.png",
				#
				#	"tooltip_text_new" : emoji.AppendEmoji("icon/emoji/key_alt.png")+"+"+emoji.AppendEmoji("icon/emoji/key_f3.png"),
				#	"tooltip_text_color" : 0xfff1e6c0,
				#	"tooltip_x" : 0,
				#	"tooltip_y" : -40,
				#},
				#{
				#	"name" : "Channel4",
				#	"type" : "radio_button",
				#	"x" : 14,
				#	"y" : 110,
				#	"default_image" : NEW_PATH + "ch4_0.png",
				#	"over_image" : NEW_PATH + "ch4_1.png",
				#	"down_image" : NEW_PATH + "ch4_2.png",
				#
				#	"tooltip_text_new" : emoji.AppendEmoji("icon/emoji/key_alt.png")+"+"+emoji.AppendEmoji("icon/emoji/key_f4.png"),
				#	"tooltip_text_color" : 0xfff1e6c0,
				#	"tooltip_x" : 0,
				#	"tooltip_y" : -40,
				#},
				#{
				#	"name" : "Channel5",
				#	"type" : "radio_button",
				#	"x" : 29,
				#	"y" : 124,
				#	"default_image" : NEW_PATH + "ch5_0.png",
				#	"over_image" : NEW_PATH + "ch5_1.png",
				#	"down_image" : NEW_PATH + "ch5_2.png",
				#
				#	"tooltip_text_new" : emoji.AppendEmoji("icon/emoji/key_alt.png")+"+"+emoji.AppendEmoji("icon/emoji/key_f5.png"),
				#	"tooltip_text_color" : 0xfff1e6c0,
				#	"tooltip_x" : 0,
				#	"tooltip_y" : -40,
				#},
				#{
				#	"name" : "Channel6",
				#	"type" : "radio_button",
				#	"x" : 48,
				#	"y" : 132,
				#	"default_image" : NEW_PATH + "ch6_0.png",
				#	"over_image" : NEW_PATH + "ch6_1.png",
				#	"down_image" : NEW_PATH + "ch6_2.png",
				#
				#	"tooltip_text_new" : emoji.AppendEmoji("icon/emoji/key_alt.png")+"+"+emoji.AppendEmoji("icon/emoji/key_f6.png"),
				#	"tooltip_text_color" : 0xfff1e6c0,
				#	"tooltip_x" : 0,
				#	"tooltip_y" : -40,
				#},
				
				{
					"name" : "bgDown",
					"type" : "image",
					"x" : 38,
					"y" : 150,
					"image" : NEW_PATH + "bg_down.png",
					"children":
					(
						{
							"name" : "ServerInfo",
							"type" : "text",
							"text_horizontal_align" : "center",
							"outline" : 1,
							"x" : 68,
							"y" : 7,
							"text" : "1",
						},
						{
							"name": "dayInfo",
							"type": "text",
							"text_horizontal_align": "center",
							"outline": 1,
							"x" : 68,
							"y" : 19,
							"text": "1",
							"outline": True,
						},
						{
							"name" : "PositionInfo",
							"type" : "text",
							"text_horizontal_align" : "center",
							"outline" : 1,
							"x" : 68,
							"y" : 31,
							"text" : "1",
						},
					),
				},
				{
					"name": "fpsInfo",
					"type": "text",
					"text_horizontal_align": "center",
					"outline": 1,
					"x": TEXT_POS_X,
					"y": 232,
					"text": "",
					"outline": True,
				},



				{
					"name" : "ObserverCount",
					"type" : "text",

					"text_horizontal_align" : "center",

					"outline" : 1,

					"x" : 70,
					"y" : 180,

					"text" : "",
				},
			),
		},
		{
			"name" : "CloseWindow",
			"type" : "window",
		
			"x" : 15,
			"y" : 0,
		
			"width" : 132,
			"height" : 48,
		
			"children" :
			(
				{
					"name" : "MiniMapShowButton",
					"type" : "button",
		
					"x" : 100,
					"y" : 4,
		
					"default_image" : ROOT + "minimap_open_default.sub",
					"over_image" : ROOT + "minimap_open_default.sub",
					"down_image" : ROOT + "minimap_open_default.sub",
				},
			),
		},
	),
}