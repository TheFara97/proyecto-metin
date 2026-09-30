import uiScriptLocale
window = {
	"name" : "PasswordDialog",

	"x" : 0,
	"y" : 0,

	"style" : ("movable", "float",),

	"width" : 250,
	"height" : 150,

	"children" :
	(
		{
			"name" : "board",
			"type" : "board",

			"x" : 0,
			"y" : 0,

			"width" : 250,
			"height" : 150,

			"children" :
			(

				{
					"name" : "Desc1",
					"type" : "text",

					"x" : 250/2,
					"y" : 30,

					"text" : uiScriptLocale.PASSWORD_DESC_1,
					"text_horizontal_align":"center"
				},
				{
					"name" : "Desc2",
					"type" : "text",

					"x" : 250/2,
					"y" : 42,

					"text" : uiScriptLocale.PASSWORD_DESC_2,
					"text_horizontal_align":"center"
				},
				{
					"name" : "titlebar",
					"type" : "titlebar",
					"style" : ("attach",),

					"x" : 8,
					"y" : 8,
 
					"width" : 234,
					"color" : "gray",

					"children" :
					(
						{
							"name" : "TitleName",
							"type" : "text",

							"x" : 234/2,
							"y" : 3,

							"text" : uiScriptLocale.PASSWORD_TITLE,
							"text_horizontal_align":"center"
						},
					),
				},

				{
					"name" : "password_slot",
					"type" : "image",

					"x" : 0,
					"y" : 56,
					"horizontal_align" : "center",

					"image" : "d:/ymir work/ui/public/Parameter_Slot_02.sub",

					"children" :
					(
						{
							"name" : "password_value",
							"type" : "editline",

							"x" : 3,
							"y" : 3,

							"width" : 60,
							"height" : 18,

							"input_limit" : 6,
							"secret_flag" : 1,
						},
					),
				},
				{
					"name" : "Desc3",
					"type" : "text",

					"x" : 250/2,
					"y" : 80,

					"text" : uiScriptLocale.PASSWORD_DESC_3,
					"text_horizontal_align":"center"
				},
				{
					"name" : "Desc4",
					"type" : "text",

					"x" : 250/2,
					"y" : 92,

					"text" : uiScriptLocale.PASSWORD_DESC_4,
					"text_horizontal_align":"center"
				},
				{
					"name" : "Desc5",
					"type" : "text",

					"x" : 250/2,
					"y" : 104,

					"text" : uiScriptLocale.PASSWORD_DESC_5,
					"text_horizontal_align":"center"
				},
				{
					"name" : "accept_button",
					"type" : "button",
					"x" : 250/2 - 61 - 5,
					"y" : 120,
					"default_image" : "d:/ymir work/ui/button_example_040.png",
					"over_image" : "d:/ymir work/ui/button_example_041.png",
					"down_image" : "d:/ymir work/ui/button_example_042.png",
				},
				{
					"name" : "cancel_button",
					"type" : "button",
					"x" : 250/2 + 5,
					"y" : 120,
					"default_image" : "d:/ymir work/ui/button_example_050.png",
					"over_image" : "d:/ymir work/ui/button_example_051.png",
					"down_image" : "d:/ymir work/ui/button_example_052.png",
				},
			),
		},
	),
}
