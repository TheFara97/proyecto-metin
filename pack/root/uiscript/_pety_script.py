import item
import app
import uiScriptLocale

BOARD_WIDTH = 300
BOARD_HEIGHT = 325
IMG_PATH = "d:/ymir work/ui/mount_window/"
PET_PATH = "elvion/game/companion{}"

window = {
	"name" : "Pet_Window",
	"style" : ("movable", "float",),
	"x" : 39, "y" : 0,
	"width": 286+39,"height": 399,
	"children" :
	(
		{
			"name" : "bg",
			"type" : "image",
		
			"x" : 39,
			"y" : 0,
		
			"image" : PET_PATH.format("/bg_pet.png"),
		},
		{
			"name" : "petButton",
			"type" : "radio_button",
			"x" : 7,
			"y" : 35,
			"default_image" : PET_PATH.format("/pet_btn_norm.png"),
			"over_image" : PET_PATH.format("/pet_btn_hover.png"),
			"down_image" : PET_PATH.format("/pet_btn_down.png"),
		},
		{
			"name" : "mountButton",
			"type" : "radio_button",
			"x" : 7,
			"y" : 35+44,
			"default_image" : PET_PATH.format("/mount_btn_norm.png"),
			"over_image" : PET_PATH.format("/mount_btn_hover.png"),
			"down_image" : PET_PATH.format("/mount_btn_down.png"),
		},
		{
			"name" : "TitleBar",
			"type" : "titlebar",
			"style" : ("attach",),
	
			"x" : 21+39,
			"y" : 11,
	
			"width" : 243,
			"color" : "red",
	
			"children" :
			(
				{
					"name" : "TitleName",
					"type" : "text",
					"text" : "Pet",
					"horizontal_align" : "center",
					"text_horizontal_align" : "center",
					"outline": 1,
					"x" : 0,
					"y" : 2,
				},
			),
		},

		{
			"name" : "ExtraBonus",
			"type" : "image",
			"x" : 76, 
			"y" : 13,
			"horizontal_align":"left",
			"image" : "d:/ymir work/ui/pattern/q_mark_01.tga",
		},
		
		{
			"name" : "petNameHeader",
			"type" : "window",
			"x" : 110,
			"y" : 67,
			"width" : 90,
			"height" : 18,
			"children":
			(
				{
					"name" : "petNameText",
					"type" : "text",
					"text": "Name",
					"x": 0,
					"y" : 0,
					"color":0xFFeed8c3,
					"outline": 1,
					"all_align" : "center"
				},
			),
		},
		
		{
			"name" : "petFeedItemHeader",
			"type" : "window",
			"x" : 110,
			"y" : 67+21,
			"width" : 90,
			"height" : 18,
			"children":
			(
				{
					"name" : "petFeedItemText",
					"type" : "text",
					"text": "Feed item",
					"x": 0,
					"y" : 0,
					"color":0xFFeed8c3,
					"outline": 1,
					"all_align" : "center"
				},
			),
		},
		
		{
			"name" : "petLevelHeader",
			"type" : "window",
			"x" : 110,
			"y" : 67+21+21,
			"width" : 90,
			"height" : 18,
			"children":
			(
				{
					"name" : "petLevelText",
					"type" : "text",
					"text": "Level",
					"x": 0,
					"y" : 0,
					"color":0xFFeed8c3,
					"outline": 1,
					"all_align" : "center"
				},
			),
		},
		
		{
			"name" : "petNameValue",
			"type" : "window",
			"x" : 203,
			"y" : 66,
			"width" : 111,
			"height" : 18,
			"children":
			(
				{
					"name" : "pet_name",
					"type" : "text",
					"text": "Petino Bombardino",
					"x": 0,
					"y" : 0,
					"all_align" : "center"
				},
			),
		},
		
		{
			"name" : "petFeedItemValue",
			"type" : "window",
			"x" : 203,
			"y" : 66+21,
			"width" : 111,
			"height" : 18,
			"children":
			(
				{
					"name" : "pet_feed_item",
					"type" : "text",
					"text": "|Eemoji/feed_1|e |cffc99573|H|h(Levels 1-24)|H|h",
					"x": 0,
					"y" : 0,
					"all_align" : "center"
				},
			),
		},
		
		{
			"name" : "petLevelValue",
			"type" : "window",
			"x" : 203,
			"y" : 66+21+21,
			"width" : 111,
			"height" : 18,
			"children":
			(
				{
					"name" : "pet_level",
					"type" : "text",
					"text": "99",
					"x": 0,
					"y" : 0,
					"all_align" : "center"
				},
			),
		},

		
		{
			"name" : "petExpWindow",
			"type" : "window",
			"x" : 48,
			"y" : 150,
			"width" : 265,
			"height" : 38,
			"children":
			(
				{ "name" : "exp_hover_info", "type" : "window", "x" : 0, "y" : 0, "width" : 265, "height" : 38},
				{ "name" : "exp_gauge_01", "type" : "expanded_image", "x" : 75 + 0, "y" : 7, "image" : PET_PATH.format("/exp_full.png") },
				{ "name" : "exp_gauge_02", "type" : "expanded_image", "x" : 75 + 31, "y" : 7, "image" : PET_PATH.format("/exp_full.png") },
				{ "name" : "exp_gauge_03", "type" : "expanded_image", "x" : 75 + 62, "y" : 7, "image" : PET_PATH.format("/exp_full.png") },
				{ "name" : "exp_gauge_04", "type" : "expanded_image", "x" : 75 + 93, "y" : 7, "image" : PET_PATH.format("/exp_full.png") },
			),
		},
		{
			"name" : "islot",
			"type" : "slot",

			"x" : 62,
			"y" : 80,

			"width" : 32,
			"height" : 32,

			"slot" : (
						{"index":item.SLOT_ITEM_NEW_PET, "x":0, "y":0, "width":32, "height":32},
					),
		},
		{
			"name" : "generalInfoHeader",
			"type" : "window",
			"x" : 48, "y" : 34,
			"width" : 267, "height" : 31,
			"children":
			(
				{ 
					"name":"header_text",
					"type":"text",
					"x":0,
					"y":0,
					"text":uiScriptLocale.PET_GENERAL_INFORMATIONS,
					"outline" : 1,
					"fontname":"Tahoma:12",
					"color":0xFFeed8c3,
					"all_align" : "center",
				},
			),
		},
		{
			"name" : "petSkillsHeader",
			"type" : "window",
			"x" : 48, "y" : 190,
			"width" : 267, "height" : 31,
			"children":
			(
				{ 
					"name":"header_text",
					"type":"text",
					"x":0,
					"y":0,
					"text":uiScriptLocale.PET_SKILS,
					"outline" : 1,
					"fontname":"Tahoma:12",
					"color":0xFFeed8c3,
					"all_align" : "center",
				},
			),
		},
		{
			"name" : "petEquipmentHeader",
			"type" : "window",
			"x" : 48, "y" : 311,
			"width" : 267, "height" : 31,
			"children":
			(
				{ 
					"name":"header_text",
					"type":"text",
					"x":0,
					"y":0,
					"text":uiScriptLocale.PET_EQUIPMENT,
					"outline" : 1,
					"fontname":"Tahoma:12",
					"color":0xFFeed8c3,
					"all_align" : "center",
				},
			),
		},
		{
			"name":"petSkillPoints",
			"type":"image",
			"x" : 280,
			"y" : 201,
			"image": PET_PATH.format("/skillpoints.png"),
			"children":
			(
				{
					"name":"skill_points",
					"type":"text",
					"x":0,"y":-1,
					"all_align":"center",
					"outline":"1",
					"text":"0",
				},
			),
		},
		{
			"name":"petSkillSlots",
			"type":"image",
			"x":57,"y":226,
			"image": PET_PATH.format("/skill_pet.png"),
			"children":
			(
				{
					"name" : "skills", "type" : "grid_table",
		
					"x" : 4, "y" : 4,
		
					"start_index" : 0,
					"x_count" : 6,
					"y_count" : 2,
					"x_step" : 0+42,
					"y_step" : 42,
		
					#"image" : "d:/ymir work/ui/public/Slot_Base.sub",
				},		
			),
		},
		{
			"name":"petEquipmentSlots",
			"type":"image",
			"x":73,"y":347,
			"image": PET_PATH.format("/item_pet.png"),
			"children":
			(
				{
					"name" : "eqs",
					"type" : "slot",
		
					"x" : 3,
					"y" : 3,
		
					"width" : 250,
					"height" : 50,
		
					"slot" : (
								{"index":item.EQUIPMENT_NEW_PET_EQ0, "x":28, "y":2, "width":32, "height":32},
								{"index":item.EQUIPMENT_NEW_PET_EQ1, "x":68, "y":2, "width":32, "height":32},
								{"index":item.EQUIPMENT_NEW_PET_EQ2, "x":108, "y":2, "width":32, "height":32},
								{"index":item.EQUIPMENT_NEW_PET_EQ3, "x":148, "y":2, "width":32, "height":32},
								{"index":item.EQUIPMENT_NEW_PET_EQ4, "x":188, "y":2, "width":32, "height":32},
					),
				},
			),
		},
	),
}