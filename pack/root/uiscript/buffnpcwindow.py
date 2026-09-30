import uiScriptLocale
import player

IMG_PATH = "d:/ymir work/ui/buff/"
IMG_PATH2 = "d:/ymir work/ui/mount_window/"
SKILL_IMG_PATH = "d:/ymir work/ui/skill/shaman/"
PUBLIC_IMG_PATH = "d:/ymir work/ui/public/"
GAME_IMG_PATH = "d:/ymir work/ui/game/windows/"

BOARD_WIDTH = 241
BOARD_HEIGHT = 380

GOLD_COLOR	= 0xFFFEE3AE
WHITE_COLOR = 0xFFFFFFFF

BUFF_EQUIPMENT_START_INDEX = player.BUFF_EQUIPMENT_SLOT_START

window = {
	"name" : "BuffNPCWindow",
	"x" : 0,
	"y" : 0,
	"style" : ("movable", "float",),
	"width" : BOARD_WIDTH,
	"height" : BOARD_HEIGHT,
	"children" :
	(
		{
			"name" : "board", 
			"type" : "board_with_titlebar", 
			"x" : 0, "y" : 0, 
			"width" : BOARD_WIDTH, 
			"height" : BOARD_HEIGHT,
			"title" : uiScriptLocale.ASLAN_BUFF_TITLE_MAIN_BOARD,
			"children" :
			(
				{
					"name" : "summon_button1", "type" : "button", "x" : 12, "y" : 8, 
					"default_image" : "d:/ymir work/ui/back_btn0.png", 
					"over_image" : "d:/ymir work/ui/back_btn1.png",
					"down_image" : "d:/ymir work/ui/back_btn1.png",
					"tooltip_text_new" : uiScriptLocale.ASLAN_BUFF_UNSUMMON,
					"tooltip_text_color" : 0xfff1e6c0,
					"tooltip_x" : 0,
					"tooltip_y" : -30,
				},
				{
					"name" : "summon_button2", "type" : "button", "x" : 12, "y" : 8, 
					"default_image" : "d:/ymir work/ui/next_btn0.png", 
					"over_image" : "d:/ymir work/ui/next_btn1.png",
					"down_image" : "d:/ymir work/ui/next_btn2.png",
					"tooltip_text_new" : uiScriptLocale.ASLAN_BUFF_SUMMON,
					"tooltip_text_color" : 0xfff1e6c0,
					"tooltip_x" : 0,
					"tooltip_y" : -30,
				},
				{
					"name" : "Page_01", 
					"type" : "window", 
					"style" : ("attach",), 
					"x" : 0, "y" : 30, 
					"width" : BOARD_WIDTH, 
					"height" : BOARD_HEIGHT-30,
					"children":
					(
						{
							"name" : "tb_board",
							"type" : "thinboard_circle",
							"x" : 10, "y" : 0,
							"width" : 220, "height" : 338,
							"children":
							(
								{ "name" : "b0ard", "type" : "image", "x" : 1, "y" : 1, "image" : IMG_PATH + "board_real.png"},
								
								{ "name" : "face_icon_empty", "type" : "image", "x" : 10, "y" : 12, "image" : IMG_PATH + "icon_face_empty.tga"},
								{ "name" : "face_icon_0", "type" : "image", "x" : 10, "y" : 12, "image" : "icon/face/shaman_m.tga" },
								{ "name" : "face_icon_1", "type" : "image", "x" : 10, "y" : 12, "image" : "icon/face/shaman_w.tga" },
								{ "name" : "face_icon_overlay", "type" : "image", "x" : 6, "y" : 8, "image" : IMG_PATH + "face_slot.sub"},

								{ 
									"name" : "info_board", "type" : "image", "x" : 65, "y" : 10, "image" : IMG_PATH + "name.png",
									"children" :
									(
										{ "name" : "buff_name", "type" : "text", "x" : 0, "y" : 9, "all_align":"center", },
									),
								},

								{
									"name":"header",
									"type":"image",
									"x" : 0,
									"y" : 152,
									"image":IMG_PATH+"header.png",
									"children":
									(
										{
											"name":"header_text",
											"type":"text",
											"x":0,"y":0,
											"text": uiScriptLocale.ASLAN_BUFF_EQUIPMENT,
											"outline":1,
											"all_align":"center",
										},
									),
								},
								{
									"name":"header",
									"type":"image",
									"x" : 0,
									"y" : 315,
									"image":IMG_PATH+"header.png",
									"children":
									(
										{
											"name":"int_points",
											"type":"text",
											"x":0,"y":-1,
											"text":"0",
											"outline":1,
											"all_align":"center",
											"fontname":"Tahoma:14",
										},
									),
								},
								{ 
									"name" : "eq_board", "type" : "image", "x" : 45, "y" : 195, "image" : IMG_PATH + "equipment.png",
									"children" :
									(
										{
											"name" : "equipment_slots", "type" : "slot", "x" : 0, "y" : 0,
											"width" : 115, "height" : 178,
											"slot" :	(
												{"index": BUFF_EQUIPMENT_START_INDEX+0, "x":  1, "y":  36, "width":32, "height":32},
												{"index": BUFF_EQUIPMENT_START_INDEX+1, "x": 50, "y": 0, "width":32, "height":32},
												{"index": BUFF_EQUIPMENT_START_INDEX+2, "x": 51, "y":  36, "width":32, "height":64},
												{"index": BUFF_EQUIPMENT_START_INDEX+3, "x":98, "y":  7, "width":32, "height":32},
											),
										},
									),
								},
								{
									"name":"header",
									"type":"image",
									"x" : 0,
									"y" : 70,
									"image":IMG_PATH+"header.png",
									"children":
									(
										{
											"name":"header_text",
											"type":"text",
											"x":0,"y":0,
											"text":	uiScriptLocale.ASLAN_BUFF_SKILLS,
											"outline":1,
											"all_align":"center",
										},
									),
								},
								{
									"name" : "skill_slots", "type" : "grid_table",

									"x" : 8, "y" : 99,

									"start_index" : 0,
									"x_count" : 3,
									"y_count" : 1,
									"x_step" : 0 + 70,
									"y_step" : 32,

									"image" : IMG_PATH+"slot.png",
								},


								{
									"name" : "skill_slots", 
									"type" : "slot", "x" : 24, "y" : 57, 
									"width" : 170, "height" : 100,
									"image" : "d:/ymir work/ui/public/Slot_Base.sub",
									"slot" :	(
										{"index": 0, "x": 0, "y":  50, "width":32, "height":32},
										{"index": 1, "x": 70, "y":  50, "width":32, "height":32},
										{"index": 2, "x": 140, "y":  50, "width":32, "height":32},
									),
								},
								
								{
									"name" : "sex_button", "type" : "button", "x" : 24, "y" : 50, 
									"default_image" : "d:/ymir work/ui/change_sex0.png", 
									"over_image" : "d:/ymir work/ui/change_sex1.png",
									"down_image" : "d:/ymir work/ui/change_sex2.png",
									"tooltip_text_new" : uiScriptLocale.ASLAN_BUFF_CHANGE_SEX,
									"tooltip_text_color" : 0xfff1e6c0,
									"tooltip_x" : 0,
									"tooltip_y" : -30,
								},
							),
						},
					),
				},
			)
		},
	),
}