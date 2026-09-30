#-*- coding: iso-8859-1 -*-
import uiScriptLocale
import app
import emoji
import localeInfo

ROOT_PATH = "d:/ymir work/ui/public/"
PATH = "d:/ymir work/ui/new_weekly_rank/"
NEW_PATH = "d:/ymir work/ui/settings/"
ETC_PATH = "d:/ymir work/ui/"

TEMPORARY_X = +13
BUTTON_TEMPORARY_X = 5
PVP_X = -10

LINE_LABEL_X 	= 20
LINE_LABEL_Y	= 10

LINE_DATA_X 	= 120
LINE_BEGIN	= 40
LINE_STEP	= 25

THINBOARD_WIDTH	= 410
THINBOARD_HEIGHT = 335

MNOZENIE = 1

SMALL_BUTTON_WIDTH 	= 45
MIDDLE_BUTTON_WIDTH 	= 65

BOARD_WIDTH = 590
BOARD_HEIGHT = 375

window = {
	"name" : "GameOptionDialog",
	"style" : ["movable", "float",],

	"x" : 0,
	"y" : 0,

	"width" : BOARD_WIDTH,
	"height" : BOARD_HEIGHT,

	"children" :
	[
		{
			"name" : "board",
			"type" : "board",

			"x" : 0,
			"y" : 0,

			"width" : BOARD_WIDTH,
			"height" : BOARD_HEIGHT,

			"children" :
			[
				{
					"name" : "titlebar",
					"type" : "titlebar",
					"style" : ["attach",],

					"x" : 21,
					"y" : 8,

					"width" : BOARD_WIDTH-16-28,
					"color" : "gray",

					"children" :
					[
						{ "name":"titlename", "type":"text", "x":0, "y":0,
						"text" : uiScriptLocale.GAMEOPTION_TITLE,"outline":1,
						"horizontal_align":"center", "text_horizontal_align":"center" },
					],
				},
				{
					"name":"global_window",
					"type":"window",
					"x":169,"y":30,
					"width":400,
					"height":300,
					"children":
					(
						{
							"name":"thinboard0",
							"type":"thinboard_circle",
							"x":0,"y":0,
							"width" : THINBOARD_WIDTH,
							"height" : THINBOARD_HEIGHT,
							"children":
							(
								{
									"name":"bg",
									"type":"image",
									"x":1,"y":1,
									"image":NEW_PATH+"def_bg.png",
								},
								{
									"name" : "name_color",
									"type" : "text",

									"x" : LINE_LABEL_X,
									"y" : LINE_LABEL_Y+2,

									"text" : uiScriptLocale.OPTION_NAME_COLOR,
								},
								{
									"name" : "name_color_normal",
									"type" : "radio_button",
									"x" : LINE_DATA_X+MIDDLE_BUTTON_WIDTH*0,
									"y" : LINE_LABEL_Y,
									"default_image" : "d:/ymir work/ui/public/middle_button_01.sub",
									"over_image" : "d:/ymir work/ui/public/middle_button_02.sub",
									"down_image" : "d:/ymir work/ui/public/middle_button_03.sub",
									"children":
									(
										{
											"name" : "btn_text",
											"type" : "text",
											"x": 0, "y": 0,
											"all_align":"center",
											"text":uiScriptLocale.OPTION_NAME_COLOR_NORMAL,
											"outline":1,
											"fontname":"Tahoma:12",
										},
									),
								},
								{
									"name" : "name_color_empire",
									"type" : "radio_button",
									"x" : LINE_DATA_X+MIDDLE_BUTTON_WIDTH*MNOZENIE,
									"y" : LINE_LABEL_Y,
									"default_image" : "d:/ymir work/ui/public/middle_button_01.sub",
									"over_image" : "d:/ymir work/ui/public/middle_button_02.sub",
									"down_image" : "d:/ymir work/ui/public/middle_button_03.sub",
									"children":
									(
										{
											"name" : "btn_text",
											"type" : "text",
											"x": 0, "y": 0,
											"all_align":"center",
											"text" : uiScriptLocale.OPTION_NAME_COLOR_EMPIRE,
											"outline":1,
											"fontname":"Tahoma:12",
										},
									),
								},

								{
									"name" : "rectangle",
									"type" : "image",
									"x" : LINE_LABEL_X-20,
									"y" : LINE_LABEL_Y+22,
									"image" : NEW_PATH+"rectangle.png",
								},

								{
									"name" : "target_board",
									"type" : "text",

									"x" : LINE_LABEL_X,
									"y" : LINE_LABEL_Y+27,

									"text" : uiScriptLocale.OPTION_TARGET_BOARD,
								},
								{
									"name" : "target_board_no_view",
									"type" : "radio_button",

									"x" : LINE_DATA_X+MIDDLE_BUTTON_WIDTH*0,
									"y" : LINE_LABEL_Y+25,

									"default_image" : "d:/ymir work/ui/public/middle_button_01.sub",
									"over_image" : "d:/ymir work/ui/public/middle_button_02.sub",
									"down_image" : "d:/ymir work/ui/public/middle_button_03.sub",
									"children":
									(
										{
											"name" : "btn_text",
											"type" : "text",
											"x": 0, "y": 0,
											"all_align":"center",
											"text" : uiScriptLocale.OPTION_TARGET_BOARD_NO_VIEW,
											"outline":1,
											"fontname":"Tahoma:12",
										},
									),
								},
								{
									"name" : "target_board_view",
									"type" : "radio_button",

									"x" : LINE_DATA_X+MIDDLE_BUTTON_WIDTH*MNOZENIE,
									"y" : LINE_LABEL_Y+25,

									"default_image" : "d:/ymir work/ui/public/middle_button_01.sub",
									"over_image" : "d:/ymir work/ui/public/middle_button_02.sub",
									"down_image" : "d:/ymir work/ui/public/middle_button_03.sub",
									"children":
									(
										{
											"name" : "btn_text",
											"type" : "text",
											"x": 0, "y": 0,
											"all_align":"center",
											"text" : uiScriptLocale.OPTION_TARGET_BOARD_VIEW,
											"outline":1,
											"fontname":"Tahoma:12",
										},
									),
								},

								{
									"name" : "rectangle",
									"type" : "image",
									"x" : LINE_LABEL_X-20,
									"y" : LINE_LABEL_Y+47,
									"image" : NEW_PATH+"rectangle.png",
								},


								{
									"name" : "pvp_mode",
									"type" : "text",

									"x" : LINE_LABEL_X,
									"y" : LINE_LABEL_Y+52,

									"text" : uiScriptLocale.OPTION_PVPMODE,
								},
								{
									"name" : "pvp_peace",
									"type" : "radio_button",

									"x" : LINE_DATA_X+SMALL_BUTTON_WIDTH*0,
									"y" : LINE_LABEL_Y+50,

									"tooltip_text_new" : uiScriptLocale.OPTION_PVPMODE_PEACE_TOOLTIP,
									"tooltip_text_color" : 0xfff1e6c0,
									"tooltip_x" : 40,
									"tooltip_y" : -30,
									"outline":1,

									"default_image" : "d:/ymir work/ui/public/middle_button_01.sub",
									"over_image" : "d:/ymir work/ui/public/middle_button_02.sub",
									"down_image" : "d:/ymir work/ui/public/middle_button_03.sub",
									"children":
									(
										{
											"name" : "btn_text",
											"type" : "text",
											"x": 0, "y": 0,
											"all_align":"center",
											"text" : uiScriptLocale.OPTION_PVPMODE_PEACE,
											"outline":1,
											"fontname":"Tahoma:12",
										},
									),
								},
								{
									"name" : "pvp_revenge",
									"type" : "radio_button",

									"x" : LINE_DATA_X+MIDDLE_BUTTON_WIDTH*MNOZENIE,
									"y" : LINE_LABEL_Y+50,

									"tooltip_text_new" : uiScriptLocale.OPTION_PVPMODE_REVENGE_TOOLTIP,
									"tooltip_text_color" : 0xfff1e6c0,
									"tooltip_x" : 10,
									"tooltip_y" : -30,

									"default_image" : "d:/ymir work/ui/public/middle_button_01.sub",
									"over_image" : "d:/ymir work/ui/public/middle_button_02.sub",
									"down_image" : "d:/ymir work/ui/public/middle_button_03.sub",
									"children":
									(
										{
											"name" : "btn_text",
											"type" : "text",
											"x": 0, "y": 0,
											"all_align":"center",
											"text" : uiScriptLocale.OPTION_PVPMODE_REVENGE,
											"outline":1,
											"fontname":"Tahoma:12",
										},
									),
								},
								{
									"name" : "pvp_guild",
									"type" : "radio_button",

									"x" : LINE_DATA_X+MIDDLE_BUTTON_WIDTH*MNOZENIE*1.98,
									"y" : LINE_LABEL_Y+50,

									"tooltip_text_new" : uiScriptLocale.OPTION_PVPMODE_GUILD_TOOLTIP,
									"tooltip_text_color" : 0xfff1e6c0,
									"tooltip_x" : 10,
									"tooltip_y" : -30,

									"default_image" : "d:/ymir work/ui/public/middle_button_01.sub",
									"over_image" : "d:/ymir work/ui/public/middle_button_02.sub",
									"down_image" : "d:/ymir work/ui/public/middle_button_03.sub",
									"children":
									(
										{
											"name" : "btn_text",
											"type" : "text",
											"x": 0, "y": 0,
											"all_align":"center",
											"text" : uiScriptLocale.OPTION_PVPMODE_GUILD,
											"outline":1,
											"fontname":"Tahoma:12",
										},
									),
								},
								{
									"name" : "pvp_free",
									"type" : "radio_button",

									"x" : LINE_DATA_X+MIDDLE_BUTTON_WIDTH*MNOZENIE*2.96,
									"y" : LINE_LABEL_Y+50,

									"tooltip_text_new" : uiScriptLocale.OPTION_PVPMODE_FREE_TOOLTIP,
									"tooltip_text_color" : 0xfff1e6c0,
									"tooltip_x" : 10,
									"tooltip_y" : -30,

									"default_image" : "d:/ymir work/ui/public/middle_button_01.sub",
									"over_image" : "d:/ymir work/ui/public/middle_button_02.sub",
									"down_image" : "d:/ymir work/ui/public/middle_button_03.sub",
									"children":
									(
										{
											"name" : "btn_text",
											"type" : "text",
											"x": 0, "y": 0,
											"all_align":"center",
											"text" : uiScriptLocale.OPTION_PVPMODE_FREE,
											"outline":1,
											"fontname":"Tahoma:12",
										},
									),
								},

								{
									"name" : "rectangle",
									"type" : "image",
									"x" : LINE_LABEL_X-20,
									"y" : LINE_LABEL_Y+72,
									"image" : NEW_PATH+"rectangle.png",
								},

								{
									"name" : "block",
									"type" : "text",

									"x" : LINE_LABEL_X,
									"y" : LINE_LABEL_Y+77,

									"text" : uiScriptLocale.OPTION_BLOCK,
								},
								{
									"name" : "block_exchange_button",
									"type" : "toggle_button",

									"x" : LINE_DATA_X+MIDDLE_BUTTON_WIDTH*0,
									"y" : LINE_LABEL_Y+75,

									"default_image" : "d:/ymir work/ui/public/middle_button_01.sub",
									"over_image" : "d:/ymir work/ui/public/middle_button_02.sub",
									"down_image" : "d:/ymir work/ui/public/middle_button_03.sub",
									"children":
									(
										{
											"name" : "btn_text",
											"type" : "text",
											"x": 0, "y": 0,
											"all_align":"center",
											"text" : uiScriptLocale.OPTION_BLOCK_EXCHANGE,
											"outline":1,
											"fontname":"Tahoma:12",
										},
									),
								},
								{
									"name" : "block_party_button",
									"type" : "toggle_button",

									"x" : LINE_DATA_X+MIDDLE_BUTTON_WIDTH*MNOZENIE,
									"y" : LINE_LABEL_Y+75,

									"default_image" : "d:/ymir work/ui/public/middle_button_01.sub",
									"over_image" : "d:/ymir work/ui/public/middle_button_02.sub",
									"down_image" : "d:/ymir work/ui/public/middle_button_03.sub",
									"children":
									(
										{
											"name" : "btn_text",
											"type" : "text",
											"x": 0, "y": 0,
											"all_align":"center",
											"text" : uiScriptLocale.OPTION_BLOCK_PARTY,
											"outline":1,
											"fontname":"Tahoma:12",
										},
									),
								},
								{
									"name" : "block_guild_button",
									"type" : "toggle_button",

									"x" : LINE_DATA_X+MIDDLE_BUTTON_WIDTH*MNOZENIE*1.98,
									"y" : LINE_LABEL_Y+75,

									"default_image" : "d:/ymir work/ui/public/middle_button_01.sub",
									"over_image" : "d:/ymir work/ui/public/middle_button_02.sub",
									"down_image" : "d:/ymir work/ui/public/middle_button_03.sub",
									"children":
									(
										{
											"name" : "btn_text",
											"type" : "text",
											"x": 0, "y": 0,
											"all_align":"center",
											"text" : uiScriptLocale.OPTION_BLOCK_GUILD,
											"outline":1,
											"fontname":"Tahoma:12",
										},
									),
								},
								{
									"name" : "block_whisper_button",
									"type" : "toggle_button",

									"x" : LINE_DATA_X+MIDDLE_BUTTON_WIDTH*0,
									"y" : LINE_LABEL_Y+100,

									"default_image" : "d:/ymir work/ui/public/middle_button_01.sub",
									"over_image" : "d:/ymir work/ui/public/middle_button_02.sub",
									"down_image" : "d:/ymir work/ui/public/middle_button_03.sub",
									"children":
									(
										{
											"name" : "btn_text",
											"type" : "text",
											"x": 0, "y": 0,
											"all_align":"center",
											"text" : uiScriptLocale.OPTION_BLOCK_WHISPER,
											"outline":1,
											"fontname":"Tahoma:12",
										},
									),
								},
								{
									"name" : "block_friend_button",
									"type" : "toggle_button",

									"x" : LINE_DATA_X+MIDDLE_BUTTON_WIDTH*MNOZENIE,
									"y" : LINE_LABEL_Y+100,

									"default_image" : "d:/ymir work/ui/public/middle_button_01.sub",
									"over_image" : "d:/ymir work/ui/public/middle_button_02.sub",
									"down_image" : "d:/ymir work/ui/public/middle_button_03.sub",
									"children":
									(
										{
											"name" : "btn_text",
											"type" : "text",
											"x": 0, "y": 0,
											"all_align":"center",
											"text" : uiScriptLocale.OPTION_BLOCK_FRIEND,
											"outline":1,
											"fontname":"Tahoma:12",
										},
									),
								},
								{
									"name" : "block_party_request_button",
									"type" : "toggle_button",

									"x" : LINE_DATA_X+MIDDLE_BUTTON_WIDTH*MNOZENIE*1.98,
									"y" : LINE_LABEL_Y+100,

									"default_image" : "d:/ymir work/ui/public/middle_button_01.sub",
									"over_image" : "d:/ymir work/ui/public/middle_button_02.sub",
									"down_image" : "d:/ymir work/ui/public/middle_button_03.sub",
									"children":
									(
										{
											"name" : "btn_text",
											"type" : "text",
											"x": 0, "y": 0,
											"all_align":"center",
											"text" : uiScriptLocale.OPTION_BLOCK_PARTY_REQUEST,
											"outline":1,
											"fontname":"Tahoma:12",
										},
									),
								},

								{
									"name" : "rectangle",
									"type" : "image",
									"x" : LINE_LABEL_X-20,
									"y" : LINE_LABEL_Y+122,
									"image" : NEW_PATH+"rectangle.png",
								},

								{
									"name" : "chat",
									"type" : "text",

									"x" : LINE_LABEL_X,
									"y" : LINE_LABEL_Y+127,

									"text" : uiScriptLocale.OPTION_VIEW_CHAT,
								},
								{
									"name" : "view_chat_on_button",
									"type" : "radio_button",

									"x" : LINE_DATA_X,
									"y" : LINE_LABEL_Y+125,

									"default_image" : "d:/ymir work/ui/public/middle_button_01.sub",
									"over_image" : "d:/ymir work/ui/public/middle_button_02.sub",
									"down_image" : "d:/ymir work/ui/public/middle_button_03.sub",
									"children":
									(
										{
											"name" : "btn_text",
											"type" : "text",
											"x": 0, "y": 0,
											"all_align":"center",
											"text" : uiScriptLocale.OPTION_VIEW_CHAT_ON,
											"outline":1,
											"fontname":"Tahoma:12",
										},
									),
								},
								{
									"name" : "view_chat_off_button",
									"type" : "radio_button",

									"x" : LINE_DATA_X+MIDDLE_BUTTON_WIDTH*MNOZENIE,
									"y" : LINE_LABEL_Y+125,

									"default_image" : "d:/ymir work/ui/public/middle_button_01.sub",
									"over_image" : "d:/ymir work/ui/public/middle_button_02.sub",
									"down_image" : "d:/ymir work/ui/public/middle_button_03.sub",
									"children":
									(
										{
											"name" : "btn_text",
											"type" : "text",
											"x": 0, "y": 0,
											"all_align":"center",
											"text" : uiScriptLocale.OPTION_VIEW_CHAT_OFF,
											"outline":1,
											"fontname":"Tahoma:12",
										},
									),
								},

								{
									"name" : "rectangle",
									"type" : "image",
									"x" : LINE_LABEL_X-20,
									"y" : LINE_LABEL_Y+147,
									"image" : NEW_PATH+"rectangle.png",
								},

								{
									"name" : "always_show_name",
									"type" : "text",

									"x" : LINE_LABEL_X,
									"y" : LINE_LABEL_Y+152,

									"text" : uiScriptLocale.OPTION_ALWAYS_SHOW_NAME,
								},
								{
									"name" : "always_show_name_on_button",
									"type" : "radio_button",

									"x" : LINE_DATA_X,
									"y" : LINE_LABEL_Y+150,

									"default_image" : "d:/ymir work/ui/public/middle_button_01.sub",
									"over_image" : "d:/ymir work/ui/public/middle_button_02.sub",
									"down_image" : "d:/ymir work/ui/public/middle_button_03.sub",
									"children":
									(
										{
											"name" : "btn_text",
											"type" : "text",
											"x": 0, "y": 0,
											"all_align":"center",
											"text" : uiScriptLocale.OPTION_ALWAYS_SHOW_NAME_ON,
											"outline":1,
											"fontname":"Tahoma:12",
										},
									),
								},
								{
									"name" : "always_show_name_off_button",
									"type" : "radio_button",

									"x" : LINE_DATA_X+MIDDLE_BUTTON_WIDTH*MNOZENIE,
									"y" : LINE_LABEL_Y+150,

									"default_image" : "d:/ymir work/ui/public/middle_button_01.sub",
									"over_image" : "d:/ymir work/ui/public/middle_button_02.sub",
									"down_image" : "d:/ymir work/ui/public/middle_button_03.sub",
									"children":
									(
										{
											"name" : "btn_text",
											"type" : "text",
											"x": 0, "y": 0,
											"all_align":"center",
											"text" : uiScriptLocale.OPTION_ALWAYS_SHOW_NAME_OFF,
											"outline":1,
											"fontname":"Tahoma:12",
										},
									),
								},

								{
									"name" : "rectangle",
									"type" : "image",
									"x" : LINE_LABEL_X-20,
									"y" : LINE_LABEL_Y+172,
									"image" : NEW_PATH+"rectangle.png",
								},

								{
									"name" : "effect_on_off",
									"type" : "text",

									"x" : LINE_LABEL_X,
									"y" : LINE_LABEL_Y+177,

									"text" : uiScriptLocale.OPTION_EFFECT,
								},
								{
									"name" : "show_damage_on_button",
									"type" : "radio_button",

									"x" : LINE_DATA_X,
									"y" : LINE_LABEL_Y+175,

									"default_image" : "d:/ymir work/ui/public/middle_button_01.sub",
									"over_image" : "d:/ymir work/ui/public/middle_button_02.sub",
									"down_image" : "d:/ymir work/ui/public/middle_button_03.sub",
									"children":
									(
										{
											"name" : "btn_text",
											"type" : "text",
											"x": 0, "y": 0,
											"all_align":"center",
											"text" : uiScriptLocale.OPTION_VIEW_CHAT_ON,
											"outline":1,
											"fontname":"Tahoma:12",
										},
									),
								},
								{
									"name" : "show_damage_off_button",
									"type" : "radio_button",

									"x" : LINE_DATA_X+MIDDLE_BUTTON_WIDTH*MNOZENIE,
									"y" : LINE_LABEL_Y+175,

									"default_image" : "d:/ymir work/ui/public/middle_button_01.sub",
									"over_image" : "d:/ymir work/ui/public/middle_button_02.sub",
									"down_image" : "d:/ymir work/ui/public/middle_button_03.sub",
									"children":
									(
										{
											"name" : "btn_text",
											"type" : "text",
											"x": 0, "y": 0,
											"all_align":"center",
											"text" : uiScriptLocale.OPTION_VIEW_CHAT_OFF,
											"outline":1,
											"fontname":"Tahoma:12",
										},
									),
								},

								{
									"name" : "rectangle",
									"type" : "image",
									"x" : LINE_LABEL_X-20,
									"y" : LINE_LABEL_Y+197,
									"image" : NEW_PATH+"rectangle.png",
								},

								{
									"name" : "salestext_on_off",
									"type" : "text",

									"x" : LINE_LABEL_X,
									"y" : LINE_LABEL_Y+202,

									"text" : uiScriptLocale.OPTION_SALESTEXT,
								},
								{
									"name" : "salestext_on_button",
									"type" : "radio_button",

									"x" : LINE_DATA_X,
									"y" : LINE_LABEL_Y+200,

									"default_image" : "d:/ymir work/ui/public/middle_button_01.sub",
									"over_image" : "d:/ymir work/ui/public/middle_button_02.sub",
									"down_image" : "d:/ymir work/ui/public/middle_button_03.sub",
									"children":
									(
										{
											"name" : "btn_text",
											"type" : "text",
											"x": 0, "y": 0,
											"all_align":"center",
											"text" : uiScriptLocale.OPTION_SALESTEXT_VIEW_ON,
											"outline":1,
											"fontname":"Tahoma:12",
										},
									),
								},
								{
									"name" : "salestext_off_button",
									"type" : "radio_button",

									"x" : LINE_DATA_X+MIDDLE_BUTTON_WIDTH*MNOZENIE,
									"y" : LINE_LABEL_Y+200,

									"default_image" : "d:/ymir work/ui/public/middle_button_01.sub",
									"over_image" : "d:/ymir work/ui/public/middle_button_02.sub",
									"down_image" : "d:/ymir work/ui/public/middle_button_03.sub",
									"children":
									(
										{
											"name" : "btn_text",
											"type" : "text",
											"x": 0, "y": 0,
											"all_align":"center",
											"text" : uiScriptLocale.OPTION_SALESTEXT_VIEW_OFF,
											"outline":1,
											"fontname":"Tahoma:12",
										},
									),
								},

								{
									"name" : "rectangle",
									"type" : "image",
									"x" : LINE_LABEL_X-20,
									"y" : LINE_LABEL_Y+222,
									"image" : NEW_PATH+"rectangle.png",
								},

								{
									"name" : "dogmode_on_off",
									"type" : "text",

									"multi_line" : 1,

									"x" : LINE_LABEL_X,
									"y" : LINE_LABEL_Y+227,

									"text" : uiScriptLocale.GAME_OPTION_MONSTERS,
								},
								{
									"name" : "dog_mode_open",
									"type" : "radio_button",

									"x" : LINE_DATA_X,
									"y" : LINE_LABEL_Y+225,

									"default_image" : "d:/ymir work/ui/public/middle_button_01.sub",
									"over_image" : "d:/ymir work/ui/public/middle_button_02.sub",
									"down_image" : "d:/ymir work/ui/public/middle_button_03.sub",
									"children":
									(
										{
											"name" : "btn_text",
											"type" : "text",
											"x": 0, "y": 0,
											"all_align":"center",
											"text" : uiScriptLocale.GAME_OPTION_MONSTERS_DOGS,
											"outline":1,
											"fontname":"Tahoma:12",
										},
									),
								},
								{
									"name" : "dog_mode_close",
									"type" : "radio_button",

									"x" : LINE_DATA_X+MIDDLE_BUTTON_WIDTH*MNOZENIE,
									"y" : LINE_LABEL_Y+225,

									"default_image" : "d:/ymir work/ui/public/middle_button_01.sub",
									"over_image" : "d:/ymir work/ui/public/middle_button_02.sub",
									"down_image" : "d:/ymir work/ui/public/middle_button_03.sub",
									"children":
									(
										{
											"name" : "btn_text",
											"type" : "text",
											"x": 0, "y": 0,
											"all_align":"center",
											"text" : uiScriptLocale.GAME_OPTION_MONSTERS_NORMAL,
											"outline":1,
											"fontname":"Tahoma:12",
										},
									),
								},
								{
									"name" : "rectangle",
									"type" : "image",
									"x" : LINE_LABEL_X-20,
									"y" : LINE_LABEL_Y+247,
									"image" : NEW_PATH+"rectangle.png",
								},
								{
									"name" : "show_mob_info",
									"type" : "text",

									"multi_line" : 1,

									"x" : LINE_LABEL_X,
									"y" : LINE_LABEL_Y+252,

									"text" : uiScriptLocale.OPTION_MOB_INFO,
								},
								{
									"name" : "show_mob_level_button",
									"type" : "toggle_button",

									"x" : LINE_DATA_X,
									"y" : LINE_LABEL_Y+250,

									"default_image" : "d:/ymir work/ui/public/middle_button_01.sub",
									"over_image" : "d:/ymir work/ui/public/middle_button_02.sub",
									"down_image" : "d:/ymir work/ui/public/middle_button_03.sub",
									"children":
									(
										{
											"name" : "btn_text",
											"type" : "text",
											"x": 0, "y": 0,
											"all_align":"center",
											"text" : uiScriptLocale.OPTION_MOB_INFO_LEVEL,
											"outline":1,
											"fontname":"Tahoma:12",
										},
									),
								},
								{
									"name" : "show_mob_AI_flag_button",
									"type" : "toggle_button",

									"x" : LINE_DATA_X+MIDDLE_BUTTON_WIDTH*MNOZENIE,
									"y" : LINE_LABEL_Y+250,

									"default_image" : "d:/ymir work/ui/public/middle_button_01.sub",
									"over_image" : "d:/ymir work/ui/public/middle_button_02.sub",
									"down_image" : "d:/ymir work/ui/public/middle_button_03.sub",
									"children":
									(
										{
											"name" : "btn_text",
											"type" : "text",
											"x": 0, "y": 0,
											"all_align":"center",
											"text" : uiScriptLocale.OPTION_MOB_INFO_AGGR,
											"outline":1,
											"fontname":"Tahoma:12",
										},
									),
								},
								{
									"name" : "effect_level",
									"type" : "text",
			
									"x" : LINE_LABEL_X,
									"y" : LINE_LABEL_Y+279,
			
									"text" : uiScriptLocale.GRAPHICONOFF_EFFECT_LEVEL, 
								},
								
								{
									"name" : "effect_level1",
									"type" : "radio_button",
			
									"x" : LINE_DATA_X,
									"y" : LINE_LABEL_Y+277,
			
									"text" :  uiScriptLocale.GRAPHICONOFF_EFFECT_LEVEL1,
									"tooltip_text" : uiScriptLocale.GRAPHICONOFF_EFFECT_LEVEL1_TOOLTIP,
			
									"default_image" : ROOT_PATH + "minimize_empty_button_01.sub",
									"over_image" : ROOT_PATH + "minimize_empty_button_02.sub",
									"down_image" : ROOT_PATH + "minimize_empty_button_03.sub",
								},
								
								{
									"name" : "effect_level2",
									"type" : "radio_button",
			
									"x" : LINE_DATA_X+25,
									"y" : LINE_LABEL_Y+275,
			
									"text" :  uiScriptLocale.GRAPHICONOFF_EFFECT_LEVEL2,
									"tooltip_text" : uiScriptLocale.GRAPHICONOFF_EFFECT_LEVEL2_TOOLTIP,
			
									"default_image" : ROOT_PATH + "minimize_empty_button_01.sub",
									"over_image" : ROOT_PATH + "minimize_empty_button_02.sub",
									"down_image" : ROOT_PATH + "minimize_empty_button_03.sub",
								},
								
								{
									"name" : "effect_level3",
									"type" : "radio_button",
			
									"x" : LINE_DATA_X+25+25,
									"y" : LINE_LABEL_Y+275,
			
									"text" :  uiScriptLocale.GRAPHICONOFF_EFFECT_LEVEL3,
									"tooltip_text" : uiScriptLocale.GRAPHICONOFF_EFFECT_LEVEL3_TOOLTIP,
			
									"default_image" : ROOT_PATH + "minimize_empty_button_01.sub",
									"over_image" : ROOT_PATH + "minimize_empty_button_02.sub",
									"down_image" : ROOT_PATH + "minimize_empty_button_03.sub",
								},
								
								{
									"name" : "effect_level4",
									"type" : "radio_button",
			
									"x" : LINE_DATA_X+25+25+25,
									"y" : LINE_LABEL_Y+275,
			
									"text" :  uiScriptLocale.GRAPHICONOFF_EFFECT_LEVEL4,
									"tooltip_text" : uiScriptLocale.GRAPHICONOFF_EFFECT_LEVEL4_TOOLTIP,
			
									"default_image" : ROOT_PATH + "minimize_empty_button_01.sub",
									"over_image" : ROOT_PATH + "minimize_empty_button_02.sub",
									"down_image" : ROOT_PATH + "minimize_empty_button_03.sub",
								},
								
								{
									"name" : "effect_level5",
									"type" : "radio_button",
			
									"x" : LINE_DATA_X+25+25+25+25,
									"y" : LINE_LABEL_Y+275,
			
									"text" :  uiScriptLocale.GRAPHICONOFF_EFFECT_LEVEL5,
									"tooltip_text" : uiScriptLocale.GRAPHICONOFF_EFFECT_LEVEL5_TOOLTIP,
			
									"default_image" : ROOT_PATH + "minimize_empty_button_01.sub",
									"over_image" : ROOT_PATH + "minimize_empty_button_02.sub",
									"down_image" : ROOT_PATH + "minimize_empty_button_03.sub",
								},
								
								{
									"name" : "effect_apply",
									"type" : "button",
			
									"x" : LINE_DATA_X+126,
									"y" : LINE_LABEL_Y+275,
			
									"text" : uiScriptLocale.GRAPHICONOFF_EFFECT_APPLY,
			
									"default_image" : "d:/ymir work/ui/public/middle_button_01.sub",
									"over_image" : "d:/ymir work/ui/public/middle_button_02.sub",
									"down_image" : "d:/ymir work/ui/public/middle_button_03.sub",
								},
							),
						},
					),
				},
				{
					"name":"system_window",
					"type":"window",
					"x":169,"y":30,
					"width":400,
					"height":360,
					"children":
					(
						{
							"name":"thinboard0",
							"type":"thinboard_circle",
							"x":0,"y":0,
							"width" : THINBOARD_WIDTH,
							"height" : THINBOARD_HEIGHT,
							"children":
							(
								{
									"name":"bg",
									"type":"image",
									"x":1,"y":1,
									"image":NEW_PATH+"def_bg.png",
								},
								{
									"name" : "music_name",
									"type" : "text",
									"x" : LINE_LABEL_X,
									"y" : LINE_LABEL_Y+34,
									"text" : uiScriptLocale.OPTION_MUSIC,
								},
								{
									"name" : "music_volume_controller",
									"type" : "sliderbarNew",
									"x" : LINE_LABEL_X+MIDDLE_BUTTON_WIDTH*1.2,
									"y" : LINE_LABEL_Y+37,
								},
								{
									"name" : "rectangle",
									"type" : "image",
									"x" : LINE_LABEL_X-20,
									"y" : LINE_LABEL_Y+25,
									"image" : NEW_PATH+"rectangle.png",
								},
								{
									"name" : "sound_name",
									"type" : "text",
									"x" : LINE_LABEL_X,
									"y" : LINE_LABEL_Y,
									"text" : uiScriptLocale.OPTION_SOUND,
								},
								{
									"name" : "sound_volume_controller",
									"type" : "sliderbarNew",

									"x" : LINE_LABEL_X+MIDDLE_BUTTON_WIDTH*1.2,
									"y" : LINE_LABEL_Y+3,
								},				
								#{
								#	"name": "effects_volume_controller",
								#	"type": "sliderbarNew",
								#	"x" : LINE_LABEL_X+MIDDLE_BUTTON_WIDTH*1.2,
								#	"y" : LINE_LABEL_Y+15,
								#},
								{
									"name" : "bgm_button",
									"type" : "button",
									"x" : LINE_LABEL_X-2,
									"y" : LINE_LABEL_Y+60,
									"default_image" : ROOT_PATH + "Middle_Button_01.sub",
									"over_image" : ROOT_PATH + "Middle_Button_02.sub",
									"down_image" : ROOT_PATH + "Middle_Button_03.sub",
									"children":
									(
										{
											"name" : "bgm_text",
											"type" : "text",
											"x": 0, "y": 0,
											"all_align":"center",
											"text" : uiScriptLocale.OPTION_MUSIC_CHANGE,
											"outline":1,
											"fontname":"Tahoma:12",
										},
									),
								},
								{
									"name" : "bgm_file",
									"type" : "text",
									"x" : LINE_LABEL_X+MIDDLE_BUTTON_WIDTH*1.75,
									"y" : LINE_LABEL_Y+62,
									"text" : uiScriptLocale.OPTION_MUSIC_DEFAULT_THEMA,
									"outline":1,
								},
								{
									"name" : "rectangle",
									"type" : "image",
									"x" : LINE_LABEL_X-20,
									"y" : LINE_LABEL_Y+90,
									"image" : NEW_PATH+"rectangle.png",
								},
								{
									"name" : "camera_mode",
									"type" : "text",
									"x" : LINE_LABEL_X,
									"y" : LINE_LABEL_Y+100,
									"text" : uiScriptLocale.OPTION_CAMERA_DISTANCE,
								},
								{
									"name" : "camera_short",
									"type" : "radio_button",
									"x" : LINE_LABEL_X+MIDDLE_BUTTON_WIDTH*1.9,
									"y" : LINE_LABEL_Y+99,
									"default_image" : ROOT_PATH + "Middle_Button_01.sub",
									"over_image" : ROOT_PATH + "Middle_Button_02.sub",
									"down_image" : ROOT_PATH + "Middle_Button_03.sub",
									"children":
									(
										{
											"name" : "camera_short_text",
											"type" : "text",
											"x": 0, "y": 0,
											"all_align":"center",
											"text" : uiScriptLocale.OPTION_CAMERA_DISTANCE_SHORT,
											"outline":1,
											"fontname":"Tahoma:12",
										},
									),
								},
								{
									"name" : "camera_long",
									"type" : "radio_button",
									"x" : LINE_LABEL_X+MIDDLE_BUTTON_WIDTH*3,
									"y" : LINE_LABEL_Y+99,
									"default_image" : ROOT_PATH + "Middle_Button_01.sub",
									"over_image" : ROOT_PATH + "Middle_Button_02.sub",
									"down_image" : ROOT_PATH + "Middle_Button_03.sub",
									"children":
									(
										{
											"name" : "camera_long_text",
											"type" : "text",
											"x": 0, "y": 0,
											"all_align":"center",
											"text" : uiScriptLocale.OPTION_CAMERA_DISTANCE_LONG,
											"outline":1,
											"fontname":"Tahoma:12",
										},
									),
								},
								{
									"name" : "rectangle",
									"type" : "image",
									"x" : LINE_LABEL_X-20,
									"y" : LINE_LABEL_Y+127,
									"image" : NEW_PATH+"rectangle.png",
								},

								{
									"name" : "fog_mode",
									"type" : "text",
									"x" : LINE_LABEL_X,
									"y" : LINE_LABEL_Y+136,
									"text" : uiScriptLocale.OPTION_FOG,
								},
								{
									"name" : "fog_level0",
									"type" : "radio_button",
									"x" : LINE_LABEL_X+MIDDLE_BUTTON_WIDTH*1.8,
									"y" : LINE_LABEL_Y+136,
									"default_image" : ROOT_PATH + "small_Button_01.sub",
									"over_image" : ROOT_PATH + "small_Button_02.sub",
									"down_image" : ROOT_PATH + "small_Button_03.sub",
									"children":
									(
										{
											"name" : "fog_level0_text",
											"type" : "text",
											"x": 0, "y": 0,
											"all_align":"center",
											"text" : uiScriptLocale.OPTION_FOG_DENSE,
											"outline":1,
											"fontname":"Tahoma:12",
										},
									),
								},
								{
									"name" : "fog_level1",
									"type" : "radio_button",
									"x" : LINE_LABEL_X+MIDDLE_BUTTON_WIDTH*2.6,
									"y" : LINE_LABEL_Y+136,
									"default_image" : ROOT_PATH + "small_Button_01.sub",
									"over_image" : ROOT_PATH + "small_Button_02.sub",
									"down_image" : ROOT_PATH + "small_Button_03.sub",
									"children":
									(
										{
											"name" : "fog_level1_text",
											"type" : "text",
											"x": 0, "y": 0,
											"all_align":"center",
											"text" : uiScriptLocale.OPTION_FOG_MIDDLE,
											"outline":1,
											"fontname":"Tahoma:12",
										},
									),
								},
								{
									"name" : "fog_level2",
									"type" : "radio_button",
									"x" : LINE_LABEL_X+MIDDLE_BUTTON_WIDTH*3.4,
									"y" : LINE_LABEL_Y+136,
									"default_image" : ROOT_PATH + "small_Button_01.sub",
									"over_image" : ROOT_PATH + "small_Button_02.sub",
									"down_image" : ROOT_PATH + "small_Button_03.sub",
									"children":
									(
										{
											"name" : "fog_level2_text",
											"type" : "text",
											"x": 0, "y": 0,
											"all_align":"center",
											"text" : uiScriptLocale.OPTION_FOG_LIGHT,
											"outline":1,
											"fontname":"Tahoma:12",
										},
									),
								},
								{
									"name" : "rectangle",
									"type" : "image",
									"x" : LINE_LABEL_X-20,
									"y" : LINE_LABEL_Y+163,
									"image" : NEW_PATH+"rectangle.png",
								},
								{
									"name" : "tiling_mode",
									"type" : "text",
									"x" : LINE_LABEL_X,
									"y" : LINE_LABEL_Y+173,
									"text" : uiScriptLocale.OPTION_TILING,
								},
								{
									"name" : "tiling_cpu",
									"type" : "radio_button",
									"x" : LINE_LABEL_X+MIDDLE_BUTTON_WIDTH*1.8,
									"y" : LINE_LABEL_Y+173,
									"default_image" : ROOT_PATH + "small_Button_01.sub",
									"over_image" : ROOT_PATH + "small_Button_02.sub",
									"down_image" : ROOT_PATH + "small_Button_03.sub",
									"children":
									(
										{
											"name" : "tilling_cpu_text",
											"type" : "text",
											"x": 0, "y": 0,
											"all_align":"center",
											"text" : uiScriptLocale.OPTION_TILING_CPU,
											"outline":1,
											"fontname":"Tahoma:12",
										},
									),
								},
								{
									"name" : "tiling_gpu",
									"type" : "radio_button",
									"x" : LINE_LABEL_X+MIDDLE_BUTTON_WIDTH*2.6,
									"y" : LINE_LABEL_Y+173,
									"default_image" : ROOT_PATH + "small_Button_01.sub",
									"over_image" : ROOT_PATH + "small_Button_02.sub",
									"down_image" : ROOT_PATH + "small_Button_03.sub",
									"children":
									(
										{
											"name" : "tiling_gpu_text",
											"type" : "text",
											"x": 0, "y": 0,
											"all_align":"center",
											"text" : uiScriptLocale.OPTION_TILING_GPU,
											"outline":1,
											"fontname":"Tahoma:12",
										},
									),
								},
								{
									"name" : "tiling_apply",
									"type" : "button",
									"x" : LINE_LABEL_X+MIDDLE_BUTTON_WIDTH*3.4,
									"y" : LINE_LABEL_Y+173,
									"default_image" : ROOT_PATH + "middle_Button_01.sub",
									"over_image" : ROOT_PATH + "middle_Button_02.sub",
									"down_image" : ROOT_PATH + "middle_Button_03.sub",
									"children":
									(
										{
											"name" : "tiling_gpu_text",
											"type" : "text",
											"x": 0, "y": 0,
											"all_align":"center",
											"text" : uiScriptLocale.OPTION_TILING_APPLY,
											"outline":1,
											"fontname":"Tahoma:12",
										},
									),
								},
								{
									"name" : "rectangle",
									"type" : "image",
									"x" : LINE_LABEL_X-20,
									"y" : LINE_LABEL_Y+201,
									"image" : NEW_PATH+"rectangle.png",
								},

								{
									"name" : "fov_option",
									"type" : "text",
									"x" : LINE_LABEL_X,
									"y" : LINE_LABEL_Y+210,
									"text" : uiScriptLocale.FOV_OPTION,
								},
								{
									"name" : "fov_controller",
									"type" : "sliderbarNew",
									"x" : LINE_LABEL_X+MIDDLE_BUTTON_WIDTH*1.2,
									"y" : LINE_LABEL_Y+211,
								},
								{
									"name" : "fov_reset_button",
									"type" : "button",
									"x" : LINE_LABEL_X+MIDDLE_BUTTON_WIDTH*5.32,
									"y" : LINE_LABEL_Y+210,

									"tooltip_text_new" : uiScriptLocale.FOV_RESET_OPTION_TOOLTIP,
									"tooltip_text_color" : 0xfff1e6c0,
									"tooltip_x" : 0,
									"tooltip_y" : -30,

									"default_image" : "d:/ymir work/ui/game/windows/reset_badge_button_default.tga",
									"over_image" : "d:/ymir work/ui/game/windows/reset_badge_button_over.tga",
									"down_image" : "d:/ymir work/ui/game/windows/reset_badge_button_down.tga",
								},
								{
									"name" : "fov_value_text",
									"type" : "text",
									"x" : LINE_LABEL_X+MIDDLE_BUTTON_WIDTH*4.85,
									"y" : LINE_LABEL_Y+211,
									"text" : "0",
									"outline":1,
									"fontname":"Tahoma:14b",
								},
								{
									"name" : "rectangle",
									"type" : "image",
									"x" : LINE_LABEL_X-20,
									"y" : LINE_LABEL_Y+241,
									"image" : NEW_PATH+"rectangle.png",
								},

								# Skybox selection section
								{
									"name" : "skybox_text",
									"type" : "text",
									"x" : LINE_LABEL_X,
									"y" : LINE_LABEL_Y+250,
									"text" : uiScriptLocale.GAME_OPTION_SKYBOX,
								},
								{
									"name" : "skybox_type_default",
									"type" : "radio_button",
									"x" : LINE_LABEL_X,
									"y" : LINE_LABEL_Y+270,
									"default_image" : ROOT_PATH + "small_Button_01.sub",
									"over_image" : ROOT_PATH + "small_Button_02.sub",
									"down_image" : ROOT_PATH + "small_Button_03.sub",
									"children":
									(
										{
											"name" : "skybox_default_text",
											"type" : "text",
											"x": 0, "y": 0,
											"all_align":"center",
											"text" : uiScriptLocale.GAME_OPTION_SKYBOX_DEFAULT,
											"outline":1,
											"fontname":"Tahoma:12",
										},
									),
								},
								{
									"name" : "skybox_type_1",
									"type" : "radio_button",
									"x" : LINE_LABEL_X+55,
									"y" : LINE_LABEL_Y+270,
									"default_image" : ROOT_PATH + "small_Button_01.sub",
									"over_image" : ROOT_PATH + "small_Button_02.sub",
									"down_image" : ROOT_PATH + "small_Button_03.sub",
									"children":
									(
										{
											"name" : "skybox_1_text",
											"type" : "text",
											"x": 0, "y": 0,
											"all_align":"center",
											"text" : "1",
											"outline":1,
											"fontname":"Tahoma:12",
										},
									),
								},
								{
									"name" : "skybox_type_2",
									"type" : "radio_button",
									"x" : LINE_LABEL_X+110,
									"y" : LINE_LABEL_Y+270,
									"default_image" : ROOT_PATH + "small_Button_01.sub",
									"over_image" : ROOT_PATH + "small_Button_02.sub",
									"down_image" : ROOT_PATH + "small_Button_03.sub",
									"children":
									(
										{
											"name" : "skybox_2_text",
											"type" : "text",
											"x": 0, "y": 0,
											"all_align":"center",
											"text" : "2",
											"outline":1,
											"fontname":"Tahoma:12",
										},
									),
								},
								{
									"name" : "skybox_type_3",
									"type" : "radio_button",
									"x" : LINE_LABEL_X+165,
									"y" : LINE_LABEL_Y+270,
									"default_image" : ROOT_PATH + "small_Button_01.sub",
									"over_image" : ROOT_PATH + "small_Button_02.sub",
									"down_image" : ROOT_PATH + "small_Button_03.sub",
									"children":
									(
										{
											"name" : "skybox_3_text",
											"type" : "text",
											"x": 0, "y": 0,
											"all_align":"center",
											"text" : "3",
											"outline":1,
											"fontname":"Tahoma:12",
										},
									),
								},
								{
									"name" : "skybox_type_4",
									"type" : "radio_button",
									"x" : LINE_LABEL_X,
									"y" : LINE_LABEL_Y+295,
									"default_image" : ROOT_PATH + "small_Button_01.sub",
									"over_image" : ROOT_PATH + "small_Button_02.sub",
									"down_image" : ROOT_PATH + "small_Button_03.sub",
									"children":
									(
										{
											"name" : "skybox_4_text",
											"type" : "text",
											"x": 0, "y": 0,
											"all_align":"center",
											"text" : "4",
											"outline":1,
											"fontname":"Tahoma:12",
										},
									),
								},
								{
									"name" : "skybox_type_5",
									"type" : "radio_button",
									"x" : LINE_LABEL_X+55,
									"y" : LINE_LABEL_Y+295,
									"default_image" : ROOT_PATH + "small_Button_01.sub",
									"over_image" : ROOT_PATH + "small_Button_02.sub",
									"down_image" : ROOT_PATH + "small_Button_03.sub",
									"children":
									(
										{
											"name" : "skybox_5_text",
											"type" : "text",
											"x": 0, "y": 0,
											"all_align":"center",
											"text" : "5",
											"outline":1,
											"fontname":"Tahoma:12",
										},
									),
								},
								{
									"name" : "skybox_type_6",
									"type" : "radio_button",
									"x" : LINE_LABEL_X+110,
									"y" : LINE_LABEL_Y+295,
									"default_image" : ROOT_PATH + "small_Button_01.sub",
									"over_image" : ROOT_PATH + "small_Button_02.sub",
									"down_image" : ROOT_PATH + "small_Button_03.sub",
									"children":
									(
										{
											"name" : "skybox_6_text",
											"type" : "text",
											"x": 0, "y": 0,
											"all_align":"center",
											"text" : "6",
											"outline":1,
											"fontname":"Tahoma:12",
										},
									),
								},
								{
									"name" : "skybox_type_7",
									"type" : "radio_button",
									"x" : LINE_LABEL_X+165,
									"y" : LINE_LABEL_Y+295,
									"default_image" : ROOT_PATH + "small_Button_01.sub",
									"over_image" : ROOT_PATH + "small_Button_02.sub",
									"down_image" : ROOT_PATH + "small_Button_03.sub",
									"children":
									(
										{
											"name" : "skybox_7_text",
											"type" : "text",
											"x": 0, "y": 0,
											"all_align":"center",
											"text" : "7",
											"outline":1,
											"fontname":"Tahoma:12",
										},
									),
								},
							),
						},
					),
				},
				{
					"name":"desc_window",
					"type":"window",
					"x":169,"y":30,
					"width":400,
					"height":300,
					"children":
					(
						{
							"name":"thinboard0",
							"type":"thinboard_circle",
							"x":0,"y":0,
							"width" : THINBOARD_WIDTH,
							"height" : THINBOARD_HEIGHT,
							"children":
							(
								{
									"name":"bg",
									"type":"image",
									"x":1,"y":1,
									"image":NEW_PATH+"def_bg.png",
								},
								{
									"name":"desc0",
									"type":"thinboard",
									"x":20,"y":10,
									"width":200,
									"height":55,
									"children":
									(
										{
											"name" : "desc0_emoji",
											"type" : "text",
											"x" : 0,
											"y" : -14,
											"all_align":"center",
											"text" : localeInfo.CHEST_DROP_INFO,
										},
										{
											"name" : "desc0_text",
											"type" : "text",
											"x" : 0,
											"y" : 0,
											"all_align":"center",
											"text" : uiScriptLocale.GAME_OPTIONS_FIND_CHESTS_DESC_1,
										},
										{
											"name" : "desc0_text",
											"type" : "text",
											"x" : 60,
											"y" : LINE_LABEL_Y+22,
											"text" : uiScriptLocale.GAME_OPTIONS_FIND_CHESTS_DESC_2,
										},
									),
								},
								{
									"name" : "desc0_on",
									"type" : "radio_button",
									"x" : LINE_DATA_X+MIDDLE_BUTTON_WIDTH*1.8,
									"y" : LINE_LABEL_Y+18,
									"default_image" : "d:/ymir work/ui/public/middle_button_01.sub",
									"over_image" : "d:/ymir work/ui/public/middle_button_02.sub",
									"down_image" : "d:/ymir work/ui/public/middle_button_03.sub",
									"children":
									(
										{
											"name" : "btn_text",
											"type" : "text",
											"x": 0, "y": 0,
											"all_align":"center",
											"text": uiScriptLocale.GAME_OPTIONS_ACTIVATE_BUTTON,
											"outline":1,
											"fontname":"Tahoma:12",
										},
									),
								},
								{
									"name" : "desc0_off",
									"type" : "radio_button",
									"x" : LINE_DATA_X+MIDDLE_BUTTON_WIDTH*MNOZENIE*2.8,
									"y" : LINE_LABEL_Y+18,
									"default_image" : "d:/ymir work/ui/public/middle_button_01.sub",
									"over_image" : "d:/ymir work/ui/public/middle_button_02.sub",
									"down_image" : "d:/ymir work/ui/public/middle_button_03.sub",
									"children":
									(
										{
											"name" : "btn_text",
											"type" : "text",
											"x": 0, "y": 0,
											"all_align":"center",
											"text" : uiScriptLocale.GAME_OPTIONS_DEACTIVATE_BUTTON,
											"outline":1,
											"fontname":"Tahoma:12",
										},
									),
								},

								{
									"name":"desc1",
									"type":"thinboard",
									"x":20,"y":80,
									"width":200,
									"height":30,
									"children":
									(
										{
											"name" : "desc1_emoji",
											"type" : "text",
											"x" : 0,
											"y" : -2,
											"all_align":"center",
											"text" : uiScriptLocale.GAME_OPTIONS_SPLIT_OPTION.format(emoji.AppendEmoji("icon/emoji/key_shift.png"),
																									emoji.AppendEmoji("icon/emoji/key_lclick.png") 
																									)
										},
									),
								},
								{
									"name" : "desc1_on",
									"type" : "radio_button",
									"x" : LINE_DATA_X+MIDDLE_BUTTON_WIDTH*1.8,
									"y" : LINE_LABEL_Y+76,
									"default_image" : "d:/ymir work/ui/public/middle_button_01.sub",
									"over_image" : "d:/ymir work/ui/public/middle_button_02.sub",
									"down_image" : "d:/ymir work/ui/public/middle_button_03.sub",
									"children":
									(
										{
											"name" : "btn_text",
											"type" : "text",
											"x": 0, "y": 0,
											"all_align":"center",
											"text": uiScriptLocale.GAME_OPTIONS_ACTIVATE_BUTTON,
											"outline":1,
											"fontname":"Tahoma:12",
										},
									),
								},
								{
									"name" : "desc1_off",
									"type" : "radio_button",
									"x" : LINE_DATA_X+MIDDLE_BUTTON_WIDTH*MNOZENIE*2.8,
									"y" : LINE_LABEL_Y+76,
									"default_image" : "d:/ymir work/ui/public/middle_button_01.sub",
									"over_image" : "d:/ymir work/ui/public/middle_button_02.sub",
									"down_image" : "d:/ymir work/ui/public/middle_button_03.sub",
									"children":
									(
										{
											"name" : "btn_text",
											"type" : "text",
											"x": 0, "y": 0,
											"all_align":"center",
											"text": uiScriptLocale.GAME_OPTIONS_DEACTIVATE_BUTTON,
											"outline":1,
											"fontname":"Tahoma:12",
										},
									),
								},
								{
									"name":"desc2",
									"type":"thinboard",
									"x":20,"y":130,
									"width":200,
									"height":30,
									"children":
									(
										{
											"name" : "desc2_emoji",
											"type" : "text",
											"x" : 0,
											"y" : -2,
											"all_align":"center",
											"text" : uiScriptLocale.GAME_OPTIONS_PULL_OUT.format(emoji.AppendEmoji("icon/emoji/key_rclick.png"))
										},
									),
								},
								{
									"name" : "desc2_on",
									"type" : "radio_button",
									"x" : LINE_DATA_X+MIDDLE_BUTTON_WIDTH*1.8,
									"y" : LINE_LABEL_Y+125,
									"default_image" : "d:/ymir work/ui/public/middle_button_01.sub",
									"over_image" : "d:/ymir work/ui/public/middle_button_02.sub",
									"down_image" : "d:/ymir work/ui/public/middle_button_03.sub",
									"children":
									(
										{
											"name" : "btn_text",
											"type" : "text",
											"x": 0, "y": 0,
											"all_align":"center",
											"text": uiScriptLocale.GAME_OPTIONS_ACTIVATE_BUTTON,
											"outline":1,
											"fontname":"Tahoma:12",
										},
									),
								},
								{
									"name" : "desc2_off",
									"type" : "radio_button",
									"x" : LINE_DATA_X+MIDDLE_BUTTON_WIDTH*MNOZENIE*2.8,
									"y" : LINE_LABEL_Y+125,
									"default_image" : "d:/ymir work/ui/public/middle_button_01.sub",
									"over_image" : "d:/ymir work/ui/public/middle_button_02.sub",
									"down_image" : "d:/ymir work/ui/public/middle_button_03.sub",
									"children":
									(
										{
											"name" : "btn_text",
											"type" : "text",
											"x": 0, "y": 0,
											"all_align":"center",
											"text": uiScriptLocale.GAME_OPTIONS_DEACTIVATE_BUTTON,
											"outline":1,
											"fontname":"Tahoma:12",
										},
									),
								},
								{
									"name":"desc3",
									"type":"thinboard",
									"x":20,"y":180,
									"width":200,
									"height":40,
									"children":
									(
										{
											"name" : "desc3_emoji",
											"type" : "text",
											"x" : 0,
											"y" : -8,
											"all_align":"center",
											"text" : emoji.AppendEmoji("icon/emoji/key_shift.png")+" + "+emoji.AppendEmoji("icon/emoji/key_rclick.png"),
										},
										{
											"name" : "desc3_text",
											"type" : "text",
											"x" : 0,
											"y" : 5,
											"all_align":"center",
											"text" : uiScriptLocale.GAME_OPTIONS_ADD_TO_SPECIAL_INVENTORY,
										},
									),
								},
								{
									"name" : "desc3_on",
									"type" : "radio_button",
									"x" : LINE_DATA_X+MIDDLE_BUTTON_WIDTH*1.8,
									"y" : LINE_LABEL_Y+180,
									"default_image" : "d:/ymir work/ui/public/middle_button_01.sub",
									"over_image" : "d:/ymir work/ui/public/middle_button_02.sub",
									"down_image" : "d:/ymir work/ui/public/middle_button_03.sub",
									"children":
									(
										{
											"name" : "btn_text",
											"type" : "text",
											"x": 0, "y": 0,
											"all_align":"center",
											"text": uiScriptLocale.GAME_OPTIONS_ACTIVATE_BUTTON,
											"outline":1,
											"fontname":"Tahoma:12",
										},
									),
								},
								{
									"name" : "desc3_off",
									"type" : "radio_button",
									"x" : LINE_DATA_X+MIDDLE_BUTTON_WIDTH*MNOZENIE*2.8,
									"y" : LINE_LABEL_Y+180,
									"default_image" : "d:/ymir work/ui/public/middle_button_01.sub",
									"over_image" : "d:/ymir work/ui/public/middle_button_02.sub",
									"down_image" : "d:/ymir work/ui/public/middle_button_03.sub",
									"children":
									(
										{
											"name" : "btn_text",
											"type" : "text",
											"x": 0, "y": 0,
											"all_align":"center",
											"text": uiScriptLocale.GAME_OPTIONS_DEACTIVATE_BUTTON,
											"outline":1,
											"fontname":"Tahoma:12",
										},
									),
								},


								{
									"name" : "rectangle",
									"type" : "image",
									"x" : LINE_LABEL_X-20,
									"y" : LINE_LABEL_Y+62,
									"image" : NEW_PATH+"rectangle.png",
								},
								{
									"name" : "rectangle",
									"type" : "image",
									"x" : LINE_LABEL_X-20,
									"y" : LINE_LABEL_Y+110,
									"image" : NEW_PATH+"rectangle.png",
								},
								{
									"name" : "rectangle",
									"type" : "image",
									"x" : LINE_LABEL_X-20,
									"y" : LINE_LABEL_Y+160,
									"image" : NEW_PATH+"rectangle.png",
								},
								{
									"name" : "rectangle",
									"type" : "image",
									"x" : LINE_LABEL_X-20,
									"y" : LINE_LABEL_Y+218,
									"image" : NEW_PATH+"rectangle.png",
								},
							),
						},
					),
				},
				{
					"name":"ukrywanie_window",
					"type":"window",
					"x":169,"y":30,
					"width":400,
					"height":300,
					"children":
					(
						{
							"name":"thinboard0",
							"type":"thinboard_circle",
							"x":0,"y":0,
							"width" : THINBOARD_WIDTH,
							"height" : THINBOARD_HEIGHT,
							"children":
							(
								{
									"name":"bg",
									"type":"image",
									"x":1,"y":1,
									"image":NEW_PATH+"def_bg.png",
								},
								{
									"name" : "offline_shop_range_label",
									"type" : "text",
									"x" : LINE_LABEL_X,
									"y" : LINE_LABEL_Y-6,
									"text" : "Offline Shops",
								},
								{
									"name" : "offline_shop_range_all",
									"type" : "radio_button",
									"x" : LINE_DATA_X+MIDDLE_BUTTON_WIDTH*0,
									"y" : LINE_LABEL_Y-6,
									"default_image" : ROOT_PATH + "Middle_Button_01.sub",
									"over_image" : ROOT_PATH + "Middle_Button_02.sub",
									"down_image" : ROOT_PATH + "Middle_Button_03.sub",
									"children" : (
										{"name":"offline_shop_range_all_text","type":"text","x":0,"y":0,"all_align":"center","text":"Show All","outline":1,"fontname":"Tahoma:12"},
									),
								},
								#{
								#	"name" : "offline_shop_range_nearby",
								#	"type" : "radio_button",
								#	"x" : LINE_DATA_X+MIDDLE_BUTTON_WIDTH*1,
								#	"y" : LINE_LABEL_Y-6,
								#	"default_image" : ROOT_PATH + "Middle_Button_01.sub",
								#	"over_image" : ROOT_PATH + "Middle_Button_02.sub",
								#	"down_image" : ROOT_PATH + "Middle_Button_03.sub",
								#	"children" : (
								#		{"name":"offline_shop_range_nearby_text","type":"text","x":0,"y":0,"all_align":"center","text":"Nearby","outline":1,"fontname":"Tahoma:12"},
								#	),
								#},
								{
									"name" : "offline_shop_range_hide",
									"type" : "radio_button",
									"x" : LINE_DATA_X+MIDDLE_BUTTON_WIDTH*1,
									"y" : LINE_LABEL_Y-6,
									"default_image" : ROOT_PATH + "Middle_Button_01.sub",
									"over_image" : ROOT_PATH + "Middle_Button_02.sub",
									"down_image" : ROOT_PATH + "Middle_Button_03.sub",
									"children" : (
										{"name":"offline_shop_range_hide_text","type":"text","x":0,"y":0,"all_align":"center","text":"Hide","outline":1,"fontname":"Tahoma:12"},
									),
								},
								
							
								{
									"name" : "rectangle",
									"type" : "image",
									"x" : LINE_LABEL_X-20,
									"y" : LINE_LABEL_Y+25,
									"image" : NEW_PATH+"rectangle.png",
								},
							
								{
									"name" : "wierzchowce_mode",
									"type" : "text",
									"x" : LINE_LABEL_X,
									"y" : LINE_LABEL_Y+35,
									"text" : uiScriptLocale.GAME_OPTIONS_MOUNTS_VISIBILITY,
								},
								{
									"name" : "wierzchowce_on",
									"type" : "radio_button",
									"x" : LINE_LABEL_X+MIDDLE_BUTTON_WIDTH*1.9,
									"y" : LINE_LABEL_Y+34,
									"default_image" : ROOT_PATH + "Middle_Button_01.sub",
									"over_image" : ROOT_PATH + "Middle_Button_02.sub",
									"down_image" : ROOT_PATH + "Middle_Button_03.sub",
									"children":
									(
										{
											"name" : "wierzchowce_on_text",
											"type" : "text",
											"x": 0, "y": 0,
											"all_align":"center",
											"text" : uiScriptLocale.GAME_OPTIONS_MOUNTS_SHOW,
											"outline":1,
											"fontname":"Tahoma:12",
										},
									),
								},
							{
									"name" : "wierzchowce_off",
									"type" : "radio_button",
									"x" : LINE_LABEL_X+MIDDLE_BUTTON_WIDTH*3,
									"y" : LINE_LABEL_Y+34,
									"default_image" : ROOT_PATH + "Middle_Button_01.sub",
									"over_image" : ROOT_PATH + "Middle_Button_02.sub",
									"down_image" : ROOT_PATH + "Middle_Button_03.sub",
									"children":
									(
										{
											"name" : "wierzchowce_off_text",
											"type" : "text",
											"x": 0, "y": 0,
											"all_align":"center",
											"text" : uiScriptLocale.GAME_OPTIONS_MOUNTS_HIDE,
											"outline":1,
											"fontname":"Tahoma:12",
										},
									),
								},
								{
									"name" : "rectangle",
									"type" : "image",
									"x" : LINE_LABEL_X-20,
									"y" : LINE_LABEL_Y+65,
									"image" : NEW_PATH+"rectangle.png",
								},
								{
									"name" : "pety_mode",
									"type" : "text",
									"x" : LINE_LABEL_X,
									"y" : LINE_LABEL_Y+75,
									"text" : uiScriptLocale.GAME_OPTIONS_PETS_VISIBILITY,
								},
								{
									"name" : "pety_on",
									"type" : "radio_button",
									"x" : LINE_LABEL_X+MIDDLE_BUTTON_WIDTH*1.9,
									"y" : LINE_LABEL_Y+73,
									"default_image" : ROOT_PATH + "Middle_Button_01.sub",
									"over_image" : ROOT_PATH + "Middle_Button_02.sub",
									"down_image" : ROOT_PATH + "Middle_Button_03.sub",
									"children":
									(
										{
											"name" : "pety_text",
											"type" : "text",
											"x": 0, "y": 0,
											"all_align":"center",
											"text" : uiScriptLocale.GAME_OPTIONS_PETS_SHOW,
											"outline":1,
											"fontname":"Tahoma:12",
										},
									),
								},
								{
									"name" : "pety_off",
									"type" : "radio_button",
									"x" : LINE_LABEL_X+MIDDLE_BUTTON_WIDTH*3,
									"y" : LINE_LABEL_Y+73,
									"default_image" : ROOT_PATH + "Middle_Button_01.sub",
									"over_image" : ROOT_PATH + "Middle_Button_02.sub",
									"down_image" : ROOT_PATH + "Middle_Button_03.sub",
									"children":
									(
										{
											"name" : "pety_text",
											"type" : "text",
											"x": 0, "y": 0,
											"all_align":"center",
											"text" : uiScriptLocale.GAME_OPTIONS_PETS_HIDE,
											"outline":1,
											"fontname":"Tahoma:12",
										},
									),
								},
								{
									"name" : "rectangle",
									"type" : "image",
									"x" : LINE_LABEL_X-20,
									"y" : LINE_LABEL_Y+103,
									"image" : NEW_PATH+"rectangle.png",
								},

								{
									"name" : "autobuff_mude",
									"type" : "text",
									"x" : LINE_LABEL_X,
									"y" : LINE_LABEL_Y+115,
									"text" : "Autobuff",
								},
								{
									"name" : "autobuff_on",
									"type" : "radio_button",
									"x" : LINE_LABEL_X+MIDDLE_BUTTON_WIDTH*1.9,
									"y" : LINE_LABEL_Y+113,
									"default_image" : ROOT_PATH + "Middle_Button_01.sub",
									"over_image" : ROOT_PATH + "Middle_Button_02.sub",
									"down_image" : ROOT_PATH + "Middle_Button_03.sub",
									"children":
									(
										{
											"name" : "autobuff_text",
											"type" : "text",
											"x": 0, "y": 0,
											"all_align":"center",
											"text" : uiScriptLocale.GAME_OPTIONS_BUFFI_SHOW,
											"outline":1,
											"fontname":"Tahoma:12",
										},
									),
								},
								{
									"name" : "autobuff_off",
									"type" : "radio_button",
									"x" : LINE_LABEL_X+MIDDLE_BUTTON_WIDTH*3,
									"y" : LINE_LABEL_Y+113,
									"default_image" : ROOT_PATH + "Middle_Button_01.sub",
									"over_image" : ROOT_PATH + "Middle_Button_02.sub",
									"down_image" : ROOT_PATH + "Middle_Button_03.sub",
									"children":
									(
										{
											"name" : "autobuff_text",
											"type" : "text",
											"x": 0, "y": 0,
											"all_align":"center",
											"text" : uiScriptLocale.GAME_OPTIONS_BUFFI_HIDE,
											"outline":1,
											"fontname":"Tahoma:12",
										},
									),
								},
								{
									"name" : "rectangle",
									"type" : "image",
									"x" : LINE_LABEL_X-20,
									"y" : LINE_LABEL_Y+142,
									"image" : NEW_PATH+"rectangle.png",
								},

								{
									"name" : "sharpness_mude",
									"type" : "text",
									"x" : LINE_LABEL_X,
									"y" : LINE_LABEL_Y+115+40,
									"text" : "Sharpness",
									"text" : uiScriptLocale.GAME_OPTIONS_SHARPNESS,
								},
								{
									"name" : "sharpness_controller",
									"type" : "sliderbarNew",
									"x" : LINE_LABEL_X+MIDDLE_BUTTON_WIDTH*1.2,
									"y" : LINE_LABEL_Y+115+40,
								},
								{
									"name" : "rectangle",
									"type" : "image",
									"x" : LINE_LABEL_X-20,
									"y" : LINE_LABEL_Y+142+39,
									"image" : NEW_PATH+"rectangle.png",
								},
								{
									"name" : "stone_scale_mude",
									"type" : "text",
									"x" : LINE_LABEL_X,
									"y" : LINE_LABEL_Y+115+40+40,
									"text" : uiScriptLocale.GAME_OPTIONS_STONE_SCALE,
								},
								{
									"name" : "stone_scale_controller",
									"type" : "sliderbarNew",
									"x" : LINE_LABEL_X+MIDDLE_BUTTON_WIDTH*1.2,
									"y" : LINE_LABEL_Y+115+40+40,
								},
								{
									"name" : "rectangle",
									"type" : "image",
									"x" : LINE_LABEL_X-20,
									"y" : LINE_LABEL_Y+142+39+39,
									"image" : NEW_PATH+"rectangle.png",
								},

								{
									"name" : "queue_effect_mude",
									"type" : "text",
									"x" : LINE_LABEL_X,
									"y" : LINE_LABEL_Y+115+40+40+40,
									"text" : uiScriptLocale.GAME_OPTIONS_QUEUE_EFFECT,
								},
								{
									"name" : "queue_effect_0",
									"type" : "radio_button",
									"x" : LINE_DATA_X+MIDDLE_BUTTON_WIDTH*0,
									"y" : LINE_LABEL_Y+115+40+40+38,
									"default_image" : ROOT_PATH + "Middle_Button_01.sub",
									"over_image" : ROOT_PATH + "Middle_Button_02.sub",
									"down_image" : ROOT_PATH + "Middle_Button_03.sub",
									"children" : (
										{
											"name" : "queue_effect_text",
											"type" : "text",
											"x": 0, "y": 0,
											"all_align":"center",
											"text" : "1",
											"outline":1,
											"fontname":"Tahoma:12",
										},
									),
								},
								{
									"name" : "queue_effect_1",
									"type" : "radio_button",
									"x" : LINE_DATA_X+MIDDLE_BUTTON_WIDTH*1,
									"y" : LINE_LABEL_Y+115+40+40+38,
									"default_image" : ROOT_PATH + "Middle_Button_01.sub",
									"over_image" : ROOT_PATH + "Middle_Button_02.sub",
									"down_image" : ROOT_PATH + "Middle_Button_03.sub",
									"children" : (
										{
											"name" : "queue_effect_text",
											"type" : "text",
											"x": 0, "y": 0,
											"all_align":"center",
											"text" : "2",
											"outline":1,
											"fontname":"Tahoma:12",
										},
									),
								},
								{
									"name" : "queue_effect_2",
									"type" : "radio_button",
									"x" : LINE_DATA_X+MIDDLE_BUTTON_WIDTH*2,
									"y" : LINE_LABEL_Y+115+40+40+38,
									"default_image" : ROOT_PATH + "Middle_Button_01.sub",
									"over_image" : ROOT_PATH + "Middle_Button_02.sub",
									"down_image" : ROOT_PATH + "Middle_Button_03.sub",
									"children" : (
										{
											"name" : "queue_effect_text",
											"type" : "text",
											"x": 0, "y": 0,
											"all_align":"center",
											"text" : "3",
											"outline":1,
											"fontname":"Tahoma:12",
										},
									),
								},
								{
									"name" : "queue_effect_3",
									"type" : "radio_button",
									"x" : LINE_DATA_X+MIDDLE_BUTTON_WIDTH*3,
									"y" : LINE_LABEL_Y+115+40+40+38,
									"default_image" : ROOT_PATH + "Middle_Button_01.sub",
									"over_image" : ROOT_PATH + "Middle_Button_02.sub",
									"down_image" : ROOT_PATH + "Middle_Button_03.sub",
									"children" : (
										{
											"name" : "queue_effect_text",
											"type" : "text",
											"x": 0, "y": 0,
											"all_align":"center",
											"text" : "4",
											"outline":1,
											"fontname":"Tahoma:12",
										},
									),
								},
								{
									"name" : "rectangle",
									"type" : "image",
									"x" : LINE_LABEL_X-20,
									"y" : LINE_LABEL_Y+142+39+39+39,
									"image" : NEW_PATH+"rectangle.png",
								},
								{
									"name" : "boss_scale_mude",
									"type" : "text",
									"x" : LINE_LABEL_X,
									"y" : LINE_LABEL_Y+115+40+40+40+40,
									"text" : uiScriptLocale.GAME_OPTIONS_BOSS_SCALE,
								},
								{
									"name" : "boss_scale_controller",
									"type" : "sliderbarNew",
									"x" : LINE_LABEL_X+MIDDLE_BUTTON_WIDTH*1.2,
									"y" : LINE_LABEL_Y+115+40+40+40+40,
								},
								{
									"name" : "rectangle",
									"type" : "image",
									"x" : LINE_LABEL_X-20,
									"y" : LINE_LABEL_Y+142+39+39+39+39,
									"image" : NEW_PATH+"rectangle.png",
								},
							),
						},
					),
				},
			],
		},
		{
			"name" : "Window_Board",
			"type" : "window",
			"x": 10, "y": 25,
			"width":160,"height":THINBOARD_HEIGHT,
			"children" :
			(
				{
					"name":"thinboard0",
					"type":"thinboard_circle",
					"x":0,"y":5,
					"width" : 160,
					"height" : THINBOARD_HEIGHT,
					"children":
					(
						{
							"name":"bg",
							"type":"image",
							"x":1,"y":1,
							"image":NEW_PATH+"cat_bg.png",
							"children":
							(
								{
									"name":"category_btn1",
									"type":"radio_button",
									"x":-1,"y":15,
									"horizontal_align":"center",
									"default_image":"d:/ymir work/ui/weekly_rank/button-example-020.png",
									"over_image":"d:/ymir work/ui/weekly_rank/button-example-021.png",
									"down_image":"d:/ymir work/ui/weekly_rank/button-example-022.png",
									"children":
									(
										{
											"name":"category_txt",
											"type":"text",
											"x":0,"y":0,
											"all_align":"center",
											"text": uiScriptLocale.GAME_OPTIONS_MAIN_CATEGORY,
											"outline":1,
											"fontname":"Tahoma:14",
										},
									),
								},
								{
									"name":"category_btn2",
									"type":"radio_button",
									"x":-1,"y":40,
									"horizontal_align":"center",
									"default_image":"d:/ymir work/ui/weekly_rank/button-example-020.png",
									"over_image":"d:/ymir work/ui/weekly_rank/button-example-021.png",
									"down_image":"d:/ymir work/ui/weekly_rank/button-example-022.png",
									"children":
									(
										{
											"name":"category_txt",
											"type":"text",
											"x":0,"y":0,
											"all_align":"center",
											"text": uiScriptLocale.GAME_OPTIONS_SYSTEM_CATEGORY,
											"outline":1,
											"fontname":"Tahoma:14",
										},
									),
								},
								{
									"name":"category_btn3",
									"type":"radio_button",
									"x":-1,"y":65,
									"horizontal_align":"center",
									"default_image":"d:/ymir work/ui/weekly_rank/button-example-020.png",
									"over_image":"d:/ymir work/ui/weekly_rank/button-example-021.png",
									"down_image":"d:/ymir work/ui/weekly_rank/button-example-022.png",
									"children":
									(
										{
											"name":"category_txt",
											"type":"text",
											"x":0,"y":0,
											"all_align":"center",
											"text": uiScriptLocale.GAME_OPTIONS_DESCRIPTIONS_CATEGORY,
											"outline":1,
											"fontname":"Tahoma:14",
										},
									),
								},
								{
									"name":"category_btn4",
									"type":"radio_button",
									"x":-1,"y":90,
									"horizontal_align":"center",
									"default_image":"d:/ymir work/ui/weekly_rank/button-example-020.png",
									"over_image":"d:/ymir work/ui/weekly_rank/button-example-021.png",
									"down_image":"d:/ymir work/ui/weekly_rank/button-example-022.png",
									"children":
									(
										{
											"name":"category_txt",
											"type":"text",
											"x":0,"y":0,
											"all_align":"center",
											"text": uiScriptLocale.GAME_OPTIONS_HIDING_CATEGORY,
											"outline":1,
											"fontname":"Tahoma:14",
										},
									),
								},
							),
						},
					),
				},
			),
		},
	],
}