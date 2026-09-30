import uiScriptLocale
import localeInfo

BOARD_WIDTH  = 259
BOARD_HEIGHT = 237
ROOT		 = "elvion/game/vote4buff/"

window = {
	"name" : "Vote4BuffWindow",
	"style" : ("movable", "float",),

	"x" : (SCREEN_WIDTH - BOARD_WIDTH) / 2,
	"y" : (SCREEN_HEIGHT - BOARD_HEIGHT) / 2,

	"width" : BOARD_WIDTH,
	"height" : BOARD_HEIGHT,

	"children" :
	(
		{
			"name" : "board",
			"type" : "board_with_titlebar",
			"style" : ("attach",),

			"x" : 0,
			"y" : 0,

			"width" : BOARD_WIDTH,
			"height" : BOARD_HEIGHT,

			"title" : uiScriptLocale.VOTE4BUFF_TITLE,

			"children" :
			(
				{
					"name" : "wrapper",
					"type" : "image",

					"x" : 10,
					"y" : 33,

					"image" : ROOT + "base.tga",

					"children" :
					(
						{
							"name" : "vote_button",
							"type" : "button",

							"x" : 2,
							"y" : 2,

							"text" : localeInfo.VOTE4BUFF_VOTE_INFORMATION,
							"text_color" : 0xFFFFCC66,

							"default_image" : ROOT + "btn_vote_1.tga",
							"over_image" : ROOT + "btn_vote_2.tga",
							"down_image" : ROOT + "btn_vote_3.tga",
							"disable_image" : ROOT + "btn_vote_3.tga",
						},
						{
							"name" : "info_label",
							"type" : "text",
							"all_align" : 1,

							"x" : 0,
							"y" : -46,

							"text" : localeInfo.VOTE4BUFF_WARNING_MSG1,
							"outline" : 1,
						},
						{
							"name" : "bonus_button0",
							"type" : "button",

							"x" : 2,
							"y" : 64,

							"default_image" : ROOT + "btn_1_0.tga",
							"over_image" : ROOT + "btn_1_1.tga",
							"down_image" : ROOT + "btn_1_1.tga",
							"disable_image" : ROOT + "btn_1_1.tga",
						},
						{
							"name" : "bonus_button1",
							"type" : "button",

							"x" : 81,
							"y" : 64,

							"default_image" : ROOT + "btn_2_0.tga",
							"over_image" : ROOT + "btn_2_1.tga",
							"down_image" : ROOT + "btn_2_1.tga",
							"disable_image" : ROOT + "btn_2_1.tga",
						},
						{
							"name" : "bonus_button2",
							"type" : "button",

							"x" : 160,
							"y" : 64,

							"default_image" : ROOT + "btn_3_0.tga",
							"over_image" : ROOT + "btn_3_1.tga",
							"down_image" : ROOT + "btn_3_1.tga",
							"disable_image" : ROOT + "btn_3_1.tga",
						},
						{
							"name" : "bottom_info_wrapper",
							"type" : "image",

							"x" : 1,
							"y" : 136,

							"image" : ROOT + "vote_available.tga",

							"children" :
							(
								{
									"name" : "bottom_info",
									"type" : "text",
									"all_align" : 1,

									"x" : 0,
									"y" : -1,

									"text" : localeInfo.VOTE4BUFF_AVAILABLE,
									"outline" : 1,
								},
							)
						},
					)
				},
				#{
				#	"name" : "info_button",
				#	"type" : "button",
				#
				#	"x" : BOARD_WIDTH - 15 - 16 - 16,
				#	"y" : 9,
				#
				#	"default_image" : ROOT + "q_mark_01.tga",
				#	"over_image" : ROOT + "q_mark_02.tga",
				#	"down_image" : ROOT + "q_mark_02.tga",
				#},
				{
					"name" : "shop_button",
					"type" : "button",

					"x" : BOARD_WIDTH / 2 + 25,
					"y" : 194,

					"default_image" : "elvion/game/warpwindow/dungeon_info/buy_ticket_norm.png",
					"over_image" : "elvion/game/warpwindow/dungeon_info/buy_ticket_hover.png",
					"down_image" : "elvion/game/warpwindow/dungeon_info/buy_ticket_down.png",
				},
			)
		},
	)
}
