import uiScriptLocale

window = {
	"name" : "CrystalExchangeWindow",
	"style" : ("movable", "float",),
	"x" : 0,
	"y" : 0,
	"width" : 676,
	"height" : 600,
	"children" :
	(
		{
			"name" : "board",
			"type" : "board",
			"style" : ("attach",),
			"x" : 0,
			"y" : 0,
			"width" : 676,
			"height" : 600,
			"children" :
			(
				## Title Bar
				{
					"name" : "TitleBar",
					"type" : "titlebar",
					"style" : ("attach",),
					"x" : 6,
					"y" : 6,
					"width" : 664,
					"color" : "gray",
					"children" :
					(
						{
							"name" : "TitleName",
							"type" : "text",
							"x" : 332,
							"y" : 3,
							"text" : "Tworzenie kryształów",
							"text_horizontal_align" : "center"
						},
					),
				},
				
				## Exit Button
				{
					"name" : "ExitButton",
					"type" : "button",
					"x" : 676 - 21,
					"y" : 1,
					"tooltip_text" : uiScriptLocale.CLOSE,
					"default_image" : "d:/ymir work/ui/public/close_button_01.sub",
					"over_image" : "d:/ymir work/ui/public/close_button_02.sub",
					"down_image" : "d:/ymir work/ui/public/close_button_03.sub",
				},

				## Left Panel - Crystal Types
				{
					"name" : "LeftPanel",
					"type" : "board",
					"x" : 15,
					"y" : 35,
					"width" : 200,
					"height" : 550,
					"children" :
					(
						{
							"name" : "LeftTitle",
							"type" : "text",
							"x" : 100,
							"y" : 10,
							"text" : "Typy Kryształów",
							"text_horizontal_align" : "center",
							"color" : 0xFFFFE3AD,
						},

						## Crystal Type Buttons
						{
							"name" : "crystal_button_0",
							"type" : "button",
							"x" : 10,
							"y" : 40,
							"width" : 180,
							"height" : 30,
							"text" : "Ogólna",
							"default_image" : "d:/ymir work/ui/public/middle_button_01.sub",
							"over_image" : "d:/ymir work/ui/public/middle_button_02.sub",
							"down_image" : "d:/ymir work/ui/public/middle_button_03.sub",
						},
						{
							"name" : "crystal_count_0",
							"type" : "text",
							"x" : 10,
							"y" : 75,
							"text" : "Kryształy: 0 szt.",
							"color" : 0xFFFFFFFF,
						},

						{
							"name" : "crystal_button_1",
							"type" : "button",
							"x" : 10,
							"y" : 100,
							"width" : 180,
							"height" : 30,
							"text" : "Mapy Podstawowe",
							"default_image" : "d:/ymir work/ui/public/middle_button_01.sub",
							"over_image" : "d:/ymir work/ui/public/middle_button_02.sub",
							"down_image" : "d:/ymir work/ui/public/middle_button_03.sub",
						},
						{
							"name" : "crystal_count_1",
							"type" : "text",
							"x" : 10,
							"y" : 135,
							"text" : "Kryształy: 0 szt.",
							"color" : 0xFFFFFFFF,
						},

						{
							"name" : "crystal_button_2",
							"type" : "button",
							"x" : 10,
							"y" : 160,
							"width" : 180,
							"height" : 30,
							"text" : "Dungeon Podstawa",
							"default_image" : "d:/ymir work/ui/public/middle_button_01.sub",
							"over_image" : "d:/ymir work/ui/public/middle_button_02.sub",
							"down_image" : "d:/ymir work/ui/public/middle_button_03.sub",
						},
						{
							"name" : "crystal_count_2",
							"type" : "text",
							"x" : 10,
							"y" : 195,
							"text" : "Kryształy: 0 szt.",
							"color" : 0xFFFFFFFF,
						},

						{
							"name" : "crystal_button_3",
							"type" : "button",
							"x" : 10,
							"y" : 220,
							"width" : 180,
							"height" : 30,
							"text" : "Mapy Amasia",
							"default_image" : "d:/ymir work/ui/public/middle_button_01.sub",
							"over_image" : "d:/ymir work/ui/public/middle_button_02.sub",
							"down_image" : "d:/ymir work/ui/public/middle_button_03.sub",
						},
						{
							"name" : "crystal_count_3",
							"type" : "text",
							"x" : 10,
							"y" : 255,
							"text" : "Kryształy: 0 szt.",
							"color" : 0xFFFFFFFF,
						},

						{
							"name" : "crystal_button_4",
							"type" : "button",
							"x" : 10,
							"y" : 280,
							"width" : 180,
							"height" : 30,
							"text" : "Dungeon Amasia",
							"default_image" : "d:/ymir work/ui/public/middle_button_01.sub",
							"over_image" : "d:/ymir work/ui/public/middle_button_02.sub",
							"down_image" : "d:/ymir work/ui/public/middle_button_03.sub",
						},
						{
							"name" : "crystal_count_4",
							"type" : "text",
							"x" : 10,
							"y" : 315,
							"text" : "Kryształy: 0 szt.",
							"color" : 0xFFFFFFFF,
						},

						{
							"name" : "crystal_button_5",
							"type" : "button",
							"x" : 10,
							"y" : 340,
							"width" : 180,
							"height" : 30,
							"text" : "Szlatuki",
							"default_image" : "d:/ymir work/ui/public/middle_button_01.sub",
							"over_image" : "d:/ymir work/ui/public/middle_button_02.sub",
							"down_image" : "d:/ymir work/ui/public/middle_button_03.sub",
						},
						{
							"name" : "crystal_count_5",
							"type" : "text",
							"x" : 10,
							"y" : 375,
							"text" : "Kryształy: 0 szt.",
							"color" : 0xFFFFFFFF,
						},

						## Info Section
						{
							"name" : "InfoTitle",
							"type" : "text",
							"x" : 10,
							"y" : 420,
							"text" : "Informacje:",
							"color" : 0xFFFFE3AD,
						},
						{
							"name" : "InfoText1",
							"type" : "text",
							"x" : 10,
							"y" : 440,
							"text" : "• Kursy odświeżane co 24h",
							"color" : 0xFFFFFFFF,
						},
						{
							"name" : "InfoText2",
							"type" : "text",
							"x" : 10,
							"y" : 455,
							"text" : "• Najedź na przedmiot",
							"color" : 0xFFFFFFFF,
						},
						{
							"name" : "InfoText3",
							"type" : "text",
							"x" : 10,
							"y" : 470,
							"text" : "  aby zobaczyć kurs",
							"color" : 0xFFFFFFFF,
						},
					),
				},

				## Right Panel - Exchange Area
				{
					"name" : "RightPanel",
					"type" : "board",
					"x" : 225,
					"y" : 35,
					"width" : 435,
					"height" : 550,
					"children" :
					(
						{
							"name" : "ExchangeTitle",
							"type" : "text",
							"x" : 217,
							"y" : 10,
							"text" : "Wymiana Przedmiotów",
							"text_horizontal_align" : "center",
							"color" : 0xFFFFE3AD,
						},

						## Item Slot Area (will be dynamically sized)
						{
							"name" : "ItemSlot",
							"type" : "grid_table",
							"x" : 15,
							"y" : 40,
							"start_index" : 0,
							"x_count" : 5,
							"y_count" : 3,
							"x_step" : 32,
							"y_step" : 32,
							"image" : "d:/ymir work/ui/public/Slot_Base.sub"
						},

						## Refresh Button
						{
							"name" : "RefreshButton",
							"type" : "button",
							"x" : 350,
							"y" : 40,
							"width" : 70,
							"height" : 30,
							"text" : "Odśwież",
							"default_image" : "d:/ymir work/ui/public/small_button_01.sub",
							"over_image" : "d:/ymir work/ui/public/small_button_02.sub",
							"down_image" : "d:/ymir work/ui/public/small_button_03.sub",
						},

						## Selected Category Display
						{
							"name" : "CategoryTitle",
							"type" : "text",
							"x" : 15,
							"y" : 500,
							"text" : "Wybrana kategoria: Ogólna",
							"color" : 0xFFFFE3AD,
						},

						## Rate Update Info
						{
							"name" : "RateInfo",
							"type" : "text",
							"x" : 15,
							"y" : 520,
							"text" : "Kursy aktualizowane są automatycznie co 24h na podstawie rzadkości",
							"color" : 0xFFFFFFFF,
						},
					),
				},
			),
		},
	),
}