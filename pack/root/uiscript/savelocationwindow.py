import uiScriptLocale

BOARD_WIDTH = 345
BOARD_HEIGHT = 361

window = {
	"name" : "SavePositionWindow",
	"style" : ("movable", "float",),

	"x" : (SCREEN_WIDTH - BOARD_WIDTH) / 10,
	"y" : (SCREEN_HEIGHT - BOARD_HEIGHT) / 2,

	"width" : BOARD_WIDTH,
	"height" : BOARD_HEIGHT,

	"children" :
	[
		{
			"name" : "board",
			"type" : "board_with_titlebar",

			"x" : 0,
			"y" : 0,

			"width" : BOARD_WIDTH,
			"height" : BOARD_HEIGHT,

			"title" : "Saved Locations",

			"children" :
			[

				{
					"name" : "bg",
					"type" : "image",
					"x" : 9, "y" : 35,
					"image" : "assets/ui/location/bg.png",
				},
				{
					"name" : "ElementContainer",
					"type" : "listboxex",

					"x" : 13, "y" : 40,
					"width" : BOARD_WIDTH - 20, "height" : BOARD_HEIGHT - 75,

				},
				{
					"name" : "Page1_Button",
					"type" : "button",
					"x" : 14, "y" : BOARD_HEIGHT - 47,
					"text" : "Page 1",
					"default_image" : "elvion/game/save_location/btn_norm.png",
					"over_image"    : "elvion/game/save_location/btn_hover.png",
					"down_image"    : "elvion/game/save_location/btn_down.png",
				},
				{
					"name" : "Page2_Button",
					"type" : "button",
					"x" : 14 + 160, "y" : BOARD_HEIGHT - 47,
					"text" : "Page 2 (Premium)",
					"default_image" : "elvion/game/save_location/btn_norm.png",
					"over_image"    : "elvion/game/save_location/btn_hover.png",
					"down_image"    : "elvion/game/save_location/btn_down.png",
				},
			],
		},
	],
}
