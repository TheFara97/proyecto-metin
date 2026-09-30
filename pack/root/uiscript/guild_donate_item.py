import uiScriptLocale
GUILD_PATH = "elvion/game/guild{}"

window = {
	"name" : "InputDialog",

	"x" : 0,
	"y" : 0,

	"style" : ("movable", "float",),

	"width" : 212,
	"height" : 213,

	"children" :
	(
		{
			"name":"guild_item_bg",
			"type":"image",
			"x" : 0,
			"y" : 0,
			"image": GUILD_PATH.format("/home/guild_donate_dialog.png"),
		},
		{
			"name":"guild_item_bg",
			"type":"image",
			"x" : 52,
			"y" : 75,
			"image": GUILD_PATH.format("/home/donate_item.png"),
		},
		{
			"name" : "donate_desc_1",
			"type":"text",
			"text":"Leader of guild can increase guild",
			"x":0,
			"y":-62,
			"all_align":"center",
			"outline" : 1,
			"fontname":"Tahoma:12",
			"color":0xFFdcc2a9,
		},
		{
			"name" : "donate_desc_2",
			"type":"text",
			"text":"passive skills thanks to this item",
			"x":0,
			"y":-47,
			"all_align":"center",
			"outline" : 1,
			"fontname":"Tahoma:12",
			"color":0xFFdcc2a9,
		},
		{
			"name" : "donate_daily_limit_count",
			"type":"text",
			"text":"Daily count 0/10",
			"x":0,
			"y":67,
			"all_align":"center",
			"outline" : 1,
			"fontname":"Tahoma:12",
			"color":0xFFdcc2a9,
		},
		#{
		#	"name" : "InputSlot",
		#	"type" : "slotbar",
		#
		#	"x" : 0,
		#	"y" : 34,
		#	"width" : 90,
		#	"height" : 18,
		#	"horizontal_align" : "center",
		#
		#
		#	"children" :
		#	(
		#		{
		#			"name" : "InputValue",
		#			"type" : "editline",
		#
		#			"x" : 3,
		#			"y" : 3,
		#
		#			"width" : 90,
		#			"height" : 18,
		#
		#			"input_limit" : 12,
		#		},
		#	),
		#},

		{
			"name" : "AcceptButton",
			"type" : "button",

			"x" : 75,
			"y" : 146,

			"text" : uiScriptLocale.GUILD_DONATE_BUTTON,

			"default_image" : GUILD_PATH.format("/home/donate_btn_norm.png"),
			"over_image" : GUILD_PATH.format("/home/donate_btn_hover.png"),
			"down_image" : GUILD_PATH.format("/home/donate_btn_down.png"),
		},
		#{
		#	"name" : "CancelButton",
		#	"type" : "button",
		#
		#	"x" : 5 + 30,
		#	"y" : 58,
		#	"horizontal_align" : "center",
		#
		#	"text" : uiScriptLocale.CANCEL,
		#
		#	"default_image" : GUILD_PATH.format("/home/donate_btn_norm.png"),
		#	"over_image" : GUILD_PATH.format("/home/donate_btn_hover.png"),
		#	"down_image" : GUILD_PATH.format("/home/donate_btn_down.png"),
		#},
		{
			"name" : "TitleBar",
			"type" : "titlebar",
			"style" : ("attach",),
	
			"x" : 20,
			"y" : 12,
	
			"width" : 170,
			"color" : "red",
	
			"children" :
			(
				{
					"name" : "TitleName",
					"type" : "text",
					"text" : "Donate Item",
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