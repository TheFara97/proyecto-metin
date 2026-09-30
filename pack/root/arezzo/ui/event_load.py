import uiScriptLocale

ROOT = "d:/ymir work/ui/public/"

window = {
	"name" : "SystemDialog",
	"style" : ("float",),

	"x" : 0,
	"y" : 0,

	"width" : 230,
	"height" : 205,

	"children" :
	(
		{
			"name" : "board",
			"type" : "thinboard",

			"x" : 0,
			"y" : 0,

			"width" : 210,
			"height" : 160,

			"children" :
			(
				{
					"name" : "EventButton1",
					"type" : "button",

					"x" : 15,
					"y" : 10,

					"text" : "Karty Okey",

					"default_image" : ROOT + "XLarge_Button_01.sub",
					"over_image" : ROOT + "XLarge_Button_02.sub",
					"down_image" : ROOT + "XLarge_Button_03.sub",
				},

				{
					"name" : "EventButton2",
					"type" : "button",

					"x" : 15,
					"y" : 35,

					"text" : "Schwytaj Króla",

					"default_image" : ROOT + "XLarge_Button_01.sub",
					"over_image" : ROOT + "XLarge_Button_02.sub",
					"down_image" : ROOT + "XLarge_Button_03.sub",
				},
				{
					"name" : "EventButton3",
					"type" : "button",

					"x" : 15,
					"y" : 60,

					"text" : "TEST",

					"default_image" : ROOT + "XLarge_Button_01.sub",
					"over_image" : ROOT + "XLarge_Button_02.sub",
					"down_image" : ROOT + "XLarge_Button_03.sub",
				},
				{
					"name" : "EventButton4",
					"type" : "button",

					"x" : 15,
					"y" : 85,

					"text" : "TEST2",

					"default_image" : ROOT + "XLarge_Button_01.sub",
					"over_image" : ROOT + "XLarge_Button_02.sub",
					"down_image" : ROOT + "XLarge_Button_03.sub",
				},
				{
					"name" : "cancel_button",
					"type" : "button",

					"x" : 15,
					"y" : 125,

					"text" : uiScriptLocale.CANCEL,

					"default_image" : ROOT + "XLarge_Button_01.sub",
					"over_image" : ROOT + "XLarge_Button_02.sub",
					"down_image" : ROOT + "XLarge_Button_03.sub",
				},
			),
		},
	),
}
