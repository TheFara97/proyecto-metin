import uiScriptLocale

HATCHING_WINDOW_WIDTH	= 276
HATCHING_WINDOW_HEIGHT	= 214
FACE_SLOT_FILE = "d:/ymir work/ui/game/windows/box_face.sub"

window = {
	"name" : "IsimIziWindow",
	"style" : ("movable", "float",),
	
	"x" : SCREEN_WIDTH / 2 - HATCHING_WINDOW_WIDTH / 2,
	"y" : SCREEN_HEIGHT / 2 - HATCHING_WINDOW_HEIGHT / 2,

	"width" : HATCHING_WINDOW_WIDTH,
	"height" : HATCHING_WINDOW_HEIGHT,

	"children" :
	(
		{
			"name" : "board",
			"type" : "board",
			"style" : ("attach",),

			"x" : 0,
			"y" : 0,

			"width" : HATCHING_WINDOW_WIDTH,
			"height" : HATCHING_WINDOW_HEIGHT,

			"children" :
			(
				{
					"name" : "IsimIzi_TitleBar",
					"type" : "titlebar",
					"style" : ("attach",),

					"x" : 61,
					"y" : 8,

					"width" : 208,
				
					"children" :
					(
						{

							"name" : "TitleName",
							"type" : "text",

							"x" : 0,
							"y" : 3,
							"horizontal_align" : "center",

							"text" : "Ýsim Deðiþtir",
							"text_horizontal_align":"center"
						},
					),
				},

				{
					"name" : "BlackBoard",
					"type" : "thinboard",
					"x" : 13, "y" : 36,
					"width" : HATCHING_WINDOW_WIDTH - 25 ,
					"height" : HATCHING_WINDOW_HEIGHT - 140,
					"children" :
					(
						{
							"name" : "yazi",
							"type" : "text",

							"x" : 0,
							"y" : 20,
							"horizontal_align" : "center",
							"text" : "Arkadaþ Listen |cfff00900|H|hSilinecek!",
							"text_horizontal_align":"center"
						},
						{
							"name" : "yazi2",
							"type" : "text",

							"x" : 0,
							"y" : 20+13,
							"horizontal_align" : "center",
							"text" : "Engellediðin Kiþiler |cfff00900|H|hSilinecek!",
							"text_horizontal_align":"center"
						},
						{
							"name" : "yazi3",
							"type" : "text",

							"x" : 0,
							"y" : 20+13*2,
							"horizontal_align" : "center",
							"text" : "Bir grup veya lonca üyesi |cfff00900|H|holmamalýsýn!",
							"text_horizontal_align":"center"
						},

					),
				},
				{
					"name" : "IsimSlot",
					"type" : "slot",
						

					"x" : 68 + 4,
					"y" : 34 + 4,
					"width" : 32,
					"height" : 32,

					"slot" : ({"index":0, "x":0, "y":0, "width":32, "height":32,},),
				},

				{ "name" : "Face_Image", "type" : "image", "x" : 11, "y" : 11, "image" : "d:/ymir work/ui/game/windows/face_warrior.sub" },
				{ "name" : "Face_Slot", "type" : "image", "x" : 7, "y" : 7, "image" : FACE_SLOT_FILE, },
				
				{
					"name" : "IsimButton",
					"type" : "button",

					"x" : 0,
					"y" : HATCHING_WINDOW_HEIGHT - 35 ,

					"text" : "Ýsim Deðiþtir",
					"horizontal_align" : "center",
					"default_image" : "zaris/whisper/messenger_user_button_norm.png",
					"over_image" : "zaris/whisper/messenger_user_button_hover.png",
					"down_image" : "zaris/whisper/messenger_user_button_down.png",
				},
				
				{
					"name" : "IsimIziGui", 
					"type" : "expanded_image",
					"style" : ("attach",),
					
					"x" : 0, 
					"y" : HATCHING_WINDOW_HEIGHT - 35 - 62, 
					"horizontal_align" : "center",
					"image" : "zaris/changename/changename_gui.tga",
					
					"children" :
					(
						{
							"name" : "isim_gir",
							"type" : "editline",

							"x" : 11,
							"y" : 28,

							"width" : 129,
							"height" : 16,

							"input_limit" : 12,

						},
					),
				},
						
				{ 
					"name" : "IsimIzigTitleWindow", "type" : "window", "x" : 22, "y" : 36, "width" : 130, "height" : 14, "style" : ("attach",),
					"children" :
					(
						{

							"name" : "TitleName",
							"type" : "text",

							"x" : 27, 
							"y" : HATCHING_WINDOW_HEIGHT - 35 - 90, 
							"horizontal_align" : "center",

							"text" : "|cffFDD017|H|hYeni Ýsim Gir",
							#"text_horizontal_align":"center"
						},
					),
				},
			),				
		},
	),
}
