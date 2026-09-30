
ELVION = "elvion/game/taskbar/expanded/"

BOARD_X = SCREEN_WIDTH - (350)
BOARD_WIDTH = (350)
BOARD_HEIGHT = 40

window = {
	"name" : "ExpandedMoneyTaskbar",
	
	"x" : BOARD_X,
	"y" : SCREEN_HEIGHT - 115 + 32,

	"width" : BOARD_WIDTH,
	"height" : BOARD_HEIGHT,

	"style" : ("float",),

	"children" :
	[
		{
			"name" : "ExpanedMoneyTaskBar_Board",
			"type" : "image",

			"x" : 6,
			"y" : 0,
			
			"image" : ELVION + "bg.png",

			"children" :
			[
				{
					"name":"Money_Icon",
					"type":"button",
					
					"x":194-44+8,
					"y":8,

					"default_image": ELVION + "money_norm.png",
                    "over_image": ELVION + "money_hover.png",
                    "down_image": ELVION + "money_down.png",
				},
				{
					"name":"Money_Slot",
					"type":"button",

					"x":223-44,
					"y":7,


					"default_image" : ELVION + "yang_slot.png",
					"over_image" : ELVION + "yang_slot.png",
					"down_image" : ELVION + "yang_slot.png",

					"children" :
					(
						{
							"name" : "Money",
							"type" : "text",

							"x" : 3,
							"y" : 3,

							"horizontal_align" : "right",
							"text_horizontal_align" : "right",

							"text" : "9,999,999,999",
						},
					),
				},
			],
		},
		{
			"name" : "expander",
			"type" : "button",
			"x":15,
			"y":-6,
			"default_image" : "d:/ymir work/ui/btn_expand_normal1.png",
			"over_image" : "d:/ymir work/ui/btn_expand_over1.png",
			"down_image" : "d:/ymir work/ui/btn_expand_down1.png",
		},
	],
}

window["children"][0]["children"] = window["children"][0]["children"] + [
				{
					"name":"Cheque_Icon",
					"type":"button",
					
					"x":85,
					"y":10,

					"default_image":"icon/emoji/cheque_icon.png",
                    "over_image":"icon/emoji/cheque_icon1.png",
                    "down_image":"icon/emoji/cheque_icon2.png",
				},					
				{
					"name":"Cheque_Slot",
					"type":"button",

					"x":104,
					"y":7,

					"default_image" : ELVION + "other_slot.png",
					"over_image" : ELVION + "other_slot.png",
					"down_image" : ELVION + "other_slot.png",

					"children" :
					(
						{
							"name" : "Cheque",
							"type" : "text",

							"x" : 3,
							"y" : 3,

							"horizontal_align" : "right",
							"text_horizontal_align" : "right",

							"text" : "999,999",
						},
					),
				},
				]
window["children"][0]["children"] = window["children"][0]["children"] + [
				{
					"name":"Pkt_Osiag_Icon",
					"type":"image",
					
					"x":9,
					"y":10,

					"image":"elvion/game/taskbar/expanded/achievement_point.png",
				},					
				{
					"name":"Pkt_Osiag_Slot",
					"type":"image",

					"x":29,
					"y":7,
					
					"image" : ELVION + "other_slot.png",

					"children" :
					(
						{
							"name" : "Pkt_Osiag",
							"type" : "text",

							"x" : 3,
							"y" : 3,

							"horizontal_align" : "right",
							"text_horizontal_align" : "right",

							"text" : "999,999",
						},
					),
				},
				]