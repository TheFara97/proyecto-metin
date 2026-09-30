import uiScriptLocale

ROOT_PATH = "d:/ymir work/ui/game/cube/"
PUBLIC_PATH = "d:/ymir work/ui/public/"
PATTERN_PATH = "d:/ymir work/ui/pattern/"

LIST_WINDOW_WIDTH = 340
LIST_WINDOW_HEIGHT = 310
LIST_WINDOW_PATTERN_X_COUNT = (LIST_WINDOW_WIDTH - 32) / 16
LIST_WINDOW_PATTERN_Y_COUNT = (LIST_WINDOW_HEIGHT - 32) / 16

LIST_WINDOW_HEIGHT2 = 86
LIST_WINDOW_PATTERN_Y_COUNT2 = 100 / 29

window = {
	"name" : "CubeRenewalWindow",
	"style" : ("movable", "float", ),
	
	"x" : 0,
	"y" : 0,
	
	"width" : 740,
	"height" : 353,
	
	"children" :
	[
		{
			"name" : "board",
			"type" : "board_with_titlebar",
			"style" : ("ltr", "attach", ),
			
			"x" : 0,
			"y" : 0,
			
			"width" : 740,
			"height" : 353,
			"title" : uiScriptLocale.CUBE_RENEWAL_TITLE,
			
			"children" :
			(
				{
					"name" : "item_list_board",
					"type" : "thinboard_circle",
					"style": ("ltr",),

					"x" : 10,
					"y" : 32,
							
					"width"	: LIST_WINDOW_WIDTH-50,
					"height" : LIST_WINDOW_HEIGHT,
					
					"children" :
					(
						{
							"name" : "cube_bg1",
							"type" : "image",
							"x" : 1,
							"y" : 1,
							"image" : "d:/ymir work/ui/cube_bg1.png",
						},
						{
							"name" : "item_list_window",
							"type" : "window",
							"style": ("ltr",),

							"x" : 3,
							"y" : 3,
									
							"width"	: LIST_WINDOW_WIDTH - 6,
							"height" : LIST_WINDOW_HEIGHT - 10,
						},
					)
				},
				{
					"name" : "cube_list_scroll_bar",
					"type" : "scrollbar",

					"x" : 270,
					"y"	: 33,
					"size" : 308,
				},
				{
					"name" : "cube_board",
					"type" : "thinboard_circle",

					"x" : 295,
					"y" : 32,
							
					"width"	: LIST_WINDOW_WIDTH+94,
					"height" : LIST_WINDOW_HEIGHT,
					
					"children" :
					(
						{
							"name" : "cube_bg",
							"type" : "image",
							"x" : 1,
							"y" : 1,
							"image" : "d:/ymir work/ui/cube_bg.png",
						},
						{
							"name" : "item_slot",
							"type" : "image",
							"style" : ("ltr", "attach"),
							"x" : 10,
							"y" : 39,
							"image" : "d:/ymir work/ui/cube.png",
							
							"children" :
							(
								{
									"name" : "result_qty_window",
									"type" : "window",
									
									"x" : 24,
									"y" : 115,
									
									"width"	: 36,
									"height" : 16,
									
									"children" :
									(
										{ 
											"name" : "result_qty",
											"type" : "editline",
											"x" : 5, "y" : 0,
											"width"	: 30,
											"height" : 16,
											
											"input_limit" : 4,
											"only_number" : 1,
											
											"text" : "",					
										},
									)
								},
								
								{
									"name" : "qty_sub_button",
									"type" : "button",

									"x" : 7,
									"y" : 117,

									"default_image" : ROOT_PATH + "cube_qty_sub_default.sub",
									"over_image" : ROOT_PATH + "cube_qty_sub_over.sub",
									"down_image" : ROOT_PATH + "cube_qty_sub_down.sub",
								},
								
								{
									"name" : "qty_add_button",
									"type" : "button",

									"x" : 62,
									"y" : 117,

									"default_image" : ROOT_PATH + "cube_qty_add_default.sub",
									"over_image" : ROOT_PATH + "cube_qty_add_over.sub",
									"down_image" : ROOT_PATH + "cube_qty_add_down.sub",
								},
							
								{
									"name" : "material_qty_window_1",
									"type" : "window",

									"x" : 85,
									"y" : 115,
									
									"width"	: 45,
									"height" : 16,

									"children" :
									(
										{ 
											"name" : "material_qty_text_1",
											"type" : "text",
											"x" : 0, "y" : 0,
											
											"horizontal_align" : "center",
											"text_horizontal_align" : "center",
											"vertical_align" : "center",
											"text_vertical_align" : "center",
											
											"text" : "",									
										},
									)
								},
								
								{
									"name" : "material_qty_window_2",
									"type" : "window",

									"x" : 133,
									"y" : 115,
									
									"width"	: 45,
									"height" : 16,

									"children" :
									(
										{ 
											"name" : "material_qty_text_2",
											"type" : "text",
											"x" : 0, "y" : 0,
											
											"horizontal_align" : "center",
											"text_horizontal_align" : "center",
											"vertical_align" : "center",
											"text_vertical_align" : "center",
											
											"text" : "",									
										},
									)
								},
								
								{
									"name" : "material_qty_window_3",
									"type" : "window",

									"x" : 180,
									"y" : 115,
									
									"width"	: 45,
									"height" : 16,

									"children" :
									(
										{ 
											"name" : "material_qty_text_3",
											"type" : "text",
											"x" : 0, "y" : 0,
											
											"horizontal_align" : "center",
											"text_horizontal_align" : "center",
											"vertical_align" : "center",
											"text_vertical_align" : "center",
											
											"text" : "",									
										},
									)
								},
								
								{
									"name" : "material_qty_window_4",
									"type" : "window",

									"x" : 224,
									"y" : 115,
									
									"width"	: 45,
									"height" : 16,

									"children" :
									(
										{ 
											"name" : "material_qty_text_4",
											"type" : "text",
											"x" : 0, "y" : 0,
											
											"horizontal_align" : "center",
											"text_horizontal_align" : "center",
											"vertical_align" : "center",
											"text_vertical_align" : "center",
											
											"text" : "",									
										},
									)
								},
								
								{
									"name" : "material_qty_window_5",
									"type" : "window",

									"x" : 270,
									"y" : 115,
									
									"width"	: 45,
									"height" : 16,

									"children" :
									(
										{ 
											"name" : "material_qty_text_5",
											"type" : "text",
											"x" : 0, "y" : 0,
											
											"horizontal_align" : "center",
											"text_horizontal_align" : "center",
											"vertical_align" : "center",
											"text_vertical_align" : "center",
											
											"text" : "",									
										},
									)
								},
								{
									"name" : "material_qty_window_6",
									"type" : "window",

									"x" : 316,
									"y" : 115,
									
									"width"	: 45,
									"height" : 16,

									"children" :
									(
										{ 
											"name" : "material_qty_text_6",
											"type" : "text",
											"x" : 0, "y" : 0,
											
											"horizontal_align" : "center",
											"text_horizontal_align" : "center",
											"vertical_align" : "center",
											"text_vertical_align" : "center",
											
											"text" : "",									
										},
									)
								},
								{
									"name" : "material_qty_window_7",
									"type" : "window",

									"x" : 362,
									"y" : 115,
									
									"width"	: 45,
									"height" : 16,

									"children" :
									(
										{ 
											"name" : "material_qty_text_7",
											"type" : "text",
											"x" : 0, "y" : 0,
											
											"horizontal_align" : "center",
											"text_horizontal_align" : "center",
											"vertical_align" : "center",
											"text_vertical_align" : "center",
											
											"text" : "",									
										},
									)
								},
							),
						},
						{
							"name" : "header",
							"type" : "image",
							"x" : 10, "y" : 20,
							"image":"d:/ymir work/ui/cube_header.png",
							"children":
							(
								{
									"name" : "header_text",
									"type" : "text",
									"x" : 0, "y" : 0,
									"all_align": "center",
									"text" : "Informacje o Wytwarzaniu",
									"fontname" : "Tahoma:14",
									"outline":1,
								},
							),
						},
						{
							"name" : "yang_icon",
							"type" : "image",
							"style" : ("ltr", ),
							"x" : 22,
							"y" : 250,
							
							
							"image":"icon/emoji/money_icon_small.png",
						},
						{
							"name" : "punkty_icon",
							"type" : "image",
							"style" : ("ltr", ),
							"x" : 27,
							"y" : 200,
							
							"image":"icon/emoji/achievement_point.tga",
						},
						{
							"name" : "cheque_icon",
							"type" : "image",
							"style" : ("ltr", ),
							"x" : 27,
							"y" : 226,
							
							
							"image":"icon/emoji/cheque_icon.png",
						},
						{
							"name" : "yang_textbg",
							"type" : "image",
							"style" : ("ltr", ),
							"x" : 54,
							"y" : 250,
							
							"image" : "d:/ymir work/ui/public/parameter_slot_05.sub",
							
							"children" :
							(
								{
									"name" : "yang_text",
									"type" : "text",

									"x" : 7,
									"y" : 1,

									"horizontal_align" : "right",
									"text_horizontal_align" : "right",

									"text" : "",
								},
							),
						},
						{
							"name" : "cheque_textbg",
							"type" : "image",
							"style" : ("ltr", ),
							"x" : 54,
							"y" : 225,
							
							"image" : "d:/ymir work/ui/public/parameter_slot_05.sub",
							
							"children" :
							(
								{
									"name" : "cheque_text",
									"type" : "text",

									"x" : 7,
									"y" : 1,

									"horizontal_align" : "right",
									"text_horizontal_align" : "right",

									"text" : "",
								},
							),
						},
						{
							"name" : "punkty_textbg",
							"type" : "image",
							"style" : ("ltr", ),
							"x" : 54,
							"y" : 200,
							
							"image" : "d:/ymir work/ui/public/parameter_slot_05.sub",
							
							"children" :
							(
								{
									"name" : "punkty_text",
									"type" : "text",

									"x" : 8,
									"y" : 1,

									"horizontal_align" : "right",
									"text_horizontal_align" : "right",

									"text" : "",
								},
							),
						},
						
						{
							"name" : "button_ok",
							"type" : "button",

							"x" : 340,
							"y" : 210,

							"default_image" : "d:/ymir work/ui/button_example_040.png",
							"over_image" : "d:/ymir work/ui/button_example_041.png",
							"down_image" : "d:/ymir work/ui/button_example_042.png",
						},
						
						{
							"name" : "button_cancel",
							"type" : "button",

							"x" : 340,
							"y" : 240,
							"default_image" : "d:/ymir work/ui/button_example_050.png",
							"over_image" : "d:/ymir work/ui/button_example_051.png",
							"down_image" : "d:/ymir work/ui/button_example_052.png",
						},
					),
				},
			),
		}
	],
}