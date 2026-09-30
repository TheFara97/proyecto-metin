import uiScriptLocale

ROOT = "d:/ymir work/ui/public/"

WIDTH = 400
MESSENGER_WIDTH = 190
HEIGHT = 280

window = {
	"name" : "WhisperDialog",
	"style" : ("movable", "float",),

	"x" : 0,
	"y" : 0,

	"width" : MESSENGER_WIDTH + WIDTH,
	"height" : HEIGHT,

	"children" :
	(
		{
			"name" : "messenger_list",
			"type" : "board_with_titlebar",
			"style" : ("attach",),
			
			"x" : 0,
			"y" : 0,
			
			"title" : uiScriptLocale.MESSENGER_LIST,
			
			"width" : MESSENGER_WIDTH,
			"height" : HEIGHT,
			
			"children" : (
				{
					"name" : "new_whisper",
					"type" : "button",

					"x" : MESSENGER_WIDTH - 33,
					"y" : 8,

					"default_image" : "zaris/whisper/new_message.png",
					"over_image" : "zaris/whisper/new_message_over.png",
					"down_image" : "zaris/whisper/new_message_down.png",

					"tooltip_text" : uiScriptLocale.MESSENGER_NEW_MESSAGE,
				},
				{
					"name" : "messenger_players_window",
					"type" : "window",
					"style" : ("attach",),
					
					"x" : 10,
					"y" : 22+25,
					
					"width" : MESSENGER_WIDTH - 10 - 10,
					"height" : HEIGHT-22-25-5,
				},
				{
					"name" : "messenger_scrollbar",
					"type" : "scrollbar_zaris",

					"x" : MESSENGER_WIDTH - 25,
					"y" : 35,

					"size" : HEIGHT - 40,
				},
			),
		},
		{
			"name" : "board",
			"type" : "board",
			"style" : ("attach",),

			"x" : MESSENGER_WIDTH,
			"y" : 0,

			"width" : WIDTH,
			"height" : HEIGHT,

			"children" :
			(
				## Title
				{
					"name" : "name_slot",
					"type" : "image",
					"style" : ("attach",),

					"x" : 28,
					"y" : 10,

					"image":"d:/ymir work/ui/public/Parameter_Slot_05.sub",

					"children" :
					(
						{
							"name" : "titlename",
							"type" : "text",

							"x" : 3,
							"y" : 3,

							"text" : uiScriptLocale.WHISPER_NAME,
						},
						{
							"name" : "titlename_edit",
							"type" : "editline",

							"x" : 3,
							"y" : 3,

							"width" : 120,
							"height" : 17,

							"input_limit" : 16,

							"text" : uiScriptLocale.WHISPER_NAME,
						},
					),
				},
				
				{
					"name" : "closebutton",
					"type" : "button",

					"x" : WIDTH - 24,
					"y" : 12,

					"tooltip_text" : uiScriptLocale.CLOSE,

					"default_image" : "d:/ymir work/ui/close_btn0.png",
					"over_image" : "d:/ymir work/ui/close_btn1.png",
					"down_image" : "d:/ymir work/ui/close_btn2.png",
				},

				{
					"name" : "gamemastermark",
					"type" : "expanded_image",
					"style" : ("attach",),

					"x" : 285,
					"y" : 10,

					"x_scale" : 0.15,
					"y_scale" : 0.15,

					"image" : "locale/cz/effect/ymirred.tga",
				},

				## Button
				{
					"name" : "friendbutton",
					"type" : "button",

					"x" : 208,
					"y" : 10,

					"default_image" : "d:/ymir work/ui/game/windows/messenger_add_friend_01.sub",
					"over_image" : "d:/ymir work/ui/game/windows/messenger_add_friend_02.sub",
					"down_image" : "d:/ymir work/ui/game/windows/messenger_add_friend_03.sub",
				},

				{
					"name" : "acceptbutton",
					"type" : "button",

					"x" : 148,
					"y" : 10,

					"text" : uiScriptLocale.OK,

					"default_image" : "d:/ymir work/ui/public/small_thin_button_01.sub",
					"over_image" : "d:/ymir work/ui/public/small_thin_button_02.sub",
					"down_image" : "d:/ymir work/ui/public/small_thin_button_03.sub",
				},

				{
					"name" : "ignorebutton",
					"type" : "button",

					"x" : 150,
					"y" : 10,

					"tooltip_text" : uiScriptLocale.WHISPER_BAN,

					"default_image" : "zaris/whisper/block.png",
					"over_image" : "zaris/whisper/block_over.png",
					"down_image" : "zaris/whisper/block_down.png",
				},
                {
                    "name" : "searchplayershopbutton",
                    "type" : "button",

                    "x" : 194,
                    "y" : 10,

                    "tooltip_text" : uiScriptLocale.WHISPER_SEARCH_PLAYER_SHOP,

                    "default_image" : "zaris/whisper/shopsearch1.png",
                    "over_image" : "zaris/whisper/shopsearch2.png",
                    "down_image" : "zaris/whisper/shopsearch3.png",
                    "disable_image" : "zaris/whisper/shopsearch1.png",
                },
				{
					"name" : "historyDeleteButton",
					"type" : "button",

					"x" : WIDTH - 80,
					"y" : 12,

					"tooltip_text" : uiScriptLocale.WHISPER_DELETE,

					"default_image" : "zaris/whisper/delete.png",
					"over_image" : "zaris/whisper/delete_over.png",
					"down_image" : "zaris/whisper/delete_down.png",
				},
				{
					"name" : "historybutton",
					"type" : "button",

					"x" : WIDTH - 43,
					"y" : 12,

					"tooltip_text" : uiScriptLocale.WHISPER_HISTORY,

					"default_image" : "zaris/whisper/history.png",
					"over_image" : "zaris/whisper/history_over.png",
					"down_image" : "zaris/whisper/history_down.png",
				},
				{
					"name" : "minimizebutton",
					"type" : "button",

					"x" : WIDTH - 41,
					"y" : 12,

					"tooltip_text" : uiScriptLocale.MINIMIZE,

					"default_image" : "d:/ymir work/ui/public/minimize_button_01.sub",
					"over_image" : "d:/ymir work/ui/public/minimize_button_02.sub",
					"down_image" : "d:/ymir work/ui/public/minimize_button_03.sub",
				},

				## ScrollBar
				{
					"name" : "scrollbar",
					"type" : "scrollbar",

					"x" : WIDTH - 25,
					"y" : 35,

					"size" : 280 - 160,
				},

				## Edit Bar
				{
					"name" : "editbar",
					"type" : "bar",

					"x" : 10,
					"y" : 200 - 60,

					"width" : WIDTH - 18,
					"height" : 50,

					"color" : 0x77000000,

					"children" :
					(
						{
							"name" : "chatline",
							"type" : "editline",

							"x" : 5,
							"y" : 5,

							"width" : WIDTH - 70,
							"height" : 40,

							"with_codepage" : 1,
							"input_limit" : 40,
							"limit_width" : WIDTH - 90,
							"multi_line" : 1,
						},
						{
							"name" : "sendbutton",
							"type" : "button",

							"x" : WIDTH - 80,
							"y" : 10,

							"text" : uiScriptLocale.WHISPER_SEND,

							"default_image" : "d:/ymir work/ui/public/xlarge_thin_button_01.sub",
							"over_image" : "d:/ymir work/ui/public/xlarge_thin_button_02.sub",
							"down_image" : "d:/ymir work/ui/public/xlarge_thin_button_03.sub",
						},
					),
				},
			),
		},
	),
}
